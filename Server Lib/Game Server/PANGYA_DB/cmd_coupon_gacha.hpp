// Arquivo cmd_coupon_gacha.hpp
// Criado em 19/03/2018 as 22:05 por Acrisio
// Definição da classe CmdCouponGacha

#pragma once
#ifndef _STDA_CMD_COUPON_GACHA_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
    class CmdCouponGacha : public pangya_db {
        public:
            explicit CmdCouponGacha(bool _waiter = false);
            CmdCouponGacha(uint32_t _uid, bool _waiter = false);
            virtual ~CmdCouponGacha();

            CouponGacha& getCouponGacha();
            void setCouponGacha(CouponGacha& _cg);

            uint32_t getUID();
            void setUID(uint32_t _uid);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCouponGacha"; };
			virtual std::wstring _wgetName() override { return L"CmdCouponGacha"; };

        private:
            uint32_t m_uid;
            CouponGacha m_cg;

            // 0x1A000080 Normal, 0x1A000083 Partial
            const char* m_szConsulta[2] = { "SELECT c0 FROM pangya.pangya_item_warehouse WHERE typeid = 436207744 AND uid = ", "SELECT c0 FROM pangya.pangya_item_warehouse WHERE typeid = 436207747 AND uid = " };
    };
}

#endif