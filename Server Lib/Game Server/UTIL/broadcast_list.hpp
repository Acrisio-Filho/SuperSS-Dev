// Arquivo broadcast_list.hpp
// Criado em 03/12/2018 as 22:44 por Acrisio
// Definição da classe BroadcastList

#pragma once
#ifndef _STDA_BROADCAST_LIST_HPP
#define _STDA_BROADCAST_LIST_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <map>
#include <memory>

namespace stdA {
	class BroadcastList {
		public:
			enum RET_TYPE : unsigned char {
				NO_NOTICE,		// Não tem notice na lista
				OK,
				WAIT,
			};

			enum TYPE : unsigned char {
				GM_NOTICE,
				CUBE_WIN_RARE,
				TICKER,
			};

			struct NoticeCtx {
				NoticeCtx(uint32_t _ul = 0u);
				NoticeCtx(uint32_t _time_second, std::string _notice, TYPE _type);
				NoticeCtx(uint32_t _time_second, std::string _notice, uint32_t _option, TYPE _type);
				NoticeCtx(uint32_t _time_second, std::string _nickname, std::string _notice, TYPE _type);
				void clear();
				TYPE type;
				uint32_t time_second;
				uint32_t option;
				std::string nickname;
				std::string notice;
			};

			struct RetNoticeCtx {
				RetNoticeCtx(uint32_t _ul = 0u);
				void clear();
				RET_TYPE ret;
				NoticeCtx nc;
			};

		public:
			BroadcastList(uint32_t _interval_time_second);
			virtual ~BroadcastList();

			void push_back(int32_t _time, std::string _notice, TYPE _type);
			void push_back(int32_t _time, std::string _notice, uint32_t _option, TYPE _type);
			void push_back(int32_t _time, std::string _nickname, std::string _notice, TYPE _type);
			void push_back(NoticeCtx _nc);

			RetNoticeCtx peek();

			uint32_t getSize();

		protected:
			std::multimap< uint32_t /*time*/, NoticeCtx > m_list;
			uint32_t m_last_peek;

			// Interval time to peek next Notice
			uint32_t m_interval;

		private:
#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};
}

#endif // !_STDA_BROADCAST_LIST_HPP
