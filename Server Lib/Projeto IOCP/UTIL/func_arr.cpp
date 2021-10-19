// Arquivo func_arr.cpp
// Criado em 22/07/2017 por Acrisio
// Implementação da classe func_arr

#include "func_arr.h"
#include "exception.h"
#include "message_pool.h"
#include "hex_util.h"
#include "../TYPE/stda_error.h"

#if defined(__linux__)
#include "WinPort.h"
#endif

using namespace stdA;

int func_arr::func_arr_ex::execCmd(void* _arg) {
	return cf(Param, _arg);
};

func_arr::func_arr() {
    ZeroMemory(m_func, sizeof m_func);
};

func_arr::~func_arr() {
    ZeroMemory(m_func, sizeof m_func);
};

void func_arr::addPacketCall(unsigned __int16 _tipo, call_func _func, void* _Param) {

	if (_tipo < MAX_CALL_FUNC_ARR) {
		m_func[_tipo].cf = _func;
		m_func[_tipo].Param = _Param;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("Adicionou Packet Function Call com sucesso."));
#endif
	}
};

func_arr::func_arr_ex* func_arr::getPacketCall(unsigned __int16 _tipo) {
   
	if (_tipo < MAX_CALL_FUNC_ARR) {
        
		if (m_func[_tipo].cf != nullptr)
            return &m_func[_tipo];
        else
            throw exception("[func_arr::getPacketCall][Error] Tipo: " + std::to_string(_tipo) + "(0x" + hex_util::ltoaToHex(_tipo) + "), desconhecido ou nao implementado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::FUNC_ARR, 1, 0));
    }else
        throw exception("[func_arr::getPacketCall][Error] Tipo: " + std::to_string(_tipo) + "(0x" + hex_util::lltoaToHex(_tipo) + ") maior que o array.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::FUNC_ARR, 2, 0));

    return nullptr;
};