<?php
    // Arquivo debug_log.inc
    // Criado em 28/02/2020 as 05:54 por Acrisio
    // Definição da classe Debug Log

    include_once('config.inc');
    include_once('player_singleton.inc');

    class DebugLog {

        public static function Log($log) {

            if (isset($log) && $log && !empty($log)) {

                $ip_session = '[IP='.(!empty(getenv("REMOTE_ADDR"))  ? getenv("REMOTE_ADDR") : 'unknwon').'] ';

                $p = PlayerSingleton::getInstance();

                if ($p != null && $p['logged'])
                    $ip_session .= '[UID='.$p['UID'].'] ';

                if (DEBUG_ERROR == DEBUG_TYPE::DT_SHOW_ALL_ERROR || DEBUG_ERROR == DEBUG_TYPE::DT_SHOW_ERROR)
                    echo '[Gacha System][Log] '.$ip_session.$log;

                if (DEBUG_ERROR == DEBUG_TYPE::DT_SHOW_ALL_ERROR || DEBUG_ERROR == DEBUG_TYPE::DT_ONLY_FILE_ERROR)
                    error_log('[Gacha System][Log] '.$ip_session.$log, 0);
            }
        }
    }
?>