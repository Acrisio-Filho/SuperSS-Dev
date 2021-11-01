// Arquivo pangya_game_st.h
// Criado em 13/08/2017 por Acrisio
// Definição das estruturas usadas no pangya game

#pragma once
#ifndef _STDA_PANGYA_GAME_ST_H
#define _STDA_PANGYA_GAME_ST_H

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../../Projeto IOCP/UTIL/exception.h"

#include "../../Projeto IOCP/TYPE/pangya_st.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

#include <vector>
#include <map>

#include <algorithm>
#include <iomanip>

#include "../../Projeto IOCP/UTIL/util_time.h"

#include <cmath>
#include <cstdint>

#define CLEAR_10_DAILY_QUEST_TYPEID 0x78800001u		// Quest 10 clear daily quest

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

#define ASSIST_ITEM_TYPEID			0x1BE00016u

#define GRAND_PRIX_TICKET			0x1A000264u
#define LIMIT_GRAND_PRIX_TICKET		50	// Limit de Grand Prix Ticket que o player pode ter, chegou nesse limit não drop mais ele do hole

#define MULLIGAN_ROSE_TYPEID		0x1800000Eu

//#define PREMIUM_TICKET_TYPEID 0x1A100002u

//#define PREMIUM_COMET_TYPEID 0x140000D8u
#define DEFAULT_COMET_TYPEID 0x14000000u

// Cv1 Default Club Set
#define AIR_KNIGHT_SET 0x10000000u

#define CLUB_PATCHER_TYPEID 0x1A00018Fu

#define MILAGE_POINT_TYPEID 0x1A0002A7u
#define TIKI_POINT_TYPEID 0x1A0002A6u

#define SPECIAL_SHUFFLE_COURSE_TICKET_TYPEID	0x1A0000F7u

#define PANG_POUCH_TYPEID	0x1A000010u
#define EXP_POUCH_TYPEID	0x1A00015Du
#define CP_POUCH_TYPEID		0x1A000160u

#define DECREASE_COMBO_VALUE	10		// No JP é 10, no USA era 3

#define MEDIDA_PARA_YARDS 0.3125f		// Usava 0.31251 Medida interna do pangya que no visual é o Yards

// Icon Player Good(angel), Quiter 1 e 2 
constexpr float GOOD_PLAYER_ICON = 3.f;
constexpr float QUITER_ICON_1 = 20.f;
constexpr float QUITER_ICON_2 = 30.f;

// Corta com toma, e corta com safety
const uint32_t active_item_cant_have_2_inveroty[]{ 402653229u, 402653231u, };

constexpr auto TROFEL_GM_EVENT_TYPEID = 0x2D0A3B00u;

constexpr unsigned char cadie_cauldron_Hermes_random_id = 2u;
constexpr unsigned char cadie_cauldron_Jester_random_id = 3u;
constexpr unsigned char cadie_cauldron_Twilight_random_id = 4u;

const uint32_t cadie_cauldron_Hermes_item_typeid[]{ 0x08010032u, 0x0804e058u, 0x0808e025u, 0x080ce041u, 0x0810a030u, 0x0814e05eu, 0x0818a060u, 0x081ce02fu, 0x0820e02fu };
const uint32_t cadie_cauldron_Jester_item_typeid[]{ 0x08000848u, 0x08040863u, 0x0808082bu, 0x080c002cu, 0x08100033u, 0x0814003eu, 0x0818005eu, 0x081c002bu, 0x08200018u, 0x0824000eu, 0x0828001du, 0x08380004u, 0x0830000cu, 0x082c0004u };
const uint32_t cadie_cauldron_Twilight_item_typeid[]{ 0x0801a812u, 0x08050811u, 0x0809081du, 0x080d481cu, 0x0811201bu, 0x08162810u, 0x08196013u, 0x081da817u, 0x0821a80cu };
	
	//---------------------Broadcast Types---------------------//
	enum eBROADCAST_TYPES : unsigned char {
		BT_HIDE_BROADCAST,				// Pangya JP ficava mandado esse tipo com "<BroadCastReservedNoticesIdx>[531, 532],[549,550]</BroadCastReservedNoticesIdx>" dos que vi
		BT_SPINNING_CUBE_RARE,
		BT_SPINNING_CUBE_WIN_PANG_POUCH,
		BT_GOLDEN_TIME_START_OF_DAY = 11,			// Habilitou o Golden Time Event, ou é a primeira do dia programado
		BT_GOLDEN_TIME_START_ROUND,
		BT_GOLDEN_TIME_ROUND_MORE_PEOPLE,			// Tem muita pessoas jogando ou em sala lounge
		BT_GOLDEN_TIME_ROUND_REWARD_PLAYER,
		BT_GOLDEN_TIME_FINISH_ROUND,
		BT_GOLDEN_TIME_FINISH_OF_DAY,				// Finaliza o dia do Golden Time Event e fala a data do próximo programado
		BT_GOLDEN_TIME_FINISH,						// Termina o Evento Golden Time não tem outro evento programado
		BT_GOLDEN_TIME_ROUND_NOT_HAVE_WINNERS,		// Não teve ganhadores no round
		BT_MESSAGE_PLAIN = 20,			// Aqui ele mostra uma message normal, como se fosse o do GM broadcast
		BT_GRAND_ZODIAC_EVENT_START_TIME,
	};
	//--------------------------End----------------------------//

	//------------------ Player Estrutura ---------------------//

	// Player Info
	struct player_info {
		player_info() {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(player_info));
		};
		uint32_t uid;
		BlockFlag block_flag;
		unsigned short level;
		char id[22];
		char nickname[22];
		char pass[40];
	};

	// SyncUpdateDB
	// !@ Teste
#if defined(__linux__)
#pragma pack()
#endif
	struct stSyncUpdateDB {
		enum eSTATE_UPDATE : unsigned char {
			NONE,
			REQUEST_UPDATE,
			UPDATE_CONFIRMED,
			ERROR_UPDATE,
		};
		stSyncUpdateDB() : m_state(eSTATE_UPDATE::NONE) {

#if defined(_WIN32)
			InitializeCriticalSection(&m_cs);
			InitializeConditionVariable(&m_cv);
#elif defined(__linux__)
			INIT_PTHREAD_MUTEXATTR_RECURSIVE;
			INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
			DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

			pthread_cond_init(&m_cv, nullptr);
#endif

		};
		~stSyncUpdateDB() {
			
#if defined(_WIN32)
			// Libera todas threads que estiverem esperando a condition variable
			WakeAllConditionVariable(&m_cv);

			DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_cond_broadcast(&m_cv);

			pthread_mutex_destroy(&m_cs);

			pthread_cond_destroy(&m_cv);
#endif
		};
		void requestUpdateOnDB() {

			uint32_t timeout_count = 3;	/* 30 segundos check, to send new request*/

#if defined(_WIN32)
			DWORD error = 0u;
#elif defined(__linux__)
			int error = 0;
#endif

#if defined(_WIN32)
			EnterCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_lock(&m_cs);
#endif

			// Só esse espera e verifica para atualizar a flag os outros modifica a flag para REQUEST_UPDATE
			if (m_state == eSTATE_UPDATE::REQUEST_UPDATE) {

				while (m_state == eSTATE_UPDATE::REQUEST_UPDATE && timeout_count > 0) {

#if defined(_WIN32)
					if (SleepConditionVariableCS(&m_cv, &m_cs, 10000/*wait 10 second*/) == 0 && (error = GetLastError()) != ERROR_TIMEOUT) {

						LeaveCriticalSection(&m_cs);

						throw exception("[stSyncUpdateDB::requestUpdateOnDB][Error] nao conseguiu pegar o sinal do Condition Variable.",
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 34, error));
					}

					// Decrementa count de timeout
					if (error == ERROR_TIMEOUT)
						--timeout_count;
#elif defined(__linux__)
					timespec wait_time = _milliseconds_to_timespec_clock_realtime(10000/*wait 10 second*/);

					if ((error = pthread_cond_timedwait(&m_cv, &m_cs, &wait_time)) != 0 && error != ETIMEDOUT) {

						pthread_mutex_unlock(&m_cs);

						throw exception("[stSyncUpdateDB::requestUpdateOnDB][Error] nao conseguiu pegar o sinal do Condition Variable.",
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 34, error));
					}

					// Decrementa count de timeout
					if (error == ETIMEDOUT)
						--timeout_count;
#endif
				}

				if (timeout_count == 0)
					_smp::message_pool::getInstance().push(new message("[stSyncUpdateDB::requestUpdateOnDB][WARNING] 30 seconds consumed, change state forced.", CL_FILE_LOG_AND_CONSOLE));

			}

			// Update state to REQUEST_UPDATE
			m_state = eSTATE_UPDATE::REQUEST_UPDATE;

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif

		};
		void confirmUpdadeOnDB() {

#if defined(_WIN32)
			EnterCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_lock(&m_cs);
#endif

			if (m_state != eSTATE_UPDATE::REQUEST_UPDATE) {

#if defined(_WIN32)
				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs);
#endif

				throw exception("[stSyncUpdateDB::confirmUpdateOnDB][Error] m_state is wrong not REQUEST_UPDATE [value="
					+ std::to_string((unsigned short)m_state) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 35, 0));
			}

			// set m_state to UPDATE_CONFIRMED
			m_state = eSTATE_UPDATE::UPDATE_CONFIRMED;

#if defined(_WIN32)
			// Acorda as thread que espera a condition variable
			WakeAllConditionVariable(&m_cv);
#elif defined(__linux__)
			// Acorda as thread que espera a condition variable
			pthread_cond_broadcast(&m_cv);
#endif

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif

		};
		void errorUpdateOnDB() {

#if defined(_WIN32)
			EnterCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_lock(&m_cs);
#endif

			if (m_state != eSTATE_UPDATE::REQUEST_UPDATE) {

#if defined(_WIN32)
				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs);
#endif

				throw exception("[stSyncUpdateDB::errorUpdateOnDB][Error] m_state is wrong not REQUEST_UPDATE [value="
					+ std::to_string((unsigned short)m_state) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 35, 0));
			}

			// set m_state to ERROR_UPDATE
			m_state = eSTATE_UPDATE::ERROR_UPDATE;

#if defined(_WIN32)
			// Acorda as thread que espera a condition variable
			WakeAllConditionVariable(&m_cv);
#elif defined(__linux__)
			// Acorda as thread que espera a condition variable
			pthread_cond_broadcast(&m_cv);
#endif

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif

		};
	private:
		eSTATE_UPDATE m_state;
#if defined(_WIN32)
		CRITICAL_SECTION m_cs;
		CONDITION_VARIABLE m_cv;
#elif defined(__linux__)
		pthread_mutex_t m_cs;
		pthread_cond_t m_cv;
#endif
	};

	// Player Location para atualização do no banco de dados
	struct stPlayerLocationDB : public stSyncUpdateDB {
		stPlayerLocationDB(uint32_t _ul = 0u) : stSyncUpdateDB() {
			
			clear();
		};
		~stPlayerLocationDB() {

			clear();
		};
		void clear() {

			channel = -1;
			lobby = -1;
			room = -1;
			place = 0u;
		};
		char channel;
		char lobby;
		short room;
		unsigned char place;
	};
	// !@ Teste
