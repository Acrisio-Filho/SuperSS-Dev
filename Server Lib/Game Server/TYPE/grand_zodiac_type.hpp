// Arquivo grand_zodiac_type.hpp
// Criado em 23/06/2020 as 16:24 por Acrisio
// Defini��o dos tipos usados no grand zodiac

#pragma once
#ifndef _STDA_GRAND_ZODIAC_TYPE_HPP
#define _STDA_GRAND_ZODIAC_TYPE_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "game_type.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include <vector>

namespace stdA {

	constexpr uint32_t CHIP_IN_PRACTICE_TICKET_TYPEID = 0x1A00017Eu;

	enum eGRAND_ZODIAC_TYPE_SHOT : unsigned char {
		GZTS_HIO_SCORE = 1u,
		GZTS_FIRST_SHOT,
		GZTS_SPECIAL_SHOT,
		GZTS_WITHOUT_COMMANDS,
		GZTS_MISS_PANGYA
	};

	struct grand_zodiac_dados {
		public:
			grand_zodiac_dados(uint32_t _ul = 0u) 
				: position(0u), pontos(0u), hole_in_one(0u), jackpot(0ull),
				total_score(0l), trofeu(0u), m_score_shot() {
			};
			virtual ~grand_zodiac_dados() {
				clear();
			};
			void clear() {
				
				position = 0u;
				pontos = 0u;
				hole_in_one = 0u;
				jackpot = 0u;
				trofeu = 0u;
				total_score = 0l;

				if (!m_score_shot.empty()) {
					m_score_shot.clear();
					m_score_shot.shrink_to_fit();
				}
			};

		public:
			uint32_t position;
			uint32_t pontos;
			uint32_t hole_in_one;
			uint64_t jackpot;
			uint32_t trofeu;
			int32_t total_score;
			std::vector< eGRAND_ZODIAC_TYPE_SHOT > m_score_shot;
	};

	struct SyncShotGrandZodiac {
		public:
			enum eSYNC_SHOT_GRAND_ZODIAC_STATE : unsigned char {
				SSGZS_FIRST_SHOT_INIT,
				SSGZS_FIRST_SHOT_SYNC
			};

		public:
			SyncShotGrandZodiac() : first_shot_init(0u), first_shot_sync(0u) {

#if defined(_WIN32)
				InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
				INIT_PTHREAD_MUTEXATTR_RECURSIVE;
				INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
				DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
			};
			virtual ~SyncShotGrandZodiac() {

				clearAllState();

#if defined(_WIN32)
				DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_destroy(&m_cs);
#endif
			};

