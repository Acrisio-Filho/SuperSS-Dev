// Arquivo personal_shop_manager.cpp
// Criado em 01/04/2022 as 19:32 por Acrisio
// Implementação da classe PersonalShopManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "personal_shop_manager.hpp"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../PACKET/packet_func_sv.h"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

using namespace stdA;

PersonalShopManager::PersonalShopManager(RoomInfoEx& _ri) : m_ri(_ri), m_shops() {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
	InitializeConditionVariable(&m_cv);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	pthread_cond_init(&m_cv, nullptr);
#endif
}

PersonalShopManager::~PersonalShopManager() {

	destroy();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
	pthread_cond_destroy(&m_cv);
#endif
}

void PersonalShopManager::destroy() {

	clear_shops();
}

bool PersonalShopManager::hasNameInSomeShop(std::string _name, uint32_t _owner_uid) {

	bool ret = false;

	try {

		// lock
		lock();

		ret = _hasNameInSomeShop(_name, _owner_uid);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::hasNameInSomeShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
	
	return ret;
}

PersonalShop* PersonalShopManager::findShop(player* _session) {

	PersonalShop* ret = nullptr;

	try {

		// lock
		lock();

		ret = _findShop(_session);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::findShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

PersonalShop* PersonalShopManager::findShop(uint32_t _owner_uid) {

	PersonalShop* ret = nullptr;

	try {

		// lock
		lock();

		ret = _findShop(_owner_uid);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::findShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

PersonalShopManager::mapShop::iterator PersonalShopManager::findShopIt(player* _session) {

	mapShop::iterator ret = m_shops.end();

	try {

		// lock
		lock();

		ret = _findShopIt(_session);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::findShopIt][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

PersonalShopManager::mapShop::iterator PersonalShopManager::findShopIt(uint32_t _owner_uid) {

	mapShop::iterator ret = m_shops.end();

	try {

		// lock
		lock();

		ret = _findShopIt(_owner_uid);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::findShopIt][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void PersonalShopManager::delete_shop(player* _session) {

	try {

		// lock
		lock();

		_delete_shop(_session);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::delete_shop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void PersonalShopManager::delete_shop(mapShop::iterator _it_shop) {

	try {

		// lock
		lock();

		_delete_shop(_it_shop);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::delete_shop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool PersonalShopManager::isItemForSale(player& _session, int32_t _item_id) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	try {

		// Sala não é Lounge, não tem como o player abrir um shop
		if (m_ri.tipo != RoomInfo::TIPO::LOUNGE)
			return false;

		auto ps = findShop(&_session);

		// Player não tem um shop aberto na sala
		if (ps == nullptr)
			return false;

		// O item não está à venda no shop do player
		if (ps->findItemById(_item_id) == nullptr)
			return false;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::isItemForSale][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	// O item está à venda no shop do player
	return true;
}

PlayerRoomInfo::PersonShop PersonalShopManager::getPersonShop(player& _session) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);
	
	PlayerRoomInfo::PersonShop person{ 0u };

	do {

		if (m_ri.tipo != RoomInfo::TIPO::LOUNGE)
			break;

		auto ps = findShop(&_session);

		if (ps == nullptr)
			break;

		if (ps->getState() == PersonalShop::STATE::OPEN_EDIT)
			break;

		person.active = 1u;

		STRCPY_TO_MEMORY_FIXED_SIZE(person.name, sizeof(person.name), ps->getName().c_str());

	} while (0);

	return person;
}

void PersonalShopManager::destroyShop(player& _session) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_DELETE);

	try {

		delete_shop(&_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::destroyShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool PersonalShopManager::openShopToEdit(player& _session, packet& _out_packet) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;

	try {

		if (m_ri.tipo != RoomInfo::TIPO::LOUNGE)
			throw exception("[PersonalShopManager::openShopToEdit][Error][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou abri um personal shop para venda em uma sala[TIPO="
					+ std::to_string((unsigned short)m_ri.tipo) + ", NUMERO=" + std::to_string(m_ri.numero) + "] diferente de Lounge. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 100, 52001001));

		if (_session.m_pi.block_flag.m_flag.stBit.personal_shop)
			throw exception("[PersonalShopManager::openShopToEdit][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou abrir um personal shop para vender na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas ele nao pode. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 4, 0x790001));

		// Verifica o level do player e bloquea se não tiver level Beginner E
		if (_session.m_pi.level < _session.m_pi.enLEVEL::BEGINNER_E)
			throw exception("[PersonalShopManager::openShopToEdit][Error] player[UID=" + std::to_string(_session.m_pi.uid) + ", LEVEL=" + std::to_string(_session.m_pi.level)
					+ "] tentou abrir um personal shop para vender na sala[NUMERO=" + std::to_string(m_ri.numero) + "], mas o level dele eh menor que Beginner E.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 3, 0));

		if (m_shops.size() >= (uint32_t)(m_ri.max_player * 0.8f))
			throw exception("[PersonalShopManager::openShopToEdit][Log] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] chegou no limite de shop(s) permitidos na sala[NUMERO=" + std::to_string(m_ri.numero) + "]", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 101, 5200102));

		PersonalShop *ps = nullptr;

		// Esse aqui não é para da esse erro por que o pacote que pede para editar a loja, é esse também
		if ((ps = findShop(&_session)) != nullptr) {

			ps->setState(PersonalShop::OPEN_EDIT);

			_smp::message_pool::getInstance().push(new message("[PersonalShopManager::openShopToEdit][Log] player[UID=" 
					+ std::to_string(_session.m_pi.uid) + "] Editando Personal Shop", CL_FILE_LOG_AND_CONSOLE));

		}else {

			// lock
			lock();

			// Cria um Personal Shop para o player, por que ele não tem 1
			auto r = m_shops.insert(std::make_pair(&_session, PersonalShopCtx(new PersonalShop(_session), eTYPE_LOCK::TL_SELECT, 1)));

			if (!r.second && (r.first == m_shops.end() || r.first->first == nullptr || r.first->first != (&_session))) {

				// unlock
				unlock();

				// Error, fail to insert personal shop into map
				throw exception("[PersonalShopManager::openShopToEdit][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] nao conseguiu adicionar o shop do player para ao map.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 701, 5200701));
			}

			// new personal shop
			ps = r.first->second.m_shop;

			// unlock
			unlock();
		}

		// Log
		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::openShopToEdit][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] abriu Personal Shop[Owner UID="
				+ std::to_string(ps->getOwner().m_pi.uid) + ", STATE=" + std::to_string(ps->getState()) + ", Name=" + ps->getName() + ", Item Count="
				+ std::to_string(ps->getCountItem()) + ", Pang Sale=" + std::to_string(ps->getPangSale()) + "] para editar na sala[numero=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));

		// Sucesso
		_out_packet.init_plain((unsigned short)0xE5);

		_out_packet.addUint32(1);

		_out_packet.addString(_session.m_pi.nickname);
		_out_packet.addUint32(_session.m_pi.uid);

		// sucesso, enviar o pacote para a sala toda
		return true;
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::openShopToEdit][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xE5);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200100);

		packet_func::session_send(p, &_session, 1);
	}

	return false;
}

bool PersonalShopManager::cancelEditShop(player& _session, packet& _out_packet) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;

	try {

		PersonalShop *ps = nullptr;

		if ((ps = findShop(&_session)) == nullptr)
			throw exception("[PersonalShopManager::cancelEditShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou cancela edit Personal Shop, mas ele nao tem nenhum na sala. Hacker ou Bug",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 400, 5200401));

		ps->setState(PersonalShop::OPEN);

		_out_packet.init_plain((unsigned short)0xE3);

		_out_packet.addUint32(1);	// OK

		_out_packet.addString(_session.m_pi.nickname);

		// sucesso, enviar o pacote para a sala toda
		return true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::cancelEditShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xE3);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200400);

		packet_func::session_send(p, &_session, 1);
	}

	return false;
}

bool PersonalShopManager::closeShop(player& _session, packet& _out_packet) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_DELETE);

	packet p;

	try {

		// Verifica se o player tem um shop aberto
		PersonalShop *ps = nullptr;

		if ((ps = findShop(&_session)) == nullptr)
			throw exception("[PersonalShopManager::closeShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem um personal shop criado.", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 150, 5200151));

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::closeShop][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] fechou o Personal Shop[Owner UID="
				+ std::to_string(ps->getOwner().m_pi.uid) + ", STATE=" + std::to_string(ps->getState()) + ", NAME=" + ps->getName() + ", Count Item="
				+ std::to_string(ps->getCountItem()) + ", Pang Sale=" + std::to_string(ps->getPangSale()) + "] na sala[numero=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));

		// Deleta shop
		delete_shop(&_session);

		// Sucesso
		_out_packet.init_plain((unsigned short)0xE4);

		_out_packet.addUint32(1);

		_out_packet.addString(_session.m_pi.nickname);
		_out_packet.addUint32(_session.m_pi.uid);

		// sucesso, enviar o pacote para a sala toda
		return true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::closeShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xE5);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200150);

		packet_func::session_send(p, &_session, 1);
	}

	return false;
}

bool PersonalShopManager::changeShopName(player& _session, std::string _name, packet& _out_packet) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;

	try {

		if (_name.empty())
			throw exception("[PersonalShopManager::changeShopName][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou trocar o no do shop mas enviou uma string vazia. Hacker ou Bug",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 200, 5200201));

		// Verifica se esse nome de shop já existe na sala, tirando o dele é claro
		if (hasNameInSomeShop(_name, _session.m_pi.uid))
				throw exception("[PersonalShopManager::changeShopName][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar o name[value="
						+ _name + "] do Personal Shop dele, but already exists on room.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 201, 5200202));

		PersonalShop *ps = nullptr;

		if ((ps = findShop(&_session)) == nullptr)
			throw exception("[PersonalShopManager::changeShopName][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] nao tem um personal shop nessa sala. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 202, 5200203));

		ps->setName(_name);

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::changeShopName][Log] player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] trocou o nome[VALUE=" + _name + "] do seu Personal Shop com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		_out_packet.init_plain((unsigned short)0xE8);

		_out_packet.addUint32(1);	// Ok

		_out_packet.addString(ps->getName());

		_out_packet.addUint32(_session.m_pi.uid);

		_out_packet.addString(_session.m_pi.nickname);

		// sucesso, enviar o pacote para a sala toda
		return true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::changeShopName][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xE8);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200200);

		packet_func::session_send(p, &_session, 1);
	}

	return false;
}

