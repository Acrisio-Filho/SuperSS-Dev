// Arquivo util_time.h
// Criado em 19/08/2017 por Acrisio
// Definição de algumas funções uteis para manipulação de tempo e data

#pragma once
#ifndef _STDA_UTIL_TIME_H
#define _STDA_UTIL_TIME_H

#include <cstdint>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include "WinPort.h"
#endif

#include <string>

#define STDA_10_MICRO_PER_MICRO		(10ll)
#define STDA_10_MICRO_PER_MILLI		(STDA_10_MICRO_PER_MICRO * 1000ll)
#define STDA_10_MICRO_PER_SEC		(STDA_10_MICRO_PER_MILLI * 1000ll)
#define STDA_10_MICRO_PER_MIN		(STDA_10_MICRO_PER_SEC * 60ll)
#define STDA_10_MICRO_PER_HOUR		(STDA_10_MICRO_PER_MIN * 60ll)
#define STDA_10_MICRO_PER_DAY		(STDA_10_MICRO_PER_HOUR * 24ll)

namespace stdA {

	// Translate Date from string to Windows System Time format
    int _translateDate(std::string date_src, SYSTEMTIME* date_dst);

	// Translate Time from string to Windows System Time format
	int _translateTime(std::string _date_src, SYSTEMTIME* _date_dst);

	// Translate Date from Unix seconds to Windows System Time format
	int translateDateSystem(time_t time_unix, SYSTEMTIME* date_dst);
	int translateDateLocal(time_t time_unix, SYSTEMTIME* date_dst);

	// Formate Date Windows System Time to String Time extended
	std::string _formatDate(SYSTEMTIME& _date);

	// Formate Date Windows System Time to String Time extended
	std::string _formatTime(SYSTEMTIME& _date);

	// Formate Date Unix seconds format to String Time extended
	std::string formatDateSystem(time_t _time_unix);
	std::string formatDateLocal(time_t _time_unix);

	// Time Zone Conversions, Local To UTC
	time_t TzLocalUnixToUnixUTC(time_t _time_unix);
	SYSTEMTIME TzLocalUnixToSystemTime(time_t _time_unix);

	time_t TzLocalTimeToUnixUTC(SYSTEMTIME& _st);
	SYSTEMTIME TzLocalTimeToSystemTime(SYSTEMTIME& _st);

	// Time Zone Conversions, UTC to Local
	time_t UnixUTCToTzLocalUnix(time_t _time_unix);
	SYSTEMTIME UnixUTCToTzLocalTime(time_t _time_unix);

	time_t SystemTimeToTzLocalUnix(SYSTEMTIME& _st);
	SYSTEMTIME SystemTimeToTzLocalTime(SYSTEMTIME& _st);
	
	// Conversion stardand from type to other only
	time_t FileTimeToUnix(FILETIME ft);
	FILETIME UnixToFileTime(time_t time);

	time_t SystemTimeToUnix(SYSTEMTIME st);
	SYSTEMTIME UnixToSystemTime(time_t time);

	// Get Local And System(UTC) time from Windows format to Unix format
	time_t GetLocalTimeAsUnix();
	time_t GetSystemTimeAsUnix();

	// String date time to Unix second format
	time_t StrToUnix(std::string _date_time);

	// Diff Time(Day, Hour, Min, Sec, Millsec) System date windows em milliseconds 
	int64_t getHourDiff(SYSTEMTIME& _st1, SYSTEMTIME& _st2);

	// Diff System date windows do micro-seconds
	int64_t getTimeDiff(SYSTEMTIME& _st1, SYSTEMTIME& _st2);

	// Diff Local Time ASC(ascendente)
	int64_t getLocalTimeDiff(SYSTEMTIME& _st);

	// Diff Local Time DESC(descendente)
	int64_t getLocalTimeDiffDESC(SYSTEMTIME& _st);

	// Diff System(UTC) Time ASC(ascendente)
	int64_t getSystemTimeDiff(SYSTEMTIME& _st);
	
	// Diff System(UTC) Time DESC(descendente)
	int64_t getSystemTimeDiffDESC(SYSTEMTIME& _st);

	// Diff System date windows do micro-seconds, (Só a data)
	int64_t getDateDiff(SYSTEMTIME& _st1, SYSTEMTIME& _st2);

	// Diff Local Date ASC(ascendente)
	int64_t getLocalDateDiff(SYSTEMTIME& _st);

	// Diff Local Date DESC(descendente)
	int64_t getLocalDateDiffDESC(SYSTEMTIME& _st);

	// Diff System(UTC) Date ASC(ascendente)
	int64_t getSystemDateDiff(SYSTEMTIME& _st);

	// Diff System(UTC) Date DESC(descendente)
	int64_t getSystemDateDiffDESC(SYSTEMTIME& _st);

	bool isSameDay(SYSTEMTIME& _st1, SYSTEMTIME& _st2);

	bool isSameDayNow(SYSTEMTIME& _st);

	// Check is System Time Windows is empty
	bool isEmpty(SYSTEMTIME& _st);
}

#endif