			void setState(eSYNC_SHOT_GRAND_ZODIAC_STATE _state) { // Com Thread Safe

				try {

#if defined(_WIN32)
					EnterCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_lock(&m_cs);
#endif

					set_state(_state);

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif

				}catch (exception& e) {

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif

					_smp::message_pool::getInstance().push(new message("[SyncShotGrandZodiac:setState][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			};

			bool checkAllState() { // Com Thread Safe

				bool ret = false;

				try {

#if defined(_WIN32)
					EnterCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_lock(&m_cs);
#endif

					ret = check_all_state();

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif

				}catch (exception& e) {

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif

					ret = false;

					_smp::message_pool::getInstance().push(new message("[SyncShotGrandZodiac::checkAllState][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

				return ret;
			}

			void clearAllState() { // Com Thread Safe

				try {

#if defined(_WIN32)
					EnterCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_lock(&m_cs);
#endif

					clear_all_state();

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif

				}catch (exception& e) {

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif

					_smp::message_pool::getInstance().push(new message("[SyncShotGrandZodiac::clearAllState][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			};

			bool setStateAndCheckAllAndClear(eSYNC_SHOT_GRAND_ZODIAC_STATE _state) {

				bool ret = false;

				try {

#if defined(_WIN32)
					EnterCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_lock(&m_cs);
#endif

					set_state(_state);

					ret = check_all_state();

					if (ret)
						clear_all_state();

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif

				}catch (exception& e) {

					ret = false;

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif

					_smp::message_pool::getInstance().push(new message("[SyncShotGrandZodiac::setStateAndCheckAllAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

				return ret;
			};

		protected:
			void clear_all_state() {	// Sem Thread Safe

				first_shot_init = 0u;
				first_shot_sync = 0u;
			};

			void set_state(eSYNC_SHOT_GRAND_ZODIAC_STATE _state) { // Sem Thread Safe

				if (_state == eSYNC_SHOT_GRAND_ZODIAC_STATE::SSGZS_FIRST_SHOT_INIT)
					first_shot_init = 1u;
				else if (_state == eSYNC_SHOT_GRAND_ZODIAC_STATE::SSGZS_FIRST_SHOT_SYNC)
					first_shot_sync = 1u;
			};

			bool check_all_state() { // Sem Thread Safe
				return first_shot_init && first_shot_sync;
			};

		protected:
			unsigned char first_shot_init : 1;
			unsigned char first_shot_sync : 1;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	// Polimorfirsmo da struct PlayerGameInfo
	struct PlayerGrandZodiacInfo : public PlayerGameInfo {
		public:
			PlayerGrandZodiacInfo(uint32_t _ul = 0u) : PlayerGameInfo(_ul), 
					m_gz(0u), init_first_hole_gz(0u), end_game(0u), m_sync_shot_gz() {
			};
			virtual ~PlayerGrandZodiacInfo() {
				m_gz.clear();
			};
			void clear() {

				// Clear Base
				PlayerGameInfo::clear();

				// Clear grand_zodiac_dados
				m_gz.clear();

				init_first_hole_gz = 0u;
				end_game = 0u;

				m_sync_shot_gz.clearAllState();
			};

		public:
			grand_zodiac_dados m_gz;
			unsigned char init_first_hole_gz : 1;			// inicializou o Primeiro hole do Grand Zodiac
			unsigned char end_game : 1;						// terminou o jogo, enviou o packet12C
			
			SyncShotGrandZodiac m_sync_shot_gz;				// Sincroniza os dois pacotes de inicializa��o  de tacada do player, j� que eu trato mais de um pacote do mesmo player ao mesmo tempo, a� pode da conflito
	};

	// Usado no Grand Zodiac Event class
	struct range_time {
		public:
			enum eTYPE_MAKE_ROOM : unsigned char {
				TMR_MAKE_ALL,
				TMR_MAKE_INTERMEDIARE,
				TMR_MAKE_ADVANCED,
			};

		public:
			range_time(uint32_t _ul = 0u)
				: m_start{ 0u }, m_end{ 0u }, m_type(eTYPE_MAKE_ROOM::TMR_MAKE_ALL), m_sended_message(false) {
			};
			range_time(unsigned short _hour_start, unsigned short _min_start, unsigned short _sec_start,
					   unsigned short _hour_end, unsigned short _min_end, unsigned short _sec_end, eTYPE_MAKE_ROOM _type)
				: m_start{ 0u, 0u, 0u, 0u, _hour_start, _min_start, _sec_start, 0u },
				  m_end{ 0u, 0u, 0u, 0u, _hour_end, _min_end, _sec_end, 0u }, m_type(_type), m_sended_message(false) {
			};
			range_time(SYSTEMTIME _start, SYSTEMTIME _end, eTYPE_MAKE_ROOM _type) 
				: m_start(_start), m_end(_end), m_type(_type), m_sended_message(false) {
			};
			virtual ~range_time() {
				clear();
			};
			void clear() {
				
				memset(&m_start, 0, sizeof(m_start));
				memset(&m_end, 0, sizeof(m_end));

				m_sended_message = false;
			};

			bool isBetweenTime(SYSTEMTIME& _st) {

				// Garante que os dados da data est�o zerados, gera eles por preven��o
				_st.wDay = 0u;
				_st.wDayOfWeek = 0u;
				_st.wMonth = 0u;
				_st.wYear = 0u;

				return intoStartTime(_st) && intoEndTime(_st);
			}

			bool isBetweenTime(unsigned short _hour, unsigned short _min, unsigned short _sec, unsigned short _milli = 0u) {
				
				SYSTEMTIME st{ 0u, 0u, 0u, 0u, _hour, _min, _sec, _milli };

				return isBetweenTime(st);
			};

			uint32_t getDiffInterval() {
				return timeToMilliseconds(m_end) - timeToMilliseconds(m_start);
			};

		protected:
			inline bool intoStartTime(SYSTEMTIME& _st) {
				return timeToMilliseconds(m_start) <= timeToMilliseconds(_st);
			};

			inline bool intoEndTime(SYSTEMTIME& _st) {
				return timeToMilliseconds(_st) < timeToMilliseconds(m_end);
			}

			inline uint32_t timeToMilliseconds(SYSTEMTIME& _st) {
				return (_st.wHour * 60 * 60 * 1000) + (_st.wMinute * 60 * 1000) + (_st.wSecond * 1000) + _st.wMilliseconds;
			};

		public:
			SYSTEMTIME m_start;
			SYSTEMTIME m_end;
			eTYPE_MAKE_ROOM m_type;

			bool m_sended_message;			// Flag que guarda se o intervalo j� enviou a mensagem
	};
}

#endif // !_STDA_GRAND_ZODIAC_TYPE_HPP
