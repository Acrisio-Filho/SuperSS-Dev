// Arquivo normal_manager_db.cpp
// Criado em 01/05/2018 as 14:32 por Acrisio
// Implementação da classe NormalManagerDB

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#endif

#include "normal_manager_db.hpp"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"
#include "../UTIL/message_pool.h"

#include <ctime>

using namespace stdA;

bool NormalManagerDB::m_state = false;
std::vector< NormalDB* > NormalManagerDB::m_dbs;
uint32_t NormalManagerDB::m_db_instance_num = NUM_DB_THREAD;

NormalManagerDB::NormalManagerDB() {

}

NormalManagerDB::~NormalManagerDB() {

	destroy();
}

void NormalManagerDB::create(uint32_t _db_instance_num) {

	try {

		if (!m_state) {

			m_db_instance_num = _db_instance_num;

			// Verifica DB Instance Number
			checkDBInstanceNumAndFix();

			// Create Data base Instance and initialize
			for (auto i = 0u; i < m_db_instance_num; ++i) {

				// Make New Normal DB Instance if not reach of limit of instances
				if (m_dbs.empty() || m_dbs.size() < m_db_instance_num)
					m_dbs.push_back(new NormalDB());

				// Initialize DB Instance
				if (m_dbs[i] != nullptr)
					m_dbs[i]->init();
			}

			// Log
			_smp::message_pool::getInstance().push(new message("[NormalManagerDB::create][Log] create " + std::to_string(m_db_instance_num) + " instance do NormalDB", CL_FILE_LOG_AND_CONSOLE));

			// Initialize with Success
			m_state = true;
		}

	}catch (exception& e) {

		// Failed to initialize
		m_state = false;

		_smp::message_pool::getInstance().push(new message("[NormalManagerDB::create][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void NormalManagerDB::destroy() {

	if (m_state) {

		// Verifica DB Instance Number
		checkDBInstanceNumAndFix();

		for (auto i = 0u; i < m_db_instance_num && i < m_dbs.size(); ++i) {
			
			if (m_dbs[i] != nullptr) {
				
				m_dbs[i]->sendCloseAndWait();

				// clean memory allocate
				delete m_dbs[i];
			}
		}

		// Clean vector
		m_dbs.clear();
		m_dbs.shrink_to_fit();
	}

	m_state = false;
}

void NormalManagerDB::checkIsDeadAndRevive() {

	// Verifica DB Instance Number
	checkDBInstanceNumAndFix();

	for (auto i = 0u; i < m_db_instance_num && i < m_dbs.size(); ++i)
		if (m_dbs[i] != nullptr)
			m_dbs[i]->checkIsDeadAndRevive();
}

int NormalManagerDB::add(NormalDB::msg_t* _msg) {

	if (!m_state)	// DB not Create, cria ela
		create();

	uint32_t index = (uint32_t)(/*std::rand() **/ std::time(nullptr)) % m_dbs.size();

	if (m_dbs[index] != nullptr)
		m_dbs[index]->add(_msg);

	return 0;
}

int NormalManagerDB::add(uint32_t _id, pangya_db *_pangya_db, callback_response _callback_response, void* _arg) {

	add(new(NormalDB::msg_t){ _id, _pangya_db, _callback_response, _arg });

	return 0;
}

// Methods que o normal db faz para o server e o channel
void NormalManagerDB::insertLoginLog(session& _session, NormalDB::msg_t* _msg) {
    // Ainda não implementei depois faço isso, só criei essa função de exemplo
    return;
}

inline void NormalManagerDB::checkDBInstanceNumAndFix() {

	// check m_db_instance_num is valid
	if ((int)m_db_instance_num <= 0)
		m_db_instance_num = NUM_DB_THREAD;
}
