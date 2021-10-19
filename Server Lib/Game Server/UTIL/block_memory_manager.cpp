// Arquivo block_memory_manager.cpp
// Criado em 17/11/2018 as 18:00 por Acrisio
// Implementa��o da classe BlockMemoryManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "block_memory_manager.hpp"

#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

std::map< uint32_t/*UID*/, BlockMemoryManager::BlockCtx > BlockMemoryManager::mp_block;

BlockMemoryManager::BlockMemoryManager() {}

BlockMemoryManager::~BlockMemoryManager() {
	clear();
}

void BlockMemoryManager::clear() {

	// Acho que desaloca a mem�ria, se n�o tenho que rodar em um loop, para desalocar ela
	if (!mp_block.empty())
		mp_block.clear();
}

void BlockMemoryManager::blockUID(uint32_t _uid) {

	auto it = mp_block.find(_uid);

	if (it == mp_block.end()) {	// N�o tem Cria um

		auto itt = mp_block.insert(std::make_pair(_uid, BlockCtx{ 0u }));

		if (!itt.second)
			_smp::message_pool::getInstance().push(new message("[BlockMemoryManager::blockUID][Error] tentou inserir um block ja existente no map[KEY=" 
					+ std::to_string(itt.first->first) + "]. Bug", CL_FILE_LOG_AND_CONSOLE));
		
		it = itt.first;
	}

	// Enter Critical Section
#if defined(_WIN32)
	EnterCriticalSection(&it->second.cs);
#elif defined(__linux__)
	pthread_mutex_lock(&it->second.cs);
#endif
}

void BlockMemoryManager::unblockUID(uint32_t _uid) {

	auto it = mp_block.find(_uid);

	if (it == mp_block.end()) {
		_smp::message_pool::getInstance().push(new message("[BlockMemoryManager::unblockUID][Error] block[UID=" + std::to_string(_uid) + "] nao existe no map. Bug", 
				CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	// Leave Critical Section
#if defined(_WIN32)
	LeaveCriticalSection(&it->second.cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&it->second.cs);
#endif
}
