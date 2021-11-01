// Arquivo smart_calculator_player_base.hpp
// Criado em 21/11/2020 as 09:04 por Acrisio
// Defini��o da classe(interface) ISmartCalculatorPlayer

#pragma once
#ifndef _STDA_SMART_CALCULATOR_PLAYER_BASE_HPP
#define _STDA_SMART_CALCULATOR_PLAYER_BASE_HPP

#include <string>
#include <vector>
#include <map>
#include <cmath>

namespace stdA {

	constexpr float YARD_VALUE				= 1.f;
	constexpr float POWER_BAR_VALUE			= 0.2167f;
	constexpr float POWER_BAR_AEREA_VALUE	= 0.8668f;
	constexpr float POWER_BAR_PLUS_VALUE	= 1.032f;

	constexpr uint32_t BASE_RESOLUTION_WIDTH	= 640u;
	constexpr uint32_t BASE_RESOLUTION_HEIGHT	= 480u;

	// Resolu��o 600 x 480
	constexpr float DENSIDADE_PIXEL_480_PANGYA = 0.006f;

	template<typename _Digit> _Digit fix(_Digit _value) {

		if (_value < _Digit(0))
			_value = std::ceil(_value);
		else
			_value = std::floor(_value);

		return _value;
	};

	// Result
	struct stResult {
		public:
			stResult(uint32_t _ul = 0u) : 
				desvio(0.f),
				percent(0.f),
				caliper(0.f),
				hwi(0.f),
				spin(0.f)
			{};

			void clear() {
				
				desvio = 0.f;
				percent = 0.f;
				caliper = 0.f;
				hwi = 0.f;
				spin = 0.f;
			};

		public:
			float desvio;
			float percent;
			float caliper;
			float hwi;
			float spin;
	};

	// Resolution
	struct stResolution {
		public:
			stResolution(uint32_t _ul = 0u) : 
				width(800u),
				height(600u)
			{};

			void clear() {

				width = 800u;
				height = 600u;
			};

		public:
			uint32_t width;
			uint32_t height;
	};

	// Type devio favorito
	enum class eTYPE_DESVIO_FAVORITO : uint32_t {
		YARD,
		POWER_BAR,
		POWER_BAR_AEREA,
		POWER_BAR_PLUS,
		CLICK_MORE_YARD,
		CLICK_MORE_POWER_BAR,
		POWER_BAR_AEREA_MORE_POWER_BAR,
		POWER_BAR_PLUS_MORE_POWER_BAR,
		END,
	};

	static std::map< eTYPE_DESVIO_FAVORITO, std::string > typeDesvioFavoritoName{
		{
			eTYPE_DESVIO_FAVORITO::YARD,
			"YARD"
		},
		{
			eTYPE_DESVIO_FAVORITO::POWER_BAR,
			"POWER BAR"
		},
		{
			eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA,
			"POWER BAR AEREA"
		},
		{
			eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS,
			"POWER BAR PLUS"
		},
		{
			eTYPE_DESVIO_FAVORITO::CLICK_MORE_YARD,
			"CLICK + YARD"
		},
		{
			eTYPE_DESVIO_FAVORITO::CLICK_MORE_POWER_BAR,
			"CLICK + POWER BAR"
		},
		{
			eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA_MORE_POWER_BAR,
			"POWER BAR AEREA + POWER BAR"
		},
		{
			eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS_MORE_POWER_BAR,
			"POWER BAR PLUS + POWER BAR"
		},
		{
			eTYPE_DESVIO_FAVORITO::END,
			"END"
		}
	};

	inline std::string getTypeDesvioFavoritoName(eTYPE_DESVIO_FAVORITO _type) {

		auto it = typeDesvioFavoritoName.find(_type);

		return it != typeDesvioFavoritoName.end() ? it->second : "UNKNOWN";
	};

