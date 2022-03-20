<?php
    // Arquivo player_singleton.inc
    // Criado em 16/07/2019 as 06:53 por Acrisio
    // Definição e Implementação da classe PlayerSingleton

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    class PlayerSingleton {
        static private $player = null;

        static protected function _updateInstance($uid) {

            if ($uid > 0) {

                $params = DBManagerSingleton::makeNewInstanceParamsArr($GLOBALS['DatabaseCurrentUsed']);  // Tem que usar novo Objeto de params para não da conflito com outros já que esse faz a consulta em qualquer lugar que o código chamar
                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);

                // Pega as informações iniciais da Lucia Attendance do player
                $params->clear();
                $params->add('i', $uid);

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetPlayerLuciaAttendanceInfo ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_UID_" as "UID", "_ID_" as "ID", "_NICKNAME_" as "NICKNAME", "_IDState_" as "IDState", "_count_day_" as "count_day", "_last_day_attendance_" as "last_day_attendance", "_last_day_get_item_" as "last_day_get_item", "_try_hacking_count_" as "try_hacking_count", "_block_type_" as "block_type", "_block_end_date_" as "block_end_date" from '.$db->con_dados['DB_NAME'].'.ProcGetPlayerLuciaAttendanceInfo(?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetPlayerLuciaAttendanceInfo(?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                    && ($row = $result->fetch_assoc()) != null && isset($row['ID']) && isset($row['UID']) && isset($row['NICKNAME']) 
                    && isset($row['IDState']) && isset($row['count_day']) && key_exists('last_day_attendance', $row) 
                    && isset($row['try_hacking_count']) && isset($row['block_type']) && key_exists('block_end_date', $row)
                    && key_exists('last_day_get_item', $row)) {

                    self::$player = [   'logged' => true, 
                                        'ID' => $row['ID'], 
                                        'UID' => $row['UID'], 
                                        'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"),
                                        'IDState' => $row['IDState'],
                                        'COUNT_DAY' => $row['count_day'],
                                        'LAST_DAY_ATTENDANCE' => $row['last_day_attendance'],
                                        'LAST_DAY_GET_ITEM' => $row['last_day_get_item'],
                                        'TRY_HACKING_COUNT' => $row['try_hacking_count'],
                                        'BLOCK_TYPE' => $row['block_type'],
                                        'BLOCK_END_DATE' => $row['block_end_date']
                                    ];
                
                }else
                    self::$player = ['logged' => false];

                $_SESSION['player'] = self::$player;

                // Referência da _SESSION
                self::$player = &$_SESSION['player'];
            
            }else {

                $_SESSION['player'] = ['logged' => false];

                // Referência da _SESSION
                self::$player = &$_SESSION['player'];
            }
        }

        static public function updateInstance() {

            if (!isset($_SESSION))
                session_start();

            if (isset($_SESSION['player']))
                self::_updateInstance($_SESSION['player']['UID']);
        }

        static public function &getInstance() {

            if (self::$player == null) {

                if (!isset($_SESSION))
                    session_start();

                if (isset($_SESSION) && isset($_SESSION['player']) && $_SESSION['player']['logged'] && isset($_SESSION['player']['UID'])) {

                    $uid = $_SESSION['player']['UID'];

                    if (isset($_SESSION['player']))
                        unset($_SESSION['player']);

                    self::_updateInstance($uid);

                }else {

                    if (isset($_SESSION['player']))
                        unset($_SESSION['player']);

                    $_SESSION['player'] = ['logged' => false];

                    self::$player = &$_SESSION['player'];

                }
            }

            return self::$player;
        }
    }

?>