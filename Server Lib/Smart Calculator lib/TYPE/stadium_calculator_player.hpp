// Arquivo stadium_calculator_player.hpp
// Criado em 21/11/2020 as 11:51 por Acrisio
// Defini��o da classe StadiumCalculatorPlayer

#pragma once
#ifndef _STDA_STADIUM_CALCULATOR_PLAYER_HPP
#define _STDA_STADIUM_CALCULATOR_PLAYER_HPP

#include "../../Projeto IOCP/TYPE/smart_calculator_player_base.hpp"
#include "calculator_player.hpp"
#include "stadium_calculator_type.hpp"

namespace stdA {

	class StadiumCalculatorPlayer : public IStadiumCalculatorPlayer, public CalculatorPlayer {
		public:
			StadiumCalculatorPlayer(uint32_t _id);
			virtual ~StadiumCalculatorPlayer();

			virtual std::string cInfoToString();
			virtual std::string cToString();
			virtual std::string cShotToString();
			virtual std::string cInputToString();
			virtual std::string cLastResultToString();
			virtual std::string cResolutionToString();
			virtual std::string cAutoFitToString();
			virtual std::string cDesvioFavoritoToString();

			virtual std::string cListDesvioFavoritoToString();
			virtual std::string cListCalculatorToString();
			virtual std::string cListShotToString();
			virtual std::string cListShotBackSpinToString();
			virtual std::string cListShotDunkToString();
			virtual std::string cListShotTomahawkToString();
			virtual std::string cListShotSpikeToString();
			virtual std::string cListShotCobraToString();
			virtual std::string cListShotOnlyPangyaToString();

			virtual void clear() override;

		protected:
			virtual float getShotPower(uint32_t _index);
			virtual bool isValidShot(uint32_t _index);

		protected:
			virtual void initPacketResponse() override;

			static void responseCommandString(ICalculatorPlayer& _this, Packet< ePACKET >& _packet);
			static void responseListCalculator(ICalculatorPlayer& _this, Packet< ePACKET >& _packet);
			static void responseOpenCalculator(ICalculatorPlayer& _this, Packet< ePACKET >& _packet);
			static void responseCalcule(ICalculatorPlayer& _this, Packet< ePACKET >& _packet);
			static void responseMacro(ICalculatorPlayer& _this, Packet< ePACKET >& _packet);

		public:
			virtual char* infoToString() override;

			virtual char* toString() override;

			virtual char* lastResultToString() override;

			virtual char* resolutionToString() override;

			virtual char* autoFitToString() override;

			virtual char* desvioFavoritoToString() override;

			virtual char* listDesvioFavoritoToString() override;

			virtual char* listCalculatorToString() override;

			virtual char* listShotToString() override;

			virtual char* listShotBackSpinToString() override;

			virtual char* listShotDunkToString() override;

			virtual char* listShotTomahawkToString() override;

			virtual char* listShotSpikeToString() override;

			virtual char* listShotCobraToString() override;

			virtual char* listShotOnlyPangyaToString() override;

			virtual char* shotToString() override;

			virtual char* inputToString() override;

			virtual const char* getShotName(uint32_t _index) override;

			virtual void freeResource(void* _resource) override;

			// Commands
			virtual void ping(const char* _args) override;

			virtual void info(const char* _args) override;
			virtual void myInfo(const char* _args) override;

			virtual void list(const char* _args) override;

			virtual void calcule(const char* _args) override;

			virtual void expression(const char* _args) override;
			virtual void macro(const char* _args) override;

			virtual void last_result(const char* _args) override;
			virtual void resolution(const char* _args) override;
			virtual void desvio_favorito(const char* _args) override;
			virtual void auto_fit(const char* _args) override;

			virtual void mycella_degree(const char* _args) override;

			virtual void open(const char* _args) override;
			virtual void shot(const char* _args) override;
			virtual void distance(const char* _args) override;
			virtual void height(const char* _args) override;
			virtual void wind(const char* _args) override;
			virtual void degree(const char* _args) override;
			virtual void ground(const char* _args) override;
			virtual void slope_break(const char* _args) override;
			virtual void green_slope(const char* _args) override;

			// Translate Packet
			virtual void translatePacket(void* _packet) override;

		protected:
			stCalculator m_calculator;

			// const shot and club name
			std::string m_shot_name;

			std::vector< std::string > m_calculator_list;
	};
}

#endif // !_STDA_STADIUM_CALCULATOR_PLAYER_HPP
