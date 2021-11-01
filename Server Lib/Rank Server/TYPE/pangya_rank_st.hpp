// Arquivo pangya_rank_st.hpp
// Criado em 15/06/2020 as 15:03 por Acrisio
// Defini��o dos tipos usados no Rank Server

#pragma once
#ifndef _STDA_PANGYA_RANK_ST_HPP
#define _STDA_PANGYA_RANK_ST_HPP

#include <memory>

#include "../../Projeto IOCP/TYPE/pangya_st.h"

namespace stdA {

// Macro enum operator++
// __type � o nome do enum(type)
// _element � a vari�vel(obbject) do enum
#define ENUM_OPERATOR_PLUS_PLUS(__type, _element) (_element) = __type(static_cast< std::underlying_type< __type >::type >((_element)) + 1)

#if defined(__linux__)
#pragma pack(1)
#endif

	// Player Info
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

	// Rank Menu Index
	enum eRANK_MENU : unsigned char {
		RM_OVERALL,
		RM_COURSE_RECORDS,
		RM_RECORDS,
		RM_COURSE_RECORDS_NATURAL,
		RM_COURSE_RECORDS_GRAND_PRIX,
	};

	// Rank Menu Item
	enum eRANK_OVERALL : unsigned char {
		RO_TOTAL_POINTS,
		RO_TOTAL_SCORE,
		RO_TROPHY_POINTS,
		RO_PANG_EARNED,
		RO_TOTAL_HOLES,
		RO_ACHIEVEMENT_POINTS,
	};

	enum eRANK_COURSE_RECORDS : unsigned char {
		RCR_BLUE_LAGOON,
		RCR_BLUE_WATER,
		RCR_SEPIA_WIND,
		RCR_WIND_HILL,
		RCR_WIZ_WIZ,
		RCR_WEST_WIZ,
		RCR_BLUE_MOON,
		RCR_SILVIA_CANNON,
		RCR_ICE_CANNON,
		RCR_WHITE_WIZ,
		RCR_SHINNING_SAND,
		RCR_PINK_WIND,
		RCR_DEEP_INFERNO,
		RCR_ICE_SPA,
		RCR_LOST_SEAWAY,
		RCR_EASTERN_VALLEY,
		RCR_ICE_INFERNO,
		RCR_WIZ_CITY,
		RCR_ABBOT_MINE,
		RCR_MYSTIC_RUINS,
	};

	enum eRANK_RECORDS : unsigned char {
		RR_ALBATROSS,
		RR_HOLE_IN_ONE,
		RR_LEVEL = 3,
		RR_TOTAL_DISTANCE,
	};

	enum ePLAYER_POSITION_RANK_TYPE : unsigned char {
		PPRT_IN_TOP_RANK,		// Tem registro e est� no top rank
		PPRT_NOT_RANK,			// N�o tem registro
		PPRT_NOT_TOP_RANK,		// Tem registro mas n�o est� no top rank
	};

	// Rank Pesquisa dados
	struct search_dados {
		public:
			search_dados(unsigned int  _ul = 0u)
				: rank_menu(eRANK_MENU::RM_OVERALL), rank_menu_item(eRANK_OVERALL::RO_TOTAL_POINTS),
				term_s5_type(0u), class_type(0u), page(0u) {
			};
			~search_dados() {
				clear();
			};
			void clear() {
				memset(this, 0, sizeof(search_dados));
			};
			std::string toString() {
				return "RANK_MENU: " + std::to_string((unsigned short)rank_menu)
					+ "\nRANK_MENU_ITEM: " + std::to_string((unsigned short)rank_menu_item)
					+ "\nPAGE: " + std::to_string(page)
					+ "\nTERM_S5_TYPE: " + std::to_string(term_s5_type)
					+ "\nCLASS_TYPE: " + std::to_string(class_type);
			};

		public:
			eRANK_MENU rank_menu;
			unsigned char rank_menu_item;
			unsigned char term_s5_type;			// Op��es descontinuadas no Fresh UP!, por�m ele ainda mant�m nos packet
			unsigned char class_type;			// Op��es descontinuadas no Fresh UP!, por�m ele ainda mant�m nos packet
			unsigned int  page;
	};

	// Rank Pesquisa dados Ex
	struct search_dados_ex : public search_dados {
		public:
			search_dados_ex(unsigned int  _ul = 0u) : search_dados(_ul), active(0u) {
			};
			~search_dados_ex() {
				clear();
			};
			void clear() {
				memset(this, 0, sizeof(search_dados_ex));
			};
			std::string toString() {
				return search_dados::toString() + "\nACTIVE: " + std::to_string((unsigned short)active);
			};

		public:
			unsigned char active;
	};

	struct key_menu {
		public:
			key_menu(unsigned int  _ul = 0u) : m_menu(eRANK_MENU::RM_OVERALL), m_item(0u) {
			};
			key_menu(eRANK_MENU _menu, unsigned char _item) : m_menu(_menu), m_item(_item) {
			};
			virtual ~key_menu() {
				clear();
			};

			void clear() {

				m_menu = eRANK_MENU::RM_OVERALL;
				m_item = 0u;
			};

			inline bool operator ==(const key_menu& _km) const {
				return m_menu == _km.m_menu && m_item == _km.m_item;
			};

			inline bool operator !=(const key_menu& _km) const {
				return m_menu != _km.m_menu || m_item != _km.m_item;
			};

			inline bool operator <(const key_menu& _km) const {

				// Classifica primeiro pelo menu depois pelo item
				if (m_menu != _km.m_menu)
					return m_menu < _km.m_menu;
				else
					return m_item < _km.m_item;
			};

		public:
			eRANK_MENU m_menu;
			unsigned char m_item;
	};

	struct key_position {
		public:
			key_position(unsigned int  _ul = 0u) : m_uid(0u), m_position(0u) {
			};
			key_position(unsigned int  _uid, unsigned int  _position)
				: m_uid(_uid), m_position(_position) {
			};
			virtual ~key_position() {
				clear();
			};

			void clear() {

				m_uid = 0u;
				m_position = 0u;
			};

			inline bool operator ==(const key_position& _kp) const {
				return m_position == _kp.m_position && m_uid == _kp.m_uid;
			};

			inline bool operator !=(const key_position& _kp) const {
				return m_position != _kp.m_position || m_uid != _kp.m_uid;
			};

			inline bool operator <(const key_position& _kp) const {

				// Classifica primeiro pela position depois pelo uid
				if (m_position != _kp.m_position)
					return m_position < _kp.m_position;
				else
					return m_uid < _kp.m_uid;
			};

		public:
			unsigned int  m_uid;
			unsigned int  m_position;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_PANGYA_RANK_ST_HPP
