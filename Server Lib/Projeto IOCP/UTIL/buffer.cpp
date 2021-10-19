// Arquivo buffer.cpp
// Criado em 28/05/2017 por Acrisio
// Implementação da classe Buffer

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "WinPort.h"
#include <pthread.h>
#endif

#include "buffer.h"
#include "exception.h"
#include "../TYPE/stdAType.h"
#include "../SOCKET/session.h"
#include "message_pool.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

volatile uint32_t stdA::Buffer::static_sequence = 0u;

Buffer::Buffer() {
	clear();

	m_mwsab = new WSABUF;

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_wr);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
};

Buffer::Buffer(int seq_mode) : m_seq_mode(seq_mode) {
	
	if (m_seq_mode)
#if defined(_WIN32)
		m_sequence = InterlockedIncrement(&Buffer::static_sequence) - 1;
#elif defined(__linux__)
		m_sequence = __atomic_add_fetch(&Buffer::static_sequence, 1, __ATOMIC_RELAXED) - 1;
#endif
	
	m_index_w = 0;
	//m_session = nullptr;

	m_mwsab = new WSABUF;

	memset(this, 0, 
#if defined(_WIN32)
	sizeof(OVERLAPPED)
#elif defined(__linux__)
	sizeof(epoll_event)
#endif
	);

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_wr);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
};

//Buffer::Buffer(session *_session, int seq_mode) : m_session(_session), m_seq_mode(seq_mode) {
//	
//	if (m_seq_mode)
//		m_sequence = InterlockedIncrement(&Buffer::static_sequence) - 1;
//	
//	m_index_w = 0;
//	
//	m_mwsab = new WSABUF;
//	memset(this, 0, sizeof(OVERLAPPED));
//
//	InitializeCriticalSection(&m_cs_wr);
//};

Buffer::Buffer(/*session *_session, */void* buffer, size_t size, int seq_mode) : /*m_session(_session), */m_seq_mode(seq_mode) {
	
	if (m_seq_mode)
#if defined(_WIN32)
		m_sequence = InterlockedIncrement(&Buffer::static_sequence) - 1;
#elif defined(__linux__)
		m_sequence = __atomic_add_fetch(&Buffer::static_sequence, 1, __ATOMIC_RELAXED) -1;
#endif
	
	m_index_w = 0;

	m_mwsab = new WSABUF;

	memset(this, 0, 
#if defined(_WIN32)
	sizeof(OVERLAPPED)
#elif defined(__linux__)
	sizeof(epoll_event)
#endif
	);
	
#if defined(_WIN32)
	InitializeCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_wr);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	write(buffer, size);
};

Buffer::~Buffer() {
	clear();

	m_index_w = 0;

	if (m_mwsab != nullptr)
		delete m_mwsab;

	m_mwsab = nullptr;

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs_wr);
#endif
};

void Buffer::clear() {
	m_index_w = 0;
	m_sequence = 0;
	m_seq_mode = 0;
	//m_session = nullptr;
	m_operation = ~0;

	memset(this, 0, 
#if defined(_WIN32)
	sizeof(OVERLAPPED)
#elif defined(__linux__)
	sizeof(epoll_event)
#endif
	);
};

void Buffer::init(int seq_mode) {
	m_seq_mode = seq_mode;

	if (m_seq_mode)
#if defined(_WIN32)
		m_sequence = InterlockedIncrement(&Buffer::static_sequence) - 1;
#elif defined(__linux__)
		m_sequence = __atomic_add_fetch(&Buffer::static_sequence, 1, __ATOMIC_RELAXED) - 1;
#endif
};

//void Buffer::init(session *_session, int seq_mode) {
//	
//	m_seq_mode = seq_mode;
//
//	if (m_seq_mode)
//		m_sequence = InterlockedIncrement(&Buffer::static_sequence) - 1;
//
//	m_session = _session;
//};

void Buffer::init(/*session *_session, */void* buffer, size_t size, int seq_mode) {

	m_seq_mode = seq_mode;

	if (m_seq_mode)
#if defined(_WIN32)
		m_sequence = InterlockedIncrement(&Buffer::static_sequence) - 1;
#elif defined(__linux__)
		m_sequence = __atomic_add_fetch(&Buffer::static_sequence, 1, __ATOMIC_RELAXED) - 1;
#endif

	//m_session = _session;

	write(buffer, size);
};

void Buffer::reset() {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_wr);
#endif

	m_index_w = 0;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_wr);
#endif
};

int Buffer::addSize(size_t size) {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_wr);
#endif

	if ((size + m_index_w) > MAX_BUFFER_SIZE) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_wr);
