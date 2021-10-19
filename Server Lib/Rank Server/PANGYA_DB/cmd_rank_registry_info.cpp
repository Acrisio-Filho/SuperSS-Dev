// Arquivo cmd_rank_registry_info.cpp
// Criado em 16/06/2020 as 15:38 por Acrisio
// Implementa��o da classe CmdRankRegistryInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "cmd_rank_registry_info.hpp"

using namespace stdA;

CmdRankRegistryInfo::CmdRankRegistryInfo(bool _waiter) : pangya_db(_waiter), m_entry() {
}

CmdRankRegistryInfo::~CmdRankRegistryInfo() {

	if (!m_entry.empty())
		m_entry.clear();
}

void CmdRankRegistryInfo::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

	checkColumnNumber(6, (uint32_t)_result->cols);

	RankRegistry rr;

	rr.setUID(IFNULL(atoi, _result->data[0]));
	rr.setCurrentPosition(IFNULL(atoi, _result->data[1]));
	rr.setLastPosition(IFNULL(atoi, _result->data[2]));
	rr.setValue(IFNULL(atoi, _result->data[3]));

	key_menu km(eRANK_MENU(IFNULL(atoi, _result->data[4])), (unsigned char)IFNULL(atoi, _result->data[5]));

	auto it_map = m_entry.find(km);

	if (it_map != m_entry.end()) { // J� tem essa Menu->Item

		key_position kp(rr.getUID(), rr.getCurrentPosition());

		auto ret = it_map->second.insert(std::make_pair(kp, rr));

		// Verifica se inseriu o valor direito no  map
		if (!ret.second) {

			if (ret.first != it_map->second.end() && ret.first->first == kp) {

				if (ret.first->second.getUID() != rr.getUID() || ret.first->second.getCurrentPosition() != rr.getCurrentPosition()) {

					// Troca o rank registro antigo pelo o novo

					try {

						// Pega o antigo registro rank para usar no Log
						auto rr_ant = it_map->second.at(kp);

						// Novo registro rank
						it_map->second.at(kp) = rr;

						// Log
						_smp::message_pool::getInstance().push(new message("[CmdRankRegistryInfo::lineResult][Log] Player[UID=" + std::to_string(rr.getUID()) 
								+ "] Atualizou o registro no rank registry map. REGISTRY_ANT[UID=" + std::to_string(rr_ant.getUID()) 
								+ ", CURRENT_POSITION=" + std::to_string(rr_ant.getCurrentPosition()) + "], REGISTRY_NEW[UID=" + std::to_string(rr.getUID()) 
								+ ", CURRENT_POSITION=" + std::to_string(rr.getCurrentPosition()) + "].", CL_FILE_LOG_AND_CONSOLE));

					}catch (std::out_of_range& e) {
						UNREFERENCED_PARAMETER(e);

						_smp::message_pool::getInstance().push(new message("[CmdRankRegistryInfo::lineResult][Error][WARNING] nao conseguiu atualizar o rank registro do player[UID=" 
								+ std::to_string(rr.getUID()) + "] no registro rank map. REGISTERY_ANT[UID=" + std::to_string(ret.first->second.getUID()) 
								+ ", CURRENT_POSITION=" + std::to_string(ret.first->second.getCurrentPosition()) + "], REGISTRY_NEW[UID=" 
								+ std::to_string(rr.getUID()) + ", CURRENT_POSITION=" + std::to_string(rr.getCurrentPosition()) + "].", CL_FILE_LOG_AND_CONSOLE));
					}

				}else
					_smp::message_pool::getInstance().push(new message("[CmdRankRegistryInfo::lineResult][Error] nao conseguiu adicionar o novo rank registro do player[UID=" 
							+ std::to_string(rr.getUID()) + "] no registro rank map, por que ele ja tem o mesmo registro[UID=" 
							+ std::to_string(ret.first->second.getUID()) + ", CURRENT_POSITION=" + std::to_string(ret.first->second.getCurrentPosition()) + "] no map.", CL_FILE_LOG_AND_CONSOLE));

			}else
				_smp::message_pool::getInstance().push(new message("[CmdRankRegistryInfo::lineResult][Error] nao conseguiu adicionar o novo rank registro do player[UID=" 
						+ std::to_string(rr.getUID()) + "] no registro rank map.", CL_FILE_LOG_AND_CONSOLE));
		}

	}else {

		key_position kp(rr.getUID(), rr.getCurrentPosition());

		// N�o tem cria um novo
		auto ret = m_entry.insert(std::make_pair(km, RankEntryValue{ std::make_pair(kp, rr) }));

		// Verifica se inseriu o valor direito no  map
		if (!ret.second) {

			if (ret.first != m_entry.end() && ret.first->first == km) {

				if (!ret.first->second.empty() && ret.first->second.begin()->first != kp) {

					// Troca o Menu rank registro antigo pelo o novo

					try {

						// Pega o antigo Menu rank registro para usar no Log
						auto rm_ant = ret.first->second.begin()->first;

						// Novo Menu rank registro
						ret.first->second.clear();

						ret.first->second[kp] = rr;

						// Log
						_smp::message_pool::getInstance().push(new message("[CmdRankRegistryInfo::lineResult][Log] Player[UID=" + std::to_string(rr.getUID()) 
								+ "] Atualizou o Menu rank registro no registry map. MENU_REGISTRY_ANT[UID=" + std::to_string(rm_ant.m_uid) 
								+ ", CURRENT_POSITION=" + std::to_string(rm_ant.m_position) + "], MENU_REGISTRY_NEW[UID=" + std::to_string(rr.getUID()) 
								+ ", CURRENT_POSITION=" + std::to_string(rr.getCurrentPosition()) + "].", CL_FILE_LOG_AND_CONSOLE));

					}catch (std::out_of_range& e) {
						UNREFERENCED_PARAMETER(e);

						_smp::message_pool::getInstance().push(new message("[CmdRankRegistryInfo::lineResult][Error][WARNING] nao conseguiu atualizar o Menu rank registro do player[UID=" 
								+ std::to_string(rr.getUID()) + "] no registro rank map. MENU_REGISTERY_ANT[UID=" + std::to_string(ret.first->second.begin()->first.m_uid) 
								+ ", CURRENT_POSITION=" + std::to_string(ret.first->second.begin()->first.m_position) + "], MENU_REGISTRY_NEW[UID=" 
								+ std::to_string(rr.getUID()) + ", CURRENT_POSITION=" + std::to_string(rr.getCurrentPosition()) + "].", CL_FILE_LOG_AND_CONSOLE));
					}

				}else
					_smp::message_pool::getInstance().push(new message("[CmdRankRegistryInfo::lineResult][Error] nao conseguiu adicionar o novo Menu rank registro do player[UID=" 
							+ std::to_string(rr.getUID()) + "] no registro rank map, por que ele ja tem o mesmo menu_registro[UID=" 
							+ std::to_string(rr.getUID()) + ", CURRENT_POSITION=" + std::to_string(rr.getCurrentPosition()) + "] no map.", CL_FILE_LOG_AND_CONSOLE));

			}else
				_smp::message_pool::getInstance().push(new message("[CmdRankRegistryInfo::lineResult][Error] nao conseguiu adicionar o novo Menu rank registro do player[UID=" 
						+ std::to_string(rr.getUID()) + "] no registro rank map.", CL_FILE_LOG_AND_CONSOLE));
		}
	}
}

response* CmdRankRegistryInfo::prepareConsulta(database& _db) {
	
	if (!m_entry.empty())
		m_entry.clear();

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "Nao conseguiu pegar os Entry do Rank");

	return r;
}

RankEntry& CmdRankRegistryInfo::getInfo() {
	return m_entry;
}
