// Arquivo mail_box_manager.cpp
// Criado em 26/05/2018 as 14:47 por Acrisio
// Implementa��o da classe MailBoxManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "mail_box_manager.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PANGYA_DB/cmd_add_msg_mail.hpp"
#include "../PANGYA_DB/cmd_put_item_mail_box.hpp"
#include "../PANGYA_DB/cmd_insert_command.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../../Projeto IOCP/UTIL/string_util.hpp"

using namespace stdA;

MailBoxManager::MailBoxManager() {
}

MailBoxManager::~MailBoxManager() {
}

int32_t MailBoxManager::sendMessage(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg) {

	int32_t msg_id = _sendMessage(_from_uid, _to_uid, _msg);

	if (msg_id <= 0)
		throw exception("[MailBoxManager::sendMessage][Error] nao conseguiu criar uma msg no banco de dados", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 3, 0));

	putCommandNewMail(_to_uid, msg_id);

	return msg_id;
}

int32_t MailBoxManager::sendMessageWithItem(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg, std::vector< stItem >& _v_item) {

	int32_t msg_id = _sendMessage(_from_uid, _to_uid, _msg);

	if (msg_id <= 0)
		throw exception("[MailBoxManager::sendMessageWithItem][Error] nao conseguiu criar uma msg no banco de dados", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 3, 0));

	putItemInMail(_from_uid, _to_uid, msg_id, _v_item);

	putCommandNewMail(_to_uid, msg_id);

	return msg_id;
}

int32_t MailBoxManager::sendMessageWithItem(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg, stItem& _item) {
	
	int32_t msg_id = _sendMessage(_from_uid, _to_uid, _msg);

	if (msg_id <= 0)
		throw exception("[MailBoxManager::sendMessageWithItem][Error] nao conseguiu criar uma msg no banco de dados", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 3, 0));

	putItemInMail(_from_uid, _to_uid, msg_id, _item);

	putCommandNewMail(_to_uid, msg_id);

	return msg_id;
}

int32_t MailBoxManager::sendMessageWithItem(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg, EmailInfo::item* _pItem, uint32_t _count) {

	int32_t msg_id = _sendMessage(_from_uid, _to_uid, _msg);

	if (msg_id <= 0)
		throw exception("[MailBoxManager::sendMessageWithItem][Error] nao conseguiu criar um msg no banco de dados", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 3, 0));

	putItemInMail(_from_uid, _to_uid, msg_id, _pItem, _count);

	putCommandNewMail(_to_uid, msg_id);

	return msg_id;
}

int32_t MailBoxManager::sendMessageWithItem(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg, EmailInfo::item& _item) {
	
	int32_t msg_id = _sendMessage(_from_uid, _to_uid, _msg);

	if (msg_id <= 0)
		throw exception("[MailBoxManager::sendMessageWithItem][Error] nao conseguiu criar uma msg no banco de dados", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 3, 0));

	putItemInMail(_from_uid, _to_uid, msg_id, _item);

	putCommandNewMail(_to_uid, msg_id);

	return msg_id;
}

inline int32_t MailBoxManager::_sendMessage(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg) {
	
	if (_to_uid == 0u)
		throw exception("[MailBoxManager::_sendMessage][Error] uid[value=" + std::to_string(_to_uid) + "] to send message is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 1, 0));

	if (_msg.empty())
		throw exception("[MailBoxManager::_sendMessage][Error] _msg is empty", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 2, 0));

	// Trata mensagem para n�o da erro no MSSQL, tipo aspas em nome, tipo Arin's gift box
	auto index = _msg.find('\'');

	if (index != std::string::npos) {
		_msg.replace(index, 1, 2, _msg[index]);

		_smp::message_pool::getInstance().push(new message("[MailBoxManager::_sendMessage][Log] replace string para[str=" + _msg + "] por que tinha valores que o MSSQL nao aceita", CL_FILE_LOG_AND_CONSOLE));
	}

	// Trate for Chat not printed ex:(Kanji) CJK
	_msg = verifyAndEncode(_msg);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[MailBoxManager::_sendMessage][Log] Encode Message: " + _msg, CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	// cmd coloca msg no gift table
	CmdAddMsgMail cmd_amm(_from_uid, _to_uid, _msg, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_amm, nullptr, nullptr);

	cmd_amm.waitEvent();

	if (cmd_amm.getException().getCodeError() != 0)
		throw cmd_amm.getException();

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[MailBoxManager::_sendMessage][Log] player[UID=" + std::to_string(_from_uid) + "] enviou Msg['" + _msg + "'] para o player[UID=" + std::to_string(_to_uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[MailBoxManager::_sendMessage][Log] player[UID=" + std::to_string(_from_uid) + "] enviou Msg['" + _msg + "'] para o player[UID=" + std::to_string(_to_uid) + "]", CL_ONLY_FILE_LOG));
#endif

	return cmd_amm.getMailID();
}

void MailBoxManager::putItemInMail(uint32_t _from_uid, uint32_t _to_uid, int32_t _mail_id, std::vector< stItem >& _v_item) {

	if (_mail_id <= 0)
		throw exception("[MailBoxManager::putItemInMail][Error] _mail_id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 4, 0));

	if (_v_item.empty())
		throw exception("[MailBoxManager::putItemInMail][Error] vector of itens is empty", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 5, 0));

	for (auto& el : _v_item) {
		try {
			putItemInMail(_from_uid, _to_uid, _mail_id, el);
		}catch (exception& e) {
			// Se n�o for erro de item invalid, relan�a a exception
			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 6))
				throw;
		}
	}
}

