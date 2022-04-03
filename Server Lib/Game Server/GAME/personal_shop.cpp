// Arquivo personal_shop.cpp
// Criado em 09/06/2018 as 10:06 por Acrisio
// Implement��o da PersonalShop

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "personal_shop.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../PACKET/packet_func_sv.h"

#include "item_manager.h"

#include "../UTIL/sys_achievement.hpp"

// Card limit price
constexpr uint32_t CARD_NORMAL_LIMIT_PRICE		= 200000u;
constexpr uint32_t CARD_RARE_LIMIT_PRICE		= 400000u;
constexpr uint32_t CARD_SUPER_RARE_LIMIT_PRICE	= 1000000u;
constexpr uint32_t CARD_SECRET_LIMIT_PRICE		= 2000000u;

// Shop min and max price item
constexpr uint32_t ITEM_MIN_PRICE = 1u;
constexpr uint32_t ITEM_MAX_PRICE = 9999999u;

using namespace stdA;

PersonalShop::PersonalShop(player& _session) 
	: m_owner(_session), m_name(), m_visit_count(0u), m_pang_sale(0ull), m_state(OPEN_EDIT) {
	
#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

PersonalShop::~PersonalShop() {
	destroy();
	clearItem();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void PersonalShop::destroy() {

	Locker _locker(*this);

	if (!v_open_shop_visit.empty()) {
		v_open_shop_visit.clear();
		v_open_shop_visit.shrink_to_fit();
	}
}

std::string& PersonalShop::getName() {

	Locker _locker(*this);

	return m_name;
}

uint32_t PersonalShop::getVisitCount() {
	
	Locker _locker(*this);

	return m_visit_count;
}

uint64_t PersonalShop::getPangSale() {

	Locker _locker(*this);

	return m_pang_sale;
}

player& PersonalShop::getOwner() {

	Locker _locker(*this);

	return m_owner;
}

PersonalShop::STATE PersonalShop::getState() {

	Locker _locker(*this);

	return m_state;
}

uint32_t PersonalShop::getCountItem() {

	Locker _locker(*this);

	return (uint32_t)v_item.size();
}

std::vector< player* >& PersonalShop::getClients() {

	Locker _locker(*this);

	return v_open_shop_visit;
}

void PersonalShop::setName(std::string& _name) {

	Locker _locker(*this);
	
	if (_name.empty())
		throw exception("[PersonalShop::setName][Error] _name is empty", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 1, 0));

	m_name = _name;
}

void PersonalShop::setState(STATE _state) {

	Locker _locker(*this);

	m_state = _state;
}

void PersonalShop::clearItem() {

	Locker _locker(*this);

	if (!v_item.empty()) {
		v_item.clear();
		v_item.shrink_to_fit();
	}
}

void PersonalShop::pushItem(PersonalShopItem& _psi) {

	Locker _locker(*this);

	// Verifica aqui se esse item por ser colocar no shop

	if (_psi.item._typeid == 0)
		throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um invalid item no Personal Shop dele. Hacker ou Bug", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 7, 0));

	auto base = sIff::getInstance().findCommomItem(_psi.item._typeid);

	if (base == nullptr)
		throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um item[TYPEID=" 
				+ std::to_string(_psi.item._typeid) + "] que nao existe no IFF_STRUCT do server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 8, 0));

	if (!base->shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop)
		throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um item[TYPEID=" 
				+ std::to_string(_psi.item._typeid) + "] que nao pode ser vendido no Personal Shop. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 9, 0));

	// Verifica o pre�o do item
	if (_psi.item.pang < ITEM_MIN_PRICE || _psi.item.pang > ITEM_MAX_PRICE)
		throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um item[TYPEID="
				+ std::to_string(_psi.item._typeid) + ", Price=" + std::to_string(_psi.item.pang) + "] que o preco esta fora do limite[MIN="
				+ std::to_string(ITEM_MIN_PRICE) + ", MAX=" + std::to_string(ITEM_MAX_PRICE) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 24, 0));

	// Card pre�o controle
	if (sIff::getInstance().getItemGroupIdentify(_psi.item._typeid) == iff::CARD) {

		auto card = sIff::getInstance().findCard(_psi.item._typeid);

		if (card == nullptr)
			throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um card[TYPEID="
					+ std::to_string(_psi.item._typeid) + "] que nao existe no IFF_STRUCT do server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 21, 0));

		switch (card->tipo) {
		case 0: // Normal
			if (_psi.item.pang > CARD_NORMAL_LIMIT_PRICE)
				throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um card[TYPEID="
						+ std::to_string(_psi.item._typeid) + ", TYPE=Normal, Price=" + std::to_string(_psi.item.pang) + "] que o preco passa do limite(" 
						+ std::to_string(CARD_NORMAL_LIMIT_PRICE) + "). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 23, 0));
			break;
		case 1:	// Rare
			if (_psi.item.pang > CARD_RARE_LIMIT_PRICE)
				throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um card[TYPEID="
						+ std::to_string(_psi.item._typeid) + ", TYPE=Rare, Price=" + std::to_string(_psi.item.pang) + "] que o preco passa do limite("
						+ std::to_string(CARD_RARE_LIMIT_PRICE) + "). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 23, 0));
			break;
		case 2: // Super Rare
			if (_psi.item.pang > CARD_SUPER_RARE_LIMIT_PRICE)
				throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um card[TYPEID="
						+ std::to_string(_psi.item._typeid) + ", TYPE=Super Rare, Price=" + std::to_string(_psi.item.pang) + "] que o preco passa do limite("
						+ std::to_string(CARD_SUPER_RARE_LIMIT_PRICE) + "). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 23, 0));
			break;
		case 3: // Secret
			if (_psi.item.pang > CARD_SECRET_LIMIT_PRICE)
				throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um card[TYPEID="
						+ std::to_string(_psi.item._typeid) + ", TYPE=Secret, Price=" + std::to_string(_psi.item.pang) + "] que o preco passa do limite("
						+ std::to_string(CARD_SECRET_LIMIT_PRICE) + "). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 23, 0));
			break;
		default: // Unknown Type
			throw exception("[PersonalShop::pushItem][Error] player[UID=" + std::to_string(m_owner.m_pi.uid) + "] tentou colocar um card[TYPEID="
					+ std::to_string(_psi.item._typeid) + ", TYPE=" + std::to_string((unsigned short)card->tipo) 
					+ "] que o tipo eh desconhecido. (N,R,SR e SC) Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 22, 0));
			break;
		}
	}

	v_item.push_back(_psi);
}

void PersonalShop::deleteItem(PersonalShopItem& _psi) {

	Locker _locker(*this);

	auto item = findItemIndexById(_psi.item.id);

	if (v_item[item].item.id == _psi.item.id)
		v_item.erase(v_item.begin() + item);
	else {
		for (auto ii = v_item.begin(); ii != v_item.end(); ++ii) {
			if (ii->item.id == _psi.item.id) {
				v_item.erase(ii);
				break;
			}
		}
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PersonalShop::deleteItem][Log] Delete Item[TYPEID=" + std::to_string(_psi.item._typeid) 
			+ "] do Personal Shop[Owner UID=" + std::to_string(m_owner.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif
}

void PersonalShop::putItemOnPacket(packet& _p) {

	Locker _locker(*this);

	if (v_item.size() == 0)
		throw exception("[PersonalShop::putItemOnPacket][Error] size vector item shop is zero", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 2, 0));

	_p.addUint32((uint32_t)v_item.size());

	for (auto i = 0u; i < v_item.size(); ++i)
		_p.addBuffer(&v_item[i], sizeof(PersonalShopItem));
}

PersonalShopItem* PersonalShop::findItemById(int32_t _id) {

	if (_id <= 0)
		throw exception("[PersonalShop::findItemById][Error] _id[value=" + std::to_string(_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 3, 0));

	Locker _locker(*this);
	
	PersonalShopItem *psi = nullptr;

	for (auto& el : v_item) {
		if (el.item.id == _id) {
			psi = &el;
			break;
		}
	}

	return psi;
}

PersonalShopItem* PersonalShop::findItemByIndex(uint32_t _index) {

	Locker _locker(*this);

	PersonalShopItem *psi = nullptr;

	for (auto& el : v_item) {
		if (el.index == _index) {
			psi = &el;
			break;
		}
	}

	return psi;
}

int32_t PersonalShop::findItemIndexById(int32_t _id) {

	Locker _locker(*this);

	int32_t index = -1;

	for (auto i = 0u; i < v_item.size(); ++i) {
		if (v_item[i].item.id == _id) {
			index = (int)i;
			break;
		}
	}

	return index;
}

player* PersonalShop::findClientByUID(uint32_t _uid) {

	Locker _locker(*this);

	player* client = nullptr;

	for (auto& el : v_open_shop_visit) {
		if (el->m_pi.uid == _uid) {
			client = el;
			break;
		}
	}

	return client;
}

int32_t PersonalShop::findClientIndexByUID(uint32_t _uid) {

	Locker _locker(*this);

	int32_t index = -1;

	for (auto i = 0u; i < v_open_shop_visit.size(); ++i) {
		if (v_open_shop_visit[i]->m_pi.uid == _uid) {
			index = (int)i;
			break;
		}
	}

	return index;
}

void PersonalShop::addClient(player& _session) {

	Locker _locker(*this);

	if (m_state != STATE::OPEN)
		throw exception("[PersonalShop::addClient][Error] client[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] tentou entrar no shop do player[UID=" + std::to_string(m_owner.m_pi.uid) + "], mas ele nao esta aberto no momento. Hacker ou Bug.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 25, 0));

	auto client = findClientByUID(_session.m_pi.uid);

	if (client != nullptr)
		throw exception("[PersonalShop::addClient][Error] client[UID=" + std::to_string(_session.m_pi.uid) + "] ja existe no Personal Shop do player[UID=" 
				+ std::to_string(m_owner.m_pi.uid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 4, 0));
	
	if (v_open_shop_visit.size() >= LIMIT_VISIT_ON_SAME_TIME)
		throw exception("[PersonalShop::addClient][Log] client[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] nao pode entrar no shop por que ja chegou ao limit de clientes ao mesmo tempo no Personal Shop do player[UID=" 
				+ std::to_string(m_owner.m_pi.uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 6, 0));

	v_open_shop_visit.push_back(&_session);

	// Add Contador de visitas
	m_visit_count++;

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PersonalShop::addClient][Log] Add client[UID=" + std::to_string(_session.m_pi.uid) 
			+ "] ao Personal Shop do player[UID=" + std::to_string(m_owner.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif
}

void PersonalShop::deleteClient(player& _session) {

	Locker _locker(*this);

	auto client = findClientIndexByUID(_session.m_pi.uid);

	if (client == -1)
		throw exception("[PersonalShop::deleteClient][Error] client[UID=" + std::to_string(_session.m_pi.uid) + "] nao existe no vector de clientes do Personal Shop do player[UID=" 
				+ std::to_string(m_owner.m_pi.uid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 5, 0));

	if (v_open_shop_visit[client]->m_pi.uid == _session.m_pi.uid)
		v_open_shop_visit.erase(v_open_shop_visit.begin() + client);
	else {
		for (auto ii = v_open_shop_visit.begin(); ii != v_open_shop_visit.end(); ++ii) {
			if ((*ii)->m_pi.uid == _session.m_pi.uid) {
				v_open_shop_visit.erase(ii);
				break;
			}
		}
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PersonalShop::deleteClient][Log] Delete client[UID=" + std::to_string(_session.m_pi.uid) 
			+ "] do Personal Shop do player[UID=" + std::to_string(m_owner.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif
}

void PersonalShop::buyItem(player& _session, PersonalShopItem& _psi) {

	Locker _locker(*this);

	if (m_state != STATE::OPEN)
		throw exception("[PersonalShop::buyItem][Error] client[UID=" + std::to_string(_session.m_pi.uid)
				+ "] tentou comprar no shop do player[UID=" + std::to_string(m_owner.m_pi.uid) + "], mas ele nao esta aberto no momento. Hacker ou Bug.",
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 25, 0));
	
	if (findClientByUID(_session.m_pi.uid) == nullptr)
		throw exception("[PersonalShop::buyItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar item[TYPEID=" + std::to_string(_psi.item._typeid) 
				+ ", ID=" + std::to_string(_psi.item.id) + "] no Shop[Owner UID="
				+ std::to_string(m_owner.m_pi.uid) + "], mas ele nao esta no shop do player. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 10, 0));

	auto psi_owner = findItemById(_psi.item.id);

	if (psi_owner == nullptr || psi_owner->index != _psi.index)
		throw exception("[PersonalShop::buyItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar item[TYPEID="
				+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + "] que nao tem no Shop[Owner UID="
				+ std::to_string(m_owner.m_pi.uid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 11, 0));

	if (psi_owner->item._typeid == 0)
		throw exception("[PersonalShop::buyItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar item[TYPEID="
				+ std::to_string(psi_owner->item._typeid) + ", ID=" + std::to_string(psi_owner->item.id) + "] invalido no Shop[Owner UID="
				+ std::to_string(m_owner.m_pi.uid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 12, 0));

	auto base = sIff::getInstance().findCommomItem(psi_owner->item._typeid);

	if (base == nullptr)
		throw exception("[PersonalShop::buyItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar item[TYPEID="
				+ std::to_string(psi_owner->item._typeid) + ", ID=" + std::to_string(psi_owner->item.id) + "] invalido que nao tem no IFF_STRUCT do server, no Shop[Owner UID="
				+ std::to_string(m_owner.m_pi.uid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 13, 0));

	if (!base->shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop)
		throw exception("[PersonalShop::buyItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar item[TYPEID="
				+ std::to_string(psi_owner->item._typeid) + ", ID=" + std::to_string(psi_owner->item.id) + "] que nao pode ser vendido no Personal Shop, no Shop[Owner UID="
				+ std::to_string(m_owner.m_pi.uid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 14, 0));

	if (_session.m_pi.ui.pang < (psi_owner->item.pang * _psi.item.qntd))
		throw exception("[PersonalShop::buyItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar item[TYPEID="
				+ std::to_string(psi_owner->item._typeid) + ", ID=" + std::to_string(psi_owner->item.id) + "] mas ele nao tem pangs suficiente, no Shop[Owner UID="
				+ std::to_string(m_owner.m_pi.uid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 15, 0));

	if (!base->level.goodLevel((unsigned char)_session.m_pi.level))
		throw exception("[PersonalShop::buyItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar item[TYPEID="
				+ std::to_string(psi_owner->item._typeid) + ", ID=" + std::to_string(psi_owner->item.id) + "] mas ele nao tem level suficiente, no Shop[Owner UID="
				+ std::to_string(m_owner.m_pi.uid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 16, 0));

	if (_psi.item.qntd > psi_owner->item.qntd)
		throw exception("[PersonalShop::buyItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comprar item[TYPEID="
				+ std::to_string(psi_owner->item._typeid) + ", ID=" + std::to_string(psi_owner->item.id) + ", QNTD=" 
				+ std::to_string(psi_owner->item.qntd) + "] mas ele quer comprar uma quantidade(" 
				+ std::to_string(_psi.item.qntd) + ") do item maior do que esta a venda. ", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 26, 0));

	// Depois o Personal Shop vai poder vender card, ent�o tem que procurar nos card tbm
	
	uint64_t pang = psi_owner->item.pang * _psi.item.qntd;

	// c�pia do personalshopitem, para enviar para o player que comprou que muda o id, quando compra card ou item, part e clubset ele so transfere msm,
	// mas pode ser que no pangya original ele cria um novo e s� transferi os dados
	auto psi_r = _psi;

	//WarehouseItemEx* pWi = nullptr;
	void* pWi = nullptr;

	if ((pWi = item_manager::transferItem(m_owner, _session, _psi, psi_r)) != nullptr) {

		_smp::message_pool::getInstance().push(new message("[Personal Shop::BuyItemSaleShop][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] comprou item[TYPEID="
				+ std::to_string(_psi.item._typeid) + ", ID=" + std::to_string(_psi.item.id) + ", QNTD=" + std::to_string(_psi.item.qntd) + ", PANG(qntd*unit)="
				+ std::to_string((_psi.item.qntd * psi_owner->item.pang)) + "] do Personal Shop[Owner UID=" + std::to_string(m_owner.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

		if (psi_owner->item.qntd == _psi.item.qntd)
			deleteItem(_psi);
		else {

			psi_owner->item.qntd -= _psi.item.qntd;

		}

		// ATUALIZA OS PANGS DO PLAYER NO DB, CHAMNANDO AS FUN��ES QUE J� CRIEI, MAS POR HORA S� VOU MEXER NO SERVER
		_session.m_pi.consomePang(pang);

		// Tira os 5% dos pangs
		m_owner.m_pi.addPang((pang = (uint64_t)std::round(pang * 0.95f)));

		// Pang(s) do Personal Shop
		m_pang_sale += pang;

		// Att no Jogo
		packet p;

		// Card
		if (sIff::getInstance().getItemGroupIdentify(_psi.item._typeid) == iff::CARD) {

			auto ci_r = *reinterpret_cast< CardInfo* >(pWi);

			// Tira de quem vendeu
			p.init_plain((unsigned short)0xEC);

			p.addUint32(1);	// OK

			p.addUint8(1);	// 1 Tira Item

			p.addUint64(pang);

			p.addBuffer(&_psi, sizeof(PersonalShopItem));

			p.addUint8(5/*Card*/);

			// Att id por que esta o novo item do player que vai receber o item
			ci_r.id = _psi.item.id;
			ci_r.qntd = _psi.item.qntd;

			p.addBuffer(&ci_r, sizeof(CardInfo));

			packet_func::session_send(p, &m_owner, 1);

			// Esse Pacote � o de add ele soma, n�o atualiza do zero como o que tira do player
			ci_r.id = psi_r.item.id;
			ci_r.qntd = (short)psi_r.item.qntd;

			// Add para quem comprou, aqui � s� para mostrar a tela que comprou e liberar o player
			p.init_plain((unsigned short)0xEC);

			p.addUint32(1);	// OK

			p.addUint8(0);	// 0 Add Item

			p.addUint64(_session.m_pi.ui.pang);

			p.addBuffer(&psi_r, sizeof(PersonalShopItem));

			p.addUint8(5/*Card*/);

			p.addBuffer(&ci_r, sizeof(CardInfo));

			packet_func::session_send(p, &_session, 1);

		}else { // WarehouseItem
			
			auto wi_r = *reinterpret_cast< WarehouseItemEx* >(pWi);

			// Tira de quem vendeu
			p.init_plain((unsigned short)0xEC);

			p.addUint32(1);	// OK

			p.addUint8(1);	// 1 Tira Item

			p.addUint64(pang);

			p.addBuffer(&_psi, sizeof(PersonalShopItem));

			p.addUint8((sIff::getInstance().getItemGroupIdentify(_psi.item._typeid) == iff::ITEM) ? 1 : 3/*ClubSet ou Parts*/);

			// Att id por que esta o novo item do player que vai receber o item
			wi_r.id = _psi.item.id;
			wi_r.STDA_C_ITEM_QNTD = (short)_psi.item.qntd;

			p.addBuffer(&wi_r, sizeof(WarehouseItem));

			packet_func::session_send(p, &m_owner, 1);

			// Esse Pacote � o de add ele soma, n�o atualiza do zero como o que tira do player
			wi_r.id = psi_r.item.id;
			wi_r.STDA_C_ITEM_QNTD = (short)psi_r.item.qntd;

			// Add para quem comprou
			p.init_plain((unsigned short)0xEC);

			p.addUint32(1);	// OK

			p.addUint8(0);	// 0 Add Item

			p.addUint64(_session.m_pi.ui.pang);

			p.addBuffer(&psi_r, sizeof(PersonalShopItem));

			p.addUint8((sIff::getInstance().getItemGroupIdentify(psi_r.item._typeid) == iff::ITEM) ? 1 : 3/*ClubSet ou Parts*/);

			p.addBuffer(&wi_r, sizeof(WarehouseItem));

			packet_func::session_send(p, &_session, 1);
		}

		// Atualiza no Shop os Itens que foram comprado
		p.init_plain((unsigned short)0xED);

		p.addString(m_owner.m_pi.nickname);

		p.addUint32(m_owner.m_pi.uid);

		p.addBuffer(&_psi, sizeof(PersonalShopItem));

		p.addUint32(v_item.size() == 0 ? 3/*Shop fechou por que nao tem mais item*/ : 1/*Shop aberto ainda*/);

		shop_broadcast(p, &_session, 1);

		// Update Achievement ON SERVER, DB and GAME
		SysAchievement sys_achieve;

		sys_achieve.incrementCounter(0x6C400083u/*Buy Personal Shop*/);

		sys_achieve.finish_and_update(_session);

	}else
		throw exception("[PersonalShop::buyItem][Error] nao conseguiu transferir o item[TYPEID=" + std::to_string(_psi.item._typeid) + ", ID=" 
				+ std::to_string(_psi.item.id) + "] da venda do personal shop, do player[UID=" + std::to_string(m_owner.m_pi.uid) + "] para o player[UID=" + std::to_string(_session.m_pi.uid) + "]", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 19, 0));
}

void PersonalShop::_lock() {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif
}

void PersonalShop::_unlock() {
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

inline void PersonalShop::shop_broadcast(packet& _p, session *_s, unsigned char _debug) {

	if (_s == nullptr)
		throw exception("[PersonalShop::shop_broadcast][Error] Session *_s is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 20, 0));

	Locker _locker(*this);

	auto& clients = v_open_shop_visit;

	// Envia para o dono do Personal Shop
	packet_func::session_send(_p, &m_owner, 1);

	for (auto& el : clients)
		packet_func::session_send(_p, el, _debug);
}

inline void PersonalShop::shop_broadcast(std::vector< packet* > _v_p, session *_s, unsigned char _debug) {

	if (_s == nullptr)
		throw exception("[PersonalShop::shop_broadcast][Error] Session *_s is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP, 20, 0));

	Locker _locker(*this);

	auto& clients = v_open_shop_visit;

	// Envia para o dono do Personal Shop
	packet_func::session_send(_v_p, &m_owner, 1);

	for (auto& el : clients)
		packet_func::session_send(_v_p, el, _debug);
}

PersonalShop::Locker::Locker(PersonalShop& _shop) : m_shop(_shop) {
	m_shop._lock();
}

PersonalShop::Locker::~Locker() {
	m_shop._unlock();
}
