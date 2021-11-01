#include <sstream>
#include <string>
#include <iostream>
#include "../ConioPort.h"
#include <cstdint>
#include <cstdlib>

int checkCommand(std::stringstream& _ss) {

	std::string cmd;

	_ss >> cmd;

	if (cmd.compare("exit") == 0)
		return 1;
	else
		std::cout << "Command: " << cmd << std::endl;

	return 0;
}

int main() {
	
	char command[2] = { '\0', '\0' };

	uint32_t ret = 0u;
	std::string s;

	bool shutdown = false;

	while (!shutdown) {

		if (_kbhit()) {

			command[0] = _getch();

			// Recebeu um valor que é de 2 bytes, então ignora esse
			if (command[1] < 0) {

				command[1] = '\0';
				
				continue;
			}

			if (command[0] == '\b' || command[0] == '\177') {
				
				if (!s.empty()) {
					
					s.pop_back();

					_putch(command[0]);
					_putch(' ');
					_putch(command[0]);	// Clear Screen

				}else
					s.shrink_to_fit();

			}

			if (command[0] == '\n' || command[0] == '\r' && !s.empty()) {

				std::stringstream ss(s);

				// Reseta std::string s
				if (!s.empty()) {
					s.clear();
					s.shrink_to_fit();
				}

				// put break line
				_putch('\n');
				_putch('\r');

				while (!ss.eof()) {
					if (checkCommand(ss)) {

						shutdown = true;	// Sai do While Principal

						break;	// Sai do While de comandos
					}
				}

			}else if (command[0] != 0 && isascii(command[0]) && (isalpha(command[0]) && isalnum(command[0])) || (command[0] > 0 && isdigit(command[0])) 
				|| command[0] == '_' || command[0] == ' ') {

				s.push_back(command[0]);

				// Show Key on Console
				_putch(command[0]);
				
			}else
				command[1] = command[0];

		}
	}
	
	return 0;
}
