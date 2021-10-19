// Arquivo packet_func.h
// Criado em 05/12/2017 por Acrisio
// Definição da classe packet_func_base
// e das macros e funções de helper

#pragma once
#ifndef _STDA_PACKET_FUNC_H
#define _STDA_PACKET_FUNC_H

#include "../TYPE/stdAType.h"
#include "../TYPE/pangya_st.h"
#include "../SOCKET/session.h"
#include "../UTIL/func_arr.h"

#if defined SHOW_DEBUG_PACKET &&  defined _DEBUG
	#undef SHOW_DEBUG_PACKET
	#define SHOW_DEBUG_PACKET(_packet, _debug) if ((_debug)) _smp::message_pool::getInstance().push(new message(hex_util::BufferToHexString((_packet).getBuffer(), (_packet).getSize()), CL_ONLY_FILE_LOG));
	#define SHOW_DEBUG_FINAL_PACKET(_mwsabuff, _debug) if ((_debug)) _smp::message_pool::getInstance().push(new message(hex_util::BufferToHexString((unsigned char*)(_mwsabuff).buf, (_mwsabuff).len), CL_ONLY_FILE_LOG));
#else
	#define SHOW_DEBUG_PACKET
	#define SHOW_DEBUG_FINAL_PACKET
#endif

//#define _MAKE_BEGIN_PACKET ParamPacket pp; \
//						  pp.m_pw = reinterpret_cast<ParamWorker *>(arg); \
//						  pp.m_session = pp.m_pw->p->getSession(); \
//						  pp.m_pl_db = (pangya_db*)&pp.m_pw->_pl_db; \

#define _MAKE_BEGIN_PACKET(_arg) ParamDispatch pd = *reinterpret_cast< ParamDispatch* >((_arg));

#define _MAKE_BEGIN_PACKET_AUTH_SERVER(_arg) packet *_packet = reinterpret_cast< packet* >((_arg));

//#define _MSG_BEGIN_PACKET _smp::message_pool::getInstance().push(new message("Trata pacote " + std::to_string(pp.m_pw->p->getTipo()) + "(0x" + hex_util::ltoaToHex(pp.m_pw->p->getTipo()) + ")", CL_ONLY_FILE_LOG));
#define _MSG_BEGIN_PACKET _smp::message_pool::getInstance().push(new message("Trata pacote " + std::to_string(pd._packet->getTipo()) + "(0x" + hex_util::ltoaToHex(pd._packet->getTipo()) + ")", CL_ONLY_FILE_LOG));

#ifdef _DEBUG
#define MAKE_BEGIN_PACKET _MAKE_BEGIN_PACKET _MSG_BEGIN_PACKET
#else
#define MAKE_BEGIN_PACKET _MAKE_BEGIN_PACKET
#endif

//#define MAKE_BEGIN_PACKET_SUB ParamPacket_sub *pp_sub = reinterpret_cast<ParamPacket_sub *>(arg); \
//							  ParamWorker pw = pp_sub->m_pw; \
//							  ParamPacket pp = { &pw, pp_sub->m_pl_db, pp_sub->m_session }; \
//							  delete pp_sub; \

#define MAKE_BEGIN_SPLIT_PACKET(packet_id, _session, element_size, max_packet) \
		size_t por_packet = ((max_packet - 100) > (element_size) ? ((max_packet) - 100) / (element_size) : 1); \
		size_t i = 0, index = 0, total = elements; \
		auto _it = v_element.begin(); \
		for (index = 0; index < elements && _it != v_element.end(); total -= por_packet) { \
			p.init_plain((unsigned short)(packet_id)); \

#define MAKE_MED_SPLIT_PACKET(tipo) \
			if ((tipo) == 0) { \
				p.addInt16((short)total); \
				p.addInt16((short)((total > por_packet) ? por_packet : total)); \
			}else { \
				p.addUint32((uint32_t)total); \
				p.addUint32((uint32_t)((total > por_packet) ? por_packet : total)); \
			} \

#define MAKE_MID_SPLIT_PACKET_VECTOR(element_size) \
			for (i = 0; i < por_packet && index < elements && _it != v_element.end(); i++, index++, ++_it) \
				p.addBuffer(&(*_it), (element_size)); \

#define MAKE_MID_SPLIT_PACKET_MAP(element_size) \
			for (i = 0; i < por_packet && index < elements && _it != v_element.end(); i++, index++, ++_it) \
				p.addBuffer(&_it->second, (element_size)); \

#define MAKE_END_SPLIT_PACKET(_debug) packet_func::session_send(p, _session, (_debug));/*v_p.push_back(p);*/ \
		} \

#define MAKE_END_SPLIT_PACKET_REF(_debug) packet_func::session_send(p, &_session, (_debug));/*v_p.push_back(p);*/ \
		} \

namespace stdA {

    class packet_func_base {
        public:
            packet_func_base();
            ~packet_func_base();

		public:
			static func_arr funcs;		// Cliente
            static func_arr funcs_sv;	// Server (Retorno)
			static func_arr funcs_as;	// Auth Server
    };
}

#endif