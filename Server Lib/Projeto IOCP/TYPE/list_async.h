// Arquivo list_async.h
// Criado em 14/01/2018 por Acrisio
// Definição e Implementação do Template list_async

#pragma once
#ifndef _STDA_LIST_ASYNC_H
#define _STDA_LIST_ASYNC_H

#include <vector>
#include <deque>
#include <string>
#include <typeinfo>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <pthread.h>
#endif

#include "../UTIL/exception.h"

#include "stda_error.h"

#include <stdexcept>

#if defined(_WIN32)
#define WAIT_NEW_OBJECT(_deque, _cs, _cv, _milliseconds, _method) \
    while ((_deque).empty()) { \
            if (SleepConditionVariableCS(&(_cv), &(_cs), (_milliseconds)) == 0) { \
				\
                DWORD error = GetLastError(); \
				\
				LeaveCriticalSection(&(_cs)); \
				\
                if (error == ERROR_TIMEOUT) { \
					\
					if ((_milliseconds) == INFINITE) \
						throw exception("Error time out, but _milliseconds is INFINITE. wrong and unknown error. On " + std::string(typeid(this).name()) + "::" + std::string(_method) + "()", \
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_ASYNC, 4, error)); \
					else \
						throw exception("Error time out. On " + std::string(typeid(this).name()) + "::" + std::string(_method) + "()", \
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_ASYNC, 2, error)); \
                }else \
                    throw exception("Error ao receber o sinal da condition variable. On " + std::string(typeid(this).name()) + "::" + std::string(_method) + "()", \
                        STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_ASYNC, 3, error)); \
            } \
        } 
