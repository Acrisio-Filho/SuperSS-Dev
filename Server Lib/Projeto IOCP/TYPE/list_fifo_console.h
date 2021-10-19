// Arquivo list_fifo_console.h
// Criado em 30/07/2017 por Acrisio
// Definição e Implementação da classe list_fifo_console_asyc

#pragma once
#ifndef _STDA_LIST_FIFO_CONSOLE_H
#define _STDA_LIST_FIFO_CONSOLE_H

#include "list_fifo.h"
#include <iostream>
#include <fstream>
#include "stda_error.h"

#include "../UTIL/reader_ini.hpp"

#ifndef _PATH_SEPARETOR
	#if defined(_WIN32)
		#define _PATH_SEPARETOR "\\"
	#elif defined(__linux__)
		#define _PATH_SEPARETOR "/"
	#endif
#endif

#ifndef _INI_PATH
	#if defined(_WIN32)
		#define _INI_PATH "\\server.ini"
	#elif defined(__linux__)
		#define _INI_PATH "/server.ini"
	#endif
#endif // _INI_PATH

#include <ctime>
#include <filesystem>

#if defined(_WIN32)
#include <direct.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <sys/stat.h>
#include <pthread.h>
#define MODE_DIR_LOG (S_IRWXU | S_IROTH | S_IXOTH)
#endif

#include "../UTIL/util_time.h"

namespace stdA {
    template<class _TA> class list_fifo_console_asyc : public list_fifo_asyc<_TA> {
        public:
			list_fifo_console_asyc();
			~list_fifo_console_asyc();
			//using list_fifo_asyc<_TA>::push;
			void push(_TA *_deque);

			void console_log(DWORD dwMilliseconds = INFINITE);

			bool checkUpdateDayLog();

			void setPrefixo(std::string _prex);
			void reload_log_files();

		private:
			void init_log_files();
			void close_log_files();

		protected:
#if defined(_WIN32)
			CRITICAL_SECTION cs_console;
#elif defined(__linux__)
			pthread_mutex_t cs_console;
#endif

			std::ofstream log_time, log, log_io_data;

			// Time to check day of Log
			time_t day_time;
			
#ifdef _DEBUG
			std::ofstream log_test;
#endif // _DEBUG

			std::string prex;
			std::string dir;
    };

    // Implementação
    template <class _TA> list_fifo_console_asyc<_TA>::list_fifo_console_asyc() : list_fifo_asyc<_TA>() {
        
#if defined(_WIN32)
		InitializeCriticalSection(&cs_console);
#elif defined(__linux__)
		INIT_PTHREAD_MUTEXATTR_RECURSIVE;
		INIT_PTHREAD_MUTEX_RECURSIVE(&cs_console);
		DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

		log_time.exceptions(std::ofstream::failbit | std::ofstream::badbit);
		log.exceptions(std::ofstream::failbit | std::ofstream::badbit);
		log_io_data.exceptions(std::ofstream::failbit | std::ofstream::badbit);

#ifdef _DEBUG
		log_test.exceptions(std::ofstream::failbit | std::ofstream::badbit);
#endif // _DEBUG

		prex = "";
		dir = "Log";

		//init_log_files();	// não inicializa o log ainda não, inicializa depois
    };

    template <class _TA> list_fifo_console_asyc<_TA>::~list_fifo_console_asyc() {

		close_log_files();

		prex.clear();
		prex.shrink_to_fit();

		dir.clear();
		dir.shrink_to_fit();

#if defined(_WIN32)
        DeleteCriticalSection(&cs_console);
#elif defined(__linux__)
		pthread_mutex_destroy(&cs_console);
#endif
    };

