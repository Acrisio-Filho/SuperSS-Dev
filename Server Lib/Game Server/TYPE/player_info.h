// Arquivo player_info.h
// Criado em 04/03/2018 as 12:30  por Acrisio
// Definição da classe PlayerInfo

#pragma once
#ifndef _STDA_PLAYER_INFO_H
#define _STDA_PLAYER_INFO_H

#include "pangya_game_st.h"
#include "../UTIL/mgr_achievement.hpp"
#include "../UTIL/extra_power.hpp"
#include "../GAME/player_mail_box.hpp"

#include <map>

namespace stdA {
    class PlayerInfo : public player_info {
		public:
			enum enLEVEL : uint32_t {
				ROOKIE_F, ROOKIE_E, ROOKIE_D, ROOKIE_C, ROOKIE_B, ROOKIE_A,
				BEGINNER_E, BEGINNER_D, BEGINNER_C, BEGINNER_B, BEGINNER_A,
				JUNIOR_E, JUNIOR_D, JUNIOR_C, JUNIOR_B, JUNIOR_A,
				SENIOR_E, SENIOR_D, SENIOR_C, SENIOR_B, SENIOR_A,
				AMADOR_E, AMADOR_D, AMADOR_C, AMADOR_B, AMADOR_A,
				SEMI_PRO_E, SEMI_PRO_D, SEMI_PRO_C, SEMI_PRO_B, SEMI_PRO_A,
				PRO_E, PRO_D, PRO_C, PRO_B, PRO_A,
				NACIONAL_E, NACIONAL_D, NACIONAL_C, NACIONAL_B, NACIONAL_A,
				WORLD_PRO_E, WORLD_PRO_D, WORLD_PRO_C, WORLD_PRO_B, WORLD_PRO_A,
				MESTRE_E, MESTRE_D, MESTRE_C, MESTRE_B, MESTRE_A,
				TOP_MASTER_V, TOP_MASTER_IV, TOP_MASTER_III, TOP_MASTER_II, TOP_MASTER_I,
				WORLD_MASTER_V, WORLD_MASTER_IV, WORLD_MASTER_III, WORLD_MASTER_II, WORLD_MASTER_I,
				LEGEND_V, LEGEND_IV, LEGEND_III, LEGEND_II, LEGEND_I,
				INFINIT_LEGEND_V, INFINIT_LEGEND_IV, INFINIT_LEGEND_III, INFINIT_LEGEND_II, INFINIT_LEGEND_I
			};

			const uint32_t ExpByLevel[71] = { 30u, 40u, 50u, 60u, 70u, 140u,					// ROOKIE
												   105u, 125u, 145u, 165u, 330u,					// BEGINNER
												   248u, 278u, 308u, 338u, 675u,					// JUNIOR
												   506u, 546u, 586u, 626u, 1253u,					// SENIOR
												   1002u, 1052u, 1102u, 1152u, 2304u,				// AMADOR
												   1843u, 1903u, 1963u, 2023u, 4046u,				// SEMI PRO
												   3237u, 3307u, 3377u, 3447u, 6894u,				// PRO
												   5515u, 5595u, 5675u, 5755u, 11511u,				// NACIONAL
												   8058u, 8148u, 8238u, 8328u, 16655u,				// WORLD PRO
												   8328u, 8428u, 8528u, 8628u, 17255u,				// MESTRE
												   9490u, 9690u, 9890u, 10090u, 20181u,			// TOP_MASTER
												   20181u, 20481u, 20781u, 21081u, 42161u,			// WORLD_MASTER
												   37945u, 68301u, 122942u, 221296u, 442592u,		// LEGEND
												   663887u, 995831u, 1493747u, 2240620u, (uint32_t)~0u };	// INFINIT_LEGEND

			struct stIdentifyKey {
				stIdentifyKey(uint32_t __typeid, int32_t _id)
					: _typeid(__typeid), id(_id) {
				};
				inline bool operator < (const stIdentifyKey& _ik) const {

					// Classifica pelo ID, depois o typeid
					if (id != _ik.id)
						return id < _ik.id;
					else
						return _typeid < _ik._typeid;
				};
				uint32_t _typeid;
				int32_t id;
			};

			/*
			* Skin[Title] map Call back function to trate Condition 
			*/
			struct stTitleMapCallback {
				
