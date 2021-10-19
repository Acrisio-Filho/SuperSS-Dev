// Auth Server.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <sys/time.h>
#include <sys/resource.h>
#endif

#if defined(_WIN32)
#include "pch.h"
#endif

#include "auth_server.hpp"
#include "../../Projeto IOCP/TYPE/pangya_st.h"
#include <string>
#include <iostream>
#include "../../Projeto IOCP/DATABASE/mssql.h"

#if defined(_WIN32)
#include <direct.h>
#endif

#include "../../Projeto IOCP/TYPE/set_se_exception_with_mini_dump.h"

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

#if defined(_WIN32)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DbgHelp.lib")
#endif

using namespace stdA;

#define STDA_BETA

int main(int argc, char* argv[]) {

	if (argc > 2 && !std::string(argv[2]).empty() && 
#if defined(_WIN32)
		_chdir(argv[2]) == 0
#elif defined(__linux__)
		chdir(argv[2]) == 0
#endif
	) {

		// Set o Prefixo do Auth Server no nome dos arquivos de log
		_smp::message_pool::getInstance().setPrefixo("AS");

		// Inicializa log Files
		_smp::message_pool::getInstance().reload_log_files();

		_smp::message_pool::getInstance().push(new message("[main][Log] change current directory to " + std::string(argv[2]), CL_FILE_LOG_AND_CONSOLE));
	
	}else // Inicializa Log files
		_smp::message_pool::getInstance().reload_log_files();
    
#if defined(_WIN32)
	STDA_SET_SE_EXCEPTION;
#elif defined(__linux__)
	// Linux Gera sozinho o miniDump(Core em Unix/Linux)
	rlimit core_limit;
	
	core_limit.rlim_cur = RLIM_INFINITY; //0x400000; // 4Mib
	core_limit.rlim_max = RLIM_INFINITY; // 4Mib

	setrlimit(RLIMIT_CORE, &core_limit);
#endif

#if defined(_WIN32)
	SYSTEM_INFO _si;
	GetSystemInfo(&_si);

	HANDLE hProcess = GetCurrentProcess();

	SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);

	SetProcessPriorityBoost(hProcess, 1);
#elif defined(__linux__)
	// set priority class e process priority no linux
	errno = 0;

	int priority = getpriority(PRIO_PROCESS, 0);

	if (priority == -1 && errno != 0) {

		_smp::message_pool::getInstance().push(new message("[main][WARNING][Error] fail to get priority process. errno: " + std::to_string(errno), CL_FILE_LOG_AND_CONSOLE));

		priority = 0;
	}
	
	_smp::message_pool::getInstance().push(new message("[main][Log] priority pocess: " + std::to_string(priority), CL_FILE_LOG_AND_CONSOLE));

	if (priority > PRIO_MIN) {

		// -2 se não -1 (menos é mais no Linux)
		priority -= 2;

		if (priority > PRIO_MIN || ++priority > PRIO_MIN) {

			if (setpriority(PRIO_PROCESS, 0, priority) == -1)
				_smp::message_pool::getInstance().push(new message("[main][WARNING][Error] fail to set priority(" + std::to_string(priority) 
						+ ") process. errno: " + std::to_string(errno), CL_FILE_LOG_AND_CONSOLE));
			else
				_smp::message_pool::getInstance().push(new message("[main][Log] setted priority process to " + std::to_string(priority), CL_FILE_LOG_AND_CONSOLE));
		}
	}
#endif

	try {
		
		//auth_server as;
		sas::as::getInstance().start();

		//as.start();
	
	}catch (exception& e) {

		std::cout << e.getFullMessageError() << std::endl;
	}

#ifndef STDA_BETA
	#if defined(_WIN32)
		system("pause");
	#elif defined(__linux__)
		pause();
	#endif
#endif // !STDA_BETA

	return 0;
}
