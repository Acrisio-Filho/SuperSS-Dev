// Arquivo item_manager.cpp
// Criado em 240/02/2018 as 19:07 por Acrisio
// Implementação da classe item_manager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "item_manager.h"
#include "../../Projeto IOCP/UTIL/iff.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include <ctime>
#include <algorithm>

#include "../../Projeto IOCP/PANGYA_DB/cmd_add_character.hpp"

#include "../PANGYA_DB/cmd_personal_shop_log.hpp"
#include "../PANGYA_DB/cmd_transfer_part.hpp"

#include "../PANGYA_DB/cmd_add_caddie.hpp"
#include "../PANGYA_DB/cmd_add_character_hair_style.hpp"
#include "../PANGYA_DB/cmd_add_clubset.hpp"
#include "../PANGYA_DB/cmd_add_card.hpp"
#include "../PANGYA_DB/cmd_add_mascot.hpp"
#include "../PANGYA_DB/cmd_add_skin.hpp"
#include "../PANGYA_DB/cmd_add_ball.hpp"
#include "../PANGYA_DB/cmd_add_item.hpp"
#include "../PANGYA_DB/cmd_add_part.hpp"
#include "../PANGYA_DB/cmd_add_furniture.hpp"
#include "../PANGYA_DB/cmd_add_trofel_especial.hpp"

#include "../PANGYA_DB/cmd_delete_item.hpp"
#include "../PANGYA_DB/cmd_delete_ball.hpp"
#include "../PANGYA_DB/cmd_delete_card.hpp"
#include "../PANGYA_DB/cmd_delete_caddie.hpp"
#include "../PANGYA_DB/cmd_delete_furniture.hpp"
#include "../PANGYA_DB/cmd_delete_mascot.hpp"

#include "../PANGYA_DB/cmd_get_gift_clubset.hpp"
#include "../PANGYA_DB/cmd_get_gift_part.hpp"

#include "../PANGYA_DB/cmd_gift_clubset.hpp"
#include "../PANGYA_DB/cmd_gift_part.hpp"

#include "../PANGYA_DB/cmd_find_character.hpp"
#include "../PANGYA_DB/cmd_find_caddie.hpp"
#include "../PANGYA_DB/cmd_find_mascot.hpp"
#include "../PANGYA_DB/cmd_find_warehouse_item.hpp"
#include "../PANGYA_DB/cmd_find_card.hpp"
#include "../PANGYA_DB/cmd_find_furniture.hpp"
#include "../PANGYA_DB/cmd_find_dolfini_locker_item.hpp"
#include "../PANGYA_DB/cmd_find_trofel_especial.hpp"
#include "../PANGYA_DB/cmd_find_mail_box_item.hpp"

#include "../PANGYA_DB/cmd_ticket_report_dados_info.hpp"

#include "../PANGYA_DB/cmd_update_mascot_time.hpp"
#include "../PANGYA_DB/cmd_update_caddie_item.hpp"
#include "../PANGYA_DB/cmd_update_ball_qntd.hpp"
#include "../PANGYA_DB/cmd_update_card_qntd.hpp"
#include "../PANGYA_DB/cmd_update_item_qntd.hpp"
#include "../PANGYA_DB/cmd_update_character_all_part_equiped.hpp"
#include "../PANGYA_DB/cmd_update_ball_equiped.hpp"
#include "../PANGYA_DB/cmd_update_trofel_especial_qntd.hpp"
#include "../PANGYA_DB/cmd_update_premium_ticket_time.hpp"
#include "../PANGYA_DB/cmd_update_clubset_time.hpp"

#include "../../Projeto IOCP/PANGYA_DB/cmd_update_character_equiped.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PACKET/packet_func_sv.h"

#include "premium_system.hpp"

#include "../UTIL/block_memory_manager.hpp"

using namespace stdA;

#if defined(_WIN32)
#define INIT_COMMOM_BUYITEM memcpy_s(_item.name, sizeof(_item.name), item->name, sizeof(_item.name)); \
							memcpy_s(_item.icon, sizeof(_item.icon), item->icon, sizeof(_item.icon)); \
							_item.id = _bi.id; \
							_item._typeid = _bi._typeid; \
							_item.date = *(stItem::stDate*)&item->date; \
							_item.price = item->shop.price; \
							_item.desconto = item->shop.desconto; \
							_item.qntd = (uint32_t)_bi.qntd; \
							_item.is_cash = item->shop.flag_shop.uFlagShop.stFlagShop.is_cash; \
 \
							_item.type = 2; /*aqui é o valor padrão, mas outros iff pode mexer nele depois*/ 
#elif defined(__linux__)
#define INIT_COMMOM_BUYITEM memcpy(_item.name, item->name, sizeof(_item.name)); \
							memcpy(_item.icon, item->icon, sizeof(_item.icon)); \
							_item.id = _bi.id; \
							_item._typeid = _bi._typeid; \
							_item.date = *(stItem::stDate*)&item->date; \
							_item.price = item->shop.price; \
							_item.desconto = item->shop.desconto; \
							_item.qntd = (uint32_t)_bi.qntd; \
							_item.is_cash = item->shop.flag_shop.uFlagShop.stFlagShop.is_cash; \
 \
							_item.type = 2; /*aqui é o valor padrão, mas outros iff pode mexer nele depois*/ 
#endif

#define CHECK_LEVEL_ITEM { \
	if (!_gift_opt && !_chk_lvl && !item->level.goodLevel((unsigned char)_pi.level)) { \
		_smp::message_pool::getInstance().push(new message("[Log] Player[UID=" + std::to_string(_pi.uid)  \
				+ "] nao tem o level[value=" + std::to_string((unsigned short)item->level.level) + "] necessario para comprar esse item[TYPEID=" + std::to_string(item->_typeid) + "]", CL_FILE_LOG_AND_CONSOLE)); \
		_item._typeid = 0; \
		return; \
	} \
}

#define CHECK_IS_GIFT { \
	if (_gift_opt && !sIff::getInstance().IsGiftItem(item->_typeid)) { \
		_smp::message_pool::getInstance().push(new message("[Log] Player[UID=" + std::to_string(_pi.uid) + "] tentou presentear um item que não pode ser presenteado.", CL_FILE_LOG_AND_CONSOLE)); \
		_item._typeid = 0; \
		return; \
	} \
}

#define BEGIN_INIT_BUYITEM if (item != nullptr) { \
							CHECK_LEVEL_ITEM \
							CHECK_IS_GIFT \
							INIT_COMMOM_BUYITEM \


#define END_INIT_BUYITEM }else  _smp::message_pool::getInstance().push(new message("Item nao encontrado. Typeid: " + std::to_string(_bi._typeid), CL_FILE_LOG_AND_CONSOLE));

// Translate Value from Base Flag Time to Second, 1 = Dia, 0x20 and 2, 3 = Hour, 0x40 and 4 = Dia, 5 = 30Dias(mês), 0x60 and 6 = Day
#define STDA_TRANSLATE_FLAG_TIME(_flag_time, _value) ((_flag_time) >= 0x10 \
	? /* If true */ \
		((_flag_time) == 0x20/*Hour*/ || (_flag_time) == 3/*Hour*/ \
		? /* If true */ \
			(_value) * 60 * 60 \
		: /* else */ \
			((_flag_time) == 0x10/*Dia*/ || (_flag_time) == 0x40/*Dia*/ || (_flag_time) == 0x60/*Day*/ \
			? /* If true */ \
				(_value) * 24 * 60 * 60 \
			: /* else */ \
				((_flag_time) == 0x50/*30 Dias(Mês)*/ \
				?/*If true*/ (_value) * 30/*30 Dias Mês*/ * 24 * 60 * 60 \
				: /* else */ 0))) \
	: /* else */ \
		((_flag_time) == 2/*Hour*/ || (_flag_time) == 3/*Hour*/ \
		? /* If true */ \
			(_value) * 60 * 60 \
		: /* else */ \
			((_flag_time) == 1/*Dia*/ || (_flag_time) == 4/*Dia*/ || (_flag_time) == 6/*Day*/ \
			? /* If true */ \
				(_value) * 24 * 60 *60 \
			: /* else */ \
				((_flag_time) == 5/*30 Dias(Mês)*/ \
				? /* If true */ \
					(_value) * 30/*30 Dias Mês*/ * 24 * 60 * 60 \
				: /* else */ 0))) \
) \

// Translate Value from Base Flag Time to Hour
#define STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_flag_time, _value) (int)(STDA_TRANSLATE_FLAG_TIME((_flag_time), (_value)) / 60 / 60)

item_manager::item_manager() {

};

item_manager::~item_manager() {

};

std::vector< stItem > item_manager::getItemOfSetItem(player& _session, uint32_t _typeid, bool _shop, int _chk_level) {
	
	if (!isSetItem(_typeid))
		throw exception("[item_manager::getItemOfSetItem][Error] item[TYPEID=" + std::to_string(_typeid) 
			+ "] not is a valid SetItem. Player: " + std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 1, 0));

	std::vector< stItem > v_item;
	stItem item;
	BuyItem bi{ 0 };
	IFF::Base *base = nullptr;

	IFF::SetItem *set_item = sIff::getInstance().findSetItem(_typeid);

	if (set_item == nullptr)
		throw exception("[item_manager::getItemOfSetItem][Error] item[TYPEID=" + std::to_string(_typeid) 
			+ "] nao foi encontrado. Player: " + std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 2, 0));

	for (auto i = 0u; i < (sizeof(set_item->packege.item_typeid) / sizeof(set_item->packege.item_typeid[0])); ++i) {
		if (set_item->packege.item_typeid[i] != 0) {
			item.clear();
			bi.clear();

			/*item.id = ~0u;
			item._typeid = set_item->packege.item_typeid[i];
			item.qntd = set_item->packege.item_qntd[i];

			item.type = 2;

			// Is Commom Item
			if ((base = sIff::getInstance().findCommomItem(item._typeid)) != nullptr) {
				strcpy_s(item.name, base->name);
				strcpy_s(item.icon, base->icon);
				item.price = base->shop.price;
				item.desconto = base->shop.desconto;
				item.date = *(stItem::stDate*)&base->date;
				item.is_cash = base->shop.flag_shop.uFlagShop.IFF_cash;

				if (base->shop.flag_shop.time_shop.uTimeShop.IFF_time_limit == 1) {
					item.STDA_C_ITEM_TIME = (unsigned short)item.qntd;
					item.type = 0x20;	// Temporário, tenho que ver o tipo direito da hora, minuto, segundo e etc
				}
			}*/

			bi.id = -1;
			bi._typeid = set_item->packege.item_typeid[i];
			bi.qntd = set_item->packege.item_qntd[i];

			initItemFromBuyItem(_session.m_pi, item, bi, _shop, 0, 0, _chk_level);

			if (item._typeid != 0)
				v_item.push_back(item);
			else
				throw exception("[item_manager::getItemOfSetItem][Error] erro ao inicializar item[TYPEID=" 
					+ std::to_string(set_item->packege.item_typeid[i]) + "]. Player: " + std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 25, 0));
		}
	}

	return v_item;
};

void item_manager::initItemFromBuyItem(PlayerInfo& _pi, stItem& _item, BuyItem& _bi, bool _shop, int _option, int _gift_opt, int _chk_lvl) {

	// Limpa o _item
	_item.clear();

	switch (sIff::getInstance().getItemGroupIdentify(_bi._typeid)) {
	case iff::CHARACTER:
	{
		auto item = sIff::getInstance().findCharacter(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		END_INIT_BUYITEM;
		break;
	}
	case iff::PART:
	{
		auto item = sIff::getInstance().findPart(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		if (_option == 1/*Rental*/ && item->valor_rental > 0) {
			_item.price = item->valor_rental;
			_item.is_cash = 0;	// Pang, por que é rental
			_item.STDA_C_ITEM_TIME = 7;		// 7 dias	// no original é no C[3] o tempos
			_item.flag = 0x60;	// dias Rental(acho)
			// time tipo 6 rental, 4, 2,
			_item.flag_time = 6;
		}else if (_bi.time > 0) {	// Roupa de tempo do CadieCauldron

			if (item->shop.flag_shop.time_shop.active)
				_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][WARNIG] Player[UID=" + std::to_string(_pi.uid) 
						+ "] inicializou Part[TYPEID=" + std::to_string(_bi._typeid) + "] com tempo[VALUE=" + std::to_string(_bi.time)
						+ "], mas no IFF_STRUCT do server ele nao eh um item por tempo. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			_item.STDA_C_ITEM_TIME = _bi.time;
			_item.flag = 0x20;
			_item.flag_time = 2;
		}

		// UCC
		if (item->type_item == IFF::Part::UCC_BLANK)
			_item.flag = 5;

		_item.type_iff = (unsigned char)item->type_item;

		/*_smp::message_pool::getInstance().push(new message("Part Typeid: " + hex_util::lltoaToHex(item->_typeid), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("Part Type: " + std::to_string(item->type_item), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("Part Char Identify: " + std::to_string(sIff::getInstance().getItemCharIdentify(item->_typeid)), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("Part Number: " + std::to_string(sIff::getInstance().getItemCharPartNumber(item->_typeid)), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("Part Char Type Number: " + std::to_string(sIff::getInstance().getItemCharTypeNumber(item->_typeid)), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("Part Identify: " + std::to_string(sIff::getInstance().getItemIdentify(item->_typeid)), CL_FILE_LOG_AND_CONSOLE));*/

		END_INIT_BUYITEM;
		break;
	}
	case iff::CLUBSET:
	{
		auto item = sIff::getInstance().findClubSet(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		if (_bi.time > 0) {

			_item.qntd = 1;
			_item.flag = 0x20;
			_item.flag_time = 2/*Hora*/;
			_item.STDA_C_ITEM_TIME = _bi.time * 24;
		}

		END_INIT_BUYITEM;
		break;
	}
	case iff::BALL:
	{
		auto item = sIff::getInstance().findBall(_bi._typeid);

		BEGIN_INIT_BUYITEM;

#if defined(_WIN32)
		memcpy_s(_item.c, sizeof(_item.c), item->c, sizeof(_item.c));
#elif defined(__linux__)
		memcpy(_item.c, item->c, sizeof(_item.c));
#endif

		if (_chk_lvl)
			_item.STDA_C_ITEM_QNTD = (short)_item.qntd;
		else if (_item.qntd != item->STDA_C_ITEM_QNTD) {
			
			_item.STDA_C_ITEM_QNTD = (short)_item.qntd;

			_item.qntd = 1;

		}else {

			if (!_chk_lvl && _item.qntd > 0 && item->STDA_C_ITEM_QNTD != 0)
				_item.qntd /= item->STDA_C_ITEM_QNTD;

		}

		END_INIT_BUYITEM;
		break;
	}
	case iff::ITEM:
	{
		auto item = sIff::getInstance().findItem(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		// essa regra é para os itens que é 1 item, mas tem mais quantidade que vai ser add, 1 item normal, mas ele vem 10, fica no STDA_C_ITEM_QNTD
		if (_item.qntd > 0 && item->STDA_C_ITEM_QNTD != 0 && (item->STDA_C_ITEM_QNTD == 1 || item->STDA_C_ITEM_QNTD == _item.qntd))
			_item.qntd /= item->STDA_C_ITEM_QNTD;

		// Copia C[] do IFF::Item para o _item
#if defined(_WIN32)
		memcpy_s(_item.c, sizeof(_item.c), item->c, sizeof(_item.c));
#elif defined(__linux__)
		memcpy(_item.c, item->c, sizeof(_item.c));
#endif

		// Tem preço de tempo
		auto empty_price = EMPTY_ARRAY_PRICE(_item.c);

		if (_bi.time > 0 && !empty_price && sIff::getInstance().getEnchantSlotStat(_item._typeid) == 0x21/*Item por tempo, enchante é ~0xFC000000 >> 20*/) {

			if (item->shop.flag_shop.time_shop.active && item->shop.flag_shop.time_shop.dia > 0) {

				switch (_bi.time) {
				case 1:
					if (_item.is_cash ? _bi.cookie == _item.c[0] : _bi.pang == _item.c[0])
						_item.price = _item.c[0];
					break;
				case 7:
					if (_item.is_cash ? _bi.cookie == _item.c[1] : _bi.pang == _item.c[1])
						_item.price = _item.c[1];
					break;
				case 15:
					if (_item.is_cash ? _bi.cookie == _item.c[2] : _bi.pang == _item.c[2])
						_item.price = _item.c[2];
					break;
				case 30:
					if (_item.is_cash ? _bi.cookie == _item.c[3] : _bi.pang == _item.c[3])
						_item.price = _item.c[3];
					break;
				case 365:
					if (_item.is_cash ? _bi.cookie == _item.c[4] : _bi.pang == _item.c[4])
						_item.price = _item.c[4];
					break;
				default:
					_bi.time = 1; // 1 dia, Coloca o menor
					break;
				}

				_item.qntd = 1;//item->shop.flag_shop.time_shop.uc_time_start;
				_item.flag = 0x20;
				_item.flag_time = 2/*Hora*/;
				_item.STDA_C_ITEM_TIME = _bi.time * 24;	// Premium Ticket

				if (_bi.time > 365)
					_smp::message_pool::getInstance().push(new message("[WARNING] Player[UID=" + std::to_string(_pi.uid) + "]. Queria colocar mais[request=" 
							+ std::to_string(_bi.time) + "] que 365 dia na compra do Premium Ticket. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
			}
		
		}else if (_bi.time == 0 && !empty_price && sIff::getInstance().getEnchantSlotStat(_item._typeid) == 0x21/*Item por tempo, enchante é ~0xFC000000 >> 20*/ && _bi.qntd > 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][WARNING] Player[UID=" + std::to_string(_pi.uid)
					+ "] tentou inicializar Item[TYPEID=" + std::to_string(_bi._typeid) + "] sem tempo no jogo e no IFF_STRUCT ele tem tempo. Hacker ou Command GM.", CL_FILE_LOG_AND_CONSOLE));

			_bi.time = (unsigned short)(_bi.qntd > 365 ? 365 : _bi.qntd);

			// Qntd tem que ser 1 por que o item é por tempo
			if (_bi.qntd > 1)
				_item.qntd = _bi.qntd = 1;

			if (item->shop.flag_shop.time_shop.active && item->shop.flag_shop.time_shop.dia > 0) {

				switch (_bi.time) {
				case 1:
					if (_item.is_cash ? _bi.cookie == _item.c[0] : _bi.pang == _item.c[0])
						_item.price = _item.c[0];
					break;
				case 7:
					if (_item.is_cash ? _bi.cookie == _item.c[1] : _bi.pang == _item.c[1])
						_item.price = _item.c[1];
					break;
				case 15:
					if (_item.is_cash ? _bi.cookie == _item.c[2] : _bi.pang == _item.c[2])
						_item.price = _item.c[2];
					break;
				case 30:
					if (_item.is_cash ? _bi.cookie == _item.c[3] : _bi.pang == _item.c[3])
						_item.price = _item.c[3];
					break;
				case 365:
					if (_item.is_cash ? _bi.cookie == _item.c[4] : _bi.pang == _item.c[4])
						_item.price = _item.c[4];
					break;
				default:
					_bi.time = 1; // 1 dia, coloca o menor
					break;
				}

				_item.qntd = 1;//item->shop.flag_shop.time_shop.uc_time_start;
				_item.flag = 0x20;
				_item.flag_time = 2/*Hora*/;
				_item.STDA_C_ITEM_TIME = _bi.time * 24;	// Premium Ticket

				if (_bi.time > 365)
					_smp::message_pool::getInstance().push(new message("[WARNING] Player[UID=" + std::to_string(_pi.uid) + "]. Queria colocar mais[request="
							+ std::to_string(_bi.time) + "] que 365 dia na compra do Premium Ticket. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
			}
		}

		if (/*item->shop.flag_shop.uFlagShop.IFF_item_pack && */(item->_typeid == 0x1A000081 || item->_typeid == 0x1A000082 || item->_typeid == 0x1A0003B3/*Event gacha Sale Pack*/)) {
			_item._typeid = 0x1A000080;	// Typeid do Coupon Gacha Single, por que os outros é item pack, em um item
		}

		if (_item.STDA_C_ITEM_QNTD <= 0 || _item.STDA_C_ITEM_QNTD < (int)_item.qntd)
			_item.STDA_C_ITEM_QNTD = (unsigned short)_item.qntd;

		if ((!_shop || _item.qntd == 0) && _item.STDA_C_ITEM_QNTD != _bi.qntd) {
			_item.STDA_C_ITEM_QNTD = (unsigned short)_bi.qntd;
			_item.qntd = _bi.qntd;
		}

		if (sIff::getInstance().IsItemEquipable(_bi._typeid)) {	// Equiável
		}else {	// Passivo
			
		}

		END_INIT_BUYITEM;
		break;
	}
	case iff::CADDIE:
	{
		auto item = sIff::getInstance().findCaddie(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		if (item->valor_mensal > 0) {
			_item.date_reserve = 30;	// 30 dias
#if defined(_WIN32)
			memcpy_s(_item.c, sizeof(_item.c), item->c, sizeof(_item.c));
#elif defined(__linux__)
			memcpy(_item.c, item->c, sizeof(_item.c));
#endif
			_item.flag = 0x20;	// Time de dias( acho que seja o 0x20, não lembro mais)
			_item.flag_time = 2;

			_item.STDA_C_ITEM_TIME = _item.date_reserve;	// Caddie depois que add, só colocar o time novamente
		}

		END_INIT_BUYITEM;
		break;
	}
	case iff::CAD_ITEM:
	{
		auto item = sIff::getInstance().findCaddieItem(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		// Aqui não precisa ver se tem time_limit e time_start, so tem que verificar se tem o item->price[0~4]
		auto empty_price = EMPTY_ARRAY_PRICE(item->price);

		if (_bi.time > 0 && !empty_price) {
			
			switch (_bi.time) {	// Dias
			case 1:
				if (_item.is_cash ? _bi.cookie == item->price[0] : _bi.pang == item->price[0])
					_item.price = item->price[0];
				break;
			case 7:
				if (_item.is_cash ? _bi.cookie == item->price[1] : _bi.pang == item->price[1])
					_item.price = item->price[1];
				break;
			case 15:
				if (_item.is_cash ? _bi.cookie == item->price[2] : _bi.pang == item->price[2])
					_item.price = item->price[2];
				break;
			case 30:
				if (_item.is_cash ? _bi.cookie == item->price[3] : _bi.pang == item->price[3])
					_item.price = item->price[3];
				break;
			}

			_item.STDA_C_ITEM_QNTD = 1;// item->shop.flag_shop.time_shop.uc_time_start;
			_item.flag_time = 2;
			_item.flag = 0x20;
			_item.STDA_C_ITEM_TIME = _bi.time * 24;	// Horas
		
		}else if (_bi.time > 0 && (item->shop.flag_shop.time_shop.active || item->shop.flag_shop.time_shop.dia > 0)) {

			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][WARNING] Player[UID=" + std::to_string(_pi.uid) 
					+ "] inicializou Caddie Item[TYPEID=" + std::to_string(_bi._typeid) + "] com tempo no jogo e no IFF_STRUCT, mas ele nao tem os precos de tempo no IFF_STRUCT. Box ou Comando GM", CL_FILE_LOG_AND_CONSOLE));

			// Qntd tem que ser 1 por que o item é por tempo
			if (_bi.qntd > 1)
				_item.qntd = _bi.qntd = 1;

			_item.STDA_C_ITEM_QNTD = 1;// item->shop.flag_shop.time_shop.uc_time_start;
			_item.flag_time = 4;
			_item.flag = 0x40;
			_item.STDA_C_ITEM_TIME = _bi.time;	// Dias

		}else if (_bi.time == 0 && !empty_price && _bi.qntd > 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][WARNING] Player[UID=" + std::to_string(_pi.uid)
					+ "] tentou inicializar Caddie Item[TYPEID=" + std::to_string(_bi._typeid) + "] sem tempo no jogo e no IFF_STRUCT ele tem tempo. Hacker ou Command GM.", CL_FILE_LOG_AND_CONSOLE));

			_bi.time = (unsigned short)(_bi.qntd > 30 ? 30 : _bi.qntd);

			// Qntd tem que ser 1 por que o item é por tempo
			if (_bi.qntd > 1)
				_item.qntd = _bi.qntd = 1;
		
			switch (_bi.time) {	// Dias
			case 1:
				if (_item.is_cash ? _bi.cookie == item->price[0] : _bi.pang == item->price[0])
					_item.price = item->price[0];
				break;
			case 7:
				if (_item.is_cash ? _bi.cookie == item->price[1] : _bi.pang == item->price[1])
					_item.price = item->price[1];
				break;
			case 15:
				if (_item.is_cash ? _bi.cookie == item->price[2] : _bi.pang == item->price[2])
					_item.price = item->price[2];
				break;
			case 30:
				if (_item.is_cash ? _bi.cookie == item->price[3] : _bi.pang == item->price[3])
					_item.price = item->price[3];
				break;
			default:	// Não passou a quantidade de dias certo manda a soma de todos os preços que tem no iff
				_item.price = SUM_ARRAY_PRICE_ULONG(item->price);
				break;
			}

			_item.STDA_C_ITEM_QNTD = 1;// item->shop.flag_shop.time_shop.uc_time_start;
			_item.flag_time = 2;
			_item.flag = 0x20;
			_item.STDA_C_ITEM_TIME = _bi.time * 24;	// Horas

		}else if (_bi.time == 0 && !empty_price && _bi.qntd == 0) {
			
			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][Error] Player[UID=" + std::to_string(_pi.uid)
					+ "] tentou inicializar Caddie Item[TYPEID=" + std::to_string(_bi._typeid) + "] sem tempo e sem quantidade no jogo e no IFF_STRUCT ele tem tempo. Hacker ou Command GM.", CL_FILE_LOG_AND_CONSOLE));

			_item._typeid = 0u;

			return;
		}

		END_INIT_BUYITEM;
		break;
	}
	case iff::SET_ITEM:
	{
		auto item = sIff::getInstance().findSetItem(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		END_INIT_BUYITEM;
		break;
	}
	case iff::SKIN:
	{
		auto item = sIff::getInstance().findSkin(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		// ESSE AQUI É ONDE COMEÇA OS TEMPO, [1] É DO 1 A 365 DIAS, [7] É DO 7 A 365 DIAS
		//item->shop.flag_shop.time_shop.uc_time_start //---- AS SKINS É DO 7

		// Aqui não precisa ver se tem time_limit e time_start, so tem que verificar se tem o item->price[0~4]
		auto empty_price = EMPTY_ARRAY_PRICE(item->price);

		if (_bi.time > 0 && !empty_price) {
			
			switch (_bi.time) {	// Dias
			case 1:
				if (_item.is_cash ? _bi.cookie == item->price[0] : _bi.pang == item->price[0])
					_item.price = item->price[0];
				break;
			case 7:
				if (_item.is_cash ? _bi.cookie == item->price[1] : _bi.pang == item->price[1])
					_item.price = item->price[1];
				break;
			case 15:
				if (_item.is_cash ? _bi.cookie == item->price[2] : _bi.pang == item->price[2])
					_item.price = item->price[2];
				break;
			case 30:
				if (_item.is_cash ? _bi.cookie == item->price[3] : _bi.pang == item->price[3])
					_item.price = item->price[3];
				break;
			case 365:
				if (_item.is_cash ? _bi.cookie == item->price[4] : _bi.pang == item->price[4])
					_item.price = item->price[4];
				break;
			default:	// Não passou a quantidade de dias certo manda a soma de todos os preços que tem no iff
				_item.price = SUM_ARRAY_PRICE_ULONG(item->price);
				break;
			}

			_item.STDA_C_ITEM_QNTD = 1;
			_item.flag_time = 4;
			_item.flag = 0x20;
			_item.STDA_C_ITEM_TIME = _bi.time;	// Dias

		}else if (_bi.time > 0 && (item->shop.flag_shop.time_shop.active || item->shop.flag_shop.time_shop.dia > 0)) {

			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][WARNING] Player[UID=" + std::to_string(_pi.uid) 
					+ "] inicializou Skin[TYPEID=" + std::to_string(_bi._typeid) + "] com tempo no jogo e no IFF_STRUCT, mas ele nao tem os precos de tempo no IFF_STRUCT. Box ou Comando GM", CL_FILE_LOG_AND_CONSOLE));

			// Qntd tem que ser 1 por que o item é por tempo
			if (_bi.qntd > 1)
				_item.qntd = _bi.qntd = 1;

			_item.STDA_C_ITEM_QNTD = 1;// item->shop.flag_shop.time_shop.uc_time_start;
			_item.flag_time = 4;
			_item.flag = 0x40;
			_item.STDA_C_ITEM_TIME = _bi.time;	// Dias

		}else if (_bi.time == 0 && !empty_price && _bi.qntd > 0) {
			
			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][WARNING] Player[UID=" + std::to_string(_pi.uid) 
					+ "] tentou inicializar Skin[TYPEID=" + std::to_string(_bi._typeid) + "] sem tempo no jogo e no IFF_STRUCT ele tem tempo. Hacker ou Command GM.", CL_FILE_LOG_AND_CONSOLE));

			_bi.time = (unsigned short)(_bi.qntd > 365 ? 365 : _bi.qntd);

			// Qntd tem que ser 1 por que o item é por tempo
			if (_bi.qntd > 1)
				_item.qntd = _bi.qntd = 1;

			switch (_bi.time) {	// Dias
			case 1:
				if (_item.is_cash ? _bi.cookie == item->price[0] : _bi.pang == item->price[0])
					_item.price = item->price[0];
				break;
			case 7:
				if (_item.is_cash ? _bi.cookie == item->price[1] : _bi.pang == item->price[1])
					_item.price = item->price[1];
				break;
			case 15:
				if (_item.is_cash ? _bi.cookie == item->price[2] : _bi.pang == item->price[2])
					_item.price = item->price[2];
				break;
			case 30:
				if (_item.is_cash ? _bi.cookie == item->price[3] : _bi.pang == item->price[3])
					_item.price = item->price[3];
				break;
			case 365:
				if (_item.is_cash ? _bi.cookie == item->price[4] : _bi.pang == item->price[4])
					_item.price = item->price[4];
				break;
			default:	// Não passou a quantidade de dias certo manda a soma de todos os preços que tem no iff
				_item.price = SUM_ARRAY_PRICE_ULONG(item->price);
				break;
			}

			_item.STDA_C_ITEM_QNTD = 1;
			_item.flag_time = 4;
			_item.flag = 0x20;
			_item.STDA_C_ITEM_TIME = _bi.time;	// Dias
		
		}else if (_bi.time == 0 && !empty_price && _bi.qntd == 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][Error] Player[UID=" + std::to_string(_pi.uid)
					+ "] tentou inicializar Skin[TYPEID=" + std::to_string(_bi._typeid) + "] sem tempo e sem quantidade no jogo e no IFF_STRUCT ele tem tempo. Hacker ou Command GM.", CL_FILE_LOG_AND_CONSOLE));

			_item._typeid = 0u;

			return;
		}

		END_INIT_BUYITEM;
		break;
	}
	case iff::HAIR_STYLE:
	{
		auto item = sIff::getInstance().findHairStyle(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		END_INIT_BUYITEM;
		break;
	}
	case iff::MASCOT:
	{
		auto item = sIff::getInstance().findMascot(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		// Dias
		// Aqui não precisa ver se tem time_limit e time_start, so tem que verificar se tem o item->price[0~4]
		auto empty_price = EMPTY_ARRAY_PRICE(item->price);

		if (_bi.time > 0 && !empty_price) {
			
			switch (_bi.time) {
			case 1:
				if (_item.is_cash ? _bi.cookie == item->price[0] : _bi.pang == item->price[0])
					_item.price = item->price[0];
				break;
			case 7:
				if (_item.is_cash ? _bi.cookie == item->price[1] : _bi.pang == item->price[1])
					_item.price = item->price[1];
				break;
			case 15:
				if (_item.is_cash ? _bi.cookie == item->price[2] : _bi.pang == item->price[2])
					_item.price = item->price[2];
				break;
			case 30:
				if (_item.is_cash ? _bi.cookie == item->price[3] : _bi.pang == item->price[3])
					_item.price = item->price[3];
				break;
			case 360:
				if (_item.is_cash ? _bi.cookie == item->price[4] : _bi.pang == item->price[4])
					_item.price = item->price[4];
				break;
			}

			_item.STDA_C_ITEM_QNTD = 1;// item->shop.flag_shop.time_shop.uc_time_start;
			_item.flag_time = 4;
			_item.flag = 0x40;
			_item.STDA_C_ITEM_TIME = _bi.time;	// Dias
		
		}else if (_bi.time > 0 && (item->shop.flag_shop.time_shop.active || item->shop.flag_shop.time_shop.dia > 0)) {

			if (_shop && !sIff::getInstance().IsBuyItem(item->_typeid) && !sIff::getInstance().IsGiftItem(item->_typeid))
				_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][WARNING] Player[UID=" + std::to_string(_pi.uid) 
						+ "] inicializou Mascot[TYPEID=" + std::to_string(_bi._typeid) + "] com tempo no jogo e no IFF_STRUCT, mas ele nao tem os precos de tempo no IFF_STRUCT. Box ou Comando GM", CL_FILE_LOG_AND_CONSOLE));

			// Qntd tem que ser 1 por que o item é por tempo
			if (_bi.qntd > 1)
				_item.qntd = _bi.qntd = 1;

			_item.STDA_C_ITEM_QNTD = 1;// item->shop.flag_shop.time_shop.uc_time_start;
			_item.flag_time = 4;
			_item.flag = 0x40;
			_item.STDA_C_ITEM_TIME = _bi.time;	// Dias

		}else if (_bi.time == 0 && !empty_price && _bi.qntd > 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][WARNING] Player[UID=" + std::to_string(_pi.uid)
					+ "] tentou inicializar Mascot[TYPEID=" + std::to_string(_bi._typeid) + "] sem tempo no jogo e no IFF_STRUCT ele tem tempo. Hacker ou Command GM.", CL_FILE_LOG_AND_CONSOLE));

			_bi.time = (unsigned short)(_bi.qntd > 365 ? 365 : _bi.qntd);

			// Qntd tem que ser 1 por que o item é por tempo
			if (_bi.qntd > 1)
				_item.qntd = _bi.qntd = 1;

			switch (_bi.time) {
			case 1:
				if (_item.is_cash ? _bi.cookie == item->price[0] : _bi.pang == item->price[0])
					_item.price = item->price[0];
				break;
			case 7:
				if (_item.is_cash ? _bi.cookie == item->price[1] : _bi.pang == item->price[1])
					_item.price = item->price[1];
				break;
			case 15:
				if (_item.is_cash ? _bi.cookie == item->price[2] : _bi.pang == item->price[2])
					_item.price = item->price[2];
				break;
			case 30:
				if (_item.is_cash ? _bi.cookie == item->price[3] : _bi.pang == item->price[3])
					_item.price = item->price[3];
				break;
			case 365:
				if (_item.is_cash ? _bi.cookie == item->price[4] : _bi.pang == item->price[4])
					_item.price = item->price[4];
				break;
			default:	// Não passou a quantidade de dias certo manda a soma de todos os preços que tem no iff
				_item.price = SUM_ARRAY_PRICE_ULONG(item->price);
				break;
			}

			_item.STDA_C_ITEM_QNTD = 1;// item->shop.flag_shop.time_shop.uc_time_start;
			_item.flag_time = 4;
			_item.flag = 0x40;
			_item.STDA_C_ITEM_TIME = _bi.time;	// Dias

		}else if (_bi.time == 0 && !empty_price && _bi.qntd == 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::initItemFromBuyItem][Error] Player[UID=" + std::to_string(_pi.uid)
					+ "] tentou inicializar Mascot[TYPEID=" + std::to_string(_bi._typeid) + "] sem tempo e sem quantidade no jogo e no IFF_STRUCT ele tem tempo. Hacker ou Command GM.", CL_FILE_LOG_AND_CONSOLE));

			_item._typeid = 0u;

			return;
		}

		END_INIT_BUYITEM;
		break;
	}
	case iff::FURNITURE:
	{
		auto item = sIff::getInstance().findFurniture(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		if (_item.STDA_C_ITEM_QNTD == 0)
			_item.STDA_C_ITEM_QNTD = (short)_item.qntd;

		END_INIT_BUYITEM;
		break;
	}
	case iff::AUX_PART:
	{
		auto item = sIff::getInstance().findAuxPart(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		if (_item.STDA_C_ITEM_QNTD == 0)
			_item.STDA_C_ITEM_QNTD = (short)_item.qntd;

		END_INIT_BUYITEM;
		break;
	}
	case iff::CARD:
	{
		auto item = sIff::getInstance().findCard(_bi._typeid);

		BEGIN_INIT_BUYITEM;

		if (_item.STDA_C_ITEM_QNTD <= 0)
			_item.STDA_C_ITEM_QNTD = (short)_item.qntd;

		END_INIT_BUYITEM;
		break;
	}
	default:	// Não tem esse item para vender no shop
		_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_pi.uid) + "] Tentou comprar um item que nao tem no shop para vender. typeid: " + std::to_string(_bi._typeid), CL_FILE_LOG_AND_CONSOLE));

		//if (item != nullptr) {
		//	strcpy_s(_item.name, item->name);
		//	strcpy_s(_item.icon, item->icon);
		//	_item.id = _bi.id;
		//	_item._typeid = _bi._typeid;
		//	_item.date = *(stItem::stDate*)&item->date;
		//	_item.price = item->shop.price;
		//	_item.desconto = item->shop.desconto;
		//	_item.qntd = (unsigned short)_bi.qntd;
		//	_item.is_cash = item->shop.flag_shop.uFlagShop.IFF_cash;

		//	if (_bi.time > 0) {
		//		_item.STDA_C_ITEM_TIME = _bi.time;
		//		_item.type = 0x20;		// Temporário, tenho que ver o tipo direito da hora, minuto, segundo e etc
		//	}else
		//		_item.type = 2;
		//}
	}
};

