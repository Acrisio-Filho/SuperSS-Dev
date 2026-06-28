// Arquivo room_grand_prix.cpp
// Criado em 16/06/2019 as 17:58 por Acrisio
// Implementa��o da classe RoomGrandPrix

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "room_grand_prix.hpp"

#include "../PACKET/packet_func_sv.h"

#include "../Game Server/game_server.h"

#include "grand_prix.hpp"

#include "../UTIL/lottery.hpp"

#include "item_manager.h"

using namespace stdA;

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[RoomGrandPrix::" + std::string((method)) + "][Error] player nao esta connectado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 12, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[RoomGrandPrix::request" + std::string((method)) + "][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 12, 0)); \

RoomGrandPrix::RoomGrandPrix(unsigned char _channel_owner, RoomInfoEx _ri, IFF::GrandPrixData& _gp) 
	: room(_channel_owner, _ri), m_gp(_gp), m_count_down(nullptr) {

	// Coloca a inst�ncia da classe que acabou de criar no vector statico
	push_instancia(this);

	// Verifica se � Grand Prix Normal e cria um temporizador para come�ar a sala
	// Rookie Grand Prix n�o tem tempo para come�ar o player come�a na hora que ele d� play por que � uma inst�ncia
	if (!(sIff::getInstance().getGrandPrixAba(m_gp._typeid) == IFF::GrandPrixData::GP_ABA::ROOKIE && sIff::getInstance().isGrandPrixNormal(m_gp._typeid))) {
		
		// Grand Prix tempo que falta para come�ar
		SYSTEMTIME local{ 0 };

		GetLocalTime(&local);

		local.wDay = local.wDayOfWeek = local.wMonth = local.wYear = 0u;

		// Adiciona 1 dia para o start se a hora for >= 23 do open e <= 1 a hora do start
		if (m_gp.open.wHour >= 23 && m_gp.start.wHour <= 1)
			m_gp.start.wDay = 1u;

		auto diff = (!isEmpty(m_gp.start) ? getHourDiff(m_gp.start, local) : 0ll);

		// mili to sec
		if (diff < 0ll)
			diff = 0ll;
		else
			diff = (int64_t)std::round(diff / 1000.f);

		count_down_to_start(diff);

	}

}

RoomGrandPrix::~RoomGrandPrix() {

	if (m_count_down != nullptr) {
		
		sgs::gs::getInstance().unMakeTime(m_count_down);

		m_count_down = nullptr;
	}

	// Tira a instância da classe do vector statico, por que a sala vai ser destruida
	pop_instancia(this);
}

eROOM_CLASS_TYPE RoomGrandPrix::getClassType() {
	return eROOM_CLASS_TYPE::RCT_GRAND_PRIX;
}

bool RoomGrandPrix::isAllReady() {

	// Grand Prix sempre é true, por que é o server que começa ou é o GP ROOKIE que ele começa sozinho
	// O cliente da erro na hora de começar se tiver convidado na sala
	// então verifica se não tem nenhum convidado na sala
	return !_haveInvited();
}

