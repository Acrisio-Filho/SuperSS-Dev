// Arquivo course.cpp
// Criado em 12/08/2018 as 12:54 por Acrisio
// Implementa��o da classe Course

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "course.hpp"
#include "../UTIL/lottery.hpp"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include <algorithm>

#define ORCHID_BLOSSOM_ART	0x1A0001A4
#define PENNE_ABACUS_ART	0x1A0001A6
#define TITAN_WINDMILL_ART	0x1A0001A8

#define ONLY_1M_RULE			0x1A000265
#define SUPER_WIND_RULE			0x1A000266
#define HOLE_CUP_MAGNET_RULE	0x1A000269
#define NO_TURNING_BACK_RULE	0x1A00026A
#define WIND_3M_A_5M_RULE		0x1A00028F
#define WIND_7M_A_9M_RULE		0x1A000290

using namespace stdA;

Course::Course(RoomInfoEx& _ri, unsigned char _channel_rookie, float _star, uint32_t _rate_rain, unsigned char _rain_persist_flag)
	: m_ri(_ri), m_channel_rookie(_channel_rookie), m_star(_star), m_rate_rain(_rate_rain), m_rain_persist_flag(_rain_persist_flag), m_hole(), m_seq(),
		m_seed_rand_game(0u), m_flag_cube_coin(1u), m_wind_flag(0u), m_wind_range{ 0, 9 }, m_chr{ 0 }, m_holes_rain(0u), m_grand_prix_special_hole(false) {

	init_seq();

	init_hole();

	init_dados_rain();		// Inicializar os dados de chuva no course, para ser usado no achievement

	// Deixa esse s� com int16(short), por que s� vejo n�mero baixo, n�o passa do valor m�ximo do int16
	m_seed_rand_game = (unsigned short)sRandomGen::getInstance().rIbeMt19937_64_chrono();
}

Course::~Course() {

	if (!m_hole.empty())
		m_hole.clear();

	if (!m_seq.empty()) {
		m_seq.clear();
		m_seq.shrink_to_fit();
	}
}

uint32_t Course::getSeedRandGame() {
	return m_seed_rand_game;
}

unsigned short Course::getFlagCubeCoin() {
	return m_flag_cube_coin;
}

float Course::getStar() {
	return m_star;
}

Hole* Course::findHole(unsigned short _number) {

	if ((short)_number < 0)
		return nullptr;

	auto it = m_hole.end();

	if ((it = std::find_if(m_hole.begin(), m_hole.end(), [&](auto& _el) {
		return _el.second.getNumero() == _number;
	})) != m_hole.end())
		return &it->second;

	return nullptr;
}

Hole* Course::findHoleBySeq(unsigned short _seq) {

	if ((short)_seq <= 0 || _seq > m_hole.size())
		return nullptr;

	auto it = m_hole.find(_seq);
	
	if (it == m_hole.end())
		_smp::message_pool::getInstance().push(new message("[Course::findHoleBySeq][WARNIG] nao encontrou a seq[value=" 
				+ std::to_string(_seq) + "] no map de hole. Bug", CL_FILE_LOG_AND_CONSOLE));

	return (it != m_hole.end()) ? &it->second : nullptr;
}

unsigned short Course::findHoleSeq(unsigned short _number) {

	if ((short)_number < 0)
		return (unsigned short)~0;	// Error

	auto it = m_hole.end();

	if ((it = std::find_if(m_hole.begin(), m_hole.end(), [&](auto& _el) {
		return _el.second.getNumero() == _number;
	})) != m_hole.end())
		return it->first;

	return (unsigned short)0;
}

std::pair< std::map< unsigned short, Hole >::iterator, std::map< unsigned short, Hole >::iterator > Course::findRange(unsigned short _number) {

	if ((short)_number >= 0) {

		auto find = std::find_if(m_hole.begin(), m_hole.end(), [&](auto& _el) {
			return _el.second.getNumero() == _number;
		});

		return std::make_pair(find, m_hole.end());
	}

	return std::make_pair(m_hole.end(), m_hole.end());
}

