// Arquivo pangya_auth_st.h
// Criado em 02/12/2018 as 13:12 por Acrisio
// Defini��o da do tipos usando na classe auth_server

#pragma once
#ifndef _STDA_PANGYA_AUTH_ST_H
#define _STDA_PANGYA_AUTH_ST_H

#include <memory>

#include "../../Projeto IOCP/TYPE/pangya_st.h"

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	// Player info
	struct player_info {
		player_info() {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(player_info));
		};
		unsigned int  uid;
		unsigned int  tipo;
		unsigned short level;
		char id[22];
		char nickname[22];
		char pass[40];
	};

	struct CommandInfo {
		CommandInfo(unsigned int  _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(CommandInfo)); };
		std::string toString() {
			return "IDX=" + std::to_string(idx) + ", ID=" + std::to_string(id) + ", ARG1=" 
					+ std::to_string(arg[0]) + ", ARG2=" + std::to_string(arg[1]) + ", ARG3=" 
					+ std::to_string(arg[2]) + ", ARG4=" + std::to_string(arg[3]) + ", ARG5=" 
					+ std::to_string(arg[4]) + ", TARGET=" + std::to_string(target) + ", FLAG=" 
					+ std::to_string(flag) + ", VALID=" + std::to_string((unsigned short)valid) + ", RESERVEDATE=" + std::to_string(reserveDate);
		};
		unsigned int  idx;
		unsigned int  id;
		unsigned int  arg[5];
		unsigned int  target;
		unsigned short flag;
		unsigned char valid : 1;
		time_t reserveDate;
	};

	enum COMMAND_ID : unsigned int  {
		BROADCAST_NOTICE,
		BROADCAST_TICKER,
		BROADCAST_CUBE_WIN,
		SHUTDOWN,
		NEW_ITEM_NOTICE,
		NEW_RATE,
		ADM_KICK_FROM_WEBSITE,
		RELOAD_SYSTEM,
	};

	struct TickerInfo {
		TickerInfo(unsigned int  _ul = 0u) {
			clear();
		};
		void clear() {

			if (!nick.empty()) {
				nick.clear();
				nick.shrink_to_fit();
			}

			if (!msg.empty()) {
				msg.clear();
				msg.shrink_to_fit();
			}
		};
		bool isValid() {
			return (!msg.empty() && !nick.empty());
		};
		std::string nick;
		std::string msg;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_PANGYA_AUTH_ST_H
