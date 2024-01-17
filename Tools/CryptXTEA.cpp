// Arquivo CryptXTEA.cpp
// Criado em 08/12/2015 por Acrisio
// Implementação da classe CryptXTEA

#include "CryptXTEA.h"

namespace stdA {
	unsigned int Keys_GB[4]  = {0x03F607A9, 0x036F5A3E, 0x011002B4, 0x04AB00EA};	// Season 9 FRESH UP
	unsigned int Keys_TH[4]  = {0x050AD33B, 0x00BAFF09, 0x0452FFDA, 0x02CB4422};	// Season 4.9 e 9
	unsigned int Keys_JP[4]  = {0x020A5FD4, 0x01EEBDFF, 0x02B3C6A0, 0x04F6A3E1};	// Season 9 FRESH UP pangya JP
}

using namespace stdA;

CryptXTEA::CryptXTEA() {};
CryptXTEA::~CryptXTEA() {};

/*Src and Dst - IO*/
int CryptXTEA::OpenFile(unsigned char*& buffer, char* path, unsigned int* tamanho, int option) {
	
	if(path != NULL) {
		
		if(option == 0) { // Open File For Read
			
			std::ifstream InFile(path, std::ifstream::binary | std::ifstream::in);
			
			if(InFile.good()) {
				InFile.seekg(0, InFile.end);
				*tamanho = (unsigned int) InFile.tellg();
				InFile.seekg(0, InFile.beg);
				
				if(buffer == NULL)
					buffer = (unsigned char*) malloc(*tamanho);
				
				memset(buffer, 0, *tamanho);
				
				InFile.read((char*)buffer, *tamanho);
				
				InFile.close();
			}else {
				std::cout << "Erro ao abrir o arquivo " << path << std::endl;
				return -2;
			}
		}else if(option == 1) { // Open File For Write
			
			if(buffer != NULL) {
				
				std::ofstream OutFile(path, std::ofstream::binary);
			
				if(OutFile.good()) {
					
					OutFile.write((char*)buffer, *tamanho);
					
					OutFile.close();
				}else {
					std::cout << "Erro ao abrir o arquivo " << path << std::endl;
					return -2;
				}
			}else {
				std::cout << "Error Path is null pointer.\n";
				return -1;
			}
		}
	}else {
		std::cout << "Error Path is null pointer.\n";
		return -1;
	}
	
	return 0;
}

void CryptXTEA::EncryptXTEA(unsigned int* keys, unsigned int* dst, unsigned int* src) {
	unsigned int count = 4;
	unsigned int padrao, padrao2;
	unsigned int temp, var1, var2;
	unsigned int valor = 0; // Delta
	const unsigned int valor2 = 0x61C88647; // Soma
	
	memmove(dst, src, sizeof(unsigned int) * 2);
	
	padrao = dst[0];
	
	while(count > 0) {
		
		var1 = (((dst[1] >> 5) ^ (dst[1] << 4)) + dst[1]);
		var2 = keys[(valor & 3)] + valor;
		temp = padrao + (var1 ^ var2);
		valor -= valor2;
		
		for(int i = 0; i < 3; i++) {
			dst[0] = temp;
			var1 = (((temp >> 5) ^ (temp << 4)) + temp);
			var2 = keys[((valor >> 11) & 3)] + valor;
			padrao2 = dst[1] + (var1 ^ var2);
			dst[1] = padrao2; // p2
			
			var1 = (((padrao2 >> 5) ^ (padrao2 << 4)) + padrao2);
			var2 = keys[(valor & 3)] + valor;
			temp = temp + (var1 ^ var2);
			valor -= valor2;
		}
		
		dst[0] = padrao = temp;
		var1 = (((temp >> 5) ^ (temp << 4)) + temp);
		var2 = keys[((valor >> 11) & 3)] + valor;
		temp = dst[1] + (var1 ^ var2);
		dst[1] = temp;
		
		count--;
	}
}