void PersonalShopManager::openShop(player& _session, packet *_packet) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;
	PersonalShopItem psi{ 0 };

	try {

		uint32_t count = _packet->readUint32();

		if (count > 6)
			throw exception("[PersonalShopManager::openShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou abrir Personal Shop com um numero[value="
					+ std::to_string(count) + "] de itens eh maior que o permitido", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 250, 5200251));

		PersonalShop *ps = nullptr;

		if ((ps = findShop(&_session)) == nullptr)
			throw exception("[PersonalShopManager::openShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou abrir um Personal Shop que ele nao tem.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 251, 5200252));

		// Limpa os itens do Shop se tiver
		ps->clearItem();

		for (auto i = 0u; i < count; ++i) {
			psi.clear();

			_packet->readBuffer(&psi, sizeof(PersonalShopItem));

			// Dentro do push ele verifica se é permitido esse item no Personal Shop
			ps->pushItem(psi);
		}

		// Abre o Shop
		ps->setState(PersonalShop::OPEN);

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::openShop][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] abriu o Personal Shop[NAME="
				+ ps->getName() + ", Count Item=" + std::to_string(ps->getCountItem()) + ", Pang Sale=" + std::to_string(ps->getPangSale()) + "] ", CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xEB);

		p.addUint32(1);	// Ok

		p.addBuffer(_session.m_pi.nickname, sizeof(_session.m_pi.nickname));

		p.addUint32(_session.m_pi.uid);

		ps->putItemOnPacket(p);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PERSONAL_SHOP, 23)) {

			p.init_plain((unsigned short)0x40);	// Msg to Chat of player

			p.addUint8(7);	// Notice

			p.addString(_session.m_pi.nickname);
			p.addString("Card price is outside the price range.");

			packet_func::session_send(p, &_session, 1);
		}

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::openShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xEB);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200250);

		packet_func::session_send(p, &_session, 1);
	}
}

