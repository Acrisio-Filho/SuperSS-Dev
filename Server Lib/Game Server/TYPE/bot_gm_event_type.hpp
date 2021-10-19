// Arquivo bot_gm_event_type.hpp
// Criado em 03/11/2020 as 20:18 por Acrisio
// Defini��o dos tipos usados no Bot GM Event

#pragma once
#ifndef _STDA_BOT_GM_EVENT_TYPE_HPP
#define _STDA_BOT_GM_EVENT_TYPE_HPP

#include <string>

#include <memory.h>

namespace stdA {

	struct stRangeTime {
		public:
			stRangeTime(uint32_t _ul = 0u)
				: m_start{ 0u }, m_end{ 0u }, m_channel_id((unsigned char)~0u), m_sended_message(false) {
			};
			stRangeTime(unsigned short _hour_start, unsigned short _min_start, unsigned short _sec_start,
				unsigned short _hour_end, unsigned short _min_end, unsigned short _sec_end, unsigned char _channel_id)
				: m_start{ 0u, 0u, 0u, 0u, _hour_start, _min_start, _sec_start, 0u },
				m_end{ 0u, 0u, 0u, 0u, _hour_end, _min_end, _sec_end, 0u }, m_channel_id(_channel_id), m_sended_message(false) {
			};
			stRangeTime(SYSTEMTIME _start, SYSTEMTIME _end, unsigned char _channel_id)
				: m_start(_start), m_end(_end), m_channel_id(_channel_id), m_sended_message(false) {
			};
			virtual ~stRangeTime() {
				clear();
			};
			void clear() {

				memset(&m_start, 0, sizeof(m_start));
				memset(&m_end, 0, sizeof(m_end));

				m_channel_id = (unsigned char)~0u;

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
			unsigned char m_channel_id;

			bool m_sended_message;			// Flag que guarda se o intervalo j� enviou a mensagem
	};

	// Reward
	struct stReward {
	public:
		stReward(uint32_t _ul = 0u) : _typeid(0u), qntd(0u), qntd_time(0u), rate(100u) {};
		stReward(uint32_t __typeid, uint32_t _qntd, uint32_t _qntd_time, uint32_t _rate = 100u)
			: _typeid(__typeid), qntd(_qntd), qntd_time(_qntd_time), rate(_rate) {};
		
		void clear() { memset(this, 0, sizeof(stReward)); };
		
		std::string toString() {
			return "TYPEID=" + std::to_string(_typeid)
				+ ", QNTD=" + std::to_string(qntd)
				+ ", QNTD_TIME=" + std::to_string(qntd_time)
				+ ", RATE=" + std::to_string(rate);
		};

	public:
		uint32_t _typeid;
		uint32_t qntd;
		uint32_t qntd_time;
		uint32_t rate;
	};
}

#endif // !_STDA_BOT_GM_EVENT_TYPE_HPP