void RoomGrandPrix::requestChangePlayerItemRoom(player& _session, ChangePlayerItemRoom& _cpir) {
	CHECK_SESSION_BEGIN("ChangePlayerItemRoom");

	packet p;

	try {

		auto gp_condition = sIff::getInstance().findGrandPrixConditionEquip(m_gp.typeid_link);

		if (!gp_condition.empty()) {

			switch (_cpir.type) {
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_CADDIE:
				{
						CaddieInfoEx *pCi = nullptr;

						if (_cpir.caddie != 0 && (pCi = _session.m_pi.findCaddieById(_cpir.caddie)) != nullptr
							&& sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE
							&& checkCaddie(_session, &pCi, gp_condition) && pCi != nullptr) {

									// Atualiza o caddie equipado do player
									_cpir.caddie = pCi->id;

									_session.m_pi.ei.cad_info = pCi;
									_session.m_pi.ue.caddie_id = pCi->id;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_CADDIE, 0);
						packet_func::room_broadcast(*this, p, 1);
					}
					break;
				}
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_BALL:
				{
						WarehouseItemEx *pWi = nullptr;

						if (_cpir.ball != 0 && (pWi = _session.m_pi.findWarehouseItemByTypeid(_cpir.ball)) != nullptr
							&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL
							&& checkBall(_session, &pWi, gp_condition) && pWi != nullptr) {

									// Atualiza a bola equipado do player
									_cpir.ball = pWi->_typeid;

									_session.m_pi.ei.comet = pWi;
									_session.m_pi.ue.ball_typeid = pWi->_typeid;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_BALL, 0);
						packet_func::room_broadcast(*this, p, 1);
					}
					break;
				}
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_CLUBSET:
				{
						WarehouseItemEx *pWi = nullptr;

						if (_cpir.clubset != 0 && (pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset)) != nullptr
							&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET
							&& checkClubSet(_session, &pWi, gp_condition) && pWi != nullptr) {

									// Atualiza o clubset equipado do player
									_cpir.clubset = pWi->id;

									_session.m_pi.ei.clubset = pWi;

						// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
						// que no original fica no warehouse msm, eu só confundi quando fiz
									_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

									IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

									if (cs != nullptr) {

										for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
											_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

										_session.m_pi.ue.clubset_id = pWi->id;
									}

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_CLUBSET, 0);
						packet_func::room_broadcast(*this, p, 1);
					}
					break;
				}
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_CHARACTER:
				{
						CharacterInfo *pCe = nullptr;

						if (_cpir.character != 0 && (pCe = _session.m_pi.findCharacterById(_cpir.character)) != nullptr
							&& sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER
							&& checkCharacter(_session, &pCe, gp_condition) && pCe != nullptr) {

									// Atualiza o character equipado do player
									_cpir.character = pCe->id;

									_session.m_pi.ei.char_info = pCe;
									_session.m_pi.ue.character_id = pCe->id;

									// Update IN GAME
									packet_func::pacote06B(p, &_session, &_session.m_pi, 5/*Character ID only*/, 4/*Sucesso*/);
									packet_func::session_send(p, &_session, 1);
					}
					break;
				}
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_MASCOT:
				{
						MascotInfoEx *pMi = nullptr;

						if (_cpir.mascot != 0 && (pMi = _session.m_pi.findMascotById(_cpir.mascot)) != nullptr 
							&& sIff::getInstance().getItemGroupIdentify(pMi->_typeid) == iff::MASCOT
							&& checkMascot(_session, &pMi, gp_condition) && pMi != nullptr) {

									// Atualiza o mascot equipado do player
									_cpir.mascot = pMi->id;

									_session.m_pi.ei.mascot_info = pMi;
									_session.m_pi.ue.mascot_id = pMi->id;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_MASCOT, 0);
						packet_func::room_broadcast(*this, p, 1);
					}
					break;
					}
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_ITEM_EFFECT_LOUNGE:
					// Esse não usa por que é só no lounge que troca esse item ou ativa o efeito
					break;
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_ALL:
				{

						CharacterInfo *pCe = nullptr;
					CaddieInfoEx *pCi = nullptr;
					WarehouseItemEx *pWi = nullptr;

					// Character
					pCe = nullptr;

						if (_cpir.character != 0 && (pCe = _session.m_pi.findCharacterById(_cpir.character)) != nullptr
							&& sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER
							&& checkCharacter(_session, &pCe, gp_condition) && pCe != nullptr) {

									// Atualiza o character equipado do player
									_cpir.character = pCe->id;

									_session.m_pi.ei.char_info = pCe;
									_session.m_pi.ue.character_id = pCe->id;

									// Update IN GAME
									packet_func::pacote06B(p, &_session, &_session.m_pi, 5/*Character ID only*/, 4/*Sucesso*/);
									packet_func::session_send(p, &_session, 1);
					}

						// Caddie
					pCi = nullptr;

						if (_cpir.caddie != 0 && (pCi = _session.m_pi.findCaddieById(_cpir.caddie)) != nullptr
							&& sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE
							&& checkCaddie(_session, &pCi, gp_condition) && pCi != nullptr) {

									// Atualiza o caddie equipado do player
									_cpir.caddie = pCi->id;

									_session.m_pi.ei.cad_info = pCi;
									_session.m_pi.ue.caddie_id = pCi->id;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_CADDIE, 0);
						packet_func::room_broadcast(*this, p, 1);
						}

						// ClubSet
					pWi = nullptr;

						if (_cpir.clubset != 0 && (pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset)) != nullptr
							&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET
							&& checkClubSet(_session, &pWi, gp_condition) && pWi != nullptr) {

									// Atualiza o clubset equipado do player
									_cpir.clubset = pWi->id;

									_session.m_pi.ei.clubset = pWi;

						// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
						// que no original fica no warehouse msm, eu só confundi quando fiz
									_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

									IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

									if (cs != nullptr) {

										for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
											_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

										_session.m_pi.ue.clubset_id = pWi->id;
									}

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_CLUBSET, 0);
						packet_func::room_broadcast(*this, p, 1);
						}

					// Ball
					pWi = nullptr;

						if (_cpir.ball != 0 && (pWi = _session.m_pi.findWarehouseItemByTypeid(_cpir.ball)) != nullptr
							&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL
							&& checkBall(_session, &pWi, gp_condition) && pWi != nullptr) {

									// Atualiza a bola equipado do player
									_cpir.ball = pWi->_typeid;

									_session.m_pi.ei.comet = pWi;
									_session.m_pi.ue.ball_typeid = pWi->_typeid;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_BALL, 0);
						packet_func::room_broadcast(*this, p, 1);
					}
					break;
				}
			}

		} // fim do iff que verifica se o gp_condition é válido

		// Chama o changePlayerItemRoom dá sala padrão para fazer as alterações
		room::requestChangePlayerItemRoom(_session, _cpir);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		packet_func::pacote04B(p, &_session, _cpir.type, (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::ROOM ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1/*Unknown Error*/));
		packet_func::session_send(p, &_session, 0);
	}
}

void RoomGrandPrix::requestChangeItemSlot(player& _session, UserEquip& _ue) {

	UserEquip* pUe = &_ue;

	auto gp_condition = sIff::getInstance().findGrandPrixConditionEquip(m_gp.typeid_link);

	if (!gp_condition.empty()) {

		if (checkItemSlot(_session, &pUe, gp_condition) && pUe != nullptr) {

			// Atualiza o item slot, troca o primeiro item do slot pelo item requerido
#if defined(_WIN32)
			memcpy_s(&_ue, sizeof(UserEquip), pUe, sizeof(UserEquip));
#elif defined(__linux__)
			memcpy(&_ue, pUe, sizeof(UserEquip));
#endif
		}

	} // fim do iff que verifica se o gp_condition é válido
}

void RoomGrandPrix::requestChangeCharacter(player& _session, CharacterInfo& _ce) {

	
	CharacterInfo* pCe = &_ce;

	auto gp_condition = sIff::getInstance().findGrandPrixConditionEquip(m_gp.typeid_link);

	if (!gp_condition.empty()) {

		if (checkCharacter(_session, &pCe, gp_condition) && pCe != nullptr) {

			// Atualiza o character equipado do player
			_session.m_pi.ei.char_info = pCe;
			_session.m_pi.ue.character_id = pCe->id;

		}
	} // fim do iff que verifica se o gp_condition é válido
}

