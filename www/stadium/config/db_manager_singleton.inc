<?php
    // Arquivo db_manager_singleton.inc
    // Criado em 19/07/2019 as 05:11 por Acrisio
    // Definição e Implementação da classe DBManagerSingleton

    include_once("config.inc");

    include_once("MysqlManager.inc");

    class DBManagerSingleton {

        private static $db = null;
        private static $params = null;
        private static $con_dados = null; 

        public static function getInstanceDB() {

            if (self::$db == null)
                self::$db = new MysqlManager(self::getInstanceConDados());

            return self::$db;
        }

        public static function getInstanceParams() {

            if (self::$params == null) {

                self::$params = new paramsArr();
            }

            return self::$params;
        }

        public static function getInstanceConDados() {
            global $con_dados;

            if (self::$con_dados == null) {

                self::$con_dados = $con_dados;
            }

            return self::$con_dados;
        }
    }
?>