// Arquivo WinPort.h
// Criado em 10/03/2021 as 09:56 por Acrisio
// Porte dos utilies do Windows para Linux

#pragma once
#ifndef _STDA_WIN_PORT_H
#define _STDA_WIN_PORT_H

#include <cstdint>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

// Pack size 1
#pragma pack(1)

#ifndef SOCKET
    typedef struct {
        uint32_t fd;
        timespec connect_time;
    } /*__attribute__ ((__packed__))*/ SOCKET;
#endif

#ifndef INVALID_SOCKET
    #define INVALID_SOCKET (uint32_t)-1
#endif

#ifndef SOCKADDR_IN
    #define SOCKADDR_IN sockaddr_in
#endif

#ifndef SYSTEMTIME

    struct _SYSTEMTIME {
        u_short wYear;
        u_short wMonth;
        u_short wDayOfWeek;
        u_short wDay;
        u_short wHour;
        u_short wMinute;
        u_short wSecond;
        u_short wMilliseconds;
    };

    #define SYSTEMTIME _SYSTEMTIME
#endif

#ifndef FILETIME

    struct _FILETIME {
        uint32_t dwLowDateTime;
        uint32_t dwHighDateTime;
    };

    #define FILETIME _FILETIME
#endif

#ifndef LONGLONG
    #define LONGLONG int64_t
#endif

#ifndef ULONGLONG
    #define ULONGLONG uint64_t
#endif

#ifndef DWORD
    #define DWORD uint32_t
#endif

#ifndef LONG
    #define LONG int32_t
#endif

#ifndef BOOL
    #define BOOL int32_t
#endif

#ifndef _mCHAR
    #define _mCHAR
    typedef char CHAR;
#endif

#ifndef _mUCHAR
    #define _mUCHAR
    typedef unsigned char UCHAR;
#endif

#ifndef _mWCHAR
    #define _mWCHAR
    typedef unsigned short WCHAR;
#endif

#ifndef __int16
    #define __int16 short
#endif

#ifndef __int8
    #define __int8 char
#endif

#ifndef UNREFERENCED_PARAMETER 
    #define UNREFERENCED_PARAMETER(x) (x); 
#endif

#ifndef INFINITE
    #define INFINITE -1
#endif

#ifndef ERROR_SUCCESS
    #define ERROR_SUCCESS 0
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef LPVOID
    #define LPVOID void*
#endif

#ifndef UINT32
    #define UINT32 uint32_t
#endif

#ifndef CALLBACK
    #define CALLBACK // __stdcall
#endif

#ifndef WINAPI
    #define WINAPI // __stdcall
#endif

#ifndef INVALID_HANDLE_VALUE
    #define INVALID_HANDLE_VALUE -1
#endif

inline timespec _milliseconds_to_timespec_clock_realtime(DWORD _dwMilliseconds) {

    timespec ts{ 0u };

    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        return ts; // Fail

    uint64_t nano = (ts.tv_sec * 1000000000 + ts.tv_nsec) + (_dwMilliseconds * 1000000);

    return { (time_t)(nano / 1000000000), (int)(nano % 1000000000) };
}

#define MILLISECONDS_TO_TIMESPEC(_dwMilliseconds) { (time_t)((_dwMilliseconds) / 1000), (int)((_dwMilliseconds) % 1000 * 1000000) }

// DIFF TICK / milliseconds
#define TIMESPEC_TO_NANO_UI64(_timespec) (uint64_t)((uint64_t)(_timespec).tv_sec * (uint64_t)1000000000 + (uint64_t)(_timespec).tv_nsec)
#define DIFF_TICK(a, b, c) (int64_t)(((int64_t)(TIMESPEC_TO_NANO_UI64((a)) - TIMESPEC_TO_NANO_UI64((b))) / TIMESPEC_TO_NANO_UI64((c))) / 1000000)
#define DIFF_TICK_MICRO(a, b, c) (int64_t)(((int64_t)(TIMESPEC_TO_NANO_UI64((a)) - TIMESPEC_TO_NANO_UI64((b))) / TIMESPEC_TO_NANO_UI64((c))) / 1000)

#ifndef LARGE_INTEGER

    union _LARGE_INTEGER {
        struct {
            DWORD LowPart;
            LONG  HighPart;
        } DUMMYSTRUCTNAME;
        struct {
            DWORD LowPart;
            LONG  HighPart;
        } u;
        LONGLONG QuadPart;
    };

    #define LARGE_INTEGER _LARGE_INTEGER
#endif

