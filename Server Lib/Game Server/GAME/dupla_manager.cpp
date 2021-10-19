// Arquivo dupla_manager.cpp
// Criado em 29/12/2019 as 11:49 por Acrisio
// Implementa��o da classe DuplaManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "dupla_manager.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_update_guild_member_points.hpp"

using namespace stdA;

DuplaManager::DuplaManager() : v_duplas() {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

DuplaManager::~DuplaManager() {

	if (!v_duplas.empty()) {
		v_duplas.clear();
		v_duplas.shrink_to_fit();
	}

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void DuplaManager::init_duplas(Guild& _g1, Guild& _g2) {

	// Limpa duplas
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!v_duplas.empty())
		v_duplas.clear();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	std::vector< uint32_t > a, b;

	uint32_t i = 0u;

	for (i = 0u; i < _g1.numPlayers(); ++i) {
		a.push_back(i);
		b.push_back(i);
	}

	std::shuffle(a.begin(), a.end(), std::default_random_engine((uint32_t)sRandomGen::getInstance().rIbeMt19937_64_chrono()));
	std::shuffle(b.begin(), b.end(), std::default_random_engine((uint32_t)sRandomGen::getInstance().rIbeMt19937_64_chrono()));

	for (i = 0u; i < _g1.numPlayers(); ++i)
		addDupla(_g1.getPlayerByIndex(a[i]), _g2.getPlayerByIndex(b[i]));

	a.clear();
	b.clear();
	a.shrink_to_fit();
	b.shrink_to_fit();
}

void DuplaManager::addDupla(player *_p1, player *_p2) {
	
	if (_p1 == nullptr || _p2 == nullptr) {

		_smp::message_pool::getInstance().push(new message("[DuplaManager::addDupla][Error] _p" 
				+ std::string(_p1 == nullptr && _p2 == nullptr ? "1 and _p2" : (_p1 == nullptr ? "1" : "2")) + " is invalid", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	v_duplas.push_back(Dupla((unsigned char)v_duplas.size() + 1u, _p1, _p2));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void DuplaManager::deleteDupla(Dupla *_dupla) {

	if (_dupla == nullptr) {

		_smp::message_pool::getInstance().push(new message("[DuplaManager::deleteDupla][Error] _dupla is invalid(nullptr). Bug", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	deleteDupla(_dupla->numero);
}

void DuplaManager::deleteDupla(unsigned char _numero) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_duplas.begin(), v_duplas.end(), [&](auto& _el) {
		return _el.numero == _numero;
	});

	if (it != v_duplas.end())
		v_duplas.erase(it);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

Dupla* DuplaManager::findDuplaByPlayer(player& _session) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_duplas.begin(), v_duplas.end(), [&](auto& _el) {
		return _el.p[0] == &_session || _el.p[1] == &_session;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != v_duplas.end() ? &(*it) : nullptr);
}

Dupla* DuplaManager::findDuplaByPlayerUID(uint32_t _uid) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_duplas.begin(), v_duplas.end(), [&](auto& _el) {
		return (_el.p[0] != nullptr && _el.p[0]->m_pi.uid == _uid) || (_el.p[1] != nullptr && _el.p[1]->m_pi.uid == _uid);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != v_duplas.end() ? &(*it) : nullptr);
}

Dupla* DuplaManager::findDuplaByNumero(unsigned char _numero) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_duplas.begin(), v_duplas.end(), [&](auto& _el) {
		return _el.numero == _numero;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != v_duplas.end() ? &(*it) : nullptr);
}

uint32_t DuplaManager::getNumDuplas() {
	return (uint32_t)v_duplas.size();
}

uint32_t DuplaManager::getNumPlayersQuit() {

	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	std::for_each(v_duplas.begin(), v_duplas.end(), [&](auto& _el) {
	
		if (_el.state[0] == Dupla::eSTATE::OUT_GAME)
			count++;
		if (_el.state[1] == Dupla::eSTATE::OUT_GAME)
			count++;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return count;
}

uint32_t DuplaManager::getNumPlayersQuitGuild(Guild *_g) {

	if (_g == nullptr) {

		_smp::message_pool::getInstance().push(new message("[DuplaManager::getNumPlayersQuitGuild][Error] _g is invalid(nullptr). Bug.", CL_FILE_LOG_AND_CONSOLE));

		return 0;
	}

	return getNumPlayersQuitGuild(_g->getUID());
}

uint32_t DuplaManager::getNumPlayersQuitGuild(uint32_t _uid) {

	size_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	count = std::count_if(v_duplas.begin(), v_duplas.end(), [&](auto& _el) {
		return (_el.state[0] == Dupla::eSTATE::OUT_GAME && _el.p[0] != nullptr && _el.p[0]->m_pi.gi.uid == _uid)
			|| (_el.state[1] == Dupla::eSTATE::OUT_GAME && _el.p[1] != nullptr && _el.p[1]->m_pi.gi.uid == _uid);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (uint32_t)count;
}

void DuplaManager::updateGuildDados(Guild *_g1, Guild *_g2) {

	unsigned short score[2] = { 0u };
	uint64_t pang[2] = { 0ull };
	uint32_t pang_win[2] = { 0u };

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_duplas) {

		// Guild 1
		score[0] += el.sumScoreP1();
		pang[0] += el.pang[0];
		pang_win[0] += el.pang_win[0];

		//Guild 2
		score[1] += el.sumScoreP2();
		pang[1] += el.pang[1];
		pang_win[1] += el.pang_win[1];
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	// Guild 1
	if (_g1 != nullptr) {

		_g1->setPoint(score[0]);
		_g1->setPang(pang[0]);
		_g1->setPangWin(pang_win[0]);
	}

	// Guild 2
	if (_g2 != nullptr) {

		_g2->setPoint(score[1]);
		_g2->setPang(pang[1]);
		_g2->setPangWin(pang_win[1]);
	}
}

void DuplaManager::updatePangWinDuplas(Guild *_g, uint32_t _pang_win) {

	if (_g == nullptr) {

		_smp::message_pool::getInstance().push(new message("[DuplaManager::updatePangWinDuplas][Error] _g is invalid(nullptr). Bug.", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	updatePangWinDuplas(_g->getUID(), _pang_win);
}

void DuplaManager::updatePangWinDuplas(uint32_t _uid, uint32_t _pang_win) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_duplas) {

		if (el.p[0] != nullptr && el.p[0]->m_pi.gi.uid == _uid)
			el.pang_win[0] = _pang_win;
		else if (el.p[1] != nullptr && el.p[1]->m_pi.gi.uid == _uid)
			el.pang_win[1] = _pang_win;
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

bool DuplaManager::oneGuildRest() {

	bool ret = false;

	uint32_t count[2] = { 0u };

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_duplas) {

		if (el.p[0] != nullptr && el.state[0] != Dupla::eSTATE::OUT_GAME)
			count[0]++;

		if (el.p[1] != nullptr && el.state[1] != Dupla::eSTATE::OUT_GAME)
			count[1]++;
	}

	// Uma das duas guilds, seus membros sairam todos do jogo
	if (count[0] == 0u || count[1] == 0u)
		ret = true;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ret;
}

void DuplaManager::saveGuildMembersData() {

	// Update Guild Members Point and Pang Win
	GuildMemberPoints gmp{ 0u };

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	try {

		for (auto& el : v_duplas) {

			// Player 1
			if (el.p[0] != nullptr) {

				gmp.clear();

				gmp.guild_uid = el.p[0]->m_pi.gi.uid;
				gmp.member_uid = el.p[0]->m_pi.uid;
				gmp.pang = el.pang_win[0];
				gmp.point = el.sumScoreP1();

				// Update ON SERVER
				el.p[0]->m_pi.mi.guild_pang = el.p[0]->m_pi.gi.pang += gmp.pang;
				el.p[0]->m_pi.mi.guild_point = el.p[0]->m_pi.gi.point += gmp.point;

				// Update ON DB
				NormalManagerDB::add(1, new CmdUpdateGuildMemberPoints(gmp), DuplaManager::SQLDBResponse, this);
			}

			// Player 2
			if (el.p[1] != nullptr) {

				gmp.clear();

				gmp.guild_uid = el.p[1]->m_pi.gi.uid;
				gmp.member_uid = el.p[1]->m_pi.uid;
				gmp.pang = el.pang_win[1];
				gmp.point = el.sumScoreP2();

				// Update ON SERVER
				el.p[1]->m_pi.mi.guild_pang = el.p[1]->m_pi.gi.pang += gmp.pang;
				el.p[1]->m_pi.mi.guild_point = el.p[1]->m_pi.gi.point += gmp.point;

				// Update ON DB
				NormalManagerDB::add(1, new CmdUpdateGuildMemberPoints(gmp), DuplaManager::SQLDBResponse, this);
			}
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[DuplaManager::saveGuildMembersData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void DuplaManager::initPacketDuplas(packet& _p) {

	_p.init_plain((unsigned short)0xBF);

	_p.addUint8((unsigned char)v_duplas.size());

	for (auto& el : v_duplas) {

		_p.addUint8(el.numero);
		_p.addUint32(el.p[0]->m_oid);
		_p.addUint32(el.p[1]->m_oid);
	}
}

bool DuplaManager::finishHoleDupla(PlayerGameInfo& _pgi, unsigned short _seq_hole) {

	if (_seq_hole == (unsigned short)~0 && _seq_hole > 18u || _seq_hole == 0u) {

		_smp::message_pool::getInstance().push(new message("[DuplaManager::finishHoleDupla][Error] _seq_hole is invalid[VALUE=" 
				+ std::to_string(_seq_hole) + "]. Bug.", CL_FILE_LOG_AND_CONSOLE));

		return false;
	}

	auto dup = findDuplaByPlayerUID(_pgi.uid);

	if (dup != nullptr) {

		auto dup_p_index = (dup->p[0] != nullptr && dup->p[0]->m_pi.uid == _pgi.uid) ? 0u/*P1*/ : 1u/*P2*/;

		dup->dados[dup_p_index][_seq_hole - 1].tacada = _pgi.data.tacada_num;
		dup->pang[dup_p_index] = _pgi.data.pang;
		dup->dados[dup_p_index][_seq_hole - 1].finish = 1u;

		if (dup->state[!dup_p_index] == Dupla::eSTATE::OUT_GAME) {

			dup->dados[dup_p_index][_seq_hole - 1].score = 2;

			return true;

		}else if (dup->dados[!dup_p_index][_seq_hole - 1].finish) {

			if (dup->dados[dup_p_index][_seq_hole - 1].tacada < dup->dados[!dup_p_index][_seq_hole - 1].tacada)
				dup->dados[dup_p_index][_seq_hole - 1].score = 2;
			else if (dup->dados[dup_p_index][_seq_hole - 1].tacada > dup->dados[!dup_p_index][_seq_hole - 1].tacada)
				dup->dados[!dup_p_index][_seq_hole - 1].score = 2;
			else {
				dup->dados[dup_p_index][_seq_hole - 1].score = 1;
				dup->dados[!dup_p_index][_seq_hole - 1].score = 1;
			}

			return true;
		}
	}

	return false;
}

void DuplaManager::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[DuplaManager::SQLDBResponse][WARNING] _arg is nullptr com msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[DuplaManager::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_channel = reinterpret_cast< DuplaManager* >(_arg);

	switch (_msg_id) {
	case 1:	// Update Guild Members Points
	{
		auto cmd_ugmp = reinterpret_cast< CmdUpdateGuildMemberPoints* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[DuplaManager::SQLDBResponse][Log] Atualizou os Guild[UID=" + std::to_string(cmd_ugmp->getInfo().guild_uid) 
				+ "] POINTS[POINT=" + std::to_string(cmd_ugmp->getInfo().point) + ", PANG=" + std::to_string(cmd_ugmp->getInfo().pang) + "] do Player[UID=" 
				+ std::to_string(cmd_ugmp->getInfo().member_uid) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:
		break;
	}
}
