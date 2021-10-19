// Arquivo cmd_update_command.cpp
// Criado em 03/12/2018 as 15:51 por Acrisio
// Implementa��o da classe CmdUpdateCommand

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_command.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdUpdateCommand::CmdUpdateCommand(bool _waiter) : pangya_db(_waiter), m_ci{0} {
}

CmdUpdateCommand::CmdUpdateCommand(CommandInfo& _ci, bool _waiter) : pangya_db(_waiter), m_ci(_ci) {
}

CmdUpdateCommand::~CmdUpdateCommand() {
}

void CmdUpdateCommand::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCommand::prepareConsulta(database& _db) {

	if (m_ci.idx == 0u)
		throw exception("[CmdUpdateCommand::prepareConsulta][Error] (CommandInfo)m_ci.idx is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	std::string reserveDate = "null";

	if (m_ci.reserveDate > 0)
		reserveDate = _db.makeText(formatDateLocal(m_ci.reserveDate));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_ci.idx) + ", " + std::to_string(m_ci.id) + ", " + std::to_string(m_ci.arg[0])
			+ ", " + std::to_string(m_ci.arg[1]) + ", " + std::to_string(m_ci.arg[2]) + ", " + std::to_string(m_ci.arg[3])
			+ ", " + std::to_string(m_ci.arg[4]) + ", " + std::to_string(m_ci.target) + ", " + std::to_string(m_ci.flag)
			+ ", " + std::to_string((unsigned short)m_ci.valid) + ", " + reserveDate
	);

	checkResponse(r, "nao conseguiu Atualizar o Command[" + m_ci.toString() + "]");

	return r;
}

CommandInfo& CmdUpdateCommand::getInfo() {
	return m_ci;
}

void CmdUpdateCommand::setInfo(CommandInfo& _ci) {
	m_ci = _ci;
}
