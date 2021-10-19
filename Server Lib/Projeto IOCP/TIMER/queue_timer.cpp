// Arquivo queue_timer.cpp
// Criado em 12/03/2021 as 13:55 por Acrisio
// Implementação da classe QueueTimer

#include "queue_timer.hpp"

#include <sys/epoll.h>
#include <sys/fcntl.h>

#include <algorithm>

#include "../UTIL/message_pool.h"
#include "../UTIL/exception.h"

using namespace stdA;

#define RETURN_AND_LEAVE_MUTEX(_mutex)  { pthread_mutex_unlock((_mutex)); \
    return; }
#define RETURNV_AND_LEAVE_MUTEX(_mutex, _ret) { pthread_mutex_unlock((_mutex)); \
    return (_ret); }

#define FIND_TIMER(_vector, _timer) std::find_if((_vector).begin(), (_vector).end(), [&](timer_epoll*& _el) { \
            return _el != nullptr && _el == (_timer); \
        });

#define FIND_TIMER_BY_FD(_vector, _fd) std::find_if((_vector).begin(), (_vector).end(), [&](timer_epoll*& _el) { \
            return _el != nullptr && _el->fd == (_fd); \
        });

// Port Windows Function
QueueTimer* QueueTimer::CreateTimerQueue() {

    QueueTimer *queue = nullptr;

    try {

        queue = new QueueTimer();

    }catch (exception& e) {

        queue = nullptr;

        _smp::message_pool::getInstance().push(new message("[QueueTimer::CreateTimerQueue][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }

    return queue;
}

uint32_t QueueTimer::CreateTimerQueueTimer(int32_t* _timerfd, QueueTimer* _queue_timer, WaitOrTimerCallback _callback, void* _lpParameter, uint32_t _dueTime, uint32_t _period, uint32_t _flags) {

    uint32_t ret = 1u;

    try {

        // !@ Na versão do windows ele usar um TimerQueue padrão se não for fornecido um TimerQueue, 
        // Como estão estou usando essa opção não vou implementar ele agora
        if (_timerfd == nullptr || _queue_timer == nullptr || !_queue_timer->is_good())
            return 0u; // Error

        ret = _queue_timer->createTimer(_timerfd, _callback, _lpParameter, _dueTime, _period, _flags);

    }catch (exception& e) {
        
        _smp::message_pool::getInstance().push(new message("[QueueTimer::CreateTimerQueue][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

        ret = 0u; // Error
    }

    return ret; // OK
}

uint32_t QueueTimer::DeleteTimerQueueTimer(QueueTimer* _queue_timer, int32_t _timerfd, Event* _event) {

    uint32_t ret = 1u; // OK

    try {

        // !@ Esse á a mesma coisa do create, se for null ele usa um TimerQueue padrão,
        // Mas não vou implementar agora, por que não uso
        if (_queue_timer == nullptr || !_queue_timer->is_good())
            return 0u; // Error

        ret = _queue_timer->deleteTimer(_timerfd, _event);

    }catch (exception& e) {

        ret = 0u; // Error

        _smp::message_pool::getInstance().push(new message("[QueueTimer::DeleteTimerQueueTimer][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }

    return ret;
}

uint32_t QueueTimer::ChangeTimerQueueTimer(QueueTimer* _queue_timer, int32_t _timerfd, uint32_t _dueTime, uint32_t _period) {

    if (_queue_timer != nullptr || !_queue_timer->is_good())
        return 0u; // Error

    // Lança uma exceção
    _queue_timer->changeTimer(_timerfd, _dueTime, _period);

    return 1u; // Não executa esse código
}

uint32_t QueueTimer::DeleteTimerQueueEx(QueueTimer* _queue_timer, Event* _event) {

    uint32_t ret = 1u; // OK

    try {

        if (_queue_timer == nullptr)
            return 0u; // Error

        // ignora o erro por enquanto
        uint32_t err = _queue_timer->destroy();

        // Suporte Event, para não da deadlock
        if (_event != nullptr)
            _event->set(1u);

        delete _queue_timer;

        _queue_timer = nullptr;

    }catch (exception& e) {

        ret = 0u; // Error

        _smp::message_pool::getInstance().push(new message("[QueueTImer::DeleteTimerQueueEx][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }

    return ret;
}

QueueTimer::QueueTimer()
    : m_epollfd(-1), m_timer_thread(nullptr), m_thread_pool(), m_msgs(), m_good(false) {

    __atomic_store_n(&m_down_io_worker, 1, __ATOMIC_RELAXED);
    __atomic_store_n(&m_down_timer, 1, __ATOMIC_RELAXED);

    // pthread mutex recursive
    pthread_mutexattr_t __mattr;
    pthread_mutexattr_init(&__mattr);
    pthread_mutexattr_settype(&__mattr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&m_cs, &__mattr);

    pthread_mutexattr_destroy(&__mattr);

    // make epoll fd
    m_epollfd = epoll_create1(0);

    if (m_epollfd == -1)
        throw exception("[QueueTimer::QueueTimer][Error] Failed to create epoll.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::QUEUETIMER, 1, errno));

    // Timer thread
    m_timer_thread = new thread(1, QueueTimer::_timer_thread, (void*)this, 0/*flag priority*/, 0/*flag mask cpu*/);

    // Worker thread
    for (auto i = 0u; i < MIN_THREAD_POOL; ++i)
        m_thread_pool.push_back(new thread(2, QueueTimer::_io_worker, (void*)this, 0, 0));

    m_good = true;
}

QueueTimer::~QueueTimer() {

    destroy();

    /*downAllThread();

    if (!m_msgs.empty())
        m_msgs.clear();

    clear_timers();

    if (m_epollfd != -1)
        close(m_epollfd);

    m_epollfd = -1;

    pthread_mutex_destroy(&m_cs);

    m_good = false;*/
}

bool QueueTimer::is_good() {
    return m_good;
};

uint32_t QueueTimer::destroy() {

    uint32_t ret = 1u; // OK

    try {

        if (!is_good())
            return ret; // Já foi destruído

        // !@ Acho que não vou fazer agora por notifição, deixa o evento de lado
        // Para deletar por evento com async, vou ter que criar uma thread de limpeza statica
        downAllThread();

        if (!m_msgs.empty())
            m_msgs.clear();

        clear_timers();

        if (m_epollfd != -1)
            close(m_epollfd);

        m_epollfd = -1;

        pthread_mutex_destroy(&m_cs);

        m_good = false;

    }catch (exception& e) {

        ret = 0u; // Error

        _smp::message_pool::getInstance().push(new message("[QueueTimer::destroy][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }

    return ret;
};

uint32_t QueueTimer::createTimer(int32_t* _fd, WaitOrTimerCallback _callback, void* _lpParameter, uint32_t _dueTime, uint32_t _period, uint32_t _flags) {

    timer_epoll *timer = nullptr;

    try {

        if (!is_good())
            return 0u; // System error

        if (_callback == nullptr || _fd == nullptr)
            return 0u; // Error, Invalid parameter

        if (_dueTime == 0u && _period == 0u)
            return 0u; // Invalid parameter

        timer = new timer_epoll;

        // Set to Default
        timer->deleting = 0u;
        timer->toDelete = 0u;
        timer->running_callback = 0u;
        timer->event_delete = nullptr;

        timer->callback = _callback;
        timer->lpParameter = _lpParameter;
        timer->flags = _flags;

        // Create timer
        timer->fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK); // 0 since kernel 2.6.2

        if (timer->fd == -1)
            throw exception("[QueueTimer::createTimer][Error] Failed to make timerfd.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::QUEUETIMER, 2, errno));

        int32_t err = -1;

        if (_dueTime == 0) {

            timer->time_val.it_value.tv_nsec = 1; // Imediato
            timer->time_val.it_value.tv_sec = 0;
        
        }else {

            timer->time_val.it_value.tv_sec = _dueTime / 1000; // Sec
            timer->time_val.it_value.tv_nsec = (_dueTime % 1000) * 1000000; // nano
            
        }

        if (_period == 0) {
            timer->time_val.it_interval.tv_sec = 0;
            timer->time_val.it_interval.tv_nsec = 0;
        }else {
            timer->time_val.it_interval.tv_sec = _period / 1000; // Sec
            timer->time_val.it_interval.tv_nsec = (_period % 1000) * 1000000; // nano
        }

        // Set Epoll
        epoll_event ev{ 0u };

        ev.data.ptr = (void*)timer;
        ev.events = EPOLLIN || EPOLLET;

        err = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, timer->fd, &ev);

        if (err == -1)
            throw exception("[QueueTimer::createTimer][Error] Failed to set timerfd to list interest of epoll.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::QUEUETIMER, 3, errno));

        // Init Timer
        err = timerfd_settime(timer->fd, 0, &timer->time_val, nullptr);

        if (err == -1)
            throw exception("[QueueTimer::createTimer][Error] Failed to init timerfd.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::QUEUETIMER, 4, errno));

        // Lock
        pthread_mutex_lock(&m_cs);

        m_timers.push_back(timer);

        pthread_mutex_unlock(&m_cs);

        // set o fd do retorno
        *_fd = timer->fd;

    }catch (exception& e) {

        // clear mem
        if (timer != nullptr)
            delete timer;

        _smp::message_pool::getInstance().push(new message("[QueueTimer::createTimer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

        return 0u; // Error
    }

    return 1u; // OK
}

uint32_t QueueTimer::changeTimer(int32_t _fd, uint32_t _dueTime, uint32_t _period) {

    // !@ não vou fazer agora
    throw exception("[QueueTimer::changeTimer][WARNING] nao vou fazer essa funcao por que nao uso ela no Windows, nao preciso porta para o Linux agora.", 
        STDA_MAKE_ERROR(STDA_ERROR_TYPE::QUEUETIMER, 550, 0));

    return 0u; // !@ não executa esse código
};

uint32_t QueueTimer::deleteTimer(int32_t _fd, Event* _event) {

    try {

        if (!is_good())
            return 0u; // System error;

        if (m_timers.empty())
            return 0u; // Error

        pthread_mutex_lock(&m_cs);

        auto it = FIND_TIMER_BY_FD(m_timers, _fd);

        if (it == m_timers.end())
            RETURNV_AND_LEAVE_MUTEX(&m_cs, -1);
        
        uint32_t ret = setDeleteTimer(it, _event);

        if (ret == eSET_DEL::SD_ESTA_EXECUTANDO)
            RETURNV_AND_LEAVE_MUTEX(&m_cs, ERROR_IO_PENDING);

        if (ret != eSET_DEL::SD_OK)
            RETURNV_AND_LEAVE_MUTEX(&m_cs, 0u); // Error

        pthread_mutex_unlock(&m_cs);

    }catch (exception& e) {

        pthread_mutex_unlock(&m_cs);

        _smp::message_pool::getInstance().push(new message("[QueueTimer::deleteTimer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

        return 0u; // Error
    }

    return 1u; // OK
}

void QueueTimer::downAllThread() {

    down_threads_io_worker();
    down_thread_timer();
}

void QueueTimer::down_thread_timer() {

    try {

        if (m_timer_thread == nullptr)
            return;

        uint32_t old = __atomic_sub_fetch(&m_down_timer, 1, __ATOMIC_RELAXED);

        if (old > 0)
            __atomic_store_n(&m_down_timer, 0, __ATOMIC_RELAXED);

        if (!m_timer_thread->isLive()) {

            delete m_timer_thread;

            m_timer_thread = nullptr;

            return;
        }

        m_timer_thread->waitThreadFinish(INFINITE);

        delete m_timer_thread;

        m_timer_thread = nullptr;

    }catch (exception& e) {

        _smp::message_pool::getInstance().push(new message("[QueueTimer::down_thread_timer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }
}

void QueueTimer::down_threads_io_worker() {

    try {

        if (m_thread_pool.empty())
            return;

        uint32_t old = __atomic_sub_fetch(&m_down_io_worker, 1, __ATOMIC_RELAXED);

        if (old > 0)
            __atomic_store_n(&m_down_io_worker, 0, __ATOMIC_RELAXED);

        for (auto& el : m_thread_pool) {

            try {

                if (el == nullptr)
                    continue;

                if (!el->isLive()) {

                    delete el;

                    el = nullptr;

                    continue;
                }

                el->waitThreadFinish(INFINITE);

                delete el;

                el = nullptr;

            }catch (exception& e) {

                _smp::message_pool::getInstance().push(new message("[QueueTimer::down_threads_io_worker::loop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
            }
        }

    }catch (exception& e) {

        _smp::message_pool::getInstance().push(new message("[QueueTimer::down_threads_io_worker][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }
}

void QueueTimer::clear_timers() {

    try {

        if (m_timers.empty())
            return;

        pthread_mutex_lock(&m_cs);

        while (!m_timers.empty()) {

            if (m_timers.front() == nullptr) {

                m_timers.erase(m_timers.begin());

                continue;
            }

            deleteTimer(m_timers.begin());
        }

        pthread_mutex_unlock(&m_cs);

    }catch (exception& e) {

        pthread_mutex_unlock(&m_cs);

        _smp::message_pool::getInstance().push(new message("[QueueTimer::clear_timers][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }
}

void* QueueTimer::_timer_thread(void* lpParameter) {
    BEGIN_THREAD_SETUP(QueueTimer);

    result = pTP->timer_thread();

    END_THREAD_SETUP("_timer_thread()");
}

void* QueueTimer::_io_worker(void* lpParameter) {
    BEGIN_THREAD_SETUP(QueueTimer);

    result = pTP->io_worker();

    END_THREAD_SETUP("_io_worker");
}

void* QueueTimer::timer_thread() {

    try {

        // Log
        _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread][Log] Timer Thread inicializada.", CL_FILE_LOG_AND_CONSOLE));

        uint32_t check_active = 1u, /*check_del = 1u,*/ i = 0u;
        int32_t count = 0;
        uint64_t buff = 0ull;
        int32_t err = 0;

        uint32_t check_ret = 0u;

        timer_epoll *timer_tmp = nullptr;

        epoll_event ev[MIN_THREAD_POOL]{ 0u };

        while (__atomic_compare_exchange_n(&m_down_timer, &check_active, 1, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {

            try {

                count = epoll_wait(m_epollfd, ev, MIN_THREAD_POOL, 10000); // 1 seg

                if (count == -1) {

                    _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread::MainLoop][Error] Failed to call epoll_wait. Code: " + std::to_string(errno), CL_FILE_LOG_AND_CONSOLE));

                    /* The call was interrupted by a signal handler before either
                    * (1) any of the requested events occurred or (2) the
                    * timeout expired; see signal(7).
                    */
                    if (errno == EINTR)
                        continue;

                    break; // Error epoll wait
                }

                for (i = 0u; i < count; ++i) {

                    if (ev[i].data.ptr == nullptr)
                        continue;

                    timer_tmp = reinterpret_cast< timer_epoll* >(ev[i].data.ptr);

                    check_ret = setExecuteTimerExpires(timer_tmp);

                    if (check_ret == eSET_EXEC::SE_DELETE_TIMER_NOW) {

                        deleteTimer(timer_tmp);

                        timer_tmp = nullptr;

                        // Continue to next timer
                        continue;
                    
                    }else if (check_ret != eSET_EXEC::SE_OK)
                        continue; // Error, next timer

                    err = read(timer_tmp->fd, &buff, sizeof(buff));

                    if (err <= 0) {
                        
                        if (errno != EAGAIN)
                            _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread::MainLoop][Error][WARNING] failed to read timer fd, Code: " + std::to_string(errno), CL_FILE_LOG_AND_CONSOLE));

                        continue; // Timer ainda não expirou ou outra thread já leu

                    }else if (err > 0 && err != sizeof(buff)) {

                        _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread::MainLoop][Error][WARNING] failed to read timer fd, Return val is wrong.", CL_FILE_LOG_AND_CONSOLE));

                        continue; // Error;
                    }

                    // Log
#ifdef _DEBUG
                    _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread::MainLoop][Log] Timer[FD=" 
                            + std::to_string(timer_tmp->fd) + "] expirou (" + std::to_string(buff) + ") vezes.", CL_FILE_LOG_AND_CONSOLE));
#endif

                    if (timer_tmp->flags & WT_EXECUTEINTIMERTHREAD) {

                        try {
                            
                            timer_tmp->callback(timer_tmp->lpParameter, 1/*TRUE*/);

                            // Check Deleting or one time expires
                            if (checkMarkToDelete(timer_tmp) || (timer_tmp->time_val.it_interval.tv_nsec == 0 && timer_tmp->time_val.it_interval.tv_sec == 0))
                                deleteTimer(timer_tmp);
                            else // Clear
                                clearExecuteTimerExpires(timer_tmp);

                            timer_tmp = nullptr;

                        }catch (exception& e) {
                            
                            _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread::MainLoop::CallBackExec][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
                        }
                    
                    }else { // Defualt

                        m_msgs.push(new msg_time{ timer_tmp });

                        if (m_msgs.size() > NUM_MSG_IN_WAIT_TO_MAKE_MORE_THREAD && m_msgs.size() < MAX_THREAD_POOL)
                            m_thread_pool.push_back(new thread(2, QueueTimer::_io_worker, (void*)this, 0, 0));

                        timer_tmp = nullptr;
                    }
                }

            }catch (exception& e) {

                _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread::MainLoop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
            }
        }

    }catch (exception& e) {

        _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }

    // Log
    _smp::message_pool::getInstance().push(new message("[QueueTimer::timer_thread][Log] Saindo da thread.", CL_FILE_LOG_AND_CONSOLE));

    return (void*)0;
}

void* QueueTimer::io_worker() {

    try {

        msg_time *msg_tmp = nullptr;

        // Log
        _smp::message_pool::getInstance().push(new message("[QueueTimer::io_worker][Log] I/O Worker inicializado.", CL_FILE_LOG_AND_CONSOLE));

        uint32_t check_active = 1u;

        while (__atomic_compare_exchange_n(&m_down_io_worker, &check_active, 1, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {

            try {

                msg_tmp = m_msgs.get(500); // Meio segundo

                if (msg_tmp == nullptr)
                    continue;

                try {

                    // Não verifica se está marcada, para ser deletada, executa depois verifica  
                    msg_tmp->te->callback(msg_tmp->te->lpParameter, 1/*TRUE*/);

                    // Check Deleting or one time expires
                    if (checkMarkToDelete(msg_tmp->te) || (msg_tmp->te->time_val.it_interval.tv_nsec == 0 && msg_tmp->te->time_val.it_interval.tv_sec == 0))
                        deleteTimer(msg_tmp->te);
                    else // clear
                        clearExecuteTimerExpires(msg_tmp->te);

                    msg_tmp->te = nullptr;

                }catch (exception& e) {
                    
                    _smp::message_pool::getInstance().push(new message("[QueueTimer::io_worker::MainLoop::CallBackExec][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
                }

                // Delete msg
                if (msg_tmp != nullptr)
                    delete msg_tmp;

                msg_tmp = nullptr;

            }catch (exception& e) {

                if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::LIST_FIFO, 2)) {

                    // Time out
                    if (m_thread_pool.size() > MIN_THREAD_POOL)
                        break; // Termina essa thread

                }else
                    _smp::message_pool::getInstance().push(new message("[QueueTimer::io_worker::MainLoop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
            }
        }

    }catch (exception& e) {

        _smp::message_pool::getInstance().push(new message("[QueueTimer::io_worker][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }

    // Log
    _smp::message_pool::getInstance().push(new message("[QueueTimer::io_worker][Log] saindo da thread.", CL_FILE_LOG_AND_CONSOLE));

    return (void*)0;
}

void QueueTimer::deleteTimer(timer_epoll* _timer) {

    try {

        if (_timer == nullptr)
            return;
                            
        pthread_mutex_lock(&m_cs);

        auto it = FIND_TIMER(m_timers, _timer);

        if (it != m_timers.end()) {

            deleteTimer(it);

            _timer = nullptr;
        }

        pthread_mutex_unlock(&m_cs);

    }catch (exception& e) {

        pthread_mutex_unlock(&m_cs);

        _smp::message_pool::getInstance().push(new message("[QueueTimer::deleteTimer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }
}

void QueueTimer::deleteTimer(std::vector< timer_epoll* >::iterator _it) {

    try {

        if (_it == m_timers.end() || (*_it) == nullptr)
            return;

        int32_t err = epoll_ctl(m_epollfd, EPOLL_CTL_DEL, (*_it)->fd, nullptr);

        if (err == -1)
            throw exception("[QueueTimer::deleteTimer][Error] Failed to call epoll_ctl to delete timerfd.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::QUEUETIMER, 50, errno));

        int32_t fd = (*_it)->fd;

        close((*_it)->fd);

        // Set event delete if it is not null
        if ((*_it)->event_delete != nullptr)
            (*_it)->event_delete->set(1u);

        delete (*_it);

        m_timers.erase(_it);

        // Log
        _smp::message_pool::getInstance().push(new message("[QueueTimer::deleteTimer][Log] deletou Timer[FD=" + std::to_string(fd) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

    }catch (exception& e) {

        _smp::message_pool::getInstance().push(new message("[QueueTimer::deleteTimer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }
}

uint32_t QueueTimer::setExecuteTimerExpires(timer_epoll* _timer) {

    try {

        // Invalid parameter
        if (_timer == nullptr)
            return eSET_EXEC::SE_INVALID_PARAMETER;

        // Lock
        pthread_mutex_lock(&m_cs);

        auto it = FIND_TIMER(m_timers, _timer);

        if (it == m_timers.end())
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_EXEC::SE_INVALID_PARAMETER);

        if ((*it)->deleting)
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_EXEC::SE_ESTA_DELETANDO);

        if ((*it)->running_callback)
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_EXEC::SE_ALREADY_EXECUTING);

        if ((*it)->toDelete)
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_EXEC::SE_DELETE_TIMER_NOW);

        (*it)->running_callback = 1u;

        // Unlock
        pthread_mutex_unlock(&m_cs);

    }catch (exception& e) {

        // Unlock
        pthread_mutex_unlock(&m_cs);

        _smp::message_pool::getInstance().push(new message("[QueueTImer::setExecuteTimerExpires][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

        return eSET_EXEC::SE_UNKNOWN_ERROR;
    }

    return eSET_EXEC::SE_OK;
}

uint32_t QueueTimer::clearExecuteTimerExpires(timer_epoll* _timer) {

    try {

        // Invalid parameter
        if (_timer == nullptr)
            return eSET_EXEC::SE_INVALID_PARAMETER;

        // Lock
        pthread_mutex_lock(&m_cs);

        auto it = FIND_TIMER(m_timers, _timer);

        if (it == m_timers.end())
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_EXEC::SE_INVALID_PARAMETER);

        if ((*it)->deleting)
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_EXEC::SE_ESTA_DELETANDO);

        if ((*it)->toDelete)
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_EXEC::SE_DELETE_TIMER_NOW);

        if (!(*it)->running_callback)
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_EXEC::SE_NOT_IN_EXECTION);

        (*it)->running_callback = 0u; // Disable

        // Unlock
        pthread_mutex_unlock(&m_cs);

    }catch (exception& e) {

        // Unlock
        pthread_mutex_unlock(&m_cs);

        _smp::message_pool::getInstance().push(new message("[QueueTimer::clearExecuteTimerExpires][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

        return eSET_EXEC::SE_UNKNOWN_ERROR;
    }

    return eSET_EXEC::SE_OK;
};

// Block(Sync)
uint32_t QueueTimer::setDeleteTimer(timer_epoll* _timer, Event* _event) {

    try {

        if (_timer == nullptr)
            return eSET_DEL::SD_INVALID_PARAMETER;

        pthread_mutex_lock(&m_cs);

        auto it = FIND_TIMER(m_timers, _timer);

        if (it == m_timers.end())
            RETURNV_AND_LEAVE_MUTEX(&m_cs, eSET_DEL::SD_INVALID_PARAMETER);

        RETURNV_AND_LEAVE_MUTEX(&m_cs, setDeleteTimer(it, _event));

        // !@ Não usa
        pthread_mutex_unlock(&m_cs);

    }catch (exception& e) {

        pthread_mutex_unlock(&m_cs);

        _smp::message_pool::getInstance().push(new message("[QueueTimer::setDeleteTimer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

        return eSET_DEL::SD_UNKNOWN_ERROR;
    }

    return eSET_DEL::SD_OK;
}

uint32_t QueueTimer::setDeleteTimer(std::vector< timer_epoll* >::iterator _it, Event* _event) {

    try {

        if (_it == m_timers.end() || (*_it) == nullptr)
            return eSET_DEL::SD_INVALID_PARAMETER;

        auto ptr = (*_it);

        // set event
        ptr->event_delete = _event; // notify

        if (ptr->running_callback > 0u) {

            ptr->toDelete = 1u;

            return eSET_DEL::SD_ESTA_EXECUTANDO; // Está executando
        }

        ptr->deleting = 1u;

        deleteTimer(_it);

        ptr = nullptr;

    }catch (exception& e) {

        _smp::message_pool::getInstance().push(new message("[QueueTimer::setDeleteTimer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

        return eSET_DEL::SD_UNKNOWN_ERROR;
    }

    return eSET_DEL::SD_OK;
}

bool QueueTimer::checkMarkToDelete(timer_epoll* _timer) {

    bool ret = false;

    try {

        if (_timer == nullptr)
            return false;

        // Lock
        pthread_mutex_lock(&m_cs);

        auto it = FIND_TIMER(m_timers, _timer);

        if (it == m_timers.end())
            RETURNV_AND_LEAVE_MUTEX(&m_cs, false);

        ret = (*it)->toDelete > 0u;

        // Unlock
        pthread_mutex_unlock(&m_cs);

    }catch (exception& e) {

        // Unlock
        pthread_mutex_unlock(&m_cs);

        ret = false;

        _smp::message_pool::getInstance().push(new message("[QueueTimer::checkMarkToDelete][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }

    return ret;
}