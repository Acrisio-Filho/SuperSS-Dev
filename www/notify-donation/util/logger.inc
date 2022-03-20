<?php
    // Arquivo de logger.inc
    // Criado em 26/03/2020 as 17:49 por Acrisio
    // Definição da classe Logger

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/config.inc');

    define('LOG_PATH', $_SERVER['DOCUMENT_ROOT'].'/log/log.log');
    define('BASE_LOG', "[Notification]");

    class Logger {
        private $path = LOG_PATH;

        public function putLog($_log, $_type) {

            if (!isset($_log) || empty($_log))
                return 1;

            if (empty($_type))
                $_type = "UNKNOWN";

            file_put_contents($this->path, $this->nowDate().' ['.$this->getIP().'] '.BASE_LOG.eTYPE_PLATAFORM_LOG[$_type].$_log.PHP_EOL, FILE_APPEND);
        }

        private function nowDate() {
            
            $tmp = '0000-00-00 00:00:00';

            $now = new DateTime();

            $tmp = $now->format("Y-m-d H:i:s");

            return $tmp;
        }

        private function getIP() {

            $ip = ':::?';

            $ip = getenv("REMOTE_ADDR") ?? $ip;

            return $ip;
        }
    }

    class sLogger {

        static private $log = null;

        static public function &getInstance() {

            if (self::$log == null)
                self::$log = new Logger();

            return self::$log;
        }
    }
?>