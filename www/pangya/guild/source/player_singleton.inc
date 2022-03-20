<?php
    // Arquivo player_singleton.inc
    // Criado em 16/07/2019 as 06:53 por Acrisio
    // Definição e Implementação da classe PlayerSingleton

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

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
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildInfo ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_ID_" as "ID", "_UID_" as "UID", "_NICKNAME_" as "NICKNAME", "_LEVEL_" as "LEVEL", "_capability_" as "capability", "_IDState_" as "IDState", "_GUILD_UID_" as "GUILD_UID", "_MEMBER_STATE_FLAG_" as "MEMBER_STATE_FLAG" from '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildInfo(?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildInfo(?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                    && ($row = $result->fetch_assoc()) != null && isset($row['ID']) && isset($row['UID']) 
                    && isset($row['NICKNAME']) && isset($row['GUILD_UID']) && isset($row['LEVEL']) 
                    && isset($row['MEMBER_STATE_FLAG']) && isset($row['IDState']) && isset($row['capability'])) {

                    // Success
                    self::$player =         [ 'logged' => true, 
                                                'ID' => $row['ID'], 
                                                'UID' => $row['UID'], 
                                                'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"), 
                                                'LEVEL' => $row['LEVEL'],
                                                'CAP' => $row['capability'],
                                                'IDState' => $row['IDState'],
                                                'GUILD_UID' => ($row['GUILD_UID'] == 0) ? -1 : $row['GUILD_UID'],
                                                'MEMBER_STATE_FLAG' => $row['MEMBER_STATE_FLAG']
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
    }

?>