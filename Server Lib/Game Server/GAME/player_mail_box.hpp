// Arquivo player_mail_box.hpp
// Criado em 13/01/2021 as 09:52 por Acrisio
// Defini��o da classe PlayerMailBox

#pragma once
#ifndef _STDA_PLAYER_MAIL_BOX_HPP
#define _STDA_PLAYER_MAIL_BOX_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <map>

#include "../TYPE/pangya_game_st.h"
#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {

	constexpr uint64_t EXPIRES_CACHE_TIME = 3 * 1000ull;	// 3 Segundos
	constexpr uint32_t NUM_OF_EMAIL_PER_PAGE	= 20u;			// 20 Emails por p�gina
	constexpr uint32_t LIMIT_OF_UNREAD_EMAIL	= 300u;		// 300 Emails n�o lidos que pode enviar para o player

	class PlayerMailBox {
		public:
			PlayerMailBox();
			virtual ~PlayerMailBox();

			void init(std::map< int32_t, EmailInfoEx >& _emails, uint32_t _uid);
			void clear();

			std::vector< MailBox > getPage(uint32_t _page);
			std::vector< MailBox > getAllUnreadEmail();

			uint32_t getTotalPages();

			void addNewEmailArrived(int32_t _id);

			EmailInfo getEmailInfo(int32_t _id, bool _ler = true);

			// Deleta todos os itens quem tem no email, eles j� foram tirados do email pelo player
			void leftItensFromEmail(int32_t _id);

			// Deleta 1 ou mais emails de uma vez
			void deleteEmail(int32_t* _a_id, uint32_t _count);

		protected:
			// Methods Static
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

			static bool sort_last_arrived(MailBox& _rhs, MailBox& _lhs);

		protected:
			// Unsafe thread sync
			bool checkLastUpdate();

			void checkAndUpdate();

			// Unsafe thread sync
			void copyEmailInfoExToMailBox(EmailInfoEx& _email, MailBox& _mail);

			void update();

		protected:
			uint32_t m_uid;

			std::map< int32_t, EmailInfoEx > m_emails;

			SYSTEMTIME m_last_update;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};
}

#endif // !_STDA_PLAYER_MAIL_BOX_HPP
