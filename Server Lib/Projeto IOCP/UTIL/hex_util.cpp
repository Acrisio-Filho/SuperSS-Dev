// Arquivo hex_util.cpp
// Criado em 10/06/2017 por Acrisio
// Implementação da classe hex_util

#include "hex_util.h"
#include <iomanip>
#include <iostream>

using namespace stdA;

void hex_util::ShowHex(unsigned char* buffer, size_t size) {
    size_t offset = 0, i = 0, j = 0;

	std::cout << std::endl;
	
	//std::cout << "ADDR 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F \tASCII            \n";
	for(i = 0; i < size; i++) {
		// Offset
		if((i % 16) == 0) {
			std::cout << std::uppercase << std::hex << std::setw(4) << std::setfill('0');
			std::cout << offset;
			std::cout << " ";

			offset += 16;
		}

		// Body
		std::cout << std::uppercase << std::hex << std::setw(2) << std::setfill('0');
		std::cout << (int)buffer[i];
		std::cout << " ";

		if(((i + 1) % 16) == 0) {
			std::cout << std::nouppercase << std::dec << "\t";
			for(j = (i - 15); j < (i + 1); j++) {
				if((buffer[j] >= 32) && (buffer[j] <= 126))
					std::cout << buffer[j];
				else
					std::cout << ".";
			}
			std::cout << std::endl;
		}
	}

	// Resto
	if((size % 16) != 0) {
		for(i = 0; i < (16 - (size % 16)); i++) {
			std::cout << std::uppercase << std::hex << std::setw(2) << std::setfill('-');
			std::cout << "-";
			std::cout << " ";
		}

		std::cout << std::nouppercase << std::dec << "\t";
		for(i = (size - (size % 16)); i < size; i++) {
			if((buffer[i] >= 32) && (buffer[i] <= 126))
				std::cout << buffer[i];
			else
				std::cout << ".";
		}

		for(i = 0; i < (16 - (size % 16)); i++)
			std::cout << ".";
		std::cout << std::endl;
	}
};

std::string hex_util::BufferToHexString(unsigned char* buffer, size_t size) {
    std::stringstream ss;

     size_t offset = 0, i = 0, j = 0;

	 ss << std::endl;
	
	//ss << "ADDR 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F \tASCII            \n";
	for(i = 0; i < size; i++) {
		// Offset
		if((i % 16) == 0) {
			ss << std::uppercase << std::hex << std::setw(4) << std::setfill('0');
			ss << offset;
			ss << " ";

			offset += 16;
		}

		// Body
		ss << std::uppercase << std::hex << std::setw(2) << std::setfill('0');
		ss << (int)buffer[i];
		ss << " ";

		if(((i + 1) % 16) == 0) {
			ss << std::nouppercase << std::dec << "\t";
			for(j = (i - 15); j < (i + 1); j++) {
				if((buffer[j] >= 32) && (buffer[j] <= 126))
					ss << buffer[j];
				else
					ss << ".";
			}
			ss << std::endl;
		}
	}

	// Resto
	if((size % 16) != 0) {
		for(i = 0; i < (16 - (size % 16)); i++) {
			ss << std::uppercase << std::hex << std::setw(2) << std::setfill('-');
			ss << "-";
			ss << " ";
		}

		ss << std::nouppercase << std::dec << "\t";
		for(i = (size - (size % 16)); i < size; i++) {
			if((buffer[i] >= 32) && (buffer[i] <= 126))
				ss << buffer[i];
			else
				ss << ".";
		}

		for(i = 0; i < (16 - (size % 16)); i++)
			ss << ".";
		ss << std::endl;
	}

	ss.seekg(0, std::ios_base::end);
	
	size_t size_str = (size_t)ss.tellg();
	
	ss.seekg(0, std::ios_base::beg);

	char *buffer_str = new char[size_str];

	ss.read(buffer_str, size_str);

	std::string str(buffer_str, size_str);

	delete[] buffer_str;

	return str;
};

std::string hex_util::StringToHexString(std::string s) {
    std::stringstream ss;
    size_t size = s.size();

    size_t offset = 0, i = 0, j = 0;

	ss << std::endl;
	
	//ss << "ADDR 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F \tASCII            \n";
	for(i = 0; i < size; i++) {
		// Offset
		if((i % 16) == 0) {
			ss << std::uppercase << std::hex << std::setw(4) << std::setfill('0');
			ss << offset;
			ss << " ";

			offset += 16;
		}

		// Body
		ss << std::uppercase << std::hex << std::setw(2) << std::setfill('0');
		ss << (int)(unsigned char)s[i];
		ss << " ";

		if(((i + 1) % 16) == 0) {
			ss << std::nouppercase << std::dec << "\t";
			for(j = (i - 15); j < (i + 1); j++) {
				if((s[j] >= 32) && (s[j] <= 126))
					ss << s[j];
				else
					ss << ".";
			}
			ss << std::endl;
		}
	}

	// Resto
	if((size % 16) != 0) {
		for(i = 0; i < (16 - (size % 16)); i++) {
			ss << std::uppercase << std::hex << std::setw(2) << std::setfill('-');
			ss << "-";
			ss << " ";
		}

		ss << std::nouppercase << std::dec << "\t";
		for(i = (size - (size % 16)); i < size; i++) {
			if((s[i] >= 32) && (s[i] <= 126))
				ss << s[i];
			else
				ss << ".";
		}

		for(i = 0; i < (16 - (size % 16)); i++)
			ss << ".";
		ss << std::endl;
	}

	ss.seekg(0, std::ios_base::end);

	size_t size_str = (size_t)ss.tellg();

	ss.seekg(0, std::ios_base::beg);
	
	char *buffer_str = new char[size_str];

	ss.read(buffer_str, size_str);

	s.assign(buffer_str, size_str);

	delete[] buffer_str;

	return s;
};

std::string hex_util::ltoaToHex(int32_t l) {
	std::stringstream ss;
	std::string s;

	ss << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << l;

	ss >> s;

	return s;
};

std::string hex_util::lltoaToHex(int64_t ll) {
	std::stringstream ss;
	std::string s;

	ss << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << ll;

	ss >> s;

	return s;
};