void item_manager::initItemFromEmailItem(PlayerInfo& _pi, stItem& _item, EmailInfo::item& _ei_item) {
	
	BuyItem item{ 0 };

	item.id = _ei_item.id;
	item._typeid = _ei_item._typeid;
	item.qntd = _ei_item.qntd;
	item.time = (unsigned short)(_ei_item.flag_time == 2/*Hora*/ ? _ei_item.tempo_qntd / 24 : _ei_item.tempo_qntd);

	_item.flag_time = _ei_item.flag_time;

	// Aqui tem que criar o proprio dele por que tem o tipo do tempo[dias, horas, minutos, segundos] e etc
	initItemFromBuyItem(_pi, _item, item, false, 0, 0, 1/*não checar o level*/);
};

void item_manager::checkSetItemOnEmail(player& _session, EmailInfo& _ei) {

	if (_ei.itens.empty())
		throw exception("[item_manager::checkSetItemOnEmail][Error] email not have item for check", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));

	for (auto i = 0u; i < _ei.itens.size(); ++i) {
		if (item_manager::isSetItem(_ei.itens[i]._typeid)) {
			auto v_item = item_manager::getItemOfSetItem(_session, _ei.itens[i]._typeid, false, 1/*Não verifica o Level*/);

			if (!v_item.empty()) {

				for (auto& el : v_item)
					_ei.itens.push_back(EmailInfo::item{ -1, el._typeid, el.flag_time, (int)el.qntd, el.STDA_C_ITEM_TIME, 0, 0, 0/*flag GM*/, 0, "", 0 });

				_ei.itens.erase(_ei.itens.begin() + i--);
			}
		}
	}
};

