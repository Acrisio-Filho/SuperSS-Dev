// Arquivo golden_time_system.cpp
// Criado em 20/10/2020 as 18:58 por Acrisio
// Implementa��o da classe GoldenTimeSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "golden_time_system.hpp"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../TYPE/pangya_game_st.h"

#include "item_manager.h"
#include "mail_box_manager.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"
#include "../PANGYA_DB/cmd_golden_time_info.hpp"
#include "../PANGYA_DB/cmd_golden_time_item.hpp"
#include "../PANGYA_DB/cmd_golden_time_round.hpp"
#include "../PANGYA_DB/cmd_update_golden_time.hpp"

#include "../UTIL/lottery.hpp"
#include <ctime>

#include <algorithm>

#include "../Game Server/game_server.h"

#if defined(_WIN32)
#define TRY_CHECK			 try { \
								EnterCriticalSection(&m_cs);
#elif defined(__linux__)
#define TRY_CHECK			 try { \
								pthread_mutex_lock(&m_cs);
#endif

#if defined(_WIN32)
#define LEAVE_CHECK				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
#define LEAVE_CHECK				pthread_mutex_unlock(&m_cs);
#endif

#if defined(_WIN32)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								LeaveCriticalSection(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

#define RETURNN_CHECK(_ret_value)	{ \
										LEAVE_CHECK; \
										return (_ret_value); \
									} \

#define RETURN_CHECK()				{ \
										LEAVE_CHECK; \
										return; \
									} \

using namespace stdA;

GoldenTimeSystem::GoldenTimeSystem() : m_events(), m_current_golden_time(nullptr) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	// Inicializa
	initialize();
}

GoldenTimeSystem::~GoldenTimeSystem() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void GoldenTimeSystem::initialize() {

	TRY_CHECK;

	// Carrega a lista de eventos
	CmdGoldenTimeInfo cmd_gti(true); // Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_gti, nullptr, nullptr);

	cmd_gti.waitEvent();

	if (cmd_gti.getException().getCodeError() != 0)
		throw cmd_gti.getException();

	CmdGoldenTimeItem cmd_gt_item(true); // Waiter;
	CmdGoldenTimeRound cmd_gt_round(true); // Waiter

	m_events = cmd_gti.getInfo();

	for (auto& el_gt : m_events) {

		cmd_gt_item.setId(el_gt.id);
		cmd_gt_round.setId(el_gt.id);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_gt_item, nullptr, nullptr);
		snmdb::NormalManagerDB::getInstance().add(0, &cmd_gt_round, nullptr, nullptr);

		try {

			cmd_gt_item.waitEvent();
			cmd_gt_round.waitEvent();

			if (cmd_gt_item.getException().getCodeError() != 0)
				throw cmd_gt_item.getException();

			if (cmd_gt_round.getException().getCodeError() != 0)
				throw cmd_gt_round.getException();

			el_gt.item_rewards = cmd_gt_item.getInfo();

			el_gt.rounds = cmd_gt_round.getInfo();

			for (auto& el_r : el_gt.rounds) {
				el_r.time.wYear = el_gt.date[0].wYear;
				el_r.time.wMonth = el_gt.date[0].wMonth;
				el_r.time.wDay = el_gt.date[0].wDay;
				el_r.time.wDayOfWeek = el_gt.date[0].wDayOfWeek;
			}

		}catch (exception& e) {

			// Log
			_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::initialize][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}

	// Inicializa a ordem dos rounds e dos golden time, e atualiza as datas se for necess�rio

	// Sort Rounds
	for (auto& el : m_events) {

		std::sort(el.rounds.begin(), el.rounds.end(), [](stRound& _el1, stRound& _el2) {
			return getTimeDiff(_el1.time, _el2.time) < 0ll;
		});
	}

	// Sort Golden Time
	std::sort(m_events.begin(), m_events.end(), [](stGoldenTime& _el1, stGoldenTime& _el2) {

		if (getDateDiff(_el1.date[0], _el2.date[0]) == 0ll) {
			
			if (!_el1.rounds.empty() && _el2.rounds.empty())
				return true;
			else if (_el1.rounds.empty() && !_el2.rounds.empty())
				return false;
			
			return getTimeDiff(_el1.rounds.begin()->time, _el2.rounds.begin()->time) < 0ll;
		}

		return getDateDiff(_el1.date[0], _el2.date[0]) < 0ll;
	});

	// Verifica
	for (auto& el : m_events) {

		if (el.type != stGoldenTime::eTYPE::ONE_DAY) {

			if (el.type == stGoldenTime::eTYPE::INTERVAL && (isEmpty(el.date[1]) || getLocalDateDiff(el.date[1]) > 0ll))
				el.is_end = true;
			else if (getLocalDateDiff(el.date[0]) > 0ll) {

				GetLocalTime(&el.date[0]);

				el.date[0].wHour = el.date[0].wMinute = el.date[0].wSecond = el.date[0].wMilliseconds = 0u;

				for (auto& el_round : el.rounds) {
					el_round.time.wYear = el.date[0].wYear;
					el_round.time.wMonth = el.date[0].wMonth;
					el_round.time.wDay = el.date[0].wDay;
					el_round.time.wDayOfWeek = el.date[0].wDayOfWeek;
				}
			}

		}else if (getLocalDateDiff(el.date[0]) > 0ll)
			el.is_end = true;
	}

	std::string log = "";

	for (auto& el_e : m_events) {

		log += "\nGolden Time[ID=" + std::to_string(el_e.id) + ", TYPE=" + std::to_string((unsigned short)el_e.type) + ", RATE=" + std::to_string(el_e.rate_of_players)
			+ "] - Date[BEGIN=" + std::to_string(el_e.date[0].wYear) + "/" + std::to_string(el_e.date[0].wMonth) + "/" + std::to_string(el_e.date[0].wDay)
			+ ", END=" + std::to_string(el_e.date[1].wYear) + "/" + std::to_string(el_e.date[1].wMonth) + "/" + std::to_string(el_e.date[1].wDay) 
			+ "] ROUND[COUNT=" + std::to_string(el_e.rounds.size()) + "]:\n\tROUND:";

		for (auto& el_r : el_e.rounds)
			log += "\n\t\tROUND - TIME[" + std::to_string(el_r.time.wHour) + ":" + std::to_string(el_r.time.wMinute) + ":" + std::to_string(el_r.time.wSecond) + "]";

		log += "\n\tITEM:";

		for (auto& el_i : el_e.item_rewards)
			log += "\n\t\tITEM[TYPEID=" + std::to_string(el_i._typeid) + ", QNTD=" + std::to_string(el_i.qntd) + ", QNTD_TIME=" + std::to_string(el_i.qntd_time) + "]";
	}

	// Log
	_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::initialize][Log] Golden Time Event - Carregou: " + log, CL_FILE_LOG_AND_CONSOLE));

	//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::initialize][Log] Golden Time System carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
	//#else
		//_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::initialize][Log] Golden Time System carregado com sucesso!", CL_ONLY_FILE_LOG));
	//#endif // _DEBUG

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void GoldenTimeSystem::clear() {

	TRY_CHECK;

	if (!m_events.empty())
		m_events.clear();

	m_current_golden_time = nullptr;

	m_load = false;

	LEAVE_CHECK;
	CATCH_CHECK("clear");
	END_CHECK;
}

void GoldenTimeSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool GoldenTimeSystem::isLoad() {
	
	bool isLoad = false;

	TRY_CHECK;

	isLoad = (m_load);

	LEAVE_CHECK;
	CATCH_CHECK("isLoad");
	END_CHECK;

	return isLoad;
}

void GoldenTimeSystem::initCurrentGoldenTime() {

	bool flag_repeat_item = !(m_current_golden_time->item_rewards.size() < m_current_golden_time->rounds.size());

	Lottery lottery((uint64_t)std::time(nullptr));

	Lottery::LotteryCtx* ctx = nullptr;

	for (auto& el : m_current_golden_time->item_rewards)
		lottery.push(el.rate == 0u ? 100u : el.rate, (size_t)&el);

	auto it_round = m_current_golden_time->rounds.begin();

	while (it_round != m_current_golden_time->rounds.end()) {

		if ((ctx = lottery.spinRoleta(flag_repeat_item)) == nullptr || (uint32_t*)ctx->value == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::initCurrentGoldenTime][Error][WARNING] nao conseguiu sortear um item para o round("
					+ _formatDate(it_round->time) + ")", CL_FILE_LOG_AND_CONSOLE));

			// Continua
			continue;
		}

		it_round->item.clear();
		it_round->executed = false;
		it_round->item = *(stItemReward*)ctx->value;

		// Next Round
		it_round++;
	}
}

