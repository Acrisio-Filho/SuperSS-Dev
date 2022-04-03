// Arquivo player_mail_box.cpp
// Criado em 13/01/2021 por Acrisio
// Implementa��o da classe PlayerMailBox

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "player_mail_box.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"
#include "../PANGYA_DB/cmd_mail_box_info2.hpp"
#include "../PANGYA_DB/cmd_item_left_from_email.hpp"
#include "../PANGYA_DB/cmd_delete_email.hpp"
#include "../PANGYA_DB/cmd_email_info2.hpp"
#include "../PANGYA_DB/cmd_update_email.hpp"

#include <cmath>
#include <algorithm>

#if defined(_WIN32)
#define TRY_CHECK			 try { \
								EnterCriticalSection(&m_cs);
#elif defined(__linux__)
#define TRY_CHECK			 try { \
								pthread_mutex_lock(&m_cs);
#endif

#if defined(_WIN32)
#define LEAVE_CHECK				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
#define LEAVE_CHECK				pthread_mutex_unlock(&m_cs);
#endif

#define RETURN_CHECK			{ LEAVE_CHECK return; }
#define RETURNN_CHECK(_value)	{ LEAVE_CHECK return (_value); }

#if defined(_WIN32)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								LeaveCriticalSection(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[PlayerMailBox::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[PlayerMailBox::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

using namespace stdA;

PlayerMailBox::PlayerMailBox() : m_emails(), m_uid(0u), m_last_update{ 0u } {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

PlayerMailBox::~PlayerMailBox() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void PlayerMailBox::init(std::map< int32_t, EmailInfoEx >& _emails, uint32_t _uid) {

	TRY_CHECK;

	if (!m_emails.empty())
		m_emails.clear();
	
	m_uid = _uid;
	m_emails = _emails;

	// Initialize last update time
	GetLocalTime(&m_last_update);

	// Log
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerMailBox::init][Log] Player[UID=" + std::to_string(m_uid) + "] Inicializou Mail Box, DATE=" 
			+ _formatDate(m_last_update) + ", with (" + std::to_string(m_emails.size()) + ") Email in Mail Box.", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerMailBox::init][Log] Player[UID=" + std::to_string(m_uid) + "] Inicializou Mail Box, DATE="
			+ _formatDate(m_last_update) + ", with (" + std::to_string(m_emails.size()) + ") Email in Mail Box.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

	LEAVE_CHECK;
	CATCH_CHECK("init");
	END_CHECK;
}

void PlayerMailBox::clear() {

	TRY_CHECK;

	m_uid = 0u;

	if (!m_emails.empty())
		m_emails.clear();

	memset(&m_last_update, 0, sizeof(m_last_update));

	LEAVE_CHECK;
	CATCH_CHECK("clear");
	END_CHECK;
}

// Unsave thread sync
bool PlayerMailBox::checkLastUpdate() {
	return (uint64_t)(getLocalTimeDiff(m_last_update) / STDA_10_MICRO_PER_MILLI) >= EXPIRES_CACHE_TIME;
}

void PlayerMailBox::checkAndUpdate() {

	TRY_CHECK;

	if (checkLastUpdate())
		update();

	LEAVE_CHECK;
	CATCH_CHECK("checkAndUpdate");

	// Relan�a
	throw;

	END_CHECK;
}

// Unsafe thread sync
void PlayerMailBox::copyEmailInfoExToMailBox(EmailInfoEx& _email, MailBox& _mail) {
	
	// Clear MailBox Object
	_mail.clear();

	_mail.id = _email.id;
#if defined(_WIN32)
	memcpy_s(_mail.from_id, sizeof(_mail.from_id), _email.from_id, sizeof(_email.from_id));
	memcpy_s(_mail.msg, sizeof(_mail.msg), _email.msg, sizeof(_mail.msg));
#elif defined(__linux__)
	memcpy(_mail.from_id, _email.from_id, sizeof(_email.from_id));
	memcpy(_mail.msg, _email.msg, sizeof(_mail.msg));
#endif
	_mail.visit_count = _email.visit_count;
	_mail.lida_yn = _email.lida_yn;

	_mail.item_num = (int)_email.itens.size();

	if (!_email.itens.empty())
		_mail.item = _email.itens.front();
}