				// Function Callback type
				typedef int (*callback)(void*);

				// Constructor
				stTitleMapCallback(uint32_t _ul = 0u) {
					clear();
				};
				stTitleMapCallback(callback _callback, void* _arg) : call_back(_callback), arg(_arg) {
				}
				void clear() {
					memset(this, 0, sizeof(this));
				};
				int exec() {

					if (call_back != nullptr)
						return call_back(arg);
					else
						_smp::message_pool::getInstance().push(new message("[PlayerInfo::stTitleMapCallBack::exec][Error] call_back is nullptr.", CL_FILE_LOG_AND_CONSOLE));

					return 0;
				};
			private:
				//uint32_t id;
				callback call_back;
				void* arg;
			};

		private:
			std::map< uint32_t/*key*/, stTitleMapCallback > mp_title_callback;

			static int better_hit_pangya_bronze(void* _arg);
			static int better_fairway_bronze(void* _arg);
			static int better_putt_bronze(void* _arg);
			static int better_quit_rate_bronze(void* _arg);
			static int better_hit_pangya_silver(void* _arg);
			static int better_fairway_silver(void* _arg);
			static int better_putt_silver(void* _arg);
			static int better_quit_rate_silver(void* _arg);
			static int better_hit_pangya_gold(void* _arg);
			static int better_fairway_gold(void* _arg);
			static int better_putt_gold(void* _arg);
			static int better_quit_rate_gold(void* _arg);
			static int atirador_de_ouro(void* _arg);
			static int atirador_de_silver(void* _arg);
			static int atirador_de_bronze(void* _arg);
			static int master_course(void* _arg);
			static int natural_record_80(void* _arg);
			static int natural_record_200(void* _arg);
			static int natural_record_300(void* _arg);
			static int natural_record_350(void* _arg);
			static int natural_record_390(void* _arg);
			static int natural_record_420(void* _arg);
			static int natural_record_470(void* _arg);
			static int natural_record_540(void* _arg);

        public:
            PlayerInfo();
            ~PlayerInfo();

			void clear();

			// getTitleMapCallBack
			stTitleMapCallback* getTitleCallBack(uint32_t _id);

			// get Size Cup Grand Zodiac from Grand Zodiac Pontos
			uint32_t getSizeCupGrandZodiac();

			// Finder

			// Friend
			FriendInfo* findFriendInfoByUID(uint32_t _uid);
			FriendInfo* findFriendInfoById(std::string _id);
			FriendInfo* findFriendInfoByNickname(std::string _nickname);

			// Itens Equipaveis
			WarehouseItemEx* findWarehouseItemById(int32_t _id);
			WarehouseItemEx* findWarehouseItemByTypeid(uint32_t _typeid);
			WarehouseItemEx* findWarehouseItemByTypeidAndId(uint32_t _typeid, int32_t _id);	// Precisa desse para caso tenho um item com o mesmo id, mas com typeid diferente

			CharacterInfo* findCharacterById(int32_t _id);
			CharacterInfo* findCharacterByTypeid(uint32_t _typeid);
			CharacterInfo* findCharacterByTypeidAndId(uint32_t _typeid, int32_t _id);	// Precisa desse para caso tenho um item com o mesmo id, mas com typeid diferente

			CaddieInfoEx* findCaddieById(int32_t _id);
			CaddieInfoEx* findCaddieByTypeid(uint32_t _typeid);
			CaddieInfoEx* findCaddieByTypeidAndId(uint32_t _typeid, int32_t _id);	// Precisa desse para caso tenho um item com o mesmo id, mas com typeid diferente

			MascotInfoEx* findMascotById(int32_t _id);
			MascotInfoEx* findMascotByTypeid(uint32_t _typeid);
			MascotInfoEx* findMascotByTypeidAndId(uint32_t _typeid, int32_t _id);	// Precisa desse para caso tenho um item com o mesmo id, mas com typeid diferente

			MyRoomItem* findMyRoomItemById(int32_t _id);	// Furniture
			MyRoomItem* findMyRoomItemByTypeid(uint32_t _typeid);

			CardInfo* findCardById(int32_t _id);
			CardInfo* findCardByTypeid(uint32_t _typeid);

