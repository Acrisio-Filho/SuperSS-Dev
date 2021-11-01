// Arquivo cmd_msg_off_info.cpp
// Criado em 24/03/2018 as 17:59 por Acrisio
// Implementa��o da classe CmdMsgOffInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_msg_off_info.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include <cstring>

using namespace stdA;

CmdMsgOffInfo::CmdMsgOffInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), v_moi() {
}

CmdMsgOffInfo::CmdMsgOffInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), v_moi() {
}

CmdMsgOffInfo::~CmdMsgOffInfo() {
}

void CmdMsgOffInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(5, (uint32_t)_result->cols);

	MsgOffInfo moi{ 0 };

	moi.id = (short)IFNULL(atoi, _result->data[0]);
	moi.from_uid = IFNULL(atoi, _result->data[1]);
	if (_result->data[2] != nullptr)
#if defined(_WIN32)
		memcpy_s(moi.nick, sizeof(moi.nick), _result->data[2], sizeof(moi.nick));
#elif defined(__linux__)
		memcpy(moi.nick, _result->data[2], sizeof(moi.nick));
#endif

	if (_result->data[3] != nullptr) {

		try {

			// Translate Msg From Encoded Char not printed
			auto msg = verifyAndTranslate(_result->data[3], 2/*fixed size*/);

			// !@ possivel erro de viola��o de acesso
			if (msg.empty())
#if defined(_WIN32)
				memcpy_s(moi.msg, sizeof(moi.msg), _result->data[3], (strlen(_result->data[3]) > sizeof(moi.msg) ? sizeof(moi.msg) : strlen(_result->data[3])));
#elif defined(__linux__)
				memcpy(moi.msg, _result->data[3], (strlen(_result->data[3]) > sizeof(moi.msg) ? sizeof(moi.msg) : strlen(_result->data[3])));
#endif
			else
#if defined(_WIN32)
				memcpy_s(moi.msg, sizeof(moi.msg), msg.data(), (msg.length() > sizeof(moi.msg) ? sizeof(moi.msg) : msg.length()));
#elif defined(__linux__)
				memcpy(moi.msg, msg.data(), (msg.length() > sizeof(moi.msg) ? sizeof(moi.msg) : msg.length()));
#endif

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[CmdMsgOffInfo::lineResult][ErrorSystem][Teste com o try para nao sair do cmd db] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}
	
	if (_result->data[4] != nullptr)
#if defined(_WIN32)
		memcpy_s(moi.date, sizeof(moi.date), _result->data[4], sizeof(moi.date) - 1);
#elif defined(__linux__)
		memcpy(moi.date, _result->data[4], sizeof(moi.date) - 1);
#endif

	v_moi.push_back(moi);
}

response* CmdMsgOffInfo::prepareConsulta(database& _db) {

	v_moi.clear();
	v_moi.shrink_to_fit();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar msg off info do player: " + std::to_string(m_uid));

	return r;
}

std::vector< MsgOffInfo >& CmdMsgOffInfo::getInfo() {
	return v_moi;
}

uint32_t CmdMsgOffInfo::getUID() {
	return m_uid;
}

void CmdMsgOffInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
