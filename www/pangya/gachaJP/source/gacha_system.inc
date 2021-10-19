<?php
    // Arquivo gacha_system.inc
    // Criado em 23/05/2020 as 17:52 por Acrisio
    // Definição e Implementação da classe GachaSystem

    include_once("design.inc");

    abstract class GachaSystem {

        abstract public function show();
        abstract protected function style();

        protected function begin() {

            Design::beginHTML();

            Design::beginHead();

            $this->style();
        }

        protected function middle() {

            Design::endHead();
            
            Design::beginBody();
            
            Design::beginContent();
        }

        protected function end() {

            Design::endContent();
            
            Design::endBody();
            
            Design::endHTML();
        }

        public function getAllTicketPlay() {
            return PlayerSingleton::getTotalTicketToPlay(PlayerSingleton::getInstance());
        }
    }
?>