	template <class _TA> void list_fifo_console_asyc<_TA>::close_log_files() {

		try {

			// Bloquea
#if defined(_WIN32)
			EnterCriticalSection(&cs_console);
#elif defined(__linux__)
			pthread_mutex_lock(&cs_console);
#endif

			try {
			
				if (log_time.is_open())
					log_time.close();

				if (log.is_open())
					log.close();

				if (log_io_data.is_open())
					log_io_data.close();

	#ifdef _DEBUG
				if (log_test.is_open())
					log_test.close();
	#endif // _DEBUG

			}catch (std::ofstream::failure& e) {

				std::cout << formatDateLocal(0) +  "\t[(" + std::string(typeid(this).name()) + ")::close_log_files][Error][" + std::string(e.what()) + " Code: " 
						+ std::to_string(e.code().value()) + "]." << std::endl;
			}

			// Libera
#if defined(_WIN32)
			LeaveCriticalSection(&cs_console);
#elif defined(__linux__)
			pthread_mutex_unlock(&cs_console);
#endif
		
		}catch (exception& e) {

			// Libera
#if defined(_WIN32)
			LeaveCriticalSection(&cs_console);
#elif defined(__linux__)
			pthread_mutex_unlock(&cs_console);
#endif

			std::cout << formatDateLocal(0) + "\t[(" + std::string(typeid(this).name()) + ")::close_log_files][ErrorSystem] + " + std::string(e.getFullMessageError()) << std::endl;
		}
	};

