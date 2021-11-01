<?php
    // Arquivo gacha_lottery_system.inc
    // Criado em 24/05/2020 as 02:34 por Acrisio
    // Definição e Implementação da classe GachaLotterySystem

    include_once("design_lottery.inc");

    abstract class GachaLotterySystem {

        abstract public function show();

        protected function begin() {

            DesignLottery::beginHTML();

            DesignLottery::beginHead();
        }

        protected function middle() {

            DesignLottery::endHead();
            
            DesignLottery::beginBody();
            
            DesignLottery::beginContent();
        }

        protected function end() {

            DesignLottery::endContent();
            
            DesignLottery::endBody();
            
            DesignLottery::endHTML();
        }

        public function getAllTicketPlay() {
            return PlayerSingleton::getTotalTicketToPlay(PlayerSingleton::getInstance());
        }
    }
?>