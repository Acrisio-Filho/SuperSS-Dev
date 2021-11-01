// Arquivo premium_system.cpp
// Criado em 28/10/2020 as 16:40 por Acrisio
// Implementa��o da classe PremiumSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "premium_system.hpp"

#include "item_manager.h"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include "../PACKET/packet_func_sv.h"

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[PremiumSystem" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 1, 0)); \

// !@ tempor�rio
constexpr uint32_t PREMIUM_TICKET_TYPEID = 0x1A100002u;
// !@ tempor�rio
constexpr uint32_t PREMIUM_2_TICKET_TYPEID = 0x1A100003u;

// !@ tempor�rio
constexpr uint32_t PREMIUM_BALL_TYPEID = 0x140000D8u;
// !@ tempor�rio
constexpr uint32_t PREMIUM_2_BALL_TYPEID = 0x140000E9u; // Sakura (Premium)

// !@ tempor�rio
constexpr uint32_t PREMIUM_2_CLUBSET_TYPEID = 0x100000F7u; // Rank D(0x1000005D), Rank S(0x1000006B), (Premium)

// !@ tempor�rio
constexpr uint32_t PREMIUM_2_AUTO_CALIPER_TYPEID = 0x1A000040u;

// !@ tempor�rio
constexpr uint32_t PREMIUM_2_MASCOT_TYPEID = 0x4000004Bu; // Lolo (Premium)

using namespace stdA;

PremiumSystem::PremiumSystem() {
}

PremiumSystem::~PremiumSystem() {
}

void PremiumSystem::checkEndTimeTicket(player& _session) {
	CHECK_SESSION_BEGIN("checkEndTimeTicket");

	try {

		if (isPremiumTicket(_session.m_pi.pt._typeid) && _session.m_pi.pt.id != 0 && _session.m_pi.pt.unix_sec_date <= 0) {

			WarehouseItemEx ticket{ 0 };

			auto it = _session.m_pi.findWarehouseItemItByTypeid(_session.m_pi.pt._typeid);

			if (it == _session.m_pi.mp_wi.end()) {

				ticket = item_manager::_ownerItem(_session.m_pi.uid, _session.m_pi.pt._typeid);

				if (ticket.id <= 0) {
					_smp::message_pool::getInstance().push(new message("[PremiumSystem::checkEndTimeTicket][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem o item Ticket Premium. Bug", CL_FILE_LOG_AND_CONSOLE));

					return;
				}

				// Add o Ticket Premium User para o map do player, para poder excluir ele
				_session.m_pi.mp_wi.insert(std::make_pair(ticket.id, ticket));

			}else
				ticket = it->second;

			stItem item{ 0 };

			item.type = 2;
			item.id = ticket.id;
			item._typeid = ticket._typeid;
			item.qntd = ticket.STDA_C_ITEM_QNTD;
			item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

			// UPDATE ON SERVER AND DB
			if (item_manager::removeItem(item, _session) <= 0)
				throw exception("[PremiumSystem::checkEndTimeTicket][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou excluir ticket premium user, mas nao conseguiu deletar ele. Bug",
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 10000, 0));

			// Log
			_smp::message_pool::getInstance().push(new message("[PremiumSystem::checkEndTimeTicket][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "].\tExcluiu ticket premium do player.", CL_ONLY_FILE_LOG));

			packet p;

			// UPDATE ON GAME
			packet_func::pacote26D(p, &_session, _session.m_pi.pt.unix_end_date);
			packet_func::session_send(p, &_session, 0);

			// Zera o Premium User Ticket que ele j� n�o tem mais
			_session.m_pi.pt.clear();
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PremiumSystem::checkEndTimeTicket][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void PremiumSystem::addPremiumUser(player& _session, WarehouseItemEx& _ticket, uint32_t _time) {
	CHECK_SESSION_BEGIN("addPremiumUser");

	try {

		// Inicializa o PremiumTicket estrutura do player
		_session.m_pi.pt.id = _ticket.id;
		_session.m_pi.pt._typeid = _ticket._typeid;
		_session.m_pi.pt.unix_end_date = _ticket.end_date_unix_local;
		_session.m_pi.pt.unix_sec_date = _ticket.end_date_unix_local - (uint32_t)GetLocalTimeAsUnix();	// Difer�ncia em segundo, quanto tempo ainda tem para acabar o ticket premium

		// add Comet para o player
		_smp::message_pool::getInstance().push(new message("[PremiumSystem::addPremiumUser][Log] Add Comet Premium e set Capability do player[UID=" + std::to_string(_session.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[PremiumSystem::addPremiumUser][Log] Agora o Player[UID=" + std::to_string(_session.m_pi.uid) + "] eh um Premium User por (" + std::to_string(_time) + ") Dias", CL_FILE_LOG_AND_CONSOLE));

		// Add comet e outros itens e atualizar no SERVER, DB e GAME
		std::vector< stItem > add_itens;

		// Flag Premium User
		_session.m_pi.m_cap.stBit.premium_user = 1u;

		// Add Ball
		auto new_ball = addPremiumBall(_session);

		if (new_ball._typeid == 0u)
			throw exception("[PremiumSystem::addPremiumUser][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] nao conseguiu adicionar a Premium Ball.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 300, 0));

		// Add Ball para o jogo
		add_itens.push_back(new_ball);

		if (isPremium2(_session.m_pi.pt._typeid)) {

			// Add ClubSet
			auto new_clubset = addPremiumClubSet(_session, _time);

			if (new_clubset._typeid == 0u)
				throw exception("[PremiumSystem::addPremiumUser][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] nao conseguiu adicionar o Premium ClubSet.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 300, 0));

			// o ClubSet atualiza com o pacote073

			// Add Mascot
			auto new_mascot = addPremiumMascot(_session, _time);

			if (new_mascot._typeid == 0u)
				throw exception("[PremiumSystem::addPremiumUser][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] nao conseguiu adicionar o Premium Mascot.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 300, 0));

			// Add Mascot para o jogo
			add_itens.push_back(new_mascot);

			packet pp;

			if (packet_func::pacote073(pp, &_session, _session.m_pi.mp_wi))
				packet_func::session_send(pp, &_session, 1);
		}

		// Atualiza Capability do player
		packet p((unsigned short)0x9A);

		p.addUint32(_session.m_pi.m_cap.ulCapability);

		packet_func::session_send(p, &_session, 1);

		if (add_itens.size()) {

			p.init_plain((unsigned short)0x216);

			p.addUint32((const unsigned int)GetSystemTimeAsUnix());
			p.addUint32((uint32_t)add_itens.size()); // Count

			for (auto& el : add_itens) {

				p.addUint8(el.type);
				p.addUint32(el._typeid);
				p.addUint32(el.id);
				p.addUint32(el.flag_time);
				p.addBuffer(&el.stat, sizeof(stItem::item_stat));
				p.addUint32((el.flag_time == 0) ? el.STDA_C_ITEM_QNTD : el.STDA_C_ITEM_TIME);
				p.addZeroByte(25);
			}

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PremiumSystem::addPremiumUser][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void PremiumSystem::removePremiumUser(player& _session) {
	CHECK_SESSION_BEGIN("removePremiumUser");

	try {

		packet p;

		// Remove Premium Ball
		removePremiumBall(_session);

		// Tira capacidade de premium user do player
		_session.m_pi.m_cap.stBit.premium_user = 0u;

		packet_func::pacote09A(p, &_session, &_session.m_pi);
		packet_func::session_send(p, &_session, 1);

		// UPDATE ON GAME - Mostra a mensagem que acabou o tempo do ticket premium
		packet_func::pacote26D(p, &_session, _session.m_pi.pt.unix_end_date);
		packet_func::session_send(p, &_session, 0);

		// Zera o Premium User Ticket que ele j� n�o tem mais
		_session.m_pi.pt.clear();

		// Log
		_smp::message_pool::getInstance().push(new message("[PremiumSystem::removePremiumUser][Log] player[UID=" + std::to_string(_session.m_pi.uid)
				+ "] removeu o Premium User do Player, acabou o tempo do ticket, tirando a capacidade e a Comet(Ball)", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PremiumSystem::removePremiumUser][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

stItem PremiumSystem::addPremiumBall(player& _session) {
	CHECK_SESSION_BEGIN("addPremiumBall");

	stItem item{ 0u };

	try {

		uint32_t ball = getPremiumBallByTicket(_session.m_pi.pt._typeid);

		// Add Ball
		WarehouseItemEx new_wi{ 0 };
		new_wi.id = -1;
		new_wi.ano = -1;
		new_wi._typeid = ball;	// Premium Ball
		new_wi.STDA_C_ITEM_QNTD = 1;
		new_wi.type = 0x6A;		// Item time Premium
		new_wi.clubset_workshop.level = -1;

		auto it = _session.m_pi.mp_wi.insert(std::make_pair(new_wi.id, new_wi));

		// Coloca a premium ball nos itens equipados
		_session.m_pi.ue.ball_typeid = ball;

		// Warehouse Item on Map Player
		_session.m_pi.ei.comet = &it->second;

		// Initialize Item
		item.type = 2;
		item.id = new_wi.id;
		item._typeid = new_wi._typeid;
		item.flag_time = new_wi.type;
		item.stat.qntd_ant = 0u;
		item.stat.qntd_dep = 1u;
		item.qntd = 1u;
		item.STDA_C_ITEM_QNTD = (short)item.qntd;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PremiumSystem::addPremiumBall][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return item;
}

stItem PremiumSystem::addPremiumClubSet(player& _session, uint32_t _time) {
	CHECK_SESSION_BEGIN("addPremiumClubSet");

	stItem item{ 0u };
	
	try {

		uint32_t clubset = getPremiumClubSetByTicket(_session.m_pi.pt._typeid);

		// Add ClubSet
		// Aqui add com o item_manager::addItem
		BuyItem bi{ 0u };

		bi.id = -1;
		bi._typeid = clubset;
		bi.qntd = 1;
		bi.time = (unsigned short)_time;

		item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1);

		if (item._typeid == 0u)
			throw exception("[PremiumSystem::addPremiumClubSet][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] nao conseguiu inicializar o item[TYPEID=" + std::to_string(bi._typeid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 400, 0));

		if (item_manager::addItem(item, _session, 0, 0) < 0)
			throw exception("[PremiumSystem::addPremiumClubSet][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] nao conseguiu adicionar o item[TYPEID=" + std::to_string(item._typeid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 401, 0));

		auto new_wi = _session.m_pi.findWarehouseItemById(item.id);

		new_wi->STDA_C_ITEM_TIME = 0;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PremiumSystem::addPremiumClubSet][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return item;
}

stItem PremiumSystem::addPremiumMascot(player& _session, uint32_t _time) {
	CHECK_SESSION_BEGIN("addPremiumMascot");

	stItem item{ 0u };

	try {

		uint32_t mascot = getPremiumMascotByTicket(_session.m_pi.pt._typeid);

		// Add Mascot
		// Aqui add com o item_manager::addItem
		BuyItem bi{ 0u };

		bi.id = -1;
		bi._typeid = mascot;
		bi.qntd = 1;
		bi.time = (unsigned short)_time;

		item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1);

		if (item._typeid == 0u)
			throw exception("[PremiumSystem::addPremiumMascot][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] nao conseguiu inicializar o item[TYPEID=" + std::to_string(bi._typeid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 400, 0));

		if (item_manager::addItem(item, _session, 0, 0) < 0)
			throw exception("[PremiumSystem::addPremiumMascot][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] nao conseguiu adicionar o item[TYPEID=" + std::to_string(item._typeid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 401, 0));

		auto new_wi = _session.m_pi.findMascotById(item.id);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PremiumSystem::addPremiumMascot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return item;
}

void PremiumSystem::removePremiumBall(player& _session) {
	CHECK_SESSION_BEGIN("removePremiumBall");

	try {

		uint32_t ball = getPremiumBallByTicket(_session.m_pi.pt._typeid);

		// Tira primeiro a Ball
		auto pWi = _session.m_pi.findWarehouseItemItByTypeid(ball);

		// Delete Premium Ball
		if (pWi != _session.m_pi.mp_wi.end()) {

			stItem item{ 0 };

			item.type = 2;
			item.id = pWi->second.id;
			item._typeid = pWi->second._typeid;
			item.qntd = 1;
			item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;
			item.stat.qntd_ant = 1;
			item.stat.qntd_dep = 0;
			item.flag_time = 0x6A;		// PREMIUM ITEM, /*0x6A expired time*/

			// Remove do Server, que esse item n�o tem no DB, 
			// � s� do server um item que ganha quando � premium user quando loga
			// !@ Aqui pode d� erro por que ent� rodando no loob o map mp_wi, e aqui est� excluindo um iterator do map
			_session.m_pi.mp_wi.erase(pWi);

			packet p((unsigned short)0x216);

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

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PremiumSystem::removePremiumBall][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void PremiumSystem::updatePremiumUser(player& _session) {
	CHECK_SESSION_BEGIN("updatePremiumUser");

	try {

		std::vector< stItem > add_itens;

		// Flag Premium User
		_session.m_pi.m_cap.stBit.premium_user = 1u;

		// Add Ball
		auto new_ball = addPremiumBall(_session);

		if (new_ball._typeid == 0u)
			throw exception("[PremiumSystem::updatePremiumUser][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] nao conseguiu adicionar a Premium Ball.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PREMIUM_SYSTEM, 300, 0));

		add_itens.push_back(new_ball);

		// Atualiza Capability do player
		packet p((unsigned short)0x9A);

		p.addUint32(_session.m_pi.m_cap.ulCapability);

		packet_func::session_send(p, &_session, 1);

		if (add_itens.size() > 0) {

			p.init_plain((unsigned short)0x216);

			p.addUint32((const unsigned int)GetSystemTimeAsUnix());
			p.addUint32((uint32_t)add_itens.size()); // Count

			for (auto& el : add_itens) {

				p.addUint8(el.type);
				p.addUint32(el._typeid);
				p.addUint32(el.id);
				p.addUint32(el.flag_time);
				p.addBuffer(&el.stat, sizeof(stItem::item_stat));
				p.addUint32((el.flag_time == 0) ? el.STDA_C_ITEM_QNTD : el.STDA_C_ITEM_TIME);
				p.addZeroByte(25);
			}

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PremiumSystem::updatePremiumUser][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

uint32_t PremiumSystem::getPremiumBallByTicket(uint32_t _typeid) {

	if (_typeid == PREMIUM_TICKET_TYPEID)
		return PREMIUM_BALL_TYPEID;

	if (_typeid == PREMIUM_2_TICKET_TYPEID)
		return PREMIUM_2_BALL_TYPEID;

	return 0u;
}

uint32_t PremiumSystem::getPremiumClubSetByTicket(uint32_t _typeid) {

	if (_typeid == PREMIUM_2_TICKET_TYPEID)
		return PREMIUM_2_CLUBSET_TYPEID;

	return 0u;
}

uint32_t PremiumSystem::getPremiumMascotByTicket(uint32_t _typeid) {

	if (_typeid == PREMIUM_2_TICKET_TYPEID)
		return PREMIUM_2_MASCOT_TYPEID;

	return 0u;
}

uint32_t PremiumSystem::getExpPangRateByTicket(uint32_t _typeid) {

	if (_typeid == PREMIUM_TICKET_TYPEID)
		return 10u;

	if (_typeid == PREMIUM_2_TICKET_TYPEID)
		return 15u;

	return 0u;
}

bool PremiumSystem::isPremiumTicket(uint32_t _typeid) {
	return _typeid == PREMIUM_TICKET_TYPEID || _typeid == PREMIUM_2_TICKET_TYPEID;
}

bool PremiumSystem::isPremiumBall(uint32_t _typeid) {
	return _typeid == PREMIUM_BALL_TYPEID || _typeid == PREMIUM_2_BALL_TYPEID;
}

bool PremiumSystem::isPremium1(uint32_t _typeid) {
	return _typeid == PREMIUM_TICKET_TYPEID;
}

bool PremiumSystem::isPremium2(uint32_t _typeid) {
	return _typeid == PREMIUM_2_TICKET_TYPEID;
}
