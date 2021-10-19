// Arquivo type_util.hpp
// Criado em 21/11/2020 as 19:51 por Acrisio
// Defini��o de type util tools

#pragma once
#ifndef _STDA_TYPE_UTIL_HPP
#define _STDA_TYPE_UTIL_HPP

#include <regex>
#include <string>
#include <string_view>
#include <map>

namespace stdA {

	inline bool isNaN(std::string _s) {
		return !std::regex_match(_s, std::regex("^-?\\d*(\\.\\d+)?$"));
	};

	template < class T>
	constexpr
	std::string_view
	type_name()
	{
		std::string_view p = 
#if defined(_WIN32)
		__FUNCSIG__;
#elif defined(__linux__)
		__PRETTY_FUNCTION__;
#endif
		return std::string_view(p.data() + 84, p.size() - 84 - 7);
	}
}

#endif // !_STDA_TYPE_UTIL_HPP
