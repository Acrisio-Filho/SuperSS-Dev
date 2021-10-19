// Arquivo smart_calculator_player.hpp
// Criado em 21/11/2020 as 09:01 por Acrisio
// Defini��o da clase SmartCalculatorPlayer

#pragma once
#ifndef _STDA_SMART_CALCULATOR_PLAYER_HPP
#define _STDA_SMART_CALCULATOR_PLAYER_HPP

#include "../../Projeto IOCP/TYPE/smart_calculator_player_base.hpp"
#include "calculator_player.hpp"

namespace stdA {

	class SmartCalculatorPlayer : public ISmartCalculatorPlayer, public CalculatorPlayer {
		public:
			SmartCalculatorPlayer(uint32_t _id);
			virtual ~SmartCalculatorPlayer();

			virtual std::string cInfoToString();
			virtual std::string cToString();
			virtual std::string cStatsToString();
			virtual std::string cShotToString();
			virtual std::string cInputToString();
			virtual std::string cLastResultToString();
			virtual std::string cResolutionToString();
			virtual std::string cAutoFitToString();
			virtual std::string cDesvioFavoritoToString();

			virtual std::string cListDesvioFavoritoToString();
			virtual std::string cListClubToString();
			virtual std::string cListShotToString();
			virtual std::string cListPowerShotToString();
		
			virtual void clear() override;

		protected:
			virtual uint32_t getClubTypeLength();

		protected:
			virtual void initPacketResponse() override;

			static void responseCommandString(ICalculatorPlayer& _this, Packet< ePACKET >& _packet);
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

			virtual char* listClubToString() override;

			virtual char* listShotToString() override;

			virtual char* listPowerShotToString() override;

			virtual char* statsToString() override;

			virtual char* shotToString() override;

			virtual char* inputToString() override;

			virtual const char* getClubName(uint32_t _index) override;

			virtual const char* getShotName(uint32_t _index) override;

			virtual const char* getPowerShotName(uint32_t _index) override;

			virtual void freeResource(void* _resource) override;

			virtual void setGameShotValue(stGameShotValue& _gsv) override;

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

			virtual void club(const char* _args) override;
			virtual void shot(const char* _args) override;
			virtual void power_shot(const char* _args) override;
			virtual void power(const char* _args) override;
			virtual void ring(const char* _args) override;
			virtual void mascot(const char* _args) override;
			virtual void card(const char* _args) override;
			virtual void card_power_shot(const char* _args) override;
			virtual void distance(const char* _args) override;
			virtual void height(const char* _args) override;
			virtual void wind(const char* _args) override;
			virtual void degree(const char* _args) override;
			virtual void ground(const char* _args) override;
			virtual void spin(const char* _args) override;
			virtual void curve(const char* _args) override;
			virtual void slope_break(const char* _args) override;
			virtual void make_slope_break(const char* _args) override;
			virtual void aim_degree(const char* _args) override;

			// Translate Packet
			virtual void translatePacket(void* _packet) override;

		protected:
			bool m_special_slope_flag;

			float m_special_slope_break[3];
	};
}

#endif // !_STDA_SMART_CALCULATOR_PLAYER_HPP