// Game, esse aqui é só para o Grand Prix ROOKIE(TUTO)
bool RoomGrandPrix::requestStartGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("StartGame");

	packet p;

	bool ret = true;

	try {

		if (sIff::getInstance().getGrandPrixAba(m_gp._typeid) == IFF::GrandPrixData::GP_ABA::ROOKIE && !sIff::getInstance().isGrandPrixNormal(m_gp._typeid))
			throw exception("[RoomGrandPrix::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o jogo na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao eh uma Grand Prix Rookie(Tuto). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 1, 0x5900201));

		// Diferente de Grand Prix ROOKIE(TUTO), manda para o requestStartGame da class room, para tratar esse requisi��o
		if (sIff::getInstance().getGrandPrixAba(m_gp._typeid) != IFF::GrandPrixData::GP_ABA::ROOKIE)
			ret = room::requestStartGame(_session, _packet);
		else {

			// Verifica se j� tem um jogo inicializado e lan�a error se tiver, para o cliente receber uma resposta
			if (m_pGame != nullptr)
				throw exception("[RoomGrandPrix::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] tentou comecar o jogo na sala[NUMERO=" + std::to_string(m_ri.numero) + "], mas ja tem um jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 7, 0x5900202));

			// Verifica se todos est�o prontos se n�o da erro
			if (!isAllReady())
				throw exception("[RoomGrandPrix::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] tentou comecar o jogo na sala[NUMERO=" + std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master)
						+ "], mas nem todos jogadores estao prontos. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 8, 0x5900202));

			// random course if random course
			if (m_ri.course >= 0x7Fu) {

				// Special Shuffle Course
				if (m_ri.tipo == RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE && m_ri.modo == Hole::eMODO::M_SHUFFLE_COURSE) {

					m_ri.course = RoomInfo::eCOURSE(0x80 | RoomInfo::eCOURSE::CHRONICLE_1_CHAOS/*SPECIAL SHUFFLE COURSE*/);

				}else {	// Random normal

					Lottery lottery((uint64_t)this);

					for (auto& el : sIff::getInstance().getCourse()) {

						auto course_id = sIff::getInstance().getItemIdentify(el.second._typeid);

						if (course_id != 17/*SSC*/ && course_id != 0x40/*GRAND ZODIAC*/)
							lottery.push(100, course_id);
					}

					auto lc = lottery.spinRoleta();

					if (lc != nullptr)
						m_ri.course = RoomInfo::eCOURSE(0x80u | (unsigned char)lc->value);
				}
			}

			RateValue rv{ 0 };

			// Att Exp rate, e Pang rate, que come�ou o jogo
			rv.exp = m_ri.rate_exp = sgs::gs::getInstance().getInfo().rate.exp;
			rv.pang = m_ri.rate_pang = sgs::gs::getInstance().getInfo().rate.pang;

			// Angel Event
			m_ri.angel_event = sgs::gs::getInstance().getInfo().rate.angel_event;

			rv.clubset = sgs::gs::getInstance().getInfo().rate.club_mastery;
			rv.rain = sgs::gs::getInstance().getInfo().rate.chuva;
			rv.treasure = sgs::gs::getInstance().getInfo().rate.treasure;

			rv.persist_rain = 0u;	// Persist rain flag isso � feito na classe game

			switch (m_ri.tipo) {
			case RoomInfo::TIPO::GRAND_PRIX:
				m_pGame = new GrandPrix(v_sessions, m_ri, rv, m_ri.channel_rookie, m_gp);
				break;
			default:
				throw exception("[RoomGrandPrix::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] tentou comecar o jogo na sala[NUMERO=" + std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master)
						+ "], mas o tipo da sala nao eh Grand Prix. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 9, 0x5900202));
			}

			// Update Room State
			m_ri.state = 0;	// IN GAME

			// Mandar para ficar igual ao original
			p.init_plain((unsigned short)0x253);

			p.addUint32(0u);

			packet_func::room_broadcast(*this, p, 1);

			// Update on GAME
			p.init_plain((unsigned short)0x230);

			packet_func::room_broadcast(*this, p, 1);

			p.init_plain((unsigned short)0x231);

			packet_func::room_broadcast(*this, p, 1);

			uint32_t rate_pang = sgs::gs::getInstance().getInfo().rate.pang;

			p.init_plain((unsigned short)0x77);

			p.addUint32(rate_pang);	// Rate Pang

			packet_func::room_broadcast(*this, p, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestStartGame][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Error
		p.init_plain((unsigned short)0x253);
		
		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError() == STDA_ERROR_TYPE::ROOM_GRAND_PRIX)) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5900200);

		packet_func::session_send(p, &_session, 1);

		ret = false;	// Error ao inicializar o Jogo
	}

	return ret;
}