	// Defini��o da classe(interface) ICalculatorPlayer
	class ICalculatorPlayer {
		public:
			ICalculatorPlayer(uint32_t _id) :
				m_id(_id),
				m_last_result(),
				m_resolution(),
				m_desvio_fav(eTYPE_DESVIO_FAVORITO::YARD),
				m_auto_fit(false)
			{};
			virtual ~ICalculatorPlayer() {};

			virtual void clear() = 0;

			virtual char* infoToString() = 0;

			virtual char* toString() = 0;

			virtual char* lastResultToString() = 0;

			virtual char* resolutionToString() = 0;

			virtual char* autoFitToString() = 0;

			virtual char* desvioFavoritoToString() = 0;

			virtual char* listDesvioFavoritoToString() = 0;

			virtual void freeResource(void* _resource) = 0;

			// Commands
			virtual void ping(const char* _args) = 0;

			virtual void info(const char* _args) = 0;
			virtual void myInfo(const char* _args) = 0;

			virtual void list(const char* _args) = 0;

			virtual void calcule(const char* _args) = 0;

			virtual void expression(const char* _args) = 0;
			virtual void macro(const char* _args) = 0;

			virtual void last_result(const char* _args) = 0;
			virtual void resolution(const char* _args) = 0;
			virtual void desvio_favorito(const char* _args) = 0;
			virtual void auto_fit(const char* _args) = 0;

			virtual void mycella_degree(const char* _args) = 0;

			// Translate Packet
			virtual void translatePacket(void* _packet) = 0;

		public:
			uint32_t m_id;

			stResult m_last_result;
			stResolution m_resolution;

			eTYPE_DESVIO_FAVORITO m_desvio_fav;

			bool m_auto_fit;
	};

	inline float getClickByResolution(stResolution& _resolution) {

		float value = 1.f;

		if (_resolution.width < BASE_RESOLUTION_WIDTH || _resolution.height < BASE_RESOLUTION_HEIGHT)
			return value;

		value = (float(BASE_RESOLUTION_HEIGHT) / _resolution.height) * DENSIDADE_PIXEL_480_PANGYA;

		value = value * (_resolution.width / 2.f);

		if (value <= 0.f)
			return 1.f;

		return value;
	};

	inline float getSlopeByResolution(stResolution& _resolution, bool _auto_fit) {

		float value = 1.f;

		if (_resolution.height < BASE_RESOLUTION_HEIGHT)
			return value;

		value = _resolution.height / float(BASE_RESOLUTION_HEIGHT);

		if (!_auto_fit)
			value = fix(value);

		if (value == 0.f)
			value = 1.f;

		return value;
	};

	inline float getDesvioFavByType(ICalculatorPlayer& _iCalc) {

		float value = YARD_VALUE;

		switch (_iCalc.m_desvio_fav) {
		case eTYPE_DESVIO_FAVORITO::YARD:
		default:
			value = YARD_VALUE;
			break;
		case eTYPE_DESVIO_FAVORITO::POWER_BAR:
			value = POWER_BAR_VALUE;
			break;
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA_MORE_POWER_BAR:
			value = POWER_BAR_AEREA_VALUE;
			break;
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS_MORE_POWER_BAR:
			value = POWER_BAR_PLUS_VALUE;
			break;
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_YARD:
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_POWER_BAR:
			value = getClickByResolution(_iCalc.m_resolution);
			break;
		}

		return value;
	};

	inline float desvioByScala(float _desvio, ICalculatorPlayer& _iCalc) {

		float value = _desvio;
		float scala = getDesvioFavByType(_iCalc);

		switch (_iCalc.m_desvio_fav) {
		case eTYPE_DESVIO_FAVORITO::YARD:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS:
		default:
			value /= (scala <= 0.f) ? 1.f : scala;
			break;
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_YARD:

			scala = (scala <= 0.f) ? 1.f : scala;

			value = fix(value / scala);
			break;
		}

		return value;
	};

