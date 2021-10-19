// Arquivo cmd_insert_command.cpp
// Criado em 04/12/2018 as 03:56 por Acrisio
// Implementa��o da classe CmdInsertCommand

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_command.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdInsertCommand::CmdInsertCommand(bool _waiter) : pangya_db(_waiter), m_ci{0} {
}

CmdInsertCommand::CmdInsertCommand(CommandInfo& _ci, bool _waiter)
	: pangya_db(_waiter), m_ci(_ci) {
}

CmdInsertCommand::~CmdInsertCommand() {
}

void CmdInsertCommand::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um INSERT
	return;
}

response* CmdInsertCommand::prepareConsulta(database& _db) {

	std::string reserveDate = "null";

	if (m_ci.reserveDate > 0)
		reserveDate = _db.makeText(formatDateLocal(m_ci.reserveDate));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_ci.id) + ", " + std::to_string(m_ci.arg[0])
			+ ", " + std::to_string(m_ci.arg[1]) + ", " + std::to_string(m_ci.arg[2]) + ", " + std::to_string(m_ci.arg[3])
			+ ", " + std::to_string(m_ci.arg[4]) + ", " + std::to_string(m_ci.target) + ", " + std::to_string(m_ci.flag)
			+ ", " + std::to_string((unsigned short)m_ci.valid) + ", " + reserveDate
	);

	checkResponse(r, "nao conseguiu adicionar o Comando[" + m_ci.toString() + "]");

	return r;
}

CommandInfo& CmdInsertCommand::getInfo() {
	return m_ci;
}

void CmdInsertCommand::setInfo(CommandInfo& _ci) {
	m_ci = _ci;
}
