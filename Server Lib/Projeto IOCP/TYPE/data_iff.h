// Arquivo data_iff.h
// Criado em 03/10/2017 por Acrisio
// Definição das estruturas do iff pangya

#if defined(_WIN32)
#pragma pack(1)
#endif

#pragma once
#ifndef _STDA_IFF_DATA_IFF_H
#define _STDA_IFF_DATA_IFF_H

#include <memory.h>
#include <string>
#include "../UTIL/hex_util.h"
#include "../UTIL/util_time.h"
#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include <algorithm>
#include <numeric>

#define EMPTY_ARRAY_PRICE(_price) (bool)!std::count_if(_price, (_price + (sizeof(_price) / sizeof(_price[0]))), [](auto& el) { return el != 0; })
#define SUM_ARRAY_PRICE_ULONG(_price) (unsigned int)std::accumulate(_price, (_price + (sizeof(_price) / sizeof(_price[0]))), 0u)

// Aqui para não fazer o pack em outras bibliotecas
#if defined(__linux__)
#pragma pack(1)
#endif

namespace stdA {
	namespace IFF {
		// Head IFF
		struct Head {
			void clear() {
				memset(this, 0, sizeof(Head));
			};
			unsigned short count_element;       // Number of Objects on file
			unsigned short flag_ligacao;        // Flag de ligação com outros IFF
			unsigned int  version;				// Versão do IFF
		};

		// Shop Dados
		struct ShopDados {
			void clear() {
				memset(this, 0, sizeof(ShopDados));
			};
			unsigned int  price;
			unsigned int  sale_price;			// By TH S4 - (Sale Price) Desconto (price - sale_price) = Desconto
			unsigned int  sell_price;			// By TH S4 - (Used Price) Preço de revenda do item, porém foi feito no beta do season 1, e depois desativado
			struct FlagShop {
				struct _8bits {
					unsigned char bit0 : 1;
					unsigned char bit1 : 1;
					unsigned char bit2 : 1;
					unsigned char bit3 : 1;
					unsigned char bit4 : 1;
					unsigned char bit5 : 1;
					unsigned char bit6 : 1;
					unsigned char bit7 : 1;
				};
				union {
					unsigned short us_flag_shop;
					unsigned char uc_bytes[2];
					struct _stFlagShop {
						unsigned char is_cash : 1;
						unsigned char can_send_mail_and_personal_shop : 1;
						unsigned char can_dup : 1;
						unsigned char unknown : 1;
						unsigned char block_mail_and_personal_shop : 1;
						unsigned char is_saleable : 1;	// Pang(Only Purchase, CP Gift and Purchase) (is_saleable e is_giftable vira flag de só Purchase CP ou Pang)
						unsigned char is_giftable : 1;	// CP só pode ser presenteado (is_saleable e is_giftable vira flag de só Purchase CP ou Pang)
						unsigned char only_display : 1;	// Apenas Display no shop
					} stFlagShop;
					struct _stIconShop {
						unsigned short : 8;	// Flag Shop é a estrutura de cima, tenho que fazer isso por que está dentro de uma union
						unsigned short is_new : 1;
						unsigned short is_hot : 1;
						unsigned short unknown_bit2 : 1;
						unsigned short unknown_bit3 : 1;
						unsigned short unknown_bit4 : 1;
						unsigned short unknown_bit5 : 1;
						unsigned short unknown_bit6 : 1;
						unsigned short unknown_bit7 : 1;
					} stIconShop;
					//_8bits _byte[2];
					// Byte 1
/*#define IFF_cash			_byte[0].bit0	// 1 point 0 pang
#define IFF_personal_shop	_byte[0].bit1	// aqui é flag de que pode vender no personal shop e enviar por email
#define IFF_dup_normal		_byte[0].bit2	// aqui é flag de dup, acho
#define IFF_gift			_byte[0].bit3	// Não sei
#define IFF_time			_byte[0].bit4	// Esse aqui Bloquea o Item de mandar no personal shop e enviar no mail
#define IFF_shop_sale		_byte[0].bit5	// purchase e gift (essa flag com o bit6, fica purchase somente)
#define IFF_condition		_byte[0].bit6	// gift somente (essa flag com o bit5, fica purchase somente)
#define IFF_item_pack		_byte[0].bit7	// ver(Display) somente // (ACHO) Esse bit só vi ele ativo nos 3 coupon gacha, item pack é um item normal que funciona como pack(setItem), ai tem que ver qual é o single do item para trocar o typeid
					// Byte 2
#define IFF_novo			_byte[1].bit1
#define IFF_hot				_byte[1].bit2*/
				} uFlagShop;
				struct TimeShop {
					/*union {
						unsigned char uc_byte;
						FlagShop::_8bits _bits;
						// Byte 1
#define IFF_time_limit		_bits.bit0
					} uTimeShop;*/
					unsigned char active : 1;	// Item por tempo
					unsigned char dia;			// Tempo em Dias, 1, 7, 15, 30 e 365 && 0xFF fica 0x6D, por que é 0x16D = 365
				} time_shop;
			};
			FlagShop flag_shop;
		};

		// Tiki Shop Dados
		struct TikiShopDados {
			void clear() {
				memset(this, 0, sizeof(TikiShopDados));
			};
			bool isActived() {
				return (tipo_tiki_shop == 1u || tipo_tiki_shop == 2u || tipo_tiki_shop == 3u) && tiki_pang > 0u && milage_pts > 0u;
			};
			unsigned int  qnt_per_tikis_pts;
			unsigned int  tiki_pts;
			unsigned short milage_pts;
			unsigned short bonus_prob;
			unsigned short bonus[2];            // bonus[0] min_bonus, bonus[1] max_bonus
			unsigned int  tipo_tiki_shop;
			unsigned int  tiki_pang;
		};

		// Date Dados
		struct DateDados {
			void clear() {
				memset(this, 0, sizeof(DateDados));
			};
			unsigned int  active_date : 1, : 0;	// 1 Actived, 0 Desatived
			SYSTEMTIME date[2];					// Date[0] Start, Date[1] End
		};