stGoldenTime* GoldenTimeSystem::findNewGoldenTime() {
	
	if (m_events.empty())
		return nullptr;

	stGoldenTime *gt = nullptr;

	// Cria um novo se tiver eventos
	auto it_ev = std::find_if(m_events.begin(), m_events.end(), findForever);

	if (it_ev != m_events.end() && gt == nullptr)
		gt = &(*it_ev);

	it_ev = std::find_if(m_events.begin(), m_events.end(), findInterval);

	if (it_ev != m_events.end() && (gt == nullptr || getTimeDiff(it_ev->date[0], gt->date[0]) <= 0ll))
		gt = &(*it_ev);

	it_ev = std::find_if(m_events.begin(), m_events.end(), findOneDay);

	if (it_ev != m_events.end() && (gt == nullptr || getTimeDiff(it_ev->date[0], gt->date[0]) <= 0ll))
		gt = &(*it_ev);

	// Retorna o com menor tempo, endependente da prioridade
	return gt;
}

void GoldenTimeSystem::updateGoldenTimeEnd() {

	// Atual golden time acabou, procura um novo
	if (m_current_golden_time->type != stGoldenTime::eTYPE::ONE_DAY) {
		
		m_current_golden_time->date[0] = UnixToSystemTime(SystemTimeToUnix(m_current_golden_time->date[0]) + (24 * 60 * 60)); // Add 1 dia

		// update all date of rounds
		for (auto& el : m_current_golden_time->rounds) {
			el.time.wYear = m_current_golden_time->date[0].wYear;
			el.time.wMonth = m_current_golden_time->date[0].wMonth;
			el.time.wDay = m_current_golden_time->date[0].wDay;
			el.time.wDayOfWeek = m_current_golden_time->date[0].wDayOfWeek;
		}

		if (m_current_golden_time->type == stGoldenTime::eTYPE::INTERVAL && !isEmpty(m_current_golden_time->date[1])
				&& getDateDiff(m_current_golden_time->date[0], m_current_golden_time->date[1]) > 0ll)
			m_current_golden_time->is_end = true;
	
	}else
		m_current_golden_time->is_end = true;

	// Atualiza no banco de dados
	if (m_current_golden_time->is_end)
		snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdateGoldenTime(m_current_golden_time->id, m_current_golden_time->is_end), GoldenTimeSystem::SQLDBResponse, this);
}

bool GoldenTimeSystem::findForever(stGoldenTime& _el) {

	if (isEmpty(_el.date[0]))
		return false;

	if (!_el.is_end && _el.type == stGoldenTime::eTYPE::FOREVER)
		return true;

	return false;
}

bool GoldenTimeSystem::findInterval(stGoldenTime& _el) {

	if (isEmpty(_el.date[0]))
		return false;

	if (!_el.is_end && !isEmpty(_el.date[1]) && _el.type == stGoldenTime::eTYPE::INTERVAL
			&& getLocalDateDiff(_el.date[1]) <= 0ll)
		return true;

	return false;
}

bool GoldenTimeSystem::findOneDay(stGoldenTime& _el) {

	if (isEmpty(_el.date[0]))
		return false;

	if (!_el.is_end && isSameDayNow(_el.date[0]) && _el.type == stGoldenTime::eTYPE::ONE_DAY)
		return true;

	return false;
};

bool GoldenTimeSystem::checkRound() {

	// init function
	bool ret = false;

	TRY_CHECK;

	if (m_events.empty() && m_current_golden_time == nullptr)
		RETURNN_CHECK(false);

	if (m_current_golden_time == nullptr) {
		
		m_current_golden_time = findNewGoldenTime();

		if (m_current_golden_time == nullptr)
			RETURNN_CHECK(false);

		m_current_golden_time->updateRound();

		if (m_current_golden_time->current_round == nullptr) {

			updateGoldenTimeEnd();

			// golden time event, finished or past time
			m_current_golden_time = nullptr;

			RETURNN_CHECK(false);
		}

		// Tira o Round, para inicializar no makeOfListOfPlayersToGoldenTime
		m_current_golden_time->current_round = nullptr;

		initCurrentGoldenTime();

		ret = true;

	}else if (m_current_golden_time->current_round == nullptr)
		ret = true;
	else if (!m_current_golden_time->current_round->executed && getLocalTimeDiffDESC(m_current_golden_time->current_round->time) <= 0ll)
		ret = true;

	LEAVE_CHECK;
	CATCH_CHECK("checkRound");
	END_CHECK;

	return ret;
}