#endif
		return (MAX_BUFFER_SIZE - (int)(size + m_index_w));
	}

	int32_t sz_write = 0;

	if ((sz_write = (int)checkSize(size)) < 0)
		sz_write = 0;

	m_index_w += sz_write;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_wr);
#endif

	return sz_write;
};

int Buffer::write(void* buffer, size_t size) {

	if (buffer == nullptr)
		throw exception("Erro: buffer is null. Buffer::write()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BUFFER, 1, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_wr);
#endif

	int32_t sz_write = 0;

	if ((sz_write = (int)checkSize(size)) <= 0) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_wr);
#endif
		return 0;
	}

	memcpy(m_buffer + m_index_w, buffer, sz_write);
	m_index_w += sz_write;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_wr);
#endif

	return sz_write;
};

int Buffer::read(void* buffer, size_t size) {

	if (buffer == nullptr)
		throw exception("Erro: buffer is null. Buffer::read()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BUFFER, 1, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_wr);
#endif

	if (size > m_index_w) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_wr);
#endif
		return (int)(m_index_w - size);
	}

	size_t sz_read = size;

	memcpy(buffer, m_buffer, sz_read);
	m_index_w -= sz_read;

	if (m_index_w > 0)
		memmove(m_buffer, m_buffer + sz_read, m_index_w);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_wr);
#endif

	return (int)sz_read;
};

int Buffer::peek(void* buffer, size_t size) {

	if (buffer == nullptr)
		throw exception("Erro: buffer is null. Buffer::peek()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BUFFER, 1, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_wr);
#endif

	if (size > m_index_w) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_wr);
#endif
		return (int)(m_index_w - size);
	}

	size_t sz_read = size;

	memcpy(buffer, m_buffer, sz_read);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_wr);
#endif

	return (int)sz_read;
};

void Buffer::consume(size_t size) {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_wr);
#endif

	if (m_index_w <= 0) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_wr);
#endif
		throw exception("Erro nao pode consumir mais, o buffer index is less or equal 0. Buffer::consume()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BUFFER, 2, 0));
	}else if ((m_index_w - size) > MAX_BUFFER_SIZE/*Por que é unsigned ai negativo fica o maximo do int32_t que é maior que o MAX_BUFFER_SIZE*/) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_wr);
#endif
		throw exception("Erro nao pode consumir mais, o tamanho requisitado e maior que o buffer index. Buffer::consume()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BUFFER, 3, 0));
	}

	m_index_w -= size;
	
	if (m_index_w > 0)
		memmove(m_buffer, m_buffer + size, m_index_w);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_wr);
#endif
};

size_t Buffer::checkSize(size_t size) {
	if ((size + m_index_w) <= MAX_BUFFER_SIZE)
		return size;
	else
		return (size_t)(MAX_BUFFER_SIZE - m_index_w);
};

const unsigned char* Buffer::getBuffer() {
	return m_buffer;
};

size_t Buffer::getSize() {
	return MAX_BUFFER_SIZE;
};

size_t Buffer::getUsed() {
	return m_index_w;
};

WSABUF *Buffer::getWSABufToRead() {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_wr);
#endif

	m_mwsab->buf = reinterpret_cast< CHAR* >(&m_buffer[m_index_w]);
	
	if (m_index_w > MAX_BUFFER_SIZE)	// chegou ao limite enviar recive de 0 bytes
		m_mwsab->len = 0;
	else
		m_mwsab->len = (uint32_t)(MAX_BUFFER_SIZE - m_index_w);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_wr);
#endif

	return m_mwsab;
};

WSABUF *Buffer::getWSABufToSend() {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_wr);
#endif

	m_mwsab->buf = (char*)m_buffer;
	m_mwsab->len = (int)m_index_w;

	//m_index_w = 0;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_wr);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_wr);
#endif

	return m_mwsab;
};

DWORD Buffer::getOperation() {
	return m_operation;
};

void Buffer::setOperation(DWORD _operation) {
	m_operation = _operation;
};

//session* Buffer::getSession() {
//	return m_session;
//};

uint32_t Buffer::getSequence() {
	return m_sequence;
};

bool Buffer::isOrder() {
	return m_seq_mode == 1;
};

void Buffer::setOverlapped() {
	memset(this, 0, 
#if defined(_WIN32)
	sizeof(OVERLAPPED)
#elif defined(__linux__)
	sizeof(epoll_event)
#endif
	);
};

WSABUF& Buffer::getWSABUF() {
	return *m_mwsab;
};