stHoleWind Course::shuffleWind(uint32_t _seed) {

	stHoleWind wind{ 0 };

	if (m_wind_flag != 0) {

		do {
			
			// Update seed
			wind.wind = (unsigned char)(m_wind_range[0] + (sRandomGen::getInstance().rIbeMt19937_64_rdevice() % (m_wind_range[1] - m_wind_range[0])));

		} while ((m_wind_flag == 2) ? ((wind.wind + 1) % 2) : !((wind.wind + 1) % 2));

	}else {

		// Update seed
		wind.wind = (unsigned char)(m_wind_range[0] + (sRandomGen::getInstance().rIbeMt19937_64_rdevice() % (m_wind_range[1] - m_wind_range[0])));
	}

	wind.degree.setDegree(sRandomGen::getInstance().rIbeMt19937_64_chrono() % LIMIT_DEGREE);

	return wind;
}

void Course::shuffleWindNextHole(unsigned short _number) {

	if ((short)_number < 0)
		throw exception("[Course::shuffleWindNextHole][Error] _number[VALUE=" + std::to_string((short)_number) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COURSE, 1, 0));

	auto it = std::find_if(m_hole.begin(), m_hole.end(), [&](auto& _el) {
		return _el.second.getNumero() == _number;
	});

	if (it == m_hole.end())
		throw exception("[Course::shuffleWindNextHole][Error] nao conseguiu encontrar o hole[NUMERO=" + std::to_string(_number) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COURSE, 2, 0));

	auto wind = shuffleWind((uint32_t)(size_t)this);

	for (; it != m_hole.end(); ++it)
		it->second.setWind(wind);
}

void Course::makePacketHoleInfo(packet& _p, int _option) {

	// Hole(s) Info
	for (auto& el : m_hole) {
		_p.addUint32(el.second.getId());
		_p.addUint8(el.second.getPin());

		if (_option == 0)
			_p.addUint8(el.second.getCourse());
		
		_p.addUint8((unsigned char)el.second.getNumero());
	}

	// Course Seed Random
	_p.addUint32(m_seed_rand_game);

	// Hole(s) Spinning Cube / Coin Info
	makePacketHoleSpinningCubeInfo(_p);
}

void Course::makePacketHoleSpinningCubeInfo(packet& _p) {

	for (auto& el : m_hole) {

		_p.addUint8((unsigned char)el.second.getCubes().size());	// Size

		for (auto& el2 : el.second.getCubes()) {
			_p.addUint32(el2.tipo);
			_p.addUint32(el2.id);
			_p.addUint32(el2.flag_unknown);
			_p.addUint32(el.second.getCourse());
			_p.addUint8((unsigned char)(el.second.getModo() == Hole::M_REPEAT ? el.second.getHoleRepeat() : el.second.getNumero()));
			_p.addUint8((unsigned char)el.first - 1);	// Index
			_p.addUint16(m_flag_cube_coin);
			_p.addBuffer(&el2.location, sizeof(el2.location));
			_p.addUint32(el2.flag_location);
		}
	}
}

uint32_t Course::countHolesRain() {
	return m_holes_rain.getCountHolesRain();
}

uint32_t Course::countHolesRainBySeq(uint32_t _seq) {
	return m_holes_rain.getCountHolesRainBySeq(_seq);
}

float Course::getMediaAllParHoles() {

	if (m_hole.empty())
		return 1.0f;	// N�o tem nenhum hole inicializado

	uint32_t count = 0u;

	for (auto& el : m_hole)
		count += el.second.getPar().par;

	return (float)(count / (float)m_hole.size());
}

