// Arquivo cmd_mail_box_info2.cpp
// Criado em 13/01/2021 as 12:02 por Acrisio
// Implementa��o da classe CmdMailBoxInfo2

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_mail_box_info2.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include <cstring>

using namespace stdA;

CmdMailBoxInfo2::CmdMailBoxInfo2(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid) {
}

CmdMailBoxInfo2::CmdMailBoxInfo2(bool _waiter) : pangya_db(_waiter), m_uid(0u) {
}

CmdMailBoxInfo2::~CmdMailBoxInfo2() {

	if (!m_emails.empty())
		m_emails.clear();
}

void CmdMailBoxInfo2::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(17, (uint32_t)_result->cols);

	int32_t id = IFNULL(atoi, _result->data[0]);

	auto it_email = m_emails.find(id);

	if (it_email != m_emails.end()) {

		// J� tem, add s� o item
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
			it_email->second.itens.push_back(item);
		}
	
	}else {

		// N�o tem, cria um novo
		EmailInfoEx email{ 0u };

		email.id = id;

		if (is_valid_c_string(_result->data[1]))
			STRCPY_TO_MEMORY_FIXED_SIZE(email.from_id, sizeof(email.from_id), _result->data[1]);
		if (is_valid_c_string(_result->data[2]))
			STRCPY_TO_MEMORY_FIXED_SIZE(email.gift_date, sizeof(email.gift_date), _result->data[2]);
		
		if (is_valid_c_string(_result->data[3])) {
			
			try {

				// Translate Msg From Encoded Char not printed
				auto _msg = verifyAndTranslate(_result->data[3], 2/*fixed size*/);
			
				// !@ possivel erro de viola��o de acesso
				if (_msg.empty())
#if defined(_WIN32)
					memcpy_s(email.msg, sizeof(email.msg), _result->data[3], (strlen(_result->data[3]) > sizeof(email.msg) ? sizeof(email.msg) : strlen(_result->data[3])));
#elif defined(__linux__)
					memcpy(email.msg, _result->data[3], (strlen(_result->data[3]) > sizeof(email.msg) ? sizeof(email.msg) : strlen(_result->data[3])));
#endif
				else
#if defined(_WIN32)
					memcpy_s(email.msg, sizeof(email.msg), _msg.data(), (_msg.length() > sizeof(email.msg) ? sizeof(email.msg) : _msg.length()));
#elif defined(__linux__)
					memcpy(email.msg, _msg.data(), (_msg.length() > sizeof(email.msg) ? sizeof(email.msg) : _msg.length()));
#endif
			
			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[CmdMailBoxInfo2::lineResult][ErrorSystem][Teste com o try para nao sair do cmd db] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}
		}

		email.visit_count = IFNULL(atoi, _result->data[4]);

		email.lida_yn = (unsigned char)IFNULL(atoi, _result->data[5]);

		// add o item
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
			email.itens.push_back(item);
		}

		// Add Email
		m_emails.insert({ email.id, email });
	}
}

response* CmdMailBoxInfo2::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdMailBoxInfo2::prepareConsulta][Error] m_uid is invalid(0)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (!m_emails.empty())
		m_emails.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar todos os emails do mail box do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdMailBoxInfo2::getUID() {
	return m_uid;
}

void CmdMailBoxInfo2::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::map< int32_t, EmailInfoEx >& CmdMailBoxInfo2::getInfo() {
	return m_emails;
}
