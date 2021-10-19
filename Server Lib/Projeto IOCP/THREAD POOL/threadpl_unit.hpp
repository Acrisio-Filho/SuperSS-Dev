// Arquivo threadpl_unit.hpp
// Criado em 02/12/2018 as 12:11 por Acrisio
// Definição da classe threadpl_unit, Cuida da comunicação do Auth Server com os outros serveres


#pragma once
#ifndef _STDA_THREADPL_UNIT_HPP
#define _STDA_THREADPL_UNIT_HPP

#include "threadpool.h"

#if defined(_WIN32)
#include "../SOCKET/wsa.h"
#endif

#define LOOP_TRANSLATE_BUFFER_TO_PACKET_SERVER_UNIT(_ph, _sz_ph, STDA_OT) \
					BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), GET_FUNC_RECV, SET_FUNC_RECV); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					MED_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), (STDA_OT), GET_FUNC_RECV, SET_FUNC_RECV); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					END_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), (STDA_OT), GET_FUNC_RECV, SET_FUNC_RECV); \

#define LOOP_TRANSLATE_BUFFER_TO_PACKET_CLIENT_UNIT(_phc, _sz_phc, STDA_OT) \
					BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), GET_FUNC_SEND, SET_FUNC_SEND); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					MED_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), (STDA_OT), GET_FUNC_SEND, SET_FUNC_SEND); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					END_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), (STDA_OT), GET_FUNC_SEND, SET_FUNC_SEND); \

#define CLEAR_PACKET_LOOP_WITH_MSG_CLIENT \
	if (_packet != nullptr) { \
		_smp::message_pool::getInstance().push(new message("Session 0x" + hex_util::ltoaToHex((int)_session) + "\tKey: " + std::to_string((int)_session->m_key) + "\tID: " + getSessionID((_session)), CL_ONLY_CONSOLE)); \
		_smp::message_pool::getInstance().push(new message(hex_util::BufferToHexString((unsigned char*)_packet->getMakedBuf().buf, _packet->getMakedBuf().len), CL_ONLY_CONSOLE)); \
		delete _packet; \
		_packet = nullptr; \
	} \

namespace stdA {
    class threadpl_unit : public threadpool {
        public:
            threadpl_unit(size_t _num_thread_workers_io, size_t _num_thread_workers_logical, uint32_t _job_thread_num);
            virtual ~threadpl_unit();

            virtual void waitAllThreadFinish(DWORD dwMilleseconds) = 0;

			virtual bool DisconnectSession(session *_session) = 0;

        protected:
#if defined(_WIN32)
			virtual DWORD accept() = 0;
            virtual DWORD monitor() = 0;
			virtual DWORD disconnect_session() = 0;
#elif defined(__linux__)
			virtual void* accept() = 0;
            virtual void* monitor() = 0;
			virtual void* disconnect_session() = 0;
#endif

			virtual void accept_completed(SOCKET *_listener, DWORD dwIOsize, myOver *lpBuffer, DWORD _operation) = 0;

			virtual void dispach_packet_same_thread(session& _session, packet *_packet) = 0;
			virtual void dispach_packet_sv_same_thread(session& _session, packet *_packet) = 0;

            virtual void translate_packet(session *_session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation) override;

#if defined(_WIN32)
		protected:
			wsa m_wsa;
#endif
    };
}

#endif // !_STDA_THREADPL_UNIT_HPP