	inline float restOfDesvioByScala(float _desvio, ICalculatorPlayer& _iCalc) {
		
		float value = 0.f;
		float scala = getDesvioFavByType(_iCalc);

		switch (_iCalc.m_desvio_fav) {
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_YARD:
#if defined(_WIN32)
			value = std::fmodf(_desvio, (scala <= 0.f ? 1.f : scala));
#elif defined(__linux__)
			value = fmodf(_desvio, (scala <= 0.f ? 1.f : scala));
#endif
			break;
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_POWER_BAR:
#if defined(_WIN32)
			value = std::fmodf(_desvio, (scala <= 0.f ? 1.f : scala)) / POWER_BAR_VALUE;
#elif defined(__linux__)
			value = fmodf(_desvio, (scala <= 0.f ? 1.f : scala)) / POWER_BAR_VALUE;
#endif
			break;
		}

		return value;
	};

	inline bool isMoreResult(eTYPE_DESVIO_FAVORITO _type) {

		bool ret = false;

		switch (_type) {
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_YARD:
			ret = true;
			break;
		}

		return ret;
	};

	inline std::string getDesvioLegendByType(eTYPE_DESVIO_FAVORITO _type) {

		std::string ret = "";

		switch (_type) {
		case eTYPE_DESVIO_FAVORITO::YARD:
		default:
			ret = "y";
			break;
		case eTYPE_DESVIO_FAVORITO::POWER_BAR:
			ret = "pb";
			break;
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA_MORE_POWER_BAR:
			ret = "pba";
			break;
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS_MORE_POWER_BAR:
			ret = "pba+";
			break;
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_YARD:
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_POWER_BAR:
			ret = "c";
			break;
		}

		return ret;
	};

	inline std::string getDesvioMoreLegendByType(eTYPE_DESVIO_FAVORITO _type) {

		std::string ret = "";

		switch (_type) {
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_AEREA_MORE_POWER_BAR:
		case eTYPE_DESVIO_FAVORITO::POWER_BAR_PLUS_MORE_POWER_BAR:
			ret = "pb";
			break;
		case eTYPE_DESVIO_FAVORITO::CLICK_MORE_YARD:
			ret = "y";
			break;
		}

		return ret;
	}

	// Game Values Shot
	struct stGameShotValue {
		public:
			stGameShotValue(uint32_t _ul = 0u) :
				gm(false), distance(0.f), wind(0.f), degree(0.f), mira_rad(0.f),
				power_range(230.f), power_slot(0u), auxpart_pwr(0), mascot_pwr(0),
				card_pwr(0), ps_card_pwr(0), safety(false), ground(false), rain(false)
			{};

			void clear() {

				gm = false;

				distance = 0.f;
				wind = 0.f;
				degree = 0.f;
				mira_rad = 0.f;

				power_range = 230.f;

				power_slot = 0u;

				auxpart_pwr = 0;
				mascot_pwr = 0;
				card_pwr = 0;
				ps_card_pwr = 0;

				safety = false;
				ground = false;
				rain = false;
			};

		public:
			float distance;
			float wind;
			float degree;

			float power_range;

			float mira_rad;

			unsigned char power_slot;

			// Pode ser negativo esses
			char auxpart_pwr;
			char mascot_pwr;
			char card_pwr;
			char ps_card_pwr;

			// Safety e Ground flag
			bool safety;
			bool ground;

			// Rain flag
			bool rain;

			bool gm;
	};

	// Defini��o da classe(interface) Smart Calculator Player
	class ISmartCalculatorPlayer : public ICalculatorPlayer {
		public:
			ISmartCalculatorPlayer(uint32_t _id) : ICalculatorPlayer(_id), m_game_shot_value() {};
			virtual ~ISmartCalculatorPlayer() {};

			virtual char* statsToString() = 0;

			virtual char* shotToString() = 0;

			virtual char* inputToString() = 0;

			virtual char* listClubToString() = 0;

			virtual char* listShotToString() = 0;