float Course::getMediaAllParHolesBySeq(uint32_t _seq) {

	if (_seq <= 0 || _seq > m_hole.size())
		return 1.0f;	// Sequ�ncia inv�lida

	if (m_hole.empty())
		return 1.0f;	// N�o tem nenhum hole inicializado

	uint32_t count = 0u;

	for (auto it = m_hole.begin(); it != m_hole.end() && it->first <= _seq; ++it)
		count += it->second.getPar().par;

	return (float)(count / (float)_seq);
}

ConsecutivosHolesRain& Course::getConsecutivesHolesRain() {
	return m_chr;
}

void Course::init_seq() {

	// Verifica se � Grand Prix e se tem Special Hole
	if (m_ri.grand_prix.active && m_ri.grand_prix.dados_typeid > 0) {

		// Grand Prix Special Hole
		auto sh = sIff::getInstance().findGrandPrixSpecialHole(m_ri.grand_prix.rank_typeid);

		if (!sh.empty()) {

			// Sort Sequencie
			// Ordena do menor para o maior por sequ�ncia do hole
			std::sort(sh.begin(), sh.end(), [&](auto& _1, auto& _2) {
				return _1.seq < _2.seq;
			});

			for (auto& el : sh)
				m_seq.push_back(Sequencia((unsigned char)el.course, (unsigned short)el.hole));

			// Completa os 18 Holes
			if (m_seq.size() < 18) {

				for (auto i = m_seq.size() + 1; i <= 18u; ++i)
					m_seq.push_back(Sequencia((unsigned char)m_ri.course/*Course padr�o do Grand Prix*/, (unsigned short)i));
			}

			// Tem Special Hole
			m_grand_prix_special_hole = true;

			// Sai da fun��o por que os holes speciais do Prand Prix
			return;
		}
	}

	// Normal
	switch (m_ri.modo) {
	case Hole::eMODO::M_FRONT:
	case Hole::eMODO::M_REPEAT:
		// !@ Teste
#ifdef _DEBUG
		for (auto i = 1u; i <= 18u; i++) m_seq.push_back(Sequencia((unsigned short)(i/* % 2 + 1*/)));
#else
		for (auto i = 1u; i <= 18u; ++i) m_seq.push_back(Sequencia((unsigned short)i));
#endif
		break;
	case Hole::eMODO::M_BACK:
	{
		auto i = 10u;
		for (; i <= 18; ++i) m_seq.push_back(Sequencia((unsigned short)i));
		for (i = 1u; i < 10; ++i) m_seq.push_back(Sequencia((unsigned short)i));
		break;
	}
	case Hole::eMODO::M_RANDOM:
	{
		unsigned short rand = (sRandomGen::getInstance().rIbeMt19937_64_chrono() % 18) + 1;
		for (auto i = rand; i <= 18; ++i) m_seq.push_back(Sequencia((unsigned short)i));
		if (rand > 1) for (auto i = 1u; i < rand; ++i) m_seq.push_back(Sequencia((unsigned short)i));
		break;
	}
	case Hole::eMODO::M_SHUFFLE:
	{
		Lottery lottery((uint64_t)this);

		for (auto i = 1u; i <= 18; ++i) lottery.push(1000, i);

		Lottery::LotteryCtx *lc = nullptr;

		for (auto i = 0u; i < 18; ++i)
			if ((lc = lottery.spinRoleta(true)) != nullptr)
				m_seq.push_back(Sequencia((unsigned short)lc->value));

		break;
	}
	case Hole::eMODO::M_SHUFFLE_COURSE:
	{

		unsigned short hole_ssc = (sRandomGen::getInstance().rIbeMt19937_64_chrono() % 2) + 1;

		Lottery lottery((uint64_t)this);

		for (auto i = 1u; i <= 18; ++i) lottery.push(1000, i);

		Lottery::LotteryCtx *lc = nullptr;

		// 17 Holes S�
		for (auto i = 0u; i < 18; ++i)
			if ((lc = lottery.spinRoleta(true)) != nullptr && lc->value != hole_ssc)
				m_seq.push_back(Sequencia((unsigned short)lc->value));

		// ultimo Hole � do SSC
		m_seq.push_back(Sequencia((unsigned short)hole_ssc));

		break;
	}	// End Case M_SHUFFLE_COURSE
	}	// End Switch

}