item_manager::RetAddItem::TYPE item_manager::addItem(stItem& _item, uint32_t _uid, unsigned char _gift_flag, unsigned char _purchase, bool _dup) {
	
	#ifdef _DEBUG
	#define MSG_ACTIVE_DEBUG	CL_FILE_LOG_AND_CONSOLE
#else
	#define MSG_ACTIVE_DEBUG	CL_ONLY_FILE_LOG
#endif

#define STATIC_ADD_ITEM_SUCESS_MSG_LOG(__item) { \
	_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] Player[UID=" + std::to_string(_uid) + "] " + std::string((__item)) + "[Typeid=" \
			+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + ", QNTD=" + std::to_string((_item.STDA_C_ITEM_QNTD > 0 && _item.qntd <= 0xFFu ? _item.STDA_C_ITEM_QNTD : _item.qntd)) + "]", MSG_ACTIVE_DEBUG)); \
} \

	item_manager::RetAddItem::TYPE ret_id = item_manager::RetAddItem::T_ERROR;

	try {

		// Block Memória para o UID, para garantir que não vai adicionar itens simuntaneamente
		BlockMemoryManager::blockUID(_uid);

		// Error Grave lança uma excessa
		if (_uid == 0)
			throw exception("[item_manager::addItem][Error] uid invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 8, 0));

		if (_item._typeid == 0)
			throw exception("[item_manager::addItem][Error] item invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 9, 0));

		switch (sIff::getInstance().getItemGroupIdentify(_item._typeid)) {
		case iff::CHARACTER:
		{

			if (ownerItem(_uid, _item._typeid))
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] add um character[TYPEID=" 
							+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			CharacterInfo ce{ 0 };
			ce.id = _item.id;
			ce._typeid = _item._typeid;

			ce.initComboDef();

			// Aqui tem que add em uma fila e manda pra query e depois ver se foi concluida, enquanto verifica outras coisas[
			// E ESSA CLASSE NÃO PODE SER STATIC, POR QUE TEM QUE GUARDA UNS VALORE NECESSÁRIOS

			// Add no banco de dados
			CmdAddCharacter cmd_ac(_uid, ce, _purchase, 0/*_gift_flag*/, true);	// Waitable

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_ac, nullptr, nullptr);
		
			cmd_ac.waitEvent();

			if (cmd_ac.getException().getCodeError() != 0)
				throw cmd_ac.getException();

			ce = cmd_ac.getInfo();
			_item.id = ce.id;

			if (ce.id <= 0)
				throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o character[TYPEID=" + std::to_string(ce._typeid) + "] para o player: " 
						+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

			_item.STDA_C_ITEM_QNTD = 1;
			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = 1;
		
			ret_id = (RetAddItem::TYPE)ce.id;

			STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Character");

			break;
		}
		case iff::CADDIE:
		{

			if (ownerItem(_uid, _item._typeid))
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] tentou add um caddie[TYPEID="
						+ std::to_string(_item._typeid) + "] que ele ja possi.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			CaddieInfoEx ci{ 0 };
			ci.id = _item.id;
			ci._typeid = _item._typeid;
			ci.check_end = 1;	// Yes;
			ci.rent_flag = 1;	// 1 Normal sem ferias(tempo), 2 com ferias(tempo)

			if (_item.date_reserve > 0) {
				ci.rent_flag = 2;
				ci.end_date_unix = _item.date_reserve;//(_item.flag == 0x20/*dia*/) ? _item.STDA_C_ITEM_TIME : (_item.flag == 0x40/*minutos*/) ? _item.STDA_C_ITEM_TIME * 60 * 60 : _item.STDA_C_ITEM_TIME;
			}
		
			CmdAddCaddie cmd_ac(_uid, ci, _purchase, 0/*_gift_flag*/, true);	// Waitable

			snmdb::NormalManagerDB::getInstance().add(2, &cmd_ac, nullptr, nullptr);

			cmd_ac.waitEvent();

			if (cmd_ac.getException().getCodeError() != 0)
				throw cmd_ac.getException();

			ci = cmd_ac.getInfo();
			_item.id = ci.id;

			if (ci.id <= 0)
				throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o caddie[TYPEID=" + std::to_string(ci._typeid) + "] para o player: " 
						+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
		
			_item.STDA_C_ITEM_QNTD = 1;
			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = 1;
		
			ret_id = (RetAddItem::TYPE)ci.id;

			STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Caddie");

			break;
		}
		case iff::CAD_ITEM:
		{
			uint32_t cad_typeid = (iff::CADDIE << 26) | sIff::getInstance().getCaddieIdentify(_item._typeid);

			auto ci = _ownerCaddieItem(_uid, _item._typeid);

			if (!(ci.id > 0)/*!hasFound*/)
				throw exception("[itme_manager::addItem][Log] Player[UID=" + std::to_string(_uid)
						+ "] tentou comprar um caddie item[TYPEID=" + std::to_string(_item._typeid) + "] sem o caddie[TYPEID=" 
						+ std::to_string(cad_typeid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 14, 0));

			time_t end_date = 0;

			if (ci.parts_typeid == _item._typeid) {	// Já tem o parts caddie, atualiza o tempo
			
				// Adiciona o Tempo em Unix Time Stamp
				end_date = SystemTimeToUnix(ci.end_parts_date) + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME); //(((_item.flag_time == 2) ? _item.STDA_C_ITEM_TIME : _item.STDA_C_ITEM_TIME * 24) * 60 * 60);
				
				// Converte para System Time novamente
				ci.end_parts_date = UnixToSystemTime(end_date);

				// update Parts End Date Unix
				ci.updatePartsEndDate();

				_item.STDA_C_ITEM_TIME = (_item.flag_time == 2) ? ci.parts_end_date_unix : ci.parts_end_date_unix * 24;

				_item.id = ci.id;

				ret_id = (RetAddItem::TYPE)ci.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou Caddie Item");
				
			}else {

				// Não tem o caddie parts ainda, add
				ci.parts_typeid = _item._typeid;
				
				end_date = GetLocalTimeAsUnix() + ((ci.parts_end_date_unix = STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME)) * 60 * 60);

				// Converte para System Time novamente
				ci.end_parts_date = UnixToSystemTime(end_date);

				_item.id = ci.id;

				ret_id = (RetAddItem::TYPE)ci.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Caddie Item");
			}

			auto str_end_date = _formatDate(ci.end_parts_date);

			// Atualiza no para os 2 aqui
			snmdb::NormalManagerDB::getInstance().add(5, new CmdUpdateCaddieItem(_uid, str_end_date, ci), item_manager::SQLDBResponse, nullptr);

			break;
		}
		case iff::MASCOT:
		{
			auto mascot = sIff::getInstance().findMascot(_item._typeid);

			if (mascot == nullptr)
				throw exception("[item_manager::addItem][Erorr] mascot[TYPEID=" + std::to_string(_item._typeid)
						+ "] nao foi encontrado no IFF_STRUCT do server, para o player[UID=" + std::to_string(_uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			auto pMi = _ownerMascot(_uid, _item._typeid);
		
			if ((pMi.id > 0)/*hasFound*/) {	// Player já tem o mascot, só add mais tempo à ele

				if (mascot->shop.flag_shop.time_shop.active && _item.STDA_C_ITEM_TIME > 0) {

					time_t unix_time = SystemTimeToUnix(pMi.data);

					_item.stat.qntd_ant = (uint32_t)unix_time;

					unix_time += STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Local time for verify server on local time
					pMi.data = UnixUTCToTzLocalTime(unix_time);

					_item.stat.qntd_dep = (uint32_t)unix_time;

					_item.date.active = 1;

					// System Time Struct is Local Time
					GetLocalTime(&_item.date.date.sysDate[0]);

					_item.date.date.sysDate[1] = pMi.data;

					auto str_date = _formatDate(pMi.data);
				
					// Cmd update time mascot db
					snmdb::NormalManagerDB::getInstance().add(6, new CmdUpdateMascotTime(_uid, pMi.id, str_date), item_manager::SQLDBResponse, nullptr/*o item_manager é static*/);
				}

				_item.id = pMi.id;

				ret_id = (RetAddItem::TYPE)_item.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou o tempo[Dia(s)=" + std::to_string(_item.STDA_C_ITEM_TIME) + "] do Mascot");
			} else {
				MascotInfoEx mi{};
				mi.id = _item.id;
				mi._typeid = _item._typeid;
				mi.is_cash = _item.is_cash;
				mi.price = _item.price;
				mi.tipo = 0;	// Padrão, é os mascot que não tem tempo
#if defined(_WIN32)
				memcpy_s(mi.message, sizeof(mi.message), "PangYa SuperSS", 15);
#elif defined(__linux__)
				memcpy(mi.message, "PangYa SuperSS", 15);
#endif

				if (mascot->msg.active)
#if defined(_WIN32)
					memcpy_s(mi.message, sizeof(mi.message), "PangYa SuperSS!", 16);
#elif defined(__linux__)
					memcpy(mi.message, "PangYa SuperSS!", 16);
#endif

				if (mascot->shop.flag_shop.time_shop.active && _item.STDA_C_ITEM_TIME > 0)
					mi.tipo = 1;	// Mascot de Tempo

				CmdAddMascot cmd_am(_uid, mi, _item.STDA_C_ITEM_TIME, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_am, nullptr, nullptr);

				cmd_am.waitEvent();

				if (cmd_am.getException().getCodeError() != 0)
					throw cmd_am.getException();

				mi = cmd_am.getInfo();
				_item.id = mi.id;

				if (mi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Mascot[TYPEID=" + std::to_string(mi._typeid) + "] para o player: " 
							+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

				_item.STDA_C_ITEM_QNTD = 1;
				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = _item.qntd;

				if (mascot->shop.flag_shop.time_shop.active && _item.STDA_C_ITEM_TIME > 0) {

					time_t unix_time = GetSystemTimeAsUnix();

					_item.stat.qntd_ant = (uint32_t)unix_time;

					unix_time += STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Local time for verify server on local time
					mi.data = UnixUTCToTzLocalTime(unix_time);

					_item.stat.qntd_dep = (uint32_t)unix_time;

					_item.date.active = 1;

					// System Time Struct is Local Time
					GetLocalTime(&_item.date.date.sysDate[0]);

					_item.date.date.sysDate[1] = mi.data;
				}

				ret_id = (RetAddItem::TYPE)mi.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Mascot");
			}

			break;
		}
		case iff::BALL:
		{
			auto pWi = _ownerBall(_uid, _item._typeid);

			if ((pWi.id > 0)/*hasFound*/) {	// já tem atualiza quantidade
			
				_item.stat.qntd_ant = pWi.STDA_C_ITEM_QNTD;

				pWi.STDA_C_ITEM_QNTD += _item.STDA_C_ITEM_QNTD;
			
				_item.stat.qntd_dep = pWi.STDA_C_ITEM_QNTD;

				_item.id = ret_id = (RetAddItem::TYPE)pWi.id;

				snmdb::NormalManagerDB::getInstance().add(7, new CmdUpdateBallQntd(_uid, pWi.id, pWi.STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou Ball");
			}else {	// não tem, add

				WarehouseItemEx wi{ 0 };
				wi.id = _item.id;
				wi._typeid = _item._typeid;

				wi.type = _item.type;
				wi.flag = _item.flag;
#if defined(_WIN32)
				memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
				memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

				wi.ano = -1;

				CmdAddBall cmd_ab(_uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ab, nullptr, nullptr);

				cmd_ab.waitEvent();

				if (cmd_ab.getException().getCodeError() != 0)
					throw cmd_ab.getException();

				wi = cmd_ab.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Ball[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
							+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = wi.STDA_C_ITEM_QNTD;

				ret_id = (RetAddItem::TYPE)wi.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Ball");
			}

			break;
		}
		case iff::CLUBSET:
		{
			
			auto clubset = sIff::getInstance().findClubSet(_item._typeid);

			if (clubset == nullptr)
				throw exception("[item_manager::addItem][Error] clubset[TYPEID=" + std::to_string(_item._typeid)
						+ "] set nao foi encontrado no IFF_STRUCT do server, para o player: " + std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 12, 0));

			auto pWi = _ownerItem(_uid, _item._typeid);

			if ((pWi.id > 0)/*hasFound*/) { // Já tem, verifica se é de tempo e atualizar, se não dá error


				// Add Mais tempo no Club Set
				if (_item.STDA_C_ITEM_TIME > 0 && ((pWi.flag & 0x20) || (pWi.flag & 0x40) || (pWi.flag & 0x60)) && pWi.end_date_unix_local > 0) {

					_item.date.active = 1;

					// update ano (Horas) que o item ainda tem
					pWi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;

					// Só atualiza o Apply date se não tiver
					if (pWi.apply_date_unix_local == 0u) {

						pWi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

						// Convert to UTC to send client
						pWi.apply_date = TzLocalUnixToUnixUTC(pWi.apply_date_unix_local);
					}

					pWi.end_date_unix_local = (uint32_t)GetLocalTimeAsUnix() + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Convert to UTC to send client
					pWi.end_date = TzLocalUnixToUnixUTC(pWi.end_date_unix_local);

					// System Time Struct is Local Time
					_item.date.date.sysDate[0] = UnixToSystemTime(pWi.apply_date_unix_local);
					_item.date.date.sysDate[1] = UnixToSystemTime(pWi.end_date_unix_local);

					// Atualiza o tempo do ClubSet do player
					snmdb::NormalManagerDB::getInstance().add(20, new CmdUpdateClubSetTime(_uid, pWi), item_manager::SQLDBResponse, nullptr);

					_item.STDA_C_ITEM_QNTD = 1;
					_item.stat.qntd_ant = 0;
					_item.stat.qntd_dep = _item.qntd;

					_item.id = ret_id = (RetAddItem::TYPE)pWi.id;

					STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou ClubSet");

				}else
					throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] tentou add clubset[TYPEID="
							+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));
				
			}else {

				WarehouseItemEx wi{ 0 };

				wi.id = _item.id;
				wi._typeid = _item._typeid;
				wi.type = _item.type;
				wi.flag = _item.flag;

				if (_item.STDA_C_ITEM_TIME > 0)
					wi.STDA_C_ITEM_TIME = _item.STDA_C_ITEM_TIME;

				wi.clubset_workshop.level = clubset->work_shop.tipo;	// Cv 1 e etc

				if (wi.STDA_C_ITEM_TIME > 0)
					wi.STDA_C_ITEM_TIME /= 24; // converte de novo para Dias para salvar no banco de dados

				wi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;	// Aqui tem que colocar para minutos ou segundos(acho)

				if (_gift_flag && wi.id > 0) {
					CmdGetGiftClubSet cmd_ggcs(_uid, wi, true);	// Waiter

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_ggcs, nullptr, nullptr);

					cmd_ggcs.waitEvent();

					if (cmd_ggcs.getException().getCodeError() != 0)
						throw cmd_ggcs.getException();

					wi = cmd_ggcs.getInfo();
					_item.id = wi.id;

					if (wi.id <= 0)
						throw exception("[item_manager::addItem][Log] nao conseguiu pegar o presente de ClubSet[TYPEID=" + std::to_string(_item._typeid) + "] para o player[UID=" 
								+ std::to_string(_uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 13, 0));
				
				}else {
					CmdAddClubSet cmd_acs(_uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_acs, nullptr, nullptr);

					cmd_acs.waitEvent();

					if (cmd_acs.getException().getCodeError() != 0)
						throw cmd_acs.getException();

					wi = cmd_acs.getInfo();
					_item.id = wi.id;

					if (wi.id <= 0)
						throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o ClubSet[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
								+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
				
				}

				if (_item.STDA_C_ITEM_TIME > 0) {
					_item.date.active = 1;

					// Tenho que mexer nessas flags direitinho, por que aqui só está a de 0x60 e 0x20
					wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

					// Convert to UTC to send client
					wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

					wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Convert to UTC to send client
					wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

					// System Time Struct is Local Time
					_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
					_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);
				}

				_item.STDA_C_ITEM_QNTD = 1;
				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = _item.qntd;

				ret_id = (RetAddItem::TYPE)wi.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou ClubSet");
			}

			break;
		}
		case iff::CARD:
		{
			auto pCi = _ownerCard(_uid, _item._typeid);

			if ((pCi.id > 0)/*hasFound*/) {	// Já tem o item atualiza quantidade

				_item.stat.qntd_ant = pCi.qntd;
			
				pCi.qntd += _item.qntd;
			
				_item.stat.qntd_dep = pCi.qntd;

				_item.id = ret_id = (RetAddItem::TYPE)pCi.id;

				snmdb::NormalManagerDB::getInstance().add(8, new CmdUpdateCardQntd(_uid, pCi.id, pCi.qntd), item_manager::SQLDBResponse, nullptr);

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou Card");
			
			}else {

				CardInfo ci{ 0 };

				ci.id = _item.id;
				ci._typeid = _item._typeid;
				ci.qntd = _item.qntd;
				ci.type = 1;

				CmdAddCard cmd_ac(_uid, ci, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ac, nullptr, nullptr);

				cmd_ac.waitEvent();

				if (cmd_ac.getException().getCodeError() != 0)
					throw cmd_ac.getException();

				ci = cmd_ac.getInfo();
				_item.id = ci.id;

				if (ci.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Card[TYPEID=" + std::to_string(ci._typeid) + "] para o player: " 
							+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = _item.qntd;

				ret_id = (RetAddItem::TYPE)ci.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Card");
			}

			break;
		}
		case iff::FURNITURE:
		{
			// Tem que fazer esse aqui, por que pode vim por Set Item ou MailBox
			auto furniture = sIff::getInstance().findFurniture(_item._typeid);

			if (furniture == nullptr)
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] tentou add um Furniture[TYPEID="
						+ std::to_string(_item._typeid) + "] que nao existe no IFF_STRUCT do server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 12, 0));

			if (ownerItem(_uid, _item._typeid))
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) +"] tentou add um Furniture[TYPEID="
						+ std::to_string(_item._typeid) + "] que ele ja tem", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			MyRoomItem mri{ 0 };

			mri._typeid = _item._typeid;
			mri.id = _item.id;

			mri.location = *(MyRoomItem::Location*)&furniture->location;

			CmdAddFurniture cmd_af(_uid, mri, true);	// Waiter

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_af, nullptr, nullptr);

			cmd_af.waitEvent();

			if (cmd_af.getException().getCodeError() != 0)
				throw cmd_af.getException();

			mri = cmd_af.getInfo();
			_item.id = mri.id;

			if (mri.id <= 0)
				throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Furniture[TYPEID=" + std::to_string(mri._typeid) + "] para o player: " 
						+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = _item.STDA_C_ITEM_QNTD;

			ret_id = (RetAddItem::TYPE)mri.id;

			STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Furniture");

			break;
		}
		case iff::AUX_PART:
		{
			// Tem que fazer esse aqui, por que pode vim por Set Item ou MailBox
			//auto auxPart = sIff::getInstance().findAuxPart(_item._typeid);

			auto pWi = _ownerAuxPart(_uid, _item._typeid);

			if ((pWi.id > 0)/*hasFound*/) {

				if (!sIff::getInstance().IsCanOverlapped(pWi._typeid))
					throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] tentou add AuxPart[TYPEID="
							+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));
			
				_item.stat.qntd_ant = pWi.STDA_C_ITEM_QNTD;
			
				pWi.STDA_C_ITEM_QNTD += _item.STDA_C_ITEM_QNTD;
			
				_item.stat.qntd_dep = pWi.STDA_C_ITEM_QNTD;

				_item.id = ret_id = (RetAddItem::TYPE)pWi.id;

				snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateItemQntd(_uid, pWi.id, pWi.STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou AuxPart");
			}else {

				WarehouseItemEx wi{ 0 };
				wi.id = _item.id;
				wi._typeid = _item._typeid;

				wi.type = _item.type;
				wi.flag = _item.flag;
#if defined(_WIN32)
				memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
				memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

				if (wi.STDA_C_ITEM_TIME > 0)
					wi.STDA_C_ITEM_TIME /= 24; // converte de novo para Dias para salvar no banco de dados

				wi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;	// Aqui tem que colocar para minutos ou segundos(acho)

				CmdAddItem cmd_ai(_uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ai, nullptr, nullptr);

				cmd_ai.waitEvent();

				if (cmd_ai.getException().getCodeError() != 0)
					throw cmd_ai.getException();

				wi = cmd_ai.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o AuxPart[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
							+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
		
				if (_item.STDA_C_ITEM_TIME > 0) {
					_item.date.active = 1;

					wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

					// Convert to UTC to send client
					wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

					wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Convert to UTC to send client
					wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

					// System Time Struct is Local Time
					_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
					_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);
				}

				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = wi.STDA_C_ITEM_QNTD;

				ret_id = (RetAddItem::TYPE)wi.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou AuxPart");
			}

			break;
		}
		case iff::ITEM:
		{
			// CHECK FOR POUCH [PANG OR EXP]
			if (_item._typeid == PANG_POUCH_TYPEID/*Pang Pouch*/) {
			
				// Pang Pouch para o player
				PlayerInfo::addPang(_uid, (_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD);

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_uid) + "] Adicionou Pang Pouch. item[TYPEID=" 
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_FILE_LOG_AND_CONSOLE));
	#else
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_uid) + "] Adicionou Pang Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_ONLY_FILE_LOG));
	#endif

				// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
				BlockMemoryManager::unblockUID(_uid);

				return item_manager::RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;

			}else if (_item._typeid == EXP_POUCH_TYPEID/*Exp Pouch*/) {
			
				// Exp Pouch para o player
				player::addExp(_uid, (uint32_t)((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD));

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_uid) + "] Adicionou Exp Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_FILE_LOG_AND_CONSOLE));
	#else
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_uid) + "] Adicionou Exp Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_ONLY_FILE_LOG));
	#endif

				// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
				BlockMemoryManager::unblockUID(_uid);

				return item_manager::RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;

			}else if (_item._typeid == CP_POUCH_TYPEID/*Cookie Point Pouch*/) {

				// Log Ganhos de CP
				CPLog cp_log;

				cp_log.setType(CPLog::TYPE::CP_POUCH);

				cp_log.setCookie((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD);

				// Cookie Point(CP) Pouch para o player
				PlayerInfo::addCookie(_uid, (_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD);

				// Log de Ganhos de CP
				player::saveCPLog(_uid, cp_log);

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_uid) + "] Adicionou CP Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_FILE_LOG_AND_CONSOLE));
#else
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_uid) + "] Adicionou CP Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_ONLY_FILE_LOG));
#endif

				// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
				BlockMemoryManager::unblockUID(_uid);

				return RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			}
			// Fim check pouch

			auto pWi = _ownerItem(_uid, _item._typeid);

			// Ticket Report Sempre Add 1 Novo
			if ((pWi.id > 0)/*hasFound*/ && _item._typeid != TICKET_REPORT_SCROLL_TYPEID) {

				if (sPremiumSystem::getInstance().isPremiumTicket(pWi._typeid) && pWi.ano > 0)
					throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] tentou add Item[TYPEID="
							+ std::to_string(_item._typeid) + "] 'Premium Ticket' que ele ja possui, com tempo, tem que esperar acabar o tempo", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 15, 0));

				_item.stat.qntd_ant = pWi.STDA_C_ITEM_QNTD;
			
				pWi.STDA_C_ITEM_QNTD += _item.STDA_C_ITEM_QNTD;
			
				_item.stat.qntd_dep = pWi.STDA_C_ITEM_QNTD;

				_item.id = ret_id = (RetAddItem::TYPE)pWi.id;

				snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateItemQntd(_uid, pWi.id, pWi.STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou Item");
			}else {

				WarehouseItemEx wi{ 0 };
				wi.id = _item.id;
				wi._typeid = _item._typeid;

				wi.type = _item.type;
				wi.flag = _item.flag;
#if defined(_WIN32)
				memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
				memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

				if (wi.STDA_C_ITEM_TIME > 0)
					wi.STDA_C_ITEM_TIME /= 24; // converte de novo para Dias para salvar no banco de dados

				wi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;	// Aqui tem que colocar para minutos ou segundos(acho)

				CmdAddItem cmd_ai(_uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ai, nullptr, nullptr);

				cmd_ai.waitEvent();

				if (cmd_ai.getException().getCodeError() != 0)
					throw cmd_ai.getException();

				wi = cmd_ai.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Item[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
							+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
		
				if (_item.STDA_C_ITEM_TIME > 0) {
					_item.date.active = 1;

					wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

					// Convert to UTC to send client
					wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

					wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Convert to UTC to send client
					wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

					// System Time Struct is Local Time
					_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
					_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);
				}

				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = wi.STDA_C_ITEM_QNTD;

				ret_id = (RetAddItem::TYPE)wi.id;

				STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Item");
			}

			break;
		}
		case iff::SKIN:
		{

			if (ownerItem(_uid, _item._typeid))
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] tentou add Skin[TYPEID="
						+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			WarehouseItemEx wi{ 0 };
			wi.id = _item.id;
			wi._typeid = _item._typeid;
			wi.type = _item.type;
			wi.flag = _item.flag;

#if defined(_WIN32)
			memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
			memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

			CmdAddSkin cmd_as(_uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_as, nullptr, nullptr);

			cmd_as.waitEvent();

			if (cmd_as.getException().getCodeError() != 0)
				throw cmd_as.getException();

			wi = cmd_as.getInfo();
			_item.id = wi.id;

			if (wi.id <= 0)
				throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Skin[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
						+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

			if (_item.STDA_C_ITEM_TIME > 0) {
				_item.date.active = 1;

				wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

				// Convert to UTC to send client
				wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

				wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

				// Convert to UTC to send client
				wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

				// System Time Struct is Local Time
				_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
				_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);
			}

			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = _item.qntd;

			ret_id = (RetAddItem::TYPE)wi.id;

			STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Skin");

			break;
		}
		case iff::PART:
		{
			if (ownerItem(_uid, _item._typeid) && !sIff::getInstance().IsCanOverlapped(_item._typeid))
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] tentou add Part[TYPEID="
						+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			// Ainda falta as parte por tempo, aquelas do couldron de cor dourada
			WarehouseItemEx wi{ 0 };
			wi.id = _item.id;
			wi._typeid = _item._typeid;

			if (_item.type_iff == IFF::Part::UCC_BLANK || _item.type_iff == IFF::Part::UCC_COPY) {
				if (_item.ucc.IDX != nullptr)
#if defined(_WIN32)
					memcpy_s(wi.ucc.idx, sizeof(wi.ucc.idx), _item.ucc.IDX, sizeof(wi.ucc.idx));
#elif defined(__linux__)
					memcpy(wi.ucc.idx, _item.ucc.IDX, sizeof(wi.ucc.idx));
#endif
				wi.ucc.seq = (unsigned short)_item.ucc.seq;
				wi.ucc.status = (unsigned char)_item.ucc.status;
			}

			wi.type = _item.type;
			wi.flag = _item.flag;
#if defined(_WIN32)
			memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
			memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

			wi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;

			if (_gift_flag && wi.id > 0) {
				CmdGetGiftPart cmd_ggp(_uid, wi, _item.type_iff, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ggp, nullptr, nullptr);

				cmd_ggp.waitEvent();

				if (cmd_ggp.getException().getCodeError() != 0)
					throw cmd_ggp.getException();

				wi = cmd_ggp.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu pegar o presente de Part[TYPEID=" + std::to_string(_item._typeid) + "] para o player[UID=" 
							+ std::to_string(_uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 13, 0));
				
			}else {
				CmdAddPart cmd_ap(_uid, wi, _purchase, 0/*_gift_flag*/, _item.type_iff, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(3, &cmd_ap, nullptr, nullptr);

				cmd_ap.waitEvent();

				if (cmd_ap.getException().getCodeError() != 0)
					throw cmd_ap.getException();

				wi = cmd_ap.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Parts[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
							+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
				
			}

			if (_item.type_iff == IFF::Part::UCC_BLANK || _item.type_iff == IFF::Part::UCC_COPY) {
				if (wi.ucc.idx != nullptr)
#if defined(_WIN32)
					memcpy_s(_item.ucc.IDX, sizeof(_item.ucc.IDX), wi.ucc.idx, sizeof(_item.ucc.IDX));
#elif defined(__linux__)
					memcpy(_item.ucc.IDX, wi.ucc.idx, sizeof(_item.ucc.IDX));
#endif
				_item.ucc.seq = wi.ucc.seq;
				_item.ucc.status = wi.ucc.status;
			}

			_item.STDA_C_ITEM_QNTD = 1;
			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = 1;

			if (_item.STDA_C_ITEM_TIME > 0) {
				_item.date.active = 1;

				wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

				// Convert to UTC to send client
				wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

				wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

				// Convert to UTC to send client
				wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

				// System Time Struct is Local Time
				_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
				_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);

				// Qntd depois em segundos
				_item.stat.qntd_dep = (uint32_t)wi.end_date;

				if (_item.flag_time == 2)
					_item.STDA_C_ITEM_TIME *= 24;	// Horas
			}
		
			ret_id = (RetAddItem::TYPE)wi.id;

			STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Part" + (wi.ano > 0 && (wi.ano / 24) == 7/*Dias*/ && wi.flag == 0x60/*Rental*/ ? " Rental[Dia(s)=" + std::to_string(wi.STDA_C_ITEM_TIME) + "]" : std::string("")));

			break;
		}
		case iff::SET_ITEM:
			break;
		case iff::HAIR_STYLE:
		{
			auto hair = sIff::getInstance().findHairStyle(_item._typeid);

			if (hair != nullptr) {
				auto ce = _ownerHairStyle(_uid, _item._typeid);

				if ((ce.id > 0)/*hasFound*/) {	// Tem o Character
					
					ce.default_hair = hair->cor;

					snmdb::NormalManagerDB::getInstance().add(4, new CmdAddCharacterHairStyle(_uid, ce, _purchase, 0/*_gift_flag*/), item_manager::SQLDBResponse, nullptr);

					ret_id = (RetAddItem::TYPE)ce.id;

					STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou HairStyle");
				
				}else
					throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] nao tem esse character.", 
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 16, 0));

			}else
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_uid) + "] hairstyle[TYPEID=" 
						+ std::to_string(_item._typeid) + "] nao tem no IFF_STRUCT.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 17, 0));
		
			break;
		}
		case iff::COUNTER_ITEM:
			break;
		case iff::ACHIEVEMENT:
			break;
		case iff::MATCH:	// Troféu
		{
			auto type_trofel = sIff::getInstance().getItemSubGroupIdentify24(_item._typeid);

			// Troféu Espacial
			if (type_trofel == 1 || type_trofel == 2) {

				auto tsi = _ownerTrofelEspecial(_uid, _item._typeid);

				if (tsi.id > 0) {	// Já tem item add quantidade do Troféu especial

					_item.stat.qntd_ant = tsi.qntd;
			
					tsi.qntd += _item.STDA_C_ITEM_QNTD;
			
					_item.stat.qntd_dep = tsi.qntd;

					_item.id = ret_id = (RetAddItem::TYPE)tsi.id;

					snmdb::NormalManagerDB::getInstance().add(18, new CmdUpdateTrofelEspecialQntd(_uid, tsi.id, tsi.qntd, CmdUpdateTrofelEspecialQntd::eTYPE::ESPECIAL), item_manager::SQLDBResponse, nullptr);

					STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou Trofel Especial");
				}else {

					TrofelEspecialInfo ts{ 0 };
					ts.id = _item.id;
					ts._typeid = _item._typeid;
					ts.qntd = _item.STDA_C_ITEM_QNTD;

					CmdAddTrofelEspecial cmd_ts(_uid, ts, CmdAddTrofelEspecial::eTYPE::ESPECIAL, true);	// Waiter

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_ts, nullptr, nullptr);

					cmd_ts.waitEvent();

					if (cmd_ts.getException().getCodeError() != 0)
						throw cmd_ts.getException();

					ts = cmd_ts.getInfo();
					_item.id = ts.id;

					if (ts.id <= 0)
						throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Trofel Especial[TYPEID=" + std::to_string(ts._typeid) + "] para o player: " 
								+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

					_item.stat.qntd_ant = 0;
					_item.stat.qntd_dep = ts.qntd;

					ret_id = (RetAddItem::TYPE)ts.id;

					STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Trofel Especial");
				}
			
			}else if (type_trofel == 3) {	// Grand Prix

				auto tsi = _ownerTrofelEspecial(_uid, _item._typeid);

				if (tsi.id > 0) {	// Já tem item add quantidade do Troféu Grand Prix

					_item.stat.qntd_ant = tsi.qntd;
			
					tsi.qntd += _item.STDA_C_ITEM_QNTD;
			
					_item.stat.qntd_dep = tsi.qntd;

					_item.id = ret_id = (RetAddItem::TYPE)tsi.id;

					snmdb::NormalManagerDB::getInstance().add(18, new CmdUpdateTrofelEspecialQntd(_uid, tsi.id, tsi.qntd, CmdUpdateTrofelEspecialQntd::eTYPE::GRAND_PRIX), item_manager::SQLDBResponse, nullptr);

					STATIC_ADD_ITEM_SUCESS_MSG_LOG("Atualizou Trofel Grand Prix");
				}else {

					TrofelEspecialInfo ts{ 0 };
					ts.id = _item.id;
					ts._typeid = _item._typeid;
					ts.qntd = _item.STDA_C_ITEM_QNTD;

					CmdAddTrofelEspecial cmd_ts(_uid, ts, CmdAddTrofelEspecial::eTYPE::GRAND_PRIX, true);	// Waiter

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_ts, nullptr, nullptr);

					cmd_ts.waitEvent();

					if (cmd_ts.getException().getCodeError() != 0)
						throw cmd_ts.getException();

					ts = cmd_ts.getInfo();
					_item.id = ts.id;

					if (ts.id <= 0)
						throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Trofel Grand Prix[TYPEID=" + std::to_string(ts._typeid) + "] para o player: " 
								+ std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

					_item.stat.qntd_ant = 0;
					_item.stat.qntd_dep = ts.qntd;

					ret_id = (RetAddItem::TYPE)ts.id;

					STATIC_ADD_ITEM_SUCESS_MSG_LOG("Adicionou Trofel Grand Prix");
				}
			}

			break;
		}	// End iff::MATCH
		}	// End Switch

		// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
		BlockMemoryManager::unblockUID(_uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[item_manager::addItem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
		BlockMemoryManager::unblockUID(_uid);

		ret_id = RetAddItem::T_ERROR;
	}

	return ret_id;
}