	template <class _TA> void list_fifo_console_asyc<_TA>::init_log_files() {

		try {

			// Bloquea
#if defined(_WIN32)
			EnterCriticalSection(&cs_console);
#elif defined(__linux__)
			pthread_mutex_lock(&cs_console);
#endif

			try {

				// Dir do arquivo .ini ou padrão se não tiver
				ReaderIni ini(_INI_PATH);

				ini.init();

				std::string tmp_dir = ini.readString("LOG", "DIR");

				if (!tmp_dir.empty()) {

					auto status = std::filesystem::status(tmp_dir);

					if (std::filesystem::exists(status)) {

						dir = tmp_dir;	// O diretório do Arquivo .ini existe, usa esse diretório

#if defined(_WIN32)
					}else if (_mkdir(tmp_dir.c_str()) == 0/*Criou o direitório com sucesso*/)	// Tenta cria o diretório que está no Arquivo .ini, se conseguir usa esse diretório criado
#elif defined(__linux__)
					}else if (mkdir(tmp_dir.c_str(), MODE_DIR_LOG) == 0/*Criou o direitório com sucesso*/)	// Tenta cria o diretório que está no Arquivo .ini, se conseguir usa esse diretório criado
#endif
						dir = tmp_dir;
					else
						throw exception("[(" + std::string(typeid(this).name()) + ")::init_log_files][Error] Nao conseguiu criar o diretorio[" + tmp_dir 
								+ "] do Arquivo .ini.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO_CONSOLE, 5000, 0));
			
				}else
					throw exception("[(" + std::string(typeid(this).name()) + ")::init_log_files][Error] O diretorio do Arquivo .ini esta vazio.", 
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO_CONSOLE, 5001, 0));

			}catch (exception& e) {
			
				std::cout << formatDateLocal(0) + "\t[(" + std::string(typeid(this).name()) + ")::init_log_files][ErrorSystem] " + e.getFullMessageError() << std::endl;

				// Log
				std::cout << formatDateLocal(0) + "\t[(" + std::string(typeid(this).name()) + ")::init_log_files][Log] Nao consguiu pegar o diretorio do Arquivo .ini, usando o diretorio padrao \"Log\"." << std::endl;

				// Não tem o log dir do arquivo ini, usa o padrão
				// Verifica se diretório padrão está criado, se não cria ele
				dir = "Log";

				auto s = std::filesystem::status(dir);

#if defined(_WIN32)
				if (!std::filesystem::exists(s) && _mkdir(dir.c_str()) != 0)
#elif defined(__linux__)
				if (!std::filesystem::exists(s) && mkdir(dir.c_str(), MODE_DIR_LOG) != 0)
#endif
					std::cout << formatDateLocal(0) + "\t[(" + std::string(typeid(this).name()) + ")::init_log_files][Error] Nao conseguiu criar o diretorio[" + dir + "] padrao." << std::endl;
			}

			close_log_files();

			std::string datetime = "";
		
			datetime.resize(255);

			tm ti{ 0 };

			// Atualiza o Day Time do Log para o atual e guarda ele na variável
			std::time(&day_time);

#if defined(_WIN32)
			if (localtime_s(&ti, &day_time) == 0)
				sprintf_s(&datetime[0], datetime.size(), "%.02u%.02u%.04u%.02u%.02u%.02u", ti.tm_mday, ti.tm_mon + 1, ti.tm_year + 1900, ti.tm_hour, ti.tm_min, ti.tm_sec);
#elif defined(__linux__)
			if (localtime_r(&day_time, &ti) != nullptr)
				sprintf(&datetime[0], "%.02u%.02u%.04u%.02u%.02u%.02u", ti.tm_mday, ti.tm_mon + 1, ti.tm_year + 1900, ti.tm_hour, ti.tm_min, ti.tm_sec);
#endif

			datetime.resize(14);

			if (!prex.empty())
				datetime += " " + prex;

			std::string full_path = "";

			try {

				// Log Time
				full_path = dir + _PATH_SEPARETOR + "log_time " + datetime + ".log";
			
				if (!log_time.is_open())
					log_time.open(full_path, std::ofstream::app);

				// Log
				full_path = dir + _PATH_SEPARETOR + "log " + datetime + ".log";

				if (!log.is_open())
					log.open(full_path, std::ofstream::app);

				// Log IO Data
				full_path = dir + _PATH_SEPARETOR + "log_io_data " + datetime + ".log";

				if (!log_io_data.is_open())
					log_io_data.open(full_path, std::ofstream::app);

	#ifdef _DEBUG
				// Log Test
				full_path = dir + _PATH_SEPARETOR + "log_test " + datetime + ".log";

				if (!log_test.is_open())
					log_test.open(full_path, std::ofstream::app);
	#endif // _DEBUG
		
			}catch (std::ofstream::failure& e) {
		
				std::cout << formatDateLocal(0) + "\t[(" + std::string(typeid(this).name()) + ")::init_log_files][Error][" + std::string(e.what()) + " Code: "
						+ std::to_string(e.code().value()) + "] nao conseguiu abrir o arquivo='" + full_path + "'" << std::endl;
			}

			// Libera
#if defined(_WIN32)
			LeaveCriticalSection(&cs_console);
#elif defined(__linux__)
			pthread_mutex_unlock(&cs_console);
#endif
		
		}catch (exception& e) {

			// Libera
#if defined(_WIN32)
			LeaveCriticalSection(&cs_console);
#elif defined(__linux__)
			pthread_mutex_unlock(&cs_console);
#endif

			std::cout << formatDateLocal(0) + "\t[(" + std::string(typeid(this).name()) + ")::init_log_files][ErrorSystem] + " + std::string(e.getFullMessageError()) << std::endl;
			
		}
	};

	template<class _TA> bool list_fifo_console_asyc<_TA>::checkUpdateDayLog() {

		bool ret = false;

		time_t now;
		tm ti_now{ 0 }, ti_day{ 0 };

		std::time(&now);

		// Transforma os segundos dos tempos em struct time
#if defined(_WIN32)
		if (localtime_s(&ti_now, &now) == 0 && localtime_s(&ti_day, &day_time) == 0) {
#elif defined(__linux__)
		if (localtime_r(&now, &ti_now) != nullptr && localtime_r(&day_time, &ti_day) != nullptr) {
#endif

			// Criar novos Logs que trocou o Dia do Log
			if (ti_day.tm_year < ti_now.tm_year || ti_day.tm_mon < ti_now.tm_mon || ti_day.tm_mday < ti_now.tm_mday) {
				
				// Recarrega os Log Files (Ele cria outros arquivos com outra data e hora do dia)
				reload_log_files();
			
				// Criou novos logs, trocou o dia do log
				ret = true;
			}
		}

		return ret;
	};

	template <class _TA> void list_fifo_console_asyc<_TA>::reload_log_files() {

		init_log_files();
	};

	template <class _TA> void list_fifo_console_asyc<_TA>::setPrefixo(std::string _prex) {
		prex = _prex;
	};

	template <class _TA> void list_fifo_console_asyc<_TA>::push(_TA *_deque) {
		//if (_deque == nullptr || _deque->getTipo() == CL_ONLY_CONSOLE)
			list_fifo_asyc<_TA>::push(_deque);
		//else if (_deque != nullptr)
			//delete _deque;
	};

    template <class _TA> void list_fifo_console_asyc<_TA>::console_log(DWORD dwMilliseconds) {
		auto m = list_fifo_asyc<_TA>::get(dwMilliseconds);

		if (m != nullptr) {

			try {

				// Bloquea
#if defined(_WIN32)
				EnterCriticalSection(&cs_console);
#elif defined(__linux__)
				pthread_mutex_lock(&cs_console);
#endif

				if (m->getTipo() == CL_ONLY_FILE_TIME_LOG || m->getTipo() == CL_FILE_TIME_LOG_AND_CONSOLE) {

					if (!log_time.is_open())
						init_log_files();

					if (log_time.good())
						log_time << m->get() << std::endl;
				}

				if (m->getTipo() == CL_ONLY_FILE_LOG_IO_DATA || m->getTipo() == CL_FILE_LOG_IO_DATA_AND_CONSOLE) {

					if (!log_io_data.is_open())
						init_log_files();

					if (log_io_data.good())
						log_io_data << m->get() << std::endl;
				}

				if (m->getTipo() == CL_FILE_LOG_AND_CONSOLE || m->getTipo() == CL_ONLY_FILE_LOG) {

					if (!log.is_open())
						init_log_files();

					if (log.good())
						log << m->get() << std::endl;
				}

#ifdef _DEBUG
				if (m->getTipo() == CL_FILE_LOG_TEST_AND_CONSOLE || m->getTipo() == CL_ONLY_FILE_LOG_TEST) {

					if (!log_test.is_open())
						init_log_files();

					if (log_test.good())
						log_test << m->get() << std::endl;
				}
#endif // _DEBUG

				if (m->getTipo() == CL_ONLY_CONSOLE || m->getTipo() == CL_FILE_LOG_AND_CONSOLE || m->getTipo() == CL_FILE_TIME_LOG_AND_CONSOLE
					|| m->getTipo() == CL_FILE_LOG_IO_DATA_AND_CONSOLE || m->getTipo() == CL_FILE_LOG_TEST_AND_CONSOLE)
					std::cout << m->get() << std::endl;

				// Libera
#if defined(_WIN32)
				LeaveCriticalSection(&cs_console);
#elif defined(__linux__)
				pthread_mutex_unlock(&cs_console);
#endif

				if (m != nullptr) {
					
					delete m;

					// Limpa a variável
					m = nullptr;
				}
			
			}catch (exception& e) {

				// Libera
#if defined(_WIN32)
				LeaveCriticalSection(&cs_console);
#elif defined(__linux__)
				pthread_mutex_unlock(&cs_console);
#endif

				if (m != nullptr) {

					delete m;

					// Limpa a variável
					m = nullptr;
				}

				std::cout << formatDateLocal(0) + "\t[(" + std::string(typeid(this).name()) + ")::console_log][ErrorSystem] " + std::string(e.getFullMessageError()) << std::endl;

			}catch (std::ofstream::failure& e) {

				// Libera
#if defined(_WIN32)
				LeaveCriticalSection(&cs_console);
#elif defined(__linux__)
				pthread_mutex_unlock(&cs_console);
#endif

				if (m != nullptr) {

					delete m;

					// Limpa a variável
					m = nullptr;
				}

				std::cout << formatDateLocal(0) + "\t[(" + std::string(typeid(this).name()) + ")::console_log][Error][" + std::string(e.what()) + " Code: "
						+ std::to_string(e.code().value()) + "] Erro em algum arquivo de log." << std::endl;
			}

		}else
			throw exception("[(" + std::string(typeid(this).name()) + ")::console_log][Error] Message is null.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO_CONSOLE, 1, 0));
    };
}

#endif