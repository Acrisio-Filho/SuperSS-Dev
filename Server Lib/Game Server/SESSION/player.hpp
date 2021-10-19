// Arquivo player.hpp
// Criado em 07/03/2018 as 21:58 por Acrisio
// Definição da classe player polimofismo

#pragma once
#ifndef _STDA_PLAYER_HPP
#define _STDA_PLAYER_HPP

#include "../../Projeto IOCP/SOCKET/session.h"
#include "../TYPE/pangya_game_st.h"
#include "../TYPE/player_info.h"
#include "../TYPE/gm_info.hpp"

// Game Guard
#include "../TYPE/game_guard_type.hpp"

// !@ Teste
#include "../UTIL/block_exec_one_per_time.hpp"

namespace stdA {
    class player : public session {
        public:
            player(threadpool_base& _threapool);
            virtual ~player();

			virtual bool clear() override;

			virtual unsigned char getStateLogged() override;

			virtual uint32_t getUID() override;
			virtual uint32_t getCapability() override;
			virtual char* getNickname() override;
			virtual char* getID() override;

			// Exp para o player
			virtual void addExp(uint32_t _exp, bool _upt_on_game = false);

			// Add (Soma) Exp para Caddie equipado
			virtual void addCaddieExp(uint32_t _exp);

			// Add (Soma) Exp para o Mascot equipado
			virtual void addMascotExp(uint32_t _exp);

			// Add Exp Estático
			static void addExp(uint32_t _uid, uint32_t _exp);

			// Pang
			virtual void addPang(uint64_t _pang);
			virtual void consomePang(uint64_t _pang);

			// Cookie
			virtual void addCookie(uint64_t _cookie);
			virtual void consomeCookie(uint64_t _cookie);

			// Moedas
			virtual void addMoeda(uint64_t _pang, uint64_t _cookie);
			virtual void consomeMoeda(uint64_t _pang, uint64_t _cookie);

			// Salva o Log no banco de dados de Gastos"Agora tem CP Pouch(ganha)" de CP(Cookie Point)
			virtual void saveCPLog(CPLog& _cp_log);

			// Salva o Log no banco de dados de Gastos"Agora tem CP Pouch(ganha)" de CP(Cookie Point)
			static void saveCPLog(uint32_t _uid, CPLog& _cp_log);

			// Verifica se Character os itens equipados são validos
			bool checkCharacterEquipedPart(CharacterInfo& ci);
			bool checkCharacterEquipedAuxPart(CharacterInfo& ci);
			bool checkCharacterEquipedCutin(CharacterInfo& ci);
			void checkCharacterAllItemEquiped(CharacterInfo& ci);

			bool checkSkinEquiped(UserEquip& _ue);
			bool checkPosterEquiped(UserEquip& _ue);
			bool checkCharacterEquiped(UserEquip& _ue);
			bool checkCaddieEquiped(UserEquip& _ue);
			bool checkMascotEquiped(UserEquip& _ue);
			bool checkClubSetEquiped(UserEquip& _ue);
			bool checkBallEquiped(UserEquip& _ue);
			bool checkItemEquiped(UserEquip& _ue);
			void checkAllItemEquiped(UserEquip& _ue);

			void equipDefaultCharacter(UserEquip& _ue);
			void equipDefaultClubSet(UserEquip& _ue);
			void equipDefaultBall(UserEquip& _ue);
			void equipDefaultBallPremiumUser(UserEquip& _ue);

			// Verifica se um AuxPart está equipado em todos Characters do player
			// Retorna um vector com ponteiro para cada Character Info se ele estiver equipado
			// Retorna vector vazio se ele não estiver equipado em nenhum character do player
			std::vector< CharacterInfo* > isAuxPartEquiped(uint32_t _typeid);

			// Verifica se um Part está equipado em algum Character do player
			// Retorna um ponteiro para o Character Info se ele estiver equipo
			// Retorna nullptr se ele não estiver equipado
			CharacterInfo* isPartEquiped(uint32_t _typeid);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

        public:
            PlayerInfo m_pi;
			GMInfo m_gi;

			// Game Guard
			PlayerGameGuard m_gg;

#if STDA_BLOCK_PACKET_ONE_TIME_DISABLE != 0x1
			// Block Packet Exec One Per Time
			SyncBlockExecOnePerTime* m_sbeopt;
#endif
    };
}

#endif