// Arquivo reader_ini.cpp
// Criado em 27/03/2018 as 21:35 por Acrisio
// Implementação da classe ReaderIni

#if defined(_WIN32)
#pragma pack(1)
#elif defined(__linux__)
//#pragma pack(1)	// Não quero colocar o pack aqui no windows e nem no linux, mas no linux vou colocar nos lugares certos
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>
#endif

#include "reader_ini.hpp"
#include <fstream>
#include "exception.h"
#include "../TYPE/stda_error.h"
#include "message_pool.h"

using namespace stdA;

ReaderIni::ReaderIni(std::string _file_name) : m_file_name(_file_name), m_full_file_name(), m_state(UNINITIALIZED) {
};

ReaderIni::~ReaderIni() {
    m_state = UNINITIALIZED;
};

void ReaderIni::init() {

	if (m_file_name.empty())
		throw exception("[ReaderIni::init][Error] file name is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 1, 0));

	char buff[1024];

#if defined(_WIN32)
	if (GetCurrentDirectoryA(sizeof(buff), buff) < 0)
		throw exception("[ReaderIni::init][Error] nao conseguiu pegar o diretorio atual.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 7, GetLastError()));
#elif defined(__linux__)
	if (getcwd(buff, sizeof(buff)) == nullptr)
		throw exception("[ReaderIni::init][Error] nao conseguiu pegar o diretorio atual.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 7, errno));
#endif

	m_full_file_name = buff + m_file_name;

	std::ifstream check;
	check.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {

		check.open(m_full_file_name, std::ifstream::in);

		if (!check.is_open())
			throw exception("[ReaderIni::init][Error] nao conseguiu abrir o arquivo='" + m_full_file_name + "'", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 2, 0));

		check.close();

		m_state = INITIALIZED;

	}catch (std::ifstream::failure& e) {
		//_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE));
		throw exception("[ReaderIni::init][Error][" + std::string(e.what()) + " Code: " + std::to_string(e.code().value()) + "] nao conseguiu abrir o arquivo='" + m_full_file_name + "'", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 2, e.code().value()));
	}
}

std::string ReaderIni::readString(std::string _section, std::string _key) {
    
	if (m_state == UNINITIALIZED)
		throw exception("[ReaderIni::readString][Error] ReaderIni uninitialized.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 3, 0));

    if (_section.empty() || _key.empty())
        throw exception("[ReaderIni::readString][Error] argumentos invalidos.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 4, 0));

    char buff[1024];

#if defined(_WIN32)
	DWORD ret = 0u;
    
	if ((ret = GetPrivateProfileStringA(_section.c_str(), _key.c_str(), "ERR_OCURR", buff, sizeof(buff), m_full_file_name.c_str())) == ERROR_FILE_NOT_FOUND
			&& errno == ERROR_FILE_NOT_FOUND && GetLastError() != 0)
        throw exception("[ReaderIni::readString][Error] nao conseguiu pegar o private profile. file name = " + m_full_file_name, STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 5, GetLastError()));

    if (strcmp(buff, "ERR_OCURR") == 0)
        throw exception("[ReaderIni::readString][Error] Section[" + _section + "] or Key[" + _key +"] not found.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 6, 0));
#elif defined(__linux__)
	
	// GLIB-2.0 (2.64.6)
	GKeyFile *keyfile = g_key_file_new();
	GError *g_err = nullptr;

	if (!g_key_file_load_from_file(keyfile, m_full_file_name.c_str(), GKeyFileFlags::G_KEY_FILE_NONE, &g_err)) {

		uint32_t code_err = 0;

		// free
		if (keyfile != nullptr)
			g_key_file_free(keyfile);

		if (g_err != nullptr) {

			code_err = g_err->code;

			// free
			g_error_free(g_err);
		}

		throw exception("[ReaderIni::readString][Error] nao conseguiu pegar o private profile. file name = " + m_full_file_name, STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 5, code_err));
	}

	g_autofree gchar* val = g_key_file_get_string(keyfile, _section.c_str(), _key.c_str(), &g_err);

	if (val == nullptr) {
		
		uint32_t code_err = 0;

		// free
		if (keyfile != nullptr)
			g_key_file_free(keyfile);

		if (g_err != nullptr) {

			code_err = g_err->code;

			// free
			g_error_free(g_err);
		}

		throw exception("[ReaderIni::readString][Error] Section[" + _section + "] or Key[" + _key +"] not found.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 6, code_err));
	}

	// Copy
	strncpy(buff, val, sizeof(buff));
	
	buff[sizeof(buff) - 1] = '\0';
#endif

    return buff;
};

int ReaderIni::readInt(std::string _section, std::string _key) {
    
	if (m_state == UNINITIALIZED)
		throw exception("[ReaderIni::readInt][Error] ReaderIni uninitialized.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 3, 0));

    if (_section.empty() || _key.empty())
        throw exception("[ReaderIni::readInt][Error] argumentos invalidos.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 4, 0));

    int ret = -3232;

#if defined(_WIN32)
	if ((ret = GetPrivateProfileIntA(_section.c_str(), _key.c_str(), -3232, m_full_file_name.c_str())) == -3232)
		throw exception("[ReaderIni::readInt][Error] nao conseguiu pegar o private profile. file name = " + m_full_file_name + "\r\n Section[" + _section + "] or Key[" + _key + "] not found.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 6, 0));
#elif defined(__linux__)
	
	// GLIB-2.0 (2.64.6)
	GKeyFile *keyfile = g_key_file_new();
	GError *g_err = nullptr;

	if (!g_key_file_load_from_file(keyfile, m_full_file_name.c_str(), GKeyFileFlags::G_KEY_FILE_NONE, &g_err)) {

		uint32_t code_err = 0;

		// free
		if (keyfile != nullptr)
			g_key_file_free(keyfile);

		if (g_err != nullptr) {

			code_err = g_err->code;

			// free
			g_error_free(g_err);
		}

		throw exception("[ReaderIni::readInt][Error] nao conseguiu pegar o private profile. file name = " + m_full_file_name, STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 5, code_err));
	}

	gint val = g_key_file_get_integer(keyfile, _section.c_str(), _key.c_str(), &g_err);

	if (val == 0 && g_err != nullptr) {
		
		uint32_t code_err = g_err->code;

		// free
		if (keyfile != nullptr)
			g_key_file_free(keyfile);

		// free
		g_error_free(g_err);

		throw exception("[ReaderIni::readInt][Error] Section[" + _section + "] or Key[" + _key +"] not found.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 6, code_err));
	}

	// Copy
	ret = val;
#endif

    return ret;
};

int64_t ReaderIni::readBigInt(std::string _section, std::string _key) {
	
	if (m_state == UNINITIALIZED)
		throw exception("[ReaderIni::readBigInt][Error] ReaderIni uninitialized.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 3, 0));

	if (_section.empty() || _key.empty())
		throw exception("[ReaderIni::readBigInt][Error] argumentos invalidos.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 4, 0));

	int64_t ret = -3232;

#if defined(_WIN32)
	char buff[1024];
	DWORD ret2 = 0u;

	if ((ret2 = GetPrivateProfileStringA(_section.c_str(), _key.c_str(), "ERR_OCURR", buff, sizeof(buff), m_full_file_name.c_str())) == ERROR_FILE_NOT_FOUND 
			&& errno == ERROR_FILE_NOT_FOUND && GetLastError() != 0)
		throw exception("[ReaderIni::readBigInt][Error] nao conseguiu pegar o private profile. file name = " + m_full_file_name, STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 5, GetLastError()));

	if (strcmp(buff, "ERR_OCURR") == 0)
		throw exception("[ReaderIni::readBigInt][Error] Section[" + _section + "] or Key[" + _key + "] not found.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 6, 0));

	ret= std::strtoll(buff, nullptr, 10);
#elif defined(__linux__)
	
	// GLIB-2.0 (2.64.6)
	GKeyFile *keyfile = g_key_file_new();
	GError *g_err = nullptr;

	if (!g_key_file_load_from_file(keyfile, m_full_file_name.c_str(), GKeyFileFlags::G_KEY_FILE_NONE, &g_err)) {

		uint32_t code_err = 0;

		// free
		if (keyfile != nullptr)
			g_key_file_free(keyfile);

		if (g_err != nullptr) {

			code_err = g_err->code;

			// free
			g_error_free(g_err);
		}

		throw exception("[ReaderIni::readBigInt][Error] nao conseguiu pegar o private profile. file name = " + m_full_file_name, STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 5, code_err));
	}

	gint64 val = g_key_file_get_int64(keyfile, _section.c_str(), _key.c_str(), &g_err);

	if (val == 0 && g_err != nullptr) {
		
		uint32_t code_err = g_err->code;

		// free
		if (keyfile != nullptr)
			g_key_file_free(keyfile);

		// free
		g_error_free(g_err);

		throw exception("[ReaderIni::readBigInt][Error] Section[" + _section + "] or Key[" + _key +"] not found.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::READER_INI, 6, code_err));
	}

	// Copy
	ret = val;
#endif

	return ret;
};