// GG Auth Server.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#pragma pack(1)
#include <WinSock2.h>
#include "gg_auth_server.hpp"

#include "../../Projeto IOCP/TYPE/set_se_exception_with_mini_dump.h"

#include <DbgHelp.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DbgHelp.lib")

using namespace stdA;

#define STDA_BETA

int main(int _argc, char* _argv[]) {

	if (_argc > 2 && !std::string(_argv[2]).empty() && _chdir(_argv[2]) == 0) {

		// Set o Prefixo do Auth Server no nome dos arquivos de log
		_smp::message_pool::getInstance().setPrefixo("GG_AS");

		// Inicializa log Files
		_smp::message_pool::getInstance().reload_log_files();

		_smp::message_pool::getInstance().push(new message("[main][Log] change current directory to " + std::string(_argv[2]), CL_FILE_LOG_AND_CONSOLE));
	
	}else // Inicializa Log files
		_smp::message_pool::getInstance().reload_log_files();
    
	STDA_SET_SE_EXCEPTION;

	SYSTEM_INFO _si;
	GetSystemInfo(&_si);

	HANDLE hProcess = GetCurrentProcess();

	SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);

	SetProcessPriorityBoost(hProcess, 1);

	try {
		
		sgg_as::getInstance().start();
	
	}catch (exception& e) {

		std::cout << e.getFullMessageError() << std::endl;
	}

#ifndef STDA_BETA
	system("pause");
#endif // !STDA_BETA

	return 0;
}