inline void MailBoxManager::putItemInMail(uint32_t _from_uid, uint32_t _to_uid, int32_t _mail_id, stItem& _item) {

	if (_mail_id <= 0)
		throw exception("[MailBoxManager::putItemInMail][Error] _mail_id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 4, 0));

	if (_item._typeid == 0)
		throw exception("[MailBoxManager::putItemInMail][Error] _item is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 6, 0));

	// Cmd add item
	CmdPutItemMailBox cmd_pimb(_from_uid, _to_uid, _mail_id, _item, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_pimb, nullptr, nullptr);

	cmd_pimb.waitEvent();

	if (cmd_pimb.getException().getCodeError() != 0)
		throw cmd_pimb.getException();

	// Pronto j� colocou o item no mail do player
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[MailBoxManager::putItemInMail][Log] player[UID=" + std::to_string(_from_uid) + "] colocou item[TYPEID=" + 
				std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + ", QNTD=" + std::to_string(_item.qntd > 0xFFu ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "] no mail[ID=" + std::to_string(_mail_id) + "] do player[UID=" + std::to_string(_to_uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[MailBoxManager::putItemInMail][Log] player[UID=" + std::to_string(_from_uid) + "] colocou item[TYPEID=" +
		std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + ", QNTD=" + std::to_string(_item.qntd > 0xFFu ? _item.qntd : _item.STDA_C_ITEM_QNTD) + "] no mail[ID=" + std::to_string(_mail_id) + "] do player[UID=" + std::to_string(_to_uid) + "]", CL_ONLY_FILE_LOG));
#endif
}

void MailBoxManager::putItemInMail(uint32_t _from_uid, uint32_t _to_uid, int32_t _mail_id, EmailInfo::item* _pItem, uint32_t _count) {

	if (_mail_id <= 0)
		throw exception("[MailBoxManager::putItemInMail][Error] _mail_id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 4, 0));

	if (_pItem == nullptr)
		throw exception("[MailBoxManager::putItemInMail][Error] _pItem is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 7, 0));

	if ((int)_count <= 0)
		throw exception("[MailBoxManager::putItemInMail][Error] count[value=" + std::to_string(_count) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 8, 0));

	for (auto i = 0u; i < _count; ++i) {
		try {
			putItemInMail(_from_uid, _to_uid, _mail_id, _pItem[i]);
		}catch (exception& e) {
			// Se n�o for erro de item invalid, relan�a a exception
			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 6))
				throw;
		}
	}
}

inline void MailBoxManager::putItemInMail(uint32_t _from_uid, uint32_t _to_uid, int32_t _mail_id, EmailInfo::item& _item) {

	if (_mail_id <= 0)
		throw exception("[MailBoxManager::putItemInMail][Error] _mail_id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 4, 0));

	if (_item._typeid == 0)
		throw exception("[MailBoxManager::putItemInMail][Error] _item is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 6, 0));

	// Cmd add item
	CmdPutItemMailBox cmd_pimb(_from_uid, _to_uid, _mail_id, _item, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_pimb, nullptr, nullptr);

	cmd_pimb.waitEvent();

	if (cmd_pimb.getException().getCodeError() != 0)
		throw cmd_pimb.getException();

	// Pronto j� colocou o item no mail do player
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[MailBoxManager::putItemInMail][Log] player[UID=" + std::to_string(_from_uid) + "] colocou item[TYPEID=" +
		std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + ", QNTD=" + std::to_string(_item.qntd) + "] no mail[ID=" + std::to_string(_mail_id) + "] do player[UID=" + std::to_string(_to_uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[MailBoxManager::putItemInMail][Log] player[UID=" + std::to_string(_from_uid) + "] colocou item[TYPEID=" +
		std::to_string(_item._typeid) + ", ID=" + std::to_string(_item.id) + ", QNTD=" + std::to_string(_item.qntd) + "] no mail[ID=" + std::to_string(_mail_id) + "] do player[UID=" + std::to_string(_to_uid) + "]", CL_ONLY_FILE_LOG));
#endif
}

inline void MailBoxManager::putCommandNewMail(uint32_t _to_uid, int32_t _mail_id) {

	if (_to_uid == 0u)
		throw exception("[MailBoxManager::putCommandNewMail][Error] uid[value=" + std::to_string(_to_uid) + "] to put Command. uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 1, 0));

	if (_mail_id <= 0)
		throw exception("[MailBoxManager::putCommandNewMail][Error] _mail_id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MAIL_BOX_MANAGER, 4, 0));

	CommandInfo ci{ 0 };

	ci.id = 4;				// New Mail Arrived on Mailbox of player
	ci.arg[0] = _to_uid;

	ci.arg[1] = _mail_id;
	
	ci.valid = 1;
	
	ci.target = 1;	// Todos os game server

	snmdb::NormalManagerDB::getInstance().add(1, new CmdInsertCommand(ci), MailBoxManager::SQLDBResponse, nullptr);

}

void MailBoxManager::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
#ifdef _DEBUG
		// Static class
		_smp::message_pool::getInstance().push(new message("[MailBoxManager::SQLDBResponse][WARNING] _arg is nullptr na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[MailBoxManager::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// isso aqui depois pode mudar para o MailBoxManager, que vou tirar de ser uma classe static e usar ela como objeto(instancia)
	auto _session = reinterpret_cast< player* >(_arg);

	switch (_msg_id) {
	case 1:	// Insert Command
	{
		auto cmd_ic = reinterpret_cast< CmdInsertCommand* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[MailBoxManager::SQLDBResponse][Log] Adicionou Command[" + cmd_ic->getInfo().toString() + "] com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:
		break;
	}
}