			CardEquipInfoEx* findCardEquipedById(int32_t _id, uint32_t _char_typeid, uint32_t _slot);
			CardEquipInfoEx* findCardEquipedByTypeid(uint32_t _typeid, uint32_t _char_typeid = 0u, uint32_t _slot = 0u, uint32_t _tipo = 0u, uint32_t _efeito = 0u);

			// Troféu especial
			TrofelEspecialInfo* findTrofelEspecialById(int32_t _id);
			TrofelEspecialInfo* findTrofelEspecialByTypeid(uint32_t _typeid);
			TrofelEspecialInfo* findTrofelEspecialByTypeidAndId(uint32_t _typeid, int32_t _id);

			TrofelEspecialInfo* findTrofelGrandPrixById(int32_t _id);
			TrofelEspecialInfo* findTrofelGrandPrixByTypeid(uint32_t _typeid);
			TrofelEspecialInfo* findTrofelGrandPrixByTypeidAndId(uint32_t _typeid, int32_t _id);

			// Return Iterator - inline methods
			std::multimap< int32_t/*ID*/, WarehouseItemEx >::iterator findWarehouseItemItById(int32_t _id);
			std::multimap< int32_t/*ID*/, WarehouseItemEx >::iterator findWarehouseItemItByTypeid(uint32_t _typeid);
			std::multimap< int32_t/*ID*/, WarehouseItemEx >::iterator findWarehouseItemItByTypeidAndId(uint32_t _typeid, int32_t _id);

			std::multimap< int32_t/*ID*/, CharacterInfo >::iterator findCharacterItById(int32_t _id);
			std::multimap< int32_t/*ID*/, CharacterInfo >::iterator findCharacterItByTypeid(uint32_t _typeid);
			std::multimap< int32_t/*ID*/, CharacterInfo >::iterator findCharacterItByTypeidAndId(uint32_t _typeid, int32_t _id);

			std::multimap< int32_t/*ID*/, CaddieInfoEx >::iterator findCaddieItById(int32_t _id);
			std::multimap< int32_t/*ID*/, CaddieInfoEx >::iterator findCaddieItByTypeid(uint32_t _typeid);
			std::multimap< int32_t/*ID*/, CaddieInfoEx >::iterator findCaddieItByTypeidAndId(uint32_t _typeid, int32_t _id);

			std::multimap< int32_t/*ID*/, MascotInfoEx >::iterator findMascotItById(int32_t _id);
			std::multimap< int32_t/*ID*/, MascotInfoEx >::iterator findMascotItByTypeid(uint32_t _typeid);
			std::multimap< int32_t/*ID*/, MascotInfoEx >::iterator findMascotItByTypeidAndId(uint32_t _typeid, int32_t _id);

			std::vector< MyRoomItem >::iterator findMyRoomItemItById(int32_t _id);
			std::vector< MyRoomItem >::iterator findMyRoomItemItByTypeid(uint32_t _typeid);

			std::vector< CardInfo >::iterator findCardItById(int32_t _id);
			std::vector< CardInfo >::iterator findCardItByTypeid(uint32_t _typeid);

			std::vector< CardEquipInfoEx >::iterator findCardEquipedItById(int32_t _id, uint32_t _char_typeid, uint32_t _slot);
			std::vector< CardEquipInfoEx >::iterator findCardEquipedItByTypeid(uint32_t _typeid, uint32_t _char_typeid = 0u, uint32_t _slot = 0u, uint32_t _tipo = 0u, uint32_t _efeito = 0u);

			// Troféu especial
			std::vector< TrofelEspecialInfo>::iterator findTrofelEspecialItById(int32_t _id);
			std::vector< TrofelEspecialInfo>::iterator findTrofelEspecialItByTypeid(uint32_t _typeid);
			std::vector< TrofelEspecialInfo>::iterator findTrofelEspecialItByTypeidAndId(uint32_t _typeid, int32_t _id);

			std::vector< TrofelEspecialInfo>::iterator findTrofelGrandPrixItById(int32_t _id);
			std::vector< TrofelEspecialInfo>::iterator findTrofelGrandPrixItByTypeid(uint32_t _typeid);
			std::vector< TrofelEspecialInfo>::iterator findTrofelGrandPrixItByTypeidAndId(uint32_t _typeid, int32_t _id);

			// Find All Part Not Equiped
			std::vector< WarehouseItemEx* > findAllPartNotEquiped(uint32_t _typeid);

