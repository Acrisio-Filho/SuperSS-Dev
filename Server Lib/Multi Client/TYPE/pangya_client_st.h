// Arquivo pangya_client_st.h
// Criado em 01/04/2018 as 22:34 por Acrisio
// Definição das estruturas usadas no cliente

#pragma once
#ifndef _STDA_PANGYA_CLIENT_ST_H
#define _STDA_PANGYA_CLIENT_ST_H

#include "../../Projeto IOCP/TYPE/pangya_st.h"

namespace stdA {
    
    struct client_info  {
        void clear() { memset(this, 0, sizeof(client_info)); };

		char m_client_version[20];		// Cliente versão "855.00"
		unsigned long m_packet_version;	// Packet Version 2016121900

		unsigned long m_uid;		// Player Unico ID
		unsigned long m_oid;		// Player Online ID
		unsigned long m_guid;		// Game Server Unico ID
		unsigned long m_luid;		// Login Server Unico ID
		unsigned long m_muid;		// Message Server Unico ID
		unsigned long m_ruid;		// Rank Server Unico ID
		unsigned long m_cuid;		// Canal Unico ID
		unsigned long m_cap;		// Player Capability
		unsigned long m_TTL;		// Time To Live[TTL]
		char m_nickname[22];		// Player Nickname
		char m_id[22];				// Player ID
		char m_pass[32];			// Player Password
		unsigned char m_level;		// Player Level
		char m_keys[2][8];			// Player Keys of authentic to Game Server
		char m_web_key[8];			// Web Key
		unsigned char m_TRWK;		// Type Request Web Key

		char link_gacha[1024];		// Link do Gacha
		char link_guild[1024];		// Link da Guild
		char link_point[1024];		// Link do payment
		char link_entrance[1024];	// Link Entrance
		char link_weblink[3][1024];	// 3 Link do Weblink

		long volatile m_RCLOP;		// Recebe Canais Ler Outros Pacotes
		long volatile m_RC;			// Recebe Canais
    };

	struct server_list {
		void clear() {
			if (a_servers != nullptr)
				delete[] a_servers;

			memset(this, 0, sizeof(server_list));
		};
		size_t num_servers;
		ServerInfo *a_servers;
	};

	struct ChannelInfo {
		void clear() {
			memset(this, 0, sizeof(ChannelInfo));
		};
		char name[64];
		short max_user;
		short curr_user;
		unsigned char id;
		long flag;
		long flag2;
		long min_level_allow;
		long max_level_allow;
	};

	struct canal_list {
		void clear() {
			if (a_canais != nullptr)
				delete[] a_canais;

			memset(this, 0, sizeof(canal_list));
		};
		size_t num_canais;
		ChannelInfo *a_canais;
	};

	/*struct ctx_client {
		void clear() { memset(this, 0, sizeof(ctx_client)); };
		char m_client_version[20];		// Cliente versão "855.00"
		unsigned long m_packet_version;	// Packet Version 2016121900
		char m_id[22];				// Player ID
		char m_pass[32];			// Player Password
	};*/
}

#endif !_STDA_PANGYA_CLIENT_ST_H