item_manager::RetAddItem item_manager::addItem(std::vector< stItem >& _v_item, uint32_t _uid, unsigned char _gift_flag, unsigned char _purchase, bool _dup) {
	
	RetAddItem rai{ 0 };
	RetAddItem::TYPE type = RetAddItem::T_INIT_VALUE;

	for (auto it = _v_item.begin(); it != _v_item.end(); /*Incrementa no Else*/) {

		if ((type = addItem(*it, _uid, _gift_flag, _purchase, _dup)) <= 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] player[UID=" + std::to_string(_uid) + "] tentou adicionar o item[TYPEID=" 
					+ std::to_string(it->_typeid) + ", ID=" + std::to_string(it->id) + "], " 
					+ ((type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) ? "mas era pang, exp ou CP pouch" : "mas nao conseguiu.Bug"), CL_FILE_LOG_AND_CONSOLE));

			rai.fails.push_back(*it);

			it = _v_item.erase(it);

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = type;
			else if (type == RetAddItem::T_ERROR) {

				if (rai.type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
				else if (rai.type != RetAddItem::TR_SUCCESS_WITH_ERROR && rai.type != RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR)
					rai.type = type;

			}else if (type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) {
				
				if (rai.type == RetAddItem::T_ERROR || rai.type == RetAddItem::TR_SUCCESS_WITH_ERROR)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			}

		}else {

			// Incrementa
			++it;

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = RetAddItem::T_SUCCESS;
			else if (rai.type == RetAddItem::T_ERROR)
				rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
		}
	}

	return rai;
}

item_manager::RetAddItem item_manager::addItem(std::vector< stItemEx >& _v_item, uint32_t _uid, unsigned char _gift_flag, unsigned char _purchase, bool _dup) {

	RetAddItem rai{ 0 };
	RetAddItem::TYPE type = RetAddItem::T_INIT_VALUE;

	for (auto it = _v_item.begin(); it != _v_item.end(); /*Incrementa no Else*/) {

		if ((type = addItem(*it, _uid, _gift_flag, _purchase, _dup)) <= 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] player[UID=" + std::to_string(_uid) + "] tentou adicionar o item[TYPEID=" 
					+ std::to_string(it->_typeid) + ", ID=" + std::to_string(it->id) + "], " 
					+ ((type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) ? "mas era pang, exp ou CP pouch" : "mas nao conseguiu.Bug"), CL_FILE_LOG_AND_CONSOLE));

			rai.fails.push_back(*it);

			it = _v_item.erase(it);

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = type;
			else if (type == RetAddItem::T_ERROR) {

				if (rai.type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
				else if (rai.type != RetAddItem::TR_SUCCESS_WITH_ERROR && rai.type != RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR)
					rai.type = type;

			}else if (type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) {
				
				if (rai.type == RetAddItem::T_ERROR || rai.type == RetAddItem::TR_SUCCESS_WITH_ERROR)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			}

		}else {

			// Incrementa
			++it;

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = RetAddItem::T_SUCCESS;
			else if (rai.type == RetAddItem::T_ERROR)
				rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
		}
	}

	return rai;
}

item_manager::RetAddItem::TYPE item_manager::addItem(stItem& _item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup) {

#ifdef _DEBUG
	#define MSG_ACTIVE_DEBUG	CL_FILE_LOG_AND_CONSOLE
#else
	#define MSG_ACTIVE_DEBUG	CL_ONLY_FILE_LOG
#endif

#define ADD_ITEM_SUCESS_MSG_LOG(__item) { \
	_smp::message_pool::getInstance().push(new message("[Pangya addItem][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] " + std::string((__item)) + "[Typeid=" \
			+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + ", QNTD=" + std::to_string((_item.STDA_C_ITEM_QNTD > 0 && _item.qntd <= 0xFFu ? _item.STDA_C_ITEM_QNTD : _item.qntd)) + "]", MSG_ACTIVE_DEBUG)); \
} \

	RetAddItem::TYPE ret_id = RetAddItem::T_ERROR;

	try {

		// Block Memória para o UID, para garantir que não vai adicionar itens simuntaneamente
		BlockMemoryManager::blockUID(_session.m_pi.uid);

		// Error Grave lança uma exception
		if (!_session.getState())
			throw exception("[item_manager::addItem][Error] session nao esta conectada.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 8, 0));

		if (_item._typeid == 0)
			throw exception("[item_manager::addItem][Error] item invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 9, 0));

		switch (sIff::getInstance().getItemGroupIdentify(_item._typeid)) {
		case iff::CHARACTER:
		{
			auto pCi = _session.m_pi.findCharacterByTypeid(_item._typeid);

			if (pCi != nullptr)
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] add um character[TYPEID=" 
							+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			CharacterInfo ce{ 0 };
			ce.id = _item.id;
			ce._typeid = _item._typeid;

			ce.initComboDef();

			// Aqui tem que add em uma fila e manda pra query e depois ver se foi concluida, enquanto verifica outras coisas[
			// E ESSA CLASSE NÃO PODE SER STATIC, POR QUE TEM QUE GUARDA UNS VALORE NECESSÁRIOS

			// Add no banco de dados
			CmdAddCharacter cmd_ac(_session.m_pi.uid, ce, _purchase, 0/*_gift_flag*/, true);	// Waitable

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_ac, nullptr, nullptr);
		
			cmd_ac.waitEvent();

			if (cmd_ac.getException().getCodeError() != 0)
				throw cmd_ac.getException();

			ce = cmd_ac.getInfo();
			_item.id = ce.id;

			if (ce.id <= 0)
				throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o character[TYPEID=" + std::to_string(ce._typeid) + "] para o player: " 
						+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

			_item.STDA_C_ITEM_QNTD = 1;
			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = 1;

			// Add List Item ON Server
			auto it_char = _session.m_pi.mp_ce.insert(std::make_pair(ce.id, ce));

			// Atualiza character equipado
			_session.m_pi.ue.character_id = it_char->second.id;
			_session.m_pi.ei.char_info = &it_char->second;

			snmdb::NormalManagerDB::getInstance().add(17, new CmdUpdateCharacterEquiped(_session.m_pi.uid, _session.m_pi.ue.character_id), item_manager::SQLDBResponse, nullptr);
		
			ret_id = (RetAddItem::TYPE)ce.id;

			ADD_ITEM_SUCESS_MSG_LOG("Adicionou Character");

			break;
		}
		case iff::CADDIE:
		{
			auto pCi = _session.m_pi.findCaddieByTypeid(_item._typeid);

			if (pCi != nullptr)
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add um caddie[TYPEID="
						+ std::to_string(_item._typeid) + "] que ele ja possi.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			CaddieInfoEx ci{ 0 };
			ci.id = _item.id;
			ci._typeid = _item._typeid;
			ci.check_end = 1;	// Yes;
			ci.rent_flag = 1;	// 1 Normal sem ferias(tempo), 2 com ferias(tempo)

			if (_item.date_reserve > 0) {
				ci.rent_flag = 2;
				ci.end_date_unix = _item.date_reserve;//(_item.flag == 0x20/*dia*/) ? _item.STDA_C_ITEM_TIME : (_item.flag == 0x40/*minutos*/) ? _item.STDA_C_ITEM_TIME * 60 * 60 : _item.STDA_C_ITEM_TIME;
			}
		
			CmdAddCaddie cmd_ac(_session.m_pi.uid, ci, _purchase, 0/*_gift_flag*/, true);	// Waitable

			snmdb::NormalManagerDB::getInstance().add(2, &cmd_ac, nullptr, nullptr);

			cmd_ac.waitEvent();

			if (cmd_ac.getException().getCodeError() != 0)
				throw cmd_ac.getException();

			ci = cmd_ac.getInfo();
			_item.id = ci.id;

			if (ci.id <= 0)
				throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o caddie[TYPEID=" + std::to_string(ci._typeid) + "] para o player: " 
						+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
			
		
			_item.STDA_C_ITEM_QNTD = 1;
			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = 1;

			// Add List Item ON Server
			_session.m_pi.mp_ci.insert(std::make_pair(ci.id, ci));
		
			ret_id = (RetAddItem::TYPE)ci.id;

			ADD_ITEM_SUCESS_MSG_LOG("Adicionou Caddie");

			break;
		}
		case iff::CAD_ITEM:
		{
			uint32_t cad_typeid = (iff::CADDIE << 26) | sIff::getInstance().getCaddieIdentify(_item._typeid);

			auto ci = _session.m_pi.findCaddieByTypeid(cad_typeid);

			if (ci == nullptr)
				throw exception("[item_manager::addItem][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar um caddie item[TYPEID=" 
						+ std::to_string(_item._typeid) + "] sem o caddie[TYPEID=" + std::to_string(cad_typeid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 14, 0));

			time_t end_date = 0;

			if (ci->parts_typeid == _item._typeid) {	// Já tem o parts caddie, atualiza o tempo
			
				end_date = SystemTimeToUnix(ci->end_parts_date) + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME); //(((_item.flag_time == 2) ? _item.STDA_C_ITEM_TIME : _item.STDA_C_ITEM_TIME * 24) * 60 * 60);

				// Converte para System Time novamente
				ci->end_parts_date = UnixToSystemTime(end_date);

				// Update Parts End Date Unix
				ci->updatePartsEndDate();

				// Update Time To Send To Client
				_item.STDA_C_ITEM_TIME = (_item.flag_time == 2) ? ci->parts_end_date_unix : ci->parts_end_date_unix * 24;

				_item.id = ci->id;

				ret_id = (RetAddItem::TYPE)ci->id;

				ADD_ITEM_SUCESS_MSG_LOG("Atualizou Caddie Item");

			}else {

				// Não tem o caddie parts ainda, add
				ci->parts_typeid = _item._typeid;

				end_date = GetLocalTimeAsUnix() + ((ci->parts_end_date_unix = STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME)) * 60 * 60);

				// Converte para System Time novamente
				ci->end_parts_date = UnixToSystemTime(end_date);

				_item.id = ci->id;

				ret_id = (RetAddItem::TYPE)ci->id;

				ADD_ITEM_SUCESS_MSG_LOG("Adicionou Caddie Item");
			}

			// Verifica se o Caddie já tem um item update do parts do caddie, por que se tiver, 
			// ele vai desequipar esse parts novo por que o player não relogou quando acabou o tempo do caddie parts
			auto v_it = _session.m_pi.findUpdateItemByTypeidAndId(ci->_typeid, ci->id);

			if (!v_it.empty()) {

				for (auto& el : v_it)
					if (el->second.type == UpdateItem::CADDIE_PARTS)
						// Tira esse Update Item do map
						_session.m_pi.mp_ui.erase(el);

			}
			// ---- fim do verifica se o caddie tem parts no update item ----

			auto str_end_date = formatDateLocal(end_date);

			// Atualiza no para os 2 aqui
			snmdb::NormalManagerDB::getInstance().add(5, new CmdUpdateCaddieItem(_session.m_pi.uid, str_end_date, *ci), item_manager::SQLDBResponse, nullptr);

			break;
		}
		case iff::MASCOT:
		{
			auto mascot = sIff::getInstance().findMascot(_item._typeid);

			if (mascot == nullptr)
				throw exception("[item_manager::addItem][Erorr] mascot[TYPEID=" + std::to_string(_item._typeid)
						+ "] nao foi encontrado no IFF_STRUCT do server, para o player[UID=" + std::to_string(_session.m_pi.uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			auto pMi = _session.m_pi.findMascotByTypeid(_item._typeid);
		
			if (pMi != nullptr) {	// Player já tem o mascot, só add mais tempo à ele

				if (mascot->shop.flag_shop.time_shop.active && _item.STDA_C_ITEM_TIME > 0) {

					//time_t unix_time = SystemTimeToUnix(pMi->data);
					time_t unix_time = TzLocalTimeToUnixUTC(pMi->data);

					_item.stat.qntd_ant = (uint32_t)unix_time;

					unix_time += STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Local Time for verify on server on local time
					pMi->data = UnixUTCToTzLocalTime(unix_time);

					_item.stat.qntd_dep = (uint32_t)unix_time;

					_item.date.active = 1;

					// System Time Struct is Local Time
					GetLocalTime(&_item.date.date.sysDate[0]);

					_item.date.date.sysDate[1] = pMi->data;

					auto str_date = _formatDate(pMi->data);
				
					// Cmd update time mascot db
					snmdb::NormalManagerDB::getInstance().add(6, new CmdUpdateMascotTime(_session.m_pi.uid, pMi->id, str_date), item_manager::SQLDBResponse, nullptr/*o item_manager é static*/);
				}

				_item.id = pMi->id;

				ret_id = (RetAddItem::TYPE)_item.id;

				ADD_ITEM_SUCESS_MSG_LOG("Atualizou o tempo[Dia(s)=" + std::to_string(_item.STDA_C_ITEM_TIME) + "] do Mascot");

				// Verifica se o Mascot está no item update, por que se tiver, 
				// ele vai desequipar esse mascot por que o player não relogou quando acabou o tempo do Mascot
				auto v_it = _session.m_pi.findUpdateItemByTypeidAndId(pMi->_typeid, pMi->id);

				if (!v_it.empty()) {

					for (auto& el : v_it)
						if (el->second.type == UpdateItem::UI_TYPE::MASCOT)
							// Tira esse Update Item do map
							_session.m_pi.mp_ui.erase(el);

				}
				// ---- fim do verifica se o Mascot está no update item ----
			
			}else {
			
				MascotInfoEx mi{};
				mi.id = _item.id;
				mi._typeid = _item._typeid;
				mi.is_cash = _item.is_cash;
				mi.price = _item.price;
				mi.tipo = 0;	// Padrão, é os mascot que não tem tempo
#if defined(_WIN32)
				memcpy_s(mi.message, sizeof(mi.message), "PangYa SuperSS", 15);
#elif defined(__linux__)
				memcpy(mi.message, "PangYa SuperSS", 15);
#endif

				if (mascot->msg.active)
#if defined(_WIN32)
					memcpy_s(mi.message, sizeof(mi.message), "PangYa SuperSS!", 16);
#elif defined(__linux__)
					memcpy(mi.message, "PangYa SuperSS!", 16);
#endif

				if (mascot->shop.flag_shop.time_shop.active && _item.STDA_C_ITEM_TIME > 0)
					mi.tipo = 1;	// Mascot de Tempo

				CmdAddMascot cmd_am(_session.m_pi.uid, mi, _item.STDA_C_ITEM_TIME, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_am, nullptr, nullptr);

				cmd_am.waitEvent();

				if (cmd_am.getException().getCodeError() != 0)
					throw cmd_am.getException();

				mi = cmd_am.getInfo();
				_item.id = mi.id;

				if (mi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Mascot[TYPEID=" + std::to_string(mi._typeid) + "] para o player: " 
							+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

				_item.STDA_C_ITEM_QNTD = 1;
				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = _item.qntd;

				if (mascot->shop.flag_shop.time_shop.active && _item.STDA_C_ITEM_TIME > 0) {

					time_t unix_time = GetSystemTimeAsUnix();

					_item.stat.qntd_ant = (uint32_t)unix_time;

					unix_time += STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Local time for verify server on local time
					mi.data = UnixUTCToTzLocalTime(unix_time);

					_item.stat.qntd_dep = (uint32_t)unix_time;

					_item.date.active = 1;
					
					// System Time Struct is Local Time
					GetLocalTime(&_item.date.date.sysDate[0]);

					_item.date.date.sysDate[1] = mi.data;
				
				}else // O Mascot não é por tempo, mas precisa do end data, por que é a mesma da data que ele foi adicionado no banco de dados, precisa na hora de atualizar o mascot
					GetLocalTime(&mi.data);

				// Add List Item ON Server
				_session.m_pi.mp_mi.insert(std::make_pair(mi.id, mi));

				ret_id = (RetAddItem::TYPE)mi.id;

				ADD_ITEM_SUCESS_MSG_LOG("Adicionou Mascot");
			}

			break;
		}
		case iff::BALL:
		{
			auto pWi = _session.m_pi.findWarehouseItemByTypeid(_item._typeid);

			if (pWi != nullptr) {	// já tem atualiza quantidade
			
				_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

				pWi->STDA_C_ITEM_QNTD += _item.STDA_C_ITEM_QNTD;
			
				_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

				_item.id = ret_id = (RetAddItem::TYPE)pWi->id;

				snmdb::NormalManagerDB::getInstance().add(7, new CmdUpdateBallQntd(_session.m_pi.uid, pWi->id, pWi->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

				ADD_ITEM_SUCESS_MSG_LOG("Atualizou Ball");
			}else {	// não tem, add

				WarehouseItemEx wi{ 0 };
				wi.id = _item.id;
				wi._typeid = _item._typeid;

				wi.type = _item.type;
				wi.flag = _item.flag;
#if defined(_WIN32)
				memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
				memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

				wi.ano = -1;

				CmdAddBall cmd_ab(_session.m_pi.uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ab, nullptr, nullptr);

				cmd_ab.waitEvent();

				if (cmd_ab.getException().getCodeError() != 0)
					throw cmd_ab.getException();

				wi = cmd_ab.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Ball[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
							+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = wi.STDA_C_ITEM_QNTD;

				// Add List Item ON Server
				_session.m_pi.mp_wi.insert(std::make_pair(wi.id, wi));

				ret_id = (RetAddItem::TYPE)wi.id;

				ADD_ITEM_SUCESS_MSG_LOG("Adicionou Ball");
			}

			break;
		}
		case iff::CLUBSET:
		{

			auto clubset = sIff::getInstance().findClubSet(_item._typeid);

			if (clubset == nullptr)
				throw exception("[item_manager::addItem][Error] clubset[TYPEID=" + std::to_string(_item._typeid)
						+ "] set nao foi encontrado no IFF_STRUCT do server, para o player: " + std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 12, 0));

			auto pWi = _session.m_pi.findWarehouseItemByTypeid(_item._typeid);

			if (pWi != nullptr) {

				// Add Mais tempo no Club Set
				if (_item.STDA_C_ITEM_TIME > 0 && ((pWi->flag & 0x20) || (pWi->flag & 0x40) || (pWi->flag & 0x60)) && pWi->end_date_unix_local > 0) {

					_item.date.active = 1;

					// update ano (Horas) que o item ainda tem
					pWi->ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;

					// Só atualiza o Apply date se não tiver
					if (pWi->apply_date_unix_local == 0u) {

						pWi->apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

						// Convert to UTC to send client
						pWi->apply_date = TzLocalUnixToUnixUTC(pWi->apply_date_unix_local);
					}

					pWi->end_date_unix_local = (uint32_t)GetLocalTimeAsUnix() + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Convert to UTC to send client
					pWi->end_date = TzLocalUnixToUnixUTC(pWi->end_date_unix_local);

					// System Time Struct is Local Time
					_item.date.date.sysDate[0] = UnixToSystemTime(pWi->apply_date_unix_local);
					_item.date.date.sysDate[1] = UnixToSystemTime(pWi->end_date_unix_local);

					// Atualiza o tempo do ClubSet do player
					snmdb::NormalManagerDB::getInstance().add(20, new CmdUpdateClubSetTime(_session.m_pi.uid, *pWi), item_manager::SQLDBResponse, nullptr);

					_item.STDA_C_ITEM_QNTD = 1;
					_item.stat.qntd_ant = 0;
					_item.stat.qntd_dep = _item.qntd;

					_item.id = ret_id = (RetAddItem::TYPE)pWi->id;

					ADD_ITEM_SUCESS_MSG_LOG("Atualizou ClubSet");

					// Verifica se o ClubSet está no item update, por que se tiver, 
					// ele vai desequipar esse clubset por que o player não relogou quando acabou o tempo do clubset
					auto v_it = _session.m_pi.findUpdateItemByTypeidAndId(pWi->_typeid, pWi->id);

					if (!v_it.empty()) {

						for (auto& el : v_it)
							if (el->second.type == UpdateItem::UI_TYPE::WAREHOUSE)
								// Tira esse Update Item do map
								_session.m_pi.mp_ui.erase(el);

					}
					// ---- fim do verifica se o ClubSet está no update item ----

				}else
					throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add clubset[TYPEID="
							+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));
			}else {

				WarehouseItemEx wi{ 0 };

				wi.id = _item.id;
				wi._typeid = _item._typeid;
				wi.type = _item.type;
				wi.flag = _item.flag;

				wi.clubset_workshop.level = clubset->work_shop.tipo;	// Cv 1 e etc

				// Tempo
				if (_item.STDA_C_ITEM_TIME > 0)
					wi.STDA_C_ITEM_TIME = _item.STDA_C_ITEM_TIME;

				if (wi.STDA_C_ITEM_TIME > 0)
					wi.STDA_C_ITEM_TIME /= 24; // converte de novo para Dias para salvar no banco de dados

				wi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;	// Aqui tem que colocar para Horas, quanto tempo falta em horas

				if (_gift_flag && wi.id > 0) {
					CmdGetGiftClubSet cmd_ggcs(_session.m_pi.uid, wi, true);	// Waiter

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_ggcs, nullptr, nullptr);

					cmd_ggcs.waitEvent();

					if (cmd_ggcs.getException().getCodeError() != 0)
						throw cmd_ggcs.getException();

					wi = cmd_ggcs.getInfo();
					_item.id = wi.id;

					if (wi.id <= 0)
						throw exception("[item_manager::addItem][Log] nao conseguiu pegar o presente de ClubSet[TYPEID=" + std::to_string(_item._typeid) + "] para o player[UID=" 
								+ std::to_string(_session.m_pi.uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 13, 0));
				
				}else {
					CmdAddClubSet cmd_acs(_session.m_pi.uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_acs, nullptr, nullptr);

					cmd_acs.waitEvent();

					if (cmd_acs.getException().getCodeError() != 0)
						throw cmd_acs.getException();

					wi = cmd_acs.getInfo();
					_item.id = wi.id;

					if (wi.id <= 0)
						throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o ClubSet[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
								+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
				}

				if (_item.STDA_C_ITEM_TIME > 0) {

					_item.date.active = 1;

					wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

					// Convert to UTC to send client
					wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

					wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Convert to UTC to send client
					wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

					// System Time Struct is Local Time
					_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
					_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);

				}

				_item.STDA_C_ITEM_QNTD = 1;
				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = _item.qntd;

				// Add List Item ON Server
				_session.m_pi.mp_wi.insert(std::make_pair(wi.id, wi));

				ret_id = (RetAddItem::TYPE)wi.id;

				ADD_ITEM_SUCESS_MSG_LOG("Adicionou ClubSet");
			}

			break;
		}
		case iff::CARD:
		{
			auto pCi = _session.m_pi.findCardByTypeid(_item._typeid);

			if (pCi != nullptr) {	// Já tem item add quantidade ao card

				_item.stat.qntd_ant = pCi->qntd;
			
				pCi->qntd += _item.qntd;
			
				_item.stat.qntd_dep = pCi->qntd;

				_item.id = ret_id = (RetAddItem::TYPE)pCi->id;

				snmdb::NormalManagerDB::getInstance().add(8, new CmdUpdateCardQntd(_session.m_pi.uid, pCi->id, pCi->qntd), item_manager::SQLDBResponse, nullptr);

				ADD_ITEM_SUCESS_MSG_LOG("Atualizou Card");
			
			}else {

				CardInfo ci{ 0 };

				ci.id = _item.id;
				ci._typeid = _item._typeid;
				ci.qntd = _item.qntd;
				ci.type = 1;

				CmdAddCard cmd_ac(_session.m_pi.uid, ci, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ac, nullptr, nullptr);

				cmd_ac.waitEvent();

				if (cmd_ac.getException().getCodeError() != 0)
					throw cmd_ac.getException();

				ci = cmd_ac.getInfo();
				_item.id = ci.id;

				if (ci.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Card[TYPEID=" + std::to_string(ci._typeid) + "] para o player: " 
							+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = _item.qntd;

				// Add List Item ON Server
				_session.m_pi.v_card_info.push_back(ci);

				ret_id = (RetAddItem::TYPE)ci.id;

				ADD_ITEM_SUCESS_MSG_LOG("Adicionou Card");
			}

			break;
		}
		case iff::FURNITURE:
		{
			// Tem que fazer esse aqui, por que pode vim por Set Item ou MailBox
			auto furniture = sIff::getInstance().findFurniture(_item._typeid);

			if (furniture == nullptr)
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add um Furniture[TYPEID="
						+ std::to_string(_item._typeid) + "] que nao existe no IFF_STRUCT do server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 12, 0));

			auto pFi = _session.m_pi.findMyRoomItemByTypeid(_item._typeid);

			if (pFi != nullptr)
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) +"] tentou add um Furniture[TYPEID="
						+ std::to_string(_item._typeid) + "] que ele ja tem", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			MyRoomItem mri{ 0 };

			mri._typeid = _item._typeid;
			mri.id = _item.id;

			mri.location = *(MyRoomItem::Location*)&furniture->location;

			CmdAddFurniture cmd_af(_session.m_pi.uid, mri, true);	// Waiter

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_af, nullptr, nullptr);

			cmd_af.waitEvent();

			if (cmd_af.getException().getCodeError() != 0)
				throw cmd_af.getException();

			mri = cmd_af.getInfo();
			_item.id = mri.id;

			if (mri.id <= 0)
				throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Furniture[TYPEID=" + std::to_string(mri._typeid) + "] para o player: " 
						+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = _item.STDA_C_ITEM_QNTD;

			// Add List Item ON Server
			_session.m_pi.v_mri.push_back(mri);

			ret_id = (RetAddItem::TYPE)mri.id;

			ADD_ITEM_SUCESS_MSG_LOG("Adicionou Furniture");

			break;
		}
		case iff::AUX_PART:
		{
			// Tem que fazer esse aqui, por que pode vim por Set Item ou MailBox
			//auto auxPart = sIff::getInstance().findAuxPart(_item._typeid);

			auto pWi = _session.m_pi.findWarehouseItemByTypeid(_item._typeid);

			if (pWi != nullptr) {	// Já tem item add quantidade do AuxPart

				if (!sIff::getInstance().IsCanOverlapped(pWi->_typeid))
					throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add AuxPart[TYPEID="
							+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));
			
				_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;
			
				pWi->STDA_C_ITEM_QNTD += _item.STDA_C_ITEM_QNTD;
			
				_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

				_item.id = ret_id = (RetAddItem::TYPE)pWi->id;

				snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateItemQntd(_session.m_pi.uid, pWi->id, pWi->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

				ADD_ITEM_SUCESS_MSG_LOG("Atualizou AuxPart");
			}else {

				WarehouseItemEx wi{ 0 };
				wi.id = _item.id;
				wi._typeid = _item._typeid;

				wi.type = _item.type;
				wi.flag = _item.flag;
#if defined(_WIN32)
				memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
				memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

				if (wi.STDA_C_ITEM_TIME > 0)
					wi.STDA_C_ITEM_TIME /= 24; // converte de novo para Dias para salvar no banco de dados

				wi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;	// Aqui tem que colocar para minutos ou segundos(acho)

				CmdAddItem cmd_ai(_session.m_pi.uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ai, nullptr, nullptr);

				cmd_ai.waitEvent();

				if (cmd_ai.getException().getCodeError() != 0)
					throw cmd_ai.getException();

				wi = cmd_ai.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o AuxPart[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
							+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
		
				if (_item.STDA_C_ITEM_TIME > 0) {
					_item.date.active = 1;

					wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

					// Convert to UTC to send client
					wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

					wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Convert to UTC to send client
					wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

					// System Time Struct is Local Time
					_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
					_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);
				}

				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = wi.STDA_C_ITEM_QNTD;

				// Add List Item ON Server
				_session.m_pi.mp_wi.insert(std::make_pair(wi.id, wi));

				ret_id = (RetAddItem::TYPE)wi.id;

				ADD_ITEM_SUCESS_MSG_LOG("Adicionou AuxPart");
			}

			break;
		}
		case iff::ITEM:
		{
			// CHECK FOR POUCH [PANG, EXP OR CP]
			if (_item._typeid == PANG_POUCH_TYPEID/*Pang Pouch*/) {
			
				// Pang Pouch para o player
				_session.addPang((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD);

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Adicionou Pang Pouch. item[TYPEID=" 
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_FILE_LOG_AND_CONSOLE));
	#else
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Adicionou Pang Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_ONLY_FILE_LOG));
	#endif

				// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
				BlockMemoryManager::unblockUID(_session.m_pi.uid);

				return RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;

			}else if (_item._typeid == EXP_POUCH_TYPEID/*Exp Pouch*/) {
			
				// Exp Pouch para o player
				_session.addExp((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD, true/*Send packet for update level and exp in game*/);

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Adicionou Exp Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_FILE_LOG_AND_CONSOLE));
	#else
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Adicionou Exp Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_ONLY_FILE_LOG));
	#endif

				// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
				BlockMemoryManager::unblockUID(_session.m_pi.uid);

				return RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			
			}else if (_item._typeid == CP_POUCH_TYPEID/*Cookie Point Pouch*/) {

				// Log Ganhos de CP
				CPLog cp_log;

				cp_log.setType(CPLog::TYPE::CP_POUCH);

				cp_log.setCookie((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD);

				// Cookie Point(CP) Pouch para o player
				_session.addCookie((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD);

				// Log de Ganhos de CP
				_session.saveCPLog(cp_log);

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Adicionou CP Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_FILE_LOG_AND_CONSOLE));
#else
				_smp::message_pool::getInstance().push(new message("[Pangya Shop][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Adicionou CP Pouch. item[TYPEID="
						+ std::to_string(_item._typeid) + "] Qntd[value=" + std::to_string((_item.qntd > 0xFFu) ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "]", CL_ONLY_FILE_LOG));
#endif

				// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
				BlockMemoryManager::unblockUID(_session.m_pi.uid);

				return RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			}
			// Fim check pouch

			auto pWi = _session.m_pi.findWarehouseItemByTypeid(_item._typeid);

			// Ticket Report Sempre Add 1 Novo
			if (pWi != nullptr && _item._typeid != TICKET_REPORT_SCROLL_TYPEID) {	// Já tem item add quantidade do Item

				if (sPremiumSystem::getInstance().isPremiumTicket(pWi->_typeid)) {

					auto st = UnixToSystemTime(pWi->end_date_unix_local);

					if (getLocalTimeDiffDESC(st) > 0ll || _session.m_pi.m_cap.stBit.premium_user)
						throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Item[TYPEID="
								+ std::to_string(_item._typeid) + "] 'Premium Ticket' que ele ja possui com tempo, tem que esperar acabar o tempo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 15, 0));
				}

				_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;
			
				pWi->STDA_C_ITEM_QNTD += _item.STDA_C_ITEM_QNTD;
			
				_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

				_item.id = ret_id = (RetAddItem::TYPE)pWi->id;

				// Verifica se é Premium Ticket
				if (sPremiumSystem::getInstance().isPremiumTicket(pWi->_typeid)) { // Renova o Premium Ticket por mais 30(tempo, coloquei mais opções) dias

					if (_item.STDA_C_ITEM_TIME > 0) {
						_item.date.active = 1;

						// Conver
						pWi->STDA_C_ITEM_TIME = _item.STDA_C_ITEM_TIME / 24; // converte de novo para Dias para salvar no banco de dados

						// Atualiza o tempo do Premium Ticket do player
						snmdb::NormalManagerDB::getInstance().add(19, new CmdUpdatePremiumTicketTime(_session.m_pi.uid, *pWi), item_manager::SQLDBResponse, nullptr);

						// update ano (Horas) que o item ainda tem
						pWi->ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;

						// Só atualiza o Apply date se não tiver
						if (pWi->apply_date_unix_local == 0u) {

							pWi->apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

							// Convert to UTC to send client
							pWi->apply_date = TzLocalUnixToUnixUTC(pWi->apply_date_unix_local);
						}

						pWi->end_date_unix_local = (uint32_t)GetLocalTimeAsUnix() + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

						// Convert to UTC to send client
						pWi->end_date = TzLocalUnixToUnixUTC(pWi->end_date_unix_local);

						// System Time Struct is Local Time
						_item.date.date.sysDate[0] = UnixToSystemTime(pWi->apply_date_unix_local);
						_item.date.date.sysDate[1] = UnixToSystemTime(pWi->end_date_unix_local);
					}

					if (sPremiumSystem::getInstance().isPremiumTicket(_item._typeid)) {
						
						_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] renovou premium ticket[TYPEID="
								+ std::to_string(pWi->_typeid) + "] por " + std::to_string(_item.STDA_C_ITEM_TIME / 24u) + " Dias", CL_FILE_LOG_AND_CONSOLE));

						sPremiumSystem::getInstance().addPremiumUser(_session, *pWi, _item.STDA_C_ITEM_TIME / 24u/*Dias*/);
					}

					// Verifica se o Premium Ticket está no item update, por que se tiver
					auto v_it = _session.m_pi.findUpdateItemByTypeidAndId(pWi->_typeid, pWi->id);

					if (!v_it.empty()) {

						for (auto& el : v_it)
							if (el->second.type == UpdateItem::UI_TYPE::WAREHOUSE)
								// Tira esse Update Item do map
								_session.m_pi.mp_ui.erase(el);

					}
					// ---- fim do verifica se o Premium Ticket está no update item ----

				}else // Atualiza a quantidade do item normal
					snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateItemQntd(_session.m_pi.uid, pWi->id, pWi->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

				// Atualiza Gacha Coupon
				if (_item._typeid == 0x1A000080/*Coupon Gacha*/) {
					_session.m_pi.cg.normal_ticket += _item.STDA_C_ITEM_QNTD;

					packet p;
					packet_func::pacote102(p, &_session, &_session.m_pi);
					packet_func::session_send(p, &_session, 1);
				}

				ADD_ITEM_SUCESS_MSG_LOG("Atualizou Item");
			}else {

				if (sPremiumSystem::getInstance().isPremiumTicket(_item._typeid)) {

					auto pWi = (_session.m_pi.pt._typeid == 0l) ? nullptr : _session.m_pi.findWarehouseItemByTypeid(_session.m_pi.pt._typeid);

					if (pWi != nullptr) {

						auto st = UnixToSystemTime(pWi->end_date_unix_local);

						if (getLocalTimeDiffDESC(st) > 0ll || _session.m_pi.m_cap.stBit.premium_user)
							throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Item[TYPEID="
									+ std::to_string(_item._typeid) + "] 'Premium Ticket' que ele ja possui outro Premium Ticket[TYPEID=" 
									+ std::to_string(pWi->_typeid) + "] com tempo, tem que esperar acabar o tempo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 15, 0));
					}
				}

				WarehouseItemEx wi{ 0 };
				wi.id = _item.id;
				wi._typeid = _item._typeid;

				wi.type = _item.type;
				wi.flag = _item.flag;
#if defined(_WIN32)
				memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
				memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

				if (wi.STDA_C_ITEM_TIME > 0)
					wi.STDA_C_ITEM_TIME /= 24; // converte de novo para Dias para salvar no banco de dados

				wi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;	// Aqui tem que colocar para minutos ou segundos(acho)

				CmdAddItem cmd_ai(_session.m_pi.uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ai, nullptr, nullptr);

				cmd_ai.waitEvent();

				if (cmd_ai.getException().getCodeError() != 0)
					throw cmd_ai.getException();

				wi = cmd_ai.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Item[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
							+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));
	

				if (_item.STDA_C_ITEM_TIME > 0) {
					_item.date.active = 1;

					wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

					// Convert to UTC to send client
					wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

					wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

					// Convert to UTC to send client
					wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

					// System Time Struct is Local Time
					_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
					_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);
				}

				if (_item._typeid == 0x1A000080/*Coupon Gacha*/) {
					_session.m_pi.cg.normal_ticket += _item.STDA_C_ITEM_QNTD;

					packet p;
					packet_func::pacote102(p, &_session, &_session.m_pi);
					packet_func::session_send(p, &_session, 1);
				}

				_item.stat.qntd_ant = 0;
				_item.stat.qntd_dep = wi.STDA_C_ITEM_QNTD;

				_session.m_pi.mp_wi.insert(std::make_pair(wi.id, wi));

				if (sPremiumSystem::getInstance().isPremiumTicket(_item._typeid)) {

					_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] comprou premium ticket[TYPEID="
							+ std::to_string(wi._typeid) + "] por " + std::to_string(_item.STDA_C_ITEM_TIME / 24u) + " Dias", CL_FILE_LOG_AND_CONSOLE));

					sPremiumSystem::getInstance().addPremiumUser(_session, wi, _item.STDA_C_ITEM_TIME / 24u);
				}

				ret_id = (RetAddItem::TYPE)wi.id;

				ADD_ITEM_SUCESS_MSG_LOG("Adicionou Item");
			}

			break;
		}
		case iff::SKIN:
		{
			auto pWi = _session.m_pi.findWarehouseItemByTypeid(_item._typeid);

			if (pWi != nullptr)
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Skin[TYPEID="
						+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			WarehouseItemEx wi{ 0 };
			wi.id = _item.id;
			wi._typeid = _item._typeid;
			wi.type = _item.type;
			wi.flag = _item.flag;

#if defined(_WIN32)
			memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
			memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

			CmdAddSkin cmd_as(_session.m_pi.uid, wi, _purchase, 0/*_gift_flag*/, true);	// Waiter

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_as, nullptr, nullptr);

			cmd_as.waitEvent();

			if (cmd_as.getException().getCodeError() != 0)
				throw cmd_as.getException();

			wi = cmd_as.getInfo();
			_item.id = wi.id;

			if (wi.id <= 0)
				throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Skin[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
						+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

			if (_item.STDA_C_ITEM_TIME > 0) {
				_item.date.active = 1;

				wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

				// Convert to UTC to send client
				wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

				wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

				// Convert to UTC to send client
				wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

				// System Time Struct is Local Time
				_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
				_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);
			}

			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = _item.qntd;

			// Add List Item ON Server
			_session.m_pi.mp_wi.insert(std::make_pair(wi.id, wi));

			ret_id = (RetAddItem::TYPE)wi.id;

			ADD_ITEM_SUCESS_MSG_LOG("Adicionou Skin");

			break;
		}
		case iff::PART:
		{
			auto pWi = _session.m_pi.findWarehouseItemByTypeid(_item._typeid);

			if (pWi != nullptr && !sIff::getInstance().IsCanOverlapped(pWi->_typeid))
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Part[TYPEID="
						+ std::to_string(_item._typeid) + "] que ele ja possui", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 10, 0));

			// Ainda falta as parte por tempo, aquelas do couldron de cor dourada, Já Fiz
			WarehouseItemEx wi{ 0 };
			wi.id = _item.id;
			wi._typeid = _item._typeid;

			if (_item.type_iff == IFF::Part::UCC_BLANK || _item.type_iff == IFF::Part::UCC_COPY) {
				if (_item.ucc.IDX != nullptr)
#if defined(_WIN32)
					memcpy_s(wi.ucc.idx, sizeof(wi.ucc.idx), _item.ucc.IDX, sizeof(wi.ucc.idx));
#elif defined(__linux__)
					memcpy(wi.ucc.idx, _item.ucc.IDX, sizeof(wi.ucc.idx));
#endif
				wi.ucc.seq = (unsigned short)_item.ucc.seq;
				wi.ucc.status = (unsigned char)_item.ucc.status;
			}

			wi.type = _item.type;
			wi.flag = _item.flag;