		// Base IFF
		struct Base {
			void clear() {
				memset(this, 0, sizeof(Base));
			};
			unsigned int  active;
			unsigned int  _typeid;
			char name[64];
			struct stLevel {
				void clear() { memset(this, 0, sizeof(stLevel)); };
				bool goodLevel(unsigned char _level) {
					if (is_max && _level <= level)
						return true;
					else if (!is_max && _level >= level)
						return true;

					return false;
				};
				unsigned char level : 7;
				unsigned char is_max : 1;
			};
			stLevel level;			// é signed aqui
			char icon[43];
			ShopDados shop;
			TikiShopDados tiki;
			DateDados date;
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid)
					 + "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					 + "\r\nName: " + name + "\r\n";
#else
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")\r\n";
#endif // _DEBUG
			};
		};

		// Item IFF
		struct Item : public Base {
			void clear() {
				memset(this, 0, sizeof(Item));
			};
			unsigned int  tipo_item;
			char mpet[40];
			short c[5];			// esse aqui, quando o item é por tempo, aqui é o preço dos dias, 1, 7, 15, 30 e 365 dias
			short point;		// By TH S4 - (Point) Aqui é o No Limit time preço, mas quando compra ele passa o preço de -1, mas no visual ele mostra o valor que está aqui
		};

		// Achievment IFF
		struct Achievement : public Base {
			void clear() {
				memset(this, 0, sizeof(Achievement));
			};
			unsigned int  typeid_quest_index;
			unsigned int  achievement_tipo;
			char quest_name[10][129];
			unsigned short s_unknown;
			unsigned int  quest_typeid[10];
			unsigned int  l_unknown;
		};

		// QuestStuff IFF
		struct QuestStuff : public Base {
			void clear() {
				memset(this, 0, sizeof(QuestStuff));
			};
			struct CounterItem {
				unsigned int  _typeid[5];
				int qntd[5];
			};
			struct RewardItem {
				unsigned int  _typeid[3];
				unsigned int  qntd[3];
				unsigned int  time[3];	// !@[ACHO] que é isso aqui também
			};
			CounterItem counter_item;
			RewardItem reward_item;
			//unsigned char uc_unknown12[12];
		};

		// QuestItem IFF
		struct QuestItem : public Base {
			void clear() {
				memset(this, 0, sizeof(QuestItem));
			};
			unsigned int  ulUnknown;
			unsigned int  type;
			struct Quest {
				unsigned int  qntd;
				unsigned int  _typeid[10];
			};
			struct Reward {
				unsigned int  _typeid[2];
				unsigned int  qntd[2];
				unsigned int  time[2];		// !@[ACHO] que é isso aqui também
			};
			Quest quest;
			Reward reward;
			unsigned int  ulUnknown2;
		};

		// CounterItem IFF
		struct CounterItem : Base {
			void clear() {
				memset(this, 0, sizeof(CounterItem));
			};
			unsigned char ucUnknown[88];
		};

		// Part IFF
		struct Part : public Base {
			// By S4 TH - (Category)
			enum PART_TYPE : unsigned int  {
				TOP,				// By TH S4 - (UPPER)
				BOTTOM,				// By TH S4 - (LOWER)
				HEAD,				// By TH S4 - (HEAD)
				ARM,				// By TH S4 - (ARM == braço) CLOVE
				FOOT,				// By TH S4 - (FOOT == pé) SHOE
				ETC,				// By TH S4 - (ETC) ACESSORY_OR_BASE
				HAIR,				// By TH S4 - (HAIR) SUB_LEG
				UCC,				// By TH S4 - (UCC)
				UCC_BLANK,			// By TH S4 - (UCC_DRAW_ONLY)
				UCC_COPY,			// By TH S4 - (UCC_COPY_ONLY)
			};
			void clear() {
				memset(this, 0, sizeof(Part));
			};
			char mpet[40];
			PART_TYPE type_item;				// By S4 TH - (Category) o tipo do item, 0, 2 normal, 8 e 9 UCC, 5 acho que é base ou commom Item
			union u_part_type {
				unsigned int  ul_part_type;
				struct {
					unsigned char slot0 : 1;
					unsigned char slot1 : 1;
					unsigned char slot2 : 1;
					unsigned char slot3 : 1;
					unsigned char slot4 : 1;
					unsigned char slot5 : 1;
					unsigned char slot6 : 1;
					unsigned char slot7 : 1;
					unsigned char slot8 : 1;
					unsigned char slot9 : 1;
					unsigned char slot10 : 1;
					unsigned char slot11 : 1;	// Wings Slot
					unsigned char slot12 : 1;
					unsigned char slot13 : 1;
					unsigned char slot14 : 1;
					unsigned char slot15 : 1;
					unsigned char slot16 : 1;
					unsigned char slot17 : 1;
					unsigned char slot18 : 1;
					unsigned char slot19 : 1;
					unsigned char slot20 : 1;
					unsigned char slot21 : 1;
					unsigned char slot22 : 1;
					unsigned char slot23 : 1;
				} _slots;
				char getSlot(unsigned int  _i) {
					switch (_i) {
					case 0:
						return (char)_slots.slot0;
					case 1:
						return (char)_slots.slot1;
					case 2:
						return (char)_slots.slot2;
					case 3:
						return (char)_slots.slot3;
					case 4:
						return (char)_slots.slot4;
					case 5:
						return (char)_slots.slot5;
					case 6:
						return (char)_slots.slot6;
					case 7:
						return (char)_slots.slot7;
					case 8:
						return (char)_slots.slot8;
					case 9:
						return (char)_slots.slot9;
					case 10:
						return (char)_slots.slot10;
					case 11:
						return (char)_slots.slot11;
					case 12:
						return (char)_slots.slot12;
					case 13:
						return (char)_slots.slot13;
					case 14:
						return (char)_slots.slot14;
					case 15:
						return (char)_slots.slot15;
					case 16:
						return (char)_slots.slot16;
					case 17:
						return (char)_slots.slot17;
					case 18:
						return (char)_slots.slot18;
					case 19:
						return (char)_slots.slot19;
					case 20:
						return (char)_slots.slot20;
					case 21:
						return (char)_slots.slot21;
					case 22:
						return (char)_slots.slot22;
					case 23:
						return (char)_slots.slot23;
					default:
						return -1;
					}
				};
			};
			u_part_type position_mask;				// By S4 TH - (PosMask) Position Mask
			u_part_type hide_mask;					// By S4 TH - (HideMask) Esconde os outros parts do slots
			char texture[3][40];					// 3 Textura
			char texture_org[3][40];				// By S4 TH - (OrgTex) 3 Org Textura(Original)
			unsigned short c[5];					// C = Stats[Control, Power, Spin e etc]
			unsigned short slot[5];					// O mesmo que C, só que esse é slot
			unsigned char equippable_with[40];		// By TH S4
			unsigned int sub_part[2];				// By TH S4
			unsigned short character_slot;			// By TH S4
			unsigned short flag_caddie_card_slot;	// By TH S4 - (caddie_slot) Terceiro Caddie Card Slot
			unsigned short npc_slot;
			unsigned short point;
			unsigned int  valor_rental;
			unsigned int  ul_unknown3;
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nType: " + std::to_string(type_item)
					+ "\r\nPosition Slot	1	2	3	4	5	6	7	8	9	10	11	12"
					+ "\r\n		"	+ std::to_string((unsigned short)position_mask._slots.slot0) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot1) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot2) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot3) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot4) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot5) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot6) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot7) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot8) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot9) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot10) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot11) + "	"
					+ "\r\nPosition Slot	13	14	15	16	17	18	16	20	21	22	23	24"
					+ "\r\n		"	+ std::to_string((unsigned short)position_mask._slots.slot12) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot13) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot14) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot15) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot16) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot17) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot18) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot19) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot20) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot21) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot22) + "	"
								+ std::to_string((unsigned short)position_mask._slots.slot23) + "	"
					+ "\r\nHide Slot	1	2	3	4	5	6	7	8	9	10	11	12"
					+ "\r\n		"	+ std::to_string((unsigned short)hide_mask._slots.slot0) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot1) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot2) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot3) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot4) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot5) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot6) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot7) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot8) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot9) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot10) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot11) + "	"
					+ "\r\nHide Slot	13	14	15	16	17	18	16	20	21	22	23	24"
					+ "\r\n		"	+ std::to_string((unsigned short)hide_mask._slots.slot12) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot13) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot14) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot15) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot16) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot17) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot18) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot19) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot20) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot21) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot22) + "	"
								+ std::to_string((unsigned short)hide_mask._slots.slot23) + "	"
					+ "\r\nC:	" + std::to_string(c[0]) + "	" + std::to_string(c[1]) + "	"
							+ std::to_string(c[2]) + "	" + std::to_string(c[3]) + "	" + std::to_string(c[4])
					+ "\r\nSlot:	" + std::to_string(slot[0]) + "	" + std::to_string(slot[1]) + "	"
							+ std::to_string(slot[2]) + "	" + std::to_string(slot[3]) + "	" + std::to_string(slot[4]) 
					+ "\r\nSell Shop: " + std::to_string(shop.sell_price)
					+ "\r\nFlag Shop	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16"
					+ "\r\n			" + std::to_string((unsigned short)shop.flag_shop.uFlagShop.stFlagShop.is_cash) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stFlagShop.can_dup) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stFlagShop.unknown) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stFlagShop.block_mail_and_personal_shop) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stFlagShop.is_saleable) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stFlagShop.is_giftable) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stFlagShop.only_display) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stIconShop.is_new) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stIconShop.is_hot) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stIconShop.unknown_bit2) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stIconShop.unknown_bit3) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stIconShop.unknown_bit4) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stIconShop.unknown_bit5) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stIconShop.unknown_bit6) + "	"
										+ std::to_string((unsigned short)shop.flag_shop.uFlagShop.stIconShop.unknown_bit7)
					+ "\r\n";
#else
				return "Typeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")\r\n";
