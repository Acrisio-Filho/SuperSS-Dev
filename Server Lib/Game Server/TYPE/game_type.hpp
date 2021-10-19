// Arquivo game_type.hpp
// Criado em 19/08/2018 as 12:09 por Acrisio
// Definição dos tipos usados na classe Game

#pragma once
#ifndef _STDA_GAME_TYPE_HPP
#define _STDA_GAME_TYPE_HPP

#include "pangya_game_st.h"

#include "../UTIL/sys_achievement.hpp"

#include <ctime>

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

#define STDA_END_LINE "\r\n"

#define TIME_BOOSTER_TYPEID		0x1A000011ul
#define COIN_TYPEID				0x1A000010ul

#ifndef SPINNING_CUBE_TYPEID
#define SPINNING_CUBE_TYPEID	0x1A00015Bul
#endif // !SPINNING_CUBE_TYPEID

#define TICKET_REPORT_SCROLL_TYPEID 0x1A000042ul
#define TICKET_REPORT_TYPEID		0x1A000041ul

// Anel que da +1.1% de Club Mastery
#define KURAFAITO_RING_CLUBMASTERY 0x70210009ul

//	0x1A00019F/*Auto Commander*/
// Auto Command [Special Shot Help]
#define AUTO_COMMAND_TYPEID 0x1A00019Ful

// Auto Caliper
#define AUTO_CALIPER_TYPEID 0x1A000040ul

// Power Milk
constexpr uint32_t POWER_MILK_TYPEID = 0x18000025u;

// CheckPassiveItem
#define CHECK_PASSIVE_ITEM(_typeid) (sIff::getInstance().getItemGroupIdentify((_typeid)) == iff::ITEM && sIff::getInstance().getItemSubGroupIdentify24((_typeid)) > 1/*Passive Item*/)

///* ROUND SOMA LEVEL ROOM GAME
// soma "Total Level"
// Return ROUND soma
#define STDA_ROUND_SOMA_LEVEL(_soma) ((_soma) + (5 - ((_soma) % 5)) - 5)

///* TRANSFORMA O VALOR de porcentagem de exibição 100% para 1,0
#define TRANSF_SERVER_RATE_VALUE(_value) ((_value) <= 0 ? 1 : (_value) / 100.f)

///* isSilentWindItem
// Argument:
// typeid
// Return bool
#define isSilentWindItem(_typeid) std::find(silent_wind_item, LAST_ELEMENT_IN_ARRAY(silent_wind_item), (_typeid)) != LAST_ELEMENT_IN_ARRAY(silent_wind_item)

///* isSafetyItem
// Argument:
// typeid
// Return bool
#define isSafetyItem(_typeid) std::find(safety_item, LAST_ELEMENT_IN_ARRAY(safety_item), (_typeid)) != LAST_ELEMENT_IN_ARRAY(safety_item)

