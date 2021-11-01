// Arquivo box_system.hpp
// Criado em 15/07/2018 as 21:25 por Acrisio
// Defini��o da classe BoxSystem

#pragma once
#ifndef _STDA_BOX_SYSTEM_HPP
#define _STDA_BOX_SYSTEM_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#endif

#include "../TYPE/box_type.hpp"
#include "../SESSION/player.hpp"

#include <map>
#include <vector>

// Spinning Cube
#ifndef SPINNING_CUBE_TYPEID
#define SPINNING_CUBE_TYPEID 0x1A00015B
#endif // !SPINNING_CUBE_TYPEID

#define OPENNED_SPINNING_CUBE_TYPEID 0x1A000161
#define KEY_OF_SPINNING_CUBE_TYPEID 0x1A00015C

// Papel Box
#define PAPEL_BOX_TYPEID 0x1A000208

// Pang Pouch
#ifndef PANG_POUCH_TYPEID
#define PANG_POUCH_TYPEID 0x1A000010
#endif // !PANG_POUCH_TYPEID

namespace stdA {
	class BoxSystem {
		public:
			BoxSystem();
			virtual ~BoxSystem();

			/*static*/ void load();

			/*static*/ bool isLoad();

			/*static*/ ctx_box* findBox(uint32_t _typeid);

			/*static*/ ctx_box_item* drawBox(player& _session, ctx_box& _ctx_b);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

		private:
			/*static*/ std::map< uint32_t, ctx_box > m_box;	// Todas as box do server que tem no DB

			/*static*/ bool m_load;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< BoxSystem > sBoxSystem;
}

#endif // !_STDA_BOX_SYSTEM_HPP