void PlayerMailBox::update() {

	TRY_CHECK;

	if (m_uid == 0u)
		throw exception("[PlayerMailBox::update][Error] m_uid is invalid(0), call init method to set uid of player.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 1, 0));

	CmdMailBoxInfo2 cmd_mbi2(m_uid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_mbi2, nullptr, nullptr);

	cmd_mbi2.waitEvent();

	if (cmd_mbi2.getException().getCodeError() != 0)
		throw cmd_mbi2.getException();

	if (!m_emails.empty())
		m_emails.clear();

	m_emails = cmd_mbi2.getInfo();

	// Update last time update
	GetLocalTime(&m_last_update);

#ifdef _DEBUG
	// Log
	_smp::message_pool::getInstance().push(new message("[PlayerMailBox::update][Log] Player[UID=" + std::to_string(m_uid) + "] Atualizou o Cache MailBox, DATE=" 
			+ _formatDate(m_last_update) + ", with (" + std::to_string(m_emails.size()) + ") Email in Mail Box.", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	LEAVE_CHECK;
	CATCH_CHECK("update");

	// Relan�a para quem chamou tratar
	throw;
	
	END_CHECK;
}

std::vector< MailBox > PlayerMailBox::getPage(uint32_t _page) {

	std::vector< MailBox > mails;

	TRY_CHECK;

	if (_page == 0u)
		throw exception("[PlayerMailBox::getPage][Error] Player[UID=" + std::to_string(m_uid) + "] Page(" 
				+ std::to_string(_page) + ") invalid page number.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 2, 0));

	// Verifica se o tempo do cache expirou, se sim atualiza ele novamente
	checkAndUpdate();

	// Check is Empty
	if (m_emails.empty())
		RETURNN_CHECK(mails);

	// Verifica se a p�gina existe
	if ((float)_page > std::ceil(m_emails.size() / (float)NUM_OF_EMAIL_PER_PAGE))
		throw exception("[PlayerMailBox::getPage][Error] Player[UID=" + std::to_string(m_uid) + "] Page(" 
				+ std::to_string(_page) + ") not exists.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 3, 0));

	// Iterators
	std::map< int32_t, EmailInfoEx >::reverse_iterator it_begin, it_end;

	it_begin = std::next(m_emails.rbegin(), ((_page - 1u) * NUM_OF_EMAIL_PER_PAGE));

	// Distance of End of map emails
	auto it_distance = std::distance(it_begin, m_emails.rend());

	it_end = std::next(it_begin, it_distance > NUM_OF_EMAIL_PER_PAGE ? NUM_OF_EMAIL_PER_PAGE : it_distance);

	MailBox mail{ 0u };

	for (; it_begin != it_end; it_begin++) {

		copyEmailInfoExToMailBox(it_begin->second, mail);

		// Add to Mails vector
		mails.push_back(mail);
	}

	LEAVE_CHECK;
	CATCH_CHECK("getPage");

	// Error
	if (!mails.empty())
		mails.clear();

	END_CHECK;

	// Classifica pelo o ultimo email ao chegar no mail box
	std::sort(mails.begin(), mails.end(), PlayerMailBox::sort_last_arrived);

	return mails;
}

std::vector< MailBox > PlayerMailBox::getAllUnreadEmail() {
	
	std::vector< MailBox > unread;

	TRY_CHECK;

	// Verifica se o tempo do cache expirou, se sim atualiza ele novamente
	checkAndUpdate();

	if (m_emails.empty())
		RETURNN_CHECK(unread);

	MailBox mail{ 0u };

	for (auto it = m_emails.rbegin(); it != m_emails.rend(); it++) {

		if (it->second.lida_yn)
			continue;	// J� foi lido

		copyEmailInfoExToMailBox(it->second, mail);

		// Add Email n�o lido
		unread.push_back(mail);

		// Verifica se chegou no limite de Email n�o lidos, que pode enviar para o player
		if (unread.size() >= LIMIT_OF_UNREAD_EMAIL)
			break;
	}

	LEAVE_CHECK;
	CATCH_CHECK("getAllUnreadEmail");

	// Error
	if (!unread.empty())
		unread.clear();

	END_CHECK;

	// Classifica pelo o ultimo email ao chegar no mail box
	std::sort(unread.begin(), unread.end(), PlayerMailBox::sort_last_arrived);

	return unread;
}

uint32_t PlayerMailBox::getTotalPages() {
	
	uint32_t total_pages = 0u;

	TRY_CHECK;

	// Verifica se o tempo do cache expirou, se sim atualiza ele novamente
	checkAndUpdate();
	
	total_pages = (uint32_t)std::ceil(m_emails.size() / (float)NUM_OF_EMAIL_PER_PAGE);

	LEAVE_CHECK;
	CATCH_CHECK("getTotalPages");
	END_CHECK;

	return total_pages;
}

void PlayerMailBox::addNewEmailArrived(int32_t _id) {

	TRY_CHECK;

	if (_id <= 0)
		throw exception("[PlayerMailBox::getEmailInfo][Error] Player[UID=" + std::to_string(m_uid) + "] email id(" + std::to_string(_id)
				+ ") is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 4, 0));

	auto it = m_emails.find(_id);
	
	// N�o precisa adicionar, o cache j� atualizou ele
	if (it != m_emails.end())
		RETURN_CHECK;

	// N�o encontrou, verifica se o cache est� desatualizado
	// Verifica se o tempo do cache expirou, se sim atualiza ele novamente
	checkAndUpdate();

	it = m_emails.find(_id);

	// Adiciona o email ao cache
	if (it == m_emails.end()) {

		CmdEmailInfo2 cmd_ei2(m_uid, _id, true); // Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_ei2, nullptr, nullptr);

		cmd_ei2.waitEvent();

		if (cmd_ei2.getException().getCodeError() != 0)
			throw cmd_ei2.getException();

		// Add New Email
		m_emails.insert({ cmd_ei2.getInfo().id, cmd_ei2.getInfo() });
	}

	LEAVE_CHECK;
	CATCH_CHECK("addNewEmailArrived");

	// Relan�a
	throw;

	END_CHECK;
}

EmailInfo PlayerMailBox::getEmailInfo(int32_t _id, bool _ler) {

	EmailInfo ei{ 0u };

	TRY_CHECK;

	if (_id <= 0)
		throw exception("[PlayerMailBox::getEmailInfo][Error] Player[UID=" + std::to_string(m_uid) + "] email id(" + std::to_string(_id) 
				+ ") is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 4, 0));

	// Verifica se o tempo do cache expirou, se sim atualiza ele novamente
	checkAndUpdate();

	// Check is Empty
	if (m_emails.empty())
		throw exception("[PlayerMailBox::getEmailInfo][Error] Player[UID=" + std::to_string(m_uid) + "] mail box empty, not have how find email id(" 
				+ std::to_string(_id) + ").", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 5, 0));

	auto it = m_emails.find(_id);

	if (it != m_emails.end()) {

		// Soma +1 no contador de visitas ao email(foi visto)
		if (_ler) {

			if (!it->second.lida_yn)
				it->second.lida_yn = 1u;

			it->second.visit_count++;

			// UPDATE ON DB
			snmdb::NormalManagerDB::getInstance().add(3, new CmdUpdateEmail(m_uid, it->second), PlayerMailBox::SQLDBResponse, this);
		}

		// Copy
		ei = it->second;
	}

	LEAVE_CHECK;
	CATCH_CHECK("getEmailInfo");

	// Error
	ei.clear();

	END_CHECK;

	return ei;
}

void PlayerMailBox::leftItensFromEmail(int32_t _id) {

	TRY_CHECK;

	if (_id <= 0)
		throw exception("[PlayerMailBox::leftItensFromEmail][Error] Player[UID=" + std::to_string(m_uid) + "] email id(" + std::to_string(_id) 
				+ ") is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 4, 0));

	// Verifica se o tempo do cache expirou, se sim atualiza ele novamente
	checkAndUpdate();

	// Check is Empty
	if (m_emails.empty())
		throw exception("[PlayerMailBox::leftItensFromEmail][Error] Player[UID=" + std::to_string(m_uid) 
				+ "] mail box empty, not have how delete itens from email id(" + std::to_string(_id) + ").", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 5, 0));

	auto it = m_emails.find(_id);

	if (it != m_emails.end() && !it->second.itens.empty()) {

		it->second.itens.clear();
		it->second.itens.shrink_to_fit();
	}

	// UPDATE ON DB
	snmdb::NormalManagerDB::getInstance().add(1, new CmdItemLeftFromEmail(_id), PlayerMailBox::SQLDBResponse, this);

	LEAVE_CHECK;
	CATCH_CHECK("leftItensFromEmail");

	// Relan�a
	throw;

	END_CHECK;
}

void PlayerMailBox::deleteEmail(int32_t* _a_id, uint32_t _count) {

	TRY_CHECK;

	if (_a_id == nullptr)
		throw exception("[PlayerMailBox::deleteEmail][Error] Player[UID=" + std::to_string(m_uid) + "] email[ptr(nulllptr), count(" + std::to_string(_count) + ")] is invalid.",
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 6, 0));

	// Verifify Email Id(s)
	for (auto i = 0u; i < _count; i++) {

		if (_a_id[i] <= 0)
			throw exception("[PlayerMailBox::deleteEmail][Error] Player[UID=" + std::to_string(m_uid) + "] email[id(" + std::to_string(_a_id[i])
					+ "), count(" + std::to_string(_count) + ")] is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 4, 0));
	}

	// Verifica se o tempo do cache expirou, se sim atualiza ele novamente
	checkAndUpdate();

	// Lambda Print All Email Id(s)
	auto lambdaPrintAllEmailIds = [](int32_t* _a_id, uint32_t _count) -> std::string {

		std::string ret = "";

		if (_a_id == nullptr)
			return ret;

		for (auto i = 0u; i < _count; i++)
			ret += (i == 0) ? std::to_string(_a_id[i]) : (", " + std::to_string(_a_id[i]));

		return ret;
	};

	// Check is Empty
	if (m_emails.empty())
		throw exception("[PlayerMailBox::deleteEmail][Error] Player[UID=" + std::to_string(m_uid) + "] mail box empty, not have how delete email id(s){"
				+ lambdaPrintAllEmailIds(_a_id, _count) + "}.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MAIL_BOX, 5, 0));

	std::map< int32_t, EmailInfoEx >::iterator it = m_emails.end();

	for (auto i = 0u; i < _count; i++) {

		if ((it = m_emails.find(_a_id[i])) == m_emails.end()) {
		
			// Log
			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::deleteEmail][Error][WARNING] Player[UID=" + std::to_string(m_uid) 
					+ "] nao encontrou o Email[id(" + std::to_string(_a_id[i]) + ")].", CL_FILE_LOG_AND_CONSOLE));
			
			continue;
		}

		// Remove Email from map
		m_emails.erase(it);
	}

	// UPDATE ON DB
	snmdb::NormalManagerDB::getInstance().add(2, new CmdDeleteEmail(m_uid, _a_id, _count), PlayerMailBox::SQLDBResponse, this);

	LEAVE_CHECK;
	CATCH_CHECK("deleteEmail");

	// Relan�a
	throw;

	END_CHECK;
}

void PlayerMailBox::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	try {

		if (_arg == nullptr) {

			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][WARNING] _arg is nullptr na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
			
			return;
		}

		auto pmb = reinterpret_cast< PlayerMailBox* >(_arg);

		// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
		if (_pangya_db.getException().getCodeError() != 0) {

			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][Error] Player[UID=" + std::to_string(pmb->m_uid) + "]" 
					+ _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			return;
		}

		switch (_msg_id) {
		case 1:
		{
			auto cmd_ilfe = reinterpret_cast< CmdItemLeftFromEmail* >(&_pangya_db);

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][Log] Player[UID=" + std::to_string(pmb->m_uid) 
					+ "] Tirou os itens do Email[ID=" + std::to_string(cmd_ilfe->getEmailID()) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][Log] Player[UID=" + std::to_string(pmb->m_uid)
					+ "] Tirou os itens do Email[ID=" + std::to_string(cmd_ilfe->getEmailID()) + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

			break;
		}
		case 2:
		{
			auto cmd_de = reinterpret_cast< CmdDeleteEmail* >(&_pangya_db);

			// Lambda Print All Email Id(s)
			auto lambdaPrintAllEmailIds = [](int32_t* _a_id, uint32_t _count) -> std::string {

				std::string ret = "";

				if (_a_id == nullptr)
					return ret;

				for (auto i = 0u; i < _count; i++)
					ret += (i == 0) ? std::to_string(_a_id[i]) : (", " + std::to_string(_a_id[i]));

				return ret;
			};

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][Log] Player[UID=" + std::to_string(pmb->m_uid) 
					+ "] deletou Email id(s){" + lambdaPrintAllEmailIds(cmd_de->getEmailID(), cmd_de->getCount()) + "} com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][Log] Player[UID=" + std::to_string(pmb->m_uid)
					+ "] deletou Email id(s){" + lambdaPrintAllEmailIds(cmd_de->getEmailID(), cmd_de->getCount()) + "} com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

			break;
		}
		case 3:
		{
			auto cmd_ue = reinterpret_cast< CmdUpdateEmail* >(&_pangya_db);

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_ue->getUID()) 
					+ "] atualizou Email[ID=" + std::to_string(cmd_ue->getEmail().id) + ", LIDA_YN=" + std::to_string((unsigned short)cmd_ue->getEmail().lida_yn) 
					+ ", VISIT_COUNT=" + std::to_string(cmd_ue->getEmail().visit_count) + "] com sucesso", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_ue->getUID())
					+ "] atualizou Email[ID=" + std::to_string(cmd_ue->getEmail().id) + ", LIDA_YN=" + std::to_string((unsigned short)cmd_ue->getEmail().lida_yn)
					+ ", VISIT_COUNT=" + std::to_string(cmd_ue->getEmail().visit_count) + "] com sucesso", CL_ONLY_FILE_LOG));
#endif // _DEBUG

			break;
		}
		case 0:
		default:
			break;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PlayerMailBox::SQLDBResponse][Error] QUERY_MSG[ID=" + std::to_string(_msg_id)
				+ "]" + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool PlayerMailBox::sort_last_arrived(MailBox& _rhs, MailBox& _lhs) {
	return _rhs.id > _lhs.id;
}
