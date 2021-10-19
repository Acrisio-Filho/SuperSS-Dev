<?php
    // Arquivo guild_new.inc
    // Criado em 16/07/2019 as 09:54 por Acrisio
    // Definição e Implementação da classe Guild New Base

    include_once("design.inc");

    abstract class GuildNew {

        protected $ACTIVITY_LABEL = [
            1 => 'Apply',
            2 => 'Cancel application',
            3 => 'Accept application',
            4 => 'Reject application',
            5 => 'Promote/Demote',
            6 => 'Kick',
            7 => 'Leave',
            8 => 'Create',
            9 => 'Closed',
            10 => 'Move master',
            11 => 'Assign as master',
            12 => 'Promote to sub master',
            13 => 'Promote to member',
            14 => 'GM reject your club mark',
            15 => 'GM accept your club mark',
            16 => 'GM block your club',
            17 => 'GM closed your club',
            18 => 'GM unblock your club',
            19 => 'Re-open',
            20 => 'GM has re-open your club'
        ];

        abstract public function show();

        protected function begin() {

            Design::beginHTML();

            Design::beginHead();
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
    }

?>