#elif defined(__linux__)
#define WAIT_NEW_OBJECT(_deque, _cs, _cv, _milliseconds, _method) \
    while ((_deque).empty()) { \
                int error = 0; \
                if ((int32_t)(_milliseconds) == INFINITE) { \
                \
                        if ((error = pthread_cond_wait(&_cv, &_cs)) != 0) { \
                                pthread_mutex_unlock(&_cs); \
                                \
                                throw exception("Erro ao receber sinal the condition variable in (" + std::string(typeid(this).name()) + ")::" + std::string((_method)) +  "()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_ASYNC, 3, error)); \
                        } \
                \
                }else { \
                        timespec wait_time = _milliseconds_to_timespec_clock_realtime(_milliseconds); \
                        \
                        if ((error = pthread_cond_timedwait(&_cv, &_cs, &wait_time)) != 0) { \
                                pthread_mutex_unlock(&_cs); \
                                \
                                if (error == ETIMEDOUT) \
                                        throw exception("Erro de timeout. (" + std::string(typeid(this).name()) + ")::" + std::string((_method)) + "().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_ASYNC, 2, 0)); \
                                else \
                                        throw exception("Error time out, but _milliseconds is INFINITE. wrong and unknown error. On (" + std::string(typeid(this).name()) + ")::" + std::string((_method)) + "()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_ASYNC, 4, error)); \
                        } \
                } \
        }
#endif

namespace stdA {
    template< class _LA > class list_async {
        public:
            list_async();
            ~list_async();

			void push(_LA _in);
			void push_front(_LA _in);
			void push_back(_LA _in);

			void pop();
			void pop_front();
			void pop_back();

			void clear();
			void erase(size_t _index);

			size_t size();
			bool empty();

			// Gets e Peeks
			_LA get(DWORD _dwMilliseconds = INFINITE);
			_LA getFirst(DWORD _dwMilliseconds = INFINITE);
			_LA getLast(DWORD _dwMilliseconds = INFINITE);

			_LA peek(DWORD _dwMilliseconds = INFINITE);
			_LA peekFirst(DWORD _dwMilliseconds = INFINITE);
			_LA peekLast(DWORD _dwMilliseconds = INFINITE);

			std::vector< _LA > getAll();

			// Operators
			_LA operator[](size_t _index);

        private:
            void init();
            void destroy();

        protected:
            std::deque < _LA > m_deque;
#if defined(_WIN32)
            CRITICAL_SECTION m_cs;
            CONDITION_VARIABLE m_cv;
#elif defined(__linux__)
            pthread_mutex_t m_cs;
            pthread_cond_t m_cv;
#endif
    };

	// Method Find
	template< class _LA, class _T1, class _T2 > _T1 find(list_async< _LA > _container, _T1 _value, _T2 compare) {
		if (_container.empty())
			return nullptr;

		for (auto i = 0u; i < _container.size(); ++i)
			if (compare(_container[i], _value))
				return _container[i];

		return nullptr;
	};

    // Implementação da classe
    template< class _LA > list_async< _LA >::list_async() {
        init();
    };

    template< class _LA > list_async< _LA >::~list_async() {
        clear();

        destroy();
    };

    template< class _LA > void list_async< _LA >::init() {
#if defined(_WIN32)
        InitializeCriticalSection(&m_cs);
        InitializeConditionVariable(&m_cv);
#elif defined(__linux__)
        INIT_PTHREAD_MUTEXATTR_RECURSIVE;
        INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
        DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

        pthread_cond_init(&m_cv, nullptr);
#endif
    };

    template< class _LA > void list_async< _LA >::destroy() {
#if defined(_WIN32)
        DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_destroy(&m_cs);
        pthread_cond_destroy(&m_cv);
#endif
    };

    template< class _LA > void list_async< _LA >::clear() {
#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        m_deque.clear();

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
#endif
    };

    template< class _LA > void list_async< _LA >::erase(size_t _index) {
#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        try {
            m_deque.erase(m_deque.begin() + _index);
        }catch (std::out_of_range& e) {
#if defined(_WIN32)
            LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
            pthread_mutex_unlock(&m_cs);
#endif

            throw exception("Error out of range, " + std::string(e.what())
				+ ". On " + std::string(typeid(this).name()) + "::erase().", 
                                STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_ASYNC, 1, 0));
        }

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
#endif
    };

    template< class _LA > size_t list_async< _LA >::size() {
        size_t size = 0u;

#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        size = m_deque.size();

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
#endif

        return size;
    };

    template< class _LA > bool list_async< _LA >::empty() {
        bool empty = false;

#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        empty = m_deque.empty();

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
#endif

        return empty;
    };

    template< class _LA > _LA list_async< _LA >::operator[](size_t _index) {
        _LA out;

#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        try {
            out = m_deque[_index];
        }catch (std::out_of_range& e) {

#if defined(_WIN32)
            LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
            pthread_mutex_unlock(&m_cs);
#endif

            throw exception("Error out of range, " + std::string(e.what())
				+ ". On " + std::string(typeid(this).name()) + "::operator[]().",
                                STDA_MAKE_ERROR(STDA_ERROR_TYPE::LIST_ASYNC, 1, 0));
        }

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
#endif

        return out;
    };

    template< class _LA > void list_async< _LA >::push(_LA _in) {
        push_back(_in);
    };

    template< class _LA > void list_async< _LA >::push_front(_LA _in) {
#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        m_deque.push_front(_in);

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
        WakeConditionVariable(&m_cv);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
        pthread_cond_signal(&m_cv);
#endif
    };

    template< class _LA > void list_async< _LA >::push_back(_LA _in) {
#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        m_deque.push_back(_in);

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
        WakeConditionVariable(&m_cv);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
        pthread_cond_signal(&m_cv);
#endif
    };

    template< class _LA > void list_async< _LA >::pop() {
        pop_front();
    };

    template< class _LA > void list_async< _LA >::pop_front() {
#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        m_deque.pop_front();

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
#endif
    };

    template< class _LA > void list_async< _LA >::pop_back() {
#if defined(_WIN32)
        EnterCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_lock(&m_cs);
#endif

        m_deque.pop_back();

#if defined(_WIN32)
        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
        pthread_mutex_unlock(&m_cs);
#endif
    };

    template< class _LA > _LA list_async< _LA >::get(DWORD _dwMilliseconds) {
        return getFirst(_dwMilliseconds);
    };

    template< class _LA > _LA list_async< _LA >::getFirst(DWORD _dwMilliseconds) {
        _LA out;

		try {

#if defined(_WIN32)
                EnterCriticalSection(&m_cs);
#elif defined(__linux__)
                pthread_mutex_lock(&m_cs);
#endif

                WAIT_NEW_OBJECT(m_deque, m_cs, m_cv, _dwMilliseconds, "getFirst");

                out = m_deque.front();
                m_deque.pop_front();

#if defined(_WIN32)
                LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                pthread_mutex_unlock(&m_cs);
#endif
		
		}catch (exception& e) {

                // Qualquer erro que não seja do LIST_ASYNC o critical section não foi liberado ainda
                if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::LIST_ASYNC)
#if defined(_WIN32)
                        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                        pthread_mutex_unlock(&m_cs);
#endif

                // Relança
                throw exception(e.getMessageError(), e.getCodeError());
        }

        return out;
    };

	template< class _LA > std::vector< _LA > list_async< _LA >::getAll() {
		std::vector< _LA > _out;

		try {
		
#if defined(_WIN32)
            EnterCriticalSection(&m_cs);
#elif defined(__linux__)
            pthread_mutex_lock(&m_cs);
#endif

			for (auto& el : m_deque)
				_out.push_back(el);

			m_deque.clear();
			m_deque.shrink_to_fit();

#if defined(_WIN32)
            LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
            pthread_mutex_unlock(&m_cs);
#endif
		
		}catch (exception& e) {

            // Qualquer erro que não seja do LIST_ASYNC o critical section não foi liberado ainda
            if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::LIST_ASYNC)
#if defined(_WIN32)
                    LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                    pthread_mutex_unlock(&m_cs);
#endif

            // Relança
            throw exception(e.getMessageError(), e.getCodeError());
        }

        return _out;
	};

    template< class _LA > _LA list_async< _LA >::getLast(DWORD _dwMilliseconds) {
        _LA out;

		try {
		
#if defined(_WIN32)
                EnterCriticalSection(&m_cs);
#elif defined(__linux__)
                pthread_mutex_lock(&m_cs);
#endif

                WAIT_NEW_OBJECT(m_deque, m_cs, m_cv, _dwMilliseconds, "getLast");

                out = m_deque.back();
                m_deque.pop_back();

#if defined(_WIN32)
                LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                pthread_mutex_unlock(&m_cs);
#endif
		
			}catch (exception& e) {

                // Qualquer erro que não seja do LIST_ASYNC o critical section não foi liberado ainda
                if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::LIST_ASYNC)
#if defined(_WIN32)
                        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                        pthread_mutex_unlock(&m_cs);
#endif

                // Relança
                throw exception(e.getMessageError(), e.getCodeError());
        }

        return out;
    };

    template< class _LA > _LA list_async< _LA >::peek(DWORD _dwMilliseconds) {
        return peekFirst(_dwMilliseconds);
    };

    template< class _LA > _LA list_async< _LA >::peekFirst(DWORD _dwMilliseconds) {
        _LA out;

		try {
		
#if defined(_WIN32)
                EnterCriticalSection(&m_cs);
#elif defined(__linux__)
                pthread_mutex_lock(&m_cs);
#endif

                WAIT_NEW_OBJECT(m_deque, m_cs, m_cv, _dwMilliseconds, "peekFirst");

                out = m_deque.front();

#if defined(_WIN32)
                LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                pthread_mutex_unlock(&m_cs);
#endif
		
        }catch (exception& e) {

                // Qualquer erro que não seja do LIST_ASYNC o critical section não foi liberado ainda
                if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::LIST_ASYNC)
#if defined(_WIN32)
                        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                        pthread_mutex_unlock(&m_cs);
#endif

                // Relança
                throw exception(e.getMessageError(), e.getCodeError());
        }

        return out;
    };

    template< class _LA > _LA list_async< _LA >::peekLast(DWORD _dwMilliseconds) {
        _LA out;

		try {
		
#if defined(_WIN32)
                EnterCriticalSection(&m_cs);
#elif defined(__linux__)
                pthread_mutex_lock(&m_cs);
#endif

                WAIT_NEW_OBJECT(m_deque, m_cs, m_cv, _dwMilliseconds, "peekLast");

                out = m_deque.back();

#if defined(_WIN32)
                LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                pthread_mutex_unlock(&m_cs);
#endif
		
        }catch (exception& e) {

                // Qualquer erro que não seja do LIST_ASYNC o critical section não foi liberado ainda
                if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::LIST_ASYNC)
#if defined(_WIN32)
                        LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
                        pthread_mutex_unlock(&m_cs);
#endif

			// Relança
			throw exception(e.getMessageError(), e.getCodeError());
		}

        return out;
    };
}

#endif // !_STDA_LIST_ASYNC_H