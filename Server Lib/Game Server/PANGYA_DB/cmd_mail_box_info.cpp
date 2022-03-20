// Arquivo cmd_mail_box_info.cpp
// Criado em 24/03/2018 as 14:49 por Acrisio
// Implementa��o da classe CmdMailBoxInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_mail_box_info.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include <cstring>

using namespace stdA;

CmdMailBoxInfo::CmdMailBoxInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_type(NORMAL), m_page(0u), m_total_page(0u), v_mb() {
}

CmdMailBoxInfo::CmdMailBoxInfo(uint32_t _uid, TYPE _type, uint32_t _page, bool _waiter) 
		: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_page(_page), m_total_page(0u), v_mb() {
}

CmdMailBoxInfo::~CmdMailBoxInfo() {
}

void CmdMailBoxInfo::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

	if (_index_result == 0) {

		checkColumnNumber(18, (uint32_t)_result->cols);

		MailBox mb{ 0 };

		mb.id = IFNULL(atoi, _result->data[0]);
		if (is_valid_c_string(_result->data[1]))
			STRCPY_TO_MEMORY_FIXED_SIZE(mb.from_id, sizeof(mb.from_id), _result->data[1]);
	
		if (is_valid_c_string(_result->data[2])) {

			try {

				// Translate Msg From Encoded Char not printed
				auto _msg = verifyAndTranslate(_result->data[2], 2/*fixed size*/);

				// !@ Provav�l erro de vila��o de acesso
				if (_msg.empty())
#if defined(_WIN32)
					memcpy_s(mb.msg, sizeof(mb.msg), _result->data[2], (strlen(_result->data[2]) > sizeof(mb.msg) ? sizeof(mb.msg) : strlen(_result->data[2])));
#elif defined(__linux__)
					memcpy(mb.msg, _result->data[2], (strlen(_result->data[2]) > sizeof(mb.msg) ? sizeof(mb.msg) : strlen(_result->data[2])));
#endif
				else
#if defined(_WIN32)
					memcpy_s(mb.msg, sizeof(mb.msg), _msg.data(), (_msg.length() > sizeof(mb.msg) ? sizeof(mb.msg) : _msg.length()));
#elif defined(__linux__)
					memcpy(mb.msg, _msg.data(), (_msg.length() > sizeof(mb.msg) ? sizeof(mb.msg) : _msg.length()));
#endif
		
			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[CmdMailBoxInfo::lineResult][ErrorSystem][Teste com o try para nao sair do cmd db] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}
		}
	
		mb.visit_count = IFNULL(atoi, _result->data[4]);
		mb.lida_yn = (unsigned char)IFNULL(atoi, _result->data[5]);
		mb.item_num = IFNULL(atoi, _result->data[6]);
		mb.item.id = IFNULL(atoi, _result->data[7]);
		mb.item._typeid = IFNULL(atoi, _result->data[8]);
		mb.item.flag_time = (unsigned char)IFNULL(atoi, _result->data[9]);
		mb.item.qntd = IFNULL(atoi, _result->data[10]);
		mb.item.tempo_qntd = IFNULL(atoi, _result->data[11]);
		mb.item.pang = IFNULL(atoll, _result->data[12]);
		mb.item.cookie = IFNULL(atoll, _result->data[13]);
		mb.item.gm_id = IFNULL(atoi, _result->data[14]);
		mb.item.flag_gift = IFNULL(atoi, _result->data[15]);
		if (is_valid_c_string(_result->data[16]))
			STRCPY_TO_MEMORY_FIXED_SIZE(mb.item.ucc_img_mark, sizeof(mb.item.ucc_img_mark), _result->data[16]);
		mb.item.type = (short)IFNULL(atoi, _result->data[17]);

		v_mb.push_back(mb);
	
	}else if (_index_result == 1) {

		checkColumnNumber(1, (uint32_t)_result->cols);

		m_total_page = IFNULL(atoi, _result->data[0]);

		// Calcule page from emails
		m_total_page = (m_total_page % 20u == 0) /* Valor exato de p�ginas*/
							? (m_total_page / 20u) 
							: (m_total_page / 20u + 1u);
	}

}

response* CmdMailBoxInfo::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdMailBoxInfo::prepareConsulta][Error] m_uid is invalid(0).", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if ((int)m_page <= 0)
		throw exception("[CmdMailBoxInfo::prepareConsulta][Error] m_page(" + std::to_string((int)m_page) 
				+ ") is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	v_mb.clear();
	v_mb.shrink_to_fit();

	m_total_page = 0u;

	response *r = nullptr;

	if (m_type == NORMAL)
		r = procedure(_db, m_szConsulta[0], std::to_string(m_uid) + ", " + std::to_string(m_page - 1u));
	else if (m_type == NAO_LIDO)
		r = procedure(_db, m_szConsulta[1], std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o Email(s)" + (m_type == NAO_LIDO ? std::string(" Nao lido(s)") : std::string("")) 
			+ " do mail box" + (m_type == NORMAL ? "[PAGE=" + std::to_string(m_page) + "]" : "") + " do player: " + std::to_string(m_uid));

	return r;
}

std::vector< MailBox >& CmdMailBoxInfo::getInfo() {
	return v_mb;
}

uint32_t CmdMailBoxInfo::getUID() {
	return m_uid;
}

void CmdMailBoxInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdMailBoxInfo::TYPE CmdMailBoxInfo::getType() {
	return m_type;
}

void CmdMailBoxInfo::setType(TYPE _type) {
	m_type = _type;
}

uint32_t CmdMailBoxInfo::getPage() {
	return m_page;
}

void CmdMailBoxInfo::setPage(uint32_t _page) {
	m_page = _page;
}

uint32_t CmdMailBoxInfo::getTotalPage() {
	return m_total_page;
}