void CryptXTEA::DecryptXTEA(unsigned int* keys, unsigned int* dst, unsigned int* src) {
	unsigned int count = 4;
	unsigned int padrao, padrao2;
	unsigned int temp, var1, var2;
	unsigned int valor = 0xE3779B90; // Delta
	const unsigned int valor2 = 0x61C88647; // Soma
	
	memmove(dst, src, sizeof(unsigned int) * 2);
	
	padrao = dst[1];
	
	while(count > 0) {
		
		var1 = (((dst[0] >> 5) ^ (dst[0] << 4)) + dst[0]);
		var2 = keys[((valor >> 11) & 3)] + valor;
		temp = padrao - (var1 ^ var2);
		valor += valor2;
		
		for(int i = 0; i < 3; i++) {
			dst[1] = temp;
			var1 = (((temp >> 5) ^ (temp << 4)) + temp);
			var2 = keys[(valor & 3)] + valor;
			padrao2 = dst[0] - (var1 ^ var2);
			dst[0] = padrao2; // p2
			
			var1 = (((padrao2 >> 5) ^ (padrao2 << 4)) + padrao2);
			var2 = keys[((valor >> 11) & 3)] + valor;
			temp = temp - (var1 ^ var2);
			valor += valor2;
		}
		
		dst[1] = padrao = temp;
		var1 = (((temp >> 5) ^ (temp << 4)) + temp);
		var2 = keys[(valor & 3)] + valor;
		temp = dst[0] - (var1 ^ var2);
		dst[0] = temp;
		
		count--;
	}
}

void CryptXTEA::EncryptUpdatelist(unsigned char* dst, unsigned char* src, unsigned int tamanho, int option) {
	unsigned int size = tamanho;
	unsigned int count = 0, count_encrypt = 0;
	unsigned int *Keys = NULL;

	// Chave que vai ser usada
	switch (option) {
	case 0:		// S4.9 e 9 TH
		Keys = Keys_TH;	// S4.9 e 9 TH
		break;
	case 1:		// S9 Global
	default:	// S9 Global
		Keys = Keys_GB; // S9 Global
		break;
	case 2:		// S9 JP Key
		Keys = Keys_JP; // S9 JP
		break;
	}
	
	unsigned int size_int = ((size / sizeof(unsigned int))) + ((size%4 != 0) ? 1 : 0);
	
	unsigned int* Dst = NULL;
	unsigned int* Src = NULL;
	
	Dst = (unsigned int*) malloc(size_int * sizeof(unsigned int));
	Src = (unsigned int*) malloc(size_int * sizeof(unsigned int));

	memset(Src, 0, size_int * sizeof(unsigned int));
	memset(Dst, 0, size_int * sizeof(unsigned int));

	memmove(Src, src, tamanho);
	
	count = size / 8;
	
	while(count > 0) {

		EncryptXTEA(Keys, &Dst[count_encrypt], &Src[count_encrypt]);
		
		count_encrypt += 2;
		count--;
	}
	
	memmove(dst, Dst, size);
	
	if(Dst != NULL)
		free(Dst);
	
	if(Src != NULL)
		free(Src);
	
}

void CryptXTEA::DecryptUpdatelist(unsigned char* dst, unsigned char* src, unsigned int tamanho, int option) {
	unsigned int size = tamanho;
	unsigned int count = 0, count_decrypt = 0;
	unsigned int *Keys = NULL;
	
	// Chave que vai ser usada
	switch (option) {
	case 0:		// S4.9 e 9 TH
		Keys = Keys_TH;	// S4.9 e 9 TH
		break;
	case 1:		// S9 Global
	default:	// S9 Global
		Keys = Keys_GB; // S9 Global
		break;
	case 2:		// S9 JP Key
		Keys = Keys_JP; // S9 JP
		break;
	}

	unsigned int size_int = ((size / sizeof(unsigned int))) + ((size%4 != 0) ? 1 : 0);
	
	unsigned int* Dst = NULL;
	unsigned int* Src = NULL;
	
	Dst = (unsigned int*) malloc(size_int * sizeof(unsigned int));
	Src = (unsigned int*) malloc(size_int * sizeof(unsigned int));
	
	memmove(Src, src, size);
	
	memset(Dst, 0, size_int * sizeof(unsigned int));
	
	count = size / 8;
	
	while(count > 0) {

		DecryptXTEA(Keys, &Dst[count_decrypt], &Src[count_decrypt]);
		
		count_decrypt += 2;
		count--;
	}
	
	memmove(dst, Dst, tamanho);
	
	if(Dst != NULL)
		free(Dst);
	
	if(Src != NULL)
		free(Src);
	
}