void Course::init_hole() {

	uCubeCoinFlag cube_coin{ 0 };

	// Enable Coin e Cube in Course Default
	cube_coin.stFlag.enable = 1u;
	cube_coin.stFlag.enable_coin = 1u;

	// Type Cube Game Mode
	if (m_ri.modo == Hole::eMODO::M_REPEAT)
		cube_coin.stFlag.type = 1u;
	else if (m_ri.tipo == RoomInfo::TIPO::STROKE || m_ri.tipo == RoomInfo::TOURNEY || m_ri.tipo == RoomInfo::GUILD_BATTLE
			|| m_ri.tipo == RoomInfo::MATCH || m_ri.tipo == RoomInfo::PRACTICE || m_ri.tipo == RoomInfo::SPECIAL_SHUFFLE_COURSE)
		cube_coin.stFlag.type = 2u;

	switch (m_ri.artefato) {
	case ORCHID_BLOSSOM_ART:	// 1 a 8m
		m_wind_range[1] = 8;
		break;
	case PENNE_ABACUS_ART:		// Wind Impar
		m_wind_flag = 1;
		break;
	case TITAN_WINDMILL_ART:	// Wind Par
		m_wind_flag = 2;
		break;
	}

	if (m_ri.grand_prix.active && m_ri.grand_prix.dados_typeid > 0) {

		// Grand Prix n�o tem cube
		cube_coin.stFlag.enable = 0u;
		
		try {

			auto gp = sIff::getInstance().findGrandPrixData(m_ri.grand_prix.dados_typeid);

			// Grand Prix Data -> Rule
			if (gp != nullptr) {

				// Aqui inicializa as regras do Grand Prix de vento
				switch (gp->rule) {
				case ONLY_1M_RULE:
					m_wind_range[1] = 1;
					break;
				case SUPER_WIND_RULE:
					m_wind_range[0] = 9;
					m_wind_range[1] = 15;
					break;
				case HOLE_CUP_MAGNET_RULE:	// Ainda n�o sei esses aqui, como funciona
				case NO_TURNING_BACK_RULE:	// Ainda n�o sei esses aqui, como funciona
					break;
				case WIND_3M_A_5M_RULE:
					m_wind_range[0] = 2;
					m_wind_range[1] = 5;
					break;
				case WIND_7M_A_9M_RULE:
					m_wind_range[0] = 6;
					break;
				}
		
			}else
				_smp::message_pool::getInstance().push(new message("[Course::init_hole][Error] tentou pegar o Grand Prix[TYPEID=" 
						+ std::to_string(m_ri.grand_prix.dados_typeid) + "] no IFF_STRUCT do server mais ele nao existe. Bug", CL_FILE_LOG_AND_CONSOLE));
		
		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Course::init_hole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}

	}else if (m_channel_rookie == 1/*Channel Rookie*/)
		m_wind_range[1] = 5;

	unsigned char new_course = m_ri.course & 0x7F;
	unsigned char pin = 0u;
	unsigned char weather = 0u;

	unsigned char persist_rain = 0u;

	stHoleWind wind{ 0 };

	// Lottery Wind
	Lottery loterry((uint64_t)this);

	auto rate_good_weather = (m_rate_rain <= 0) ? 1000 : ((m_rate_rain < 1000) ? 1000 - m_rate_rain : 1);

	// Coloquei 4 pra 1, antes estava 3 pra 1
	loterry.push(rate_good_weather, 0);
	loterry.push(rate_good_weather, 0);
	loterry.push(rate_good_weather, 0);
	loterry.push(rate_good_weather, 0);
	loterry.push(m_rate_rain, 2/*Rain*/);

	// Lottery Course
	Lottery lottery_map((uint64_t)this);

	uint32_t course_id = 0u;

	for (auto& el : sIff::getInstance().getCourse()) {

		course_id = sIff::getInstance().getItemIdentify(el.second._typeid);

		if (course_id != 17/*SSC*/ && course_id != 0x40/*GRAND ZODIAC*/)
			lottery_map.push(100, course_id);
	}
		
	for (auto i = 1u; i <= 18u; ++i) {

		// Reseta flag cube
		cube_coin.stFlag.enable_cube = 0u;
		cube_coin.stFlag.enable_coin = 0u;

		if (i <= m_ri.qntd_hole) {

			if (m_ri.modo == Hole::eMODO::M_REPEAT && i == 1)
				wind = shuffleWind(i);
			else if (m_ri.modo != Hole::eMODO::M_REPEAT)
				wind = shuffleWind(i);

			if (m_ri.fixed_hole == 7 && i == 1)
				pin = sRandomGen::getInstance().rIbeMt19937_64_rdevice() % 3;
			else if (m_ri.fixed_hole != 7)
				pin = sRandomGen::getInstance().rIbeMt19937_64_rdevice() % 3;

			weather = 0u;

			auto lc = loterry.spinRoleta();

			if (lc != nullptr)
				weather = (unsigned char)lc->value;

			if (persist_rain || weather == 2) {

				if (!persist_rain && weather == 2 && m_rain_persist_flag)
					persist_rain = 1;
				else if (persist_rain) {
					weather = 2;
					persist_rain = 0;
				}

				try {
					if (i > 1 && m_hole.at(i - 1).getWeather() == 0)
						m_hole.at(i - 1).setWeather(1);
				}catch (std::out_of_range& e) {
					UNREFERENCED_PARAMETER(e);
				}
			}

			if (m_ri.tipo == RoomInfo::SPECIAL_SHUFFLE_COURSE && m_ri.modo == Hole::eMODO::M_SHUFFLE_COURSE) {

				if (i == 18)	// Ultimo Hole � do SSC
					new_course = RoomInfo::CHRONICLE_1_CHAOS;
				else {
					
					auto lc = lottery_map.spinRoleta();

					if (lc != nullptr)
						new_course = (unsigned char)lc->value;
				}
			}

			// Cube a cada 3 hole
			if (i % 3 == 0)
				cube_coin.stFlag.enable_cube = 1u;

			// Coin todos os holes
			if (cube_coin.stFlag.enable)
				cube_coin.stFlag.enable_coin = 1u;

			if (m_ri.grand_prix.active && m_ri.grand_prix.dados_typeid > 0 && m_grand_prix_special_hole) {
				
				// A fun��o init_seq j� inicializa a sequ�ncia se for Grand Prix e se ele tiver Special Hole
				m_hole.insert(std::make_pair((unsigned short)i, Hole(m_seq[i - 1].m_course, m_seq[i - 1].m_hole, pin, Hole::eMODO(m_ri.modo), (unsigned char)m_ri.hole_repeat, weather, wind.wind, wind.degree.getDegree(), cube_coin)));
			
			}else
				m_hole.insert(std::make_pair((unsigned short)i, Hole(new_course, m_seq[i - 1].m_hole, pin, Hole::eMODO(m_ri.modo), (unsigned char)m_ri.hole_repeat, weather, wind.wind, wind.degree.getDegree(), cube_coin)));

		}else
			m_hole.insert(std::make_pair((unsigned short)i, Hole(new_course, m_seq[i - 1].m_hole, sRandomGen::getInstance().rIbeMt19937_64_rdevice() % 3, Hole::eMODO(m_ri.modo), (unsigned char)m_ri.hole_repeat, weather, wind.wind, wind.degree.getDegree(), cube_coin)));
	}

}