///* MAKE TROFEL ROOM GAME
// Argument:
//	soma "Total Level"
//	num_player "Numero de Jogadores
// Return trofel Typeid or 0
#define STDA_MAKE_TROFEL(_soma, _num_player) (((_num_player) != 0) ? (iff::MATCH << 26) | (((STDA_ROUND_SOMA_LEVEL(_soma) / (_num_player)) / 5) << 16)  : 0)

	const uint32_t silent_wind_item[]{ 0x18000006, 0x1800002C, 0x1800002D, 0x1800002F };

	const uint32_t safety_item[]{ 0x18000028, 0x1800002D };

	const uint32_t passive_item[]{ 0x1A00000A, 0x1A00000B, 0x1A00000D, 0x1A00000E, 0x1A00000F/*1 Por Game*/, 0x1A000013, 0x1A000014/*1 Por Game*/
															, 0x1A00002F, 0x1A000035, 0x1A000084, 0x1A000085, 0x1A000086, 0x1A000090, 0x1A000099, 0x1A0000AD, 0x1A0000FC/*Double Exp*/,
										0x1A000001/*2*/ , 0x1A000002/*2*/ , 0x1A0000AE/*2*/  , 0x1A000005/*x4*/ , 0x1A0003B7/*x4*/, 0x1A0001D7/*0,5*/, 0x1A0001D8/*0,5*/
																		, 0x1A00025A/*0,4*/, 0x1A000007/*0,2*/, 0x1A000008/*0,2*/, 0x1A000009/*0,2*/ , 0x1A00000C/*0,2*/ /*Double Pang*/,
										0x1A000040/*Auto Caliper*/ , 0x1A000011/*Time Booster*/ , 0x1A00019F/*Auto Commander*/ , 0x1A0001A0/*Vector Sign*/ , 0x1A000136/*Fairy's Tears*/ ,
										0x1A000338/*Banana Club Mastery Boost*/ /*Help On Game*/ };

	// Consome 1 por jogo
	// Pirulito x2 de Exp boost que só consome 1 por jogo, os outros consome por hole
	const uint32_t passive_item_exp_1perGame[]{ 0x1A00000F/*1 Por Game*/, 0x1A000014/*1 Por Game*/ };

	// Exp Boost todos são x2
	const uint32_t passive_item_exp[]{ 0x1A00000A, 0x1A00000B, 0x1A00000D, 0x1A00000E, 0x1A00000F/*1 Por Game*/, 0x1A000013, 0x1A000014/*1 Por Game*/
										  , 0x1A00002F, 0x1A000035, 0x1A000084, 0x1A000085, 0x1A000086, 0x1A000090, 0x1A000099, 0x1A0000AD, 0x1A0000FC,/*Double Exp*/ };
	
	// Pang Boost X2
	const uint32_t passive_item_pang_x2[]{ 0x1A000001/*2*/ , 0x1A000002/*2*/ , 0x1A0000AE/*2*/ };

	// Pang Boost X4
	const uint32_t passive_item_pang_x4[]{ 0x1A000005/*x4*/, 0x1A0003B7/*x4*/, };

	// Pang Boost X1.5
	const uint32_t passive_item_pang_x1_5[]{ 0x1A0001D7/*0,5*/, 0x1A0001D8/*0,5*/ };

	// Pang Boost X1.4
	const uint32_t passive_item_pang_x1_4[]{ 0x1A00025A/*0,4*/ };

	// Pang Boost X1.2
	const uint32_t passive_item_pang_x1_2[]{ 0x1A000007/*0,2*/, 0x1A000008/*0,2*/, 0x1A000009/*0,2*/, 0x1A00000C/*0,2*/ };

	const uint32_t passive_item_pang[]{ 0x1A000001/*2*/, 0x1A000002/*2*/, 0x1A0000AE/*2*/, 
											 0x1A000005/*x4*/, 0x1A0003B7/*x4*/, 
											 0x1A0001D7/*0,5*/, 0x1A0001D8/*0,5*/,
											 0x1A00025A/*0,4*/,
											 0x1A000007/*0,2*/, 0x1A000008/*0,2*/, 0x1A000009/*0,2*/, 0x1A00000C/*0,2*/ /*Double Pang*/ };
	
	// Banana que da x2 Club Mastry, consome 1 por Hole [BANANA_CLUB_MASTERY_BOOST]
	const uint32_t passive_item_club_boost[]{ 0x1A000338 };

	union uSpecialShot {
		void clear() { memset(this, 0, sizeof(uSpecialShot)); };
		uint32_t ulSpecialShot;
		struct _stSpecialShot {
			uint32_t spin_front : 1;
			uint32_t spint_back : 1;
			uint32_t curve_left : 1;
			uint32_t curve_right : 1;
			uint32_t tomahawk : 1;
			uint32_t cobra : 1;
			uint32_t spike : 1;
			uint32_t _unused : 25;	// Não usa
		}stSpecialShot;
		std::string toString() {
			return "Spin Front: " + std::to_string(stSpecialShot.spin_front)
				+ " Spin Back: " + std::to_string(stSpecialShot.spint_back)
				+ " Curve Left: " + std::to_string(stSpecialShot.curve_left)
				+ " Curve Right: " + std::to_string(stSpecialShot.curve_right)
				+ " Tomahwak: " + std::to_string(stSpecialShot.tomahawk)
				+ " Cobra: " + std::to_string(stSpecialShot.cobra)
				+ " Spike: " + std::to_string(stSpecialShot.spike)
				+ " Unused: " + std::to_string(stSpecialShot._unused)
				;
		};
	};

	struct ShotDataBase {
		ShotDataBase(uint32_t _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(ShotDataBase)); };
		std::string toString() {
			return "Bar Point: Forca: " + std::to_string(bar_point[0]) + " Hit PangYa: " + std::to_string(bar_point[1]) + STDA_END_LINE
				+ "Ball Effect: X: " + std::to_string(ball_effect[0]) + " Y: " + std::to_string(ball_effect[1]) + STDA_END_LINE
				+ "Acerto PangYa Flag: " + std::to_string((unsigned short)acerto_pangya_flag) + STDA_END_LINE
				+ "Special Shot: " + special_shot.toString() + STDA_END_LINE
				+ "Time Hole SYNC: " + std::to_string(time_hole_sync) + STDA_END_LINE
				+ "Mira(shot): " + std::to_string(mira) + STDA_END_LINE
				+ "Time Shot: " + std::to_string(time_shot) + STDA_END_LINE
				+ "Bar Point: Start: " + std::to_string(bar_point1) + STDA_END_LINE
				+ "Club: " + std::to_string((unsigned short)club) + STDA_END_LINE
				+ "fUnknown: [1]: " + std::to_string(fUnknown[0]) + " [2]: " + std::to_string(fUnknown[1]) + STDA_END_LINE
				+ "Impact Zone Size Pixel: " + std::to_string(impact_zone_pixel) + STDA_END_LINE
				+ "Natural Wind: X: " + std::to_string(natural_wind[0]) + " Y: " + std::to_string(natural_wind[1]) + STDA_END_LINE
				;
		};
		float bar_point[2];					// [0] 2 Força, [1] 3 Impact Zone
		float ball_effect[2];				// [0] X Spin,  [1] Y Spin
		unsigned char acerto_pangya_flag;
		uSpecialShot special_shot;			// Especial Short, Tomahawk, Cobra e Spike
		uint32_t time_hole_sync;
		float mira;							// Mira da tacada do player, seria o R do location[x,y,z,r]
		uint32_t time_shot;
		float bar_point1;
		unsigned char club;
		float fUnknown[2];					// Float Unknown [0] 1 unknown, [1] 2 unknown 
		float impact_zone_pixel;
		int32_t natural_wind[2];				// Natural Wind Valor [0] X valor, [1] Y valor 
	};

	// Separei o spand time, que o pang battle não tem ele
	struct ShotData : public ShotDataBase {
		ShotData(uint32_t _ul = 0u) : ShotDataBase(_ul) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(ShotData));
		};
		std::string toString() {
			return ShotDataBase::toString() + "Spend Time Game: " + std::to_string(spend_time_game) + STDA_END_LINE;
		};
		float spend_time_game;				// O Acumolo de tempo gasto no jogo, é o tempo decorrido geral
	};

	struct ShotDataEx : public ShotData {
		ShotDataEx(uint32_t _ul = 0u) : ShotData(_ul) {
			clear();
		}
		void clear() { memset(this, 0, sizeof(ShotDataEx)); };
		struct PowerShot {
			PowerShot(uint32_t _ul = 0u) {
				clear();
			};
			std::string toString() {
				return "Option: " + std::to_string((unsigned short)option) + STDA_END_LINE
					+ "Decrease Power Shot: " + std::to_string(decrease_power_shot) + STDA_END_LINE
					+ "Increase Power Shot: " + std::to_string(increase_power_shot) + STDA_END_LINE
					;
			};
			void clear() { memset(this, 0, sizeof(PowerShot)); };
			unsigned char option;
			int32_t decrease_power_shot;
			int32_t increase_power_shot;
		};
		std::string toString() {
			return (option != 0) ? (power_shot.toString() + ShotData::toString()) : ShotData::toString();
		};
		unsigned short option;
		PowerShot power_shot;
	};

	struct ShotSyncData {
		void clear() { memset(this, 0, sizeof(ShotSyncData)); };
		struct Location {
			void clear() { memset(this, 0, sizeof(Location)); };
			std::string toString() {
				return "X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z: " + std::to_string(z);
			};
			float x;
			float y;
			float z;
		};
		enum SHOT_STATE : unsigned char {
			PLAYABLE_AREA = 2,
			OUT_OF_BOUNDS,
			INTO_HOLE,
			UNPLAYABLE_AREA,
		};
		std::string toString() {
			return "OID: " + std::to_string(oid) + STDA_END_LINE
				+ "Location: " + location.toString() + STDA_END_LINE
				+ "STATE: " + std::to_string((unsigned short)state) + STDA_END_LINE
				+ "Bunker Flag: " + std::to_string((unsigned short)bunker_flag) + STDA_END_LINE
				+ "ucUnknown: " + std::to_string((unsigned short)ucUnknown) + STDA_END_LINE
				+ "Pang: " + std::to_string(pang) + STDA_END_LINE
				+ "Pang Bonus: " + std::to_string(bonus_pang) + STDA_END_LINE
				+ "State Shot: " + state_shot.toString() + STDA_END_LINE
				+ "Tempo Shot: " + std::to_string(tempo_shot) + STDA_END_LINE
				+ "Grand Prix Penalidade: " + std::to_string((unsigned short)grand_prix_penalidade) + STDA_END_LINE
				;
		};
		uint32_t oid;
		Location location;
		SHOT_STATE state;
		unsigned char bunker_flag;
		unsigned char ucUnknown;		// Deve ser relacionando ao bunker esses negocios
		uint32_t pang;
		uint32_t bonus_pang;
		inline bool isMakeHole() {
			return state_shot.display.stDisplay.acerto_hole == 1u;
		};
		struct stStateShot {
			void clear() { memset(this, 0, sizeof(stStateShot)); };
			union uDisplayState {
				void clear() { memset(this, 0, sizeof(uDisplayState)); };
				uint32_t ulState;
				struct _stDisplay {
					unsigned char over_drive : 1;
					unsigned char _bit2_unknown : 1;
					unsigned char super_pangya : 1;					// Só da com Power shot na primeira tacada
					unsigned char special_shot : 1;
					unsigned char beam_impact : 1;
					unsigned char chip_in_17_a_199 : 1;
					unsigned char chip_in_200_plus : 1;
					unsigned char long_putt : 1;
					unsigned char acerto_hole : 1;
					unsigned char approach_shot : 1;				// Bonus
					unsigned char chip_in_with_special_shot : 1;	// Chipou com especial tacada Ex: Tomahawk, Cobra e Spike, Power Spin e Power Curve
					unsigned char _bit12_unknown : 1;
					unsigned char happy_bonus : 1;	// Happy Bonus
					unsigned char clear_bonus : 1;
					unsigned char aztec_bonus : 1;
					unsigned char recovery_bonus : 1;				// Esse é quando manda back spin e cai no rough e o spin puxa para o fairway
					unsigned char chip_in_without_special_shot : 1;	// Chipou sem especial tacada Ex: Tomahawk, Cobra e Spike
					unsigned char bound_bonus : 1;					// Ganha bonus pang para cada kick na bola no chão
					unsigned char _bit19_unknown : 1;
					unsigned char _bit20_unknown : 1;
					unsigned char mascot_bonus_with_pangya : 1;		// Só da na primeira tacada
					unsigned char mascot_bonus_without_pangya : 1;
					unsigned char special_bonus_with_pangya : 1;	// Special bonus quando acerta pangya (esse bonus da com angel wings)
					unsigned char special_bonus_without_pangya : 1;	// Special bonus quando erra pangya (esse bonus da com angel wings)
					unsigned char _bit25_unknown : 1;	// Esse pode ser Angel Bonus
					unsigned char _bit26_unknown : 1;	// Esse pode ser ANgel Bonus 
					unsigned char devil_bonus : 1;
					unsigned char _bit28_a_32_unknown : 5;
				}stDisplay;
			};
			union uShotState {
				void clear() { memset(this, 0, sizeof(uShotState)); };
				uint32_t ulState;
				struct _stShot {
					unsigned char _bit1_unknown : 1;
					unsigned char tomahawk : 1;
					unsigned char spike : 1;
					unsigned char cobra : 1;
					unsigned char spin_front : 1;
					unsigned char spin_back : 1;
					unsigned char curve_left : 1;
					unsigned char curve_right : 1;
					unsigned char _bit9_unknown : 1;
					unsigned char _bit10_unknown : 1;
					unsigned char _bit11_unknown : 1;
					unsigned char sem_setas : 1;
					unsigned char power_shot : 1;
					unsigned char double_power_shot : 1;
					unsigned char _bit15_unknown : 1;
					unsigned char _bit16_unknown : 1;
					unsigned char _bit17_unknown : 1;
					unsigned char _bit18_unknown : 1;
					unsigned char _bit19_unknown : 1;
					unsigned char _bit20_unknown : 1;
					unsigned char club_wood : 1;
					unsigned char club_iron : 1;
					unsigned char club_pw_sw : 1;
					unsigned char club_putt : 1;
					unsigned char _bit25_a_32_unknown : 8;
				}stShot;
			};
			uDisplayState display;
			uShotState shot;
			std::string toString() {
				
				std::string s = "Display State.\n\r";

				s += "OverDrive: " + std::to_string((unsigned short)display.stDisplay.over_drive) + " SuperPangya: " + std::to_string((unsigned short)display.stDisplay.super_pangya);
				s += " SpecialShot: " + std::to_string((unsigned short)display.stDisplay.special_shot) + " BeamImpact: " + std::to_string((unsigned short)display.stDisplay.beam_impact);
				s += " ChipIn17a199: " + std::to_string((unsigned short)display.stDisplay.chip_in_17_a_199) + " ChipIn200+: " + std::to_string((unsigned short)display.stDisplay.chip_in_200_plus);
				s += " LongPutt: " + std::to_string((unsigned short)display.stDisplay.long_putt) + " AcertoHole: " + std::to_string((unsigned short)display.stDisplay.acerto_hole);
				s += " ApproachShot: " + std::to_string((unsigned short)display.stDisplay.approach_shot) + " ChipInWithSpecialShot(BS,FS): " + std::to_string((unsigned short)display.stDisplay.chip_in_with_special_shot);
				s += " HappyBonus: " + std::to_string((unsigned short)display.stDisplay.happy_bonus) + " ClearBonus: " + std::to_string((unsigned short)display.stDisplay.clear_bonus) + " AztecBonus: " + std::to_string((unsigned short)display.stDisplay.aztec_bonus);
				s += " RecoveryBonus: " + std::to_string((unsigned short)display.stDisplay.recovery_bonus) + " ChipInWithoutSpecialShot: " + std::to_string((unsigned short)display.stDisplay.chip_in_without_special_shot);
				s += " BoundBonus: " + std::to_string((unsigned short)display.stDisplay.bound_bonus);
				s += " MascotBonusWithPangya: " + std::to_string((unsigned short)display.stDisplay.mascot_bonus_with_pangya) + " MascotBonusWithoutPangya: " + std::to_string((unsigned short)display.stDisplay.mascot_bonus_without_pangya);
				s += " SpecialBonusWithPangya: " + std::to_string((unsigned short)display.stDisplay.special_bonus_with_pangya);
				s += " SpecialBonusWithouPangya: " + std::to_string((unsigned short)display.stDisplay.special_bonus_without_pangya);
				s += " DevilBonus: " + std::to_string((unsigned short)display.stDisplay.devil_bonus) + STDA_END_LINE;

				s += "Shot State.\n\r";

				s += "Tomahawk: " + std::to_string((unsigned short)shot.stShot.tomahawk) + " Spike: " + std::to_string((unsigned short)shot.stShot.spike);
				s += " Cobra: " + std::to_string((unsigned short)shot.stShot.cobra) + " SpinFront: " + std::to_string((unsigned short)shot.stShot.spin_front);
				s += " SpinBack: " + std::to_string((unsigned short)shot.stShot.spin_back) + " CurveLeft: " + std::to_string((unsigned short)shot.stShot.curve_left);
				s += " CurveRight: " + std::to_string((unsigned short)shot.stShot.curve_right) + " SemSetas: " + std::to_string((unsigned short)shot.stShot.sem_setas);
				s += " PowerShot: " + std::to_string((unsigned short)shot.stShot.power_shot) + " DoublePowerShot: " + std::to_string((unsigned short)shot.stShot.double_power_shot);
				s += " ClubWood: " + std::to_string((unsigned short)shot.stShot.club_wood) + " ClubIron: " + std::to_string((unsigned short)shot.stShot.club_iron);
				s += " ClubPWeSW: " + std::to_string((unsigned short)shot.stShot.club_pw_sw) + " ClubPutt: " + std::to_string((unsigned short)shot.stShot.club_putt);
				
				return s;
			};
		};
		stStateShot state_shot;
		unsigned short tempo_shot;					// Acho que seja o tempo da tacada
		unsigned char grand_prix_penalidade;		// Flag(valor) de penalidade do Grand Prix quando tem regras com penalidades
	};

	struct ShotEndLocationData {
		void clear() { memset(this, 0, sizeof(ShotEndLocationData)); };
		struct stLocation {
			void clear() { memset(this, 0, sizeof(stLocation)); };
			float x;
			float y;
			float z;
			std::string toString() {
				return "X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z: " + std::to_string(z);
			};
		};
		struct BallPoint {
			void clear() { memset(this, 0, sizeof(BallPoint)); };
			std::string toString() {
				return "X: " + std::to_string(x) + " Y: " + std::to_string(y);
			};
			float x;
			float y;
		};
		float porcentagem;
		stLocation ball_velocity;
		unsigned char option;
		stLocation location;
		stLocation wind_influence;
		BallPoint ball_point;
		uSpecialShot special_shot;	// Tipo da tacada
		float ball_rotation_spin;
		float ball_rotation_curve;	// Esse é a quantidade do efeito final depois de todos os algorithmos do pangya
		unsigned char ucUnknown;
		unsigned char taco;			// Club
		float power_factor;
		float power_club;
		float rotation_spin_factor;
		float rotation_curve_factor;
		float power_factor_shot;
		uint32_t time_hole_sync;
		std::string toString() {
			return "Porcentagem: " + std::to_string(porcentagem) + STDA_END_LINE
				+ "Option: " + std::to_string((unsigned short)option) + STDA_END_LINE
				+ "Ball Velocity (Initial): " + ball_velocity.toString() + STDA_END_LINE
				+ "Location (Begin Shot): " + location.toString() + STDA_END_LINE
				+ "Wind Influence: " + wind_influence.toString() + STDA_END_LINE
				+ "Ball Point: " + ball_point.toString() + STDA_END_LINE
				+ "Special Shot(Tipo da tacada): " + special_shot.toString() + STDA_END_LINE
				+ "Ball Rotation (Spin): " + std::to_string(ball_rotation_spin) + STDA_END_LINE
				+ "Ball Rotation (Curva): " + std::to_string(ball_rotation_curve) + STDA_END_LINE
				+ "ucUnknown: " + std::to_string((unsigned short)ucUnknown) + STDA_END_LINE
				+ "Taco: " + std::to_string((unsigned short)taco) + STDA_END_LINE
				+ "Power Factor (Full): " + std::to_string(power_factor) + STDA_END_LINE
				+ "Power Club(Range): " + std::to_string(power_club) + STDA_END_LINE
				+ "Rotation Spin Factor: " + std::to_string(rotation_spin_factor) + STDA_END_LINE
				+ "Rotation Curve Factor: " + std::to_string(rotation_curve_factor) + STDA_END_LINE
				+ "Power Factor (Shot): " + std::to_string(power_factor_shot) + STDA_END_LINE
				+ "Time Hole SYNC: " + std::to_string(time_hole_sync) + STDA_END_LINE
				;
		};
	};

	struct DropItem {
		enum eTYPE : uint64_t {
			NONE,
			NORMAL_QNTD,
			QNTD_MULTIPLE_500,
			COIN_EDGE_GREEN,
			COIN_GROUND,
			CUBE,
		};
		void clear() {
			memset(this, 0, sizeof(DropItem));
		};
		uint32_t _typeid;
		unsigned char course;
		unsigned char numero_hole;
		short qntd;
		eTYPE type;
	};

	struct DropItemRet {
		DropItemRet(uint32_t _ul = 0u) {
			clear();
		};
		~DropItemRet() {};
		void clear() {
			
			if (!v_drop.empty()) {
				v_drop.clear();
				v_drop.shrink_to_fit();
			}
		};
		std::vector< DropItem > v_drop;
	};

	struct GameData {
		void clear() { memset(this, 0, sizeof(GameData)); };
		uint32_t tacada_num;
		uint32_t total_tacada_num;
		int32_t score;
		unsigned char giveup : 1;
		uint32_t bad_condute;			// Má conduta, 3 give ups o jogo kika o player
		uint32_t penalidade;			// Penalidade do Grand Prix Rule
		uint64_t pang;
		uint64_t bonus_pang;
		int64_t pang_pang_battle;			// Pang do Pang Battle que o player ganhou ou perdeu
		int32_t pang_battle_run_hole;			// Player saiu do pang battle(-1) ou alguém saiu(+1)
		uint32_t time_out;				// Count de time outs do player, 3 time outs o jogo kika o player
		uint32_t exp;					// Exp que o player, ganhou no jogo
	};

	struct BarSpace {
		BarSpace(uint32_t _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(BarSpace)); };
		bool setStateAndPoint(unsigned char _state, float _point) {

			if (_state > 4)
				return false;

			state = (_state == 4) ? 3 /*Impact Zone Update*/ : _state;

			// Tentou atualizar o State, mas os valores eram diferente, 
			// mas teria que ser o mesmo por que ele só está com lag, pedindo para mandar o pacote de initShot
			if (_state == 4 && point[state] != _point)
				return false;

			point[state] = _point;

			return true;
		};
		bool setState(unsigned char _state) {

			if (_state > 3)
				return false;

			state = _state;

			return true;
		};
		unsigned char getState() {
			return state;
		};
		float* getPoint() {
			return point;
		};
		std::string toString() {
			return "Point. Start: " + std::to_string(point[0]) + " Impact Zone: " + std::to_string(point[1]) + " Forca: " + std::to_string(point[2]) + " Hit PangYa: " + std::to_string(point[3]);
		};
	protected:
		unsigned char state;
		float point[4];							// 0 ainda não está tacando, 1 início, 2 força, 3 impact zone
	};

	struct UsedItem {
		~UsedItem() {};
		void clear() {

			rate.clear();
			club.clear();

			if (!v_passive.empty())
				v_passive.clear();

			if (!v_active.empty())
				v_active.clear();
		};
		struct Passive {
			void clear() { memset(this, 0, sizeof(Passive)); };
			uint32_t _typeid;
			uint32_t count;
		};
		struct Active {
			~Active() {};
			void clear() {

				_typeid = 0u;
				count = 0u;

				if (!v_slot.empty()) {
					v_slot.clear();
					v_slot.shrink_to_fit();
				}
			};
			uint32_t _typeid;
			uint32_t count;
			std::vector< unsigned char > v_slot;
		};
		struct Rate {
			void clear() { 
				// Default value
				pang = 100u;
				exp = 100u;
				club = 100u;
				drop = 100u;
			};
			uint32_t pang;
			uint32_t exp;
			uint32_t club;
			uint32_t drop;
		};
		struct ClubMastery {
			void clear() { memset(this, 0, sizeof(ClubMastery)); };
			uint32_t _typeid;
			uint32_t count;
			float rate;
		};
		std::map< uint32_t, Passive > v_passive;
		std::map< uint32_t, Active > v_active;
		Rate rate;
		ClubMastery club;
	};

	// Effect Item Flag
	union uEffectFlag {
		uEffectFlag(uint64_t _ull = 0ull) : ullFlag(_ull) {};
		void clear() { ullFlag = 0ull; };
		uint64_t ullFlag;
		struct {
			uint64_t NONE : 1;
			uint64_t PIXEL : 1;						// Pixel o valor em rate
			uint64_t PIXEL_BY_WIND_NO_ITEM : 1;		// Pixel dependendo do vento o valor em rate, se usar item ou ps cancela o efeito
			uint64_t PIXEL_OVER_WIND_NO_ITEM : 1;		// Pixel acima de um vento o valor em rate, se usar item ou ps cancela o efeito
			uint64_t PIXEL_BY_WIND : 1;				// Pixel dependendo do vento o valor em rate
			uint64_t PIXEL_2 : 1;						// Pixel o valor em rate
			uint64_t PIXEL_WITH_WEAK_WIND : 1;		// Pixel quando o vento é fraco o valor em rate
			uint64_t POWER_GAUGE_TO_START_HOLE : 1;	// Power Gauge no começo do hole para cada hole o valor em rate
			uint64_t POWER_GAUGE_MORE_ONE : 1;		// Power Gauge da uma barra a+ 33 Units, o valor em rate
			uint64_t POWER_GUAGE_TO_START_GAME : 1;	// Power Gauge no começo do jogo o valor em rate
			uint64_t PAWS_NOT_ACCUMULATE : 1;			// Patinha não acumula com outro efeito de patinha, probabilidade está em rate
			uint64_t SWITCH_TWO_EFFECT : 1;			// Item com 2 efeitos não simutâneos, qual efeito está em rate, 0 Yards, 1 Power Gauge
			uint64_t EARCUFF_DIRECTION_WIND : 1;		// Muda a direção do vento, a probabilidade quem escolhe é o pangya
			uint64_t COMBINE_ITEM_EFFECT : 1;			// Combinação de itens, em rate tem o ID da combinação em (IFF)SetEffectTable
			uint64_t SAFETY_CLIENT_RANDOM : 1;		// Safety a probabilidade o cliente que decide
			uint64_t PIXEL_RANDOM : 1;				// Pixel aleatório o valor está em rate, a probabilidade o cliente que decide
			uint64_t WIND_1M_RANDOM : 1;				// Wind 1m aleatório a probabilidade está em rate
			uint64_t PIXEL_BY_WIND_MIDDLE_DOUBLE : 1;	// Pixel dependendo do vento, vento médio dá o dobro, o valor em rate
			uint64_t GROUND_100_PERCENT_RONDOM : 1;	// Terreno 100% aleatório, a probabilidade está em rate
			uint64_t ASSIST_MIRACLE_SIGN : 1;			// Assist Olho Mágico
			uint64_t VECTOR_SIGN : 1;					// Mostra uma seta na bola, dependendo do vento, tipo trajetória do assist
			uint64_t ASSIST_TRAJECTORY_SHOT : 1;		// Assist Trajectory Shot
			uint64_t PAWS_ACCUMULATE : 1;				// Patinha acumula com outro efeito de patinha, a probabilidade está em rate
			uint64_t POWER_GAUGE_FREE : 1;			// Power Gauge, ganha 1 Power Gauge de graça para usar na tacada
			uint64_t SAFETY_RANDOM : 1;				// Safety aleatório a probabilidade está em rate
			uint64_t ONE_IN_ALL_STATS : 1;			// [UNKNOWN] mas vou deixar o (Combine Itens) ONE IN ALL STATS, dá 1 para todos os stats, power, cltr, accuracy, spin e curve
			uint64_t POWER_GAUGE_BY_MISS_SHOT : 1;	// Power Gauge mesmo que erre pangya ou use item de Power Gauge ele ainda dá Power Gauge
			uint64_t PIXEL_BY_WIND_2 : 1;				// Pixel dependendo do vento o valor está em rate
			uint64_t PIXEL_WITH_RAIN : 1;				// Pixel quando estiver chovendo(recovery) o valor está em rate
			uint64_t NO_RAIN_EFFECT : 1;				// Sem efeito dá chuva no terreno
			uint64_t PUTT_MORE_10Y_RANDOM : 1;		// +10y no Putt aleatório a probabilidade está em rate
			uint64_t UNKNOWN_31 : 1;
			uint64_t MIRACLE_SIGN_RANDOM : 1;			// Olho Mágico aleatório a probabilidade está em rate
			uint64_t UNKNOWN_33 : 1;
			uint64_t DECREASE_1M_OF_WIND : 1, : 0;	// Diminui 1m do vento
		}stFlag;
	};

	// Bit value
	template<typename _ENUM_TYPE, typename _RET> _RET enumToBitValue(_ENUM_TYPE _enum) {
		return _RET(1) << _RET(_enum);
	};

	struct PlayerGameInfo {
		enum eCARD_WIND_FLAG : unsigned char {
			NONE,					// Nenhum Efeito
			NORMAL,					// Normal, diminui 1m do vento, quando é 9m
			RARE,					// Rare, diminui 1m de todos os ventos exceto menos 1m
			SUPER_RARE,				// Super Rare, diminui 2m do vento, quando é 6m a 9m
			SECRET,					// Secret, diminui 1m do vento, quando é 2m a 5m e diminui 2m do vento, quando é 6m a 9m
		};
		enum eFLAG_GAME : unsigned char {
			PLAYING,				// Ainda esta jogando
			TICKET_REPORT,			// Saiu com ticket report, "Terminou o jogo"
			FINISH,					// Jogador terminou o jogo
			BOT,					// É Bot do Grand Prix
			QUIT,					// Saiu do jogo
			END_GAME,				// Terminou o jogo, antes do jogar acabar
		};
		enum eTEAM : unsigned char {
			T_RED,
			T_BLUE,
			T_NONE,
		};
		PlayerGameInfo(uint32_t _ul = 0u) {
			clear();
		};
		virtual ~PlayerGameInfo() {
			clear();
		};
		void clear() {
			
			uid = 0u;
			oid = 0u;
			level = 0u;
			finish_load_hole = 0u;
			finish_char_intro = 0u;
			init_shot = 0u;
			finish_shot = 0u;
			finish_shot2 = 0u;
			sync_shot_flag = 0u;
			sync_shot_flag2 = 0u;
			finish_hole = 0u;
			finish_hole2 = 0u;
			finish_hole3 = 0u;
			finish_game = 0u;
			finish_item_used = 0u;
			premium_flag = 0u;
			trofel = 0u;
			char_motion_item = 0u;
			assist_flag = 0u;
			enter_after_started = 0u;
			progress_bar = 0u;
			tempo = 0u;
			power_shot = 0u;
			club = 0u;
			chat_block = 0u;
			degree = 0u;
			mascot_typeid = 0u;

			init_first_hole = 0u;

			tick_sync_shot.clear();
			tick_sync_end_shot.clear();

			card_wind_flag = eCARD_WIND_FLAG::NONE;
			flag = eFLAG_GAME::PLAYING;
			team = eTEAM::T_NONE;					// Valor Padrão

			effect_flag_shot.clear();
			item_active_used_shot = 0u;
			earcuff_wind_angle_shot = 0.f;

			memset(&time_finish, 0, sizeof(SYSTEMTIME));

			boost_item_flag.clear();

			thi.clear();
			bar_space.clear();
			location.clear();
			data.clear();
			shot_data.clear();
			shot_data_for_cube.clear();
			shot_sync.clear();
			ui.clear();
			drop_list.clear();
			used_item.clear();
			progress.clear();
			medal_win.clear();

			typeing = -1;
			hole = -1;
		};
		struct stProgress {
			void clear() {
				memset(this, 0, sizeof(stProgress));

				hole = -1;
			};
			bool isGoodScore() {
				
				for (auto i = 0u; i < 18u; ++i)
					if (score[i] > 0)
						return false;

				return true;
			};
			int32_t getBestRecovery() {

				int32_t first = 0, last = 0;
				uint32_t i = 0u;

				for (i = 0u; i < 9u; ++i)
					first += score[i];

				for (i = 9u; i < 18u; ++i)
					last += score[i];

				return (first * -1) - last;	// Pronto agora ele reflete o quanto que o player recuperou-se
			};
			short hole;						// Hole Atual
			float best_chipin;
			float best_long_puttin;
			float best_drive;
			unsigned char finish_hole[18];	// Flag para verificar se o player terminou o hole
			unsigned char par_hole[18];		// Par do hole, [18 Holes o máximo de um jogo]
			uint32_t tacada[18];		// Tacadas do hole, [18 Holes o máximo de um jogo]
			char score[18];					// Score do hole, [18 Holes o máximo de um jogo]
		};
		struct stTreasureHunterInfo {
			stTreasureHunterInfo(uint32_t _ul = 0u) {
				clear();
			};
			~stTreasureHunterInfo() {};
			void clear() {

				all_score = 0u;
				par_score = 0u;
				birdie_score = 0u;
				eagle_score = 0u;

				treasure_point = 0u;
				
				if (!v_item.empty()) {
					v_item.clear();
					v_item.shrink_to_fit();
				}
			};
			uint32_t getPoint(uint32_t _tacada, unsigned char _par_hole) {
				unsigned char point = all_score;

				if (_tacada == 1)	// HIO
					return point;

				unsigned char score = (unsigned char)(_tacada - _par_hole);

				switch (score) {
				case 0:		// Par
					point += par_score;
					break;
				case (unsigned char)-1:	// Birdie
					point += birdie_score;
					break;
				case (unsigned char)-2:	// Eagle
					point += eagle_score;
					break;
				}

				return point;
			};
			stTreasureHunterInfo& operator+=(stTreasureHunterInfo& _thi) {

				all_score += _thi.all_score;
				par_score += _thi.par_score;
				birdie_score += _thi.birdie_score;
				eagle_score += _thi.eagle_score;

				return *this;
			};
			uint32_t treasure_point;				// Treasure Hunter point do player no game
			std::vector< TreasureHunterItem > v_item;	// Treasure Hunter Item
		public:
			unsigned char all_score;
			unsigned char par_score;
			unsigned char birdie_score;
			unsigned char eagle_score;
		};
		struct TickTimeSync {
			TickTimeSync(uint32_t _ul = 0u) {
				clear();
			};
			void clear() {
				memset(this, 0, sizeof(TickTimeSync));
			};
			unsigned char count;
			unsigned char active : 1;
#if defined(_WIN32)
			LARGE_INTEGER tick;
#elif defined(__linux__)
			timespec tick;
#endif
		};
		union uBoostItemFlag {
			void clear() { memset(this, 0, sizeof(uBoostItemFlag)); };
			unsigned char ucFlag;
			struct stFlag {
				unsigned char pang : 1;
				unsigned char pang_nitro : 1;
				unsigned char exp : 1, : 0;	// não sei esse aqui é o de experiência mesmo, por que só fiz o de pang no outro
			}flag;
		};
		uint32_t uid;
		uint32_t oid;
		unsigned char level;
		char		  hole;						// Número do Hole que o player está
		unsigned char init_first_hole : 1;		// Flag que guarda quando o player inicializou o primeiro hole do jogo
		unsigned char finish_load_hole : 1;
		unsigned char finish_char_intro : 1;
		unsigned char init_shot : 1;
		unsigned char finish_shot : 1;
		unsigned char finish_shot2 : 1;
		unsigned char finish_hole : 1;			// Usa no Grand Prix, flag de sincronização de hole conluído para trocar para o prox
		unsigned char finish_hole2 : 1;			// Usa no Grand Prix, flag de sincronização do tempo do hole do player, para não dá time out depois que ele concluiu o hole
		unsigned char finish_hole3 : 1;			// Usa no Grand Prix, flag de sincronização se o player já enviou o pacote de finalizar o hole antes
		unsigned char sync_shot_flag : 1;
		unsigned char sync_shot_flag2 : 1;
		unsigned char finish_game : 1;			// Terminou o jogo
		unsigned char assist_flag : 1;			// 0 não está com assist ligado, 1 está com assist ligado
		unsigned char char_motion_item : 1;		// Está com intro de character Equipado
		unsigned char premium_flag : 1;			// 1 Player é um usuário premium, 0 player normal
		unsigned char enter_after_started : 1;	// Entrou no Jogo depois de ele ter começado
		unsigned char finish_item_used : 1;		// 1 Player já finalizou os itens usados no jogo, não finalizar de novo se ele já estiver finalizado
		unsigned char trofel;					// Trofel que ele ganhou, 1 ouro, 2 prate, 3 bronze
		unsigned short progress_bar;
		uint32_t tempo;
		unsigned char power_shot;
		unsigned char club;						// Taco
		short		  typeing;					// Escrevendo
		unsigned char chat_block;				// Chat Block
		unsigned short degree;					// Degree(Graus) do player no Hole
		uint32_t mascot_typeid;			// Typeid do Mascot equipado
		uint32_t item_active_used_shot;	// O item Active usado na tacada
		float		  earcuff_wind_angle_shot;	// Ângulo que o efeito earcuff ativou na tacada para o player
		uEffectFlag effect_flag_shot;			// Effect Flag Shot(tacada), Wind 1m, Safety, Patinha e etc
		eFLAG_GAME flag;						// Flag se acabou o camp, ainda esta jogando, quitou, saiu, ou o jogo terminou pro ele
		uBoostItemFlag boost_item_flag;			// Flag que Exibe os icon de quais boost item o player está usando
		eCARD_WIND_FLAG card_wind_flag;			// Card Wind Flag
		stTreasureHunterInfo thi;				// Treasure Hunter Info do player, esse é que aumenta com card
		eTEAM team;								// Team(time) que o player está, antes usado no tourney de time, agora só usado no Match
		TickTimeSync tick_sync_shot;			// Tick de quando o player recebeu o pacote para ele enviar o pacote sync shot
		TickTimeSync tick_sync_end_shot;		// Tick de quando o player enviou o pacote de termino de tacada (FinishShot)
		BarSpace bar_space;
		Location location;
		GameData data;
		ShotDataEx shot_data;
		ShotEndLocationData shot_data_for_cube;	// Dados que vou usar para os locais de spaw do Spinning Cube
		ShotSyncData shot_sync;
		UserInfoEx ui;
		DropItemRet drop_list;					// Drop List do player
		UsedItem used_item;						// Item usado no jogo
		stProgress progress;					// Progresso do jogo, tacadas e score
		SYSTEMTIME time_finish;					// Tempo que acabou o game
		uMedalWin medal_win;					// Medal que Ganhou no Jogo
		SysAchievement sys_achieve;				// System of Achievement of Player
	};

	// Ticket Report Info
	struct TicketReportInfo {
		TicketReportInfo(uint32_t _ul = 0u) {
			clear();
		};
		~TicketReportInfo() {};
		void clear() {
			id = -1;
			v_dados.clear();
		};
		struct stTicketReportDados {
			void clear() { memset(this, 0, sizeof(stTicketReportDados)); };
			uint32_t uid;
			int32_t score;
			uMedalWin medal;
			unsigned char trofel;
			uint64_t pang;
			uint64_t bonus_pang;
			uint32_t exp;
			uint32_t mascot_typeid;
			uint32_t flag_item_pang;
			uint32_t premium;
			uint32_t state;
			SYSTEMTIME finish_time;
		};
		int32_t id;
		std::vector< stTicketReportDados > v_dados;
	};

	// Enter After Start Info
	struct EnterAfterStartInfo {
		void clear() { memset(this, 0, sizeof(EnterAfterStartInfo)); };
		unsigned char		tacada[18];	// 18 Holes
		int32_t				score[18];	// 18 Holes
		uint64_t	pang[18];	// 18 Holes
		uint32_t		request_oid;
		uint32_t		owner_oid;
	};

	// Player Order Turn Context
	class Hole;

	struct PlayerOrderTurnCtx {
		PlayerOrderTurnCtx(uint32_t _ul = 0u) {
			clear();
		};
		PlayerOrderTurnCtx(PlayerGameInfo* _pgi, Hole *_hole) 
			: pgi(_pgi), hole(_hole) {
		}
		void clear() {
			pgi = nullptr;
			hole = nullptr;
		};
		PlayerGameInfo *pgi;
		Hole *hole;
	};

	// Table Rate Voice And Effect On Versus
	struct TableRateVoiceAndEffect {
		enum eTYPE : unsigned char {
			NONE,
			W_BIGBONGDARI,
			R_BIGBONGDARI,
			VOICE_CLUB,
		};
		TableRateVoiceAndEffect(uint32_t _ul = 0u) {
			clear();
		};
		TableRateVoiceAndEffect(std::string _name, eTYPE _type) : name(_name), type(_type) {
			randomTable();
		};
		~TableRateVoiceAndEffect() {};
		void clear() {

			if (!name.empty()) {
				name.clear();
				name.shrink_to_fit();
			}

			type = eTYPE::NONE;

			memset(table, 0, sizeof(table));
		};
		void randomTable() {

			//std::srand(std::clock() * (uint32_t)this * (uint32_t)GetCurrentThreadId());

			unsigned short min_value = 0u;

			if (type == eTYPE::VOICE_CLUB)
				min_value = 1u;

			for (auto i = 0u; i < (sizeof(table) / sizeof(table[0])); ++i)
				table[i] = min_value + ((unsigned char)sRandomGen::getInstance().rIbeMt19937_64_chrono() % (4 - min_value));
		};
		std::string name;
		eTYPE type;
		unsigned char table[100];
	};

	struct TreasureHunterVersusInfo {
		TreasureHunterVersusInfo(uint32_t _ul = 0u) {
			clear();
		};
		~TreasureHunterVersusInfo() {};
		void clear() {

			all_score = 0u;
			par_score = 0u;
			birdie_score = 0u;
			eagle_score = 0u;

			treasure_point = 0u;

			if (!v_item.empty()) {
				v_item.clear();
				v_item.shrink_to_fit();
			}
		};
		uint32_t getPoint(uint32_t _tacada, unsigned char _par_hole) {
			unsigned char point = all_score;

			if (_tacada == 1)	// HIO
				return point;

			unsigned char score = (unsigned char)(_tacada - _par_hole);

			switch (score) {
			case 0:		// Par
				point += par_score;
				break;
			case (unsigned char)-1:	// Birdie
				point += birdie_score;
				break;
			case (unsigned char)-2:	// Eagle
				point += eagle_score;
				break;
			}

			return point;
		};
		TreasureHunterVersusInfo& operator+=(TreasureHunterVersusInfo& _thi) {

			all_score += _thi.all_score;
			par_score += _thi.par_score;
			birdie_score += _thi.birdie_score;
			eagle_score += _thi.eagle_score;

			return *this;
		};
		TreasureHunterVersusInfo& operator+=(PlayerGameInfo::stTreasureHunterInfo& _thi) {

			all_score += _thi.all_score;
			par_score += _thi.par_score;
			birdie_score += _thi.birdie_score;
			eagle_score += _thi.eagle_score;

			return *this;
		};
		struct _stTreasureHunterItem {
			_stTreasureHunterItem(uint32_t _ul = 0u) {
				clear();
			};
			_stTreasureHunterItem(uint32_t _uid, TreasureHunterItem& _thi)
				: uid(_uid), thi(_thi) {
			};
			void clear() {
				uid = 0u;
				thi.clear();
			};
			uint32_t uid;	// Player UID
			TreasureHunterItem thi;
		};
		uint32_t treasure_point;				// Treasure Hunter point do player no game
		std::vector< _stTreasureHunterItem > v_item;	// Treasure Hunter Item
	public:
		unsigned char all_score;
		unsigned char par_score;
		unsigned char birdie_score;
		unsigned char eagle_score;
	};

	// Ret Finish Shot
	struct RetFinishShot {
		RetFinishShot(uint32_t _ul = 0u) { clear(); };
		void clear() { memset(this, 0, sizeof(RetFinishShot)); };
		int ret;
		player *p;
	};

	// Holes rain count
	struct HolesRain {
		HolesRain(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(HolesRain));
		};
		unsigned char getCountHolesRainBySeq(uint32_t _seq) {

			// Sequência de hole valor errado
			if (_seq < 1 || _seq > 18)
				return 0u;

			return std::accumulate(rain, N_ELEMENT_IN_ARRAY(rain, _seq), (unsigned char)0u);
		};
		unsigned char getCountHolesRain() {
			return std::accumulate(rain, LAST_ELEMENT_IN_ARRAY(rain), (unsigned char)0u);
		};
		void setRain(uint32_t _index, unsigned char _value) {

			// Index invalido
			if ((int)_index < 0 || _index >= 18)
				return;

			rain[_index] = _value;
		};
	protected:
		unsigned char rain[18];	// Máximo número de holes de um jogo
	};

	// Consecutivos Holes Rain(Recovery) Tempo Ruim
	struct ConsecutivosHolesRain {
		ConsecutivosHolesRain(uint32_t _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(ConsecutivosHolesRain)); };
		bool isValid() {
			return (_4_pluss_count.getCountHolesRain() > 0u || _3_count.getCountHolesRain() > 0u || _2_count.getCountHolesRain() > 0u);
		};
		HolesRain _4_pluss_count;
		HolesRain _3_count;
		HolesRain _2_count;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_GAME_TYPE_HPP
