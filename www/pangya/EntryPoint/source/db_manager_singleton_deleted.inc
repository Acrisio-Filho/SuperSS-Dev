<?php
    // Arquivo db_manager_singleton.inc
    // Criado em 19/07/2019 as 05:11 por Acrisio
    // Definição e Implementação da classe DBManagerSingleton

    // Config do Pangya Document Root
    include_once($_SERVER['DOCUMENT_ROOT']."/config/config.inc");

    if (_MSSQL_)
        include_once($_SERVER['DOCUMENT_ROOT']."/config/mssql_odbc_manager.inc");
    else
        include_once($_SERVER['DOCUMENT_ROOT']."/config/MysqlManager.inc");

    class DBManagerSingleton {

        private static $db = null;
        private static $params = null;
        private static $con_dados = null; 

        public static function getInstanceDB() {

            if (self::$db == null) {
               
                if (_MSSQL_)
                    self::$db = new MSSqlODBCManager(self::getInstanceConDados());
                else
                    self::$db = new MysqlManager(self::getInstanceConDados());
            }

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