			ItemBuffEx* findItemBuff(uint32_t _typeid, uint32_t _tipo = 0u);

			// Find Update Item
			std::multimap< stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator findUpdateItemByTypeid(uint32_t _typeid);
			std::multimap< stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator findUpdateItemByType(UpdateItem::UI_TYPE _type);
			std::vector< std::multimap< stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator > findUpdateItemByTypeidAndId(uint32_t _typeid, int32_t _id);
			std::multimap< stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator findUpdateItemByIdAndType(int32_t _id, UpdateItem::UI_TYPE _type);
			std::multimap< stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator findUpdateItemByTypeidAndType(uint32_t _typeid, UpdateItem::UI_TYPE _type);

			GrandPrixClear* findGrandPrixClear(uint32_t _typeid);

			// Get Power Extra
			ExtraPower getExtraPower(bool _pwr_condition_actived);

			// Get Slot Power
			uint32_t getSlotPower();

			int32_t getCharacterMaxSlot(CharacterInfo::Stats _stats);
			int32_t getClubSetMaxSlot(CharacterInfo::Stats _stats);

			// --- Checkers

			// Verifica se ele tem Item Boost equipado (agora só pang mastery e nitro) exp acho que não precisa agora
			PlayerRoomInfo::uItemBoost checkEquipedItemBoost();

			// Verifica se fez record em todos course, que pode fazer record
			// Cria um map com todos os maps que foram feito record, no normal, grand prix e natural, excluido o record assit
			bool isMasterCourse();

			// Verifica se está com o item equipado
			bool checkEquipedItem(uint32_t _typeid);

			// Soma dos score dos record do natural(mas o JP pega o do grand prix)
			int32_t getSumRecordGrandPrix();

			// verifica se é um amigo
			bool isFriend(uint32_t _uid);

			// Tem o item(Possuí o item)
			bool ownerCaddieItem(uint32_t _typeid);
			bool ownerHairStyle(uint32_t _typeid);
			bool ownerSetItem(uint32_t _typeid);
			bool ownerItem(uint32_t _typeid, int option = 0);	// Verifica todos os itens;
			bool ownerMailBoxItem(unsigned _typeid);

			bool isPartEquiped(uint32_t _typeid, int32_t _id);
			bool isAuxPartEquiped(uint32_t _typeid);

			// Consome moedas
			void consomeMoeda(uint64_t _pang, uint64_t _cookie);
			void consomeCookie(uint64_t _cookie);
			void consomePang(uint64_t _pang);

			// Adiciona moedas
			void addMoeda(uint64_t _pang, uint64_t _cookie);
			void addCookie(uint64_t _cookie);
			void addPang(uint64_t _pang);

			// Atualiza os valores do server com o que está no banco de dados
			void updateMoeda();
			void updateCookie();
			void updatePang();
			
			// Adiciona Pang Estático
			static void addPang(uint32_t _uid, uint64_t _pang);

			// Adiciona Cookie Point(CP) Estático
			static void addCookie(uint32_t _uid, uint64_t _cookie);

			// Add (Soma) User Info
			void addUserInfo(UserInfoEx& _ui, uint64_t _total_pang_win_game = 0ull);

			// Update User Info ON DB
			void updateUserInfo();

			// Update User Info ON DB Estático
			static void updateUserInfo(uint32_t _uid, UserInfoEx& _ui);

			// Update Trofel Info
			void updateTrofelInfo(uint32_t _trofel_typeid, unsigned char _trofel_rank);

			// Update Trofel Info Estático
			static void updateTrofelInfo(uint32_t _uid, uint32_t _trofel_typeid, unsigned char _trofel_rank);

			// Update Medal
			void updateMedal(uMedalWin _medal_win);

			// Update Medal Estático
			static void updateMedal(uint32_t _uid, uMedalWin _medal_win);

			// Adiciona Exp
			int addExp(uint32_t _exp);

			// update location player on DB
			void updateLocationDB();

			// Update Grand Prix Clear
			bool updateGrandPrixClear(uint32_t _typeid, uint32_t _position);

			// Update Grand Zodiac Pontos
			void addGrandZodiacPontos(uint64_t _pontos);

			// Methods Static
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		private:
			bool checkAlterationCookieOnDB();
			bool checkAlterationPangOnDB();

