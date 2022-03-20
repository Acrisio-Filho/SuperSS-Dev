// Arquivo cmd_email_info2.cpp
// Criado em 14/01/2021 as 06:27 por Acrisio
// Implementa��o da classe CmdEmailInfo2

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_email_info2.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include <cstring>

using namespace stdA;

CmdEmailInfo2::CmdEmailInfo2(uint32_t _uid, int32_t _email_id, bool _waiter) 
	: pangya_db(_waiter), m_uid(_uid), m_email_id(_email_id) {
}

CmdEmailInfo2::CmdEmailInfo2(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_email_id(-1l) {
}

CmdEmailInfo2::~CmdEmailInfo2() {
}

void CmdEmailInfo2::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(17, (uint32_t)_result->cols);

	if (m_ei.id <= 0l) {

		m_ei.id = IFNULL(atoi, _result->data[0]);

		if (is_valid_c_string(_result->data[1]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_ei.from_id, sizeof(m_ei.from_id), _result->data[1]);
		if (is_valid_c_string(_result->data[2]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_ei.gift_date, sizeof(m_ei.gift_date), _result->data[2]);
		
		if (is_valid_c_string(_result->data[3])) {
			
			try {

				// Translate Msg From Encoded Char not printed
				auto _msg = verifyAndTranslate(_result->data[3], 2/*fixed size*/);
			
				// !@ possivel erro de viola��o de acesso
				if (_msg.empty())
#if defined(_WIN32)
					memcpy_s(m_ei.msg, sizeof(m_ei.msg), _result->data[3], (strlen(_result->data[3]) > sizeof(m_ei.msg) ? sizeof(m_ei.msg) : strlen(_result->data[3])));
#elif defined(__linux__)
					memcpy(m_ei.msg, _result->data[3], (strlen(_result->data[3]) > sizeof(m_ei.msg) ? sizeof(m_ei.msg) : strlen(_result->data[3])));
#endif
				else
#if defined(_WIN32)
					memcpy_s(m_ei.msg, sizeof(m_ei.msg), _msg.data(), (_msg.length() > sizeof(m_ei.msg) ? sizeof(m_ei.msg) : _msg.length()));
#elif defined(__linux__)
					memcpy(m_ei.msg, _msg.data(), (_msg.length() > sizeof(m_ei.msg) ? sizeof(m_ei.msg) : _msg.length()));
#endif
			
			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[CmdEmailInfo2::lineResult][ErrorSystem][Teste com o try para nao sair do cmd db] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}
		}

		m_ei.visit_count = IFNULL(atoi, _result->data[4]);

		m_ei.lida_yn = (unsigned char)IFNULL(atoi, _result->data[5]);

	}

	// Add o item
	EmailInfo::item item{ 0 };

	item._typeid = IFNULL(atoi, _result->data[7]);

	if (item._typeid != 0) {

		item.id = IFNULL(atoi, _result->data[6]); // ID vem antes de typeid na consulta

		item.flag_time = (unsigned char)IFNULL(atoi, _result->data[8]);
		item.qntd = IFNULL(atoi, _result->data[9]);
		item.tempo_qntd = IFNULL(atoi, _result->data[10]);
		item.pang = IFNULL(atoll, _result->data[11]);
		item.cookie = IFNULL(atoll, _result->data[12]);
		item.gm_id = IFNULL(atoi, _result->data[13]);
		item.flag_gift = IFNULL(atoi, _result->data[14]);
		if (is_valid_c_string(_result->data[15]))
			STRCPY_TO_MEMORY_FIXED_SIZE(item.ucc_img_mark, sizeof(item.ucc_img_mark), _result->data[15]);
		item.type = (short)IFNULL(atoi, _result->data[16]);

		// Add Item
		m_ei.itens.push_back(item);
	}

	if (m_ei.id != m_email_id)
		throw exception("[CmdEmailInfo2::lineResult][Error] o email info retornado nao e igual ao requisitado. req id: " 
				+ std::to_string(m_email_id) + " != " + std::to_string(m_ei.id), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdEmailInfo2::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdEmailInfo2::prepareConsulta][Error] m_uid is invalid(0).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_email_id <= 0l)
		throw exception("[CmdEmailInfo2::prepareConsulta][Error] m_email_id is invalid(" 
				+ std::to_string(m_email_id) + ").", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_ei.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_email_id));

	checkResponse(r, "nao conseguiu pegar o Email[ID=" + std::to_string(m_email_id) + "] information do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdEmailInfo2::getUID() {
	return m_uid;
}

void CmdEmailInfo2::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdEmailInfo2::getEmailId() {
	return m_email_id;
}

void CmdEmailInfo2::setEmailId(int32_t _email_id) {
	m_email_id = _email_id;
}

EmailInfoEx& CmdEmailInfo2::getInfo() {
	return m_ei;
}
