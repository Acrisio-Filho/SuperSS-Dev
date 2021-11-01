<?php
    // Arquivo guild_new.inc
    // Criado em 16/07/2019 as 12:11 por Acrisio
    // Definição e Implementação da classe Guild Home Base

    include_once("design.inc");

    define("AUTH_LEVEL_MEMBER", 0);
    define("AUTH_LEVEL_REAL_MEMBER", 3);
    define("AUTH_LEVEL_ADMIN", 1);
    define("AUTH_LEVEL_SUBMASTER", 2);

    abstract class GuildHome {

        private $guild_id = null;

        protected $MEMBER_STATE_LABEL = [
            1 => 'Master',
            2 => 'Submaster',
            3 => 'Member',
            9 => 'Pending'
        ];

        abstract public function show();

        protected function isMember() {
            return PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] > 0 && PlayerSingleton::getInstance()['GUILD_UID'] == GuildSingleton::getInstance()['UID'];
        }

        protected function isRealMember() {
            return $this->isMember() && PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] != 9/*PENDING*/;
        }

        protected function isMaster() {
            return $this->isMember() && PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 1/*MASTER*/ && PlayerSingleton::getInstance()['UID'] == GuildSingleton::getInstance()['MASTER']['UID'];
        }

        protected function isSubmaster() {
            return $this->isMember() && PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 2/*SUBMASTER*/;
        }

        protected function isMasterOrSubmaster() {
            return $this->isMaster() || $this->isSubmaster();
        }

        protected function isBlocked() {
            return GuildSingleton::getInstanceFullInfo()['STATE'] == ST_GM_BLOCKED;
        }

        protected function isClosure() {
            return GuildSingleton::getInstanceFullInfo()['STATE'] == ST_CLOSURE || GuildSingleton::getInstanceFullInfo()['STATE'] == ST_GM_CLOSURE;
        }

        protected function checkAuthority($level) {

            if ($level == AUTH_LEVEL_MEMBER) {

                if (!$this->isMember()) {
                    
                    header("Location: ".LINKS['GUILD_ERROR']);

                    // sai do script para o navegador redirecionar
                    exit();
                }
            
            }else if ($level == AUTH_LEVEL_REAL_MEMBER) {

                if (!$this->isRealMember()) {

                    header("Location: ".LINKS['GUILD_ERROR']);

                    // sai do script para o navegador redirecionar
                    exit();
                }

            }else if ($level == AUTH_LEVEL_ADMIN) {

                if (!$this->isMaster()) {

                    header("Location: ".LINKS['GUILD_ERROR']);

                    // sai do script para o navegador redirecionar
                    exit();
                }
            
            }else if ($level == AUTH_LEVEL_SUBMASTER) {

                if (!$this->isMasterOrSubmaster()) {

                    header("Location: ".LINKS['GUILD_ERROR']);

                    // sai do script para o navegador redirecionar
                    exit();
                }
            }
        }

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