			virtual char* listPowerShotToString() = 0;

			virtual const char* getClubName(uint32_t _index) = 0;

			virtual const char* getShotName(uint32_t _index) = 0;

			virtual const char* getPowerShotName(uint32_t _index) = 0;

			virtual void setGameShotValue(stGameShotValue& _gsv) = 0;

			// Commands
			virtual void club(const char* _args) = 0;
			virtual void shot(const char* _args) = 0;
			virtual void power_shot(const char* _args) = 0;
			virtual void power(const char* _args) = 0;
			virtual void ring(const char* _args) = 0;
			virtual void mascot(const char* _args) = 0;
			virtual void card(const char* _args) = 0;
			virtual void card_power_shot(const char* _args) = 0;
			virtual void distance(const char* _args) = 0;
			virtual void height(const char* _args) = 0;
			virtual void wind(const char* _args) = 0;
			virtual void degree(const char* _args) = 0;
			virtual void ground(const char* _args) = 0;
			virtual void spin(const char* _args) = 0;
			virtual void curve(const char* _args) = 0;
			virtual void slope_break(const char* _args) = 0;
			virtual void make_slope_break(const char* _args) = 0;
			virtual void aim_degree(const char* _args) = 0;

		public:
			unsigned char m_power;

			// Pode ser negativo esses
			char m_auxpart_pwr;
			char m_mascot_pwr;
			char m_card_pwr;
			char m_ps_card_pwr;

			unsigned char m_club_index;
			unsigned char m_shot_index;
			unsigned char m_power_shot_index;

			float m_distance;
			float m_height;
			float m_wind;
			float m_degree;
			float m_ground;
			float m_spin;
			float m_curve;
			float m_slope_break;

			stGameShotValue m_game_shot_value;
	};

	// Defini��o da classe(interface) Smart Calculator Player
	class IStadiumCalculatorPlayer : public ICalculatorPlayer {
	public:
		IStadiumCalculatorPlayer(uint32_t _id) : ICalculatorPlayer(_id) {};
		virtual ~IStadiumCalculatorPlayer() {};

		virtual char* shotToString() = 0;

		virtual char* inputToString() = 0;

		virtual char* listCalculatorToString() = 0;

		virtual char* listShotToString() = 0;

		virtual char* listShotBackSpinToString() = 0;
		
		virtual char* listShotDunkToString() = 0;

		virtual char* listShotTomahawkToString() = 0;

		virtual char* listShotSpikeToString() = 0;

		virtual char* listShotCobraToString() = 0;

		virtual char* listShotOnlyPangyaToString() = 0;

		virtual const char* getShotName(uint32_t _index) = 0;

		// Commands
		virtual void open(const char* _args) = 0;
		virtual void shot(const char* _args) = 0;
		virtual void distance(const char* _args) = 0;
		virtual void height(const char* _args) = 0;
		virtual void wind(const char* _args) = 0;
		virtual void degree(const char* _args) = 0;
		virtual void ground(const char* _args) = 0;
		virtual void slope_break(const char* _args) = 0;
		virtual void green_slope(const char* _args) = 0;

	public:
		unsigned char m_shot_index;

		float m_distance;
		float m_height;
		float m_wind;
		float m_degree;
		float m_ground;
		float m_slope_break;
		float m_green_slope;
	};

	// Block Pointer Calculator Player
	template<class _Ptr> class BlockPointerSmart {
	public:
		BlockPointerSmart(ICalculatorPlayer& _this, _Ptr _ptr = nullptr) : m_this(_this), m_ptr(_ptr) {};
		virtual ~BlockPointerSmart() {

			if (m_ptr != nullptr)
				m_this.freeResource(m_ptr);

			m_ptr = nullptr;
		};

	public:
		ICalculatorPlayer& m_this;
		_Ptr m_ptr;
	};
}

#endif // !_STDA_SMART_CALCULATOR_PLAYER_BASE_HPP