#if defined(__linux__)
#pragma pack(1)
#endif

	// Log de gastos CP(Cookie Point)
	struct CPLog {
		public:
			enum TYPE : unsigned char {
				BUY_SHOP,
				GIFT_SHOP,
				TICKER,
				CP_POUCH,		// Player ganha CP pela CP(Cookie Point) Pouch
			};

			struct stItem {
				stItem(uint32_t _ul = 0u) {
					clear();
				};
				stItem(uint32_t __typeid, uint32_t _qntd, uint64_t _cp)
					: _typeid(__typeid), qntd(_qntd), price(_cp) {

				};
				void clear() {
					memset(this, 0, sizeof(stItem));
				};
				uint32_t _typeid;
				uint32_t qntd;
				uint64_t price;
			};

		public:
			CPLog(uint32_t _ul = 0u) {
				clear();
			};
			~CPLog() {};

			void clear() {

				m_type = BUY_SHOP;
				m_mail_id = -1;
				m_cookie = 0ull;

				if (!v_item.empty()) {
					v_item.clear();
					v_item.shrink_to_fit();
				}
			};

			TYPE getType() {
				return m_type;
			};

			void setType(TYPE _type) {
				m_type = _type;
			};

			int32_t getMailId() {
				return m_mail_id;
			};

			void setMailId(int32_t _mail_id) {
				m_mail_id = _mail_id;
			};

			uint64_t getCookie() {

				uint64_t total = m_cookie;

				std::for_each(v_item.begin(), v_item.end(), [&](auto& _el) {
					total += _el.price;
				});

				return total;
			};

			void setCookie(uint64_t _cp) {
				m_cookie = _cp;
			};

			uint32_t getItemCount() {
				return (uint32_t)v_item.size();
			};

			std::vector< stItem >& getItens() {
				return v_item;
			};

			void putItem(uint32_t _typeid, uint32_t _qntd, uint64_t _cp) {
				v_item.push_back({ _typeid, _qntd, _cp });
			};

			void putItem(stItem _item) {
				v_item.push_back(_item);
			};

			std::string toString() {

				std::string str = "TYPE=" + std::to_string((unsigned short)m_type)
						+ ", mail_id=" + std::to_string(m_mail_id)
						+ ", cookie=" + std::to_string(getCookie())
						+ ", item(ns) quantidade=" + std::to_string(v_item.size());

				for (auto& el : v_item)
					str = ", {TYPEID=" + std::to_string(el._typeid) + ", QNTD=" + std::to_string(el.qntd) + ", PRICE=" + std::to_string(el.price) + "}";

				return str;
			};

		protected:
			TYPE m_type;
			int32_t m_mail_id;
			uint64_t m_cookie;
			std::vector< stItem > v_item;
	};

	// State Flag Para Member Info
	union uMemberInfoStateFlag {
		void clear() { memset(this, 0, sizeof(uMemberInfoStateFlag)); };
		unsigned char ucByte;
		struct {
			unsigned char channel : 1;			// channel
			unsigned char visible : 1;			// Visible
			unsigned char whisper : 1;			// Whisper
			unsigned char sexo : 1;				// Genero - (ACHO)Já logou mais de uma vez
			unsigned char azinha : 1;			// Azinha, Quit rate menor que 3%
			unsigned char icon_angel : 1;		// Angel Wings
			unsigned char quiter_1 : 1;			// Quit rate maior que 31% e menor que 41%
			unsigned char quiter_2 : 1;			// Quit rate maior que 41%
		}stFlagBit;
	};

	// Player Papel Shop Info
	struct PlayerPapelShopInfo {
		void clear() { memset(this, 0, sizeof(PlayerPapelShopInfo)); };
		short remain_count;
		short current_count;
		short limit_count;
	};

	// Union Capability, que guarda a estrutura de bits do Capability do Player
	union uCapability {
		uCapability(uint32_t _ul = 0u) : ulCapability(_ul) {};
		void clear() { ulCapability = 0u; };
		uint32_t ulCapability;
		struct {
			uint32_t A_I_MODE : 1;					// Inteligência Artificial Modo
			uint32_t : 1;							// Unknwon
			uint32_t game_master : 1;				// GM(Game Master)
			uint32_t gm_edit_site : 1;				// GM(Game Master) que pode mexer na parte adm do site
			uint32_t block_give_item_gm : 1;		// Bloquea o GM de enviar item pelo comando Goldenbell e giveitem
			uint32_t : 2;							// Unknown
			uint32_t gm_normal : 1;				// GM player normal, é para poder voltar o GM novamente, isso é para quando o GM quer ficar normal player para poder jogar novamente
			uint32_t : 2;							// Unknown
			uint32_t mantle : 1;					// Player Autorizado ao entrar no matle(Server escondido para usuários normais), server para testes
			uint32_t : 3;							// Unknown
			uint32_t premium_user : 1;				// Premium User
			uint32_t title_gm : 1, : 0;			// Title de GM, mostra o Nome GM no lugar no Level ou o Title que o player esteja
		}stBit;
	};

	// MemberInfo dados principais do player, tem id, nick, guild, level, exp, e etc)
	struct MemberInfo {
		MemberInfo() {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(MemberInfo));
			oid = -1;
		};
		char id[22];
		char nick_name[22];
		char guild_name[17];
		char guild_mark_img[12];
		unsigned char ucUnknown35[35];		// ainda não sei direito o que tem aqui
		uint32_t ulUnknown;			// ainda não o que é aqui direito
		//uint32_t  capability;
		uCapability capability;
		uint32_t ulUnknown2;			// ainda não o que é aqui direito
		int32_t  oid;
		uint32_t ulUnknown3;			// ainda não o que é aqui direito
		uint64_t ullUnknown;		// ainda não o que é aqui direito
		unsigned guild_uid;
		uint32_t guild_mark_img_no;	// só tem no JP
		uMemberInfoStateFlag state_flag;
		unsigned short flag_login_time;		// 1 é primeira vez que logou, 2 já não é mais a primeira vez que fez login no server
		PlayerPapelShopInfo papel_shop;
		unsigned char ucUnknown16[16];		// ainda não sei direito o que tem aqui
		char id_NT[22];						// ainda não sei direito o que tem aqui
		unsigned char ucUnknown107[106];	// ainda não sei direito o que tem aqui
	};

	// MemberInfoEx extendido tem o uid, limite papel shop e tutorial,
	// so os que nao manda para o pangya no pacote MemberInfo
	struct MemberInfoEx : public MemberInfo {
		MemberInfoEx() : MemberInfo() {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(MemberInfoEx));
			sala_numero = -1;
			oid = -1;
		};
		int32_t uid;
		int guild_point;
		int64_t guild_pang;
		__int16 sala_numero;
		__int8 sexo;
		__int8 level;
		__int8 do_tutorial;
		__int8 event_1;
		__int8 event_2;
		int school;
		int manner_flag;
		SYSTEMTIME papel_shop_last_update;
	};

	// Medal Win
	union uMedalWin {
		void clear() { memset(this, 0, sizeof(uMedalWin)); };
		unsigned char ucMedal;
		struct _stMedal {
			unsigned char lucky : 1;
			unsigned char speediest : 1;
			unsigned char best_drive : 1;
			unsigned char best_chipin : 1;
			unsigned char best_long_puttin : 1;
			unsigned char best_recovery : 1, : 0;
		}stMedal;
	};

	// Medal
	struct stMedal {
		void clear() { memset(this, 0, sizeof(stMedal)); };
		void add(stMedal& _medal) {

			lucky += _medal.lucky;
			fast += _medal.fast;
			best_drive += _medal.best_drive;
			best_chipin += _medal.best_chipin;
			best_puttin += _medal.best_puttin;
			best_recovery += _medal.best_recovery;

		};
		void add(uMedalWin _medal_win) {

			if (_medal_win.stMedal.lucky)
				lucky++;
			else if (_medal_win.stMedal.speediest)
				fast++;
			else if (_medal_win.stMedal.best_drive)
				best_drive++;
			else if (_medal_win.stMedal.best_chipin)
				best_chipin++;
			else if (_medal_win.stMedal.best_long_puttin)
				best_puttin++;
			else if (_medal_win.stMedal.best_recovery)
				best_recovery++;

		};
		uint32_t lucky;
		uint32_t fast;
		uint32_t best_drive;
		uint32_t best_chipin;
		uint32_t best_puttin;
		uint32_t best_recovery;
	};

	// User Info (dados como HIO, ALBA, BUNKER, FAIRWAY e ETC)
	struct UserInfo {
		UserInfo(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(UserInfo));
		};
		void add(UserInfo& _ui) {

			if (_ui.best_drive > best_drive)
				best_drive = _ui.best_drive;

			if (_ui.best_long_putt > best_long_putt)
				best_long_putt = _ui.best_long_putt;

			if (_ui.best_chip_in > best_chip_in)
				best_chip_in = _ui.best_chip_in;

			// Combo e Todal Combos
			if (_ui.combo < 0) {	// Negativo

				// tira só do combo, não de todos os combos que foram feitos
				if (combo <= DECREASE_COMBO_VALUE)
					combo = 0;
				else
					combo += _ui.combo;

			}else {					// Positivo

				combo += _ui.combo;

				// Só soma o all combo se combo > que all_combo
				if (combo > all_combo)
					all_combo += _ui.combo;
			}

			// Event Angel ativado, quitado < 0
			if (_ui.quitado < 0) {

				// Se for 0 não subtrai
				if ((quitado + _ui.quitado) <= 0)
					quitado = 0;
				else
					quitado += _ui.quitado;

			}else // Normal soma o quit do player se ele quitou
				quitado += _ui.quitado;

			// Skin (Pang Battle)
			if ((skin_all_in_count + _ui.skin_all_in_count) >= 5) {

				skin_all_in_count = 0l;
				skin_pang += 1000; // dá 1000 pangs por que ele jogou 5 Pang Battle
			
			}else
				skin_all_in_count += _ui.skin_all_in_count;

			tacada += _ui.tacada;
			putt += _ui.putt;
			tempo += _ui.tempo;
			tempo_tacada += _ui.tempo_tacada;
			acerto_pangya += _ui.acerto_pangya;
			timeout += _ui.timeout;
			ob += _ui.ob;
			total_distancia += _ui.total_distancia;
			hole += _ui.hole;
			hole_in += (_ui.hole - _ui.hole_in);
			hio += _ui.hio;
			bunker += _ui.bunker;
			fairway += _ui.fairway;
			albatross += _ui.albatross;
			putt_in += _ui.putt_in;
			media_score += _ui.media_score;
			best_score[0] += _ui.best_score[0];
			best_score[1] += _ui.best_score[1];
			best_score[2] += _ui.best_score[2];
			best_score[3] += _ui.best_score[3];
			best_score[4] += _ui.best_score[4];
			best_pang[0] += _ui.best_pang[0];
			best_pang[1] += _ui.best_pang[1];
			best_pang[2] += _ui.best_pang[2];
			best_pang[3] += _ui.best_pang[3];
			best_pang[4] += _ui.best_pang[4];
			sum_pang += _ui.sum_pang;
			event_flag += _ui.event_flag;
			jogado += _ui.jogado;
			team_game += _ui.team_game;
			team_win += _ui.team_win;
			team_hole += _ui.team_hole;
			ladder_point += _ui.ladder_point;
			ladder_hole += _ui.ladder_hole;
			ladder_win += _ui.ladder_win;
			ladder_lose += _ui.ladder_lose;
			ladder_draw += _ui.ladder_draw;
			skin_pang += _ui.skin_pang;
			skin_win += _ui.skin_win;
			skin_lose += _ui.skin_lose;
			skin_run_hole += _ui.skin_run_hole;
			//skin_all_in_count += _ui.skin_all_in_count; // aqui adiciona lá em cima, por que ele reseta em 5
			skin_strike_point += _ui.skin_strike_point;
			disconnect += _ui.disconnect;
			jogados_disconnect += _ui.jogados_disconnect;
			event_value += _ui.event_value;
			sys_school_serie += _ui.sys_school_serie;
			game_count_season += _ui.game_count_season;

			// Medal
			medal.add(_ui.medal);

		};
		int32_t tacada;
		int32_t putt;
		int32_t tempo;
		int32_t tempo_tacada;
		float best_drive;			// Max Distancia
		int32_t acerto_pangya;
		int32_t timeout;
		int32_t ob;
		int32_t total_distancia;
		int32_t hole;
		int32_t hole_in;		// Aqui é os holes que não foram concluídos Ex: Give up, ou no Match o outro player ganho sem precisar do player terminar o hole
		int32_t hio;
		short bunker;
		int32_t fairway;
		int32_t albatross;
		int32_t mad_conduta;	// Aqui é hole in, mas no info não tras ele por que ele já foi salvo no hole alí em cima
		int32_t putt_in;
		float best_long_putt;
		float best_chip_in;
		uint32_t exp;
		unsigned char level;
		uint64_t pang;
		int32_t media_score;
		char best_score[5];				// Best Score Por Estrela, mas acho que o pangya nao usa mais isso
		unsigned char event_flag;
		int64_t best_pang[5];			// Best Pang por Estrela, mas acho que o pangya nao usa mais isso
		int64_t sum_pang;				// A soma do pangs das 5 estrela acho
		int32_t jogado;
		int32_t team_hole;
		int32_t team_win;
		int32_t team_game;
		int32_t ladder_point;				// Ladder é o Match acho, de tourneio não sei direito
		int32_t ladder_hole;
		int32_t ladder_win;
		int32_t ladder_lose;
		int32_t ladder_draw;
		int32_t combo;
		int32_t all_combo;
		int32_t quitado;
		int64_t skin_pang;			// Skin é o Pang Battle tem valor negativo ele """##### Ajeitei agora(ACHO)
		int32_t skin_win;
		int32_t skin_lose;
		int32_t skin_all_in_count;
		int32_t skin_run_hole;				// Correu desistiu (ACHO)
		int32_t skin_strike_point;			// Antes era o nao_sei
		int32_t jogados_disconnect;		// Antes era o jogos_nao_sei
		short event_value;
		int32_t disconnect;				// Vou deixar aqui o disconect count (antes era skin_strike_point)
		stMedal medal;
		int32_t sys_school_serie;			// Sistema antigo do pangya JP que era de Serie de escola, respondia as perguntas se passasse ia pra outra serie é da 1° a 5°
		int32_t game_count_season;
		short _16bit_nao_sei;
		float getMediaScore() {	// AVG SCORE
			
			// Verifica se é 0, por que não pode dividir 18 por 0 que dá excessão, 
			// por que não pode dividir nenhum número por 0
			if ((hole - hole_in) == 0)
				return 0.f;

			return (18.f / (hole - hole_in)) * media_score + 72.f;
		};
		float getPangyaShotRate() {

			// Previne divisão por 0
			if (tacada == 0)
				return 0.f;

			return ((float)acerto_pangya / tacada) * 100.f;
		};
		float getFairwayRate() {

			// Previne divisão por 0
			if ((hole - hole_in) == 0)
				return 0.f;

			return ((float)fairway / (hole - hole_in)) * 100.f;
		};
		float getPuttRate() {

			// Previne divisão por 0
			if (putt == 0)
				return 0.f;

			return ((float)putt_in / putt) * 100.f;
		};
		float getOBRate() {

			// Previne divisão por 0
			if ((tacada + putt) == 0)
				return 0.f;

			return ((float)ob / (tacada + putt)) * 100.f;
		};
		float getMatchWinRate() {

			// Previne divisão por 0
			if (team_game == 0)
				return 0.f;

			return ((float)team_win / team_game) * 100.f;
		};
		float getShotTimeRate() {

			// Previne divisão por 0
			if ((tacada + putt) == 0)
				return 0.f;

			return ((float)tempo_tacada / (tacada + putt)) * 100.f;
		};
		float getQuitRate() {

			// Previne divisão por 0
			if (jogado == 0)
				return 0.f;

			return quitado * 100.f / jogado;
		};
		std::string toString() {
			return "Tacada: " + std::to_string(tacada) + "  Putt: " + std::to_string(putt) + "  Tempo: " + std::to_string(tempo) + "  Tempo Tacada: " + std::to_string(tempo_tacada)
				+ "  Best drive: " + std::to_string(best_drive) + "  Acerto pangya: " + std::to_string(acerto_pangya) + "  timeout: " + std::to_string(timeout)
				+ "  OB: " + std::to_string(ob) + "  Total distancia: " + std::to_string(total_distancia) + "  hole: " + std::to_string(hole)
				+ "  Hole in: " + std::to_string(hole_in) + "  HIO: " + std::to_string(hio) + "  Bunker: " + std::to_string(bunker) + "  Fairway: " + std::to_string(fairway)
				+ "  Albratross: " + std::to_string(albatross) + "  Mad conduta: " + std::to_string(mad_conduta) + "  Putt in: " + std::to_string(putt_in)
				+ "  Best long puttin: " + std::to_string(best_long_putt) + "  Best chipin: " + std::to_string(best_chip_in) + "  Exp: " + std::to_string(exp)
				+ "  Level: " + std::to_string((unsigned short)level) + "  Pang: " + std::to_string(pang) + "  Media score: " + std::to_string(media_score)
				+ "  Best score[" + std::to_string((unsigned short)best_score[0]) + ", " + std::to_string((unsigned short)best_score[1]) + ", " + std::to_string((unsigned short)best_score[2])
				+ ", " + std::to_string((unsigned short)best_score[3]) + ", " + std::to_string((unsigned short)best_score[4]) + "]  Event flag: " + std::to_string((unsigned short)event_flag)
				+ "  Best pang[" + std::to_string(best_pang[0]) + ", " + std::to_string(best_pang[1]) + ", " + std::to_string(best_pang[2]) + ", " + std::to_string(best_pang[3])
				+ ", " + std::to_string(best_pang[4]) + "]  Soma pang: " + std::to_string(sum_pang) + "  Jogado: " + std::to_string(jogado) + "  Team Hole: " + std::to_string(team_hole)
				+ "  Team win: " + std::to_string(team_win) + "  Team game: " + std::to_string(team_game) + "  Ladder point: " + std::to_string(ladder_point)
				+ "  Ladder hole: " + std::to_string(ladder_hole) + "  Ladder win: " + std::to_string(ladder_win) + "  Ladder lose: " + std::to_string(ladder_lose)
				+ "  Ladder draw: " + std::to_string(ladder_draw) + "  Combo: " + std::to_string(combo) + "  All combo: " + std::to_string(all_combo)
				+ "  Quitado: " + std::to_string(quitado) + "  Skin pang: " + std::to_string(skin_pang) + "  Skin win: " + std::to_string(skin_win)
				+ "  Skin lose: " + std::to_string(skin_lose) + "  Skin all in count: " + std::to_string(skin_all_in_count) + "  Skin run hole: " + std::to_string(skin_run_hole)
				+ "  Disconnect(MY): " + std::to_string(disconnect) + "  Jogados Disconnect(MY): " + std::to_string(jogados_disconnect) + "  Event value: " + std::to_string(event_value)
				+ "  Skin Strike Point: " + std::to_string(skin_strike_point) + "  Sistema School Serie: " + std::to_string(sys_school_serie)
				+ "  Game count season: " + std::to_string(game_count_season) + "  _16bit nao sei: " + std::to_string(_16bit_nao_sei);
		};
	};

	// User Info Ex
	struct UserInfoEx : public UserInfo {
		UserInfoEx(uint32_t _ul = 0u) : UserInfo() {
			clear();
		};
		void clear() { memset(this, 0, sizeof(UserInfoEx)); };
		void add(UserInfoEx& _ui, uint64_t _total_pang_win_game = 0ull) {
			
			UserInfo::add(_ui);

			if (_total_pang_win_game > 0)
				total_pang_win_game += _total_pang_win_game;

		};
		uint64_t total_pang_win_game;
	};

	// Trofel Info
	struct TrofelInfo {
		void clear() {
			memset(this, 0, sizeof(TrofelInfo));
		};
		void update(uint32_t _type, unsigned char _rank) {

			// Maior que Pro 7
			if (_type > 12)
				throw exception("[TrofelInfo::update][Error] _type[VALUE=" + std::to_string(_type) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 200, 0));

			if (_rank == 0u || _rank > 3)
				throw exception("[TrofelInfo::update][Error] _rank[VALUE=" + std::to_string((unsigned short)_rank) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 201, 0));

			if (_type < 6) { // AMA

				ama_6_a_1[_type][_rank - 1]++;

			}else { // >= 6 PRO

				pro_1_a_7[_type - 6][_rank - 1]++;
			}

			/*switch (_type) {
			case 0:		// AMA 6
				ama_6[_rank - 1]++;
				break;
			case 1:		// AMA 5
				ama_5[_rank - 1]++;
				break;
			case 2:		// AMA 4
				ama_4[_rank - 1]++;
				break;
			case 3:		// AMA 3
				ama_3[_rank - 1]++;
				break;
			case 4:		// AMA 2
				ama_2[_rank - 1]++;
				break;
			case 5:		// AMA 1
				ama_1[_rank - 1]++;
				break;
			case 6:		// PRO 1
				pro_1[_rank - 1]++;
				break;
			case 7:		// PRO 2
				pro_2[_rank - 1]++;
				break;
			case 8:		// PRO 3
				pro_3[_rank - 1]++;
				break;
			case 9:		// PRO 4
				pro_4[_rank - 1]++;
				break;
			case 10:	// PRO 5
				pro_5[_rank - 1]++;
				break;
			case 11:	// PRO 6
				pro_6[_rank - 1]++;
				break;
			case 12:	// PRO 7
				pro_7[_rank - 1]++;
				break;
			}*/
		};
		uint32_t getSumGold() {

			uint32_t gold_sum = 0u;

			for (auto& el : ama_6_a_1)
				gold_sum += el[0];

			for (auto& el : pro_1_a_7)
				gold_sum += el[0];

			return gold_sum;
		};
		uint32_t getSumSilver() {

			uint32_t silver_sum = 0u;

			for (auto& el : ama_6_a_1)
				silver_sum += el[1];

			for (auto& el : pro_1_a_7)
				silver_sum += el[1];

			return silver_sum;
		};
		uint32_t getSumBronze() {

			uint32_t bronze_sum = 0u;

			for (auto& el : ama_6_a_1)
				bronze_sum += el[2];

			for (auto& el : pro_1_a_7)
				bronze_sum += el[2];

			return bronze_sum;
		};
		short ama_6_a_1[6][3];	// Ama 6~1, Ouro, Prata e Bronze
		short pro_1_a_7[7][3];	// Pro 1~7, Ouro, Prate e Bronze
		/*short ama_6[3];		// Ouro, Prata e Bronze
		short ama_5[3];		// Ouro, Prata e Bronze
		short ama_4[3];		// Ouro, Prata e Bronze
		short ama_3[3];		// Ouro, Prata e Bronze
		short ama_2[3];		// Ouro, Prata e Bronze
		short ama_1[3];		// Ouro, Prata e Bronze
		short pro_1[3];		// Ouro, Prata e Bronze
		short pro_2[3];		// Ouro, Prata e Bronze
		short pro_3[3];		// Ouro, Prata e Bronze
		short pro_4[3];		// Ouro, Prata e Bronze
		short pro_5[3];		// Ouro, Prata e Bronze
		short pro_6[3];		// Ouro, Prata e Bronze
		short pro_7[3];		// Ouro, Prata e Bronze*/
	};

	// Trofel Especial Info
	struct TrofelEspecialInfo {
		void clear() {
			memset(this, 0, sizeof(TrofelEspecialInfo));
		};
		int32_t id;
		int32_t _typeid;
		int32_t qntd;
	};

	// Item Equipados
	struct UserEquip {
		void clear() {
			memset(this, 0, sizeof(UserEquip));
		};
		int32_t caddie_id;
		int32_t character_id;
		int32_t clubset_id;
		int32_t ball_typeid;
		int32_t item_slot[10];		// 10 Item slot
		int32_t skin_id[6];		// 6 skin id, tem o title, frame, stick e etc
		int32_t skin_typeid[6];	// 6 skin typeid, tem o title, frame, stick e etc
		int32_t mascot_id;
		int32_t poster[2];			// Poster, tem 2 o poster A e poster B
#define m_title skin_typeid[5]	// Titulo Typeid
	};

	// MapStatistics
	struct MapStatistics {
		MapStatistics(uint32_t _ul = 0u) {
			clear();
		};
		void clear(unsigned char _course = 0) {
			memset(this, 0, sizeof(MapStatistics));
			best_score = 127;
			course = _course;
		};
		unsigned char isRecorded() {

			// Player fez record nesse Course
			return (best_score != 127 ? 1 : 0);
		};
		unsigned char course;
		int32_t tacada;
		int32_t putt;
		int32_t hole;
		int32_t fairway;
		int32_t hole_in;
		int32_t putt_in;
		int32_t total_score;
		char best_score;
		int64_t best_pang;
		int32_t character_typeid;
		unsigned char event_score;
	};

	// MapStatisticsEx esse tem o tipo que não vai no pacote que passa pro cliente
	struct MapStatisticsEx : public MapStatistics {
		MapStatisticsEx(uint32_t _ul = 0u) : MapStatistics() {
			clear();
		};
		MapStatisticsEx(MapStatisticsEx& _cpy) {
			*this = _cpy;
		};
		MapStatisticsEx(MapStatistics& _cpy) : MapStatistics(_cpy) {
			tipo = 0u;
		};
		void clear(unsigned char _course = 0) {
			memset(this, 0, sizeof(MapStatisticsEx));
			best_score = 127;
			course = _course;
		};
		unsigned char tipo;				// Tipo, 0 Normal, 0x32 Natural, 0x33 Grand Prix
	};

	// Caddie Info
	struct CaddieInfo {
		CaddieInfo(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(CaddieInfo));
		};
		int32_t id;
		uint32_t _typeid;
		uint32_t parts_typeid;
		unsigned char level;
		uint32_t exp;
		unsigned char rent_flag;
		unsigned short end_date_unix;
		unsigned short parts_end_date_unix;
		unsigned char purchase;
		short check_end;
	};

	// Caddie Info Ex
	struct CaddieInfoEx : public CaddieInfo {
		CaddieInfoEx(uint32_t _ul = 0u) : CaddieInfo(_ul) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(CaddieInfoEx));
		};
		inline void updatePartsEndDate() {

			int64_t diff_end_parts_date = isEmpty(end_parts_date) ? 0ll : getLocalTimeDiffDESC(end_parts_date);

			// Não tem mais o parts _typeid acabou o tempo dela
			if (diff_end_parts_date <= 0) {
				
				parts_end_date_unix = 0u;

				// Zera Parts_Typeid
				if (parts_typeid > 0) {

					parts_typeid = 0u;

					need_update = 1u;	// Precisa Atulizar para o cliente
				}

			}else
				parts_end_date_unix = ((diff_end_parts_date /= STDA_10_MICRO_PER_HOUR) == 0 ? 1/*Less 1 Hour, but not left time*/ : (unsigned short)diff_end_parts_date);

		};
		inline void updateEndDate() {

			int64_t diff_end_date = isEmpty(end_date) ? 0ll : getLocalTimeDiffDESC(end_date);;

			if (diff_end_date <= 0)
				end_date_unix = 0u;
			else
				end_date_unix = ((diff_end_date /= STDA_10_MICRO_PER_DAY) == 0 ? 1/*Less 1 Day, bot not left time*/ : (unsigned short)diff_end_date);

		};
		CaddieInfo* getInfo() {
			
			// Update Timestamp Unix of caddie and caddie Parts

			// End Date Unix Update 
			updateEndDate();

			// Parts End Date Unix Update
			updatePartsEndDate();

			return this;
		};
		SYSTEMTIME end_date;
		SYSTEMTIME end_parts_date;
		unsigned char need_update : 1;			// Precisa Atulizar para o cliente
	};

	// Club Set Info
	struct ClubSetInfo {
		ClubSetInfo() {
			clear();
		};
		ClubSetInfo(int32_t _id, int32_t __typeid, short *_slot_c) {
			clear();

			id = _id;
			_typeid = __typeid;

			if (_slot_c != nullptr)
#if defined(_WIN32)
				memcpy_s(slot_c, sizeof(slot_c), _slot_c, sizeof(slot_c));
#elif defined(__linux__)
				memcpy(slot_c, _slot_c, sizeof(slot_c));
#endif
		};
		void clear() {
			memset(this, 0, sizeof(ClubSetInfo));
		};
		int32_t id;
		int32_t _typeid;
		short slot_c[5];		// Total de slot para upa do stats, força, controle, spin e etc
		short enchant_c[5];		// Enchant Club, Força, controle, spin e etc
	};

	// Mascot Info
	struct MascotInfo {
		MascotInfo(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(MascotInfo));
		};
		int32_t id;
		uint32_t _typeid;
		unsigned char level;
		uint32_t exp;
		char message[30];
		short tipo;
		SYSTEMTIME data;
		unsigned char flag;
	};

	// Mascot Info Ex, tem o IsCash flag nele
	struct MascotInfoEx : public MascotInfo {
		MascotInfoEx(uint32_t _ul = 0u) : MascotInfo(_ul) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(MascotInfoEx));
		};
		bool checkUpdate() {

			if (getLocalTimeDiffDESC(data) <= 0)
				need_update = 1;

			return (need_update == 1);
		}
		unsigned char is_cash;
		uint32_t price;
		unsigned char need_update : 1;
	};

	// Item Warehouse
	struct WarehouseItem {
		WarehouseItem(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(WarehouseItem));
		};
		int32_t id;
		int32_t _typeid;
		int32_t ano;			// acho que seja só tempo que o item ainda tem
		short c[5];			// Stats do item ctrl, força etc, se não usa isso o [0] é a quantidade
		unsigned char purchase;
		unsigned char flag;
		int64_t apply_date;
		int64_t end_date;
		unsigned char type;
		struct UCC {
			void clear() { memset(this, 0, sizeof(UCC)); };
			char name[40];
			unsigned char trade;		// Aqui pode(acho) ser qntd de sd que foi vendida
			char idx[9];				// 8 
			//unsigned char flag;		// acho que aqui seja o ultimo byte 0 pra dizer que é uma string isso
			unsigned char status;
			unsigned short seq;			// aqui é a seq de sd que vendeu
			char copier_nick[22];
			int32_t copier;				// uid de quem fez a sd
		};
		UCC ucc;
		struct Card {
			void clear() { memset(this, 0, sizeof(Card)); };
			int32_t character[4];
			int32_t caddie[4];
			int32_t NPC[4];
		};
		Card card;
		struct ClubsetWorkshop {
			void clear() { memset(this, 0, sizeof(ClubsetWorkshop)); };
			short flag;
			short c[5];
			int32_t mastery;
			int32_t recovery_pts;
			int32_t level;
			int32_t rank;			// UP eu chamo esse
			int32_t calcRank(short* _c) {
				int32_t total = c[0] + _c[0] + c[1] + _c[1] + c[2] + _c[2] + c[3] + _c[3] + c[4] + _c[4];

				if (total >= 30 && total < 60)
					return (total - 30) / 5;
				
				return -1;
			};
			int32_t calcLevel(short* _c) {
				int32_t total = c[0] + _c[0] + c[1] + _c[1] + c[2] + _c[2] + c[3] + _c[3] + c[4] + _c[4];

				if (total >= 30 && total < 60)
					return (total - 30) % 5;

				return -1;
			};
			static int32_t s_calcRank(short* _c) {
				int32_t total = _c[0] + _c[1] + _c[2] + _c[3] + _c[4];

				if (total >= 30 && total < 60)
					return (total - 30) / 5;

				return -1;
			};
			static int32_t s_calcLevel(short* _c) {
				int32_t total = _c[0] + _c[1] + _c[2] + _c[3] + _c[4];

				if (total >= 30 && total < 60)
					return (total - 30) % 5;

				return -1;
			};
		};
		ClubsetWorkshop clubset_workshop;
	};

	struct WarehouseItemEx: public WarehouseItem {
		WarehouseItemEx(uint32_t _ul = 0u) : WarehouseItem() {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(WarehouseItemEx));
		};
		// Date to Calcule dates
		uint32_t apply_date_unix_local;
		uint32_t end_date_unix_local;
	};

	// ClubSet Workshop Last Up Level
	struct ClubSetWorkshopLasUpLevel {
		void clear() { memset(this, 0, sizeof(ClubSetWorkshopLasUpLevel)); };
		int32_t clubset_id;
		uint32_t stat;
	};

	// ClubSet WorkShop Transform ClubSet In Special ClubSet
	struct ClubSetWorkshopTransformClubSet {
		void clear() { memset(this, 0, sizeof(ClubSetWorkshopTransformClubSet)); };
		int32_t clubset_id;
		uint32_t stat;
		uint32_t transform_typeid;
	};

	// TradeItem
	struct TradeItem {
		void clear() { memset(this, 0, sizeof(TradeItem)); };
		uint32_t _typeid;
		int32_t id;
		uint32_t qntd;
		unsigned char ucUnknown3[3];
		uint64_t pang;
		uint32_t upgrade_custo;
		unsigned short c[5];
		unsigned short usUnknown;
		char sd_idx[9];
		unsigned short sd_seq;
		unsigned char sd_status;
		struct Card {
			void clear() { memset(this, 0, sizeof(Card)); };
			uint32_t character[4];
			uint32_t caddie[4];
			uint32_t NPC[4];
			unsigned short character_slot_count;
			unsigned short caddie_slot_count;
			unsigned short NPC_slot_count;
		};
		Card card;
		char sd_name[41];
		char sd_copier_nick[22];
	};

	// Dolfini Locker Item
	struct DolfiniLockerItem {
		void clear() { memset(this, 0, sizeof(DolfiniLockerItem)); };
		uint64_t index;	// ID do item no dolfini Locker
		TradeItem item;
	};

	// Personal Shop Item
	struct PersonalShopItem {
		void clear() { memset(this, 0, sizeof(PersonalShopItem)); };
		uint32_t index;		// Index Sequência do item no shop
		TradeItem item;
	};

	// Tutorial Info
	struct TutorialInfo {
		void clear() {
			memset(this, 0, sizeof(TutorialInfo));
		};
		uint32_t getTutoAll() {
			return rookie | beginner | advancer;
		};
		uint32_t rookie;
		uint32_t beginner;
		uint32_t advancer;
	};

	// Card Info
	struct CardInfo {
		void clear() {
			memset(this, 0, sizeof(CardInfo));
		};
		int32_t id;
		int32_t _typeid;
		int32_t slot;
		int32_t efeito;
		int32_t efeito_qntd;
		int32_t qntd;
		SYSTEMTIME use_date;
		SYSTEMTIME end_date;
		unsigned char type;
		unsigned char use_yn;
	};

	// Card Equip Info
	struct CardEquipInfo {
		void clear() {
			memset(this, 0, sizeof(CardEquipInfo));
		};
		int32_t id;
		int32_t _typeid;
		int32_t parts_typeid;
		int32_t parts_id;
		int32_t efeito;
		int32_t efeito_qntd;
		int32_t slot;
		SYSTEMTIME use_date;
		SYSTEMTIME end_date;
		int32_t tipo;
		unsigned char use_yn;
	};

	// Card Equip Info Ex
	struct CardEquipInfoEx : public CardEquipInfo {
		CardEquipInfoEx(uint32_t _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(CardEquipInfoEx)); };
		int64_t index;
	};

	// Time 32, HighTime, LowTime
	struct time32 {
		void clear() {
			memset(this, 0, sizeof(time32));
		};
		void setTime(uint32_t time) {
			high_time = (unsigned short)(time / 0xFFFF);
			low_time = time % 0xFFFF;
		};
		uint32_t getTime() {
			return (uint32_t)((high_time * 0xFFFF) | low_time);
		};
	private:
		unsigned short high_time;
		unsigned short low_time;
	};

	// Item Buff (Exemple: Yam, Bola Arco-iris)
	struct ItemBuff {
		enum eTYPE : uint32_t {
			NONE,
			YAM_AND_GOLD,
			RAINBOW,
			RED,
			GREEN,
			YELLOW,
		};
		void clear() {
			memset(this, 0, sizeof(ItemBuff));
		};
		int32_t id;
		int32_t _typeid;
		int32_t parts_typeid;
		int32_t parts_id;
		int32_t efeito;
		int32_t efeito_qntd;
		int32_t slot;
		SYSTEMTIME use_date;
		unsigned char ucUnknown12[12];
		time32 tempo;
		int32_t tipo;
		unsigned char use_yn;
	};

	// Item Buff Ex
	struct ItemBuffEx : public ItemBuff {
		ItemBuffEx(uint32_t _ul = 0u) { clear(); };
		void clear() { memset(this, 0, sizeof(ItemBuffEx)); };
		int64_t index;
		SYSTEMTIME end_date;
		uint32_t percent;		// Rate, tipo 2 é 0 por que é 100
	};

	// Guild Info
	struct GuildInfo {
		GuildInfo(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(GuildInfo));
			uid = 0;
		};
		uint32_t uid;
		unsigned char leadder;	// Acho que seja o lider da guild (ACHO) ou não que no meu fica 0
		char name[32];
		uint32_t index_mark_emblem;
		uint64_t ull_unknown;
		uint64_t pang;
		char _16unknown[16];
		uint32_t point;
	};

	// GuildInfoEx
	struct GuildInfoEx : public GuildInfo {
		GuildInfoEx(uint32_t _ul = 0u) : GuildInfo(_ul) {
			clear();
		};
		void clear() {

			GuildInfo::clear();

			memset(mark_emblem, sizeof(mark_emblem), 0);
		};
		char mark_emblem[12];
	};

	// MapStatistics = MS
