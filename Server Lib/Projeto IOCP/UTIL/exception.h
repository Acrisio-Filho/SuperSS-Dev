// Arquivo exception.h
// Criado em 02/03/2017 por Acrisio
// Definição da classe exception

#pragma once
#ifndef _STDA_EXCEPTION_H
#define _STDA_EXCEPTION_H

#include <string>
#include <exception>

#define STDA_SOURCE_ERROR_ENCODE(source_error) (int)(((source_error) << 24) & 0xFF000000)
#define STDA_SOURCE_ERROR_DECODE(err_code) (int)(((err_code) >> 24) & 0x000000FF)
#define STDA_ERROR_ENCODE(err) (int)(((err) << 16) & 0x00FF0000)
#define STDA_ERROR_DECODE(err_code) (int)(((err_code) >> 16) & 0x000000FF)
#define STDA_SYSTEM_ERROR_ENCODE(_err_sys) (int)((_err_sys) & 0x0000FFFF)
#define STDA_SYSTEM_ERROR_DECODE(err_code) (int)((err_code) & 0x0000FFFF)
#define STDA_MAKE_ERROR(source_error, err_code, _err_sys) (int)(STDA_SOURCE_ERROR_ENCODE((source_error)) | STDA_ERROR_ENCODE((err_code)) | STDA_SYSTEM_ERROR_ENCODE((_err_sys)))
#define STDA_ERROR_CHECK_SOURCE_AND_ERROR(err_code, source_error, error) (bool)(STDA_SOURCE_ERROR_DECODE((err_code)) == (source_error) && STDA_ERROR_DECODE((err_code)) == (error))

namespace stdA {
	class exception : public std::exception {
	public:
		exception(std::string message_error, uint32_t code_error);
		exception(std::wstring message_error, uint32_t code_error);
		~exception();

		const std::string getMessageError();
		const uint32_t getCodeError();

		const std::string getFullMessageError();

	protected:
		std::string m_message_error;
		std::string m_message_error_full;
		uint32_t m_code_error;
	};
}

#endif