stGoldenTimeReward GoldenTimeSystem::calculeRoundReward(std::vector< stPlayerReward > _player_reward) {
	
	stGoldenTimeReward reward;

	// Lambda[getNumberOfRate]
	auto getNumberOfRate = [](bool _is_playing, bool _is_premium) -> uint32_t {

		uint32_t rate = 0u;

		if (_is_playing)
			rate++;

		if (_is_premium)
			rate++;

		return rate * 100u;
	};

	TRY_CHECK;

	if (_player_reward.empty())
		throw exception("[GoldenTimeSystem::calculeRoundReward][Error] not have player to reward(_player_reward is empty).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GOLDEN_TIME_SYSTEM, 1, 0));

	if (m_current_golden_time == nullptr)
		throw exception("[GoldenTimeSystem::calculeRoundReward][Error] current goden time is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GOLDEN_TIME_SYSTEM, 2, 0));

	if (m_current_golden_time->current_round == nullptr)
		throw exception("[GoldenTimeSystem::calculeRoundReward][Error] current round is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GOLDEN_TIME_SYSTEM, 3, 0));

	reward.round = *m_current_golden_time->current_round;

	uint32_t number_of_winners = ((uint32_t)_player_reward.size() / NUMBER_OF_PLAYER_TO_WINNER) * m_current_golden_time->rate_of_players;

	if (number_of_winners == 0u)
		number_of_winners = 1u;

	if ((number_of_winners * m_current_golden_time->rate_of_players) > _player_reward.size())
		number_of_winners = (uint32_t)_player_reward.size();

	// Log
	_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::calculeRoundReward][Log] Golden Time[Rate=" + std::to_string(m_current_golden_time->rate_of_players) 
			+ "] Round(" + _formatDate(reward.round.time) + ") - Total de participantes(" + std::to_string(_player_reward.size()) 
			+ ") - Total de ganhadores(" + std::to_string(number_of_winners) + ").", CL_FILE_LOG_AND_CONSOLE));

	Lottery lottery((uint64_t)std::time(nullptr));

	for (auto& el : _player_reward)
		lottery.push(100 + getNumberOfRate(el.is_playing, el.is_premium), (size_t)&el);

	Lottery::LotteryCtx* ctx = nullptr;

	number_of_winners = (lottery.getCountItem() < number_of_winners ? lottery.getCountItem() : number_of_winners);

	while (number_of_winners > 0) {

		if ((ctx = lottery.spinRoleta(true/*N�o pode ganhar 2x o mesmo player*/)) != nullptr && (uint32_t*)ctx->value != nullptr) {

			reward.players.push_back(*(stPlayerReward*)ctx->value);

			number_of_winners--;

		}else
			_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::calculeRoundReward][Error][WARNING] nao conseguiu sortear um player em Lottery.spinRoleta(). Bug", CL_FILE_LOG_AND_CONSOLE));
	} 

	LEAVE_CHECK;
	CATCH_CHECK("calculeRoundReward");
	END_CHECK;

	return reward;
}

bool GoldenTimeSystem::checkFirstMessage() {
	
	bool ret = false;

	TRY_CHECK;

	if (m_current_golden_time->current_round != nullptr)
		RETURNN_CHECK(false);

	m_current_golden_time->updateRound();

	// Para quem chama d� o error
	if (m_current_golden_time->current_round == nullptr)
		RETURNN_CHECK(true);

	ret = true;

	LEAVE_CHECK;
	CATCH_CHECK("checkFirstMessage");
	END_CHECK;

	return ret;
}

stRound* GoldenTimeSystem::checkNextRound() {

	stRound* round = nullptr;
	
	TRY_CHECK;

	if (m_current_golden_time == nullptr) {

		// Cria um novo se tiver eventos
		m_current_golden_time = findNewGoldenTime();

		if (m_current_golden_time == nullptr)
			RETURNN_CHECK(nullptr);

		initCurrentGoldenTime();

		m_current_golden_time->updateRound();

		RETURNN_CHECK(m_current_golden_time->current_round);
	}

	if (m_current_golden_time->current_round == nullptr || m_current_golden_time->current_round->executed) {
		
		while (m_current_golden_time != nullptr && m_current_golden_time->updateRound() == nullptr) {

			updateGoldenTimeEnd();

			m_current_golden_time = findNewGoldenTime();

			if (m_current_golden_time == nullptr)
				RETURNN_CHECK(nullptr);
		}

		initCurrentGoldenTime();
		
		RETURNN_CHECK(m_current_golden_time->current_round);
	}

	// Terminou turno
	if (getLocalTimeDiffDESC(m_current_golden_time->current_round->time) < 0ll) {

		m_current_golden_time->current_round->executed = true;

		while (m_current_golden_time != nullptr && m_current_golden_time->updateRound() == nullptr) {

			updateGoldenTimeEnd();

			m_current_golden_time = findNewGoldenTime();

			if (m_current_golden_time == nullptr)
				RETURNN_CHECK(nullptr);
		}

		initCurrentGoldenTime();

		RETURNN_CHECK(m_current_golden_time->current_round);
	}

	LEAVE_CHECK;
	CATCH_CHECK("checkNextRound");
	END_CHECK;

	return round;
}

