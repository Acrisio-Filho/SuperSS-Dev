<?php
    // Arquivo database_config.inc
    // Criado em 03/08/2021 as 14:30 por Acrisio
    // Definição da classe DatabaseConfig

    class DatabaseConfig {

        // Types Config
        public const _MSSQL_ = '__MSSQL__'; // Microsoft SQL Server
        public const _MYSQL_ = '__MYSQL__'; // MySQL
        public const _PSQL_  = '__PSQL__';  // PostgreSQL

        private $db_config = [];

        public function __construct() {
            // empty
        }

        public static function isKnownType($type) {
            return self::_MSSQL_ === $type || self::_MYSQL_ === $type || self::_PSQL_ === $type;
        }

        public function addConfig($type, $server_ip, $db_name, $user, $pass) {

            // check if type is known
            if (!DatabaseConfig::isKnownType($type)) {

                // Error log
                error_log("[DatabaseConfig::addConfig][Error] Type Database Engine($type) is invalid.", 0);

                exit();
            }

            $this->db_config[$type] = [
                'IP_ADDR' => $server_ip,
                'DB_NAME' => $db_name,
                'User' => $user,
                'pwd' => $pass
            ];
        }

        public function getConfig($type) {

            // check if type is known
            if (!DatabaseConfig::isKnownType($type)) {

                // Error log
                error_log("[DatabaseConfig::addConfig][Error] Type Database Engine($type) is invalid.", 0);

                exit();
            }

            if (!key_exists($type, $this->db_config))
                return null;

            return $this->db_config[$type];
        }
    }
?>