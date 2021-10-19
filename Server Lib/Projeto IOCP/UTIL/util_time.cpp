// Arquivo util_time.cpp
// Criado em 19/08/2017 por Acrisio
// Implementação das funções de auxilo para tempo e data

#include "util_time.h"
#include "exception.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

int stdA::_translateDate(std::string date_src, SYSTEMTIME* date_dst) {
	
	if (date_dst == nullptr)
		throw exception("Erro date_dst e nullptr. translateDate()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UTIL_TIME, 2, 0));
	
	if (date_src.empty() || date_src[0] == '\0')
		*date_dst = { 0 };	// Zera o SYSTEMTIME
		//throw exception("Erro date_src esta vazio. translateDate()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UTIL_TIME, 1, 0));

#if defined(_WIN32)
	sscanf_s(date_src.c_str(), "%hd-%hd-%hd %hd:%hd:%hd", 
#elif defined(__linux__)
	sscanf(date_src.c_str(), "%hd-%hd-%hd %hd:%hd:%hd", 
#endif
		&date_dst->wYear, &date_dst->wMonth, &date_dst->wDay,
		&date_dst->wHour, &date_dst->wMinute, &date_dst->wSecond);

    return 0;
};

int stdA::_translateTime(std::string _date_src, SYSTEMTIME* _date_dst) {
	
	if (_date_dst == nullptr)
		throw exception("Erro _date_dst e nullptr. _translateTime()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UTIL_TIME, 2, 0));

	if (_date_src.empty() || _date_src[0] == '\0')
		*_date_dst = { 0 };	// Zera o SYSTEMTIME
		//throw exception("Erro date_src esta vazio. translateDate()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UTIL_TIME, 1, 0));

#if defined(_WIN32)
	sscanf_s(_date_src.c_str(), "%hd:%hd:%hd.%hd",
#elif defined(__linux__)
	sscanf(_date_src.c_str(), "%hd:%hd:%hd.%hd",
#endif
		&_date_dst->wHour, &_date_dst->wMinute, &_date_dst->wSecond, &_date_dst->wMilliseconds);

	return 0;
};

int stdA::translateDateSystem(time_t time_unix, SYSTEMTIME* date_dst) {
	
	if (date_dst == nullptr)
		throw exception("[stdA::translateDateSystem][Error] date_dst is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UTIL_TIME, 2, 0));

	if (time_unix == 0) {	// Data atual do sistema no UTC
		
		GetSystemTime(date_dst);
	
	}else {
		FILETIME ft = UnixToFileTime(time_unix);

		FileTimeToSystemTime(&ft, date_dst);
	}

	return 0;
};

int stdA::translateDateLocal(time_t time_unix, SYSTEMTIME* date_dst) {
	
	if (date_dst == nullptr)
		throw exception("[stdA::translateDateLocal][Error] date_dst is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UTIL_TIME, 2, 0));

	if (time_unix == 0) {	// Data atual do sistema no local da máquina
		
		GetLocalTime(date_dst);
	
	}else {
		FILETIME ft = UnixToFileTime(time_unix);

		FileTimeToSystemTime(&ft, date_dst);
	}

	return 0;
};

std::string stdA::_formatDate(SYSTEMTIME& _date) {
	
	std::string s;
	char tmp[100];

#if defined(_WIN32)
	sprintf_s(tmp, "%.04u-%.02u-%.02u %.02u:%.02u:%.02u.%.03u",
#elif defined(__linux__)
	sprintf(tmp, "%.04u-%.02u-%.02u %.02u:%.02u:%.02u.%.03u",
#endif
			_date.wYear, _date.wMonth, _date.wDay,
			_date.wHour, _date.wMinute, _date.wSecond, _date.wMilliseconds);

	return s.assign(tmp);
};

std::string stdA::_formatTime(SYSTEMTIME& _date) {
	
	std::string s;
	char tmp[100];

#if defined(_WIN32)
	sprintf_s(tmp, "%.02u:%.02u:%.02u.%.03u",
#elif defined(__linux__)
	sprintf(tmp, "%.02u:%.02u:%.02u.%.03u",
#endif
		_date.wHour, _date.wMinute, _date.wSecond, _date.wMilliseconds);

	return s.assign(tmp);
};

std::string stdA::formatDateSystem(time_t _time_unix) {
	
	std::string s;
	char tmp[100];

	SYSTEMTIME date = { 0 };

	translateDateSystem(_time_unix, &date);

#if defined(_WIN32)
	sprintf_s(tmp, "%.04u-%.02u-%.02u %.02u:%.02u:%.02u.%.03u",
#elif defined(__linux__)
	sprintf(tmp, "%.04u-%.02u-%.02u %.02u:%.02u:%.02u.%.03u",
#endif
			date.wYear, date.wMonth, date.wDay,
			date.wHour, date.wMinute, date.wSecond, date.wMilliseconds);

	return s.assign(tmp);
};

std::string stdA::formatDateLocal(time_t _time_unix) {
	
	std::string s;
	char tmp[100];

	SYSTEMTIME date = { 0 };

	translateDateLocal(_time_unix, &date);

#if defined(_WIN32)
	sprintf_s(tmp, "%.04u-%.02u-%.02u %.02u:%.02u:%.02u.%.03u",
#elif defined(__linux__)
	sprintf(tmp, "%.04u-%.02u-%.02u %.02u:%.02u:%.02u.%.03u",
#endif
			date.wYear, date.wMonth, date.wDay,
			date.wHour, date.wMinute, date.wSecond, date.wMilliseconds);

	return s.assign(tmp);
};

time_t stdA::TzLocalUnixToUnixUTC(time_t _time_unix) {
	return SystemTimeToUnix(TzLocalUnixToSystemTime(_time_unix));
};

inline SYSTEMTIME stdA::TzLocalUnixToSystemTime(time_t _time_unix) {
	
	bool error = false;

	SYSTEMTIME st = UnixToSystemTime(_time_unix);

	SYSTEMTIME tzUTC{ 0 };

	if (TzSpecificLocalTimeToSystemTime(NULL/*Current Time Zone(Local)*/, &st, &tzUTC) == 0)
		error = true;	// Por hora só para ter, não utiliza essa variável para nada ainda

	return tzUTC;
};

time_t stdA::TzLocalTimeToUnixUTC(SYSTEMTIME& _st) {
	return SystemTimeToUnix(TzLocalTimeToSystemTime(_st));
};

inline SYSTEMTIME stdA::TzLocalTimeToSystemTime(SYSTEMTIME& _st) {
	
	bool error = false;

	SYSTEMTIME tzUTC{ 0 };

	if (TzSpecificLocalTimeToSystemTime(NULL/*Current Time Zone(Local)*/, &_st, &tzUTC) == 0)
		error = true;	// Por hora só para ter, não utiliza essa variável para nada ainda

	return tzUTC;
};

time_t stdA::UnixUTCToTzLocalUnix(time_t _time_unix) {
	return SystemTimeToUnix(UnixUTCToTzLocalTime(_time_unix));
};

SYSTEMTIME stdA::UnixUTCToTzLocalTime(time_t _time_unix) {
	
	bool error = false;

	SYSTEMTIME utc = UnixToSystemTime(_time_unix);

	SYSTEMTIME tzLocal{ 0 };

	if (SystemTimeToTzSpecificLocalTime(NULL/*Current Time Zone(Local)*/, &utc, &tzLocal) == 0)
		error = true;	// Por hora só para ter, não utiliza essa variável para nada ainda

	return tzLocal;
};

time_t stdA::SystemTimeToTzLocalUnix(SYSTEMTIME& _st) {
	return SystemTimeToUnix(SystemTimeToTzLocalTime(_st));
};

inline SYSTEMTIME stdA::SystemTimeToTzLocalTime(SYSTEMTIME& _st) {
	
	bool error = false;

	SYSTEMTIME tzLocal{ 0 };

	if (SystemTimeToTzSpecificLocalTime(NULL/*Current Time Zone(Local)*/, &_st, &tzLocal) == 0)
		error = true;	// Por hora só para ter, não utiliza essa variável para nada ainda

	return tzLocal;
};

time_t stdA::FileTimeToUnix(FILETIME ft) {
	return (((((int64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000000) - 11644473600ll);
};

FILETIME stdA::UnixToFileTime(time_t time) {
	// Agora o meu está certo, era o valor 1644473600ll que faltava 1 é 11644473600ll a constante de conversão de segundos microsoft para unix
	//int64_t ll = (time * 10000000) + 11644473600ll;
	//return {(DWORD)ll, (DWORD)(ll >> 32)};

	// EXEMPLE MICROSOFT
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;
	FILETIME ft{ 0 };

	ll = Int32x32To64(time, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = ll >> 32;
	
	return ft;
};

time_t stdA::SystemTimeToUnix(SYSTEMTIME st) {
	
	FILETIME ft{ 0 };

	SystemTimeToFileTime(&st, &ft);

	return FileTimeToUnix(ft);
};

SYSTEMTIME stdA::UnixToSystemTime(time_t time) {

	SYSTEMTIME st{ 0 };
	
	FILETIME ft = UnixToFileTime(time);

	FileTimeToSystemTime(&ft, &st);

	return st;
};

time_t stdA::GetLocalTimeAsUnix() {

	SYSTEMTIME st{ 0 };

	GetLocalTime(&st);

	return SystemTimeToUnix(st);
};

time_t stdA::GetSystemTimeAsUnix() {
	
	SYSTEMTIME st{ 0 };

	GetSystemTime(&st);

	return SystemTimeToUnix(st);
};

time_t stdA::StrToUnix(std::string _date_time) {

	SYSTEMTIME st{ 0 };
	FILETIME ft{ 0 };

	_translateDate(_date_time, &st);

	SystemTimeToFileTime(&st, &ft);

	return FileTimeToUnix(ft);
};

int64_t stdA::getHourDiff(SYSTEMTIME& _st1, SYSTEMTIME& _st2) {
	return ((((((((_st1.wDay - _st2.wDay) * 24/*Hour*/) + (_st1.wHour - _st2.wHour)) * 60/*min*/) + (_st1.wMinute - _st2.wMinute)) * 60/*sec*/) + (_st1.wSecond - _st2.wSecond)) * 1000/*Millsec*/) + (_st1.wMilliseconds - _st2.wMilliseconds);
};

int64_t stdA::getTimeDiff(SYSTEMTIME& _st1, SYSTEMTIME& _st2) {

	FILETIME ft1{ 0 }, ft2{ 0 };

	SystemTimeToFileTime(&_st1, &ft1);
	SystemTimeToFileTime(&_st2, &ft2);

	return (reinterpret_cast<LARGE_INTEGER*>(&ft1)->QuadPart - reinterpret_cast<LARGE_INTEGER*>(&ft2)->QuadPart);
};

int64_t stdA::getLocalTimeDiff(SYSTEMTIME& _st) {

	SYSTEMTIME local{ 0 };

	GetLocalTime(&local);

	return getTimeDiff(local, _st);
};

int64_t stdA::getLocalTimeDiffDESC(SYSTEMTIME& _st) {
	
	SYSTEMTIME local{ 0 };

	GetLocalTime(&local);

	return getTimeDiff(_st, local);
};

int64_t stdA::getSystemTimeDiff(SYSTEMTIME& _st) {

	SYSTEMTIME system{ 0 };

	GetSystemTime(&system);

	return getTimeDiff(system, _st);
};

int64_t stdA::getSystemTimeDiffDESC(SYSTEMTIME& _st) {

	SYSTEMTIME system{ 0 };

	GetSystemTime(&system);

	return getTimeDiff(_st, system);
}

int64_t stdA::getDateDiff(SYSTEMTIME& _st1, SYSTEMTIME& _st2) {

	// Apenas data a diferença
	_st1.wHour = _st1.wMinute = _st1.wSecond = _st1.wMilliseconds = 0u;
	_st2.wHour = _st2.wMinute = _st2.wSecond = _st2.wMilliseconds = 0u;

	return getTimeDiff(_st1, _st2);
}

int64_t stdA::getLocalDateDiff(SYSTEMTIME& _st) {

	SYSTEMTIME local{ 0 };

	GetLocalTime(&local);

	return getDateDiff(local, _st);
}

int64_t stdA::getLocalDateDiffDESC(SYSTEMTIME& _st) {

	SYSTEMTIME local{ 0 };

	GetLocalTime(&local);

	return getDateDiff(_st, local);
}

int64_t stdA::getSystemDateDiff(SYSTEMTIME& _st) {

	SYSTEMTIME system{ 0 };

	GetSystemTime(&system);

	return getDateDiff(system, _st);
}

int64_t stdA::getSystemDateDiffDESC(SYSTEMTIME& _st) {

	SYSTEMTIME system{ 0 };

	GetSystemTime(&system);

	return getDateDiff(_st, system);
}

bool stdA::isSameDay(SYSTEMTIME& _st1, SYSTEMTIME& _st2) {
	return (_st1.wYear == _st2.wYear && _st1.wMonth == _st2.wMonth && _st1.wDay == _st2.wDay);
};

bool stdA::isSameDayNow(SYSTEMTIME& _st) {
	
	SYSTEMTIME st{ 0u };

	GetLocalTime(&st);

	return isSameDay(_st, st);
}

bool stdA::isEmpty(SYSTEMTIME& _st) {
	return (_st.wYear == 0 && _st.wMonth == 0 && _st.wDay == 0 && _st.wHour == 0 && _st.wMinute == 0 && _st.wSecond == 0);
};