#endif
			};
		};

		// SetItem IFF
		/*Typeids
		*
		* Ex: 0x2420 20 01 (2420 "NÃO EXIBE MAIS NO SHOP") (20 "HANA") (01 "ID")
		* 0x(2420~243F)(char_id)(item_id) Character Set "EXIBE NO SHOP"
		* 0x(2440~245F)(char_id)(item_id) Character Set "EXIBE NO SHOP"
		* 0x(24C0~24DE)(char_id)(item_id) Character Set "NÃO EXIBE NO SHOP"
		*
		* 0x(2480~249F) ClubSet
		* 0x(24A0~24BF) Ball
		* 0x(24DFC0~24DFDF) Item Active
		* 0x(24DFE0~24DFFF) Item Passive
		*
		* 0x(24DF00~24DFBF) Caddie
		* 0x(24E0~24FF) Caddie
		*
		* 0x(2500~251F) Card e Card Pack
		* 0x(2520~253F) Anel
		*
		*/
		struct SetItem : public Base {
			enum SUB_TYPE : unsigned char {
				COMMON,					// By TH S4 - (COMMON)
				CHARACTER,				// By TH S4 - (CHARACTER) CHARACTER_SET
				PARTS,					// By TH S4 - (PARTS) CHARACTER_SET_NEW
				CLUBSET = 4,			// By TH S4 - (CLUBSET)
				BALL,					// By TH S4 - (BALL)
				ITEM,					// By TH S4 - (ITEM) CHARACTER_SET_DUP_AND_ITEM_PASSIVE_AND_ACTIVE
				CADDIE,					// By TH S4 - (CADDIE)
				CARD,					// By TH S4 - (CARD)
				AUXPART,				// Anel
			};
			// By S4 TH
			enum SUB_TYPE_CHAR : unsigned char {
				NURI,
				HANA,
				AZER,
				CECILIA,
				MAX,
				KOOH,
				ARIN,
				KAZ,
				LUCIA,
				NELL,
				SPIKA,
				NURI_R,
				HANA_R,
				AZER_R,
				CECILIA_R,
				STC_CARD = 0xFD,
				EQUIP_ITEM,
				NOEQUIP_ITEM,
			};
			void clear() { memset(this, 0, sizeof(SetItem)); };
			struct Packege {
				unsigned int  qntd;
				unsigned int  item_typeid[10];
				unsigned short item_qntd[10];
			};
			Packege packege;
			unsigned short c[5];			// By TH S4 - (COM[5])
			unsigned short point;
		};

		// Mascot IFF
		struct Mascot : public Base {
			void clear() { memset(this, 0, sizeof(Mascot)); };
			char mpet[40];
			char textura[40];
			unsigned char price[5];
			unsigned char c[5];
			struct Efeito {
				short power_drive;			// power sem perder ctrl
				short drop_rate;			// % drop item
				short power_gague;			// (Acho) rate ou valor de unit
				short pang_rate;			// % pang
				short exp_rate;				// % exp
				unsigned char item_slot;
			};
			struct Mensagem {
				unsigned char active;
				short flag;					// pode ser 1 flag de 2 bytes ou 2 flag de 1 byte
				unsigned int  change_price;
			};
			struct BonusPangya {
				unsigned short pang;	// Acertando pang
				unsigned short flag;	// Errando pang
			};
			Efeito efeito;
			Mensagem msg;
			BonusPangya bonus_pangya;
		};

		// AuxPart IFF
		struct AuxPart : public Base {
			void clear() { memset(this, 0, sizeof(AuxPart)); };
			unsigned short cc[5];	// [0] qntd, (Acho) usa para tempo e essas coisas, 1day, 7, 15, 30, 365 e assim vai
			unsigned char c[5];
			unsigned char slot[5];
			struct stEfeito {
				unsigned short power_drive;	// power sem penalidade
				unsigned short drop_rate;
				unsigned short power_gauge;	// Rate ou valor de unit
				unsigned short pang_rate;
				unsigned short exp_rate;
				unsigned short unknown;		// Pode ser o efeito de item slot
				std::string toString() {
					return "Efeito [\n\tPOWER_DRIVE: " + std::to_string(power_drive) 
						+ ";\n\tDROP_RATE: " + std::to_string(drop_rate)
						+ ";\n\tPOWER_GAUGE: " + std::to_string(power_gauge)
						+ ";\n\tPANG_RATE: " + std::to_string(pang_rate)
						+ ";\n\tEXP_RATE: " + std::to_string(exp_rate)
						+ ";\n\tUNKNOWN: " + std::to_string(unknown)
						+".\n\t]";
				};
			};
			stEfeito efeito;
			//unsigned short efeito[6];	// [0] power sem penalidade, [1] Sorte acho que seja Drop, [2] EXP, [3] pode ser PANG, [4] tem valor no typeid 0x7001000B
			unsigned int  ulUnknown2;
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nCC[]: [CC0=" + std::to_string(cc[0]) + ", CC1=" + std::to_string(cc[1]) + ", CC2=" + std::to_string(cc[2]) 
							+ ", CC3=" + std::to_string(cc[3]) + ", CC4=" + std::to_string(cc[4]) + "]"
					+ "\r\nC[]: [C0=" + std::to_string((unsigned short)c[0]) + ", C1=" + std::to_string((unsigned short)c[1]) + ", C2=" 
							+ std::to_string((unsigned short)c[2]) + ", C3=" + std::to_string((unsigned short)c[3]) + ", C4=" + std::to_string((unsigned short)c[4]) + "]"
					+ "\r\nSlot[]: [Slot0=" + std::to_string((unsigned short)slot[0]) + ", Slot1=" + std::to_string((unsigned short)slot[1]) + ", Slot2="
							+ std::to_string((unsigned short)slot[2]) + ", Slot3=" + std::to_string((unsigned short)slot[3]) + ", Slot4=" + std::to_string((unsigned short)slot[4]) + "]"
					+ "\r\n" + efeito.toString()
					+ "\r\nulUnknown2: " + std::to_string(ulUnknown2) + "\r\n";
#else
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")";
#endif // _DEBUG
			};
		};

		// Ball IFF
		struct Ball : public Base {
			void clear() { memset(this, 0, sizeof(Ball)); };
			unsigned int  ulUnknown;
			char mpet[40];
			unsigned int  bound;			// By TH S4 - (Bound)
			unsigned int  roll;				// By TH S4 - (Roll)
			char seq[7][40];				// By TH S4 - (Fx)
			char fx[7][40];					// By TH S4 - (FxBone)
			unsigned short c[5];
			unsigned short point;
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nC[]: [C0=" + std::to_string((unsigned short)c[0]) + ", C1=" + std::to_string((unsigned short)c[1]) + ", C2="
							+ std::to_string((unsigned short)c[2]) + ", C3=" + std::to_string((unsigned short)c[3]) + ", C4=" + std::to_string((unsigned short)c[4]) + "]"
					+ "\r\nusUnknown: " + std::to_string(usUnknown) + "\r\n"; 
#else
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")";
#endif // _DEBUG
			};
		};

		// Caddie IFF
		struct Caddie : public Base {
			void clear() { memset(this, 0, sizeof(Caddie)); };
			unsigned int  valor_mensal;
			char mpet[40];
			unsigned short c[5];
			unsigned short point;
		};

		// CaddieItem IFF
		struct CaddieItem : public Base {
			enum Type : unsigned char {
				COOKIE,		// CASH
				PANG,		// PANG
				ESPECIAL,	// ACHO, por que não tem nenhum item com esse, não vi pelo menos
				UPGRADE
			};
			void clear() { memset(this, 0, sizeof(CaddieItem)); };
			char mpet[40];							// By TH S4 - (FaceTex)
			char textura[40];						// By TH S4 - (BodyTex)
			unsigned short price[5];				// By TH S4 - (COM[5])
			unsigned short unit_power_guage_start;	// unit start power guage que começa o jogo
		};

		// CadieMagicBox IFF
		struct CadieMagicBox {
			void clear() { memset(this, 0, sizeof(CadieMagicBox)); };
			unsigned int  seq;
			unsigned int  active;
			unsigned int  setor;
			unsigned int  character;
			unsigned int  level;
			unsigned int  ulUnknown;
			struct ItemReceive {
				unsigned int  _typeid;
				unsigned int  qntd;
			};
			struct ItemTrade {
				unsigned int  _typeid[4];
				unsigned int  qntd[4];
			};
			ItemReceive item_receive;
			ItemTrade item_trade;
			unsigned int  box_random_id;
			char name[40];
			SYSTEMTIME date[2];					// Date[0] Start, Date[1] End
		};

		// CadieMagicBoxRandom IFF
		struct CadieMagicBoxRandom {
			void clear() { memset(this, 0, sizeof(CadieMagicBoxRandom)); };
			unsigned int  id;
			struct ItemRandom {
				unsigned int  _typeid;
				unsigned int  qntd;
				unsigned int  rate;
			};
			ItemRandom item_random;
		};

		// Card IFF
		struct Card : public Base {
			void clear() { memset(this, 0, sizeof(Card)); };
			enum CARD_SUB_TYPE : unsigned {
				T_CHARACTER,
				T_CADDIE,
				T_SPECIAL,
				T_PACK,
				T_BOX_PACK,
				T_NPC,
			};
			unsigned char tipo;
			// era 40, mas acho que esse 1 de baixo seja o valor \0(final da string)
			char mpet[41];				// By TH S4 - (Image)
			unsigned short c[5];
			struct Efeito {
				unsigned short type;
				unsigned short qntd;
			};
			Efeito efeito;
			char textura[3][40];		// By TH S4 - (SubIcon[40], SlotImg[40], BuffImg[40])
			unsigned short tempo;		// By TH S4 - (UseTime) Os Cards especial que tem tempo
			unsigned short volume;		// By TH S4 - (Volume) Book
			unsigned int  position;		// By TH S4 - (CardIndex) Slot
			unsigned int  flag1;		// !@flag que guarda alguns valores de de N, R, SR, SC e etc
			unsigned int  flag2;		// flag que guarda alguns valores de de N, R, SR, SC e etc
			std::string toString() {
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nTipo: " + std::to_string((unsigned short)tipo)
					+ "\r\nEfeito[Type=" + std::to_string(efeito.type) + ", Qntd=" + std::to_string(efeito.qntd) + "]"
					+ "\r\nTempo: " + std::to_string(tempo)
					+ "\r\nVolume(Pack): " + std::to_string(volume)
					+ "\r\nPosistion(Book): " + std::to_string(position)
					+ "\r\nC[C0=" + std::to_string(c[0]) + ", C1=" + std::to_string(c[1]) + ", C2=" + std::to_string(c[2]) + ", C3=" + std::to_string(c[3]) + ", C4=" + std::to_string(c[4]) + "]"
					+ "\r\nFlag[F1=" + std::to_string(flag1) + ", F2=" + std::to_string(flag2) + "]\r\n";
			};
		};

		// Character IFF
		struct Character : public Base {
			void clear() { memset(this, 0, sizeof(Character)); };
			char mpet[40];
			char textura[3][40];				// By TH S4 - (HairTex[40], ShirtsTex[40], FaceTex[40])
			unsigned short c[5];
			unsigned char num_parts;			// By TH S4 - (nParts)
			unsigned char num_accessorios;		// By TH S4 - (nAcsries)
			unsigned int  club_type;			// By TH S4 - (ClubType)
			float scale_club_set;				// By TH S4 - (ClubScale)
			unsigned char c_stat[5];			// By TH S4 - (PCL)
			char camera[43];					// By TH S4 - (Mtn30sWinner == Motion Tourney Winner) Textura, Animação ou Câmera
		};

		// CharacterMastery IFF
		struct CharacterMastery {
			void clear() { memset(this, 0, sizeof(CharacterMastery)); };
			unsigned int  active;
			unsigned int  _typeid;
			unsigned int  seq;
			unsigned int  stats;
			unsigned int  level;
			struct Condition {
				unsigned int  condition[5];
				unsigned int  qntd[5];
			};
			Condition condition;
		};

		// Club IFF
		struct Club : public Base {
			void clear() { memset(this, 0, sizeof(Club)); };
			char mpet[40];
			unsigned short tipo;		// 0 Wood, 1 Iron, 2 SW, 3 PUTT
			unsigned short c[5];
		};

		// ClubSet IFF
		struct ClubSet : public Base {
			void clear() { memset(this, 0, sizeof(ClubSet)); };
			unsigned int  club[4];
			unsigned short c[5];
			unsigned short slot[5];
			struct WorkShop {
				int tipo;							// -1 não pode up rank e nem level, 0 pode tudo
				unsigned int  rank_s_stat;			// para o stat do rank S bonus
				unsigned int  total_recovery;		// recovery points
				float rate;							// Rate que vai pegar por hole jogados
				unsigned int  tipo_rank_s;			// power, spin, control end special para EXP
				unsigned int  flag_transformar;		// Que pode Transformar nas taqueiras especiais
				// o de cima pode ser short e aqui em baixo ter outra flag
			};
			WorkShop work_shop;
			unsigned int  ulUnknown;		// Pode ser do WorkShop, mas ainda não sei
			unsigned int  text_pangya;		// [text_pangya_Logo] é o texto que aparece quando acerta pangya, Uso como ClubSet Original Transforme por que todos vão ter o mesmo logo, e o número do logo é o do Original
		};

		// ClubSetWorkShopLevelUpLimit IFF
		struct ClubSetWorkShopLevelUpLimit {
			void clear() { memset(this, 0, sizeof(ClubSetWorkShopLevelUpLimit)); };
			unsigned int  tipo;
			unsigned int  rank;
			unsigned short c[5];
			unsigned short option;
		};

		// ClubSetWorkShopLevelUpProb IFF
		struct ClubSetWorkShopLevelUpProb {
			void clear() { memset(this, 0, sizeof(ClubSetWorkShopLevelUpProb)); };
			unsigned int  tipo;
			unsigned int  c[5];
		};

		// ClubSetWorkShopRankUpExp IFF
		struct ClubSetWorkShopRankUpExp {
			void clear() { memset(this, 0, sizeof(ClubSetWorkShopRankUpExp)); };
			unsigned int  tipo;
			unsigned int  rank[6];	// F ~ A
		};

		// Course IFF
		struct Course : public Base {
			void clear() { memset(this, 0, sizeof(Course)); };
			char mpet[40];
			char gbin[40];									// By TH S4 - (AmbiendSnd)
			struct Star {
				union {
					unsigned char ucStar;
					struct StarMask {
						unsigned char star_num : 4;			// By TH S4 - (Difficulty)
						unsigned char star_size : 4;		// By TH S4 - (DiffFlag)
					};
					StarMask star_mask;
				} uStarMask;
			};
			Star star;
			char xml[43];									// By TH S4 - (TexProp)
			float rate_pang;								// By TH S4 - (Slope)
			char seq[40];									// By TH S4 - (SkyFx)
			unsigned int  ulUnknown[12];					// pode ser unsigned short COM[5], unsigned short point
			struct ParScore {
				unsigned char par_hole[18];
				unsigned char min_score_hole[18];
				unsigned char max_score_hole[18];
			};
			ParScore par_score_hole;
			unsigned short usUnknown;
		};

		// CutinInfomation IFF
		struct CutinInfomation {
			// union Condition
			union uCondition {
				uCondition(uint32_t _ul = 0u) : ulCondition(_ul) {};
				void clear() { ulCondition = 0u; };
				uint32_t ulCondition;
				struct {
					uint32_t power_shot : 1;			// By TH S4
					uint32_t double_power_short : 1;	// By TH S4
					uint32_t power_shot_failed : 1;		// By TH S4
					uint32_t chipin : 1, : 0;			// By TH S4
				}stBit;
			};
			void clear() { memset(this, 0, sizeof(CutinInfomation)); };
			unsigned int  active;
			unsigned int  _typeid;						// By TH S4 - (NormalTypeid)
			unsigned int  rare_typeid;					// By TH S4 - (RareTypeid)
			unsigned int  rarity;						// By TH S4 - (Rarity)
			uCondition	  tipo;							// By TH S4 - (Condition) 1= 1PS, 2= 2PS, 4= Erro Pangya com especial(Toma, Spike Cobra)
			unsigned int  sector;						// By TH S4 - (Img_Pos)
			unsigned int  character_id;					// By TH S4 - (CharIndex) 0 Nuri, 1 Hana, 2 Azer e etc
			struct Img {
				char sprite[40];						// By TH S4 - (Tex)
				unsigned int  tipo;						// By TH S4 - (Ani)
			};
			Img img[4];									// By TH S4 - (Char, Bg, Patter, Text)
			unsigned int  tempo;						// By TH S4 - (Out_Ani)
		};

		// Enchant IFF
		struct Enchant {
			void clear() { memset(this, 0, sizeof(Enchant)); };
			unsigned int  active;
			unsigned int  _typeid;
			uint64_t pang;	// Value para ser descontado
		};

		// Furniture IFF
		struct Furniture : public Base {
			void clear() { memset(this, 0, sizeof(Furniture)); };
			char mpet[40];
			unsigned short num;				// By TH S4 - (Num)
			unsigned short is_own;			// By TH S4 - (IsOwn)
			unsigned short is_move;			// By TH S4 - (IsMove) 5 Poster
			unsigned short is_function;		// By TH S4 - (IsFunction)  0x62 Poster B, 0x63 Poster A
			unsigned int  etc;				// By TH S4 - (Etc)
			struct Location {
				float x;
				float y;
				float z;
				float r;
			};
			Location location;
			char textura[3][40];			// By TH S4 - (Tex)
			char textura_org[3][40];		// By TH S4 - (OrgTex)
			unsigned short c[5];			// By TH S4 - (COM[5])
			unsigned short use_time;		// By TH S4 - (UseTime)
		};

		// HairStyle IFF
		struct HairStyle : public Base {
			void clear() { memset(this, 0, sizeof(HairStyle)); };
			unsigned char cor;				// By TH S4 - (HairID)
			unsigned char character;		// By TH S4 - (CharID)
			unsigned short usUnknown;
		};

		// Match IFF
		struct Match {
			void clear() { memset(this, 0, sizeof(Match)); };
			unsigned int  active;
			unsigned int  _typeid;
			char name[80];
			unsigned char level;			// By TH S4 - (Level) Não sei o que é, mas em todos é 10(0x0A)
			char trophy[6][40];				// By TH S4 - (icon[6][40])
			unsigned char ucUnknown2[3];
		};

		// Skin IFF
		struct Skin : public Base {
			void clear() { memset(this, 0, sizeof(Skin)); };
			char mpet[40];
			unsigned char horizontal_scroll;	// By TH S4 - (HScroll)
			unsigned char vertical_scroll;		// By TH S4 - (VScroll) 256 efeito de rolagem vertical
			unsigned short price[5];
		};

		// Ability IFF
		struct Ability {
			enum class eEFFECT_TYPE : unsigned int  {
				NONE, 
				PIXEL,							// Pixel o valor em rate
				PIXEL_BY_WIND_NO_ITEM,			// Pixel dependendo do vento o valor em rate, se usar item ou ps cancela o efeito
				PIXEL_OVER_WIND_NO_ITEM,		// Pixel acima de um vento o valor em rate, se usar item ou ps cancela o efeito
				PIXEL_BY_WIND,					// Pixel dependendo do vento o valor em rate
				PIXEL_2,						// Pixel o valor em rate
				PIXEL_WITH_WEAK_WIND,			// Pixel quando o vento é fraco o valor em rate
				POWER_GAUGE_TO_START_HOLE,		// Power Gauge no começo do hole para cada hole o valor em rate
				POWER_GAUGE_MORE_ONE,			// Power Gauge da uma barra a+ 33 Units, o valor em rate
				POWER_GUAGE_TO_START_GAME,		// Power Gauge no começo do jogo o valor em rate
				PAWS_NOT_ACCUMULATE,			// Patinha não acumula com outro efeito de patinha, probabilidade está em rate
				SWITCH_TWO_EFFECT,				// Item com 2 efeitos não simutâneos, qual efeito está em rate, 0 Yards, 1 Power Gauge
				EARCUFF_DIRECTION_WIND,			// Muda a direção do vento, a probabilidade quem escolhe é o pangya
				COMBINE_ITEM_EFFECT,			// Combinação de itens, em rate tem o ID da combinação em (IFF)SetEffectTable
				SAFETY_CLIENT_RANDOM,			// Safety a probabilidade o cliente que decide
				PIXEL_RANDOM,					// Pixel aleatório o valor está em rate, a probabilidade o cliente que decide
				WIND_1M_RANDOM,					// Wind 1m aleatório a probabilidade está em rate
				PIXEL_BY_WIND_MIDDLE_DOUBLE,	// Pixel dependendo do vento, vento médio dá o dobro, o valor em rate
				GROUND_100_PERCENT_RONDOM,		// Terreno 100% aleatório, a probabilidade está em rate
				ASSIST_MIRACLE_SIGN,			// Assist Olho Mágico
				VECTOR_SIGN,					// Mostra uma seta na bola, dependendo do vento, tipo trajetória do assist
				ASSIST_TRAJECTORY_SHOT,			// Assist Trajectory Shot
				PAWS_ACCUMULATE,				// Patinha acumula com outro efeito de patinha, a probabilidade está em rate
				POWER_GAUGE_FREE,				// Power Gauge, ganha 1 Power Gauge de graça para usar na tacada
				SAFETY_RANDOM,					// Safety aleatório a probabilidade está em rate
				ONE_IN_ALL_STATS,				// [UNKNOWN] mas vou deixar o (Combine Itens) ONE IN ALL STATS, dá 1 para todos os stats, power, cltr, accuracy, spin e curve
				POWER_GAUGE_BY_MISS_SHOT,		// Power Gauge mesmo que erre pangya ou use item de Power Gauge ele ainda dá Power Gauge
				PIXEL_BY_WIND_2,				// Pixel dependendo do vento o valor está em rate
				PIXEL_WITH_RAIN,				// Pixel quando estiver chovendo(recovery) o valor está em rate
				NO_RAIN_EFFECT,					// Sem efeito dá chuva no terreno
				PUTT_MORE_10Y_RANDOM,			// +10y no Putt aleatório a probabilidade está em rate
				UNKNOWN_31,
				MIRACLE_SIGN_RANDOM,			// Olho Mágico aleatório a probabilidade está em rate
				UNKNOWN_33,
				DECREASE_1M_OF_WIND,			// Diminui 1m do vento
			};
			void clear() { memset(this, 0, sizeof(Ability)); };
			unsigned int  _typeid;
			struct Efeito {
				unsigned int  EfeitoOrNo[3];		// Ativação na hora da tacada(ACHO)
				unsigned int  type[3];
				float rate[3];
			};
			Efeito efeito;
			unsigned char ucUnknown[32];
			unsigned int  flag1;
			unsigned int  flag2;
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nEfeitoOuNao: " + std::to_string(efeito.EfeitoOrNo[0])
					+ ", " + std::to_string(efeito.EfeitoOrNo[1])
					+ ", " + std::to_string(efeito.EfeitoOrNo[2])
					+ "\r\nEfeito:      " + std::to_string(efeito.type[0])
					+ ", " + std::to_string(efeito.type[1])
					+ ", " + std::to_string(efeito.type[2])
					+ "\r\nRate:        " + std::to_string(efeito.rate[0])
					+ ", " + std::to_string(efeito.rate[1])
					+ ", " + std::to_string(efeito.rate[2])
					+ "\r\nFlag1: " + std::to_string(flag1)
					+ "\tFlag2: " + std::to_string(flag2)
					+ hex_util::BufferToHexString(ucUnknown, sizeof(ucUnknown));
#else
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")";
#endif
			};
		};

		// Desc IFF
		struct Desc {
			void clear() { memset(this, 0, sizeof(Desc)); };
			unsigned int  _typeid;
			char description[512];
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nDescription:\r\n" + description + "\r\n";
#else
				return "Typeid: " + std::to_string(_typeid)
					+ "(0x" + hex_util::ltoaToHex(_typeid) + ")";
#endif // _DEBUG
			};
		};

		// GrandPrixAIOptionalData IFF(.sff)
		struct GrandPrixAIOptionalData {
			void clear() { memset(this, 0, sizeof(GrandPrixAIOptionalData)); };
			unsigned int  active;
			unsigned int  id;
			char name[36];
			unsigned int  BetterOrNo;		// Os Primeiro da classe
			unsigned int  char_id;			// Character ID, 4000000 | ID
			unsigned int  _class;			// Class do GrandPrix
			unsigned int  parts_typeid[24];
			unsigned char ucUnknown[452];
			std::string toString() {
#ifdef _DEBUG
				return "Info Bot\r\nName: " + std::string(name)
					+ "\r\nID: " + std::to_string(id)
					+ "\r\nCharacter ID: " + std::to_string(char_id)
					+ "\r\nFirster Class: " + std::to_string(BetterOrNo)
					+ "\r\nClass: " + std::to_string(_class)
					+ hex_util::BufferToHexString(ucUnknown, sizeof(ucUnknown));
#else
				return "Info Bot\r\nTypeid: " + std::to_string(id);
#endif
			};
		};

		// GrandPrixConditionEquip IFF
		struct GrandPrixConditionEquip {
			void clear() { memset(this, 0, sizeof(GrandPrixConditionEquip)); };
			unsigned int  active;
			unsigned int  _typeid;
			unsigned int  item_typeid;
			char info[516];
		};

		// GrandPrixData IFF
		// Typeid 0x[00(1)][00(2)][00(3)][00(4)]
		//
		// (1) 2 bit == 3 Event, 2 bit == 0 Normal
		// (2) Class "Aba"
		// (3) Tipo (course ou config diferente)
		// (4) Id (sequência) horários
		struct GrandPrixData {
		public:
			enum GP_ABA : unsigned char {
				ROOKIE,			// Tutorial joga em istância - Aba Tuto
				BEGINNER,		// Class 1 a 2				 - Aba 1
				JUNIOR,			// Class 3 a 5				 - Aba 2
				SENIOR,			// Class 6 a 9				 - Aba 3
			};

		public:
			void clear() { memset(this, 0, sizeof(GrandPrixData)); };
			unsigned int  active;
			unsigned int  _typeid;
			unsigned int  typeid_link;			// Link para outros IFF do GrandPrix de suporte
			unsigned int  type;
			unsigned short time_hole;
			char name[64];
			unsigned short usUnknown;			// Esse pode ser continuação do nome, ou um valor desconhecido
			struct Ticket {
				unsigned int  _typeid;
				unsigned int  qntd;
			};
			Ticket ticket;
			char img[41];
			struct Flag {
				unsigned char natural;
				unsigned char short_game;
				unsigned char hole_cup_x2;
			};
			Flag flag;
			unsigned int  rule;					// Typeid da rule, regra do GrandPrix Ex: 1m, 10s, 15s e etc
			struct CourseInfo {
				unsigned int  course;
				unsigned int  modo;
				unsigned char qntd_hole;
			};
			CourseInfo course_info;
			unsigned char level_min;
			unsigned char level_max;
			unsigned char ucUnknown;
			unsigned int  condition[2];			// Condition[0] = AVG menor, Condition[1] = AVG maior
			struct BOT {
				int score_max;
				int score_med;
				int score_min;
			};
			BOT bot;
			unsigned int  _class;				// estava dificuldade no antigo, mas acho que seja classe
			unsigned int  pang;					// Pode ser os pangs que o bot faz
			struct Reward {
				unsigned int  _typeid[5];
				unsigned int  qntd[5];
				unsigned int  time[5];			// Time do Item se for por tempo
			};
			Reward reward;
			SYSTEMTIME open;					// aqui é quando abre a sala do gp
			SYSTEMTIME start;					// aqui quando começa
			SYSTEMTIME end;						// aqui quando termina
			unsigned int  ulUnknown;			// Não sei o que seja direito, mas pode ser outro do clear_GP
			unsigned int  clear_gp_typeid;		// typeid do GrandPrix que bloquea ele se nao estiver concluido
			unsigned int  lock_yn;				// Esse só bloquea se tiver um typeid de um gp no de cima sem concluir
			char info[516];
		};

		// GrandPrixRankReward IFF
		struct GrandPrixRankReward {
			void clear() { memset(this, 0, sizeof(GrandPrixRankReward)); };
			unsigned int  active;
			unsigned int  _typeid;
			unsigned int  rank;
			struct Reward {
				unsigned int  _typeid[5];
				unsigned int  qntd[5];
				unsigned int  time[5];	// Time do Item se for por tempo
			};
			Reward reward;
			unsigned int  trophy_typeid;
		};

		// GrandPrixSpecialHole IFF
		struct GrandPrixSpecialHole {
			void clear() { memset(this, 0, sizeof(GrandPrixSpecialHole)); };
			unsigned int  active;
			unsigned int  _typeid;
			unsigned int  seq;				// Esse é a ordem dos holes, de 1 a 18 dependendo da quantidade de holes
			unsigned int  course;
			unsigned int  hole;
		};

		// MemorialShopCoinItem IFF(.sff)
		struct MemorialShopCoinItem {
			enum TYPE_FILTER : unsigned int  {
				SPRING = 1,
				SUMMER,
				FALL,
				WINTER,
				CLUBSET,
				SETITEM,
				EAR,
				WING,
				LUVA,
				RING_R,
				RING_L,
				CADDIE,
				MASCOT,
				SUMMER_HOLYDAY,
				XMAS,
				HALLOWEEN,
				MAN,
				WOMAN,
				NURI,
				HANA,
				AZER,
				CECI,
				MAX,
				KOOH,
				ARIN,
				KAZ,
				LUCIA,
				NELL,
				SPIKA,
				NURI_R,
				HANA_R,
				AZER_R,
				CECI_R,
			};
			void clear() { memset(this, 0, sizeof(MemorialShopCoinItem)); };
			unsigned int  active;
			unsigned int  _typeid;
			unsigned int  type;
			unsigned int  probability;
			struct GachaRange {
				unsigned int  number_min;
				unsigned int  number_max;
				bool empty() {
					return (number_min == 0 && number_max == 0);
				};
				bool isBetweenGacha(unsigned int  _number) {
					return (number_min <= _number && _number <= number_max);
				};
			};
			GachaRange gacha_range;
			/*
			+ Tipos dos filtros
			+ 1, 2, 3 e 4, Estação, Primavera, Verão, Outono, Inverno
			+ 5 Clubset, 6 SetItem, 7 Orelha(Ear), 8 Asa(Wing), 9 Luva
			+ 10 Anel Mão direita, 11 Anel Mão esquerda, 12 Caddie
			+ 13 Mascot, 14 Summer(verias de verão), 15 Natal, 16 Halloween
			+ 17 Homem, 18 Mulher, 19 Nuri, 20 Hana, 21 Azer, 22 Cecilia
			+ 23 Max, 24 Kooh, 25 Arin, 26 Kaz, 27 Lucia, 28 Nell, 29 Spika
			+ 30 Nuri R, 31 Hana R, (32 é Azer R, mas a ntreev não fez ele), 33 Cecilia R
			*/
			unsigned int  filter[10];		// Filtros com os tipos dos item para pesquisar no outro IFF do memorial shop rare item 
			bool hasFilter(unsigned int  _filter) {
				if (_filter == 0)
					return false;

				for (auto i = 0u; i < (sizeof(filter) / sizeof(filter[0])); ++i)
					if (filter[i] == _filter)
						return true;
				
				return false;
			};
			bool emptyFilter() {
				unsigned int  count = 0u;

				for (auto i = 0u; i < (sizeof(filter) / sizeof(filter[0])); ++i)
					count += filter[i];

				return count == 0;
			};
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nType: " + std::to_string(type)
					+ "\r\nProbability: " + std::to_string(probability)
					+ "\r\nFilter(s): " + std::to_string(filter[0])
					+ ", " + std::to_string(filter[1])
					+ ", " + std::to_string(filter[2])
					+ ", " + std::to_string(filter[3])
					+ ", " + std::to_string(filter[4])
					+ ", " + std::to_string(filter[5])
					+ ", " + std::to_string(filter[6])
					+ ", " + std::to_string(filter[7])
					+ ", " + std::to_string(filter[8])
					+ ", " + std::to_string(filter[9]) + "\r\n";
#else
				return "Typeid: " + std::to_string(_typeid);
#endif
			};
		};

		// MemorialShopRareItem IFF
		struct MemorialShopRareItem {
			void clear() { memset(this, 0, sizeof(MemorialShopRareItem)); };
			unsigned int  active;
			struct Gacha {
				unsigned int  number;
				unsigned int  count;
			};
			Gacha gacha;
			unsigned int  _typeid;
			unsigned int  probability;
			unsigned int  rare_type;		// Tipo Raro, EX: -1 - 0 normal, 1 - 2 raro, 3 - 4 Super raro
			/*
			+ Tipos dos filtros
			+ 1, 2, 3 e 4, Estação, Primavera, Verão, Outono, Inverno
			+ 5 Clubset, 6 SetItem, 7 Orelha(Ear), 8 Asa(Wing), 9 Luva
			+ 10 Anel Mão direita, 11 Anel Mão esquerda, 12 Caddie
			+ 13 Mascot, 14 Summer(verias de verão), 15 Natal, 16 Halloween
			+ 17 Homem, 18 Mulher, 19 Nuri, 20 Hana, 21 Azer, 22 Cecilia
			+ 23 Max, 24 Kooh, 25 Arin, 26 Kaz, 27 Lucia, 28 Nell, 29 Spika
			+ 30 Nuri R, 31 Hana R, (32 é Azer R, mas a ntreev não fez ele), 33 Cecilia R
			*/
			unsigned int  filter[10];		// O mesmo de cima
			char s_string[28];
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nRare Type: " + std::to_string(rare_type)
					+ "\r\nProbability: " + std::to_string(probability)
					+ "\r\nFilter(s): " + std::to_string(filter[0])
						+ ", " + std::to_string(filter[1])
						+ ", " + std::to_string(filter[2])
						+ ", " + std::to_string(filter[3])
						+ ", " + std::to_string(filter[4])
						+ ", " + std::to_string(filter[5])
						+ ", " + std::to_string(filter[6])
						+ ", " + std::to_string(filter[7])
						+ ", " + std::to_string(filter[8])
						+ ", " + std::to_string(filter[9])
					+ "\r\nString S (Index): " + std::string(s_string) + "\r\n";
#else
				return "Typeid: " + std::to_string(_typeid);
#endif
			};
		};

		// AddonPart IFF
		struct AddonPart {
			void clear() { memset(this, 0, sizeof(AddonPart)); };
			unsigned int  active;
			unsigned int  _typeid;
			char name[40];
			char textura[6][40];		// Textura e effect textura
		};

		// ArtifactManaInfo IFF
		struct ArtifactManaInfo {
			void clear() { memset(this, 0, sizeof(ArtifactManaInfo)); };
			unsigned int  active;
			unsigned int  artifact_typeid;
			unsigned int  mana_typeid;
			char info[132];
			unsigned int  type;			// 0 = Todos modos (VS, CAMP, MATCH ...) menos GrandPrix, 1 = Só 18H, 3 = Só GrandPrix
			unsigned int  ulUnknown;
			std::string toString() {
#ifdef _DEBUG
				return "Artifact Typeid: " + std::to_string(artifact_typeid) + "(0x" + hex_util::ltoaToHex(artifact_typeid) + ")"
					+ "\r\nMana Typeid: " + std::to_string(mana_typeid) + "(0x" + hex_util::ltoaToHex(mana_typeid) + ")"
					+ "\r\nType: " + std::to_string(type)
					+ "\r\nulUnknown: " + std::to_string(ulUnknown)
					+ "\r\nInfo.\r\n" + std::string(info);
#else
				return "Artifact Typeid: " + std::to_string(artifact_typeid) + "(0x" + hex_util::ltoaToHex(artifact_typeid) + ")"
					+ "\r\nMana Typeid: " + std::to_string(mana_typeid) + "(0x" + hex_util::ltoaToHex(mana_typeid) + ")";
#endif
			}
		};

		// CaddieVoiceTable IFF
		struct CaddieVoiceTable {
			void clear() { memset(this, 0, sizeof(CaddieVoiceTable)); };
			unsigned int  _typeid;
			char name[64];
			unsigned char type;
			char shot_name[65];
			std::string toString() {
#ifdef _DEBUG
				return "Name: " + std::string(name)
					+ "\r\nTypeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nType: " + std::to_string(type)
					+ "\r\nShot Name: " + std::string(shot_name);
#else
				return "Typeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")";
#endif
			};
		};

		// ErrorCodeInfo IFF
		struct ErrorCodeInfo {
			void clear() { memset(this, 0, sizeof(ErrorCodeInfo)); };
			unsigned int  active;
			unsigned int  code;
			unsigned int  type;
			char info[260];
			std::string toString() {
#ifdef _DEBUG
				return "ErrorCode: " + std::to_string(code) + " (0x" + hex_util::ltoaToHex(code) + ")"
					+ "\r\nType: " + std::to_string(type)
					+ "\r\nInfo.\r\n" + std::string(info);
#else
				return "ErrorCode: " + std::to_string(code) + " (0x" + hex_util::ltoaToHex(code) + ")";
#endif
			};
		};

		// FurnitureAbility IFF
		struct FurnitureAbility {
			// union uAbilityType
			union uAbilityType {
				uAbilityType(uint32_t _ul = 0u) : ulAbilityType(_ul) {};
				void clear() { ulAbilityType = 0u; };
				uint32_t ulAbilityType;
				struct {
					uint32_t buff : 1, : 0;			// By TH S4
				}stBit;
			};
			union uSuccessType {
				uSuccessType(uint16_t _ul = 0u) : ulSuccessType(_ul) {};
				void clear() { ulSuccessType = 0u; };
				uint16_t ulSuccessType;
				struct {
					uint16_t stay : 1;			// By TH S4
					uint16_t putin : 1;			// By TH S4
					uint16_t putout : 1, : 0;	// By TH S4
				}stBit;
			};
			union uEffectType {
				uEffectType(uint16_t _ul = 0u) : ulEffectType(_ul) {};
				void clear() { ulEffectType = 0u; };
				uint16_t ulEffectType;
				struct {
					uint16_t me : 1;			// By TH S4
					uint16_t _friend : 1;		// By TH S4
					uint16_t guild : 1;			// By TH S4
					uint16_t all : 1, : 0;		// By TH S4
				}stBit;
			};
			void clear() { memset(this, 0, sizeof(FurnitureAbility)); };
			unsigned int  active;
			unsigned int  _typeid;
			uAbilityType  type;					// By TH S4 - (btAbilityType)
			unsigned int  stay_time;			// By TH S4 - (iStayTime)
			uSuccessType success_type;			// By TH S4 - (btSuccessType)
			uEffectType  effect_type;			// By TH S4 - (btEffectType) 0 Seu Furniture, 3 Furniture de outra pessoa
			unsigned int  set_in_typeid;		// By TH S4 - (dwSetInTID)
			unsigned int  max_qntd;				// By TH S4 - (iMaxCountAtFurniture) O maximo de item que pode ganhar
			SYSTEMTIME    date;					// Date Inicio
			unsigned int  during_time;			// By TH S4 - (dwDuringTime)
			struct Item {
				unsigned int  _typeid;
				unsigned int  probability;
			};
			Item item;
			unsigned int  max_count_by_user;	// By TH S4 - (iMaxCountAtUser)
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nType: " + std::to_string(type.ulAbilityType)
					+ "\r\nSetInTypeid): " + std::to_string(set_in_typeid)
					+ "\r\nMax Qntd: " + std::to_string(max_qntd)
					+ "\r\nSuccessType: " + std::to_string(success_type.ulSuccessType)
					+ "\r\nEffectType: " + std::to_string(effect_type.ulEffectType)
					+ "\r\nMaxCountByUser: " + std::to_string(max_count_by_user)
					+ "\r\nStayTime: " + std::to_string(stay_time)
					+ "\r\nDuringTime " + std::to_string(during_time)
					+ "\r\nDate Start: " + _formatDate(date);
#else
				return "Typeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")";