void Course::init_dados_rain() {

	// Inicializa dados de chuva em holes consecutivos
	m_chr.clear();

	// Inicializa dados do n�mero de holes com chuva
	m_holes_rain.clear();

	uint32_t count = 0u;

	for (auto& el : m_hole) {

		// Quantidade de holes que tem o Game
		if (el.first <= m_ri.qntd_hole) {

			if (el.second.getWeather() == 2) {
				
				// Chuva
				m_holes_rain.setRain(el.first - 1, 1u);
				
				count++;
			}
			
			// �ltimo hole ou acabou a sequ�ncia de chuva consecutivas
			if (count > 1u && (el.second.getWeather() != 2 || el.first == m_ri.qntd_hole)) {

				if (count >= 4)			// 4 ou mais Holes consecutivos
					m_chr._4_pluss_count.setRain(el.first - 1, 1u);
				else if (count == 3)	// 3 Holes consecutivos
					m_chr._3_count.setRain(el.first - 1, 1u);
				else					// 2 Holes consecutivos
					m_chr._2_count.setRain(el.first - 1, 1u);

				// Zera
				count = 0u;
			}
		}
	}

#ifdef _DEBUG
	// teste
	auto c = m_holes_rain.getCountHolesRainBySeq(15);
	auto c2 = m_holes_rain.getCountHolesRainBySeq(18);
	auto c3 = m_holes_rain.getCountHolesRainBySeq(3);
	auto c4 = m_holes_rain.getCountHolesRain();

	_smp::message_pool::getInstance().push(new message("[Course::init_dados_rain][Log] Count Holes Rain: SEQ(15)[VALUE=" + std::to_string(c) + "], SEQ(18)[VALUE=" + std::to_string(c2) 
			+ "], SEQ(3)[VALUE=" + std::to_string(c3) + "] e SEQ(1~18)[VALUE=" + std::to_string(c4) + "]", CL_FILE_LOG_AND_CONSOLE));

	auto cc2 = m_chr._2_count.getCountHolesRainBySeq(15);
	auto cc3 = m_chr._3_count.getCountHolesRainBySeq(15);
	auto cc4 = m_chr._4_pluss_count.getCountHolesRainBySeq(15);

	auto cd2 = m_chr._2_count.getCountHolesRainBySeq(18);
	auto cd3 = m_chr._3_count.getCountHolesRainBySeq(18);
	auto cd4 = m_chr._4_pluss_count.getCountHolesRainBySeq(18);

	auto ce2 = m_chr._2_count.getCountHolesRainBySeq(3);
	auto ce3 = m_chr._3_count.getCountHolesRainBySeq(3);
	auto ce4 = m_chr._4_pluss_count.getCountHolesRainBySeq(3);

	auto cf2 = m_chr._2_count.getCountHolesRain();
	auto cf3 = m_chr._3_count.getCountHolesRain();
	auto cf4 = m_chr._4_pluss_count.getCountHolesRain();

	_smp::message_pool::getInstance().push(new message("[Course::init_dados_rain][Log] Count Consecutives Holes Rain: SEQ(15)[C2=" + std::to_string(cc2) + ", C3=" + std::to_string(cc3) + ", C4=" + std::to_string(cc4) 
			+ "], SEQ(18)[C2=" + std::to_string(cd2) + ", C3=" + std::to_string(cd3) + ", C4=" + std::to_string(cd4) + "], SEQ(3)[C2=" + std::to_string(ce2) + ", C3=" + std::to_string(ce3) + ", C4=" + std::to_string(ce4) 
			+ "] e SEQ(1~18)[C2=" + std::to_string(cf2) + ", C3=" + std::to_string(cf3) + ", C4=" + std::to_string(cf4) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG


	//m_holes_rain = std::count_if(m_hole.begin(), m_hole.end(), [&](auto& el) {
	//	return (el.first <= m_ri.qntd_hole/*Quantidade de holes que tem o Game*/ && el.second.getWeather() == 2);
	//});
}
