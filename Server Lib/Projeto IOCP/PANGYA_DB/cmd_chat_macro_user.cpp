// Arquivo cmd_chat_macro_user.cpp
// Criado em 18/03/2018 as 11:21 por Acrisio
// Implementação da classe CmdChatMacroUser

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_chat_macro_user.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

using namespace stdA;

CmdChatMacroUser::CmdChatMacroUser(bool _waiter) : pangya_db(_waiter), m_uid(0), m_macro_user{} {
};

CmdChatMacroUser::CmdChatMacroUser(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_macro_user{} {
};

CmdChatMacroUser::~CmdChatMacroUser() {
};

void CmdChatMacroUser::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(9, (uint32_t)_result->cols);

	for (auto i = 0u; i < 9u; i++) {

		if (_result->data[i] != nullptr) {

			try {

				// Translate Msg From Encoded Char not printed
				auto _chat = verifyAndTranslate(_result->data[i], 2/*fixed size*/);

				// !@ possivel erro de violação de acesso
				if (_chat.empty())
#if defined(_WIN32)
					memcpy_s(m_macro_user.macro[i], sizeof(chat_macro_user::macro[0]), _result->data[i], (strlen(_result->data[i]) > sizeof(chat_macro_user::macro[0]) ? sizeof(chat_macro_user::macro[0]) : strlen(_result->data[i])));
#elif defined(__linux__)
					memcpy(m_macro_user.macro[i], _result->data[i], (strlen(_result->data[i]) > sizeof(chat_macro_user::macro[0]) ? sizeof(chat_macro_user::macro[0]) : strlen(_result->data[i])));
#endif
				else
#if defined(_WIN32)
					memcpy_s(m_macro_user.macro[i], sizeof(m_macro_user.macro[i]), _chat.data(), (_chat.length() > sizeof(chat_macro_user::macro[0]) ? sizeof(chat_macro_user::macro[0]) : _chat.length()));
#elif defined(__linux__)
					memcpy(m_macro_user.macro[i], _chat.data(), (_chat.length() > sizeof(chat_macro_user::macro[0]) ? sizeof(chat_macro_user::macro[0]) : _chat.length()));
#endif

			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[CmdChatMacroUser::lineResult][ErrorSystem][Teste com o try para nao sair do cmd db]  " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}
		}
	}
};

response* CmdChatMacroUser::prepareConsulta(database& _db) {
    
    m_macro_user.clear();

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar o macro do player: " + std::to_string(m_uid));

	return r; 
};

chat_macro_user& CmdChatMacroUser::getMacroUser() {
    return m_macro_user;
};

void CmdChatMacroUser::setMacroUser(chat_macro_user& _macro_user) {
    m_macro_user = _macro_user;
};

uint32_t CmdChatMacroUser::getUID() {
    return m_uid;
};

void CmdChatMacroUser::setUID(uint32_t _uid) {
    m_uid = _uid;
};