void PersonalShopManager::buyInShop(player& _session, packet *_packet) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;

	try {

		uint32_t owner_uid = _packet->readUint32();

		PersonalShopItem psi{ 0 };

		_packet->readBuffer(&psi, sizeof(PersonalShopItem));

		PersonalShop *ps = nullptr;

		if ((ps = findShop(owner_uid)) == nullptr)
			throw exception("[room::requestBuyItemSaleShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou comprar item[TYPEID=" + std::to_string(psi.item._typeid) + ", ID=" + std::to_string(psi.item.id) + "] no Shop[Owner UID="
					+ std::to_string(owner_uid) + "], mas ele nao tem um shop nesta nessa sala[numero=" + std::to_string(m_ri.numero) + "]. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 551, 5200552));

		ps->buyItem(_session, psi);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::buyInShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xEC);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200550);

		packet_func::session_send(p, &_session, 1);
	}
}

void PersonalShopManager::visitCountShop(player& _session) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;

	try {

		PersonalShop *ps = nullptr;

		if ((ps = findShop(&_session)) == nullptr)
			throw exception("[PersonalShopManager::visitCountShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou pedir visit count do Personal Shop, mas ele nao tem na sala. Hacker ou Bug",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 300, 5200301));

		p.init_plain((unsigned short)0xE9);

		p.addUint32(1);	// OK

		p.addUint32(ps->getVisitCount());

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::visitCountShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xE9);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200300);

		packet_func::session_send(p, &_session, 1);
	}
}

