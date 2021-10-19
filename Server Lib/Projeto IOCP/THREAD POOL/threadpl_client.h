// Arquivo threadpl_client.h
// Criado em 03/12/2017 por Acrisio
// Definição da classe threadpl_client

#pragma once
#ifndef _STDA_THREADPL_CLIENT_H
#define _STDA_THREADPL_CLIENT_H

#include "threadpool.h"

#include "../SOCKET/wsa.h"

#define LOOP_TRANSLATE_BUFFER_TO_PACKET_SERVER_INVERT(_ph, _sz_ph, STDA_OT) \
					BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), GET_FUNC_RECV, SET_FUNC_RECV); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					MED_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), (STDA_OT), GET_FUNC_RECV, SET_FUNC_RECV); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					END_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), (STDA_OT), GET_FUNC_RECV, SET_FUNC_RECV); \

#define LOOP_TRANSLATE_BUFFER_TO_PACKET_CLIENT_INVERT(_phc, _sz_phc, STDA_OT) \
					BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), GET_FUNC_SEND, SET_FUNC_SEND); \
						\
					_packet->unMake(_session->m_key); \
						\
					MED_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), (STDA_OT), GET_FUNC_SEND, SET_FUNC_SEND); \
						\
					_packet->unMake(_session->m_key); \
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
    class threadpl_client : public threadpool {
        public:
            threadpl_client(size_t _num_thread_workers_io, size_t _num_thread_workers_logical);
            virtual ~threadpl_client();

			virtual void waitAllThreadFinish(DWORD dwMilleseconds) = 0;

			virtual void DisconnectSession(session *_session) = 0;

		protected:
			virtual std::string getSessionID(session *_session) = 0;

        protected:
			virtual DWORD monitor() = 0;

			virtual void dispach_packet_same_thread(session& _session, packet *_packet) = 0;
			virtual void dispach_packet_sv_same_thread(session& _session, packet *_packet) = 0;

            virtual void translate_packet(session *_session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation) override;

		protected:
			wsa m_wsa;
    };
}

#endif