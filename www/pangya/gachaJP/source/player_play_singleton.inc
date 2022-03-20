<?php
    // Arquivo player_play_singleton.inc
    // Criado em 24/05/2020 as 02:44 por Acrisio
    // Definição e Implementação da classe PlayerPlaySingleton

    class PlayerPlaySingleton {
        static private $player_play = null;

        static public function &getInstance() {

            if (self::$player_play == null) {

                if (!isset($_SESSION))
                    session_start();

                if (!isset($_SESSION['player_play']) || !isset($_SESSION['player_play']['created']) || $_SESSION['player_play']['created'] == false 
                    || !isset($_SESSION['player_play']['modo']) || !is_numeric($_SESSION['player_play']['modo'])
                    || !isset($_SESSION['player_play']['itens']) || !is_array($_SESSION['player_play']['itens']) || empty($_SESSION['player_play']['itens'])
                    || !isset($_SESSION['player_play']['SpinningLottery'])) {

                    // Limpa e cria uma nova com o valor falso
                    unset($_SESSION['player_play']);

                    $_SESSION['player_play'] = ['created' => false];

                }

                // Passa a referência da SESSION['player_play']
                self::$player_play = &$_SESSION['player_play'];
            }

            return self::$player_play;
        }
    }
?>