        public:
			uint64_t cookie;
			CouponGacha cg;
			MemberInfoEx mi;
			UserInfoEx ui;
			EquipedItem ei;
			ClubSetWorkshopLasUpLevel cwlul;
			ClubSetWorkshopTransformClubSet cwtc;
			PremiumTicket pt;
			TrofelInfo ti_current_season;
			TrofelInfo ti_rest_season;
			TutorialInfo TutoInfo;
			UserEquip ue;
			chat_macro_user cmu;
			MapStatistics a_ms_normal[MS_NUM_MAPS];
			MapStatistics a_msa_normal[MS_NUM_MAPS];
			MapStatistics a_ms_natural[MS_NUM_MAPS];
			MapStatistics a_msa_natural[MS_NUM_MAPS];
			MapStatistics a_ms_grand_prix[MS_NUM_MAPS];
			MapStatistics a_msa_grand_prix[MS_NUM_MAPS];
			MapStatistics aa_ms_normal_todas_season[9][MS_NUM_MAPS];	// Esse aqui é diferente, explico ele no pacote principal
			std::map< size_t, StateCharacterLounge > mp_scl;

			std::multimap< int32_t/*ID*/, CharacterInfo > mp_ce;		// Tem que usar multimap aqui, para nao ficar realocando memória, uso o ponteiro de um element, para o item equipado
			std::multimap< int32_t/*ID*/, CaddieInfoEx > mp_ci;		// Tem que usar multimap aqui, para nao ficar realocando memória, uso o ponteiro de um element, para o item equipado
			std::multimap< int32_t/*ID*/, MascotInfoEx > mp_mi;		// Tem que usar multimap aqui, para nao ficar realocando memória, uso o ponteiro de um element, para o item equipado
			std::multimap< int32_t/*ID*/, WarehouseItemEx > mp_wi;		// Tem que usar multimap aqui, para nao ficar realocando memória, uso o ponteiro de um element, para o item equipado
			
			std::map< uint32_t/*UID*/, FriendInfo > mp_fi;	// Friend List

			AttendanceRewardInfoEx ari;

			MgrAchievement mgr_achievement;				// Manager Achievement
			std::vector< CardInfo > v_card_info;
			
			std::vector< CardEquipInfoEx > v_cei;
			std::vector< ItemBuffEx > v_ib;

			std::multimap< stIdentifyKey/*int32_t/*ID*/, UpdateItem > mp_ui;

			std::vector< TrofelEspecialInfo > v_tsi_current_season;
			std::vector< TrofelEspecialInfo > v_tsi_rest_season;
			std::vector< TrofelEspecialInfo > v_tgp_current_season;	// Trofel Grand Prix
			std::vector< TrofelEspecialInfo > v_tgp_rest_season; // Trofel Grand Prix
			std::vector< MyRoomItem > v_mri;		// MyRoomItem

			std::vector< GrandPrixClear > v_gpc;	// Grand Prix Clear os grand prix que o player já jogou

			MyRoomConfig mrc;
			DolfiniLocker df;	// DolfiniLocker
			GuildInfoEx gi;
			DailyQuestInfoUser dqiu;
			Last5PlayersGame l5pg;
			struct stLocation {
				stLocation();
				stLocation(float _x, float _z, float _r);
				void clear();
				stLocation& operator+=(stLocation& _add_location);
				float x;
				float y;
				float z;
				float r;	// Face
			};
			stLocation location;
			unsigned char place;			// Lugar que o player está no momento
			unsigned char lobby;			// Lobby
			unsigned char channel;			// Channel
			unsigned char whisper : 1, : 0;	// Whisper 0 e 1, 0 OFF, 1 ON
			uint32_t state;
			uint32_t state_lounge;
			unsigned char m_state_logged;		// State logged que usa no login server, e que eu possa usar aqui, por que tbm tenho que prevenir contra ataques DDoS
#define m_cap mi.capability
			uint64_t grand_zodiac_pontos;

			uint64_t m_legacy_tiki_pts; // Point Shop(Tiki Shop antigo)

			// Mail Box
			PlayerMailBox m_mail_box;

		private:
#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif

			stPlayerLocationDB m_pl;
			stSyncUpdateDB m_update_pang_db;
			stSyncUpdateDB m_update_cookie_db;
    };
}

#endif