#if defined(_WIN32)
			memcpy_s(wi.c, sizeof(wi.c), _item.c, sizeof(wi.c));
#elif defined(__linux__)
			memcpy(wi.c, _item.c, sizeof(wi.c));
#endif

			wi.ano = (_item.STDA_C_ITEM_TIME > 0) ? STDA_TRANSLATE_FLAG_TIME_TO_HOUR(_item.flag_time, _item.STDA_C_ITEM_TIME) : -1;

			if (_gift_flag && wi.id > 0) {
				CmdGetGiftPart cmd_ggp(_session.m_pi.uid, wi, _item.type_iff, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_ggp, nullptr, nullptr);

				cmd_ggp.waitEvent();

				if (cmd_ggp.getException().getCodeError() != 0)
					throw cmd_ggp.getException();

				wi = cmd_ggp.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu pegar o presente de Part[TYPEID=" + std::to_string(_item._typeid) + "] para o player[UID=" 
							+ std::to_string(_session.m_pi.uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 13, 0));

			}else {
				CmdAddPart cmd_ap(_session.m_pi.uid, wi, _purchase, 0/*_gift_flag*/, _item.type_iff, true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(3, &cmd_ap, nullptr, nullptr);

				cmd_ap.waitEvent();

				if (cmd_ap.getException().getCodeError() != 0)
					throw cmd_ap.getException();

				wi = cmd_ap.getInfo();
				_item.id = wi.id;

				if (wi.id <= 0)
					throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Parts[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " 
							+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

			}

			if (_item.type_iff == IFF::Part::UCC_BLANK || _item.type_iff == IFF::Part::UCC_COPY) {
				if (wi.ucc.idx != nullptr)
#if defined(_WIN32)
					memcpy_s(_item.ucc.IDX, sizeof(_item.ucc.IDX), wi.ucc.idx, sizeof(_item.ucc.IDX));
#elif defined(__linux__)
					memcpy(_item.ucc.IDX, wi.ucc.idx, sizeof(_item.ucc.IDX));
#endif
				_item.ucc.seq = wi.ucc.seq;
				_item.ucc.status = wi.ucc.status;
			}

			_item.STDA_C_ITEM_QNTD = 1;
			_item.stat.qntd_ant = 0;
			_item.stat.qntd_dep = 1;

			if (_item.STDA_C_ITEM_TIME > 0) {
				_item.date.active = 1;

				wi.apply_date_unix_local = (uint32_t)GetLocalTimeAsUnix();

				// Convert to UTC to send client
				wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

				wi.end_date_unix_local = wi.apply_date_unix_local + STDA_TRANSLATE_FLAG_TIME(_item.flag_time, _item.STDA_C_ITEM_TIME);

				// Convert to UTC to send client
				wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

				// System Time Struct is Local Time
				_item.date.date.sysDate[0] = UnixToSystemTime(wi.apply_date_unix_local);
				_item.date.date.sysDate[1] = UnixToSystemTime(wi.end_date_unix_local);

				// Qntd depois em segundos
				_item.stat.qntd_dep = (uint32_t)wi.end_date;

				if (_item.flag_time == 2)
					_item.STDA_C_ITEM_TIME *= 24;	// Horas
			}

			// Add List Item ON Server
			_session.m_pi.mp_wi.insert(std::make_pair(wi.id, wi));
		
			ret_id = (RetAddItem::TYPE)wi.id;

			ADD_ITEM_SUCESS_MSG_LOG("Adicionou Part" + (wi.ano > 0 && (wi.ano / 24) == 7/*Dia*/ && wi.flag == 0x60/*Rental*/ ? " Rental[Dia(s)=" + std::to_string(wi.STDA_C_ITEM_TIME) + "]" : std::string("")));

			break;
		}
		case iff::SET_ITEM:
			break;
		case iff::HAIR_STYLE:
		{
			auto hair = sIff::getInstance().findHairStyle(_item._typeid);

			if (hair != nullptr) {
				uint32_t char_typeid = (iff::CHARACTER << 26) | hair->character;

				CharacterInfo *ce = _session.m_pi.findCharacterByTypeid(char_typeid);

				if (ce != nullptr) {

					ce->default_hair = hair->cor;

					snmdb::NormalManagerDB::getInstance().add(4, new CmdAddCharacterHairStyle(_session.m_pi.uid, *ce, _purchase, 0/*_gift_flag*/), item_manager::SQLDBResponse, &_session);

					ret_id = (RetAddItem::TYPE)ce->id;

					ADD_ITEM_SUCESS_MSG_LOG("Atualizou HairStyle");

				}else
					throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem esse character[TYPEID=" 
							+ std::to_string(char_typeid) + "].", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 16, 0));

			}else
				throw exception("[item_manager::addItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] hairstyle[TYPEID=" 
						+ std::to_string(_item._typeid) + "] nao tem no IFF_STRUCT.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 17, 0));
		
			break;
		}
		case iff::COUNTER_ITEM:
			break;
		case iff::ACHIEVEMENT:
			break;
		case iff::MATCH:	// Troféu
		{
			auto type_trofel = sIff::getInstance().getItemSubGroupIdentify24(_item._typeid);

			// Troféu Espacial
			if (type_trofel == 1 || type_trofel == 2) {

				auto tsi = _session.m_pi.findTrofelEspecialByTypeid(_item._typeid);

				if (tsi != nullptr) {	// Já tem item add quantidade do Troféu Especial

					_item.stat.qntd_ant = tsi->qntd;
			
					tsi->qntd += _item.STDA_C_ITEM_QNTD;
			
					_item.stat.qntd_dep = tsi->qntd;

					_item.id = ret_id = (RetAddItem::TYPE)tsi->id;

					snmdb::NormalManagerDB::getInstance().add(18, new CmdUpdateTrofelEspecialQntd(_session.m_pi.uid, tsi->id, tsi->qntd, CmdUpdateTrofelEspecialQntd::eTYPE::ESPECIAL), item_manager::SQLDBResponse, nullptr);

					ADD_ITEM_SUCESS_MSG_LOG("Atualizou Trofel Especial");
				}else {

					TrofelEspecialInfo ts{ 0 };
					ts.id = _item.id;
					ts._typeid = _item._typeid;
					ts.qntd = _item.STDA_C_ITEM_QNTD;

					CmdAddTrofelEspecial cmd_ts(_session.m_pi.uid, ts, CmdAddTrofelEspecial::eTYPE::ESPECIAL, true);	// Waiter

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_ts, nullptr, nullptr);

					cmd_ts.waitEvent();

					if (cmd_ts.getException().getCodeError() != 0)
						throw cmd_ts.getException();

					ts = cmd_ts.getInfo();
					_item.id = ts.id;

					if (ts.id <= 0)
						throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Trofel Especial[TYPEID=" + std::to_string(ts._typeid) + "] para o player: " 
								+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

					_item.stat.qntd_ant = 0;
					_item.stat.qntd_dep = ts.qntd;

					// Add List Item ON Server
					_session.m_pi.v_tsi_current_season.push_back(ts);

					ret_id = (RetAddItem::TYPE)ts.id;

					ADD_ITEM_SUCESS_MSG_LOG("Adicionou Trofel Especial");
				}
			
			}else if (type_trofel == 3) {	// Grand Prix

				auto tsi = _session.m_pi.findTrofelGrandPrixByTypeid(_item._typeid);

				if (tsi != nullptr) {	// Já tem item add quantidade do Troféu Grand Prix

					_item.stat.qntd_ant = tsi->qntd;
			
					tsi->qntd += _item.STDA_C_ITEM_QNTD;
			
					_item.stat.qntd_dep = tsi->qntd;

					_item.id = ret_id = (RetAddItem::TYPE)tsi->id;

					snmdb::NormalManagerDB::getInstance().add(18, new CmdUpdateTrofelEspecialQntd(_session.m_pi.uid, tsi->id, tsi->qntd, CmdUpdateTrofelEspecialQntd::eTYPE::GRAND_PRIX), item_manager::SQLDBResponse, nullptr);

					ADD_ITEM_SUCESS_MSG_LOG("Atualizou Trofel Grand Prix");
				}else {

					TrofelEspecialInfo ts{ 0 };
					ts.id = _item.id;
					ts._typeid = _item._typeid;
					ts.qntd = _item.STDA_C_ITEM_QNTD;

					CmdAddTrofelEspecial cmd_ts(_session.m_pi.uid, ts, CmdAddTrofelEspecial::eTYPE::GRAND_PRIX, true);	// Waiter

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_ts, nullptr, nullptr);

					cmd_ts.waitEvent();

					if (cmd_ts.getException().getCodeError() != 0)
						throw cmd_ts.getException();

					ts = cmd_ts.getInfo();
					_item.id = ts.id;

					if (ts.id <= 0)
						throw exception("[item_manager::addItem][Log] nao conseguiu adicionar o Trofel Grand Prix[TYPEID=" + std::to_string(ts._typeid) + "] para o player: " 
								+ std::to_string(_session.m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 11, 0));

					_item.stat.qntd_ant = 0;
					_item.stat.qntd_dep = ts.qntd;

					// Add List Item ON Server
					_session.m_pi.v_tgp_current_season.push_back(ts);

					ret_id = (RetAddItem::TYPE)ts.id;

					ADD_ITEM_SUCESS_MSG_LOG("Adicionou Trofel Grand Prix");
				}
			}

			break;
		}	// End iff::MATCH
		}	// End Switch

		// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
		BlockMemoryManager::unblockUID(_session.m_pi.uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[item_manager::addItem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Libera Block memória para o UID, previne de add mais de um item simuntaneamente, para não gerar valores errados
		BlockMemoryManager::unblockUID(_session.m_pi.uid);

		ret_id = RetAddItem::T_ERROR;
	}

	return ret_id;
};

