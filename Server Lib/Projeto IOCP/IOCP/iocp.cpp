// Arquivo iocp.cpp
// Criado em 02/03/2017 por Acrisio
// Implementação da classe iocp

#include "iocp.h"

#include "../util/exception.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

iocp::iocp(size_t max_num_thread_same_time) {

	if ((m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, (uint32_t)max_num_thread_same_time)) == NULL)
		throw exception("Erro ao criar o iocp:iocp()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 1, GetLastError()));
};

iocp::~iocp() {
	CancelIo(m_iocp);
	CloseHandle(m_iocp);
};

void iocp::postStatus(ULONG_PTR completionKey, DWORD numBytes, OVERLAPPED *pOverlapped) {
	if (0 == PostQueuedCompletionStatus(m_iocp, numBytes, completionKey, pOverlapped))
		throw exception("Erro ao postar um completion status,  iocp::postStatus()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 2, GetLastError()));
};

void iocp::getStatus(PULONG_PTR completionKey, PDWORD numBytes, LPOVERLAPPED *pOverlapped, DWORD timeout_mili) {
	if (0 == GetQueuedCompletionStatus(m_iocp, numBytes, completionKey, pOverlapped, timeout_mili))
		throw exception("Erro ao pegar um completion status, iocp::getStatus()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 3, GetLastError()));
};

bool iocp::associaDeviceToPort(ULONG_PTR completionKey, HANDLE device) {
	bool ok = true;

	if ((m_iocp = CreateIoCompletionPort(device, m_iocp, completionKey, 0)) == NULL) {
		DWORD lastError = GetLastError();

		if (lastError != WAIT_TIMEOUT)
			throw exception("Erro ao associar o despositivo ao completion port, ioco::associaDeviceToPort()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 4, GetLastError()));

		ok = false;
	}

	return ok;
};