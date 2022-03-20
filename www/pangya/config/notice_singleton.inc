<?php
    // Arquivo notice_singleton.inc
    // Criado em 21/10/2019 as 21:18 por Acrisio
    // Definição e Implementação da classe NoticeSingleton

    include_once("notice_manager.inc");

    class NoticeSingleton {

        static private $notices = null;

        static public function &getInstance() {

            if (self::$notices == null) {

                self::$notices = new NoticeManager();
            }

            return self::$notices;
        }
    }
?>