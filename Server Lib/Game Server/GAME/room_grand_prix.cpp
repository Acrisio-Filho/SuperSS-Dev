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

	// Tira a inst�ncia da classe do vector statico, por que a sala vai ser destruida
	pop_instancia(this);
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

		if (gp_condition != nullptr) {

			auto grup_type = sIff::getInstance().getItemGroupIdentify(gp_condition->item_typeid);

			switch (_cpir.type) {
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_CADDIE:
					if (grup_type == iff::CADDIE) {
						CaddieInfoEx *pCi = nullptr;

						// Caddie
						if (_cpir.caddie != 0 && (pCi = _session.m_pi.findCaddieById(_cpir.caddie)) != nullptr
								&& sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE) {

							if (gp_condition->item_typeid != pCi->_typeid) {

								// Procura o caddie que o player tem que ter para jogar esse Grand Prix
								if ((pCi = _session.m_pi.findCaddieByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE) {

									// Atualiza o caddie equipado do player
									_cpir.caddie = pCi->id;

									_session.m_pi.ei.cad_info = pCi;
									_session.m_pi.ue.caddie_id = pCi->id;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_CADDIE, 0);
									packet_func::room_broadcast(*this, p, 1);
								
								}else {
									
									// Player n�o tem o caddie necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[ID=" + std::to_string(_cpir.caddie) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o caddie equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}
					}
					break;
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_BALL:
					if (grup_type == iff::BALL) {
						WarehouseItemEx *pWi = nullptr;

						if (_cpir.ball != 0 && (pWi = _session.m_pi.findWarehouseItemByTypeid(_cpir.ball)) != nullptr
								&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL) {

							if (gp_condition->item_typeid != pWi->_typeid) {

								// Procura a Ball que o player tem que ter para jogar esse Grand Prix
								if ((pWi = _session.m_pi.findWarehouseItemByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL) {

									// Atualiza a bola equipado do player
									_cpir.ball = pWi->_typeid;

									_session.m_pi.ei.comet = pWi;
									_session.m_pi.ue.ball_typeid = pWi->_typeid;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_BALL, 0);
									packet_func::room_broadcast(*this, p, 1);
								
								}else {
									
									// Player n�o tem a bola necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[TYPEID=" + std::to_string(_cpir.ball) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar a bola equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}
					}
					break;
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_CLUBSET:
					if (grup_type == iff::CLUBSET) {
						WarehouseItemEx *pWi = nullptr;

						// ClubSet
						if (_cpir.clubset != 0 && (pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset)) != nullptr
								&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET) {

							if (gp_condition->item_typeid != pWi->_typeid) {

								// Procura a ClubSet que o player tem que ter para jogar esse Grand Prix
								if ((pWi = _session.m_pi.findWarehouseItemByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET) {

									// Atualiza o clubset equipado do player
									_cpir.clubset = pWi->id;

									_session.m_pi.ei.clubset = pWi;

									// Esse C do WarehouseItem, que pega do DB, n�o � o ja updado inicial da taqueira � o que fica tabela enchant, 
									// que no original fica no warehouse msm, eu s� confundi quando fiz
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
								
								}else {
									
									// Player n�o tem o clubset necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[ID=" + std::to_string(_cpir.clubset) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o ClubSet equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}
					}
					break;
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_CHARACTER:
					if (grup_type == iff::CHARACTER) {
						CharacterInfo *pCe = nullptr;

						if (_cpir.character != 0 && (pCe = _session.m_pi.findCharacterById(_cpir.character)) != nullptr
								&& sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER) {

							if (gp_condition->item_typeid != pCe->_typeid) {

								// Procura o character que o player tem que ter para jogar esse Grand Prix
								if ((pCe = _session.m_pi.findCharacterByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER) {

									// Atualiza o character equipado do player
									_cpir.character = pCe->id;

									_session.m_pi.ei.char_info = pCe;
									_session.m_pi.ue.character_id = pCe->id;

									// Update IN GAME
									packet_func::pacote06B(p, &_session, &_session.m_pi, 5/*Character ID only*/, 4/*Sucesso*/);
									packet_func::session_send(p, &_session, 1);
								
								}else {
									
									// Player n�o tem o character necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[ID=" + std::to_string(_cpir.character) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o character equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}
					}
					break;
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_MASCOT:
					if (grup_type == iff::MASCOT) {
						MascotInfoEx *pMi = nullptr;

						if (_cpir.mascot != 0 && (pMi = _session.m_pi.findMascotById(_cpir.mascot)) != nullptr 
								&& sIff::getInstance().getItemGroupIdentify(pMi->_typeid) == iff::MASCOT) {

							if (gp_condition->item_typeid != pMi->_typeid) {

								// Procura o mascot que o player tem que ter para jogar esse Grand Prix
								if ((pMi = _session.m_pi.findMascotByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pMi->_typeid) == iff::MASCOT) {

									// Atualiza o mascot equipado do player
									_cpir.mascot = pMi->id;

									_session.m_pi.ei.mascot_info = pMi;
									_session.m_pi.ue.mascot_id = pMi->id;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_MASCOT, 0);
									packet_func::room_broadcast(*this, p, 1);
								
								}else {
									
									// Player n�o tem o mascot necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[ID=" + std::to_string(_cpir.mascot) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o mascot equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}
					}
					break;
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_ITEM_EFFECT_LOUNGE:
					if (grup_type == iff::PART) {
						// Esse n�o usa por que � s� no lounge que troca esse item ou ativa o efeito
					}
					break;
				case ChangePlayerItemRoom::TYPE_CHANGE::TC_ALL:
				{
					if (grup_type == iff::CHARACTER) {
						CharacterInfo *pCe = nullptr;

						if (_cpir.character != 0 && (pCe = _session.m_pi.findCharacterById(_cpir.character)) != nullptr
								&& sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER) {

							if (gp_condition->item_typeid != pCe->_typeid) {

								// Procura o character que o player tem que ter para jogar esse Grand Prix
								if ((pCe = _session.m_pi.findCharacterByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER) {

									// Atualiza o character equipado do player
									_cpir.character = pCe->id;

									_session.m_pi.ei.char_info = pCe;
									_session.m_pi.ue.character_id = pCe->id;

									// Update IN GAME
									packet_func::pacote06B(p, &_session, &_session.m_pi, 5/*Character ID only*/, 4/*Sucesso*/);
									packet_func::session_send(p, &_session, 1);
								
								}else {
									
									// Player n�o tem o character necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[ID=" + std::to_string(_cpir.character) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o character equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}

					}else if (grup_type == iff::CADDIE) {
						CaddieInfoEx *pCi = nullptr;

						// Caddie
						if (_cpir.caddie != 0 && (pCi = _session.m_pi.findCaddieById(_cpir.caddie)) != nullptr
								&& sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE) {

							if (gp_condition->item_typeid != pCi->_typeid) {

								// Procura o caddie que o player tem que ter para jogar esse Grand Prix
								if ((pCi = _session.m_pi.findCaddieByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE) {

									// Atualiza o caddie equipado do player
									_cpir.caddie = pCi->id;

									_session.m_pi.ei.cad_info = pCi;
									_session.m_pi.ue.caddie_id = pCi->id;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_CADDIE, 0);
									packet_func::room_broadcast(*this, p, 1);
								
								}else {
									
									// Player n�o tem o caddie necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[ID=" + std::to_string(_cpir.caddie) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o caddie equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}

					}else if (grup_type == iff::CLUBSET) {
						WarehouseItemEx *pWi = nullptr;

						// ClubSet
						if (_cpir.clubset != 0 && (pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset)) != nullptr
								&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET) {

							if (gp_condition->item_typeid != pWi->_typeid) {

								// Procura a ClubSet que o player tem que ter para jogar esse Grand Prix
								if ((pWi = _session.m_pi.findWarehouseItemByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET) {

									// Atualiza o clubset equipado do player
									_cpir.clubset = pWi->id;

									_session.m_pi.ei.clubset = pWi;

									// Esse C do WarehouseItem, que pega do DB, n�o � o ja updado inicial da taqueira � o que fica tabela enchant, 
									// que no original fica no warehouse msm, eu s� confundi quando fiz
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
								
								}else {
									
									// Player n�o tem o clubset necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[ID=" + std::to_string(_cpir.clubset) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar o ClubSet equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}

					}else if (grup_type == iff::BALL) {
						WarehouseItemEx *pWi = nullptr;

						if (_cpir.ball != 0 && (pWi = _session.m_pi.findWarehouseItemByTypeid(_cpir.ball)) != nullptr
								&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL) {

							if (gp_condition->item_typeid != pWi->_typeid) {

								// Procura a Ball que o player tem que ter para jogar esse Grand Prix
								if ((pWi = _session.m_pi.findWarehouseItemByTypeid(gp_condition->item_typeid)) != nullptr && sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL) {

									// Atualiza a bola equipado do player
									_cpir.ball = pWi->_typeid;

									_session.m_pi.ei.comet = pWi;
									_session.m_pi.ue.ball_typeid = pWi->_typeid;

									// Update IN GAME
									packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_BALL, 0);
									packet_func::room_broadcast(*this, p, 1);
								
								}else {
									
									// Player n�o tem a bola necess�rio para jogar esse Grand Prix, kick ele d� sala
									_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid)
											+ "] tentou trocar item[TYPEID=" + std::to_string(_cpir.ball) + "] equipado na sala[NUMERO=" + std::to_string(m_ri.numero)
											+ "] Grand Prix, mas a sala tem uma condicao que nao pode trocar a bola equipada. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
								}
							}
						}

					}
					break;
				}
			}

		} // fim do iff que verifica se o gp_condition � v�lido

		// Chama o changePlayerItemRoom d� sala padr�o para fazer as altera��es
		room::requestChangePlayerItemRoom(_session, _cpir);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RoomGrandPrix::requestChangePlayerItemRoom][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		packet_func::pacote04B(p, &_session, _cpir.type, (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::ROOM ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1/*Unknown Error*/));
		packet_func::session_send(p, &_session, 0);
	}
}

// Game, esse aqui � s� para o Grand Prix ROOKIE(TUTO)
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
