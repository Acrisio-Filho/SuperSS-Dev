// Multi Client.cpp : define o ponto de entrada para o aplicativo do console.
//

#pragma pack(1)

#include <WinSock2.h>
#include "stdafx.h"
#include "multi_client.h"
#include <iostream>
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

#include "../../Projeto IOCP/TYPE/set_se_exception_with_mini_dump.h"

#include <DbgHelp.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DbgHelp.lib")

using namespace stdA;

int main(int _argc, char *_argv[]) {

	STDA_SET_SE_EXCEPTION;

	SYSTEM_INFO _si;
	GetSystemInfo(&_si);

	HANDLE hProcess = GetCurrentProcess();

	SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);

	SetProcessPriorityBoost(hProcess, 1);

	int qnt = 1;

	if (_argc == 1) {
		std::cout << "Numero de conexoes.\r\n";
		std::cin >> qnt;
	}else if (_argc > 1) {
		qnt = atoi(_argv[1]);
	}

	if (qnt <= 0 || qnt > 2000)
		qnt = 1;

	try {
		multi_client mc(qnt);

		mc.start();
	}catch (exception& e) {
		std::cout << e.getFullMessageError() << std::endl;
	}

	system("pause");

    return 0;
}

