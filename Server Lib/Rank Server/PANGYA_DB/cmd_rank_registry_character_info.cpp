// Arquivo cmd_rank_registry_character_info.cpp
// Criado em 16/06/2020 as 18:16 por Acrisio
// Implementa��o da classe CmdRankRegistryCharacterInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "cmd_rank_registry_character_info.hpp"

using namespace stdA;

CmdRankRegistryCharacterInfo::CmdRankRegistryCharacterInfo(bool _waiter) : pangya_db(_waiter) {
}

CmdRankRegistryCharacterInfo::~CmdRankRegistryCharacterInfo() {

	if (!m_entry.empty())
		m_entry.clear();
}

void CmdRankRegistryCharacterInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(85, (uint32_t)_result->cols);

	RankCharacter rc;

	CharacterInfo ce{ 0 };
	auto i = 0;

	rc.setUID(IFNULL(atoi, _result->data[0]));

	if (_result->data[1] != nullptr)
		rc.setId(_result->data[1]);

	if (_result->data[2] != nullptr)
		rc.setNickname(_result->data[2]);

	rc.setLevel((unsigned short)IFNULL(atoi, _result->data[3]));

	// Initialize Character Info
	ce.id = IFNULL(atoi, _result->data[4]);
	ce._typeid = IFNULL(atoi, _result->data[5]);
	for (i = 0; i < 24; i++)
		ce.parts_id[i] = IFNULL(atoi, _result->data[6 + i]);		// 6 + 24
	for (i = 0; i < 24; i++)
		ce.parts_typeid[i] = IFNULL(atoi, _result->data[30 + i]);	// 30 + 24
	ce.default_hair = (unsigned char)IFNULL(atoi, _result->data[54]);
	ce.default_shirts = (unsigned char)IFNULL(atoi, _result->data[55]);
	ce.gift_flag = (unsigned char)IFNULL(atoi, _result->data[56]);
	for (i = 0; i < 5; i++)
		ce.pcl[i] = (unsigned char)IFNULL(atoi, _result->data[57 + i]);	// 57 + 5
	ce.purchase = (unsigned char)IFNULL(atoi, _result->data[62]);
	for (i = 0; i < 5; i++)
		ce.auxparts[i] = IFNULL(atoi, _result->data[63 + i]);				// 63 + 5
	for (i = 0; i < 4; i++)
		ce.cut_in[i] = IFNULL(atoi, _result->data[68 + i]);					// 68 + 4 Cut-in deveria guarda no db os outros 3 se for msm os 4 que penso q seja, � sim no JP USA os 4
	ce.mastery = IFNULL(atoi, _result->data[72]);
	for (i = 0; i < 4; i++)
		ce.card_character[i] = IFNULL(atoi, _result->data[73 + i]);		// 73 + 4
	for (i = 0; i < 4; i++)
		ce.card_caddie[i] = IFNULL(atoi, _result->data[77 + i]);			// 77 + 4
	for (i = 0; i < 4; i++)
		ce.card_NPC[i] = IFNULL(atoi, _result->data[81 + i]);				// 81 + 4

	// Inject Character Info into RankCharacter
	rc.setCharacterInfo(ce);

	auto it_map = m_entry.find(rc.getUID());

	if (it_map != m_entry.end()) {

		// J� tem um character, troca pelo ultimo
		_smp::message_pool::getInstance().push(new message("[CmdReankRegistryCharacterInfo::lineResult][WARNING] Player[UID=" + std::to_string(rc.getUID()) 
				+ "] CHARACTER_ANT[TYPEID=" + std::to_string(it_map->second.getCharacterInfo()._typeid) 
				+ ", ID=" + std::to_string(it_map->second.getCharacterInfo().id) + "] CHARACTER_REPLACE[TYPEID=" + std::to_string(ce._typeid) + ", ID=" 
				+ std::to_string(ce.id) + "] tem mais de um character equipado nos registro do rank no banco de dados. Trocando o character antigo pelo novo.", CL_FILE_LOG_AND_CONSOLE));

		it_map->second = rc;
	
	}else {

		// N�o tem no map cria um novo
		auto ret = m_entry.insert(std::make_pair(rc.getUID(), rc));

		// Verifica se inseriu o valor direito no  map
		if (!ret.second) {

			if (ret.first != m_entry.end() && ret.first->first == rc.getUID()) {

				if (ret.first->second.getCharacterInfo()._typeid != ce._typeid) {

					// Troca o character info antigo pelo o novo

					try {

						// Pega o antigo character registro para usar no Log
						auto rc_ant = m_entry.at(rc.getUID());

						// Novo Character Info registro
						m_entry.at(rc.getUID()) = rc;

						// Log
						_smp::message_pool::getInstance().push(new message("[CmdRankRegistryCharacterInfo::lineResult][Log] Player[UID=" + std::to_string(rc.getUID()) 
								+ "] Atualizou o character registro do character rank map. CHARACTER_ANT[TYPEID=" + std::to_string(rc_ant.getCharacterInfo()._typeid) 
								+ ", ID=" + std::to_string(rc_ant.getCharacterInfo().id) + "], CHARACTER_NEW[TYPEID=" + std::to_string(ce._typeid) 
								+ ", ID=" + std::to_string(ce.id) + "].", CL_FILE_LOG_AND_CONSOLE));

					}catch (std::out_of_range& e) {
						UNREFERENCED_PARAMETER(e);

						_smp::message_pool::getInstance().push(new message("[CmdRankRegistryCharacterInfo::lineResult][Error][WARNING] nao conseguiu atualizar o character registro do player[UID=" 
								+ std::to_string(rc.getUID()) + "] no character rank map. CHARACTER_ANT[TYPEID=" + std::to_string(ret.first->second.getCharacterInfo()._typeid) 
								+ ", ID=" + std::to_string(ret.first->second.getCharacterInfo().id) + "], CHARACTER_NEW[TYPEID=" 
								+ std::to_string(ce._typeid) + ", ID=" + std::to_string(ce.id) + "].", CL_FILE_LOG_AND_CONSOLE));
					}

				}else
					_smp::message_pool::getInstance().push(new message("[CmdRankRegistryCharacterInfo::lineResult][Error] nao conseguiu adicionar o novo chararacter registro do player[UID=" 
							+ std::to_string(rc.getUID()) + "] no character rank map, por que ele ja tem o mesmo character[TYPEID=" 
							+ std::to_string(ret.first->second.getCharacterInfo()._typeid) + ", ID=" + std::to_string(ret.first->second.getCharacterInfo().id) + "] no map.", CL_FILE_LOG_AND_CONSOLE));

			}else
				_smp::message_pool::getInstance().push(new message("[CmdRankRegistryCharacterInfo::lineResult][Error] nao conseguiu adicionar o novo character registro do player[UID=" 
						+ std::to_string(rc.getUID()) + "] no character rank map.", CL_FILE_LOG_AND_CONSOLE));
		}
	}
}

response* CmdRankRegistryCharacterInfo::prepareConsulta(database& _db) {
	
	if (!m_entry.empty())
		m_entry.clear();

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "Nao conseguiu pegar os registro de characters do Rank");

	return r;
}

RankCharacterEntry& CmdRankRegistryCharacterInfo::getInfo() {
	return m_entry;
}
