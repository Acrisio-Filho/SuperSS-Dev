// Arquivo lottery.cpp
// Criado em 24/06/2018 as 12:02 por Acrisio
// Implementa��o da classe Lottery

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "lottery.hpp"

#include <algorithm>
#include <random>
#include <ctime>

#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

using namespace stdA;

Lottery::Lottery(uint64_t _value_rand) : m_prob_limit(0ull) {

	initialize(_value_rand);
}

Lottery::~Lottery() {

	clear();
	clear_roleta();

	if (!m_rand_values.empty()) {
		m_rand_values.clear();
		m_rand_values.shrink_to_fit();
	}
}

void Lottery::initialize(uint64_t _value_rand) {

	// 5 Rands Values
	for (auto i = 0u; i < 5u; ++i)
		m_rand_values.push_back(sRandomGen::getInstance().rIbeMt19937_64_chrono());

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Lottery][Test] Values RAND: L=" + std::to_string(m_rand_values[0]) + " R=" + std::to_string(m_rand_values[1]) + " T="
			+ std::to_string(m_rand_values[2]) + " E=" + std::to_string(m_rand_values[3]) + " S=" + std::to_string(m_rand_values[4]) + "", CL_ONLY_FILE_LOG));
#endif // _DEBUG

	shuffle_values_rand();
}

void Lottery::fill_roleta() {

	if (m_ctx.empty())
		throw exception("[Lottery::fill_roleta][Error] nao tem lottery ctx, por favor popule o lottery primeiro.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOTTERY, 1, 0));
	
	// Limpa Roleta
	clear_roleta();

	// Shuffle Ctx
	std::shuffle(m_ctx.begin(), m_ctx.end(), std::mt19937_64(sRandomGen::getInstance().rDevice()));
	std::shuffle(m_ctx.begin(), m_ctx.end(), std::default_random_engine((uint32_t)sRandomGen::getInstance().rIbeMt19937_64_chrono()));

	m_prob_limit = 0ull;

	// Preenche Roleta
	for (auto& el : m_ctx) {
		if (el.active) {
			el.offset[0] = (m_prob_limit == 0 ? m_prob_limit : m_prob_limit + 1);
			el.offset[1] = m_prob_limit += (el.prob <= 0) ? 100 : el.prob;

			m_roleta[el.offset[0]] = &el;
			m_roleta[el.offset[1]] = &el;
		}
	}
}

void Lottery::clear_roleta() {

	if (!m_roleta.empty())
		m_roleta.clear();
}

void Lottery::remove_draw_item(LotteryCtx* _lc) {

	if (_lc != nullptr) {
		_lc->active = 0;
		//auto it = std::find_if(m_ctx.begin(), m_ctx.end(), [&](auto& el) {
		//	return el.offset[0] == _lc->offset[0] && el.offset[1] == _lc->offset[1];
		//});

		//// Remove from vector
		//if (it != m_ctx.end())
		//	//m_ctx.erase(it);
		//	it->active = 0;
	}
}

void Lottery::clear() {	// Clear Ctx

	if (!m_ctx.empty()) {
		m_ctx.clear();
		m_ctx.shrink_to_fit();
	}
}

void Lottery::shuffle_values_rand() {

	std::shuffle(m_rand_values.begin(), m_rand_values.end(), std::mt19937_64(sRandomGen::getInstance().rDevice()));

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Lottery][Test][Shuffle::mt19937] Values RAND: L=" + std::to_string(m_rand_values[0]) + " R=" + std::to_string(m_rand_values[1]) + " T="
			+ std::to_string(m_rand_values[2]) + " E=" + std::to_string(m_rand_values[3]) + " S=" + std::to_string(m_rand_values[4]) + "", CL_ONLY_FILE_LOG));
#endif

	std::shuffle(m_rand_values.begin(), m_rand_values.end(), std::default_random_engine((uint32_t)sRandomGen::getInstance().rIbeMt19937_64_chrono()));

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Lottery][Test][Shuffle::Default_engine] Values RAND: L=" + std::to_string(m_rand_values[0]) + " R=" + std::to_string(m_rand_values[1]) + " T="
			+ std::to_string(m_rand_values[2]) + " E=" + std::to_string(m_rand_values[3]) + " S=" + std::to_string(m_rand_values[4]) + "", CL_ONLY_FILE_LOG));
#endif
}

void Lottery::push(LotteryCtx& _lc) {
	m_ctx.push_back(_lc);
}

void Lottery::push(uint32_t _prob, size_t _value) {
	
	LotteryCtx lc{ 0 };

	lc.active = 1;
	lc.prob = _prob;
	lc.value = _value;

	push(lc);
}

uint64_t Lottery::getLimitProbilidade() {

	// Preenche roleta, para poder pegar o limite da probabilidade
	fill_roleta();

	return m_prob_limit;
}

uint32_t Lottery::getCountItem() {
	return (uint32_t)m_ctx.size();
}

Lottery::LotteryCtx* Lottery::spinRoleta(bool _remove_item_draw) {
	
	try {

		LotteryCtx *lc = nullptr;

		// Preencha a Roleta
		fill_roleta();

		uint64_t lucky = 0ull;

		shuffle_values_rand();

		lucky = (m_rand_values[(uint32_t)sRandomGen::getInstance().rIbeMt19937_64_chronoRange(0, 4)] * sRandomGen::getInstance().rDevice()) % (m_prob_limit == 0 ? 1 : m_prob_limit + 1);

		auto bound = m_roleta.equal_range(lucky);

		lc = (bound.first != m_roleta.end() ? bound.first->second : (bound.second != m_roleta.end() ? bound.second->second : nullptr));

		// Deleta o Item Sorteado, para n�o sair ele de novo, se for passado true
		if (_remove_item_draw)
			remove_draw_item(lc);

//#ifdef _DEBUG
//		for (auto& el : m_roleta)
//			_smp::message_pool::getInstance().push(new message("Value[Offset_s=" + std::to_string(el.second->offset[0]) + ", Offset_e=" + std::to_string(el.second->offset[1]) + ", Prob=" 
//					+ std::to_string(el.second->prob) + " value=" + std::to_string(el.second->value) + "]", CL_ONLY_FILE_LOG));
//#endif

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Lottery::spinRoleta][Log] m_prob_limit=" + std::to_string(m_prob_limit) + " Lucky=" + std::to_string(lucky) + " lower="
				+ (bound.first != m_roleta.end() ? std::to_string(bound.first->first) : std::string("NULL")) + ", upper=" + (bound.second != m_roleta.end() ? std::to_string(bound.second->first) : std::string("NULL")) + "", CL_FILE_LOG_AND_CONSOLE));
#endif

		return lc;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Lottery::spinRoleta][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}

	return nullptr;
}
