<?php
    // Arquivo player_singleton.inc
    // Criado em 23/05/2020 as 20:21 por Acrisio
    // Definição e Implementação da classe PlayerSingleton

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once("config.inc");

    class PlayerSingleton {
        static private $player = null;

        static protected function updateInstance($uid) {

            if ($uid > 0) {

                $params = DBManagerSingleton::makeNewInstanceParamsArr($GLOBALS['DatabaseCurrentUsed']);  // Tem que usar novo Objeto de params para não da conflito com outros já que esse faz a consulta em qualquer lugar que o código chamar
                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);

                // Pega as informações iniciais da Guild do player
                $params->clear();
                $params->add('i', $uid);

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGachaSystemInfo ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_UID_" as "UID", "_ID_" as "ID", "_NICKNAME_" as "NICKNAME", "_LEVEL_" as "LEVEL", "_capability_" as "capability", "_IDState_" as "IDState", "_TICKET_" as "TICKET", "_TICKET_ID_" as "TICKET_ID", "_TICKET_SUB_" as "TICKET_SUB", "_TICKET_SUB_ID_" as "TICKET_SUB_ID" from '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGachaSystemInfo(?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGachaSystemInfo(?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                    && ($row = $result->fetch_assoc()) != null && isset($row['ID']) && isset($row['UID']) 
                    && isset($row['NICKNAME']) && isset($row['TICKET']) && isset($row['LEVEL']) 
                    && isset($row['TICKET_SUB']) && isset($row['IDState']) && isset($row['capability'])
                    && isset($row['TICKET_ID']) && isset($row['TICKET_SUB_ID'])) {

                    // Success
                    self::$player =   [ 'logged' => true, 
                                        'ID' => $row['ID'], 
                                        'UID' => $row['UID'], 
                                        'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"), 
                                        'LEVEL' => $row['LEVEL'],
                                        'IDState' => $row['IDState'],
                                        'TICKET' => $row['TICKET'],
                                        'TICKET_ID' => $row['TICKET_ID'],
                                        'TICKET_SUB' => $row['TICKET_SUB'],
                                        'TICKET_SUB_ID' => $row['TICKET_SUB_ID']
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

        static public function &getInstance() {

            if (self::$player == null) {

                if (!isset($_SESSION))
                    session_start();
                    
                if (isset($_SESSION) && isset($_SESSION['player']) && $_SESSION['player']['logged'] && isset($_SESSION['player']['UID'])) {
                    
                    $uid = $_SESSION['player']['UID'];

                    if (isset($_SESSION['player']))
                        unset($_SESSION['player']);

                    self::updateInstance($uid);
                    
                    //self::$player = &$_SESSION['player'];

                }else {

                    if (isset($_SESSION['player']))
                        unset($_SESSION['player']);

                    $_SESSION['player'] = ['logged' => false];

                    self::$player = &$_SESSION['player'];

                }
            }

            return self::$player;
        }

        static public function checkPlayerHaveTicketToPlay($modo) {

            $ret = false;

            $p = self::getInstance();

            if ($p != null && $p['logged'] == true) {

                $total = self::getTotalTicketToPlay($p);

                switch ($modo) {
                    case PLAY_MODO::PM_ONE:
                        $ret = ($total >= 1);
                        break;
                    case PLAY_MODO::PM_TEN:
                        $ret = ($total >= 9); // Joga 10, mas gasta 9 ticket
                        break;
                }
            }

            return $ret;
        }

        static public function getTotalTicketToPlay($p) {

            $total = 0;

            if ($p != null && $p['logged'] == true)
                $total = (int)($p['TICKET_SUB'] / 10) + $p['TICKET'];

            return $total;
        }

        static public function getNumTicketByPlayModo($p, $modo) {

            $num = -1;

            if ($p != null && $p['logged'] == true) {

                switch ($modo) {
                    case PLAY_MODO::PM_ONE:
                        $num = 1;
                        break;
                    case PLAY_MODO::PM_TEN:
                        $num = 9;
                        break;
                }
            }

            return $num;
        }
    }

?>