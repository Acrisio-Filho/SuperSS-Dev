// Arquivo string_util.hpp
// Criado em 11/11/2018 as 13:58 por Acrisio
// Update em 13/12/2018 as 21:42 por Acrisio
// Definição dos tipos e metôdos auxíliares de string

#pragma once
#ifndef _STDA_STRING_UTIL_HPP
#define _STDA_STRING_UTIL_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <wchar.h>
#include <errno.h>
#endif

#include <memory.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cctype>
#include <vector>

#include "exception.h"
#include "message_pool.h"

#if defined(_WIN32)
#define STRCPY_TO_MEMORY_FIXED_SIZE(_memory, _size, _str) \
{ \
	if ((_size) > 0 && (_str) != nullptr) { \
\
		if ((_str)[0] == '\0') { \
			(_memory)[0] = '\0'; \
		}else { \
			int32_t len = (int32_t)strlen((_str)); \
			len = (len > (_size) ? _size : len); \
			memcpy_s((_memory), (_size), (_str), len); \
		} \
	} \
}
#elif defined(__linux__)
#define STRCPY_TO_MEMORY_FIXED_SIZE(_memory, _size, _str) \
{ \
	if ((_size) > 0 && (_str) != nullptr) { \
\
		if ((_str)[0] == '\0') { \
			(_memory)[0] = '\0'; \
		}else { \
			int32_t len = (int32_t)strlen((_str)); \
			len = (len > (_size) ? _size : len); \
			memcpy((_memory), (_str), len); \
		} \
	} \
}
#endif

namespace stdA {
    
	// Return vector of to_string< typename >
	template< typename _Parameter, typename _Return > _Return to_string(_Parameter _parameter) {
        
        _Return s;

		for (auto it = _parameter.begin(); it != _parameter.end(); ++it) {
			
			if (it != _parameter.begin())
				s += _Return(", ");
			
			s += std::to_string(*it);
		}

        return s;
    };

	inline std::string getCharHex(unsigned char c, int opt = 0) {
		char ch[5];

#if defined(_WIN32)
		sprintf_s(ch, ((opt) ? "0x%02x" : "%02x"), (short)c);
#elif defined(__linux__)
		sprintf(ch, ((opt) ? "0x%02x" : "%02x"), (short)c);
#endif

		return std::string(ch);
	}

	inline std::string verifyAndTranslate(std::string _str, unsigned char _fixed = 0) {

		if (_str.empty())
			return std::string();

		std::string _new = _str;

		size_t pos = 0, len = 0;
		int32_t num = 0;

		try {

			while ((pos = _new.find("\\x", pos, 2)) != std::string::npos) {

				len = 0;

				while (isxdigit(_new[pos + 2 + len]))
					len++;

				if (_fixed != 0 && len > _fixed)
					len = _fixed;

				if (len > 0) {

					auto sub = _new.substr(pos + 2, len);

					num = strtol(sub.c_str(), nullptr, 16);

					if (errno == ERANGE)
						throw exception("[string_util][Error] of Range in strtol, got value: " + std::to_string(num), ERANGE);
				}

				_new.replace(pos, len + 2, 1, (char)num);

				//pos += len;
				pos++;
			}

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[string_util::verifyAndTranslate][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			_new = _str;
		}

		return _new;
	}

	inline std::string verifyAndEncode(std::string _str) {

		std::string _new;

		for (auto& el : _str) {

			if (isprint((unsigned char)el) || ispunct((unsigned char)el))
				_new.push_back(el);
			else
				_new += "\\x" + getCharHex((unsigned char)el);
		}

		return _new;
	}

	inline std::wstring MbToWc(std::string _str) {

		std::wstring ws;

		if (_str.empty())
			return ws;

#if defined(_WIN32)
		int size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _str.data(), (int)_str.length(), &ws[0], 0);

		if (size > 0) {

			ws.resize(size);

			if (!MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _str.data(), (int)_str.length(), &ws[0], size))
				throw exception("[string_util::MbToWc][Error] nao conseguiu transformar o byte em wide char com o code page CP_ACP.", GetLastError());

		}else
			throw exception("[string_util::MbToWc][Error] nao conseguiu pegar o size de conversao do mb para wide char com o code page CP_ACP.", GetLastError());
#elif defined(__linux__)
		size_t size = mbstowcs(nullptr, _str.c_str(), 0);

		if (size != (size_t)-1) {

			ws.resize(size);

			if (mbstowcs((wchar_t*)ws.data(), _str.c_str(), ws.length()) == (size_t)-1)
				throw exception("[string_util::MbToWc][Error] nao conseguiu transformar o byte em wide char com o code page LC_CTYPE.", errno);

		}else if (errno == EILSEQ) {

			// Converte a string malformada do mesmo jetio
			ws = std::wstring(_str.begin(), _str.end());

		}else
			throw exception("[string_util::MbToWc][Error] nao conseguiu pegar o size de conversao do mb para wide char com o code page LC_CTYPE.", errno);
#endif