#endif
			};
		};

		// HoleCupDropItem IFF
		struct HoleCupDropItem {
			void clear() { memset(this, 0, sizeof(HoleCupDropItem)); };
			unsigned int  _typeid;
			char animation[40];
		};

		// LevelUpPrizeItem IFF
		struct LevelUpPrizeItem {
			void clear() { memset(this, 0, sizeof(LevelUpPrizeItem)); };
			unsigned char active;
			char name[33];
			unsigned short level;
			struct Reward {
				unsigned int  _typeid[2];
				unsigned int  qntd[2];
				unsigned int  time[2];		// Time do Item se for por tempo
			};
			Reward reward;
			char description[132];
		};

		// NonVisibleItemTable IFF
		struct NonVisibleItemTable {
			void clear() { memset(this, 0, sizeof(NonVisibleItemTable)); };
			unsigned int  active;
			unsigned int  type;
			unsigned int  _typeid;
			struct Date {
				SYSTEMTIME start;
				SYSTEMTIME end;
			};
			Date date;	// ACHO
		};

		// PointShop IFF(Antigo Sistema de troca de itens da Tiki)
		struct PointShop {
			void clear() { memset(this, 0, sizeof(PointShop)); };
			unsigned int  active;
			unsigned int  _typeid;
			unsigned int  point;		// Qntd de pontos que vai trocar pelo itens
			unsigned int  qntd;			// Qntd de itens que vai trocar pelos pontos
			unsigned int  flag;			// pode ser rate
		};

		// ScratchRewardSetting IFF(No JP não tem nenhum para eu ter de exemplo), depois procuro no KR
		struct ScratchRewardSetting {
			void clear() { memset(this, 0, sizeof(ScratchRewardSetting)); };
			unsigned int  active;
			unsigned int  _typeid;
		};

		// SetEffectTable IFF
		/*
		+ Effects
		+ 1 - Cutin
		+ 2 - Unknown
		+ 3 - Unknown
		+ 4 - +1px
		+ 5 - +Yard, o valor está em effect_add_power
		+ 6 - +1 all stats
		+ 7 - (-1) wind speed
		+ 8 - Patinha
		*/
		struct SetEffectTable {
			enum eEFFECT : unsigned char {
				ANIMATION = 1,
				UNKNOWN_V2,
				CUTIN,
				PIXEL,
				BASE,			// O 5 - esse aqui é um valor base, que usa quando quer da power sem ps e quando quer combinar item  só roupa Hana R
				ONE_ALL_STATS,
				WIND_DECREASE,
				PATINHA,
			};
			enum eEFFECT_TYPE : unsigned char {
				UNKNOWN_V1 = 1,
				GAME = 2,
				ROOM = 4,
				LOUNGE = 8,
			};
			void clear() { memset(this, 0, sizeof(SetEffectTable)); };
			unsigned int  id;
			struct Effect {
				unsigned int  effect[3];	// eEFFECT = Effect[0~2] é o da descrição em cima
				unsigned int  type[3];		// eEFFECT_TYPE = type[0~2], 2 Game, 4 Room e 8 Lounge
			};
			Effect effect;
			struct Item {
				unsigned int  _typeid[5];
				unsigned char active[5];
			};
			Item item;
			unsigned char ucUnknown[11];
			unsigned short slot[5];
			unsigned short effect_add_power;	// Força sem penalidade
			std::string toString() {
#ifdef _DEBUG
				return "ID: " + std::to_string(id)
					+ "\r\nType(s): " + std::to_string(effect.type[0])
						+ ", " + std::to_string(effect.type[1])
						+ ", " + std::to_string(effect.type[2])
					+ "\r\nEffect(s): " + std::to_string(effect.effect[0])
						+ ", " + std::to_string(effect.effect[1])
						+ ", " + std::to_string(effect.effect[2])
					+ "\r\nItem(ns) Typeid: " + std::to_string(item._typeid[0])
						+ ", " + std::to_string(item._typeid[1])
						+ ", " + std::to_string(item._typeid[2])
						+ ", " + std::to_string(item._typeid[3])
						+ ", " + std::to_string(item._typeid[4])
					+ "\r\nItem(ns) active: " + std::to_string(item.active[0])
						+ ", " + std::to_string(item.active[1])
						+ ", " + std::to_string(item.active[2])
						+ ", " + std::to_string(item.active[3])
						+ ", " + std::to_string(item.active[4])
					+ "\r\nSlot(s): " + std::to_string(slot[0])
						+ ", " + std::to_string(slot[1])
						+ ", " + std::to_string(slot[2])
						+ ", " + std::to_string(slot[3])
						+ ", " + std::to_string(slot[4])
					+ "\r\nEffect Add Power: " + std::to_string(effect_add_power)
					+ "\r\nucUnknown\r\n" + hex_util::BufferToHexString(ucUnknown, sizeof(ucUnknown));
#else
				return "ID: " + std::to_string(id);
#endif
			};
		};

		// ShopLimitItem IFF
		struct ShopLimitItem {
			void clear() { memset(this, 0, sizeof(ShopLimitItem)); };
			unsigned int  active;
			unsigned int  type;
			unsigned int  _typeid;
			unsigned int  level_max;		// ACHO
			unsigned int  level_min;		// ACHO
			unsigned int  ulUnknown;
			unsigned int  ulUnknown2;
			struct Date {
				SYSTEMTIME start;
				SYSTEMTIME end;
			};
			Date date;
		};

		// SpecialPrizeItem IFF
		struct SpecialPrizeItem {
			void clear() { memset(this, 0, sizeof(SpecialPrizeItem)); };
			unsigned int  _typeid;
			unsigned int  type;
			float rate;				// Não ainda
			std::string toString() {
#ifdef _DEBUG
				return "Typeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")"
					+ "\r\nType: " + std::to_string(type)
					+ "\r\nRate: " + std::to_string(rate);
#else
				return "Typeid: " + std::to_string(_typeid) + "(0x" + hex_util::ltoaToHex(_typeid) + ")";
#endif
			};
		};

		// SubscriptionItemTable IFF
		struct SubscriptionItemTable {
			void clear() { memset(this, 0, sizeof(SubscriptionItemTable)); };
			unsigned int  active;
			unsigned int  type;
			unsigned int  _typeid;
			struct Date {
				SYSTEMTIME start;
				SYSTEMTIME end;
			};
			Date date;	// ACHO
		};

		// TikiShopBase estrutura de suporte do (Antigo Sistema de troca de itens da Tiki)
		struct TikiShopBase {
			void clear() { memset(this, 0, sizeof(TikiShopBase)); };
			unsigned int  id;
			unsigned char type;
			char name[35];
		};

		// TikiPointTable IFF(Antigo Sistema de troca de itens da Tiki)
		struct TikiPointTable : public TikiShopBase {
			void clear() { memset(this, 0, sizeof(TikiPointTable)); };
			unsigned int  min;
			unsigned int  max;
		};

		// TikieRecipe IFF(Antigo Sistema de troca de itens da Tiki)
		struct TikiRecipe : public TikiShopBase {
			void clear() { memset(this, 0, sizeof(TikiRecipe)); };
			unsigned int  recipe_qntd[3];
		};

		// TikiSpecialTable IFF(Antigo Sistema de troca de itens da Tiki)
		struct TikiSpecialTable : public TikiShopBase {
			void clear() { memset(this, 0, sizeof(TikiSpecialTable)); };
			unsigned int  qntd;				// qtnd de recipes, max 4;
			unsigned int  recipe_qntd[4];
		};

		// TimeLimitItem IFF
		struct TimeLimitItem {
			void clear() { memset(this, 0, sizeof(TimeLimitItem)); };
			unsigned int  active;
			unsigned int  _typeid;
			char name[40];
			char icon[40];
			unsigned int  type;
			unsigned int  percent;	// Rate
			unsigned int  time;
		};

		// TwinsItemTable IFF
		struct TwinsItemTable {
			void clear() { memset(this, 0, sizeof(TwinsItemTable)); };
			unsigned int  active;
			unsigned int  type;
			unsigned int  _typeid[5];
		};

    }
}


#if defined(__linux__)
#pragma pack()
#endif

#endif // !_STDA_IFF_DATA_IFF_H