#ifndef ULARGE_INTEGER

    union _ULARGE_INTEGER {
        struct {
            DWORD LowPart;
            DWORD HighPart;
        } DUMMYSTRUCTNAME;
        struct {
            DWORD LowPart;
            DWORD HighPart;
        } u;
        ULONGLONG QuadPart;
    };

    #define ULARGE_INTEGER _ULARGE_INTEGER
#endif

#ifndef TIME_ZONE_INFORMATION

    struct _TIME_ZONE_INFORMATION {
        LONG       Bias;
        WCHAR      StandardName[32];
        SYSTEMTIME StandardDate;
        LONG       StandardBias;
        WCHAR      DaylightName[32];
        SYSTEMTIME DaylightDate;
        LONG       DaylightBias;
    };

    #define TIME_ZONE_INFORMATION _TIME_ZONE_INFORMATION
#endif

#ifndef WSABUF

    typedef struct _WSABUF {
        uint32_t len;
        CHAR* buf;
    } WSABUF;

    typedef WSABUF *LPWSABUF;
#endif

#ifndef SD_BOTH
    #define SD_BOTH SHUT_RDWR
#endif

#ifndef SD_RECEIVE
    #define SD_RECEIVE SHUT_RD
#endif

#ifndef closesocket
    #define closesocket(_socket) close((_socket))
#endif

#ifndef SOCKET_ERROR
    #define SOCKET_ERROR (-1)
#endif

#ifndef Int32x32To64
    #define Int32x32To64(a, b) (int64_t)((int64_t)(a) * (int64_t)(b))
#endif

void GetSystemTime(SYSTEMTIME* _st);
void GetLocalTime(SYSTEMTIME* _st);
void FileTimeToSystemTime(const FILETIME* _ft, SYSTEMTIME* _st);
void SystemTimeToFileTime(const SYSTEMTIME* _st, FILETIME* _ft);
BOOL TzSpecificLocalTimeToSystemTime(const TIME_ZONE_INFORMATION* _tz, const SYSTEMTIME* _st_local, SYSTEMTIME* _st);
BOOL SystemTimeToTzSpecificLocalTime(const TIME_ZONE_INFORMATION* _tz, const SYSTEMTIME* _st, SYSTEMTIME* _st_local);
DWORD GetTimeZoneInformation(TIME_ZONE_INFORMATION* _tz);

void ZeroMemory(void* _mem, DWORD _size);

// Init pthread_mutexattr_t RECURSIVE
#define INIT_PTHREAD_MUTEXATTR_RECURSIVE \
    pthread_mutexattr_t __mattr_0; \
	pthread_mutexattr_init(&__mattr_0); \
	pthread_mutexattr_settype(&__mattr_0, PTHREAD_MUTEX_RECURSIVE); \

// Init pthread_mutex_t
#define INIT_PTHREAD_MUTEX_RECURSIVE(_pthread_mutex) pthread_mutex_init((_pthread_mutex), &__mattr_0);

// Destroy pthread_mutexattr_t
#define DESTROY_PTHREAD_MUTEXATTR_RECURSIVE pthread_mutexattr_destroy(&__mattr_0);

#ifndef LPTHREAD_START_ROUTINE
    typedef void* (*_LPTHREAD_START_ROUTINE)(void* _lpParameter);

    #define LPTHREAD_START_ROUTINE _LPTHREAD_START_ROUTINE
#endif

#ifndef THREAD_PRIORITY_ABOVE_NORMAL
    #define THREAD_PRIORITY_ABOVE_NORMAL (int32_t)1
#endif

#ifndef THREAD_PRIORITY_BELOW_NORMAL
    #define THREAD_PRIORITY_BELOW_NORMAL (int32_t)-1
#endif

#ifndef THREAD_PRIORITY_HIGHEST
    #define THREAD_PRIORITY_HIGHEST (int32_t)2
#endif

#ifndef THREAD_PRIORITY_IDLE
    #define THREAD_PRIORITY_IDLE (int32_t)-15
#endif

#ifndef THREAD_PRIORITY_LOWEST
    #define THREAD_PRIORITY_LOWEST (int32_t)-2
#endif

#ifndef THREAD_PRIORITY_NORMAL
    #define THREAD_PRIORITY_NORMAL (int32_t)0
#endif

#ifndef THREAD_PRIORITY_TIME_CRITICAL
    #define THREAD_PRIORITY_TIME_CRITICAL (int32_t)15
#endif

// Pack size default
#pragma pack()

#endif // !_STDA_WIN_PORT_H