#define MS_NUM_MAPS 22

	// Treasure Hunter Info
	struct TreasureHunterInfo {
		void clear() {
			memset(this, 0, sizeof(TreasureHunterInfo));
		};
		unsigned char course;
		uint32_t point;
	};

	struct TreasureHunterItem {
		void clear() { memset(this, 0, sizeof(TreasureHunterItem)); };
		uint32_t _typeid;
		uint32_t qntd;
		uint32_t probabilidade;
		unsigned char flag;
		unsigned char active : 1;
	};

	// Counter Item Info
	struct CounterItemInfo {
		CounterItemInfo(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(CounterItemInfo));
		};
		bool isValid() { return (id > 0 && _typeid != 0); };
		unsigned char active;
		uint32_t _typeid;
		int32_t id;
		int32_t value;
	};

	// Quest Stuff Info
	struct QuestStuffInfo {
		void clear() {
			memset(this, 0, sizeof(QuestStuffInfo));
		};
		bool isValid() { return (id > 0 && _typeid != 0); };
		int32_t id;
		uint32_t _typeid;
		int32_t counter_item_id;
		uint32_t clear_date_unix;
	};

	// Quest Stuff Info Extended
	//struct QuestStuffInfoEx : public QuestStuffInfo {
	//	void clear() {
	//		memset(this, 0, sizeof(QuestStuffInfoEx));
	//	};
	//	bool isValid() { return (id > 0 && _typeid != 0); };
	//	int32_t id;			// ID do quest stuff info
	//};

	// Achievement Info
	struct AchievementInfo {
		enum ACHIEVEMENT_STATUS : unsigned char {
			PENDENTING = 1,
			EXCLUEDED,
			ACTIVED,
			CONCLUEDED,
		};

		~AchievementInfo() {};
		void clear() {
			active = 0;
			_typeid = 0;
			id = 0;
			status = 0;

			if (!v_qsi.empty()) {
				v_qsi.clear();
				v_qsi.shrink_to_fit();
			}

			if (!map_counter_item.empty())
				map_counter_item.clear();
		};
		CounterItemInfo* findCounterItemById(int32_t _id) {
			if (_id < 0)
				throw exception("[AchievementInfo::findCounterItemById][Error] _id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 50, 0));

			auto it = map_counter_item.end();

			if ((it = map_counter_item.find(_id)) != map_counter_item.end())
				return &it->second;

			return nullptr;
		};
		CounterItemInfo* findCounterItemByTypeId(uint32_t _typeid) {
			if (_typeid == 0)
				throw exception("[AchievementInfo::findCounterItemByTypeid][Error] _typeid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 50, 0));

			auto it = map_counter_item.end();

			if ((it = VECTOR_FIND_ITEM(map_counter_item, second._typeid, == , _typeid)) != map_counter_item.end())
				return &it->second;

			return nullptr;
		};
		QuestStuffInfo* findQuestStuffById(int32_t _id) {
			if (_id < 0)
				throw exception("[AchievementInfo::findQuestStuffById][Error] _id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 50, 0));

			auto it = v_qsi.end();

			if ((it = VECTOR_FIND_ITEM(v_qsi, id, == , _id)) != v_qsi.end())
#if defined(_WIN32)
				return it._Ptr;
#elif defined(__linux__)
				return &(*it);
#endif

			return nullptr;
		};
		QuestStuffInfo* findQuestStuffByTypeId(uint32_t _typeid) {
			if (_typeid == 0)
				throw exception("[AchievementInfo::findQuestStuffByTypeId][Error] _typeid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 50, 0));

			auto it = v_qsi.end();

			if ((it = VECTOR_FIND_ITEM(v_qsi, _typeid, == , _typeid)) != v_qsi.end())
#if defined(_WIN32)
				return it._Ptr;
#elif defined(__linux__)
				return &(*it);
#endif

			return nullptr;
		};
		uint32_t addCounterByTypeId(uint32_t _typeid, int32_t _value) {
			if (_typeid == 0)
				throw exception("[AchievementInfo::addCounterByTypeId][Error] _typeid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_GAME_ST, 50, 0));

			uint32_t count = 0u;
			std::map< int32_t, CounterItemInfo* > map_cii;

			std::for_each(v_qsi.begin(), v_qsi.end(), [&](auto& el) {
				
				// Só Quest que ainda não foi concluída
				if (el.clear_date_unix == 0) {
					CounterItemInfo *cii = nullptr;

					if ((cii = findCounterItemById(el.counter_item_id)) != nullptr && cii->_typeid == _typeid)
						map_cii[cii->id] = cii;
				}
			});

			for (auto& it : map_cii) {
				it.second->value += _value;
				++count;
			}

			return count;
		};
		bool checkAllQuestClear() {

			auto size = v_qsi.size(), count = (size_t)0;

			for (auto& el : v_qsi)
				if (el.clear_date_unix != 0)
					++count;

			return count == size;
		};
		unsigned char active;
		uint32_t _typeid;
		int32_t id;
		int32_t status;		// 1 pedente, 2 excluído, 3 ativo, 4 concluído
		std::map< int32_t, CounterItemInfo > map_counter_item;
		std::vector< QuestStuffInfo > v_qsi;
	};

	// Achievement Info Ex
	struct AchievementInfoEx : AchievementInfo {
		AchievementInfoEx() : AchievementInfo() {
			clear();
		};
		void clear() {
			AchievementInfo::clear();
			quest_base_typeid = 0;
		};
		// A ultima quest do Achievement que tem o counter item adicionado no db e depois replica para os outro que nao foi concluído
		// Se não tiver cria um counter item para todas as quest
		uint32_t quest_base_typeid;
		std::vector< QuestStuffInfo >::iterator getQuestBase() {
			
			if (quest_base_typeid == 0)
				return v_qsi.end();

			return VECTOR_FIND_ITEM(v_qsi, _typeid, == , quest_base_typeid);
		};
	};

	// Coupons Gacha
	struct CouponGacha {
		void clear() {
			memset(this, 0, sizeof(CouponGacha));
		};
		int32_t partial_ticket;
		int32_t normal_ticket;
	};

	// Premium Ticket User
	struct PremiumTicket {
		void clear() {
			memset(this, 0, sizeof(PremiumTicket));
		};
		int32_t id;
		int32_t _typeid;
		int32_t unix_sec_date;
		int32_t unix_end_date;
	};

	// Request Info
	struct RequestInfo {
		void clear() {
			memset(this, 0, sizeof(RequestInfo));
		};
		uint32_t uid;
		unsigned char season;
		unsigned char show;		// 12 pacotes enviados pode enviar o pacote089
	};

	// Itens Equipado do Player
	struct EquipedItem {
		void clear() { memset(this, 0, sizeof(EquipedItem)); };
		CharacterInfo *char_info;
		CaddieInfoEx *cad_info;
		MascotInfoEx *mascot_info;
		ClubSetInfo csi;
		WarehouseItem *comet;
		WarehouseItem *clubset;
	};

	// Estado do Character no Lounge
	struct StateCharacterLounge {
		StateCharacterLounge() { clear(); };
		void clear() {
			camera_zoom = 1.f;
			scale_head = 1.f;
			walk_speed = 1.f;
			fUnknown = 1.f;
		};
		float camera_zoom;	// Zoom da câmera
		float scale_head;	// Tamanho da cabeça do character
		float walk_speed;	// Velocidade que o player anda no lounge
		float fUnknown;
	};

	// MyRoom Config
	struct MyRoomConfig {
		void clear() { memset(this, 0, sizeof(MyRoomConfig)); };
		unsigned short allow_enter;		// Se pode ou não entrar no My Room
		unsigned char public_lock;		// Se tem senha ou não
		char pass[15];					// Senha
		unsigned char ucUnknown90[90];	// Não o que é ainda
	};

	// MyRoom Item
	struct MyRoomItem {
		void clear() { memset(this, 0, sizeof(MyRoomItem)); };
		int32_t id;
		uint32_t _typeid;
		unsigned short number;
		struct Location {
			void clear() { memset(this, 0, sizeof(Location)); };
			float x;
			float y;
			float z;
			float r;
		};
		Location location;
		unsigned char equiped : 1, : 0;		// Equipado ou não, 1 YES, 0 NO
	};

	// Dolfine Locker
	struct DolfiniLocker {
		DolfiniLocker() { clear();  };
		~DolfiniLocker() {};
		void clear() {
			memset(pass, 0, sizeof(pass));
			pang = 0ull;
			locker = 0;
			pass_check = 0;

			if (!v_item.empty()) {
				v_item.clear();
				v_item.shrink_to_fit();
			}
		};
		uint32_t isLocker() {
			
			if (pass[0] == '\0')
				return 2;	// Senha não foi criada ainda
			else if (!locker && pass_check)
				return 76;// 1;	// Senha já foi verificada para essa session

			return 76;	// Senha ainda não foi verificada para essa session
		};
		bool ownerItem(uint32_t _typeid) {

			auto it = std::find_if(v_item.begin(), v_item.end(), [&](auto& _el) {
				return (_el.item._typeid == _typeid);
			});

			return (it != v_item.end() ? true : false);
		};
		char pass[7];
		uint64_t pang;
		unsigned char locker;				// Essa opção tem que ser do server para pedir para o player verificar a senha todas vez do locker
		unsigned char pass_check : 1, : 0;	// 1 já foi verificado a senha nessa session, 0 ainda não foi verificada
		std::vector< DolfiniLockerItem > v_item;
	};

	/**** Base Item do pacote 0x216 Update Item No Game
	**/
	struct stItem216 {
		void clear() { memset(this, 0, sizeof(stItem216)); };
		unsigned char type;
		uint32_t _typeid;
		int32_t id;
		uint32_t flag_time;
		uint32_t qntd_ant;
		uint32_t qntd_dep;
		uint32_t qntd;
		unsigned short c[5];
		unsigned char ucc_idx[9];
		unsigned char seq;	// ou stats
		uint32_t card_typeid;
		unsigned char card_slot;
	};
	/**/

	// Item Generico
	struct stItem {
		void clear() { memset(this, 0, sizeof(stItem)); };
		int32_t id;
		uint32_t _typeid;

		unsigned char type_iff;		// Tipo que está no iff structure, tipo no Part.iff, 1 parte de baixo da roupa, 3 luva, 8 e 9 UCC etc
		unsigned char type;			// 2 Normal Item
		unsigned char flag;			// 1 Padrão item fornecido pelo server, 5 UCC_BLANK
		unsigned char flag_time;	// 6 rental(dia), 2 hora(acho), 4 minuto(acho)
		int32_t qntd;

		char name[64];
		char icon[41];

		struct item_stat {
			void clear() { memset(this, 0, sizeof(item_stat)); };
			int32_t qntd_ant;
			int32_t qntd_dep;
		};

		item_stat stat;

		struct UCC {
			void clear() { memset(this, 0, sizeof(UCC)); };
			char IDX[9];			// UCC INDEX STRING
			uint32_t status;
			uint32_t seq;
		};

		UCC ucc;

		unsigned char is_cash : 1, : 0;
		uint32_t price;
		uint32_t desconto;

		struct stDate {
			void clear() { memset(this, 0, sizeof(stDate)); };
			uint32_t active : 1, : 0;	// 1 Actived, 0 Desatived
			struct stDateSys {
				void clear() { memset(this, 0, sizeof(stDateSys)); };
				SYSTEMTIME sysDate[2];				// 0 Begin, 1 End
			}date;
		};

		stDate date;
		unsigned short date_reserve;

		short c[5];
#define STDA_C_ITEM_QNTD c[0]
#define STDA_C_ITEM_TICKET_REPORT_ID_HIGH c[1]
#define STDA_C_ITEM_TICKET_REPORT_ID_LOW c[2]
#define STDA_C_ITEM_TIME c[3]
	};

	// stItem Extended
	struct stItemEx : public stItem {
		stItemEx(uint32_t _ul = 0u) {
			clear();
		}
		void clear() { memset(this, 0, sizeof(stItemEx)); };
		struct ClubSetWorkshop {
			void clear() { memset(this, 0, sizeof(ClubSetWorkshop)); };
			unsigned short c[5];
			uint32_t mastery;
			char level;
			uint32_t rank;
			uint32_t recovery;
		};
		ClubSetWorkshop clubset_workshop;
	};

	// Location
	struct Location {
		void clear() { memset(this, 0, sizeof(Location)); };
		double diffXZ(Location& _l) {
			return sqrt(pow(x - _l.x, 2) + pow(z - _l.z, 2));
		};
		static double diffXZ(Location& _l1, Location& _l2) {
			return sqrt(pow(_l1.x - _l2.x, 2) + pow(_l1.z - _l2.z, 2));
		};
		double diff(Location& _l) {
			return sqrt(pow(x - _l.x, 2) + pow(y - _l.y, 2) + pow(z - _l.z, 2));
		};
		static double diff(Location& _l1, Location& _l2) {
			return sqrt(pow(_l1.x - _l2.x, 2) + pow(_l1.y - _l2.y, 2) + pow(_l1.z - _l2.z, 2));
		};
		std::string toString() {
			return "X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z: " + std::to_string(z) + " R: " + std::to_string(r);
		};
		float x;
		float y;
		float z;
		float r;	// face
	};

	// Canal Info
	struct ChannelInfo {
		// Union Flag do Canal, que guarda estrutura de bits da flag do Canal
		union uFlag {
			uFlag(uint32_t _ul = 0u) : ulFlag(_ul) {};
			void clear() { ulFlag = 0u; };
			uint32_t ulFlag;
			struct {
				uint32_t : 9;								// Unknown
				uint32_t junior_bellow : 1;				// De Junior A para baixo
				uint32_t junior_above : 1;					// De Junior E para cima
				uint32_t only_rookie : 1;					// Somente Rookie(Iniciante)
				uint32_t beginner_between_junior : 1;		// De Beginner a Junior
				uint32_t junior_between_senior : 1, : 0;	// De Junior a Senior
			}stBit;
		};
		void clear() {
			memset(this, 0, sizeof(ChannelInfo));
		};
		char name[64];
		short max_user;
		short curr_user;
		unsigned char id;
		//int32_t flag;
		uFlag flag;
		int32_t flag2;
		int32_t min_level_allow;
		int32_t max_level_allow;
	};

	// ServerInfoEx2
	struct ServerInfoEx2 : ServerInfoEx {
		~ServerInfoEx2() {};
		void clear() {
			ServerInfoEx::clear();
			
			if (!v_ci.empty()) {
				v_ci.clear();
				v_ci.shrink_to_fit();
			}
		};
		std::vector< ChannelInfo > v_ci;
	};

	// Message Off
	struct MsgOffInfo {
		void clear() {
			memset(this, 0, sizeof(MsgOffInfo));
		};
		int32_t from_uid;
		short id;
		char nick[22];
		char msg[64];
		char date[17];
	};

	// Attendence Reward Info
	struct AttendanceRewardInfo {
		AttendanceRewardInfo(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(AttendanceRewardInfo));
		};
		unsigned char login;
		struct item {
			void clear() {
				memset(this, 0, sizeof(item));
			};
			int32_t _typeid;
			int32_t qntd;
		};
		item now;
		item after;
		int32_t counter;
	};

	// Attendance Reward Info Ex
	struct AttendanceRewardInfoEx : public AttendanceRewardInfo {
		AttendanceRewardInfoEx(uint32_t _ul = 0u) : AttendanceRewardInfo(_ul) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(AttendanceRewardInfoEx)); };
		SYSTEMTIME last_login;	// Data do ultimo login
	};

	// Attendance Reward Item Context
	struct AttendanceRewardItemCtx {
		void clear() { memset(this, 0, sizeof(AttendanceRewardItemCtx)); };
		uint32_t _typeid;
		int32_t qntd;
		unsigned char tipo;
	};

	// Last Five Players Played with player
	struct Last5PlayersGame {
		Last5PlayersGame(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(Last5PlayersGame));
		};
		struct LastPlayerGame {
			void clear() {
				memset(this, 0, sizeof(LastPlayerGame));
			};
			uint32_t sex;	// gender, genero, sexo, 0 masculino, 1 Feminino
			char nick[22];
			char id[22];
			uint32_t uid;
		};
		void add(player_info& _pi, uint32_t _sex) {

			if (players[0].uid != _pi.uid) {

				auto it = std::find_if(players, players + 5, [&](auto& _el) {
					return _el.uid == _pi.uid;
				});

				// Put Player Last Position
				if (it != (players + 5/*End*/))
					std::rotate(it, it + 1, players + 5/*End*/);

				// Rotate To Right
				std::rotate(players, players + 4, players + 5/*End*/);

				// Update
				players[0].uid = _pi.uid;
#if defined(_WIN32)
				memcpy_s(players[0].id, sizeof(players[0].id), _pi.id, sizeof(players[0].id));
#elif defined(__linux__)
				memcpy(players[0].id, _pi.id, sizeof(players[0].id));
#endif

			}	// já está em primeiro não precisa mexer mais, só atualizar o friend e o nick, que ele pode ter mudado
			
			players[0].sex = _sex;
#if defined(_WIN32)
			memcpy_s(players[0].nick, sizeof(players[0].nick), _pi.nickname, sizeof(players[0].nick));
#elif defined(__linux__)
			memcpy(players[0].nick, _pi.nickname, sizeof(players[0].nick));
#endif
		};
		LastPlayerGame players[5];	// Last Five Players Played with player
	};

	// Friend Info
	struct FriendInfo {
		FriendInfo(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(FriendInfo));

#if defined(_WIN32)
			memcpy_s(apelido, sizeof(apelido), "Friend", 7);
#elif defined(__linux__)
			memcpy(apelido, "Friend", 7);
#endif
		};
		uint32_t uid;
		unsigned char sex;	// gender, genero, sexo, 0 masculino, 1 Feminino
		char id[22];
		char nickname[22];
		char apelido[15];
	};

	// Daily Quest Info
	struct DailyQuestInfo {
		DailyQuestInfo(uint32_t _ul = 0u) {
			clear();
		};
		DailyQuestInfo(uint32_t _typeid_0, uint32_t _typeid_1, uint32_t _typeid_2, SYSTEMTIME& _st)
			: _typeid{ _typeid_0, _typeid_1, _typeid_2 }, date(_st) {
		}
		void clear() { memset(this, 0, sizeof(DailyQuestInfo)); };
		std::string toString() {
			return "QUEST_TYPEID_0=" + std::to_string(_typeid[0]) + ", QUEST_TYPEID_1=" + std::to_string(_typeid[1]) + ", QUEST_TYPEID_2=" 
					+ std::to_string(_typeid[2]) + ", UPDATE_DATE=" + _formatDate(date);
		};
		//char date[20];				// Data
		SYSTEMTIME date;			// System Time Windows
		uint32_t _typeid[3];	// array[3] Typeid da Quest
	};

	// Daily Quest Info User
	struct DailyQuestInfoUser {
		void clear() {
			memset(this, 0, sizeof(DailyQuestInfoUser));
		};
		uint32_t now_date;		// Data que a quest está (current quest), do sistema de daily quest
		uint32_t accept_date;	// Data da ultima quest que foi aceita
		uint32_t current_date;	// Data que a quest está (current quest), do player
		uint32_t count;		// count de quest que tem no dia
		uint32_t _typeid[3];	// Max 3 quest por dia
	};

	// Remove Daily Quest
	struct RemoveDailyQuestUser {
		void clear() {
			memset(this, 0, sizeof(RemoveDailyQuestUser));
		};
		int32_t id;
		uint32_t _typeid;
	};

	// Add DailyQuest
	struct AddDailyQuestUser {
		void clear() { memset(this, 0, sizeof(AddDailyQuestUser)); };
		char name[64];
		uint32_t _typeid;
		uint32_t quest_typeid;
		int status;
	};

	// Player Canal Info
	struct PlayerCanalInfo {
		void clear() {
			memset(this, 0, sizeof(PlayerCanalInfo));
		};
		uint32_t uid;
		uint32_t oid;
		short sala_numero;
		char nickname[22];
		unsigned char level;
		//uint32_t capability;
		uCapability capability;
		int32_t title;
		int32_t team_point;				// Acho que é o team point
		union uStateFlag {
			void clear() {
				memset(this, 0, sizeof(uStateFlag));
			};
			unsigned char ucByte;
			struct {
				unsigned char away : 1;				// AFK
				unsigned char sexo : 1;				// Genero
				unsigned char quiter_1 : 1;			// Quit rate maior que 31% e menor que 41%
				unsigned char quiter_2 : 1;			// Quit rate maior que 41%
				unsigned char azinha : 1;			// Azinha, Quit rate menor que 3%
				unsigned char icon_angel : 1;		// Angel Wings
				unsigned char ucUnknown_bit7 : 1;	// Unknown Bit 7
				unsigned char ucUnknown_bit8 : 1;	// Unknown Bit 8
			}sBit;
		};
		uStateFlag state_flag;
		uint32_t guid_uid;
		uint32_t guild_index_mark;
		char guild_mark_img[12];
		unsigned short flag_visible_gm;
		int32_t l_unknown;
		char nickNT[22];				// Acho
		char unknown106[106];
	};

	// Player Room Info {
	struct PlayerRoomInfo {
		PlayerRoomInfo() { clear(); };
		void clear() {
			memset(this, 0, sizeof(PlayerRoomInfo));
			
			state_flag.clear();
		};
		uint32_t oid;
		char nickname[22];
		char guild_name[20];
		unsigned char position;
		//uint32_t capability;
		uCapability capability;
		uint32_t title;
		uint32_t char_typeid;		// Character Typeid
		uint32_t skin[6];
		struct StateFlag {
			void clear() {
				memset(this, 0, sizeof(StateFlag));

				uFlag.stFlagBit.ready = 0; // Unready
			};
			union {
				unsigned short usFlag;
				unsigned char ucByte[2];
				struct {
					unsigned char team : 1;				// Team que está na sala
					unsigned char team2 : 1;			// Esse é relacionado com o team, mas acho que seja do torneio team ou guild battle
					unsigned char away : 1;				// AFK
					unsigned char master : 1;			// Master da sala
					unsigned char master2 : 1;			// Master da sala, tbm esse aqui
					unsigned char sexo : 1;				// Genero
					unsigned char quiter_1 : 1;			// de 31% a 41% de quit rate
					unsigned char quiter_2 : 1;			// maior que 41% de quit rate
					unsigned char azinha : 1;			// menor que 3% de quit rate
					unsigned char ready : 1;			// se está pronto para começar o jogo, 0 Unready, 1 Ready
					unsigned char unknown_bit11 : 1;	// Unknown Bit 11, ainda não sei
					unsigned char unknown_bit12 : 1;	// Unknown Bit 12, ainda não sei
					unsigned char unknown_bit13 : 1;	// Unknown Bit 13, ainda não sei
					unsigned char unknown_bit14 : 1;	// Unknown Bit 14, ainda não sei
					unsigned char unknown_bit15 : 1;	// Unknown Bit 15, ainda não sei
					unsigned char unknown_bit16 : 1;	// Unknown Bit 16, ainda não sei
				}stFlagBit;
			}uFlag;
		};
		StateFlag state_flag;
		unsigned char level;
		unsigned char icon_angel;
		unsigned char ucUnknown_0A;			// Tem o valor 0x0A aqui quase sempre das vezes que vi esse pacote, Pode ser o Place(lugar que o player está) tipo Room = 10(hex:0x0A)
		uint32_t guild_uid;
		char guild_mark_img[12];
		uint32_t guild_mark_index;
		uint32_t uid;
		uint32_t state_lounge;
		unsigned short usUnknown_flg;		// Acho que seja uma flag tbm
		uint32_t state;				// Acho que seja estado de "lugar" pelo que lembro
		struct stLocation {
			void clear() { memset(this, 0, sizeof(stLocation)); };
			stLocation& operator+=(stLocation& _add_location) {
				x += _add_location.x;
				z += _add_location.z;
				r += _add_location.r;

				return *this;
			};
			float x;
			float z;
			float r;
		};
		stLocation location;
		struct PersonShop {
			void clear() { memset(this, 0, sizeof(PersonShop)); };
			uint32_t active;
			char name[64];
		};
		PersonShop shop;
		union uItemBoost {
			void clear() { memset(this, 0, sizeof(uItemBoost)); };
			unsigned short ulItemBoost;
			struct _stItemBoost {
				unsigned char ucPangMastery : 1, : 0;	// Pang Mastery X2
				unsigned char ucPangNitro : 1, : 0;		// Pang Nitro X4
			};
			_stItemBoost stItemBoost;
		};
		uint32_t mascot_typeid;
		uItemBoost flag_item_boost;		// Boost EXP, Pang e etc
		uint32_t ulUnknown_flg;		// Pode ser a flag de teasure do player, ou de drop item
		unsigned char id_NT[22];			// Acho que seja o ID na ntreev do player, a empresa que mantêm as contas, no JP era o gamepot
		unsigned char ucUnknown106[106];	// Unknown 106 bytes que sempre fica com valor 0
		unsigned char convidado : 1, : 0;	// Flag Convidado, [Não sei bem por que os que entra na sala normal tem valor igual aqui, já que é flag de convidado waiting]
		float avg_score;					// Media score "media de holes feito pelo player"
		unsigned char ucUnknown3[3];		// Não sei mas sempre é 0 depois do media score
	};

	// Player Room Info Ex
	struct PlayerRoomInfoEx : public PlayerRoomInfo {
		PlayerRoomInfoEx() : PlayerRoomInfo() {
			//clear();
			ci.clear();
		};
		void clear() { memset(this, 0, sizeof(PlayerRoomInfoEx)); };
		CharacterInfo ci;
	};

	// Sala Guild Info
	struct RoomGuildInfo {
		void clear() {
			memset(this, 0, sizeof(RoomGuildInfo));
			/*guild_1_uid = ~0;
			guild_2_uid = ~0;*/
		};
		uint32_t guild_1_uid;
		uint32_t guild_2_uid;
		char guild_1_mark[12];				// mark string o pangya JP não usa aqui fica 0
		char guild_2_mark[12];				// mark string o pangya JP não usa aqui fica 0
		unsigned short guild_1_index_mark;
		unsigned short guild_2_index_mark;
		char guild_1_nome[20];
		char guild_2_nome[20];
	};

	// Sala Grand Prix Info
	struct RoomGrandPrixInfo {
		void clear() {
			memset(this, 0, sizeof(RoomGrandPrixInfo));
		};
		uint32_t dados_typeid;
		uint32_t rank_typeid;
		uint32_t tempo;
		uint32_t active;
	};

	// Union Natural e Short Game Flag da sala, que guarda a estrutura de bits da flag da sala de natural e short game
	union uNaturalAndShortGame {
		uNaturalAndShortGame(uint32_t _ul = 0u) : ulNaturalAndShortGame(_ul) {};
		void clear() { ulNaturalAndShortGame = 0u; };
		uint32_t ulNaturalAndShortGame;
		struct {
			uint32_t natural : 1;			// Natural Modo
			uint32_t short_game : 1, : 0;	// Short Game Modo
		}stBit;
	};

	// SalaInfo
	struct RoomInfo {
		enum eCOURSE : unsigned char {
			BLUE_LAGOON,
			BLUE_WATER,
			SEPIA_WIND,
			WIND_HILL,
			WIZ_WIZ,
			WEST_WIZ,
			BLUE_MOON,
			SILVIA_CANNON,
			ICE_CANNON,
			WHITE_WIZ,
			SHINNING_SAND,
			PINK_WIND,
			DEEP_INFERNO = 13,
			ICE_SPA,
			LOST_SEAWAY,
			EASTERN_VALLEY,
			CHRONICLE_1_CHAOS,
			ICE_INFERNO,
			WIZ_CITY,
			ABBOT_MINE,
			MYSTIC_RUINS,
			GRAND_ZODIAC = 64,
			RANDOM = 127,
		};
		enum TIPO : uint32_t {
			STROKE,
			MATCH,
			LOUNGE,
			TOURNEY = 4,
			TOURNEY_TEAM,
			GUILD_BATTLE,
			PANG_BATTLE,
			APPROCH = 10,
			GRAND_ZODIAC_INT,
			GRAND_ZODIAC_ADV = 13,
			GRAND_ZODIAC_PRACTICE,
			SPECIAL_SHUFFLE_COURSE = 18,
			PRACTICE,
			GRAND_PRIX,
		};
		enum MODO : uint32_t {
			M_FRONT,
			M_BACK,
			M_RANDOM,
			M_SHUFFLE,
			M_REPEAT,
			M_SHUFFLE_COURSE,
		};
		enum INFO_CHANGE : uint32_t {
			NAME,
			SENHA,
			TIPO,
			COURSE,
			QNTD_HOLE,
			MODO,
			TEMPO_VS,
			MAX_PLAYER,
			TEMPO_30S,
			STATE_FLAG,
			UNKNOWN,
			HOLE_REPEAT,
			FIXED_HOLE,
			ARTEFATO,
			NATURAL,
		};
		RoomInfo(uint32_t _ul = 0u) {	// Valores padrões
			
			clear();
			
		};
		void clear() {

			memset(this, 0, sizeof(RoomInfo));

			numero = -1;
			senha_flag = 1;
			state = 1;
			_30s = 30;

			guilds.clear(); // Valores inicias
		};
		char nome[64];
		unsigned char senha_flag : 1, : 0;	// Sala sem senha = 1, Sala com senha = 0
		unsigned char state : 1, : 0;		// Sala em espera = 1, Sala em Game = 0
		unsigned char flag;					// Sala que pode entrar depois que começou = 1
		unsigned char max_player;
		unsigned char num_player;
		char key[17];
		unsigned char _30s;					// Modo Multiplayer do pangya acho, sempre 0x1E (dec: 30) no pangya
		unsigned char qntd_hole;
		unsigned char tipo_show;			// esse é o tipo que mostra no pacote, esse pode mudar dependendo do tipo real da sala, fica ou camp, ou VS ou especial, não coloca todos os tipos aqui
		short numero;
		unsigned char modo;
		//unsigned char course;
		eCOURSE course;
		uint32_t time_vs;
		uint32_t time_30s;
		uint32_t trofel;
		unsigned short state_flag;			// Quando é sala de 100 player o mais de gm event aqui é 0x100
		RoomGuildInfo guilds;
		uint32_t rate_pang;
		uint32_t rate_exp;
		unsigned char flag_gm;
		int		   master;			// Tem valores negativos, por que a sala usa ele para grand prix e etc
		unsigned char tipo_ex;			// tipo extended, que fala o tipo da sala certinho
		uint32_t artefato;			// Aqui usa pra GP efeitos especiais do GP
		//uint32_t natural;			// Aqui usa para Short Game Também
		uNaturalAndShortGame natural;		// Aqui usa para Short Game Também
		RoomGrandPrixInfo grand_prix;
	};

	struct RoomInfoEx : public RoomInfo {
		RoomInfoEx(uint32_t _ul = 0u) : RoomInfo(_ul) {

			hole_repeat = 0u;
			fixed_hole = 0u;
			tipo = 0u;
			state_afk = 0u;
			channel_rookie = 0u;
			angel_event = 0u;
		};
		void clear() {
			
			RoomInfo::clear();

			hole_repeat = 0u;
			fixed_hole = 0u;
			tipo = 0u;
			state_afk = 0u;
			channel_rookie = 0u;
			angel_event = 0u;
		};
		char senha[64];						// Senha da sala
		unsigned char tipo;					// Tipo real da sala
		unsigned char hole_repeat;			// Número do hole que vai ser repetido
		uint32_t fixed_hole;			// Aqui é 1 Para Hole(Pin"Beam") Fixo, e 0 para aleatório
		unsigned char state_afk : 1, : 0;	// Estado afk da sala, usar para depois começar a sala, já que o pangya não mostra se a sala está afk
		unsigned char channel_rookie : 1;	// Flag que guarda, se o channel é rookie ou não, onde a sala foi criada, vem da Flag do channel
		unsigned char angel_event : 1;		// Flag que guarda se o Angel Event está ligado
	};

	/*struct GameValue {
		void clear() { memset(this, 0, sizeof(GameValue)); };
		uint32_t master;
		unsigned char tipo;
		unsigned char course;
		unsigned char modo;
		unsigned char qntd_hole;
		uint32_t artefato;
		uint32_t tempo;
		uint32_t* pTrofel;
		uint32_t fixed_hole;
		uint32_t hole_repeat;
		unsigned char vento_rookie;
		unsigned char senha_flag;
		uint32_t natural;
	};*/

	struct RateValue {
		void clear() { memset(this, 0, sizeof(RateValue)); };
		uint32_t pang;
		uint32_t exp;
		uint32_t clubset;
		uint32_t rain;
		uint32_t treasure;
		unsigned char persist_rain;
	};

	// Client Version
	struct ClientVersion {
		ClientVersion() {
			memset(this, 0, sizeof(ClientVersion));

			flag = REDUZI_VERSION;
		};
		ClientVersion(uint32_t _high, uint32_t _low) {
			memset(this, 0, sizeof(ClientVersion));
			
			high = _high;
			low = _low;

			flag = REDUZI_VERSION;
		};
		ClientVersion(const char _region[3], const char _season[3], uint32_t _high, uint32_t _low) {
			memset(this, 0, sizeof(ClientVersion));

			if (_region == nullptr || _season == nullptr)
				throw exception("Error argument invalid, _region or _season is nullptr. ClientVersion::ClientVersion()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENTVERSION, 7, 0));

#if defined(_WIN32)
			strcpy_s(region, _region);
			strcpy_s(season, _season);
#elif defined(__linux__)
			strcpy(region, _region);
			strcpy(season, _season);
#endif

			high = _high;
			low = _low;

			flag = COMPLETE_VERSION;
		};
		static ClientVersion make_version(std::string& _cv) {

			ClientVersion cv;

			if (_cv.empty())
				throw exception("Error cv is empty, ClientVersion::make_version()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENTVERSION, 1, 0));

			std::vector< std::string > v_s;
			
			const char* start = _cv.data();
			const char *tmp = start;
			
			size_t i;

			while ((tmp = strpbrk(tmp, ".")) != nullptr) {
				v_s.push_back(std::string(start, tmp - start));
				
				if ((i = strspn(tmp, ".")))
					tmp += i;
				
				start = tmp;
			}

#if defined(_WIN32)
			if (start < _cv.end()._Ptr)
				v_s.push_back(std::string(start, _cv.end()._Ptr - start));
#elif defined(__linux__)
			if (start < &(*_cv.end()))
				v_s.push_back(std::string(start, &(*_cv.end()) - start));
#endif
			
			if (v_s.empty())
				throw exception("Error Invalid argument. ClientVersion::make_version()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENTVERSION, 2, 0));

			if (v_s.size() < 2)
				throw exception("Erro Not string token enough, ClientVersion::make_version()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENTVERSION, 3, 0));

			try {
				if (v_s.size() == 2)
					cv = { (unsigned int )std::stoi(v_s[0]), (unsigned int )std::stoi(v_s[1]) };
				else if (v_s.size() == 4)
					cv = { v_s[0].c_str(), v_s[1].c_str(), (unsigned int )std::stoi(v_s[2]), (unsigned int )std::stoi(v_s[3]) };
				else
					throw exception("Error pegou token string estranho. ClientVersion::make_version()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENTVERSION, 4, 0));
			}catch (std::invalid_argument& e) {
				throw exception("Error invalid argument std::stoul(), ClientVersion::make_version(). " + std::string(e.what()), STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENTVERSION, 5, errno));
			}catch (std::out_of_range& e) {
				throw exception("Error out of range std::stoul(), ClientVersion::make_version(). " + std::string(e.what()), STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENTVERSION, 6, errno));
			}

			v_s.clear();
			v_s.shrink_to_fit();

			return cv;
		};
		std::string fixedValue(uint32_t _value, uint32_t _width) {

			std::ostringstream results;

			results.fill('0');

			results.setf(std::ios_base::internal, std::ios_base::adjustfield);

			results << std::setw((_value < 0 ? _width + 1 : _width)) << _value;

			return results.str();
		};
		std::string toString() {
			return std::string(region) + "." + std::string(season) + "." + fixedValue(high, 2) + "." + fixedValue(low, 2);
		};
		char region[3];
		char season[3];
		uint32_t high;
		uint32_t low;
		unsigned char flag : 1;
		enum : unsigned char {
			REDUZI_VERSION,
			COMPLETE_VERSION,
		};
	};

	// Item Pangya Base Para Pacote216
	struct ItemPangyaBase {
		void clear() {
			memset(this, 0, sizeof(ItemPangyaBase));
		};
		unsigned char tipo;
		int32_t _typeid;
		int32_t id;
		int32_t tipo_unidade_add;
		int32_t qntd_ant;
		int32_t qntd_dep;
		int32_t qntd;
		unsigned char unknown[8];
		short qntd_time;
	};

	struct ItemPangya : public ItemPangyaBase {
		ItemPangya() : ItemPangyaBase() {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(ItemPangya));
		};
		char sd_idx[9];
		int32_t sd_status;
		int32_t sd_seq;
		unsigned char unknown2[5];
	};

	// BuyItem
	struct BuyItem {
		void clear() { memset(this, 0, sizeof(BuyItem)); };
		uint32_t id;
		uint32_t _typeid;
		unsigned short time;
		unsigned short usUnknown;
		uint32_t qntd;
		uint32_t pang;
		uint32_t cookie;
		unsigned char ucUnknown13[13];
	};

	// Email Info
	struct EmailInfo {
		EmailInfo(uint32_t _ul = 0u) {
			clear();
		};
		~EmailInfo() {
			clear();
		};
		void clear() {

			id = -1;
			memset(from_id, 0, sizeof(from_id));
			memset(gift_date, 0, sizeof(gift_date));
			memset(msg, 0, sizeof(msg));
			lida_yn = 0u;

			if (!itens.empty()) {
				itens.clear();
				itens.shrink_to_fit();
			}
		};
		int32_t id;
		char from_id[22];
		char gift_date[20];
		char msg[100];
		unsigned char lida_yn;
		struct item {
			void clear() { memset(this, 0, sizeof(item)); };
			int32_t id;
			uint32_t _typeid;
			unsigned char flag_time;
			int32_t qntd;
			int32_t tempo_qntd;
			uint64_t pang;
			uint64_t cookie;
			int32_t gm_id;
			int32_t flag_gift;
			char ucc_img_mark[9];
			unsigned char ucUnknown3[3];
			short type;
		};
		std::vector< EmailInfo::item > itens;
	};

	// EmailInfoEx
	struct EmailInfoEx : public EmailInfo {
		EmailInfoEx(uint32_t _ul = 0u) : EmailInfo(_ul), visit_count(0l) {
		};
		void clear() {

			EmailInfo::clear();

			visit_count = 0l;
		};
		int32_t visit_count;
	};

	// Mail Box
	struct MailBox {
		void clear() {
			memset(this, 0, sizeof(MailBox));
		};
		int32_t id;
		char from_id[30];
		char msg[80];
		char unknown2[18];
		int32_t visit_count;
		unsigned char lida_yn;
		int32_t item_num;			// Número de itens que tem nesse anexado a esse email
		EmailInfo::item item;
	};

	// Ticket Report Scroll Info
	struct TicketReportScrollInfo {
		TicketReportScrollInfo(uint32_t _ul = 0u) {
			clear();
		};
		~TicketReportScrollInfo() {};
		void clear() {

			id = -1;
			date = { 0 };

			if (!v_players.empty()) {
				v_players.clear();
				v_players.shrink_to_fit();
			}
		};
		struct stPlayerDados {
			stPlayerDados(uint32_t _ul = 0u) {
				clear();
			};
			void clear() {
				memset(this, 0, sizeof(stPlayerDados));

				ucUnknown_flg = 2u;
			};
			uint32_t uid;
			uint64_t pang;
			uint64_t bonus_pang;
			uint32_t trofel_typeid;
			uint32_t exp;
			uint32_t mascot_typeid;
			unsigned char premium_user : 1, : 0;
			unsigned char item_boost : 3, : 0; // [Bit] 1 = Pang Mastery x2, 2 = Pang Nitro x4, 3 = (ACHO) Exp x2
			uint32_t level;
			char score;
			uMedalWin medalha;
			unsigned char trofel;
			char id[22];
			char nickname[22];
			uint32_t ulUnknown;
			uint32_t guild_uid;
			uint32_t mark_index;		// Guild, isso é do JP, que ele nao usa o EMBLEM NUMER
			char guild_mark_img[12];
			uint32_t tipo;
			unsigned char state;
			unsigned char ucUnknown_flg;	// Não sei mas sempre peguei o valor 2
			SYSTEMTIME finish_time;
		};
		int32_t id;
		SYSTEMTIME date;
		std::vector< stPlayerDados > v_players;
	};

	// Estrutura que Guarda as informações dos Convites do Canal
	struct InviteChannelInfo {
#define STDA_INVITE_TIME_MILLISECONDS		5000	// 5 segundos em millisegundos
		void clear() { memset(this, 0, sizeof(InviteChannelInfo)); };
		short room_number;
		uint32_t invite_uid;
		uint32_t invited_uid;
		SYSTEMTIME time;
	};

	// Command Info, os Comando do Auth Server
	struct CommandInfo {
		CommandInfo(uint32_t _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(CommandInfo)); };
		std::string toString() {
			return "IDX=" + std::to_string(idx) + ", ID=" + std::to_string(id) + ", ARG1="
				+ std::to_string(arg[0]) + ", ARG2=" + std::to_string(arg[1]) + ", ARG3="
				+ std::to_string(arg[2]) + ", ARG4=" + std::to_string(arg[3]) + ", ARG5="
				+ std::to_string(arg[4]) + ", TARGET=" + std::to_string(target) + ", FLAG="
				+ std::to_string(flag) + ", VALID=" + std::to_string((unsigned short)valid) + ", RESERVEDATE=" + std::to_string(reserveDate);
		};
		uint32_t idx;
		uint32_t id;
		uint32_t arg[5];
		uint32_t target;
		unsigned short flag;
		unsigned char valid : 1;
		time_t reserveDate;
	};

	// Update Item
	struct UpdateItem {
		enum UI_TYPE : unsigned char {
			CADDIE,
			CADDIE_PARTS,
			MASCOT,
			WAREHOUSE,
		};
		UpdateItem(uint32_t _ul = 0u) {
			clear();
		};
		UpdateItem(UI_TYPE _type, uint32_t __typeid, int32_t _id)
			: type(_type), _typeid(__typeid), id(_id) {

		};
		void clear() {
			memset(this, 0, sizeof(UpdateItem));
		};
		UI_TYPE type;
		uint32_t _typeid;
		int32_t id;
	};

	// Grand Prix Clear
	struct GrandPrixClear {
		GrandPrixClear(uint32_t _ul = 0u) {
			clear();
		};
		GrandPrixClear(uint32_t __typeid, uint32_t _position)
			: _typeid(__typeid), position(_position) {
		};
		void clear() {
			memset(this, 0, sizeof(GrandPrixClear));
		};
		uint32_t _typeid;
		uint32_t position;
	};

	// Guild Update Activity Info
	// Guarda os dados das atualizações que os Clubs tem de alterações
	// Como membro kickado, sair do club e aceito no club
	struct GuildUpdateActivityInfo {
		enum TYPE_UPDATE : unsigned char {
			TU_ACCEPTED_MEMBER,
			TU_EXITED_MEMBER,
			TU_KICKED_MEMBER,
		};
		void clear() {
			memset(this, 0, sizeof(GuildUpdateActivityInfo));
		};
		uint64_t index;	// ID do update activity
		uint32_t club_uid;
		uint32_t owner_uid;	// Quem fez a Ação
		uint32_t player_uid;
		TYPE_UPDATE type;
		time_t reg_date;
	};

	struct ChangePlayerItemRoom {
		ChangePlayerItemRoom(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(ChangePlayerItemRoom));
		};
		enum TYPE_CHANGE : unsigned char {
			TC_CADDIE = 1,
			TC_BALL,
			TC_CLUBSET,
			TC_CHARACTER,
			TC_MASCOT,
			TC_ITEM_EFFECT_LOUNGE,	// Hermes x2, Twilight, Jester x2
			TC_ALL,					// CHARACTER, CADDIE, CLUBSET e BALL essa é a ordem
			TC_UNKNOWN = 255,
		};
		struct stItemEffectLounge {
			void clear() { memset(this, 0, sizeof(stItemEffectLounge)); };
			enum TYPE_EFFECT : uint32_t {
				TE_BIG_HEAD = 1,
				TE_FAST_WALK,
				TE_TWILIGHT,
			};
			uint32_t item_id;	// Aqui ele manda 0 o cliente, não sei por que, deveria mandar o id do item equipado
			TYPE_EFFECT effect;
		};
		TYPE_CHANGE type;					// Type Change
		uint32_t caddie;				// Caddie ID
		uint32_t ball;					// Ball Typeid
		uint32_t clubset;				// ClubSet ID
		uint32_t character;			// Character ID
		uint32_t mascot;				// Mascot ID
		stItemEffectLounge effect_lounge;	// Item effect lounge
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif