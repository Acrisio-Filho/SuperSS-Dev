// Arquivo pangya_message_st.hpp
// Criado em 29/07/2018 as 13:24 por Acrisio
// Defini��o dos tipos usados no pangya message

#pragma once
#ifndef _STDA_PANGYA_MESSAGE_ST_HPP
#define _STDA_PANGYA_MESSAGE_ST_HPP

#include <memory>

#include "../../Projeto IOCP/TYPE/pangya_st.h"

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	// PlayerInfo
	struct player_info {
		player_info(unsigned int  _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(player_info)); };
		unsigned int  uid;
		unsigned int  m_cap;
		BlockFlag block_flag;
		unsigned int  guild_uid;
		char guild_name[50];
		unsigned int  server_uid;
		unsigned short level;
		unsigned char sex : 1, : 0;
		char id[22];
		char nickname[22];
	};

	// Canal Player Info
	struct ChannelPlayerInfo {
		void clear() { memset(this, 0, sizeof(ChannelPlayerInfo)); };
		struct Room {
			void clear() { memset(this, 0, sizeof(Room)); };
			short number;
			int type;
		};
		Room room;
		unsigned int  server_uid;
		char id;
		char name[64];
	};

	// Friend Info
	struct FriendInfo {
		FriendInfo(unsigned int  _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(FriendInfo));
			
			lUnknown = -1;
			lUnknown2 = 0;
			lUnknown3 = -1;
			lUnknown4 = 0;
			lUnknown5 = 0;
			lUnknown6 = 0;
			lUnknown7 = 0;
		};
		char nickname[22];
		char apelido[11];
		unsigned int  uid;
		int lUnknown;
		int lUnknown2;
		int lUnknown3;
		int lUnknown4;
		int lUnknown5;
		int lUnknown6;
		int lUnknown7;	// Esse aqui s� tem no JP, esse valor a+, peguei ele sempre zero, das vezes que vi no pacote
	};

	// Friend Info Ex
	struct FriendInfoEx : public FriendInfo {
		FriendInfoEx(unsigned int  _ul = 0u) : FriendInfo() {
			clear();
		};
		void clear() {
			
			FriendInfo::clear();

			cUnknown_flag = -1;
			level = 0u;
			flag.clear();
			state.clear();
		};
		union uState {
			void clear() { memset(this, 0, sizeof(uState)); };
			unsigned char ucState;
			struct {
				unsigned char sex				: 1;	// Bit 0
				unsigned char online			: 1;	// Bit 1
				unsigned char _friend			: 1;	// Bit 2
				unsigned char request_friend	: 1;	// Bit 3
				unsigned char block				: 1;	// Bit 4
				unsigned char play				: 1;	// Bit 5
				unsigned char AFK				: 1;	// Bit 6
				unsigned char busy				: 1;	// Bit 7
			}stState;
		};
		union uFlag {
			void clear() { memset(this, 0, sizeof(uFlag)); };
			unsigned char ucFlag;
			struct {
				unsigned char _friend		: 1;		// Bit 0
				unsigned char guild_member	: 1, : 0;	// Bit 1 and rest of Bits from 1 Byte
			}stFlag;
		};
		char cUnknown_flag;
		uFlag flag;						// Flag se o player � amigo ou � membro guild
		uState state;					// Sex, online, friend, request, block e etc
		unsigned char level;
	};

	// Many Packet
	struct ManyPacket {
	public:
		ManyPacket(const unsigned short _size, const unsigned short _limit) 
			: const_total(_size), const_limit(_limit) {
			
			// Initialize data
			init();
		};
		void clear() {
			memset(this, 0, sizeof(ManyPacket));
		};
		void init() {
			// Calcula Initial data

			paginas = (const_total / const_limit);

			if ((const_total % const_limit) != 0)
				++paginas;

			pag.pagina = 1;
			pag.total = const_total;
			pag.current = (const_total <= const_limit) ? const_total : const_limit;

			// Calcule Index
			calcIndex();
		};
		void operator++ (int _i) {
			increse();
		};
		void operator++ () {
			increse();
		};
		void increse() {

			if (pag.total > 0) {
				pag.pagina++;
				
				if (pag.total <= const_limit)
					pag.current = pag.total = 0;
				else {
					pag.total -= const_limit;
					pag.current = (pag.total <= const_limit) ? pag.total : const_limit;
				}

				// Cacule Index
				calcIndex();
			}
		};
		struct Pagina {
			void clear() { memset(this, 0, sizeof(Pagina)); };
			unsigned char pagina;
			unsigned short total;
			unsigned short current;
		};
		struct Index {
			void clear() { memset(this, 0, sizeof(Index)); };
			unsigned short start;
			unsigned short end;
		};
	protected:
		void calcIndex() {

			// Calcule Index
			index.start = (pag.pagina - 1) * const_limit;
			index.end = index.start + ((pag.total <= const_limit) ? pag.total : const_limit);
		};
		const unsigned short const_total;
	public:
		const unsigned short const_limit;
		unsigned short paginas;
		Pagina pag;
		Index index;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_PANGYA_MESSAGE_ST_HPP