void PersonalShopManager::pangShop(player& _session) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;

	try {

		PersonalShop *ps = nullptr;

		if ((ps = findShop(&_session)) == nullptr)
			throw exception("[PersonalShopManager::pangShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou pedir pang sale do Personal Shop, mas ele nao tem na sala. Hacker ou Bug",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 350, 5200351));

		p.init_plain((unsigned short)0xEA);

		p.addUint32(1);	// OK

		p.addUint64(ps->getPangSale());

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::pangShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xEA);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200350);

		packet_func::session_send(p, &_session, 1);
	}
}

void PersonalShopManager::viewShop(player& _session, uint32_t _owner_uid) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;

	try {

		PersonalShop *ps = nullptr;

		if ((ps = findShop(_owner_uid)) == nullptr)
			throw exception("[PersonalShopManager::viewShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ver o Shop[Owner UID="
					+ std::to_string(_owner_uid) + "], mas ele nao tem um shop nesta nessa sala[numero=" + std::to_string(m_ri.numero) + "]. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 451, 5200452));

		try {
			// Add Client
			ps->addClient(_session);
		}catch (exception& e) {

			if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PERSONAL_SHOP, 6)) {

				_smp::message_pool::getInstance().push(new message("[PersonalShopManager::viewShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

				throw exception("[PersonalShopManager::viewShop][Log] " + e.getFullMessageError(), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 452, 5200453));
			}
			else
				throw;
		}

		p.init_plain((unsigned short)0xE6);

		p.addUint32(1);	// OK

		p.addBuffer(ps->getOwner().m_pi.nickname, sizeof(PlayerInfo::nickname));

		p.addString(ps->getName());

		p.addUint32(ps->getOwner().m_pi.uid);

		ps->putItemOnPacket(p);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::viewShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xE6);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200450);

		packet_func::session_send(p, &_session, 1);
	}
}

void PersonalShopManager::closeViewShop(player& _session, uint32_t _owner_uid) {

	Locker _locker(*this, _session.m_pi.uid, eTYPE_LOCK::TL_SELECT);

	packet p;

	try {

		PersonalShop *ps = nullptr;

		if ((ps = findShop(_owner_uid)) == nullptr)
			throw exception("[PersonalShopManager::closeViewShop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou fechar o Shop[Owner UID="
					+ std::to_string(_owner_uid) + "], mas ele nao tem um shop nesta nessa sala[numero=" + std::to_string(m_ri.numero) + "]. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER, 501, 5200502));

		// deleta viewer
		ps->deleteClient(_session);

		// Sucesso
		p.init_plain((unsigned short)0xE7);

		p.addUint32(1);	// OK

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::closeViewShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0xE7);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PERSONAL_SHOP_MANAGER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 5200500);

		packet_func::session_send(p, &_session, 1);
	}
}

void PersonalShopManager::lock() {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif
}

