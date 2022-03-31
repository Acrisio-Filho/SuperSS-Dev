// Arquivo login_reward_type.hpp
// Criado em 27/10/2020 as 14:48 por Acrisio
// Defini��o dos tipos usados no Login Reward System

#pragma once
#ifndef _STDA_LOGIN_REWARD_TYPE_HPP
#define _STDA_LOGIN_REWARD_TYPE_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include <vector>
#include <map>
#include <string>

#include <cstdint>
#include <cstring>

#include "../../Projeto IOCP/UTIL/util_time.h"

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	struct stPlayerState {
		public:
			stPlayerState(uint32_t _ul = 0u) {
				UNREFERENCED_PARAMETER(_ul);

				clear();
			};
			stPlayerState(uint64_t _id, uint32_t _uid, uint32_t _count_days, uint32_t _count_seq, SYSTEMTIME _upt_date, bool _is_clear = false)
				: id(_id), uid(_uid), count_days(_count_days), update_date(_upt_date), count_seq(_count_seq), is_clear(_is_clear) {};

			void clear() { memset(this, 0, sizeof(stPlayerState)); };

			std::string toString() {
				return "ID=" + std::to_string(id) + ", UID=" + std::to_string(uid) + ", COUNT_DAYS=" + std::to_string(count_days)
					+ ", COUNT_SEQ=" + std::to_string(count_seq) + ", IS_CLEAR=" + std::string(is_clear ? "TRUE" : "FALSE") + ", UPDATE_DATE=" + _formatDate(update_date);
			};

		public:
			uint64_t id;		// index
			uint32_t uid;
			uint32_t count_days;		// Quantos dias j� logou no evento
			uint32_t count_seq;		// Quantas vezes j� repediu esse evento
			SYSTEMTIME update_date;

			bool is_clear;
	};

	struct stLoginReward {
		public:
			struct stItemReward {
				public:
					stItemReward(uint32_t _ul = 0u) {
						UNREFERENCED_PARAMETER(_ul);

						clear();
					};
					stItemReward(uint32_t __typeid, uint32_t _qntd, uint32_t _qntd_time)
						: _typeid(__typeid), qntd(_qntd), qntd_time(_qntd_time) {};
					void clear() { memset(this, 0, sizeof(stItemReward)); };

					std::string toString() {
						return "TYPEID=" + std::to_string(_typeid) + ", QNTD=" + std::to_string(qntd) + ", QNTD_TIME=" + std::to_string(qntd_time);
					};

				public:
					uint32_t _typeid;
					uint32_t qntd;
					uint32_t qntd_time;
			};

		public:
			enum class eTYPE : unsigned char {
				N_TIME,		// Executa N Times, 1, 2, 3, 4, 5, 6, 7 e etc
				FOREVER,	// Enquanto ele estiver ativo ele executa sempre
			};

		public:
			stLoginReward(uint32_t _ul = 0u) 
				: id(0u), type(eTYPE::N_TIME), name{ 0u }, is_end(false), end_date{ 0u },
					days_to_gift(1u), n_times_gift(1u), item_reward{ 0u } {
				UNREFERENCED_PARAMETER(_ul);
			};
			stLoginReward(uint64_t _id, eTYPE _type, const char* _name, uint32_t _days_to_gift, uint32_t _n_times_gift, stItemReward _item, SYSTEMTIME _end_date, bool _is_end = false)
				: id(_id), type(_type), end_date(_end_date), days_to_gift(_days_to_gift), n_times_gift(_n_times_gift), item_reward(_item), is_end(_is_end) {

				setName(_name);
			};
			virtual ~stLoginReward() {
				clear();
			};
			void clear() {

				id = 0u;
				is_end = false;
				type = eTYPE::N_TIME;

				memset(name, 0, sizeof name);

				days_to_gift = 1u;
				n_times_gift = 1u;

				end_date = { 0u };

				item_reward.clear();
			};

			void setName(const char* _name) {
				
				if (_name != nullptr) {

					auto len = strlen(_name);

#if defined(_WIN32)
					memcpy_s(name, sizeof(name), _name, (len > sizeof(name) ? sizeof(name) : len));
#elif defined(__linux__)
					memcpy(name, _name, (len > sizeof(name) ? sizeof(name) : len));
#endif

					name[(len >= sizeof(name)) ? sizeof(name) -1 : len] = '\0';
				}
			};

			std::string getName() {

				if (name == nullptr)
					return "";

				return name;
			};

			std::string toString() {
				return "ID=" + std::to_string(id) + ", TYPE=" + std::to_string((unsigned short)type)
					+ ", NAME=" + getName() + ", DAYS_TO_GIFT=" + std::to_string(days_to_gift) 
					+ ", N_TIMES_GIFT=" + std::to_string(n_times_gift) + ", IS_END=" + std::string(is_end ? "TRUE" : "FALSE") 
					+ ", ITEM{" + item_reward.toString() + "}, END_DATE=" + _formatDate(end_date);
			};

		public:
			uint64_t id;
			eTYPE type;

			bool is_end;

			uint32_t days_to_gift;
			uint32_t n_times_gift;

			SYSTEMTIME end_date;

			stItemReward item_reward;

		private:
			char name[100];
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_LOGIN_REWARD_TYPE_HPP