item_manager::RetAddItem item_manager::addItem(std::vector< stItem >& _v_item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup) {
	
	RetAddItem rai{ 0 };
	RetAddItem::TYPE type = RetAddItem::T_INIT_VALUE;

	for (auto it = _v_item.begin(); it != _v_item.end(); /*Incrementa no Else*/) {

		if ((type = addItem(*it, _session, _gift_flag, _purchase, _dup)) <= 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou adicionar o item[TYPEID=" 
					+ std::to_string(it->_typeid) + ", ID=" + std::to_string(it->id) + "], " 
					+ ((type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) ? "mas era pang, exp ou CP pouch" : "mas nao conseguiu. Bug"), CL_FILE_LOG_AND_CONSOLE));

			rai.fails.push_back(*it);

			it = _v_item.erase(it);

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = type;
			else if (type == RetAddItem::T_ERROR) {

				if (rai.type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
				else if (rai.type != RetAddItem::TR_SUCCESS_WITH_ERROR && rai.type != RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR)
					rai.type = type;

			}else if (type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) {
				
				if (rai.type == RetAddItem::T_ERROR || rai.type == RetAddItem::TR_SUCCESS_WITH_ERROR)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			}

		}else {

			// Incrementa
			++it;
			
			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = RetAddItem::T_SUCCESS;
			else if (rai.type == RetAddItem::T_ERROR)
				rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
		}
	}

	return rai;
};

item_manager::RetAddItem item_manager::addItem(std::vector< stItemEx >& _v_item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup) {
	
	RetAddItem rai{ 0 };
	RetAddItem::TYPE type = RetAddItem::T_INIT_VALUE;

	for (auto it = _v_item.begin(); it != _v_item.end(); /*Incrementa no Else*/) {

		if ((type = addItem(*it, _session, _gift_flag, _purchase, _dup)) <= 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou adicionar o item[TYPEID=" 
					+ std::to_string(it->_typeid) + ", ID=" + std::to_string(it->id) + "], " 
					+ ((type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) ? "mas era pang, exp ou CP pouch" : "mas nao conseguiu. Bug"), CL_FILE_LOG_AND_CONSOLE));

			rai.fails.push_back(*it);

			it = _v_item.erase(it);

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = type;
			else if (type == RetAddItem::T_ERROR) {

				if (rai.type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
				else if (rai.type != RetAddItem::TR_SUCCESS_WITH_ERROR && rai.type != RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR)
					rai.type = type;

			}else if (type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) {
				
				if (rai.type == RetAddItem::T_ERROR || rai.type == RetAddItem::TR_SUCCESS_WITH_ERROR)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			}

		}else {

			// Incrementa
			++it;

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = RetAddItem::T_SUCCESS;
			else if (rai.type == RetAddItem::T_ERROR)
				rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
		}
	}

	return rai;
};

item_manager::RetAddItem item_manager::addItem(std::map< uint32_t, stItem >& _v_item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup) {
	
	RetAddItem rai{ 0 };
	RetAddItem::TYPE type = RetAddItem::T_INIT_VALUE;

	for (auto it = _v_item.begin(); it != _v_item.end(); /*Incrementa no Else*/) {

		if ((type = addItem(it->second, _session, _gift_flag, _purchase, _dup)) <= 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou adicionar o item[TYPEID=" 
					+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "], " 
					+ ((type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) ? "mas era pang, exp ou CP pouch" : "mas nao conseguiu.Bug"), CL_FILE_LOG_AND_CONSOLE));

			rai.fails.push_back(it->second);

			it = _v_item.erase(it);

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = type;
			else if (type == RetAddItem::T_ERROR) {

				if (rai.type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
				else if (rai.type != RetAddItem::TR_SUCCESS_WITH_ERROR && rai.type != RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR)
					rai.type = type;

			}else if (type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) {
				
				if (rai.type == RetAddItem::T_ERROR || rai.type == RetAddItem::TR_SUCCESS_WITH_ERROR)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			}

		}else {

			// Incrementa
			++it;

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = RetAddItem::T_SUCCESS;
			else if (rai.type == RetAddItem::T_ERROR)
				rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
		}
	}

	return rai;
};

item_manager::RetAddItem item_manager::addItem(std::map< uint32_t, stItemEx >& _v_item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup) {
	
	RetAddItem rai{ 0 };
	RetAddItem::TYPE type = RetAddItem::T_INIT_VALUE;

	for (auto it = _v_item.begin(); it != _v_item.end(); /*Incrementa no Else*/) {

		if ((type = addItem(it->second, _session, _gift_flag, _purchase, _dup)) <= 0) {

			_smp::message_pool::getInstance().push(new message("[item_manager::addItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou adicionar o item[TYPEID=" 
					+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "], " 
					+ ((type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) ? "mas era pang, exp ou CP pouch" : "mas nao conseguiu.Bug"), CL_FILE_LOG_AND_CONSOLE));

			rai.fails.push_back(it->second);

			it = _v_item.erase(it);

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = type;
			else if (type == RetAddItem::T_ERROR) {

				if (rai.type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
				else if (rai.type != RetAddItem::TR_SUCCESS_WITH_ERROR && rai.type != RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR)
					rai.type = type;

			}else if (type == RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) {
				
				if (rai.type == RetAddItem::T_ERROR || rai.type == RetAddItem::TR_SUCCESS_WITH_ERROR)
					rai.type = RetAddItem::TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR;
				else if (rai.type == RetAddItem::T_SUCCESS)
					rai.type = RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH;
			}

		}else {

			// Incrementa
			++it;

			if (rai.type == RetAddItem::T_INIT_VALUE)
				rai.type = RetAddItem::T_SUCCESS;
			else if (rai.type == RetAddItem::T_ERROR)
				rai.type = RetAddItem::TR_SUCCESS_WITH_ERROR;
		}
	}

	return rai;
};

int32_t item_manager::giveItem(stItem& _item, player& _session, unsigned char _gift_flag) {
	
	if (!_session.getState())
		throw exception("[item_manager::giveItem][Error] session nao esta conectada.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 0, 8));

	int32_t ret_id = -1;

	switch (sIff::getInstance().getItemGroupIdentify(_item._typeid)) {
	case iff::CHARACTER:
		// Não pode deletar Character
		throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode presentear um Character[TYPEID=" 
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ja comprado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));
		break;
	case iff::CADDIE:
		throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode presentear um Caddie[TYPEID=" 
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ja comprado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));
		break;
	case iff::CAD_ITEM:
		throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode presentear um CaddieItem[TYPEID=" 
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ja comprado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));
		break;
	case iff::CARD:
	{
		auto pCi = _session.m_pi.findCardById(_item.id);

		if (pCi == nullptr)
			throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Card[TYPEID="
				+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] para ser presenteado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 21, 0));

		_item.stat.qntd_ant = pCi->qntd;

		pCi->qntd -= _item.STDA_C_ITEM_QNTD;

		_item.stat.qntd_dep = pCi->qntd;

		_item.STDA_C_ITEM_QNTD *= -1;	// deixa negativo para tirar no pangya

		// Return value, avoid memory leaks
		ret_id = pCi->id;

		if (pCi->qntd == 0) {
			snmdb::NormalManagerDB::getInstance().add(12, new CmdDeleteCard(_session.m_pi.uid, pCi->id), item_manager::SQLDBResponse, nullptr);	// Delete Card

			//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_ci, second.id, == , pCi->id);
			auto it = _session.m_pi.findCaddieItById(pCi->id);

			if (it != _session.m_pi.mp_ci.end())
				_session.m_pi.mp_ci.erase(it);
		}else
			snmdb::NormalManagerDB::getInstance().add(8, new CmdUpdateCardQntd(_session.m_pi.uid, pCi->id, pCi->qntd), item_manager::SQLDBResponse, nullptr);	// Update

		break;
	}
	case iff::CLUBSET:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr)
			throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o ClubSet[TYPEID="
				+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] para ser presenteado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 21, 0));

		_item.stat.qntd_ant = 1;
		_item.stat.qntd_dep = 0;
		_item.STDA_C_ITEM_QNTD = -1;

		snmdb::NormalManagerDB::getInstance().add(13, new CmdGiftClubSet(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);	// Gift ClubSet

		// Return value, avoid memory leaks
		ret_id = pWi->id;

		auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

		if (it != _session.m_pi.mp_wi.end())
			_session.m_pi.mp_wi.erase(it);

		break;
	}
	case iff::BALL:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr)
			throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Ball[TYPEID=" 
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] para ser presenteado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 21, 0));

		if (_item.STDA_C_ITEM_QNTD > pWi->STDA_C_ITEM_QNTD)
			throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem quantidade[value=" 
					+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", req=" + std::to_string(_item.STDA_C_ITEM_QNTD) + "] suficiente para o Ball[TYPEID=" 
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ser presenteado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 22, 0));

		_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

		pWi->STDA_C_ITEM_QNTD -= _item.STDA_C_ITEM_QNTD;

		_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

		_item.STDA_C_ITEM_QNTD *= -1;	// deixa negativo para tirar no pangya

		// Return value, avoid memory leaks
		ret_id = pWi->id;

		if (pWi->STDA_C_ITEM_QNTD == 0) {
			snmdb::NormalManagerDB::getInstance().add(11, new CmdDeleteBall(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);	// Delete Ball

			//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_wi, id, == , pWi->id);
			auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

			if (it != _session.m_pi.mp_wi.end())
				_session.m_pi.mp_wi.erase(it);
		}else
			snmdb::NormalManagerDB::getInstance().add(7, new CmdUpdateBallQntd(_session.m_pi.uid, pWi->id, pWi->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);	// Update

		break;
	}
	case iff::FURNITURE:
		throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode presentear um Furniture[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ja comprado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));
		break;
	case iff::HAIR_STYLE:
		throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode presentear um HairStyle[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ja comprado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));
		break;
	case iff::ITEM:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr)
			throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o item[TYPEID=" 
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] para ser presenteado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 21, 0));

		if (_item.STDA_C_ITEM_QNTD > pWi->STDA_C_ITEM_QNTD)
			throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem quantidade[value=" 
					+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", req=" + std::to_string(_item.STDA_C_ITEM_QNTD) + "] suficiente para o item[TYPEID=" 
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ser presenteado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 22, 0));

		_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

		pWi->STDA_C_ITEM_QNTD -= _item.STDA_C_ITEM_QNTD;

		_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

		_item.STDA_C_ITEM_QNTD *= -1;	// deixa negativo para tirar no pangya

		// Return value, avoid memory leaks
		ret_id = pWi->id;

		if (pWi->STDA_C_ITEM_QNTD == 0) {
			snmdb::NormalManagerDB::getInstance().add(10, new CmdDeleteItem(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);	// Delete Item

			//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_wi, id, == , pWi->id);
			auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

			if (it != _session.m_pi.mp_wi.end())
				_session.m_pi.mp_wi.erase(it);
		}else
			snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateItemQntd(_session.m_pi.uid, pWi->id, pWi->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);	// Update

		break;
	}
	case iff::SET_ITEM:
		throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode presentear um SetItem[TYPEID="
			+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ja comprado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));
		break;
	case iff::SKIN:
		throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode presentear um Skin[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ja comprado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));
		break;
	case iff::MASCOT:
		throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode presentear um Mascot[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] ja comprado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 20, 0));
		break;
	case iff::PART:
	{

		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr)
			throw exception("[item_manager::giveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Part[TYPEID="
				+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] para ser presenteado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 21, 0));

		_item.stat.qntd_ant = 1;
		_item.stat.qntd_dep = 0;
		_item.STDA_C_ITEM_QNTD = -1;

		snmdb::NormalManagerDB::getInstance().add(14, new CmdGiftPart(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);	// Gift Part

		// Return value, avoid memory leaks
		ret_id = pWi->id;

		auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

		if (it != _session.m_pi.mp_wi.end())
			_session.m_pi.mp_wi.erase(it);

		break;
	}	// Case iff::PART End
	}	// Switch End
	
	return ret_id;
};

int32_t item_manager::giveItem(std::vector< stItem >& _v_item, player& _session, unsigned char _gift_flag) {
	
	uint32_t i;

	for (i = 0u; i < _v_item.size(); ++i)
		if (giveItem(_v_item[i], _session, _gift_flag) <= 0)
			_v_item.erase(_v_item.begin() + i--/*Decrementa o 'i' por que esta tira um no vector*/);

	return (int)i;
};

int32_t item_manager::giveItem(std::vector< stItemEx >& _v_item, player& _session, unsigned char _gift_flag) {

	uint32_t i;

	for (i = 0u; i < _v_item.size(); ++i)
		if (giveItem(_v_item[i], _session, _gift_flag) <= 0)
			_v_item.erase(_v_item.begin() + i--/*Decrementa o 'i' por que esta tira um no vector*/);

	return (int)i;
};

