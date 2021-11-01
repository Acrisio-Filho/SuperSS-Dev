// Arquivo WinPort.cpp
// Criado em 10/03/2021 as 11:18 por Acrisio
// Porte dos utilies do Windows para Linux

#include "WinPort.h"
#include <sys/time.h>
#include <ctime>
#include <memory.h>

void GetSystemTime(SYSTEMTIME* _st) {

    if (_st == nullptr)
        return;

    timespec ts{ 0u };

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {

        memset(_st, 0, sizeof(SYSTEMTIME));

        return;
    }

    tm _tm{ 0u };

    if (gmtime_r(&ts.tv_sec, &_tm) == nullptr) {

        memset(_st, 0, sizeof(SYSTEMTIME));

        return;
    }

    _st->wYear = _tm.tm_year + 1900;
    _st->wMonth = _tm.tm_mon + 1;
    _st->wDay = _tm.tm_mday;
    _st->wDayOfWeek = _tm.tm_wday;
    _st->wHour = _tm.tm_hour;
    _st->wMinute = _tm.tm_min;
    _st->wSecond = _tm.tm_sec;
    _st->wMilliseconds = (u_short)(ts.tv_nsec / 1000000);
}

void GetLocalTime(SYSTEMTIME* _st) {

    if (_st == nullptr)
        return;

    timespec ts{ 0u };

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {

        memset(_st, 0, sizeof(SYSTEMTIME));

        return;
    }

    tm _tm{ 0u };

    if (localtime_r(&ts.tv_sec, &_tm) == nullptr) {

        memset(_st, 0, sizeof(SYSTEMTIME));

        return;
    }

    _st->wYear = _tm.tm_year + 1900;
    _st->wMonth = _tm.tm_mon + 1;
    _st->wDay = _tm.tm_mday;
    _st->wDayOfWeek = _tm.tm_wday;
    _st->wHour = _tm.tm_hour;
    _st->wMinute = _tm.tm_min;
    _st->wSecond = _tm.tm_sec;
    _st->wMilliseconds = (u_short)(ts.tv_nsec / 1000000);
}

void FileTimeToSystemTime(const FILETIME* _ft, SYSTEMTIME* _st) {

    if (_ft == nullptr || _st == nullptr)
        return;

    int64_t unix_time_milli = (reinterpret_cast<LARGE_INTEGER*>((FILETIME*)_ft)->QuadPart - 116444736000000000) / 10000;
    
    time_t sec = unix_time_milli / 1000;

    tm _tm{ 0u };

    if (gmtime_r(&sec, &_tm) == nullptr) {

        memset(_st, 0, sizeof(SYSTEMTIME));

        return;
    }

    _st->wYear = _tm.tm_year + 1900;
    _st->wMonth = _tm.tm_mon + 1;
    _st->wDay = _tm.tm_mday;
    _st->wDayOfWeek = _tm.tm_wday;
    _st->wHour = _tm.tm_hour;
    _st->wMinute = _tm.tm_min;
    _st->wSecond = _tm.tm_sec;
    _st->wMilliseconds = (u_short)(unix_time_milli % 1000);
}

void SystemTimeToFileTime(const SYSTEMTIME* _st, FILETIME* _ft) {

    if (_st == nullptr || _ft == nullptr)
        return;

    tm _tm{ 0u };

    _tm.tm_year = _st->wYear - 1900;
    _tm.tm_mon = _st->wMonth - 1;
    _tm.tm_wday = _st->wDayOfWeek;
    _tm.tm_mday = _st->wDay;
    _tm.tm_hour = _st->wHour;
    _tm.tm_min = _st->wMinute;
    _tm.tm_sec = _st->wSecond;

    time_t unix_sec = timegm(&_tm);

    if (unix_sec == (time_t)-1) {

        memset(&_ft, 0, sizeof(_ft));

        return;
    }

    reinterpret_cast<LARGE_INTEGER*>(_ft)->QuadPart = Int32x32To64(unix_sec, 10000000) + 116444736000000000 + (_st->wMilliseconds * 10000);
}

BOOL TzSpecificLocalTimeToSystemTime(const TIME_ZONE_INFORMATION* _tz, const SYSTEMTIME* _st_local, SYSTEMTIME* _st) {

    if (_st_local == nullptr || _st == nullptr)
        return -1; // Fail

    int64_t _10micro_tz = 0;

    if (_tz == nullptr) {

        // Current Time Zone Local
        _10micro_tz = __timezone * 10000000;        // seconds to 10 microseconds

    }else
        _10micro_tz = _tz->Bias * 60 * 10000000;    // Minutos to 10 microseconds

    FILETIME ft_local{ 0u };

    SystemTimeToFileTime(_st_local, &ft_local);

    reinterpret_cast<LARGE_INTEGER*>(&ft_local)->QuadPart += _10micro_tz;

    FileTimeToSystemTime(&ft_local, _st);

    return 0;
}

BOOL SystemTimeToTzSpecificLocalTime(const TIME_ZONE_INFORMATION* _tz, const SYSTEMTIME* _st, SYSTEMTIME* _st_local) {

    if (_st_local == nullptr || _st == nullptr)
        return -1; // Fail

    int64_t _10micro_tz = 0;

    if (_tz == nullptr) {

        // Current Time Zone Local
        _10micro_tz = __timezone * 10000000;        // seconds to 10 microseconds
    
    }else
        _10micro_tz = _tz->Bias * 60 * 10000000;    // Minutos to 10 microseconds

    FILETIME ft{ 0u };

    SystemTimeToFileTime(_st, &ft);

    reinterpret_cast<LARGE_INTEGER*>(&ft)->QuadPart -= _10micro_tz;

    FileTimeToSystemTime(&ft, _st_local);

    return 0;
}

DWORD GetTimeZoneInformation(TIME_ZONE_INFORMATION* _tz) {

    if (_tz == nullptr)
        return (DWORD)-1;

    _tz->Bias = __timezone / 60; // Minutes

    return 0;
}

void ZeroMemory(void* _mem, DWORD _size) {

    if (_mem != nullptr)
        memset(_mem, 0, _size);
}