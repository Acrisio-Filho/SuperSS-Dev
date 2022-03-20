// Arquivo cmd_coin_cube_location_info.hpp
// Criado em 16/10/2020 as 03:09 por Acrisio
// Defini��o da classe CmdCoinCubeLocationInfo

#pragma once
#ifndef _STDA_CMD_COIN_CUBE_LOCATION_INFO_HPP
#define _STDA_CMD_COIN_CUBE_LOCATION_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/hole_type.hpp"

#include <map>
#include <vector>

namespace stdA {

	typedef std::map< unsigned char/*Hole number*/, std::vector< CubeEx > > MAP_HOLE_COIN_CUBE;

	class CmdCoinCubeLocationInfo : public pangya_db {
		public:
			explicit CmdCoinCubeLocationInfo(bool _waiter = false);
			CmdCoinCubeLocationInfo(unsigned char _course, bool _waiter = false);
			virtual ~CmdCoinCubeLocationInfo();

			unsigned char getCourse();
			MAP_HOLE_COIN_CUBE& getInfo();

			void setCourse(unsigned char _course);

			std::vector< CubeEx > getAllCoinCubeHole(unsigned char _hole_number);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdCoinCubeLocationInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdCoinCubeLocationInfo"; };

		private:
			unsigned char m_course;
			MAP_HOLE_COIN_CUBE m_coin_cube;

			const char* m_szConsulta = "SELECT " DB_MAKE_ESCAPE_KEYWORD_A("index") ", course, hole, tipo, tipo_location, rate, x, y, z FROM pangya.pangya_coin_cube_location WHERE course = ";
	};
}

#endif // !_STDA_CMD_COIN_CUBE_LOCATION_INFO_HPP
