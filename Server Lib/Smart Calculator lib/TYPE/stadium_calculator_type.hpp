// Arquivo stadium_calculator_type.hpp
// Criado em 25/11/2020 as 09:45 por Acrisio
// Defini��o dos tipos usados no Stadium Calculator

#pragma once
#ifndef _STDA_STADIUM_CALCULATOR_TYPE_HPP
#define _STDA_STADIUM_CALCULATOR_TYPE_HPP

#include <string>
#include <vector>

namespace stdA {

	enum class eTYPE_SHOT_ID : uint32_t {
		COBRA,
		BACK_SPIN,
		DUNK,
		ONLY_PANGYA,
		SPIKE,
		TOMAHAWK,
		INVALID_SHOT = (uint32_t)~0u,
	};

	struct stClub {
		public:
			stClub() :
				m_index((uint32_t)~0u),
				m_name(),
				m_power(230.f)
			{};

			stClub(uint32_t _index, std::string _name, float _power) :
				m_index(_index), m_name(_name), m_power(_power)
			{};
			
			void clear() {

				m_index = (uint32_t)~0u;
				m_power = 230.f;
				
				if (!m_name.empty()) {
					m_name.clear();
					m_name.shrink_to_fit();
				}
			};

		public:
			uint32_t m_index;
			std::string m_name;

			float m_power;
	};

	struct stShot {
		public:
			stShot() :
				m_id(eTYPE_SHOT_ID::INVALID_SHOT),
				m_name(),
				m_club()
			{};
			
			stShot(eTYPE_SHOT_ID _id, std::string _name) :
				m_id(_id), m_name(_name), m_club()
			{};

			void clear() {

				m_id = eTYPE_SHOT_ID::INVALID_SHOT;

				if (!m_name.empty()) {
					m_name.clear();
					m_name.shrink_to_fit();
				}

				if (!m_club.empty()) {
					m_club.clear();
					m_club.shrink_to_fit();
				}
			};

		public:
			eTYPE_SHOT_ID m_id;
			std::string m_name;

			std::vector< stClub > m_club;
	};

	struct stCalculator {
		public:
			stCalculator() :
				m_id((uint32_t)~0u),
				m_name(),
				m_shot()
			{};

			stCalculator(uint32_t _id, std::string _name) :
				m_id(_id), m_name(_name), m_shot()
			{};

			void clear() {

				m_id = (uint32_t)~0u;
				
				if (!m_name.empty()) {
					m_name.clear();
					m_name.shrink_to_fit();
				}

				if (!m_shot.empty()) {
					m_shot.clear();
					m_shot.shrink_to_fit();
				}
			};

		public:
			uint32_t m_id;
			std::string m_name;

			std::vector< stShot > m_shot;
	};
}

#endif // !_STDA_STADIUM_CALCULATOR_TYPE_HPP
