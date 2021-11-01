// Arquivo list_fifo.h
// Criado em 03/06/2017 por Acrisio
// Definição e Implementação da classe template list_fifo_asyc

#pragma once
#ifndef _STDA_LIST_FIFO_H
#define _STDA_LIST_FIFO_H

#include <cstdint>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <pthread.h>
#endif

#include <deque>
#include <typeinfo>
#include "../UTIL/exception.h"

#include "stda_error.h"

namespace stdA {
    template <class _TA> class list_fifo_asyc {
        public:
            list_fifo_asyc();
            ~list_fifo_asyc();

            void init();
            void destroy();

            void push(_TA *_deque);
            void push_front(_TA *_deque);
            void push_back(_TA *_deque);

			void remove(_TA *_deque);

            _TA* get(DWORD dwMilliseconds = INFINITE);
            _TA* getFirst(DWORD dwMilliseconds = INFINITE);
            _TA* getLast(DWORD dwMilliseconds = INFINITE);

            _TA* peek(DWORD dwMilliseconds = INFINITE);
            _TA* peekFirst(DWORD dwMilliseconds = INFINITE);
            _TA* peekLast(DWORD dwMilliseconds = INFINITE);

			size_t size();
			bool empty();
			void clear();

        protected:
            std::deque< _TA* > m_deque;

#if defined(_WIN32)
            CRITICAL_SECTION cs;
            CONDITION_VARIABLE cv;
#elif defined(__linux__)
			pthread_mutex_t cs;
			pthread_cond_t cv;
#endif
    };

	// Implementação da classe template list_fifo_asyc
	template<class _TA> list_fifo_asyc<_TA>::list_fifo_asyc() {
		init();
	};

	template<class _TA> list_fifo_asyc<_TA>::~list_fifo_asyc() {
		clear();

		destroy();
	};

	template<class _TA> void list_fifo_asyc<_TA>::init() {

#if defined(_WIN32)
		InitializeConditionVariable(&cv);
		InitializeCriticalSection(&cs);
#elif defined(__linux__)
		INIT_PTHREAD_MUTEXATTR_RECURSIVE;
		INIT_PTHREAD_MUTEX_RECURSIVE(&cs);
		DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
		
		pthread_cond_init(&cv, nullptr);
#endif
	};

	template<class _TA> void list_fifo_asyc<_TA>::destroy() {
#if defined(_WIN32)
		DeleteCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_destroy(&cs);
		pthread_cond_destroy(&cv);
#endif
	};

	template<class _TA> void list_fifo_asyc<_TA>::push(_TA *_deque) {
		push_back(_deque);
	};

	template<class _TA> void list_fifo_asyc<_TA>::push_front(_TA *_deque) {
#if defined(_WIN32)
		EnterCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_lock(&cs);
#endif
		m_deque.push_front(_deque);

#if defined(_WIN32)
		LeaveCriticalSection(&cs);

		WakeConditionVariable(&cv);
#elif defined(__linux__)
		pthread_mutex_unlock(&cs);

		pthread_cond_signal(&cv);
#endif
	};

	template<class _TA> void list_fifo_asyc<_TA>::push_back(_TA *_deque) {
#if defined(_WIN32)
		EnterCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_lock(&cs);
#endif

		m_deque.push_back(_deque);

#if defined(_WIN32)
		LeaveCriticalSection(&cs);

		WakeConditionVariable(&cv);
#elif defined(__linux__)
		pthread_mutex_unlock(&cs);

		pthread_cond_signal(&cv);
#endif
	};

	template<class _TA> void list_fifo_asyc<_TA>::remove(_TA *_deque) {

		if (_deque == nullptr)
			throw exception("Error argument is nullptr, " + std::string(typeid(_deque).name()) + ". (" + std::string(typeid(this).name()) + ")::remove()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 3, 0));
		
		for (auto i = m_deque.begin(); i != m_deque.end(); ++i) {
			if (*i == _deque) {
#if defined(_WIN32)
				EnterCriticalSection(&cs);
#elif defined(__linux__)
				pthread_mutex_lock(&cs);
#endif

				m_deque.erase(i);

#if defined(_WIN32)
				LeaveCriticalSection(&cs);
#elif defined(__linux__)
				pthread_mutex_unlock(&cs);
#endif
				break;
			}
		}
	};

	template<class _TA> _TA* list_fifo_asyc<_TA>::get(DWORD dwMilliseconds) {
		return getFirst(dwMilliseconds);
	};

	template<class _TA> _TA* list_fifo_asyc<_TA>::getFirst(DWORD dwMilliseconds) {
		_TA* deque = nullptr;

#if defined(_WIN32)
		EnterCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_lock(&cs);
#endif

		while (m_deque.empty()) {	
#if defined(_WIN32)
			if (SleepConditionVariableCS(&cv, &cs, dwMilliseconds) == 0) {
				LeaveCriticalSection(&cs);

				if (GetLastError() == ERROR_TIMEOUT)
					throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::getFirst().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 2, 0));
				else
					throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::getFirst()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, GetLastError()));
			}
#elif defined(__linux__)

			int32_t error = 0;

			if ((int32_t)dwMilliseconds == INFINITE) {

				if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
					pthread_mutex_unlock(&cs);

					throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::getFirst()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, error));
				}

			}else {
				timespec wait_time = _milliseconds_to_timespec_clock_realtime(dwMilliseconds);
				
				if ((error = pthread_cond_timedwait(&cv, &cs, &wait_time)) != 0) {
					pthread_mutex_unlock(&cs);

					if (error == ETIMEDOUT)
						throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::getFirst().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 2, 0));
					else
						throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::getFirst()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, error));
				}
			}
