<?php
    // Arquivo player_singleton.inc
    // Criado em 23/05/2020 as 20:21 por Acrisio
    // Definição e Implementação da classe PlayerSingleton

    //include_once("db_manager_singleton.inc");
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');
    
    include_once("config.inc");

    class PlayerSingleton {
        static private $player = null;

        static public function &getInstance() {

            if (self::$player == null) {

                if (!isset($_SESSION))
                    session_start();
                    
                if (isset($_SESSION) && isset($_SESSION['player_log']) && isset($_SESSION['player_log']['uid'])) {
                    
                    $_SESSION['player_log']['logged'] = true;
                    
                    self::$player = &$_SESSION['player_log'];

                }else {

                    if (isset($_SESSION['player_log']))
                        unset($_SESSION['player_log']);

                    self::$player = ['logged' => false];

                }
            }

            return self::$player;
        }
    }

?>