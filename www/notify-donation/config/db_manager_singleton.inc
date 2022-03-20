<?php
    // Arquivo db_manager_singleton.inc
    // Criado em 03/08/2021 as 14:14 por Acrisio
    // Definição da classe DBManagerSingleton

    include_once('config.inc');
    
    // Dbs
    include_once('MysqlManager.inc');
    include_once('mssql_odbc_manager.inc');
    include_once('postgresql_odbc_manager.inc');

    class DBManagerContext {
        public $db = null;
        public $params = null;
        public $con_dados = null;

        public function __construct($db = null, $params = null, $con_dados = null) {
            $this->db = $db;
            $this->params = $params;
            $this->con_dados = $con_dados;
        }
    }
    
    class DBManagerSingleton {

        private static $db_ctx = [];

        public static function getInstanceDB($type) {

            if (!DatabaseConfig::isKnownType($type)) {

                error_log("[DBManagerSingleton::getInstanceDB][Error] Type Database Engine($type) is invalid.", 0);

                exit();
            }

            if (!key_exists($type, self::$db_ctx)) {
               
                switch ($type) {
                    case DatabaseConfig::_MYSQL_:
                        self::$db_ctx[$type] = DBManagerSingleton::makeMySQLDatabaseContext();
                        break;
                    case DatabaseConfig::_MSSQL_:
                        self::$db_ctx[$type] = DBManagerSingleton::makeMSSQLDatabaseContext();
                        break;
                    case DatabaseConfig::_PSQL_:
                        self::$db_ctx[$type] = DBManagerSingleton::makePSQLDatabaseContext();
                        break;
                }
            }

            return self::$db_ctx[$type];
        }

        public static function makeNewInstanceParamsArr($type) {
            
            if (!DatabaseConfig::isKnownType($type)) {

                error_log("[DBManagerSingleton::makeNewInstanceParamsArr][Error] Type Database Engine($type) is invalid.", 0);

                exit();
            }
               
            switch ($type) {
                case DatabaseConfig::_MYSQL_:
                    return new paramsArr(0);
                case DatabaseConfig::_MSSQL_:
                    return new paramsArr(1);
                case DatabaseConfig::_PSQL_:
                    return new paramsArr(1);
            }
        }

        private static function makeMySQLDatabaseContext() {
            global $DatabaseConfigInstance;

            $ctx = new DBManagerContext();

            $ctx->con_dados = $DatabaseConfigInstance->getConfig(DatabaseConfig::_MYSQL_);

            if ($ctx->con_dados == null) {

                error_log("[DBManagerSingleton][Error] con_dados of __MYSQL__ not found.", 0);

                exit();
            }

            $ctx->params = new paramsArr(0);
            $ctx->db = new MysqlManager($ctx->con_dados);

            return $ctx;
        }

        private static function makeMSSQLDatabaseContext() {
            global $DatabaseConfigInstance;

            $ctx = new DBManagerContext();

            $ctx->con_dados = $DatabaseConfigInstance->getConfig(DatabaseConfig::_MSSQL_);

            if ($ctx->con_dados == null) {

                error_log("[DBManagerSingleton][Error] con_dados of __MSSQL__ not found.", 0);

                exit();
            }

            $ctx->params = new paramsArr(1);
            $ctx->db = new MSSqlODBCManager($ctx->con_dados);

            return $ctx;
        }

        private static function makePSQLDatabaseContext() {
            global $DatabaseConfigInstance;

            $ctx = new DBManagerContext();

            $ctx->con_dados = $DatabaseConfigInstance->getConfig(DatabaseConfig::_PSQL_);

            if ($ctx->con_dados == null) {

                error_log("[DBManagerSingleton][Error] con_dados of __PSQL__ not found.", 0);

                exit();
            }

            $ctx->params = new paramsArr(1);
            $ctx->db = new PostgreSQLODBCManager($ctx->con_dados);

            return $ctx;
        }
    }
?>