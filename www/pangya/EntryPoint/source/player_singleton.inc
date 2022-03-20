<?php
    // Arquivo player_singleton.inc
    // Criado em 28/06/2020 as 14:31 por Acrisio
    // Definição e Implementação da classe PlayerSingleton

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once($_SERVER['DOCUMENT_ROOT']."/EntryPoint/config/config.inc");

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
                    $query = 'SELECT b.[UID], b.NICK AS NICKNAME, b.ID, b.IDState, a.EMAIL, c.COOKIE FROM '
                        .$db->con_dados['DB_NAME'].'.contas_beta a INNER JOIN '
                        .$db->con_dados['DB_NAME'].'.account b ON a.[UID] = b.[UID] INNER JOIN '
                        .$db->con_dados['DB_NAME'].'.user_info c ON b.[UID] = c.[UID] WHERE b.[UID] = ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'SELECT b.UID AS "UID", b.NICK AS "NICKNAME", b.ID as "ID", b.IDState as "IDState", a.EMAIL AS "EMAIL", c.COOKIE as "COOKIE" FROM '
                        .$db->con_dados['DB_NAME'].'.contas_beta a INNER JOIN '
                        .$db->con_dados['DB_NAME'].'.account b ON a.UID = b.UID INNER JOIN '
                        .$db->con_dados['DB_NAME'].'.user_info c ON b.UID = c.UID WHERE b.UID = ?';
                else
                    $query = 'SELECT b.`UID`, b.NICK AS NICKNAME, b.ID, b.IDState, a.EMAIL, c.COOKIE FROM '
                        .$db->con_dados['DB_NAME'].'.contas_beta a INNER JOIN '
                        .$db->con_dados['DB_NAME'].'.account b ON a.`UID` = b.`UID` INNER JOIN '
                        .$db->con_dados['DB_NAME'].'.user_info c ON b.`UID` = c.`UID` WHERE b.`UID` = ?';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                    && ($row = $result->fetch_assoc()) != null && isset($row['ID']) && isset($row['UID']) 
                    && isset($row['NICKNAME']) && isset($row['IDState']) && isset($row['EMAIL']) && isset($row['COOKIE'])) {

                    // Success
                    self::$player =   [ 'logged' => true, 
                                        'ID' => $row['ID'], 
                                        'UID' => $row['UID'], 
                                        'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"), 
                                        'IDState' => $row['IDState'],
                                        'COOKIE' => $row['COOKIE'],
                                        'EMAIL' => $row['EMAIL'],
                                    ];
                
                }else
                    self::$player = ['logged' => false];

                $_SESSION['player_etp'] = self::$player;

                // Referência da _SESSION
                self::$player = &$_SESSION['player_etp'];
            
            }else {

                $_SESSION['player_etp'] = ['logged' => false];

                // Referência da _SESSION
                self::$player = &$_SESSION['player_etp'];
            }
        }

        static public function &getInstance() {

            if (self::$player == null) {

                if (!isset($_SESSION))
                    session_start();
                    
                if (isset($_SESSION) && isset($_SESSION['player_etp']) && $_SESSION['player_etp']['logged'] && isset($_SESSION['player_etp']['UID'])) {
                    
                    $uid = $_SESSION['player_etp']['UID'];

                    if (isset($_SESSION['player_etp']))
                        unset($_SESSION['player_etp']);

                    self::updateInstance($uid);

                }else {

                    if (isset($_SESSION['player_etp']))
                        unset($_SESSION['player_etp']);

                    $_SESSION['player_etp'] = ['logged' => false];

                    self::$player = &$_SESSION['player_etp'];

                }
            }

            return self::$player;
        }
    }

?>