		return ws;		
	};

	inline std::string WcToMb(std::wstring _wstr) {

		std::string s;

		if (_wstr.empty())
			return s;

#if defined(_WIN32)
		CPINFO cpi{0};
		BOOL usedDefaultChar = FALSE;

		if (!GetCPInfo(CP_ACP, &cpi))
			throw exception("[string_util::WcToMb][Error] nao conseguiu pegar o Code Page Info, do CP_ACP.", GetLastError());

		int size = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, _wstr.data(), (int)_wstr.length(), &s[0], 0, (LPCCH)cpi.DefaultChar, &usedDefaultChar);

		if (size > 0) {

			s.resize(size);

			if (!WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, _wstr.data(), (int)_wstr.length(), &s[0], size, (LPCCH)cpi.DefaultChar, (LPBOOL)&usedDefaultChar))
				throw exception("[string_util::WcToMb][Error] nao conseguiu transformar o wide char em byte com o code page CP_ACP.", GetLastError());

		}else
			throw exception("[string_util::WcToMb][Error] nao conseguiu pegar o size de conversao do wide char para mb com o code page CP_ACP.", GetLastError());
#elif defined(__linux__)
		size_t size = wcstombs(nullptr, _wstr.c_str(), 0);

		if (size != (size_t)-1) {

			s.resize(size);

			if (wcstombs((char*)s.data(), _wstr.c_str(), s.length()) == (size_t)-1)
				throw exception("[string_util::WcToMb][Error] nao conseguiu transformar o wide char em byte com o code page CP_ACP.", errno);

		}else if (errno == EILSEQ) {

			// Converte a string malformada do mesmo jetio
			s = std::string(_wstr.begin(), _wstr.end());

		}else
			throw exception("[string_util::WcToMb][Error] nao conseguiu pegar o size de conversao do wide char para mb com o code page CP_ACP.", errno);
#endif

		return s;
	};

	inline std::vector< std::string > split(std::string _str, std::string _delimiter) {

		std::vector< std::string > ret;

		size_t pos = std::string::npos;

		while ((pos = _str.find(_delimiter)) != std::string::npos) {

			ret.push_back(_str.substr(0, pos /*+ _delimiter.length() - Não pega o delimiter*/));

			_str.erase(0, pos + _delimiter.length()); // Mas tira ele
		}

		if (!_str.empty())
			ret.push_back(_str);

		return ret;
	};

	inline std::string clearBreakLineAndTab(std::string _str, char _value = -1) {

		size_t pos = 0u;

		while ((pos = _str.find('\r')) != std::string::npos)
			_str.erase(pos);

		pos = 0u;

		while ((pos = _str.find('\t', pos)) != std::string::npos)
			_str.replace(pos, 1, 3, ' ');

		pos = 0u;

		if (_value != -1)
			while ((pos = _str.find(_value)) != std::string::npos)
				_str.erase(pos);

		return _str;
	};

	inline std::vector< std::string > limit_chat_display(std::vector< std::string > _lines, uint32_t _limit) {

		std::vector< std::string > ret;

		for (auto& el : _lines) {

			if (el.length() <= _limit) {

				auto tst = clearBreakLineAndTab(el, '\n'/*New Line*/);

				if (!tst.empty())
					ret.push_back(tst);

			}else {

				while (el.length() > _limit) {

					size_t pos = el.find_last_of(' ', _limit);

					ret.push_back(el.substr(0, (pos != std::string::npos) ? pos : _limit));

					el.erase(0, (pos != std::string::npos) ? pos : _limit);
				}

				if (el.length() > 0)
					ret.push_back(el);
			}
		}

		return ret;
	};

	inline std::string join(std::vector< std::string > _vec, std::string _join = ",") {

		std::string ret = "";

		if (_vec.empty())
			return ret;

		for (auto it = _vec.begin(); it != _vec.end(); ++it) {
			
			if (it != _vec.begin())
				ret += _join;

			ret += *it;
		}

		return ret;
	};

	inline std::string toLowerCase(std::string _str) {

		std::string ret = "";

		if (_str.empty())
			return ret;

		for (auto& el : _str)
			ret.push_back(std::tolower(el));

		return ret;
	};

	inline std::string toUpperCase(std::string _str) {

		std::string ret = "";

		if (_str.empty())
			return ret;

		for (auto& el : _str)
			ret.push_back(std::toupper(el));

		return ret;
	};

	template<typename _Digit> std::string toFixed(_Digit _value, uint32_t _fixed) {

		std::stringstream ss;

		ss.precision(_fixed);

		ss << std::fixed << _value;

		return ss.str();
	}

}

#endif