void PersonalShopManager::unlock() {
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void PersonalShopManager::clear_shops() {

	try {
		
		// lock
		lock();

		while (!m_shops.empty()) {

			auto it = m_shops.begin();

			if (it->second.m_type != eTYPE_LOCK::TL_NONE && waitSpinDown())
				continue;

			if (it->second.m_shop != nullptr)
				delete it->second.m_shop;

			it->second.m_shop = nullptr;

			m_shops.erase(it);
		}

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::clear_shops][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool PersonalShopManager::_hasNameInSomeShop(std::string _name, uint32_t _owner_uid) {

	return std::find_if(m_shops.begin(), m_shops.end(), [&_owner_uid, &_name](auto& _el) {

		return _el.second.m_shop != nullptr && _el.second.m_shop->getOwner().m_pi.uid != _owner_uid && _el.second.m_shop->getName().compare(_name) == 0;

	}) != m_shops.end();
}

PersonalShop* PersonalShopManager::_findShop(player* _session) {
	
	auto it = _findShopIt(_session);

	return it == m_shops.end() ? nullptr : it->second.m_shop;
}

PersonalShop* PersonalShopManager::_findShop(uint32_t _owner_uid) {

	auto it = _findShopIt(_owner_uid);

	return it == m_shops.end() ? nullptr : it->second.m_shop;
}

PersonalShopManager::mapShop::iterator PersonalShopManager::_findShopIt(player* _session) {
	return m_shops.find(_session);
}

PersonalShopManager::mapShop::iterator PersonalShopManager::_findShopIt(uint32_t _owner_uid) {
	return std::find_if(m_shops.begin(), m_shops.end(), [&_owner_uid](auto& _el) {
		return _el.second.m_shop != nullptr && _el.second.m_shop->getOwner().m_pi.uid == _owner_uid;
	});
}

void PersonalShopManager::_delete_shop(player* _session) {
	_delete_shop(m_shops.find(_session));
}

void PersonalShopManager::_delete_shop(mapShop::iterator _it_shop) {

	if (_it_shop != m_shops.end()) {

		if (_it_shop->second.m_shop != nullptr)
			delete _it_shop->second.m_shop;

		_it_shop->second.m_shop = nullptr;

		m_shops.erase(_it_shop);
	}
}

bool PersonalShopManager::waitSpinDown() {

#if defined(_WIN32)
	if (SleepConditionVariableCS(&m_cv, &m_cs, INFINITE) == 0) {

		DWORD error = GetLastError();
			
		LeaveCriticalSection(&(m_cs));
			
		if (error == ERROR_TIMEOUT)
			_smp::message_pool::getInstance().push(new message("[PersonalShopManager::waitSpinDown][Error] time out, but _milliseconds is INFINITE. wrong and unknown error. Error Code: " 
					+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));
		else
			_smp::message_pool::getInstance().push(new message("[PersonalShopManager::waitSpinDown][Error] ao receber o sinal da condition variable. Error Code: " 
					+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

		return false;
	}
#elif defined(__linux__)
	int error = 0;
	if ((error = pthread_cond_wait(&m_cv, &m_cs)) != 0) {
		
		pthread_mutex_unlock(&m_cs);
		
		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::waitSpinDown][Error] ao receber sinal the condition variable. Error Code: " 
				+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

		return false;
	}
#endif

	return true;
}

void PersonalShopManager::wakeAllLocked() {
#if defined(_WIN32)
	WakeAllConditionVariable(&m_cv);
#elif defined(__linux__)
	pthread_cond_broadcast(&m_cv);
#endif
}

void PersonalShopManager::spinUp(uint32_t _owner_uid, eTYPE_LOCK _type) {

	try {

		mapShop::iterator it = m_shops.end();
		bool bContinue = false;

		// lock
		lock();

		do {

			it = _findShopIt(_owner_uid);

			if (it == m_shops.end())
				break;

			if (it->second.m_type == eTYPE_LOCK::TL_NONE) {

				it->second.m_type = _type;
				it->second.m_count = 1;

			}else if (it->second.m_type == eTYPE_LOCK::TL_SELECT && _type == eTYPE_LOCK::TL_SELECT)
				it->second.m_count++;
			else
				bContinue = waitSpinDown();

		} while (bContinue);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::spinUp][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void PersonalShopManager::spinDown(uint32_t _owner_uid, eTYPE_LOCK _type) {

	try {

		mapShop::iterator it = m_shops.end();

		// lock
		lock();

		do {

			it = _findShopIt(_owner_uid);

			if (it == m_shops.end())
				break;

			if (it->second.m_type != _type)
				_smp::message_pool::getInstance().push(new message("[PersonalShopManager::spinDown][Error] type lock not match, CTX_TYPE: " 
						+ std::to_string((uint32_t)it->second.m_type) + " != " + std::to_string((uint32_t)_type), CL_FILE_LOG_AND_CONSOLE));

			if (it->second.m_count <= 1) {

				it->second.m_count = 0;
				it->second.m_type = eTYPE_LOCK::TL_NONE;
			
				wakeAllLocked();

			}else
				it->second.m_count--;

		} while (0);

		// unlock
		unlock();

	}catch (exception& e) {

		// unlock
		unlock();

		_smp::message_pool::getInstance().push(new message("[PersonalShopManager::spinDown][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

PersonalShopManager::Locker::Locker(PersonalShopManager& _manager, uint32_t _owner_uid, eTYPE_LOCK _type)
	: m_manager(_manager), m_owner_uid(_owner_uid), m_type(_type) {

	m_manager.spinUp(m_owner_uid, m_type);
}

PersonalShopManager::Locker::~Locker() {

	m_manager.spinDown(m_owner_uid, m_type);
}