#endif
		}

		deque = m_deque.front();
		m_deque.pop_front();

#if defined(_WIN32)
		LeaveCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&cs);
#endif

		return deque;
	};

	template<class _TA> _TA* list_fifo_asyc<_TA>::getLast(DWORD dwMilliseconds) {
		_TA *deque = nullptr;

#if defined(_WIN32)
		EnterCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_lock(&cs);
#endif

		while (m_deque.empty()) {
#if defined(_WIN32)
			if (SleepConditionVariableCS(&cv, &cs, dwMilliseconds) == 0) {
				LeaveCriticalSection(&cs);

				if (GetLastError() == ERROR_TIMEOUT)
					throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::getLast().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 2, 0));
				else
					throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::getLast()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, GetLastError()));
			}
#elif defined(__linux__)

			int32_t error = 0;

			if (dwMilliseconds == INFINITE) {

				if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
					pthread_mutex_unlock(&cs);

					throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::getLast()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, error));
				}

			}else {

				timespec wait_time = _milliseconds_to_timespec_clock_realtime(dwMilliseconds);

				if ((error = pthread_cond_timedwait(&cv, &cs, &wait_time)) != 0) {
					pthread_mutex_unlock(&cs);

					if (error == ETIMEDOUT)
						throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::getLast().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 2, 0));
					else
						throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::getLast()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, error));
				}
			}
#endif
		}

		deque = m_deque.back();
		m_deque.pop_back();

#if defined(_WIN32)
		LeaveCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&cs);
#endif

		return deque;
	};

	template<class _TA> _TA* list_fifo_asyc<_TA>::peek(DWORD dwMilliseconds) {
		return peekFirst(dwMilliseconds);
	};

	template<class _TA> _TA* list_fifo_asyc<_TA>::peekFirst(DWORD dwMilliseconds) {
		_TA *deque = nullptr;

#if defined(_WIN32)
		EnterCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_lock(&cs);
#endif

		while (m_deque.empty()) {
#if defined(_WIN32)
			if (SleepConditionVariableCS(&cv, &cs, dwMilliseconds) == 0) {
				LeaveCriticalSection(&cs);

				if (GetLastError() == ERROR_TIMEOUT)
					throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::peekFirst().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 2, 0));
				else
					throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::peekFirst()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, GetLastError()));
			}
#elif defined(__linux__)

			int32_t error = 0;

			if (dwMilliseconds == INFINITE) {

				if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
					pthread_mutex_unlock(&cs);

					throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::peekFirst()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, error));
				}

			}else {

				timespec wait_time = _milliseconds_to_timespec_clock_realtime(dwMilliseconds);

				if ((error = pthread_cond_timedwait(&cv, &cs, &wait_time)) != 0) {
					pthread_mutex_unlock(&cs);

					if (error == ETIMEDOUT)
						throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::peekFirst().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 2, 0));
					else
						throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::peekFirst()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, error));
				}
			}
#endif
		}

		deque = m_deque.front();

#if defined(_WIN32)
		LeaveCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&cs);
#endif

		return deque;
	};

	template<class _TA> _TA* list_fifo_asyc<_TA>::peekLast(DWORD dwMilliseconds) {
		_TA *deque = nullptr;

#if defined(_WIN32)
		EnterCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_lock(&cs);
#endif

		while (m_deque.empty()) {
#if defined(_WIN32)
			if (SleepConditionVariableCS(&cv, &cs, dwMilliseconds) == 0) {
				LeaveCriticalSection(&cs);

				if (GetLastError() == ERROR_TIMEOUT)
					throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::peekLast().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 2, 0));
				else
					throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::peekLast()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, GetLastError()));
			}
#elif defined(__linux__)

			int32_t error = 0;

			if (dwMilliseconds == INFINITE) {

				if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
					pthread_mutex_unlock(&cs);

					throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::peekLast()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, error));
				}

			}else {

				timespec wait_time = _milliseconds_to_timespec_clock_realtime(dwMilliseconds);

				if ((error = pthread_cond_timedwait(&cv, &cs, &wait_time)) != 0) {
					pthread_mutex_unlock(&cs);

					if (error == ETIMEDOUT)
						throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::peekLast().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 2, 0));
					else
						throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::peekLast()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_FIFO, 1, error));
				}
			}
#endif
		}

		deque = m_deque.back();

#if defined(_WIN32)
		LeaveCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&cs);
#endif

		return deque;
	};

	template<class _TA> size_t list_fifo_asyc<_TA>::size() {
		return m_deque.size();
	};

	template<class _TA> bool list_fifo_asyc<_TA>::empty() {
		return m_deque.empty();
	};

	template<class _TA> void list_fifo_asyc<_TA>::clear() {
#if defined(_WIN32)
		EnterCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_lock(&cs);
#endif

		if (!m_deque.empty()) {
			for (size_t i = 0; i < m_deque.size(); i++)
				if (m_deque[i] != nullptr)
					delete m_deque[i];

			m_deque.clear();
			m_deque.shrink_to_fit();
		}

#if defined(_WIN32)
		LeaveCriticalSection(&cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&cs);
#endif
	};
}

#endif