bool RoomGrandPrix::startGame() {

	packet p;

	bool ret = true;

	try {

		if (sIff::getInstance().getGrandPrixAba(m_gp._typeid) == IFF::GrandPrixData::GP_ABA::ROOKIE && !sIff::getInstance().isGrandPrixNormal(m_gp._typeid))
			throw exception("[RoomGrandPrix::startGame][Error] Server tentou comecar o jogo na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao eh uma Grand Prix Rookie(Tuto). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 1, 0x5900201));

		// Verifica se j� tem um jogo inicializado e lan�a error se tiver, para o cliente receber uma resposta
		if (m_pGame != nullptr)
			throw exception("[RoomGrandPrix::startGame][Error] Server tentou comecar o jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas ja tem um jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 7, 0x5900202));

		// Verifica se todos est�o prontos se n�o da erro
		if (!isAllReady())
			throw exception("[RoomGrandPrix::startGame][Error] Server tentou comecar o jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master) 
					+ "], mas nem todos jogadores estao prontos. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 8, 0x5900202));

		// random course if random course
		if (m_ri.course >= 0x7Fu) {

			// Special Shuffle Course
			if (m_ri.tipo == RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE && m_ri.modo == Hole::eMODO::M_SHUFFLE_COURSE) {

				m_ri.course = RoomInfo::eCOURSE(0x80 | RoomInfo::eCOURSE::CHRONICLE_1_CHAOS/*SPECIAL SHUFFLE COURSE*/);

			}else {	// Random normal

				Lottery lottery((uint64_t)this);

				for (auto& el : sIff::getInstance().getCourse()) {
				
					auto course_id = sIff::getInstance().getItemIdentify(el.second._typeid);

					if (course_id != 17/*SSC*/ && course_id != 0x40/*GRAND ZODIAC*/)
						lottery.push(100, course_id);
				}

				auto lc = lottery.spinRoleta();

				if (lc != nullptr)
					m_ri.course = RoomInfo::eCOURSE(0x80u | (unsigned char)lc->value);
			}
		}

		RateValue rv{ 0 };

		// Att Exp rate, e Pang rate, que come�ou o jogo
		rv.exp = m_ri.rate_exp = sgs::gs::getInstance().getInfo().rate.exp;
		rv.pang = m_ri.rate_pang = sgs::gs::getInstance().getInfo().rate.pang;

		// Angel Event
		m_ri.angel_event = sgs::gs::getInstance().getInfo().rate.angel_event;

		rv.clubset = sgs::gs::getInstance().getInfo().rate.club_mastery;
		rv.rain = sgs::gs::getInstance().getInfo().rate.chuva;
		rv.treasure = sgs::gs::getInstance().getInfo().rate.treasure;

		rv.persist_rain = 0u;	// Persist rain flag isso � feito na classe game

		switch (m_ri.tipo) {
		case RoomInfo::TIPO::GRAND_PRIX:
			m_pGame = new GrandPrix(v_sessions, m_ri, rv, m_ri.channel_rookie, m_gp);
			break;
		default:
			throw exception("[RoomGrandPrix::startGame][Error] Server tentou comecar o jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master)
					+ "], mas o tipo da sala nao eh Grand Prix. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_GRAND_PRIX, 9, 0x5900202));
		}

		// Update Room State
		m_ri.state = 0;	// IN GAME

		// Update on GAME
		p.init_plain((unsigned short)0x230);

		packet_func::room_broadcast(*this, p, 1);

		p.init_plain((unsigned short)0x231);

		packet_func::room_broadcast(*this, p, 1);

		uint32_t rate_pang = sgs::gs::getInstance().getInfo().rate.pang;

		p.init_plain((unsigned short)0x77);

		p.addUint32(rate_pang);	// Rate Pang

		packet_func::room_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::startGame][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		ret = false;	// Error ao inicializar o Jogo
	}

	return ret;
}

void RoomGrandPrix::initFirstInstance() {

	if (m_cs_instancia::getInstance().m_state && m_instancias::getInstance().empty())
		_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::initFirstInstance][Log] Criou primeira instance do Singleton da classe Room Grand Prix static vector.", CL_FILE_LOG_AND_CONSOLE));
}

int RoomGrandPrix::_count_down_to_start(void* _arg1, void* _arg2) {

	RoomGrandPrix *_rgp = reinterpret_cast< RoomGrandPrix* >(_arg1);
	int64_t sec_to_start = reinterpret_cast< int64_t >(_arg2);

	try {

		if (_rgp != nullptr && instancia_valid(_rgp))
			_rgp->count_down_to_start(sec_to_start);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::_count_down_to_start][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
}

void RoomGrandPrix::count_down_to_start(int64_t _sec_to_start) {

	try {
		
		// Bloquea a sala para ela n�o ser destruida antes de acabar de fazer o que tem que fazer aqui
		lock();

		if (_sec_to_start <= 0) {	// Começa o jogo
			
			// excluí o timer se ele ainda existir
			if (m_count_down != nullptr) {
				
				sgs::gs::getInstance().unMakeTime(m_count_down);

				m_count_down = nullptr;
			}

			// Começa o jogo se tem pelo menos 1 jogador na sala
			if (v_sessions.size() >= 1 && startGame())
				sgs::gs::getInstance().sendUpdateRoomInfo(this, 3);
			else if (v_sessions.size() >= 1)
				// tenta de novo daqui a 10 segundos, por que pode ter convidado na sala, 
				// GP não tem, o cliente não deixa, mas vai que algum hacker conseguiu
				count_down_to_start(10);

		}else {

			uint32_t wait = 0u;
			int32_t rest = 0;

			unsigned char type = 1u;

			DWORD interval = 0u;
			float diff = 0.f;

			int32_t elapsed_sec = (m_count_down != nullptr) ? (int)std::round(m_count_down->getElapsed() / 1000.f)/*Mili para segundos*/ : 0;

			_sec_to_start -= elapsed_sec;

			if ((diff = ((_sec_to_start - 10/*10 segundos*/) / 30.f/* 30 segundos*/)) >= 1.f) {	// Intervalo de 30 segundos

				if ((_sec_to_start % 30) == 0) {
					
					// Intervalo
					interval = 30 * 1000;	// 30 segundos

					wait = interval * (int)diff;	// 30 * diff minutos em milisegundos
				
				}else {

					// Corrige o tempo para ficar no intervalo certo
					wait = interval = (_sec_to_start % 30) * 1000;
				
				}

			}else if ((diff = ((_sec_to_start - 1/*1 segundo*/) / 10.f/*10 segundos*/)) >= 1.f) {			// Intervalo de 10 segundos
			
				if ((_sec_to_start % 10) == 0) {
					
					// Intervalo
					interval = 10 * 1000;	// 10 segundos

					wait = interval * (int)diff;	// 10 * diff segundos em milisegundos
				
				}else {

					// Corrige o tempo para ficar no intervalo certo
					wait = interval = (_sec_to_start % 10) * 1000;
				}

			}else {		// Intervalo de 1 segundo

				diff = std::round(_sec_to_start / 1.f);

				// Intervalo
				interval = 1000;	// 1 segundo
			
				wait = interval * (int)diff;	// 1 * diff segundos em milesegundos

			}

			// UPDATE ON GAME
			packet p((unsigned short)0x40);

			p.addUint8(11);	// Temporizador Grand Prix

			p.addUint16(0u);	// Nick
			p.addUint16(0u);	// Msg

			p.addUint32((uint32_t)_sec_to_start);

			packet_func::room_broadcast(*this, p, 1);

			// Make Timer
			if (m_count_down == nullptr || m_count_down->getState() == timer::STOP ||
				m_count_down->getState() == timer::STOPPING || m_count_down->getState() == timer::STOPPED) {

				// Make Time
				job _job(RoomGrandPrix::_count_down_to_start, this, (void*)_sec_to_start);

				// Se o Shutdown Timer estiver criado descria e cria um novo
				if (m_count_down != nullptr)
					sgs::gs::getInstance().unMakeTime(m_count_down);

				m_count_down = sgs::gs::getInstance().makeTime(wait, _job, std::vector< DWORD > { interval });
			}
		}

		// Libera a sala
		unlock();
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::count_down_to_start][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Libera a sala
		unlock();
	}
}

bool RoomGrandPrix::checkCaddie(player& _session, CaddieInfoEx** _pCi, std::vector< IFF::GrandPrixConditionEquip >& _gp_condition) {

	if (_pCi == nullptr || *_pCi == nullptr)
		return false;

	bool ret = false;
	CaddieInfoEx* pCi = *_pCi;
	uint32_t changed_caddie_typeid = pCi->_typeid;
	
	std::vector< IFF::GrandPrixConditionEquip > all_caddie;
	std::vector< IFF::GrandPrixConditionEquip >::iterator it_caddie;

	std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_caddie), [](auto& _el) {
		return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::CADDIE;
	});

	if (!all_caddie.empty() && !std::any_of(all_caddie.begin(), all_caddie.end(), [&pCi](auto& _el) -> bool {
		return _el.item_typeid == pCi->_typeid;
	})) {

		it_caddie = std::find_if(all_caddie.begin(), all_caddie.end(), [&_session, &pCi](auto& _el) {
			return (pCi = _session.m_pi.findCaddieByTypeid(_el.item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE;
		});

		// Procura o caddie que o player tem que ter para jogar esse Grand Prix
		if (it_caddie != all_caddie.end()) {

			// Atualiza o caddie equipado do player
			_session.m_pi.ei.cad_info = pCi;
			_session.m_pi.ue.caddie_id = pCi->id;

			ret = true;
								
		}else {
									
			// Player não tem o caddie necessário para jogar esse Grand Prix, kick ele dá sala
			_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar item[ID=" + std::to_string(changed_caddie_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
					+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o caddie equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			*_pCi = nullptr;
			ret = true;
		}

	} else if (all_caddie.empty()) { // Verifica caddie item

		std::vector< IFF::GrandPrixConditionEquip > all_cad_item;
		std::vector< IFF::GrandPrixConditionEquip >::iterator it_cad_item;

		std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_cad_item), [](auto& _el) {
			return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::CAD_ITEM;
		});

		if (!all_cad_item.empty() && !std::any_of(all_cad_item.begin(), all_cad_item.end(), [&pCi](auto& _el) -> bool {

			uint32_t cad_typeid = (iff::CADDIE << 26) | sIff::getInstance().getCaddieIdentify(_el.item_typeid);

			return pCi->_typeid == cad_typeid && pCi->parts_typeid == _el.item_typeid;
		})) {

			it_cad_item = std::find_if(all_cad_item.begin(), all_cad_item.end(), [&_session, &pCi](auto& _el) {

				uint32_t cad_typeid = (iff::CADDIE << 26) | sIff::getInstance().getCaddieIdentify(_el.item_typeid);

				return (pCi = _session.m_pi.findCaddieByTypeid(cad_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE
					&& pCi->parts_typeid == _el.item_typeid;
			});

			// Procura o caddie que o player tem que ter para jogar esse Grand Prix
			if (it_cad_item != all_cad_item.end()) {

				// Atualiza o caddie equipado do player
				_session.m_pi.ei.cad_info = pCi;
				_session.m_pi.ue.caddie_id = pCi->id;

				ret = true;
								
			}else {
									
				// Player não tem o caddie necessário para jogar esse Grand Prix, kick ele dá sala
				_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] tentou trocar item[ID=" + std::to_string(changed_caddie_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
						+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o caddie equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

				*_pCi = nullptr;
				ret = true;
			}
		}
	}

	return ret;
}

bool RoomGrandPrix::checkBall(player& _session, WarehouseItemEx** _pWi, std::vector< IFF::GrandPrixConditionEquip >& _gp_condition) {
	
	if (_pWi == nullptr || *_pWi == nullptr)
		return false;

	bool ret = false;
	WarehouseItemEx* pWi = *_pWi;
	uint32_t changed_ball_typeid = pWi->_typeid;

	std::vector< IFF::GrandPrixConditionEquip > all_ball;
	std::vector< IFF::GrandPrixConditionEquip >::iterator it_ball;

	std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_ball), [](auto& _el) {
		return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::BALL;
	});

	if (!all_ball.empty() && !std::any_of(all_ball.begin(), all_ball.end(), [&pWi](auto& _el) -> bool {
		return _el.item_typeid == pWi->_typeid;
	})) {

		it_ball = std::find_if(all_ball.begin(), all_ball.end(), [&_session, &pWi](auto& _el) {
			return (pWi = _session.m_pi.findWarehouseItemByTypeid(_el.item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL;
		});

		// Procura a Ball que o player tem que ter para jogar esse Grand Prix
		if (it_ball != all_ball.end()) {

			// Atualiza a bola equipado do player
			_session.m_pi.ei.comet = pWi;
			_session.m_pi.ue.ball_typeid = pWi->_typeid;

			ret = true;
								
		}else {
									
			// Player não tem a bola necessário para jogar esse Grand Prix, kick ele dá sala
			_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar item[TYPEID=" + std::to_string(changed_ball_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
					+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar a bola equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			*_pWi = nullptr;
			ret = true;
		}
	}

	return ret;
}

bool RoomGrandPrix::checkClubSet(player& _session, WarehouseItemEx** _pWi, std::vector< IFF::GrandPrixConditionEquip >& _gp_condition) {
	
	if (_pWi == nullptr || *_pWi == nullptr)
		return false;

	bool ret = false;
	WarehouseItemEx* pWi = *_pWi;
	uint32_t changed_clubset_typeid = pWi->_typeid;

	std::vector< IFF::GrandPrixConditionEquip > all_clubset;
	std::vector< IFF::GrandPrixConditionEquip >::iterator it_clubset;

	std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_clubset), [](auto& _el) {
		return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::CLUBSET;
	});

	if (!all_clubset.empty() && !std::any_of(all_clubset.begin(), all_clubset.end(), [&pWi](auto& _el) -> bool {
		return _el.item_typeid == pWi->_typeid;
	})) {

		it_clubset = std::find_if(all_clubset.begin(), all_clubset.end(), [&_session, &pWi](auto& _el) {
			return (pWi = _session.m_pi.findWarehouseItemByTypeid(_el.item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET;
		});

		// Procura a ClubSet que o player tem que ter para jogar esse Grand Prix
		if (it_clubset != all_clubset.end()) {

			// Atualiza o clubset equipado do player
			_session.m_pi.ei.clubset = pWi;

			// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
			// que no original fica no warehouse msm, eu só confundi quando fiz
			_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

			IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

			if (cs != nullptr) {

				for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
					_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

				_session.m_pi.ue.clubset_id = pWi->id;
			}

			ret = true;
								
		}else {
									
			// Player não tem o clubset necessário para jogar esse Grand Prix, kick ele dá sala
			_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar item[ID=" + std::to_string(changed_clubset_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
					+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o ClubSet equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			*_pWi = nullptr;
			ret = true;
		}
	}

	return ret;
}

bool RoomGrandPrix::checkCharacter(player& _session, CharacterInfo** _pCe, std::vector< IFF::GrandPrixConditionEquip >& _gp_condition) {
	
	if (_pCe == nullptr || *_pCe == nullptr)
		return false;

	bool ret = false;
	CharacterInfo* pCe = *_pCe;
	uint32_t changed_char_typeid = pCe->_typeid;

	std::vector< IFF::GrandPrixConditionEquip > all_char;
	std::vector< IFF::GrandPrixConditionEquip >::iterator it_char;

	std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_char), [](auto& _el) {
		return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::CHARACTER;
	});

	if (!all_char.empty() && !std::any_of(all_char.begin(), all_char.end(), [&pCe](auto& _el) -> bool {
		return pCe->_typeid == _el.item_typeid;
	})) {

		it_char = std::find_if(all_char.begin(), all_char.end(), [&_session, &pCe](auto& _el) {
			return (pCe = _session.m_pi.findCharacterByTypeid(_el.item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER;
		});

		// Procura o character que o player tem que ter para jogar esse Grand Prix
		if (it_char != all_char.end()) {

			// Atualiza o character equipado do player
			_session.m_pi.ei.char_info = pCe;
			_session.m_pi.ue.character_id = pCe->id;

			ret = true;
								
		}else {
									
			// Player não tem o character necessário para jogar esse Grand Prix, kick ele dá sala
			_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar item[ID=" + std::to_string(changed_char_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
					+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o character equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			*_pCe = nullptr;
			ret = true;
		}
						
	} else if (all_char.empty()) { // Verifica parts, auxparts, cards, hair e setitem

		std::vector< IFF::GrandPrixConditionEquip > all_part;
		std::vector< IFF::GrandPrixConditionEquip >::iterator it_part;

		std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_part), [](auto& _el) {
			return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::PART;
		});

		if (!all_part.empty() && !std::any_of(all_part.begin(), all_part.end(), [&pCe](auto& _el) {
			return std::find(pCe->parts_typeid, LAST_ELEMENT_IN_ARRAY(pCe->parts_typeid), _el.item_typeid) != LAST_ELEMENT_IN_ARRAY(pCe->parts_typeid);
		})) {

			it_part = std::find_if(all_part.begin(), all_part.end(), [&_session, &pCe](auto& _el) {

				uint32_t char_typeid = (iff::CHARACTER << 26) | sIff::getInstance().getItemCharIdentify(_el.item_typeid);

				return (pCe = _session.m_pi.findCharacterByTypeid(char_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER
					&& std::find(pCe->parts_typeid, LAST_ELEMENT_IN_ARRAY(pCe->parts_typeid), _el.item_typeid) != LAST_ELEMENT_IN_ARRAY(pCe->parts_typeid);
			});

			// Procura o character que o player tem que ter para jogar esse Grand Prix
			if (it_part != all_part.end()) {

				// Atualiza o character equipado do player
				_session.m_pi.ei.char_info = pCe;
				_session.m_pi.ue.character_id = pCe->id;

				ret = true;
								
			}else {
									
				// Player não tem o character necessário para jogar esse Grand Prix, kick ele dá sala
				_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] tentou trocar item[ID=" + std::to_string(changed_char_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
						+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o character equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

				*_pCe = nullptr;
				ret = true;
			}

		} else if (all_part.empty()) { // Verifica auxparts

			std::vector< IFF::GrandPrixConditionEquip > all_auxpart;
			std::vector< IFF::GrandPrixConditionEquip >::iterator it_auxpart;

			std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_auxpart), [](auto& _el) {
				return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::AUX_PART;
			});

			if (!all_auxpart.empty() && !std::any_of(all_auxpart.begin(), all_auxpart.end(), [&pCe](auto& _el) {
				return std::find(pCe->auxparts, LAST_ELEMENT_IN_ARRAY(pCe->auxparts), _el.item_typeid) != LAST_ELEMENT_IN_ARRAY(pCe->auxparts);
			})) {

				it_auxpart = std::find_if(all_auxpart.begin(), all_auxpart.end(), [&_session, &pCe](auto& _el) {
					return std::find_if(_session.m_pi.mp_ce.begin(), _session.m_pi.mp_ce.end(), [&_el, &pCe](auto& _el2) {

						pCe = &_el2.second;

						return std::find(pCe->auxparts, LAST_ELEMENT_IN_ARRAY(pCe->auxparts), _el.item_typeid) != LAST_ELEMENT_IN_ARRAY(pCe->auxparts);
					}) != _session.m_pi.mp_ce.end();
				});

				// Procura o character que o player tem que ter para jogar esse Grand Prix
				if (it_auxpart != all_auxpart.end()) {

					// Atualiza o character equipado do player
					_session.m_pi.ei.char_info = pCe;
					_session.m_pi.ue.character_id = pCe->id;

					ret = true;
								
				}else {
									
					// Player não tem o character necessário para jogar esse Grand Prix, kick ele dá sala
					_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar item[ID=" + std::to_string(changed_char_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
							+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o character equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					*_pCe = nullptr;
					ret = true;
				}

			} else if (all_auxpart.empty()) { // Verifica cards

				std::vector< IFF::GrandPrixConditionEquip > all_card;
				std::vector< IFF::GrandPrixConditionEquip >::iterator it_card;

				std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_card), [](auto& _el) {
					return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::CARD;
				});

				if (!all_card.empty() && !std::any_of(all_card.begin(), all_card.end(), [&pCe](auto& _el) {
					return std::find(pCe->card_character, LAST_ELEMENT_IN_ARRAY(pCe->card_NPC), _el.item_typeid) != LAST_ELEMENT_IN_ARRAY(pCe->card_NPC);
				})) {

					it_card = std::find_if(all_card.begin(), all_card.end(), [&_session, &pCe](auto& _el) {
						return std::find_if(_session.m_pi.mp_ce.begin(), _session.m_pi.mp_ce.end(), [&_el, &pCe](auto& _el2) {

							pCe = &_el2.second;

							return std::find(pCe->card_character, LAST_ELEMENT_IN_ARRAY(pCe->card_NPC), _el.item_typeid) != LAST_ELEMENT_IN_ARRAY(pCe->card_NPC);
						}) != _session.m_pi.mp_ce.end();
					});

					// Procura o character que o player tem que ter para jogar esse Grand Prix
					if (it_card != all_card.end()) {

						// Atualiza o character equipado do player
						_session.m_pi.ei.char_info = pCe;
						_session.m_pi.ue.character_id = pCe->id;

						ret = true;
								
					}else {
									
						// Player não tem o character necessário para jogar esse Grand Prix, kick ele dá sala
						_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] tentou trocar item[ID=" + std::to_string(changed_char_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
								+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o character equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

						*_pCe = nullptr;
						ret = true;
					}

				}else if (all_card.empty()) { // hair

					std::vector< IFF::GrandPrixConditionEquip > all_hair;
					std::vector< IFF::GrandPrixConditionEquip >::iterator it_hair;

					std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_hair), [](auto& _el) {
						return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::HAIR_STYLE;
					});

					if (!all_hair.empty() && !std::any_of(all_hair.begin(), all_hair.end(), [&pCe](auto& _el) {

						auto hair = sIff::getInstance().findHairStyle(_el.item_typeid);

						uint32_t char_typeid = (iff::CHARACTER << 26) | hair->character;

						return pCe->_typeid == char_typeid && pCe->default_hair == hair->cor;
					})) {

						it_hair = std::find_if(all_hair.begin(), all_hair.end(), [&_session, &pCe](auto& _el) {

							auto hair = sIff::getInstance().findHairStyle(_el.item_typeid);

							uint32_t char_typeid = (iff::CHARACTER << 26) | hair->character;

							return (pCe = _session.m_pi.findCharacterByTypeid(char_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER
								&& pCe->default_hair == hair->cor;
						});

						// Procura o character que o player tem que ter para jogar esse Grand Prix
						if (it_hair != all_hair.end()) {

							// Atualiza o character equipado do player
							_session.m_pi.ei.char_info = pCe;
							_session.m_pi.ue.character_id = pCe->id;

							ret = true;
								
						}else {
									
							// Player não tem o character necessário para jogar esse Grand Prix, kick ele dá sala
							_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
									+ "] tentou trocar item[ID=" + std::to_string(changed_char_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
									+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o character equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

							*_pCe = nullptr;
							ret = true;
						}

					} else if (all_hair.empty()) { // Verifica setitem

						std::vector< IFF::GrandPrixConditionEquip > all_setitem;
						std::vector< IFF::GrandPrixConditionEquip >::iterator it_setitem;

						std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_setitem), [](auto& _el) {
							return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::SET_ITEM;
						});

						if (!all_setitem.empty() && !std::any_of(all_setitem.begin(), all_setitem.end(), [&_session, &pCe](auto& _el) {

							auto setitem = sIff::getInstance().findSetItem(_el.item_typeid);

							return setitem != nullptr && std::all_of(setitem->packege.item_typeid, LAST_ELEMENT_IN_ARRAY(setitem->packege.item_typeid), [&_session, &pCe](auto& _el2) {
								
								if (_el2 == 0)
									return true;

								uint32_t type = sIff::getInstance().getItemGroupIdentify(_el2);
								uint32_t char_typeid = 0u;

								if (type == iff::CHARACTER)
									return pCe->_typeid == _el2;

								if (type == iff::PART) {

									char_typeid = (iff::CHARACTER << 26) | sIff::getInstance().getItemCharIdentify(_el2);

									if (pCe->_typeid != char_typeid)
										return false;

									return std::any_of(pCe->parts_typeid, LAST_ELEMENT_IN_ARRAY(pCe->parts_typeid), [&_el2](auto& _el3) {
										return _el2 == _el3;
									});
								}

								if (type == iff::HAIR_STYLE) {

									auto hair = sIff::getInstance().findHairStyle(_el2);

									if (hair == nullptr)
										return false;

									char_typeid = (iff::CHARACTER << 26) | hair->character;

									return pCe->_typeid == char_typeid && pCe->default_hair == hair->cor;
								}

								if (type == iff::AUX_PART)
									return std::any_of(pCe->auxparts, LAST_ELEMENT_IN_ARRAY(pCe->auxparts), [&_el2](auto& _el3) {
										return _el2 == _el3;
									});

								if (type == iff::CARD)
									return std::any_of(pCe->card_character, LAST_ELEMENT_IN_ARRAY(pCe->card_NPC), [&_el2](auto& _el3) {
										return _el2 == _el3;
									});

								return _session.m_pi.checkEquipedItem(_el2);
							});
						})) {

							it_setitem = std::find_if(all_setitem.begin(), all_setitem.end(), [&_session, &pCe](auto& _el) {
								return std::find_if(_session.m_pi.mp_ce.begin(), _session.m_pi.mp_ce.end(), [&_session, &pCe, &_el](auto& _el2) {

									auto setitem = sIff::getInstance().findSetItem(_el.item_typeid);

									pCe = &_el2.second;

									return setitem != nullptr && std::all_of(setitem->packege.item_typeid, LAST_ELEMENT_IN_ARRAY(setitem->packege.item_typeid), [&_session, &pCe](auto& _el3) {
								
										if (_el3 == 0)
											return true;

										uint32_t type = sIff::getInstance().getItemGroupIdentify(_el3);
										uint32_t char_typeid = 0u;

										if (type == iff::CHARACTER)
											return pCe->_typeid == _el3;

										if (type == iff::PART) {

											char_typeid = (iff::CHARACTER << 26) | sIff::getInstance().getItemCharIdentify(_el3);

											if (pCe->_typeid != char_typeid)
												return false;

											return std::any_of(pCe->parts_typeid, LAST_ELEMENT_IN_ARRAY(pCe->parts_typeid), [&_el3](auto& _el4) {
												return _el3 == _el4;
											});
										}

										if (type == iff::HAIR_STYLE) {

											auto hair = sIff::getInstance().findHairStyle(_el3);

											if (hair == nullptr)
												return false;

											char_typeid = (iff::CHARACTER << 26) | hair->character;

											return pCe->_typeid == char_typeid && pCe->default_hair == hair->cor;
										}

										if (type == iff::AUX_PART)
											return std::any_of(pCe->auxparts, LAST_ELEMENT_IN_ARRAY(pCe->auxparts), [&_el3](auto& _el4) {
												return _el3 == _el4;
											});

										if (type == iff::CARD)
											return std::any_of(pCe->card_character, LAST_ELEMENT_IN_ARRAY(pCe->card_NPC), [&_el3](auto& _el4) {
												return _el3 == _el4;
											});

										return _session.m_pi.checkEquipedItem(_el3);
									});

								}) != _session.m_pi.mp_ce.end();
							});

							// Procura o character que o player tem que ter para jogar esse Grand Prix
							if (it_setitem != all_setitem.end()) {

								// Atualiza o character equipado do player
								_session.m_pi.ei.char_info = pCe;
								_session.m_pi.ue.character_id = pCe->id;

								ret = true;
								
							}else {
									
								// Player não tem o character necessário para jogar esse Grand Prix, kick ele dá sala
								_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
										+ "] tentou trocar item[ID=" + std::to_string(changed_char_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
										+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o character equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

								*_pCe = nullptr;
								ret = true;
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

bool RoomGrandPrix::checkMascot(player& _session, MascotInfoEx** _pMi, std::vector< IFF::GrandPrixConditionEquip >& _gp_condition) {
	
	if (_pMi == nullptr || *_pMi == nullptr)
		return false;

	bool ret = false;
	MascotInfoEx* pMi = *_pMi;
	uint32_t changed_mascot_typeid = pMi->_typeid;

	std::vector< IFF::GrandPrixConditionEquip > all_mascot;
	std::vector< IFF::GrandPrixConditionEquip >::iterator it_mascot;

	std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_mascot), [](auto& _el) {
		return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::MASCOT;
	});

	if (!all_mascot.empty() && !std::any_of(all_mascot.begin(), all_mascot.end(), [&pMi](auto& _el) -> bool {
		return _el.item_typeid == pMi->_typeid;
	})) {

		it_mascot = std::find_if(all_mascot.begin(), all_mascot.end(), [&_session, &pMi](auto& _el) {
			return (pMi = _session.m_pi.findMascotByTypeid(_el.item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pMi->_typeid) == iff::MASCOT;
		});

		// Procura o mascot que o player tem que ter para jogar esse Grand Prix
		if (it_mascot != all_mascot.end()) {

			// Atualiza o mascot equipado do player
			_session.m_pi.ei.mascot_info = pMi;
			_session.m_pi.ue.mascot_id = pMi->id;

			ret = true;
								
		}else {
									
			// Player não tem o mascot necessário para jogar esse Grand Prix, kick ele dá sala
			_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar item[ID=" + std::to_string(changed_mascot_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
					+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o mascot equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			*_pMi = nullptr;
			ret = true;
		}
	}

	return ret;
}

bool RoomGrandPrix::checkItemSlot(player& _session, UserEquip** _pUe, std::vector< IFF::GrandPrixConditionEquip >& _gp_condition) {

	if (_pUe == nullptr || *_pUe == nullptr)
		return false;

	bool ret = false;
	UserEquip* pUe = *_pUe;

	std::vector< IFF::GrandPrixConditionEquip > all_item_equipable;
	std::vector< IFF::GrandPrixConditionEquip >::iterator it_item_equipable;

	std::copy_if(_gp_condition.begin(), _gp_condition.end(), std::back_inserter(all_item_equipable), [](auto& _el) {
		return sIff::getInstance().getItemGroupIdentify(_el.item_typeid) == iff::ITEM && sIff::getInstance().IsItemEquipable(_el.item_typeid);
	});

	if (!all_item_equipable.empty() && !std::any_of(all_item_equipable.begin(), all_item_equipable.end(), [&pUe](auto& _el) -> bool {
		return std::find(pUe->item_slot, LAST_ELEMENT_IN_ARRAY(pUe->item_slot), _el.item_typeid) != LAST_ELEMENT_IN_ARRAY(pUe->item_slot);
	})) {

		WarehouseItemEx *pWi = nullptr;

		it_item_equipable = std::find_if(all_item_equipable.begin(), all_item_equipable.end(), [&_session, &pWi](auto& _el) {
			return (pWi = _session.m_pi.findWarehouseItemByTypeid(_el.item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::ITEM && sIff::getInstance().IsItemEquipable(pWi->_typeid);
		});

		// Procura o item equipável que o player tem que ter para jogar esse Grand Prix
		if (it_item_equipable != all_item_equipable.end()) {

			// Atualiza o item slot, troca o primeiro item do slot pelo item requerido
			pUe->item_slot[0] = pWi->_typeid;

			ret = true;
								
		}else {
									
			// Player não tem o Item Equipável necessário para jogar esse Grand Prix, kick ele dá sala
			_smp::message_pool::getInstance().push(new message("Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou trocar item[ID=" + std::to_string(all_item_equipable[0].item_typeid) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
					+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o item equipado. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

			*_pUe = nullptr;
			ret = true;
		}
	}

	return ret;
}

// Static Help Check room is valid
void RoomGrandPrix::push_instancia(RoomGrandPrix* _rgp) {

	m_cs_instancia::getInstance().lock();

	m_instancias::getInstance().push_back(RoomGrandPrixInstanciaCtx(_rgp, RoomGrandPrixInstanciaCtx::eSTATE::GOOD));

	m_cs_instancia::getInstance().unlock();
}

void RoomGrandPrix::pop_instancia(RoomGrandPrix* _rgp) {

	m_cs_instancia::getInstance().lock();

	auto index = get_instancia_index(_rgp);

	if (index >= 0)
		m_instancias::getInstance().erase(m_instancias::getInstance().begin() + index);

	m_cs_instancia::getInstance().unlock();
}

void RoomGrandPrix::set_instancia_state(RoomGrandPrix* _rgp, RoomGrandPrixInstanciaCtx::eSTATE _state) {

	m_cs_instancia::getInstance().lock();

	auto index = get_instancia_index(_rgp);

	if (index >= 0)
		m_instancias::getInstance()[index].m_state = _state;

	m_cs_instancia::getInstance().unlock();
}

int RoomGrandPrix::get_instancia_index(RoomGrandPrix* _rgp) {
	
	int index = -1;

	for (auto i = 0u; i < m_instancias::getInstance().size(); ++i) {

		if (m_instancias::getInstance()[i].m_rgp == _rgp) {

			index = (int)i;

			break;
		}
	}

	return index;
}

bool RoomGrandPrix::instancia_valid(RoomGrandPrix* _rgp) {
	
	bool valid = false;

	m_cs_instancia::getInstance().lock();

	auto index = get_instancia_index(_rgp);

	if (index >= 0)
		valid = (m_instancias::getInstance()[index].m_state == RoomGrandPrixInstanciaCtx::eSTATE::GOOD);

	m_cs_instancia::getInstance().unlock();

	return valid;
}