stRound* GoldenTimeSystem::getCurrentRound() {

	stRound* round = nullptr;

	TRY_CHECK;

	if (m_current_golden_time == nullptr)
		RETURNN_CHECK(nullptr);

	round = m_current_golden_time->current_round;

	LEAVE_CHECK;
	CATCH_CHECK("getCurrentRound");
	END_CHECK;

	return round;
}

void GoldenTimeSystem::sendRewardToMailOfPlayers(stGoldenTimeReward& _reward) {

	// Lambda[getItemName]
	auto getItemName = [](uint32_t _typeid) -> std::string {

		std::string ret = "";

		auto base = sIff::getInstance().findCommomItem(_typeid);

		if (base != nullptr)
			ret = std::string(base->name);

		return ret;
	};

	try {

		stItem item{ 0 };
		BuyItem bi{ 0 };

		player* p = nullptr;

		for (auto& el : _reward.players) {

			if ((p = sgs::gs::getInstance().findPlayer(el.uid)) == nullptr) {

				// Log, Player que ganhou n�o est� mais online, vai ficar sem o item
				_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::sendRewardToMailOfPlayers][WARNING] Player[UID=" + std::to_string(el.uid)
						+ "] ganhou o item[TYPEID=" + std::to_string(_reward.round.item._typeid) + ", QNTD=" + std::to_string(_reward.round.item.qntd)
						+ ", QNTD_TIME=" + std::to_string(_reward.round.item.qntd_time) + "], mas saiu antes dos pr�mios ser entregues, vai ficar sem o pr�mio.", CL_FILE_LOG_AND_CONSOLE));

				continue;
			}

			// Limpa
			bi.clear();
			item.clear();

			// Initialize
			bi.id = -1;
			bi._typeid = _reward.round.item._typeid;
			bi.qntd = _reward.round.item.qntd;
			bi.time = (unsigned short)_reward.round.item.qntd_time;

			item_manager::initItemFromBuyItem(p->m_pi, item, bi, false, 0, 0, 1/*~nao Check Level*/);

			if (item._typeid == 0)
				_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::sendRewardToMailOfPlayers][Error][WARNING] tentou enviar o reward para o player[UID="
						+ std::to_string(p->m_pi.uid) + "] o Item[TYPEID=" + std::to_string(_reward.round.item._typeid) + ", QNTD=" + std::to_string(_reward.round.item.qntd)
						+ ", QNTD_TIME=" + std::to_string(_reward.round.item.qntd_time) + "], mas nao conseguiu inicializar o item. Bug", CL_FILE_LOG_AND_CONSOLE));

			auto msg = std::string("Golden Time - Round(" + _formatDate(_reward.round.time) + "): item[ " + getItemName(_reward.round.item._typeid) + " ]");

			if (MailBoxManager::sendMessageWithItem(0, p->m_pi.uid, msg, item) <= 0)
				_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::sendRewardToMailOfPlayers][Error][WARNING] tentou enviar reward para o player[UID="
						+ std::to_string(p->m_pi.uid) + "] o Item[TYPEID=" + std::to_string(_reward.round.item._typeid) + ", QNTD=" + std::to_string(_reward.round.item.qntd) + ", QNTD_TIME="
						+ std::to_string(_reward.round.item.qntd_time) + "], mas nao conseguiu colocar o item no mail box dele. Bug", CL_FILE_LOG_AND_CONSOLE));
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::sendRewardToMailOfPlayers][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GoldenTimeSystem::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
#ifdef _DEBUG
		// Static class
		_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::SQLDBResponse][WARNING] _arg is nullptr na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto gts = reinterpret_cast<GoldenTimeSystem*>(_arg);

	switch (_msg_id) {
	case 1:	// Update Golden Time is_end
	{

		auto cmd_ugt = reinterpret_cast<CmdUpdateGoldenTime*>(&_pangya_db);

		// Log
		_smp::message_pool::getInstance().push(new message("[GoldenTimeSystem::SQLDBResponse][Log] Atualizou o Golden Time[ID=" + std::to_string(cmd_ugt->getId())
			+ ", IS_END=" + std::string(cmd_ugt->getIsEnd() ? "TRUE" : "FALSE") + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:
		break;
	}
};
