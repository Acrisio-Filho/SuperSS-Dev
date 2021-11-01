// Arquivo rank_registry_manager.cpp
// Criado em 16/06/2020 as 15:29 por Acrisio
// Implementa��o da classe RankRegistryManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <filesystem>
#define MODE_DIR_LOG (S_IRWXU | S_IROTH | S_IXOTH)
#endif

#include "rank_registry_manager.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_rank_registry_info.hpp"
#include "../PANGYA_DB/cmd_rank_registry_character_info.hpp"

#include "../PACKET/packet_func_rs.hpp"

#include <algorithm>
#include <ctime>

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[RankRegistryManager::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 1, 0)); \

#define NUMBER_OF_PAGE_MENU_REGISTRY(__num_registrys) (((__num_registrys) % LIMIT_REGISTRY_FOR_PAGE) == 0u ? (__num_registrys) / LIMIT_REGISTRY_FOR_PAGE : (__num_registrys) / LIMIT_REGISTRY_FOR_PAGE + 1u)

using namespace stdA;

RankRegistryManager::RankRegistryManager() : m_entry(), m_character_entry(), m_state(false) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	// Log
	log.exceptions(std::ofstream::failbit | std::ofstream::badbit);

	prex = "";
	dir = "Log";

	// Inicializa
	initialize();
}