int32_t item_manager::removeItem(stItem& _item, player& _session) {


#ifdef _DEBUG
#define MSG_ACTIVE_DEBUG	CL_FILE_LOG_AND_CONSOLE
#else
#define MSG_ACTIVE_DEBUG	CL_ONLY_FILE_LOG
#endif

#define DEL_ITEM_SUCESS_MSG_LOG(__item) { \
	_smp::message_pool::getInstance().push(new message("[Pangya removeItem][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] " + std::string((__item)) + "[Typeid=" \
			+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + ", QNTD=" + std::to_string((_item.STDA_C_ITEM_QNTD > 0 ? _item.STDA_C_ITEM_QNTD : _item.qntd)) + "]", MSG_ACTIVE_DEBUG)); \
}

	if (!_session.getState())
		throw exception("[item_manager::removeItem][Error] session nao esta conectada.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 0, 8));

	int32_t ret_id = -1;

	switch (sIff::getInstance().getItemGroupIdentify(_item._typeid)) {
	case iff::IFF_GROUP_ID::AUX_PART:	// Warehouse
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um AuxPart[TYPEID=" 
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (pWi->STDA_C_ITEM_QNTD <= (short)_item.qntd) {	// Exclui o Item[AxuPart]

			_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

			_item.STDA_C_ITEM_QNTD = pWi->STDA_C_ITEM_QNTD * -1;

			pWi->STDA_C_ITEM_QNTD = 0;

			_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

			snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteItem(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);

			//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_wi, id, == , pWi->id);
			auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

			if (it != _session.m_pi.mp_wi.end())
				_session.m_pi.mp_wi.erase(it);

			ret_id = _item.id;

			// Se deletou a AuxPart que estava equipada 
			// Desequipa o AuxPart
			auto v_ci = _session.isAuxPartEquiped(_item._typeid);

			if (!v_ci.empty()) {

				for (auto& el : v_ci) {

					if (el != nullptr) {

						// Desequipa o AuxPart
						el->unequipAuxPart(_item._typeid);

						// Update ON DB
						snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCharacterAllPartEquiped(_session.m_pi.uid, *el), item_manager::SQLDBResponse, nullptr);

#ifdef _DEBUG
						_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] desequipou o AuxPart[TYPEID=" + std::to_string(_item._typeid) + "] do Character[TYPEID=" + std::to_string(el->_typeid) 
								+ ", ID=" + std::to_string(el->id) + "] por que ele foi deletado.", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

						// Update ON GAME
						packet p((unsigned short)0x6B);

						p.addInt8(4);		// 4 Sucesso
						p.addInt8(0);		// Character All Parts

						p.addBuffer(el, sizeof(CharacterInfo));

						packet_func::session_send(p, &_session, 1);
					}
				}
			}

			DEL_ITEM_SUCESS_MSG_LOG("Deletou AuxPart");

		}else {	// Att quantidade do Item

			_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

			pWi->STDA_C_ITEM_QNTD -= (short)_item.qntd;

			_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

			snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateItemQntd(_session.m_pi.uid, pWi->id, pWi->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

			ret_id = pWi->id;

			DEL_ITEM_SUCESS_MSG_LOG("Atualizou AuxPart");
		}

		break;
	}
	case iff::IFF_GROUP_ID::BALL:	// Warehouse
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um Ball[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (pWi->STDA_C_ITEM_QNTD <= (short)_item.qntd) {	// Exclui o Item[Ball]

			_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

			_item.STDA_C_ITEM_QNTD = pWi->STDA_C_ITEM_QNTD * -1;

			pWi->STDA_C_ITEM_QNTD = 0;

			_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

			// Passa o typeid do Warehouse para o _item para garantir, se não tiver colocado o typeid, na estrutura
			_item._typeid = pWi->_typeid;
			
			snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteBall(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);

			//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_wi, id, == , pWi->id);
			auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

			if (it != _session.m_pi.mp_wi.end())
				_session.m_pi.mp_wi.erase(it);

			ret_id = _item.id;

			// Se deletou a bola que estava equipada 
			// troca para a bola padrão
			if (_session.m_pi.ue.ball_typeid == _item._typeid) {

				WarehouseItemEx *pBall = nullptr;
				packet p;

				if ((pBall = _session.m_pi.findWarehouseItemByTypeid(0x14000000/*Comet Padrão*/)) == nullptr)
					_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error][WARNING] player[UID=" 
							+ std::to_string(_session.m_pi.uid) + "] nao tem a Comet padrao para substituir a bola[TYPEID=" 
							+ std::to_string(_item._typeid) + "] deletada. Bug", CL_FILE_LOG_AND_CONSOLE));
				else {	// Substitui

					// Update ON SERVER
					_session.m_pi.ue.ball_typeid = DEFAULT_COMET_TYPEID;	// Comet Padrão

					_session.m_pi.ei.comet = pBall;

					// Update ON DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _session.m_pi.ue.ball_typeid), item_manager::SQLDBResponse, nullptr);

#ifdef _DEBUG
					_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] substitui a bola[TYPEID="
							+ std::to_string(_item._typeid) + "] deletada pela COMET PADRAO[TYPEID=" + std::to_string(DEFAULT_COMET_TYPEID) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

					// Update ON GAME
					packet_func::pacote04B(p, &_session, 2/*COMET*/);
					packet_func::session_send(p, &_session, 1);
				}
			}

			DEL_ITEM_SUCESS_MSG_LOG("Deletou Ball");
			
		}else {	// Att quantidade do Item

			_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

			pWi->STDA_C_ITEM_QNTD -= (short)_item.qntd;

			_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

			snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallQntd(_session.m_pi.uid, pWi->id, pWi->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

			ret_id = pWi->id;

			DEL_ITEM_SUCESS_MSG_LOG("Atualizou Ball");
		}

		break;
	}
	case iff::IFF_GROUP_ID::CADDIE:
	{
		auto pCi = _session.m_pi.findCaddieById(_item.id);

		if (pCi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um Caddie[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		_item.stat.qntd_ant = 1;

		_item.STDA_C_ITEM_QNTD = -1;

		_item.stat.qntd_dep = 0;

		snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteCaddie(_session.m_pi.uid, pCi->id), item_manager::SQLDBResponse, nullptr);

		//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_ci, second.id, == , pCi->id);
		auto it = _session.m_pi.findCaddieItById(pCi->id);

		if (it != _session.m_pi.mp_ci.end())
			_session.m_pi.mp_ci.erase(it);

		ret_id = _item.id;

		DEL_ITEM_SUCESS_MSG_LOG("Deletou Caddie");

		break;
	}
	case iff::IFF_GROUP_ID::CAD_ITEM:
		// por hora nao exclui esse por que ainda nao vi nenhum que exclui caddie item
		_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um caddie item[TYPEID=" 
				+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] mas nao e permitido. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		break;
	case iff::IFF_GROUP_ID::CARD:
	{
		auto pCi = _session.m_pi.findCardById(_item.id);

		if (pCi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um Card[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (pCi->qntd <= (int)_item.qntd) {	// Exclui Item[Card]


			_item.stat.qntd_ant = pCi->qntd;

			_item.stat.qntd_dep = 0;

			_item.STDA_C_ITEM_QNTD = (short)pCi->qntd * -1;

			pCi->qntd = 0;

			snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteCard(_session.m_pi.uid, pCi->id), item_manager::SQLDBResponse, nullptr);

			//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_card_info, id, == , pCi->id);
			auto it = _session.m_pi.findCardItById(pCi->id);

			if (it != _session.m_pi.v_card_info.end())
				_session.m_pi.v_card_info.erase(it);

			ret_id = _item.id;

			DEL_ITEM_SUCESS_MSG_LOG("Deletou Card");

		}else {	// Att quantidade do Item

			_item.stat.qntd_ant = pCi->qntd;

			pCi->qntd -= _item.qntd;

			_item.stat.qntd_dep = pCi->qntd;

			snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCardQntd(_session.m_pi.uid, pCi->id, pCi->qntd), item_manager::SQLDBResponse, nullptr);

			ret_id = pCi->id;

			DEL_ITEM_SUCESS_MSG_LOG("Atualizou Card");
		}

		break;
	}
	case iff::IFF_GROUP_ID::CHARACTER:
		_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um character[TYPEID="
				+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] mas nao e permitido. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		break;
	case iff::IFF_GROUP_ID::CLUBSET:	// Warehouse
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um ClubSet[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		_item.stat.qntd_ant = 1;

		_item.STDA_C_ITEM_QNTD = -1;

		_item.stat.qntd_dep = 0;

		snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteItem(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);

		//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_wi, id, == , pWi->id);
		auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

		if (it != _session.m_pi.mp_wi.end())
			_session.m_pi.mp_wi.erase(it);

		ret_id = _item.id;

		DEL_ITEM_SUCESS_MSG_LOG("Deletou ClubSet");

		break;
	}
	case iff::IFF_GROUP_ID::FURNITURE:
	{
		auto pFi = _session.m_pi.findMyRoomItemById(_item.id);

		if (pFi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um Furniture[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		_item.stat.qntd_ant = 1;

		_item.STDA_C_ITEM_QNTD = -1;

		_item.stat.qntd_dep = 0;

		snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteFurniture(_session.m_pi.uid, pFi->id), item_manager::SQLDBResponse, nullptr);

		//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_mri, id, == , pFi->id);
		auto it = _session.m_pi.findMyRoomItemItById(pFi->id);

		if (it != _session.m_pi.v_mri.end())
			_session.m_pi.v_mri.erase(it);

		ret_id = _item.id;

		DEL_ITEM_SUCESS_MSG_LOG("Deletou Furniture");

		break;
	}
	case iff::IFF_GROUP_ID::HAIR_STYLE:
		_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um hairstyle[TYPEID="
				+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] mas nao e permitido. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		break;
	case iff::IFF_GROUP_ID::ITEM:	// Warehouse
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um Item[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (pWi->STDA_C_ITEM_QNTD <= (short)_item.qntd) {	// Exclui o Item[Item]

			_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

			_item.STDA_C_ITEM_QNTD = pWi->STDA_C_ITEM_QNTD * -1;

			pWi->STDA_C_ITEM_QNTD = 0;

			_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

			snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteItem(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);

			//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_wi, id, == , pWi->id);
			auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

			if (it != _session.m_pi.mp_wi.end())
				_session.m_pi.mp_wi.erase(it);

			ret_id = _item.id;

			DEL_ITEM_SUCESS_MSG_LOG("Deletou Item");
			
		}else {	// Att quantidade do Item

			_item.stat.qntd_ant = pWi->STDA_C_ITEM_QNTD;

			pWi->STDA_C_ITEM_QNTD -= (short)_item.qntd;

			_item.stat.qntd_dep = pWi->STDA_C_ITEM_QNTD;

			snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateItemQntd(_session.m_pi.uid, pWi->id, pWi->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

			ret_id = pWi->id;

			DEL_ITEM_SUCESS_MSG_LOG("Atualizou Item");
		}

		break;
	}
	case iff::IFF_GROUP_ID::MASCOT:
	{
		auto pMi = _session.m_pi.findMascotById(_item.id);

		if (pMi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um Mascot[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		_item.stat.qntd_ant = 1;

		_item.STDA_C_ITEM_QNTD = -1;

		_item.stat.qntd_dep = 0;

		snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteMascot(_session.m_pi.uid, pMi->id), item_manager::SQLDBResponse, nullptr);

		//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_mi, id, == , pMi->id);
		auto it = _session.m_pi.findMascotItById(pMi->id);

		if (it != _session.m_pi.mp_mi.end())
			_session.m_pi.mp_mi.erase(it);

		ret_id = _item.id;

		DEL_ITEM_SUCESS_MSG_LOG("Deletou Mascot");

		break;
	}
	case iff::IFF_GROUP_ID::PART:	// Warehouse
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um Part[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		_item.stat.qntd_ant = 1;

		_item.STDA_C_ITEM_QNTD = -1;
		
		_item.stat.qntd_dep = 0;

		snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteItem(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);

		//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_wi, id, == , pWi->id);
		auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

		if (it != _session.m_pi.mp_wi.end())
			_session.m_pi.mp_wi.erase(it);

		ret_id = _item.id;

		// Se deletou a Part que estava equipada 
		// Desequipa o Part
		auto ci = _session.isPartEquiped(_item._typeid);

		if (ci != nullptr) {

			// Desequipa o Part
			ci->unequipPart(_item._typeid);

			// Update ON DB
			snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCharacterAllPartEquiped(_session.m_pi.uid, *ci), item_manager::SQLDBResponse, nullptr);

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] desequipou o Part[TYPEID="
					+ std::to_string(_item._typeid) + "] por que ele foi deletado.", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

			// Update ON GAME
			packet p((unsigned short)0x6B);

			p.addInt8(4);		// 4 Sucesso
			p.addInt8(0);		// Character All Parts

			p.addBuffer(ci, sizeof(CharacterInfo));

			packet_func::session_send(p, &_session, 1);
		}

		DEL_ITEM_SUCESS_MSG_LOG("Deletou Part");

		break;
	}
	case iff::IFF_GROUP_ID::SET_ITEM:
		_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um SetItem[TYPEID="
				+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] mas nao e permitido. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		break;
	case iff::IFF_GROUP_ID::SKIN:	// Warehouse
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_item.id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::removeItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou remover um Skin[TYPEID="
					+ std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		_item.stat.qntd_ant = 1;

		_item.STDA_C_ITEM_QNTD = -1;

		_item.stat.qntd_dep = 0;

		snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteItem(_session.m_pi.uid, pWi->id), item_manager::SQLDBResponse, nullptr);

		//auto it = VECTOR_FIND_ITEM(_session.m_pi.v_wi, id, == , pWi->id);
		auto it = _session.m_pi.findWarehouseItemItById(pWi->id);

		if (it != _session.m_pi.mp_wi.end())
			_session.m_pi.mp_wi.erase(it);

		ret_id = _item.id;

		DEL_ITEM_SUCESS_MSG_LOG("Deletou Skin");

		break;
	}
	default:
		break;
	}

	return ret_id;
};

int32_t item_manager::removeItem(std::vector< stItem >& _v_item, player& _session) {

	uint32_t i;

	for (i = 0u; i < _v_item.size(); ++i)
		if (removeItem(_v_item[i], _session) <= 0)
			_v_item.erase(_v_item.begin() + i--/*Decrementa o 'i' por que esta tira um no vector*/);

	return (int)i;
};

int32_t item_manager::removeItem(std::vector< stItemEx >& _v_item, player& _session) {

	uint32_t i;

	for (i = 0u; i < _v_item.size(); ++i)
		if (removeItem(_v_item[i], _session) <= 0)
			_v_item.erase(_v_item.begin() + i--/*Decrementa o 'i' por que esta tira um no vector*/);

	return (int)i;
};

//WarehouseItemEx* item_manager::transferItem(player& _s_snd, player& _s_rcv, PersonalShopItem& _psi, PersonalShopItem& _psi_r) {
void* item_manager::transferItem(player& _s_snd, player& _s_rcv, PersonalShopItem& _psi, PersonalShopItem& _psi_r) {

#ifdef _DEBUG
#define MSG_ACTIVE_DEBUG	CL_FILE_LOG_AND_CONSOLE
#else
#define MSG_ACTIVE_DEBUG	CL_ONLY_FILE_LOG
#endif

#define TRANSFER_ITEM_SUCESS_MSG_LOG(__item) { \
	_smp::message_pool::getInstance().push(new message("[Transfer Personal Shop][Log] Player_s[UID=" + std::to_string(_s_snd.m_pi.uid) + "] " + std::string((__item)) + "[Typeid=" \
			+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + ", QNTD=" + std::to_string(_psi.item.qntd) + ", PANG(qntd*unit)=" \
			+ std::to_string(_psi.item.pang * _psi.item.qntd) + "] transferiu para o Player_r[UID=" + std::to_string(_s_rcv.m_pi.uid) + "]", MSG_ACTIVE_DEBUG)); \
}

	if (!_s_rcv.getState() || !_s_snd.getState())
		throw exception("[item_manger::transferItem][Error] player& _s_rcv or player& _s_snd is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 8, 0));

	//WarehouseItemEx *ret_wi = nullptr;
	void* ret_wi = nullptr;

	switch (sIff::getInstance().getItemGroupIdentify(_psi.item._typeid)) {
	case iff::ITEM:
	{
		auto pWi_s = _s_snd.m_pi.findWarehouseItemById(_psi.item.id);

		if (pWi_s == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player send[UID=" + std::to_string(_s_snd.m_pi.uid) + "] nao tem o item[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] para enviar para transferir para o player recv[UID=" + std::to_string(_s_rcv.m_pi.uid) + "]. Hacker ou Bug",
					CL_FILE_LOG_AND_CONSOLE));

			return nullptr;
		}

		if (pWi_s->STDA_C_ITEM_QNTD < (int)_psi.item.qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player send[UID=" + std::to_string(_s_snd.m_pi.uid) + "] nao tem quantidade de item[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + ", QNTD=" + std::to_string(_psi.item.qntd) + "] para transferir para o player[UID=" + std::to_string(_s_rcv.m_pi.uid) + "]. Hacker ou Bug",
					CL_FILE_LOG_AND_CONSOLE));

			return nullptr;
		}

		auto pWi_r = _s_rcv.m_pi.findWarehouseItemByTypeid(_psi.item._typeid);

		if (pWi_r != nullptr) { // Ele já tem atualiza

			pWi_r->STDA_C_ITEM_QNTD += (short)_psi.item.qntd;

			_psi_r.item.id = pWi_r->id;

			ret_wi = pWi_r;

			snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateItemQntd(_s_rcv.m_pi.uid, pWi_r->id, pWi_r->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);

		}else {	// Cria um novo item para ele

			WarehouseItemEx wi{ 0 };
			wi.id = _psi.item.id;
			wi._typeid = _psi.item._typeid;

			wi.type = pWi_s->type;
			wi.flag = 5; // Personal Shop /*pWi_s->flag*/;

			wi.STDA_C_ITEM_QNTD = (short)_psi.item.qntd;

			if (wi.STDA_C_ITEM_TIME > 0)
				wi.STDA_C_ITEM_TIME /= 24; // converte de novo para Dias para salvar no banco de dados

			wi.ano = -1;	// Aqui tem que colocar para minutos ou segundos(acho)

			CmdAddItem cmd_ai(_s_rcv.m_pi.uid, wi, 0, 0/*_gift_flag*/, true);	// Waiter

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_ai, nullptr, nullptr);

			cmd_ai.waitEvent();

			if (cmd_ai.getException().getCodeError() != 0)
				throw cmd_ai.getException();

			wi = cmd_ai.getInfo();
			_psi_r.item.id = wi.id;

			if (wi.id <= 0) {
				_smp::message_pool::getInstance().push(new message("[Log] nao conseguiu adicionar o Item[TYPEID=" + std::to_string(wi._typeid) + "] para o player: " + std::to_string(_s_rcv.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));
				return nullptr;
			}

			ret_wi = &_s_rcv.m_pi.mp_wi.insert(std::make_pair(wi.id, wi))->second;
		}

		// Att Item do player que vendeu
		pWi_s->STDA_C_ITEM_QNTD -= (short)_psi.item.qntd;

		if (pWi_s->STDA_C_ITEM_QNTD == 0) {
			snmdb::NormalManagerDB::getInstance().add(10, new CmdDeleteItem(_s_snd.m_pi.uid, pWi_s->id), item_manager::SQLDBResponse, nullptr);	// Delete Item

			//auto it = VECTOR_FIND_ITEM(_s_snd.m_pi.v_wi, id, == , pWi_s->id);
			auto it = _s_snd.m_pi.findWarehouseItemItById(pWi_s->id);

			if (it != _s_snd.m_pi.mp_wi.end())
				_s_snd.m_pi.mp_wi.erase(it);

		}else
			snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateItemQntd(_s_snd.m_pi.uid, pWi_s->id, pWi_s->STDA_C_ITEM_QNTD), item_manager::SQLDBResponse, nullptr);	// Update

		TRANSFER_ITEM_SUCESS_MSG_LOG("Transferiu Item");

		// Depois que que fazer um CmdPersonalShop Venda Log
		break;
	}
	case iff::PART:
	{
		auto pWi_s = _s_snd.m_pi.findWarehouseItemById(_psi.item.id);

		if (pWi_s == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player send[UID=" + std::to_string(_s_snd.m_pi.uid) + "] nao tem o Part[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] para enviar para transferir para o player recv[UID=" + std::to_string(_s_rcv.m_pi.uid) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return nullptr;
		}

		auto pWi_r = _s_rcv.m_pi.findWarehouseItemByTypeid(_psi.item._typeid);

		if (pWi_r != nullptr && !sIff::getInstance().IsCanOverlapped(pWi_r->_typeid)) {
			_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player[UID=" + std::to_string(_s_rcv.m_pi.uid) + "] tentou comprar o Part[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] que ele ja possui do player[UID=" + std::to_string(_s_snd.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			return nullptr;
		}else {

			auto part = sIff::getInstance().findPart(_psi.item._typeid);

			if (part == nullptr) {
				_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player[UID=" + std::to_string(_s_rcv.m_pi.uid) + "] tentou comprar o Part[TYPEID="
						+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] que nao esta no IFF_STRUCT do server, do player[UID=" + std::to_string(_s_snd.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

				return nullptr;
			}

			// CmdTransferPart
			snmdb::NormalManagerDB::getInstance().add(16, new CmdTransferPart(_s_snd.m_pi.uid, _s_rcv.m_pi.uid, pWi_s->id, (unsigned char)part->type_item), item_manager::SQLDBResponse, nullptr);

			// Add para o player que comprou
			ret_wi = &_s_rcv.m_pi.mp_wi.insert(std::make_pair(pWi_s->id, *pWi_s))->second;

			// Deleta do player que vendeu
			//auto it = VECTOR_FIND_ITEM(_s_snd.m_pi.v_wi, id, == , pWi_s->id);
			auto it = _s_snd.m_pi.findWarehouseItemItById(pWi_s->id);

			if (it != _s_snd.m_pi.mp_wi.end())
				_s_snd.m_pi.mp_wi.erase(it);

			TRANSFER_ITEM_SUCESS_MSG_LOG("Transferiu Part");
		}

		break;
	}
	case iff::CLUBSET:
	{
		// Vai ter mas só quando eu liberar no cliente
		auto base = sIff::getInstance().findCommomItem(_psi.item._typeid);

		if (base == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player[UID=" + std::to_string(_s_rcv.m_pi.uid) + "] tentou comprar ClubSet[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] que nao existe no IFF_STRUCT do server, do player[UID=" + std::to_string(_s_snd.m_pi.uid) +"]", CL_FILE_LOG_AND_CONSOLE));

			return nullptr;
		}

		_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Log][WARNING] Player[UID=" + std::to_string(_s_rcv.m_pi.uid) + "] tentou comprar ClubSet[TYPEID=" 
				+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] no Personal Shop[Owner UID=" + std::to_string(_s_snd.m_pi.uid) + "], mas eu ainda nao liberei " 
				+ (base->shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop ? std::string("") : std::string("no cliente e")) + " no server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case iff::CARD:
	{
		// Vai ter mas só quando eu liberar no cliente
		auto base = sIff::getInstance().findCommomItem(_psi.item._typeid);

		if(base == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player[UID=" + std::to_string(_s_rcv.m_pi.uid) + "] tentou comprar Card[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] que nao existe no IFF_STRUCT do server, do player[UID=" + std::to_string(_s_snd.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			return nullptr;
		}

		auto pCi_s = _s_snd.m_pi.findCardById(_psi.item.id);

		if (pCi_s == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player send[UID=" + std::to_string(_s_snd.m_pi.uid) + "] nao tem o Card[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] para enviar para transferir para o player recv[UID=" 
					+ std::to_string(_s_rcv.m_pi.uid) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return nullptr;
		}

		if (pCi_s->qntd < (int)_psi.item.qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player send[UID=" + std::to_string(_s_snd.m_pi.uid) + "] nao tem quantidade do Card[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + ", QNTD=" + std::to_string(_psi.item.qntd) 
					+ "] para transferir para o player[UID=" + std::to_string(_s_rcv.m_pi.uid) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return nullptr;
		}

		auto pCi_r = _s_rcv.m_pi.findCardByTypeid(_psi.item._typeid);

		if (pCi_r != nullptr) { // Ele já tem atualiza

			pCi_r->qntd += (short)_psi.item.qntd;

			_psi_r.item.id = pCi_r->id;

			ret_wi = pCi_r;

			snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateCardQntd(_s_rcv.m_pi.uid, pCi_r->id, pCi_r->qntd), item_manager::SQLDBResponse, nullptr);

		}else {	// Cria um novo item para ele

			CardInfo ci{ 0 };
			ci.id = _psi.item.id;
			ci._typeid = _psi.item._typeid;

			ci.type = pCi_s->type;

			ci.qntd = (short)_psi.item.qntd;

			CmdAddCard cmd_ac(_s_rcv.m_pi.uid, ci, 0, 0/*_gift_flag*/, true);	// Waiter

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_ac, nullptr, nullptr);

			cmd_ac.waitEvent();

			if (cmd_ac.getException().getCodeError() != 0)
				throw cmd_ac.getException();

			ci = cmd_ac.getInfo();
			_psi_r.item.id = ci.id;

			if (ci.id <= 0) {
				_smp::message_pool::getInstance().push(new message("[Log] nao conseguiu adicionar o Card[TYPEID=" + std::to_string(ci._typeid) + "] para o player: " + std::to_string(_s_rcv.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));
				return nullptr;
			}

			ret_wi = &(*_s_rcv.m_pi.v_card_info.insert(_s_rcv.m_pi.v_card_info.end(), ci));
		}

		// Att Item do player que vendeu
		pCi_s->qntd -= (short)_psi.item.qntd;

		if (pCi_s->qntd == 0) {
			snmdb::NormalManagerDB::getInstance().add(10, new CmdDeleteCard(_s_snd.m_pi.uid, pCi_s->id), item_manager::SQLDBResponse, nullptr);	// Delete Item

			//auto it = VECTOR_FIND_ITEM(_s_snd.m_pi.v_wi, id, == , pWi_s->id);
			auto it = _s_snd.m_pi.findCardItById(pCi_s->id);

			if (it != _s_snd.m_pi.v_card_info.end())
				_s_snd.m_pi.v_card_info.erase(it);

		}else
			snmdb::NormalManagerDB::getInstance().add(9, new CmdUpdateCardQntd(_s_snd.m_pi.uid, pCi_s->id, pCi_s->qntd), item_manager::SQLDBResponse, nullptr);	// Update

		TRANSFER_ITEM_SUCESS_MSG_LOG("Transferiu Card");

		/*_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Log][WARNING] Player[UID=" + std::to_string(_s_rcv.m_pi.uid) + "] tentou comprar Card[TYPEID="
				+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] no Personal Shop[Owner UID=" + std::to_string(_s_snd.m_pi.uid) + "], mas eu ainda nao liberei "
				+ (base->shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop ? std::string("") : std::string("no cliente e")) + " no server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));*/
		break;
	}
	default:	// Não suporta todos os outros, [não é permitido vender no Personal Shop]
		_smp::message_pool::getInstance().push(new message("[item_manager::transferItem][Error] player_rcv[UID=" + std::to_string(_s_rcv.m_pi.uid) + "], player_snd[UID=" + std::to_string(_s_snd.m_pi.uid) + "] Esse Item[TYPEID=" 
				+ std::to_string(_psi.item._typeid) + "] nao pode ser vendido no Personal Shop", CL_FILE_LOG_AND_CONSOLE));
		break;
	}

	if (ret_wi != nullptr)
		snmdb::NormalManagerDB::getInstance().add(15, new CmdPersonalShopLog(_s_snd.m_pi.uid, _s_rcv.m_pi.uid, _psi, _psi_r.item.id), item_manager::SQLDBResponse, nullptr);

	return ret_wi;
};

int32_t item_manager::exchangeCadieMagicBox(player& _session, uint32_t _typeid, int32_t _id, uint32_t _qntd) {

	if (!_session.getState())
		throw exception("[item_manager::exchangeCadieMagicBox][Error] player& _session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 8, 0));

	int32_t ret_id = -1;

	switch (sIff::getInstance().getItemGroupIdentify(_typeid)) {
	case iff::CADDIE:
	{
		auto pCi = _session.m_pi.findCaddieById(_id);

		if (pCi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Caddie[TYPEID=" 
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		auto caddie = sIff::getInstance().findCaddie(_typeid);

		if (caddie == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Caddie[TYPEID="
				+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (caddie->valor_mensal > 0) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar um caddie[TYPEID=" 
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] que eh por tempo, isso nao eh permitido pelo server", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		ret_id = 1;	// Passa

		break;
	}
	case iff::MASCOT:
	{
		auto pMi = _session.m_pi.findMascotById(_id);

		if (pMi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Mascot[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (_qntd != 1) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] quantidade[value=" 
					+ std::to_string(_qntd) + "] de mascot eh errado, nao pode mais que 1. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		auto mascot = sIff::getInstance().findMascot(_typeid);

		if (mascot == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Mascot[TYPEID=" 
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (mascot->shop.flag_shop.time_shop.dia > 0 && mascot->shop.flag_shop.time_shop.active) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar um Mascot[TYPEID=" 
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] que nao eh permitido[de tempo] trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		ret_id = 1;	// passa

		break;
	}
	case iff::CARD:
	{
		auto pCi = _session.m_pi.findCardById(_id);

		if (pCi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Card[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (pCi->qntd < (int)_qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar Card no CadiCauldron mais nao tem quantidade[have="
					+ std::to_string(pCi->qntd) + ", request=" + std::to_string(_qntd) + "] de item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		auto card = sIff::getInstance().findCard(_typeid);

		if (card == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Card[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		ret_id = 1;	// passa

		break;
	}
	// Warehouse Item
	case iff::AUX_PART:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o AuxPart[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (pWi->STDA_C_ITEM_QNTD < (short)_qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar AuxPart no CadiCauldron mais nao tem quantidade[have="
					+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", request=" + std::to_string(_qntd) + "] de item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		auto auxPart = sIff::getInstance().findAuxPart(_typeid);

		if (auxPart == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o AuxPart[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		ret_id = 1;	// Passa

		break;
	}
	case iff::BALL:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Ball[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (pWi->STDA_C_ITEM_QNTD < (short)_qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar Ball no CadiCauldron mais nao tem quantidade[have="
					+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", request=" + std::to_string(_qntd) + "] de item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		auto ball = sIff::getInstance().findBall(_typeid);

		if (ball == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Ball[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		ret_id = 1;	// Passa

		break;
	}
	case iff::ITEM:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		if (pWi->STDA_C_ITEM_QNTD < (short)_qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar Item no CadiCauldron mais nao tem quantidade[have=" 
					+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", request=" + std::to_string(_qntd) + "] de item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		auto item = sIff::getInstance().findItem(_typeid);

		if (item == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}
		
		ret_id = 1;	// Passa

		break;
	}
	case iff::CLUBSET:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o ClubSet no Warehouse Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		auto clubset = sIff::getInstance().findClubSet(_typeid);

		if (clubset == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o ClubSet[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		ret_id = 1; // Passa

		break;
	}
	case iff::PART:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Part no Warehouse Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}
		
		auto part = sIff::getInstance().findPart(_typeid);

		if (part == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Part[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		ret_id = 1;	// Passa

		break;
	}
	case iff::SKIN:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Skin no Warehouse Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		auto skin = sIff::getInstance().findSkin(_typeid);

		if (skin == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Skin[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return -1;
		}

		ret_id = 1;	// Passa

		break;
	}	// End Warehouse Item
	default:
	{
		_smp::message_pool::getInstance().push(new message("[item_manager::exchangeCadieMagicBox][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar um item[TYPEID=" 
				+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] que nao pode no CadieCauldron. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		break;
	}	// End Default
	}	// End Switch

	return ret_id;
};

std::vector< stItem > item_manager::exchangeTikiShop(player& _session, uint32_t _typeid, int32_t _id, uint32_t _qntd) {
	
	if (!_session.getState())
		throw exception("[item_manager::exchangeTikiShop][Error] player& _session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 8, 0));

	std::vector< stItem > v_item;
	stItem item{ 0 };

	switch (sIff::getInstance().getItemGroupIdentify(_typeid)) {
	case iff::CADDIE:
	{

		if (_qntd > 1) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem duplicata de Caddie[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto pCi = _session.m_pi.findCaddieById(_id);

		if (pCi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Caddie[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto caddie = sIff::getInstance().findCaddie(_typeid);

		if (caddie == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Caddie[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (caddie->valor_mensal > 0) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar um caddie[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] que eh por tempo, isso nao eh permitido pelo server", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		item.clear();

		item.type = 2;
		item.id = pCi->id;
		item._typeid = pCi->_typeid;
		item.qntd = _qntd;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		v_item.push_back(item);

		break;
	}
	case iff::MASCOT:
	{

		if (_qntd > 1) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem duplicata de Mascot[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto pMi = _session.m_pi.findMascotById(_id);

		if (pMi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Mascot[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (_qntd != 1) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] quantidade[value="
					+ std::to_string(_qntd) + "] de mascot eh errado, nao pode mais que 1. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto mascot = sIff::getInstance().findMascot(_typeid);

		if (mascot == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Mascot[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (mascot->shop.flag_shop.time_shop.dia > 0 && mascot->shop.flag_shop.time_shop.active) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar um Mascot[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] que nao eh permitido[de tempo] trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		item.clear();

		item.type = 2;
		item.id = pMi->id;
		item._typeid = pMi->_typeid;
		item.qntd = _qntd;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		v_item.push_back(item);

		break;
	}
	case iff::CARD:
	{
		auto pCi = _session.m_pi.findCardById(_id);

		if (pCi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Card[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (pCi->qntd < (int)_qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar Card no Tiki Shop mais nao tem quantidade[have="
					+ std::to_string(pCi->qntd) + ", request=" + std::to_string(_qntd) + "] de item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto card = sIff::getInstance().findCard(_typeid);

		if (card == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Card[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		item.clear();

		item.type = 2;
		item.id = pCi->id;
		item._typeid = pCi->_typeid;
		item.qntd = _qntd;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		v_item.push_back(item);

		break;
	}
	// Warehouse Item
	case iff::AUX_PART:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o AuxPart[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if ((pWi->flag & 0x20) == 0x20 || (pWi->flag & 0x40) == 0x40 || (pWi->flag & 0x60) == 0x60) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou trocar AuxPart no Tiki Shop mais nao pode trocar item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] de tempo. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (pWi->STDA_C_ITEM_QNTD < (short)_qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar AuxPart no Tiki Shop mais nao tem quantidade[have="
					+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", request=" + std::to_string(_qntd) + "] de item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto auxPart = sIff::getInstance().findAuxPart(_typeid);

		if (auxPart == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o AuxPart[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (_session.m_pi.isAuxPartEquiped(_typeid)) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode trocar o AuxPart[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] equipado no Tiki Shop. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		item.clear();

		item.type = 2;
		item.id = pWi->id;
		item._typeid = pWi->_typeid;
		item.qntd = _qntd;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		v_item.push_back(item);

		break;
	}
	case iff::BALL:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Ball[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if ((pWi->flag & 0x20) == 0x20 || (pWi->flag & 0x40) == 0x40 || (pWi->flag & 0x60) == 0x60) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar Ball no Tiki Shop mais nao pode trocar item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] de tempo. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (pWi->STDA_C_ITEM_QNTD < (short)_qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar Ball no Tiki Shop mais nao tem quantidade[have="
					+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", request=" + std::to_string(_qntd) + "] de item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto ball = sIff::getInstance().findBall(_typeid);

		if (ball == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Ball[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		item.clear();

		item.type = 2;
		item.id = pWi->id;
		item._typeid = pWi->_typeid;
		item.qntd = _qntd;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		v_item.push_back(item);

		break;
	}
	case iff::ITEM:
	{
		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if ((pWi->flag & 0x20) == 0x20 || (pWi->flag & 0x40) == 0x40 || (pWi->flag & 0x60) == 0x60) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar Item no Tiki Shop mais nao pode trocar item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] de tempo. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (pWi->STDA_C_ITEM_QNTD < (short)_qntd) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar Item no Tiki Shop mais nao tem quantidade[have="
					+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", request=" + std::to_string(_qntd) + "] de item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto iff_item = sIff::getInstance().findItem(_typeid);

		if (iff_item == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		item.clear();

		item.type = 2;
		item.id = pWi->id;
		item._typeid = pWi->_typeid;
		item.qntd = _qntd;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		v_item.push_back(item);

		break;
	}
	case iff::CLUBSET:
	{

		if (_qntd > 1) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem duplicata de ClubSet[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o ClubSet no Warehouse Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if ((pWi->flag & 0x20) == 0x20 || (pWi->flag & 0x40) == 0x40 || (pWi->flag & 0x60) == 0x60) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar ClubSet no Tiki Shop mais nao pode trocar item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] de tempo. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto clubset = sIff::getInstance().findClubSet(_typeid);

		if (clubset == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o ClubSet[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		item.clear();

		item.type = 2;
		item.id = pWi->id;
		item._typeid = pWi->_typeid;
		item.qntd = _qntd;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		v_item.push_back(item);

		break;
	}
	case iff::PART:
	{
		auto part = sIff::getInstance().findPart(_typeid);

		if (part == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Part[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if (_qntd > 0) {
			auto pWi_all = _session.m_pi.findAllPartNotEquiped(_typeid);

			if (pWi_all.size() < _qntd) {
				_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Part no Warehouse Item[TYPEID="
						+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] quantidade[resq=" + std::to_string(_qntd) + ", value=" + std::to_string(pWi_all.size()) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

				return std::vector< stItem >();
			}

			if (_qntd == 1) {
				auto pWi = _session.m_pi.findWarehouseItemById(_id);

				if (pWi == nullptr) {
					_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Warehouse Item[TYPEID="
							+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					return std::vector< stItem >();
				}

				if ((pWi->flag & 96) == 96/*Rental não pode trocar no Tiki Shop*/ || (pWi->flag & 0x20) == 0x20 || (pWi->flag & 0x40) == 0x40) {
					_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao pode trocar Part Rental[TYPEID="
							+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no Tiki Shop. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					return std::vector< stItem >();
				}

				item.clear();

				item.type = 2;
				item.id = pWi->id;
				item._typeid = pWi->_typeid;
				item.qntd = 1;
				item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

				v_item.push_back(item);
			}else  {
				for (auto i = 0u; i < _qntd; ++i) {
					item.clear();

					item.type = 2;
					item.id = pWi_all[i]->id;
					item._typeid = pWi_all[i]->_typeid;
					item.qntd = 1;
					item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

					v_item.push_back(item);
				}
			}
		}

		break;
	}
	case iff::SKIN:
	{
		if (_qntd > 1) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem duplicata de Skin[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto pWi = _session.m_pi.findWarehouseItemById(_id);

		if (pWi == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Skin no Warehouse Item[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] para trocar. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		if ((pWi->flag & 0x20) == 0x20 || (pWi->flag & 0x40) == 0x40 || (pWi->flag & 0x60) == 0x60) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar Skin no Tiki Shop mais nao pode trocar item[TYPEID=" + std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] de tempo. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		auto skin = sIff::getInstance().findSkin(_typeid);

		if (skin == nullptr) {
			_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o Skin[TYPEID="
					+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			return std::vector< stItem >();
		}

		item.clear();

		item.type = 2;
		item.id = pWi->id;
		item._typeid = pWi->_typeid;
		item.qntd = _qntd;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		v_item.push_back(item);

		break;
	}	// End Warehouse Item
	default:
	{
		_smp::message_pool::getInstance().push(new message("[item_manager::exchangeTikiShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar um item[TYPEID="
				+ std::to_string(_typeid) + ", ID=" + std::to_string(_id) + "] que nao pode no CadieCauldron. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		break;
	}	// End Default
	}	// End Switch

	return v_item;
};

void item_manager::openTicketReportScroll(player& _session, int32_t _ticket_scroll_item_id, int32_t _ticket_scroll_id, bool _upt_on_game) {
	
	if (!_session.getState())
		throw exception("[item_manager::openTicketReportScrool][Error] player[UID=" + std::to_string(_session.m_pi.uid) + ", OID=" 
				+ std::to_string(_session.m_oid) + "] mas a session is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 2550, 0));

	packet p;

	try {

		if (_ticket_scroll_item_id < 0 || _ticket_scroll_id < 0)
			throw exception("[item_manager::openTicketReportScroll][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou abrir Ticket Teport Scroll[ITEM_ID=" 
					+ std::to_string(_ticket_scroll_item_id) + ", ID=" + std::to_string(_ticket_scroll_id) + "], mas o ticket_scroll item ou id is invalid. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 2500, 0));

		auto pWi = _session.m_pi.findWarehouseItemById(_ticket_scroll_item_id);

		if (pWi == nullptr)
			throw exception("[item_manager::openTicketReportScroll][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou abrir Ticket Teport Scroll[ITEM_ID="
					+ std::to_string(_ticket_scroll_item_id) + ", ID=" + std::to_string(_ticket_scroll_id) + "], mas ele nao tem o ticket_scroll item. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 2501, 0));

		if (((pWi->c[1] * 0x800) | pWi->c[2]) != _ticket_scroll_id)
			throw exception("[item_manager::openTicketReportScroll][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou abrir Ticket Teport Scroll[ITEM_ID="
					+ std::to_string(_ticket_scroll_item_id) + ", ID=" + std::to_string(_ticket_scroll_id) + "], mas ticket_scroll id nao bate com o do item[VALUE="
					+ std::to_string((pWi->c[1] * 0x800) | pWi->c[2]) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 2502, 0));

		CmdTicketReportDadosInfo cmd_trdi(_ticket_scroll_id, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_trdi, nullptr, nullptr);

		cmd_trdi.waitEvent();

		if (cmd_trdi.getException().getCodeError() != 0)
			throw cmd_trdi.getException();

		auto trsi = cmd_trdi.getInfo();

		// Remove o Ticket Report Scroll Item
		stItem item{ 0 };

		item.type = 2;
		item.id = pWi->id;
		item._typeid = pWi->_typeid;
		item.qntd = pWi->STDA_C_ITEM_QNTD;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		if (item_manager::removeItem(item, _session) <= 0)
			throw exception("[item_manager::openTicketReportScroll][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou abrir Ticket Teport Scroll[ITEM_ID="
					+ std::to_string(_ticket_scroll_item_id) + ", ID=" + std::to_string(_ticket_scroll_id) + "], mas nao conseguiu deletar Ticket Report Scroll item. Bug",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 2503, 0));

		// Check if it is on Update Item Map
		auto ui_it = _session.m_pi.findUpdateItemByIdAndType(_ticket_scroll_item_id, UpdateItem::WAREHOUSE);

		// Add Experiencia se ele ganhou
		auto it = std::find_if(trsi.v_players.begin(), trsi.v_players.end(), [&](auto& _el) {
			return _el.uid == _session.m_pi.uid;
		});

		uint32_t exp = 0u;

		// Guarda exp para enviar depois que enviar o pacote do ticker report open
		if (it != trsi.v_players.end() && it->exp > 0)
			exp = it->exp;

		// Tempo do Item Acabou, Exclui ele do Server, DB and Game, e manda a resposta de erro para o Game(player)
		if (ui_it != _session.m_pi.mp_ui.end()) {

			if (_upt_on_game) {
				// Exclui do Game
				p.init_plain((unsigned short)0x216);

				p.addUint32((const uint32_t)GetSystemTimeAsUnix());
				p.addUint32(1);	// Count

				p.addUint8(item.type);
				p.addUint32(item._typeid);
				p.addInt32(item.id);
				p.addUint32(item.flag_time);
				p.addBuffer(&item.stat, sizeof(item.stat));
				p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
				p.addZeroByte(25);

				packet_func::session_send(p, &_session, 1);
			}

			// Exclui Update Item
			_session.m_pi.mp_ui.erase(ui_it);

			// Add experiência do player e enviar o pacote de experiência
			if (exp > 0u)
				_session.addExp(exp, _upt_on_game/*Update ON Game*/);

			// Resposta para o cliente
			throw exception("[item_manager::openTicketReportScroll][Error] player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou abrir Ticket Report Scroll[ITEM_ID=" + std::to_string(_ticket_scroll_item_id) + ", ID=" + std::to_string(_ticket_scroll_id) + ", END_DATE="
					+ formatDateLocal(pWi->end_date_unix_local) + ", EXP=" + std::to_string(exp) 
					+ "], mas o tempo do item expirou. Da so a exp para o player.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 2504, 0));
		}
		// End Check Time Item Ticket Scroll

		// Log
		_smp::message_pool::getInstance().push(new message("[item_manager::openTicketReportScroll][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] abriu Ticket Report Scroll[ITEM_ID=" 
				+ std::to_string(_ticket_scroll_item_id) + ", ID=" + std::to_string(_ticket_scroll_id) + ", EXP=" + std::to_string(exp) + "]", CL_FILE_LOG_AND_CONSOLE));

		// Reposta para o cliente
		p.init_plain((unsigned short)0x11A);

		p.addUint32((uint32_t)trsi.v_players.size());

		p.addBuffer(&trsi.date, sizeof(SYSTEMTIME));

		for (auto& el : trsi.v_players)
			p.addBuffer(&el, sizeof(TicketReportScrollInfo::stPlayerDados));

		packet_func::session_send(p, &_session, 1);

		// Tem que add exp aqui depois do pacote11A por que att antes e depois o pacote11A começa a contar a experiência do exp já atualizado, 
		// aí o visual fica errado, mas no info fica tudo certo
		// Add experiência do player e enviar o pacote de experiência
		if (exp > 0u)
			_session.addExp(exp, _upt_on_game/*Update ON Game*/);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[item_manager::openTicketReportScroll][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Reposta Error;
		p.init_plain((unsigned short)0x11A);

		p.addInt32(-1);	// Error
		p.addZeroByte(16);	// Date

		packet_func::session_send(p, &_session, 1);
	}
};

bool item_manager::isSetItem(uint32_t _typeid) {
	return sIff::getInstance().getItemGroupIdentify(_typeid) == iff::SET_ITEM;
};

bool item_manager::isTimeItem(stItem::stDate& _date) {
	return (_date.active || _date.date.sysDate[0].wYear != 0 || _date.date.sysDate[1].wYear != 0);
};

bool item_manager::isTimeItem(stItem::stDate::stDateSys& _date) {
	return (_date.sysDate[0].wYear != 0 || _date.sysDate[1].wYear != 0);
};

bool item_manager::ownerItem(uint32_t _uid, uint32_t _typeid) {

	bool ret = false;

	// Procura primeiro no Dolfini Locker o item, se for diferente de SetItem
	if (sIff::getInstance().getItemGroupIdentify(_typeid) != iff::SET_ITEM) {
		
		CmdFindDolfiniLockerItem cmd_dli(_uid, _typeid, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_dli, nullptr, nullptr);

		cmd_dli.waitEvent();

		if (cmd_dli.getException().getCodeError() != 0)
			throw cmd_dli.getException();

		if (cmd_dli.hasFound())
			return true;
	}
	// Find de busca em dolfini locker

	switch (sIff::getInstance().getItemGroupIdentify(_typeid)) {
	case iff::CHARACTER:
	{
		CmdFindCharacter cmd_fc(_uid, _typeid, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_fc, nullptr, nullptr);

		cmd_fc.waitEvent();

		if (cmd_fc.getException().getCodeError() != 0)
			throw cmd_fc.getException();

		ret = cmd_fc.hasFound();

		break;
	}
	case iff::CADDIE:
	{
		CmdFindCaddie cmd_fc(_uid, _typeid, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_fc, nullptr, nullptr);

		cmd_fc.waitEvent();

		if (cmd_fc.getException().getCodeError() != 0)
			throw cmd_fc.getException();

		ret = cmd_fc.hasFound();

		break;
	}
	case iff::MASCOT:
	{
		auto mi = _ownerMascot(_uid, _typeid);

		ret = (mi.id > 0)/*hasFound*/; //cmd_fm.hasFound();
		
		break;
	}
	case iff::CARD:
	{
		auto card = _ownerCard(_uid, _typeid);

		ret = (card.id > 0)/*hasFound*/;	//cmd_fc.hasFound();
		
		break;
	}
	case iff::FURNITURE:
	{
		CmdFindFurniture cmd_ff(_uid, _typeid, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_ff, nullptr, nullptr);

		cmd_ff.waitEvent();

		if (cmd_ff.getException().getCodeError() != 0)
			throw cmd_ff.getException();

		ret = cmd_ff.hasFound();
		
		break;
	}
	case iff::BALL:
	case iff::AUX_PART:
	case iff::CLUBSET:
	case iff::ITEM:
	case iff::PART:
	case iff::SKIN:
	{
		auto aux_part = _ownerAuxPart(_uid, _typeid);

		ret = (aux_part.id > 0)/*hasFound*/; //cmd_fwi.hasFound();
		
		break;
	}
	case iff::SET_ITEM:
		ret = ownerSetItem(_uid, _typeid);
		break;
	case iff::HAIR_STYLE:
		ret = ownerHairStyle(_uid, _typeid);
		break;
	case iff::CAD_ITEM:		// Esse aqui verifica se já tem, mas não que não pode ter mais. mas sim para aumentar o tempo
		ret = ownerCaddieItem(_uid, _typeid);
		break;
	case iff::MATCH:
	{
		auto tsi = _ownerTrofelEspecial(_uid, _typeid);

		ret = (tsi.id > 0)/*hasFound*/;

		break;
	} // End iff::MATCH
	} // End Switch

	// Player não tem o item no warehouse e nem no Dolfini Locker, Verifica no Mail Box dele
	if (!ret) {

		// Procura no Mail Box do player
		ret = ownerMailBoxItem(_uid, _typeid);
	}

	return ret;
};

bool item_manager::ownerSetItem(uint32_t _uid, uint32_t _typeid) {

	auto set = sIff::getInstance().findSetItem(_typeid);

	if (set != nullptr) {
		for (auto i = 0u; i < (sizeof(set->packege.item_typeid) / sizeof(set->packege.item_typeid[0])); ++i) {
			// Eleminar a verificação do character que ele só inclui se o player não tiver ele
			// se ele tiver não faz diferença não anula o verificação do set
			if (set->packege.item_typeid[i] != 0 && sIff::getInstance().getItemGroupIdentify(set->packege.item_typeid[i]) != iff::CHARACTER)
				if (ownerItem(_uid, set->packege.item_typeid[i]))	// se tiver 1 item que seja não pode ganhar o set se não vai duplicar os itens, que ele tem
					return true;
		}
	}

	return false;
};

bool item_manager::ownerCaddieItem(uint32_t _uid, uint32_t _typeid) {

	auto caddie = _ownerCaddieItem(_uid, _typeid);

	if (!(caddie.id > 0)/*!hasFound*/)	// Não tem o caddie então não pode ter o caddie item
		return true;

	// Pode enviar o mesmo
	/*if (caddie.parts_typeid == _typeid)	// Tem a Parts
		return true;*/

	return false;
};

CaddieInfoEx item_manager::_ownerCaddieItem(uint32_t _uid, uint32_t _typeid) {

	CmdFindCaddie cmd_fc(_uid, (iff::CADDIE << 26) | sIff::getInstance().getCaddieIdentify(_typeid), true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fc, nullptr, nullptr);

	cmd_fc.waitEvent();

	if (cmd_fc.getException().getCodeError() != 0)
		throw cmd_fc.getException();

	return cmd_fc.getInfo();
};

CharacterInfo item_manager::_ownerHairStyle(uint32_t _uid, uint32_t _typeid) {
	
	auto hair = sIff::getInstance().findHairStyle(_typeid);

	if (hair != nullptr) {

		CmdFindCharacter cmd_fc(_uid, (iff::CHARACTER << 26) | hair->character, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_fc, nullptr, nullptr);

		cmd_fc.waitEvent();

		if (cmd_fc.getException().getCodeError() != 0)
			throw cmd_fc.getException();

		return cmd_fc.getInfo();
	}

	return {0};/*CharacterInfo Vazio*/
};

MascotInfoEx item_manager::_ownerMascot(uint32_t _uid, uint32_t _typeid) {
	
	CmdFindMascot cmd_fm(_uid, _typeid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fm, nullptr, nullptr);

	cmd_fm.waitEvent();

	if (cmd_fm.getException().getCodeError() != 0)
		throw cmd_fm.getException();

	return cmd_fm.getInfo();
};

WarehouseItemEx item_manager::_ownerBall(uint32_t _uid, uint32_t _typeid) {
	
	CmdFindWarehouseItem cmd_fwi(_uid, _typeid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fwi, nullptr, nullptr);

	cmd_fwi.waitEvent();

	if (cmd_fwi.getException().getCodeError() != 0)
		throw cmd_fwi.getException();

	return cmd_fwi.getInfo();
};

CardInfo item_manager::_ownerCard(uint32_t _uid, uint32_t _typeid) {
	
	CmdFindCard cmd_fc(_uid, _typeid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fc, nullptr, nullptr);

	cmd_fc.waitEvent();

	if (cmd_fc.getException().getCodeError() != 0)
		throw cmd_fc.getException();

	return cmd_fc.getInfo();
};

WarehouseItemEx item_manager::_ownerAuxPart(uint32_t _uid, uint32_t _typeid) {
	
	CmdFindWarehouseItem cmd_fwi(_uid, _typeid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fwi, nullptr, nullptr);

	cmd_fwi.waitEvent();

	if (cmd_fwi.getException().getCodeError() != 0)
		throw cmd_fwi.getException();

	return cmd_fwi.getInfo();
};

WarehouseItemEx item_manager::_ownerItem(uint32_t _uid, uint32_t _typeid) {
	
	CmdFindWarehouseItem cmd_fwi(_uid, _typeid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fwi, nullptr, nullptr);

	cmd_fwi.waitEvent();

	if (cmd_fwi.getException().getCodeError() != 0)
		throw cmd_fwi.getException();

	return cmd_fwi.getInfo();
};

TrofelEspecialInfo item_manager::_ownerTrofelEspecial(uint32_t _uid, uint32_t _typeid) {
	
	auto type_trofel = sIff::getInstance().getItemSubGroupIdentify24(_typeid);
	
	CmdFindTrofelEspecial::eTYPE type = CmdFindTrofelEspecial::eTYPE::ESPECIAL;

	if (type_trofel == 1 || type_trofel == 2)
		type = CmdFindTrofelEspecial::eTYPE::ESPECIAL;
	else if (type_trofel == 3)
		type = CmdFindTrofelEspecial::eTYPE::GRAND_PRIX;

	CmdFindTrofelEspecial cmd_fts(_uid, _typeid, type, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fts, nullptr, nullptr);

	cmd_fts.waitEvent();

	if (cmd_fts.getException().getCodeError() != 0)
		throw cmd_fts.getException();

	return cmd_fts.getInfo();
};

bool item_manager::ownerHairStyle(uint32_t _uid, uint32_t _typeid) {
	
	auto hair = sIff::getInstance().findHairStyle(_typeid);

	if (hair != nullptr) {
		auto character = _ownerHairStyle(_uid, _typeid);

		if (!(character.id > 0)/*!hasFound*/)	// Não tem o Character
			return true;

		if (character.default_hair == hair->cor)
			return true;
	}

	return false;
};

bool item_manager::ownerMailBoxItem(uint32_t _uid, uint32_t _typeid) {

	CmdFindMailBoxItem cmd_fmbi(_uid, _typeid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fmbi, nullptr, nullptr);

	cmd_fmbi.waitEvent();

	if (cmd_fmbi.getException().getCodeError() != 0)
		throw cmd_fmbi.getException();

	if (cmd_fmbi.hasFound())
		return true;

	return false;
};

bool item_manager::betweenTimeSystem(stItem::stDate& _date) {

	if (!isTimeItem(_date))
		throw exception("[item_manager::betweenTimeSystem][Error] Item nao e um item de tempo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_ITEM_MANAGER, 3, 0));

	FILETIME ft1{ 0 }, ft2{ 0 }, ft3{ 0 };

	SYSTEMTIME st{ 0 };

	GetLocalTime(&st);

	SystemTimeToFileTime(&_date.date.sysDate[0], &ft1);
	SystemTimeToFileTime(&_date.date.sysDate[1], &ft2);
	SystemTimeToFileTime(&st, &ft3);

	// Date de termino é 0, então passa a do LocalTime, por que ele só tem a data de quando começa
	if (ft2.dwHighDateTime == 0 && ft2.dwLowDateTime == 0)
		ft2 = ft3;

	return (reinterpret_cast<ULARGE_INTEGER*>(&ft1)->QuadPart <= reinterpret_cast<ULARGE_INTEGER*>(&ft3)->QuadPart && reinterpret_cast<ULARGE_INTEGER*>(&ft3)->QuadPart <= reinterpret_cast<ULARGE_INTEGER*>(&ft2)->QuadPart);
};

bool item_manager::betweenTimeSystem(IFF::DateDados& _date) {
	return betweenTimeSystem(*(stItem::stDate*)&_date);
};

bool item_manager::betweenTimeSystem(stItem::stDate::stDateSys& _date) {
	
	stItem::stDate date{ 0, _date };
	
	return betweenTimeSystem(date);
};

void item_manager::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
#ifdef _DEBUG
		// Static class
		_smp::message_pool::getInstance().push(new message("[item_manager::SQLDBResponse][WARNING] _arg is nullptr na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		return;
	}

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[item_manager::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// isso aqui depois pode mudar para o Item_manager, que vou tirar de ser uma classe static e usar ela como objeto(instancia)
	//auto _session = reinterpret_cast< player* >(_arg);

	switch (_msg_id) {
	case 1:	// Add Character
	{
		// Não usa mais
		break;
	}
	case 2:	// add Caddie
	{
		// Não usa mais
		break;
	}
	case 3:	// add Warehouse
	{
		// Não usa mais
		break;
	}
	case 4:	// Add HairStyle
	{
		// Esse aqui só tem att o cabelo no character do player
		break;
	}
	case 5: // Caddie Item
	{
		// Esse aqui só att o parts typeid do caddie e o tempo no DB, não precisa esperar a resposta do DB já que ele não retorna nada
		break;
	}
	case 6: // Update Mascot Time
	{
		// Não usa mais
		break;
	}
	case 7:	// Update Ball Quantidade
	{
		// Não usa mais
		break;
	}
	case 8:	// Update Card Quantidade
	{
		// Não usa mais
		break;
	}
	case 9: // Update Item Quantidade
	{
		// Não usa mais
		break;
	}
	case 10: // Delete Item
	{
		// Não usa mais
		break;
	}
	case 11: // Delete Ball
	{
		// Não usa mais
		break;
	}
	case 12: // Delete Card
	{
		// Não usa mais
		break;
	}
	case 13: // Gift ClubSet
	{
		// Não usa mais
		break;
	}
	case 14: // Gift Part
	{
		// Não usa mais
		break;
	}
	case 15: // Personal Shop Log
	{
		// Não usa por que não retorna nada é um INSERT
		break;
	}
	case 16:	// Personal Shop Transfer Part
	{
		// Não usa por que não retorna nada é um UPDATE
		break;
	}
	case 17:	// Update Character Equipped
	{
		// Não usa por que não retorna nada é um UPDATE
		break;
	}
	case 18:	// Update Trofel Especial e Grand Prix
	{
		// Não usa por que não retorna nada é um UPDATE
		break;
	}
	case 19:	// Update Premium Ticket Time
	{
		// Não usa por que não retorna nada é um UPDATE
		break;
	}
	case 20:	// Update Clubset Time
	{
		// Não uda por que não retorna nada é um UPDATE
		break;
	}
	case 0:
	default:
		break;
	}
};