RankRegistryManager::~RankRegistryManager() {

	clear();

	// Log
	close_log();

	prex.clear();
	prex.shrink_to_fit();

	dir.clear();
	dir.shrink_to_fit();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void RankRegistryManager::load() {

	if (isLoad())
		clear();

	initialize();
}

bool RankRegistryManager::isLoad() {

	bool ret = false;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		ret = (m_state && !m_entry.empty() && !m_character_entry.empty());

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::isLoad][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void RankRegistryManager::pageToPacket(packet& _packet, search_dados& _sd) {

	if (!isLoad())
		throw exception("[RankRegistryManager::pageToPacket][Error] rank registry manager not loaded, please call load function first.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		key_menu km(_sd.rank_menu, _sd.rank_menu_item);

		auto it = m_entry.find(km);

		if (it != m_entry.end()) {

			// Encontrou
			auto range = getPage(it, _sd.page);

			if (range.second && range.first.first != range.first.second && std::distance(range.first.first, range.first.second) > 0u) {

				_packet.addUint32(_sd.page);															// P�gina atual
				_packet.addUint32(NUMBER_OF_PAGE_MENU_REGISTRY((uint32_t)it->second.size()));		// P�ginas

				// N�mero de registros
				_packet.addUint16((unsigned short)std::distance(range.first.first, range.first.second));

				RankCharacterEntry::iterator it_chr_entry;

				for (auto it_entry = range.first.first; it_entry != range.first.second; ++it_entry) {

					it_entry->second.toPacket(_packet);

					if ((it_chr_entry = m_character_entry.find(it_entry->second.getUID())) != m_character_entry.end())
						it_chr_entry->second.playerInfoToPacket(_packet);
					else {

						// N�o tem character Info do player, WARNING
						_smp::message_pool::getInstance().push(new message("[RankRegistryManager::pageToPacket][WARNING] Nao tem o Character Info do player[UID="
								+ std::to_string(it_entry->second.getUID()) + "], manda valor padrao para nao da erro no cliente.", CL_FILE_LOG_AND_CONSOLE));

						_packet.addZeroByte(7u);	// 1 Level, 2 Unknown, 2 size id, 2 size nickname
					}
				}

			}else
				_packet.addZeroByte(10); // 4 P�gina, 4 P�ginas, e 2 num entrys
		
		}else 
			_packet.addZeroByte(10); // 4 P�gina, 4 P�ginas, e 2 num entrys

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::pageToPacket][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void RankRegistryManager::playerPositionToPacket(packet& _packet, player& _session, search_dados& _sd) {
	CHECK_SESSION_BEGIN("playerPositionToPacket");

	if (!isLoad())
		throw exception("[RankRegistryManager::playerPositionToPacket][Error] rank registry manager not loaded, please call load function first.",
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		key_menu km(_sd.rank_menu, _sd.rank_menu_item);

		auto it_entry = m_entry.find(km);

		if (it_entry != m_entry.end()) {

			auto it = std::find_if(it_entry->second.begin(), it_entry->second.end(), [&](auto& _el) {
				return _el.first.m_uid == _session.m_pi.uid;
			});

			if (it != it_entry->second.end()) {

				_packet.addUint8(ePLAYER_POSITION_RANK_TYPE::PPRT_IN_TOP_RANK);

				it->second.toPacket(_packet);

				RankCharacterEntry::iterator it_chr_entry;

				if ((it_chr_entry = m_character_entry.find(it->second.getUID())) != m_character_entry.end())
					it_chr_entry->second.playerInfoToPacket(_packet);
				else {

					// N�o tem character Info do player, WARNING
					_smp::message_pool::getInstance().push(new message("[RankRegistryManager::playerPositionToPacket][WARNING] Nao tem o Character Info do player[UID=" 
							+ std::to_string(it->second.getUID()) + "], manda valor padrao para nao da erro no cliente.", CL_FILE_LOG_AND_CONSOLE));

					_packet.addZeroByte(7u);	// 1 Level, 2 Unknown, 2 size id, 2 size nickname
				}

			}else
				_packet.addUint8(ePLAYER_POSITION_RANK_TYPE::PPRT_NOT_RANK); // N�o tem registro do player nesse Menu->Item (Rank)

		}else
			_packet.addUint8(ePLAYER_POSITION_RANK_TYPE::PPRT_NOT_RANK); // N�o tem registro nesse Menu->Item (Rank)

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::playerPositionToPacket][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void RankRegistryManager::sendPlayerFullInfo(player& _session, uint32_t _uid/*Player Info*/) {
	CHECK_SESSION_BEGIN("sendPlayerFullInfo");

	packet p;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!isLoad())
			throw exception("[RankRegistryManager::sendPlayerFullInfo][Error] rank registry manager not loaded, please call load function first.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

		auto it_chr_entry = m_character_entry.find(_uid);

		if (it_chr_entry == m_character_entry.end())
			throw exception("[RankRegistryManager::sendPlayerFullInfo][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] Pediu o info completo do Player[UID=" + std::to_string(_uid) + "], mas nao tem o registro de character no Rank.", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 3, 0));

		auto all_overall = getAllOverallInfoFromPlayer(_uid);

		if (all_overall.empty())
			_smp::message_pool::getInstance().push(new message("[RankRegistryManager::sendPlayerFullInfo][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] pediu o info completo do Player[UID=" + std::to_string(_uid) + "], mas nao tem nenhum registro do rank Overall.", CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x138A);

		p.addUint8(0u);	// OK

		it_chr_entry->second.playerFullInfoPacket(p);
		it_chr_entry->second.playerCharacterInfoToPacket(p);

		if (!all_overall.empty())
			p.addUint8(0u);	// OK Tem os dados do Rank Overall
		else
			p.addUint8(1u);	// N�o tem os dados do Rank Overall

		for (auto& el : all_overall)
			el.second.toCompactPacket(p);

		packet_func::session_send(p, &_session, 1);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::sendPlayerFullInfo][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x138A);

		p.addUint8(1u);	// Error

		packet_func::session_send(p, &_session, 1);
	}
}

void RankRegistryManager::sendPageFoundPlayer(player& _session, FoundPlayer& _fp, search_dados& _sd) {
	CHECK_SESSION_BEGIN("sendPageFoundPlayer");

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!isLoad())
			throw exception("[RankRegistryManager::sendPageFoundPlayer][Error] rank registry manager not loaded, please call load function first.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

		packet p((unsigned short)0x138C);

		key_menu km(_sd.rank_menu, _sd.rank_menu_item);

		auto it = m_entry.find(km);

		if (it != m_entry.end()) {

			// New Page, Page Found Player
			_sd.page = (uint32_t)_fp.second;

			// Encontrou
			auto range = getPage(it, _sd.page);

			if (range.second && range.first.first != range.first.second && std::distance(range.first.first, range.first.second) > 0u) {

				p.addUint8(0u);	// OK
				
				p.addUint8(_sd.rank_menu);
				p.addUint8(_sd.rank_menu_item);

				// Op��es descontinuadas no Fresh UP!, por�m ele ainda mant�m nos packet
				p.addUint8(_sd.term_s5_type);
				
				// Op��es descontinuadas no Fresh UP!, por�m ele ainda mant�m nos packet
				p.addUint8(_sd.class_type);

				p.addUint32(_sd.page);																// P�gina atual
				p.addUint32(NUMBER_OF_PAGE_MENU_REGISTRY((uint32_t)it->second.size()));		// P�ginas

				// N�mero de registros
				p.addUint16((unsigned short)std::distance(range.first.first, range.first.second));

				RankCharacterEntry::iterator it_chr_entry;

				for (auto it_entry = range.first.first; it_entry != range.first.second; ++it_entry) {

					it_entry->second.toPacket(p);

					if ((it_chr_entry = m_character_entry.find(it_entry->second.getUID())) != m_character_entry.end())
						it_chr_entry->second.playerInfoToPacket(p);
					else {

						// N�o tem character Info do player, WARNING
						_smp::message_pool::getInstance().push(new message("[RankRegistryManager::sendPageFoundPlayer][WARNING] Nao tem o Character Info do player[UID="
								+ std::to_string(it_entry->second.getUID()) + "], manda valor padrao para nao da erro no cliente.", CL_FILE_LOG_AND_CONSOLE));

						p.addZeroByte(7u);	// 1 Level, 2 Unknown, 2 size id, 2 size nickname
					}
				}

				// Player Found position
				p.addUint16((unsigned short)_fp.first);

			}else
				p.addUint8(1u); // Error
		
		}else 
			p.addUint8(1u); // Error

		packet_func::session_send(p, &_session, 1);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::sendPageFoundPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Relan�a a exception por que quem d� a resposta de error para o cliente � quem chamou essa fun��o
		throw;
	}
}

void RankRegistryManager::searchPlayerByNicknameAndSendPage(player& _session, std::string _nickname, search_dados& _sd) {
	CHECK_SESSION_BEGIN("searchPlayerByNicknameAndSendPage");

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!isLoad())
			throw exception("[RankRegistryManager::searchPlayerByNicknameAndSendPage][Error] rank registry manager not loaded, please call load function first.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

		if (_nickname.empty())
			throw exception("[RankRegistryManager::searchPlayerByNicknameAndSendPage][Error] Player[UID=" 
					+ std::to_string(_session.m_pi.uid) + "] esta procurando por um player, mas _nickname is invalid(empty). Search Rank:{\n" + _sd.toString()
					+ "\n}.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 6, 0));

		auto found_player = searchPlayerByNickname(_nickname, _sd);

		if (found_player.second == -1l)
			throw exception("[RankRegistryManager::searchPlayerByNicknameAndSendPage][Error] Player[UID=" 
					+ std::to_string(_session.m_pi.uid) + "] esta procurando por um player, mas nao encontrou o player[NICKNAME=" 
					+ _nickname + "] no rank. Search Rank:{\n" + _sd.toString() + "\n}.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 8, 0));

		// Send Page
		sendPageFoundPlayer(_session, found_player, _sd);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::searchPlayerByNicknameAndSendPage][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Relan�a que quem manda a resposta de erro para o cliente � a quem chamou essa fun��o
		throw;
	}
}

void RankRegistryManager::searchPlayerByRankAndSendPage(player& _session, uint32_t _position, search_dados& _sd) {
	CHECK_SESSION_BEGIN("searchPlayerByRankAndSendPage");

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!isLoad())
			throw exception("[RankRegistryManager::searchPlayerByRankAndSendPage][Error] rank registry manager not loaded, please call load function first.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

		auto found_player = searchPlayerByRank(_position, _sd);

		if (found_player.second == -1l)
			throw exception("[RankRegistryManager::searchPlayerByRankAndSendPage][Error] Player[UID=" 
					+ std::to_string(_session.m_pi.uid) + "] esta procurando por um player, mas nao encontrou o player[POSITION="
					+ std::to_string(_position) + "] no rank. Search Rank:{\n" + _sd.toString() + "\n}.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 8, 0));

		// Send Page
		sendPageFoundPlayer(_session, found_player, _sd);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::searchPlayerByRankAndSendPage][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Relan�a que quem manda a resposta de erro para o cliente � a quem chamou essa fun��o
		throw;
	}
}

FoundPlayer RankRegistryManager::searchPlayerByNickname(std::string _nickname, search_dados& _sd) {
	FoundPlayer ret(0u, -1);

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!isLoad())
			throw exception("[RankRegistryManager::searchPlayerByNickname][Error] rank registry manager not loaded, please call load function first.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

		if (_nickname.empty())
			throw exception("[RankRegistryManager::searchPlayerByNickname][Error] Search Rank:{\n" + _sd.toString() 
					+ "\n} _nickname is invalid(empty).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 6, 0));

		// Find player By nickname, sem Case sensitive
		auto it_player = std::find_if(m_character_entry.begin(), m_character_entry.end(), [&](auto& _el) {
			return _el.second.getNickname() != nullptr && 
#if defined(_WIN32)
				_stricmp(_el.second.getNickname(), _nickname.c_str()) == 0;
#elif defined(__linux__)
				strcasecmp(_el.second.getNickname(), _nickname.c_str()) == 0;
#endif
		});

		if (it_player == m_character_entry.end()) {
			
#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif
			
			return ret; // N�o tem o player nos registros com esse nickname
		}

		key_menu km(_sd.rank_menu, _sd.rank_menu_item);

		auto it_map = m_entry.find(km);

		if (it_map == m_entry.end() || it_map->second.empty())
			return ret;	// N�o tem nenhum registro nesse Rank Menu->Item

		auto it_entry = std::find_if(it_map->second.begin(), it_map->second.end(), [&](auto& _el) {
			return _el.second.getUID() == it_player->second.getUID();
		});

		if (it_entry == it_map->second.end()) {
			
#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif
			
			return ret;	// N�o tem registro do player no Rank Menu->item
		}

		// Calcule Page
		auto diff = std::distance(it_map->second.begin(), it_entry);

		ret.first = (it_entry->second.getCurrentPosition() - 1u) % LIMIT_REGISTRY_FOR_PAGE; // Possition relative at page
		ret.second = (int)diff / LIMIT_REGISTRY_FOR_PAGE;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::searchPlayerByNickname][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Error
		ret.second = -1l;
	}

	return ret;
}

FoundPlayer RankRegistryManager::searchPlayerByRank(uint32_t _position, search_dados& _sd) {
	FoundPlayer ret(0u, -1);

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!isLoad())
			throw exception("[RankRegistryManager::searchPlayerByRank][Error] rank registry manager not loaded, please call load function first.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

		key_menu km(_sd.rank_menu, _sd.rank_menu_item);

		auto it_map = m_entry.find(km);

		if (it_map == m_entry.end() || it_map->second.empty()) {

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif

			return ret;	// N�o tem nenhum registro nesse Rank Menu->Item
		}

		if (_position > it_map->second.size())
			throw exception("[RankRegistryManager::searchPlayerByRank][Error] Search Rank:{\n" + _sd.toString() 
					+ "\n} _position eh maior que o numeros de registros do Rank. POSITION=" 
					+ std::to_string(_position) + ", NUM_REGISTROS=" + std::to_string(it_map->second.size()), STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 7, 0));

		auto it_entry = std::find_if(it_map->second.begin(), it_map->second.end(), [&](auto& _el) {
			return _el.second.getCurrentPosition() == _position;
		});

		if (it_entry == it_map->second.end()) {

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif

			return ret;	// N�o tem registro do player no Rank Menu->item
		}

		// Calcule Page
		auto diff = std::distance(it_map->second.begin(), it_entry);

		ret.first = (it_entry->second.getCurrentPosition() - 1u) % LIMIT_REGISTRY_FOR_PAGE; // Possition relative at page
		ret.second = (int)diff / LIMIT_REGISTRY_FOR_PAGE;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::searchPlayerByRank][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Error
		ret.second = -1l;
	}

	return ret;
}

void RankRegistryManager::makeLog() {

	try {

		init_log();

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		putLog("------------------------------------------------ Player Log -----------------------------------------\n");

		for (auto& el : m_character_entry)
			putLog("Player [UID=" + std::to_string(el.second.getUID()) + ", ID=" + el.second.getId() 
					+ ", NICKNAME=" + el.second.getNickname() + ", LEVEL=" + std::to_string(el.second.getLevel()) 
					+ "] CHARACTER[TYPEID=" + std::to_string(el.second.getCharacterInfo()._typeid) 
					+ ", ID=" + std::to_string(el.second.getCharacterInfo().id) + "] equiped.");

		putLog("----------------------------------------------- Rank Log ---------------------------------------------\n");

		for (auto& el : m_entry) {

			putLog("******************************************* Rank Menu[" + std::to_string((unsigned short)el.first.m_menu) 
					+ "] Item[" + std::to_string((unsigned short)el.first.m_item) + "] **************************************\n");

			for (auto& el2 : el.second)
				putLog("Player[UID=" + std::to_string(el2.second.getUID()) 
						+ "] RANK[CURRENT=" + std::to_string(el2.second.getCurrentPosition()) 
						+ ", LAST=" + std::to_string(el2.second.getLastPosition()) 
						+ ", VALUE=" + std::to_string(el2.second.getValue()) + "]");
		}

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		close_log();

		// Log console
		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::makeLog][Log] Log dos registros criado com sucesso!", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		close_log();

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::makeLog][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void RankRegistryManager::initialize() {

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

#ifdef _DEBUG
		// Teste de tempo de carregamento
		std::clock_t begin = std::clock(), finish = 0u;

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::initialize][Log] Comecei a carregar. std::clock: " + std::to_string(begin), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		CmdRankRegistryInfo cmd_rri(true);	// Waiter

		NormalManagerDB::add(0, &cmd_rri, nullptr, nullptr);

		cmd_rri.waitEvent();

		if (cmd_rri.getException().getCodeError() != 0)
			throw cmd_rri.getException();

		m_entry = cmd_rri.getInfo();

		if (m_entry.empty())
			_smp::message_pool::getInstance().push(new message("[RankRegistryManager::initialize][Log] Nao tem nenhum registro do rank no banco de dados.", CL_FILE_LOG_AND_CONSOLE));

		CmdRankRegistryCharacterInfo cmd_rrci(true); // Waiter

		NormalManagerDB::add(0, &cmd_rrci, nullptr, nullptr);

		cmd_rrci.waitEvent();

		if (cmd_rrci.getException().getCodeError() != 0)
			throw cmd_rrci.getException();

		m_character_entry = cmd_rrci.getInfo();

		if (!m_entry.empty() && m_character_entry.empty())
			throw exception("[RankRegistryManager::initialize][Error] m_entry tem registros mas o m_rank_character_entry nao tem registros no banco de dados.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));
		else if (m_character_entry.empty())
			_smp::message_pool::getInstance().push(new message("[RankRegistryManager::initialiaze][Log] Nao tem nenhum registro do character do rank no banco de dados.", CL_FILE_LOG_AND_CONSOLE));

		// Log
		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::initialize][Log] Inicializou o Rank Registry Manager com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		// Sucesso
		m_state = true;

#ifdef _DEBUG
		// Teste de tempo de carregamento
		finish = std::clock();

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::initialize][Log] Terminei de carregar. std::clock: " 
				+ std::to_string(finish) + ", Diff: " + std::to_string((finish - begin) / (double)CLOCKS_PER_SEC), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		m_state = false;

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::initialize][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void RankRegistryManager::clear() {

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (isLoad())
			m_entry.clear();

		m_state = false;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::clear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

std::map< eRANK_OVERALL, RankRegistry > RankRegistryManager::getAllOverallInfoFromPlayer(uint32_t _uid) {

	std::map< eRANK_OVERALL, RankRegistry > v_rcr;
	
	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!isLoad())
			throw exception("[RankRegistryManager::getAllOverallInfoFromPlayer][Error] rank registry manager not loaded, please call load function first.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

		auto it_entry = m_entry.end();

		for (auto menu_item = eRANK_OVERALL::RO_TOTAL_POINTS; menu_item <= eRANK_OVERALL::RO_ACHIEVEMENT_POINTS; ENUM_OPERATOR_PLUS_PLUS(eRANK_OVERALL, menu_item)) {

			if ((it_entry = m_entry.find(key_menu(eRANK_MENU::RM_OVERALL, menu_item))) != m_entry.end()) {

				auto it = std::find_if(it_entry->second.begin(), it_entry->second.end(), [&](auto& _el) {
					return _el.second.getUID() == _uid;
				});

				if (it != it_entry->second.end())
					v_rcr.insert(std::make_pair(menu_item, it->second));
				else // N�o tem info do player no Menu->Item (Rank), add item vazio para enviar para o cliente
					v_rcr.insert(std::make_pair(menu_item, RankRegistry()));

			}else // N�o tem o Item do Menu->Item (Rank), add item vazio para enviar para o cliente
				v_rcr.insert(std::make_pair(menu_item, RankRegistry()));

		}

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::getAllOverallInfoFromPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return v_rcr;
}

std::pair< RankEntryValueRange, bool > RankRegistryManager::getPage(RankEntry::iterator _registrys, unsigned int & _page) {

	std::pair< RankEntryValueRange, bool > ret(RankEntryValueRange(), false);

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!isLoad())
			throw exception("[RankRegistryManager::getPage][Error] rank registry manager not loaded, please call load function first.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 2, 0));

		if (_registrys == m_entry.end())
			throw exception("[RankRegistryManager::getPage][Error] iterator _rigistrys is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 4, 0));

		auto num_registrys = _registrys->second.size();
		auto num_pages = NUMBER_OF_PAGE_MENU_REGISTRY(num_registrys);

		if (num_pages == 0u)
			throw exception("[RankRegistryManager::getPage][Error] Nao tem nenhum registro no RANK_SERVER[MENU=" + std::to_string((unsigned short)_registrys->first.m_menu)
					+ ", ITEM=" + std::to_string((unsigned short)_registrys->first.m_item) + ", PAGES=" + std::to_string(num_pages)
					+ ", REGISTRYS=" + std::to_string(num_registrys) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 5, 0));

		if ((_page + 1u) > num_pages) {

			_smp::message_pool::getInstance().push(new message("[RankRegistryManager::getPage][Error] A pagina requisitada eh maior do que o numero de paginas que tem no rank server. REQUEST=" 
					+ std::to_string(_page + 1) + ", RANK_SERVER[MENU=" + std::to_string((unsigned short)_registrys->first.m_menu) 
					+ ", ITEM=" + std::to_string((unsigned short)_registrys->first.m_item) + ", PAGES=" + std::to_string(num_pages) 
					+ ", REGISTRYS=" + std::to_string(num_registrys) + "], passa a ultima pagina para ele.", CL_FILE_LOG_AND_CONSOLE));

			// _page � uma referen�a para trocar aqui quando o player pedir uma p�gina que n�o tem
			_page = (uint32_t)num_pages - 1u;
		}

		auto first_el = _page * LIMIT_REGISTRY_FOR_PAGE;
		auto last_el = first_el + ((num_registrys - first_el) >= LIMIT_REGISTRY_FOR_PAGE ? LIMIT_REGISTRY_FOR_PAGE : num_registrys - first_el);

		RankEntryValueRange revr{ _registrys->second.begin(), _registrys->second.begin() };

		std::advance(revr.first, first_el);
		std::advance(revr.second, last_el);

		ret.first = revr;
		ret.second = true;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::getPage][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		ret.second = false;
	}

	return ret;
}

void RankRegistryManager::init_log() {

	try {

		try {

			// Dir do arquivo .ini ou padr�o se n�o tiver
			ReaderIni ini(_INI_PATH);

			ini.init();

			std::string tmp_dir = ini.readString("LOG", "DIR");

			if (!tmp_dir.empty()) {

				auto status = std::filesystem::status(tmp_dir);

				if (std::filesystem::exists(status)) {

					dir = tmp_dir;	// O diret�rio do Arquivo .ini existe, usa esse diret�rio

				}else if (
#if defined(_WIN32)
					_mkdir(tmp_dir.c_str()) == 0/*Criou o direit�rio com sucesso*/
#elif defined(__linux__)
					mkdir(tmp_dir.c_str(), MODE_DIR_LOG) == 0/*Criou o direit�rio com sucesso*/
#endif
				)	// Tenta cria o diret�rio que est� no Arquivo .ini, se conseguir usa esse diret�rio criado
					dir = tmp_dir;
				else
					throw exception("[RankRegistryManager::init_log][Error] Nao conseguiu criar o diretorio[" + tmp_dir 
							+ "] do Arquivo .ini.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 5000, 0));
			
			}else
				throw exception("[RankRegistryManager::init_log][Error] O diretorio do Arquivo .ini esta vazio.", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_REGISTRY_MANAGER, 5001, 0));

		}catch (exception& e) {
			
			_smp::message_pool::getInstance().push(new message("[RankRegistryManager::init_log][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			// Log
			_smp::message_pool::getInstance().push(new message("[RankRegistryManager::init_log][Log] Nao consguiu pegar o diretorio do Arquivo .ini, usando o diretorio padrao \"Log\".", CL_FILE_LOG_AND_CONSOLE));

			// N�o tem o log dir do arquivo ini, usa o padr�o
			// Verifica se diret�rio padr�o est� criado, se n�o cria ele
			dir = "Log";

			auto s = std::filesystem::status(dir);

			if (!std::filesystem::exists(s) && 
#if defined(_WIN32)
				_mkdir(dir.c_str()) != 0
#elif defined(__linux__)
				mkdir(dir.c_str(), MODE_DIR_LOG) != 0
#endif
			)
				_smp::message_pool::getInstance().push(new message("[RankRegistryManager::init_log][Error] Nao conseguiu criar o diretorio[" + dir + "] padrao.", CL_FILE_LOG_AND_CONSOLE));
		}

		close_log();

		std::string datetime = "";
		
		datetime.resize(255);

		tm ti{ 0 };
		time_t day_time = 0u;

		// Atualiza o Day Time do Log para o atual e guarda ele na vari�vel
		std::time(&day_time);

#if defined(_WIN32)
		if (localtime_s(&ti, &day_time) == 0)
			sprintf_s(&datetime[0], datetime.size(), "%.02u%.02u%.04u%.02u%.02u%.02u", ti.tm_mday, ti.tm_mon + 1, ti.tm_year + 1900, ti.tm_hour, ti.tm_min, ti.tm_sec);
#elif defined(__linux__)
		if (localtime_r(&day_time, &ti) != nullptr)
			sprintf(&datetime[0], "%.02u%.02u%.04u%.02u%.02u%.02u", ti.tm_mday, ti.tm_mon + 1, ti.tm_year + 1900, ti.tm_hour, ti.tm_min, ti.tm_sec);
#endif

		datetime.resize(14);

		if (!prex.empty())
			datetime += " " + prex;

		std::string full_path = "";

		try {

			// Log
			full_path = dir + _PATH_SEPARETOR + "log Registros " + datetime + ".log";

			if (!log.is_open())
				log.open(full_path, std::ofstream::app);

		}catch (std::ofstream::failure& e) {
		
			_smp::message_pool::getInstance().push(new message("[RankRegistryManager::init_log][Error][" + std::string(e.what()) + " Code: "
					+ std::to_string(e.code().value()) + "] nao conseguiu abrir o arquivo='" + full_path + "'", CL_FILE_LOG_AND_CONSOLE));
		}
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::init_log][ErrorSystem] + " + std::string(e.getFullMessageError()), CL_FILE_LOG_AND_CONSOLE));
			
	}
}

void RankRegistryManager::close_log() {

	try {

		try {
			
			if (log.is_open())
				log.close();

		}catch (std::ofstream::failure& e) {

			_smp::message_pool::getInstance().push(new message("[RankRegistryManager::close_log][Error][" 
					+ std::string(e.what()) + " Code: " + std::to_string(e.code().value()) + "].", CL_FILE_LOG_AND_CONSOLE));
		}
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::close_log][ErrorSystem] + " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

inline void RankRegistryManager::putLog(std::string _str_log) {

	try {

		if (!log.is_open())
			init_log();

		if (log.good())
			log << _str_log << std::endl;

	}catch (std::ofstream::failure& e) {

		_smp::message_pool::getInstance().push(new message("[RankRegistryManager::putLog][Error][" + std::string(e.what()) + " Code: "
				+ std::to_string(e.code().value()) + "] Erro no arquivo de log.", CL_FILE_LOG_AND_CONSOLE));
	}
}
