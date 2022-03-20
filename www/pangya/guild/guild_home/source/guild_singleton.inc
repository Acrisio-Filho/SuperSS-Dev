<?php
    // Arquivo guild_singleton.inc
    // Criado em 17/07/2019 as 13:46 por Acrisio
    // Definição e Implementação da classe GuildSingleton

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    class GuildSingleton {

        private static $guild = null;
        private static $guild_info = null;

        private static function updateGuildInstance($uid) {

            if ($uid > 0) {
                
                $params = DBManagerSingleton::makeNewInstanceParamsArr($GLOBALS['DatabaseCurrentUsed']);  // Tem que usar novo Objeto de params para não da conflito com outros já que esse faz a consulta em qualquer lugar que o código chamar
                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);

                $params->clear();
                $params->add('i', $uid);

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildInfoNew ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_GUILD_UID_" as "GUILD_UID", "_GUILD_NAME_" as "GUILD_NAME", "_GUILD_MASTER_" as "GUILD_MASTER", "_GUILD_MASTER_NICKNAME_" as "GUILD_MASTER_NICKNAME", "_GUILD_CONDITION_LEVEL_" as "GUILD_CONDITION_LEVEL", "_GUILD_STATE_" as "GUILD_STATE", "_GUILD_FLAG_" as "GUILD_FLAG", "_GUILD_PERMITION_JOIN_" as "GUILD_PERMITION_JOIN", "_GUILD_MEMBERS_" as "GUILD_MEMBERS", "_GUILD_REG_DATE_" as "GUILD_REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildInfoNew(?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildInfoNew(?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null 
                    && $db->db->getLastError() == 0 && ($row = $result->fetch_assoc()) != null
                    && isset($row['GUILD_UID']) && isset($row['GUILD_NAME']) && isset($row['GUILD_MASTER_NICKNAME'])
                    && isset($row['GUILD_MASTER']) && isset($row['GUILD_CONDITION_LEVEL']) && isset($row['GUILD_STATE']) 
                    && isset($row['GUILD_PERMITION_JOIN']) && isset($row['GUILD_MEMBERS']) && isset($row['GUILD_REG_DATE'])
                    && isset($row['GUILD_FLAG'])) {
                    
                    self::$guild =  [   'exists' => true,
                                        'UID' => $row['GUILD_UID'], 
                                        'NAME' => htmlspecialchars(mb_convert_encoding($row['GUILD_NAME'], "UTF-8", "SJIS")),
                                        'MASTER' => [   'NICKNAME' => mb_convert_encoding($row['GUILD_MASTER_NICKNAME'], "UTF-8", "SJIS"),
                                                        'UID' => $row['GUILD_MASTER']
                                                    ],
                                        'CONDITION_LEVEL' => $row['GUILD_CONDITION_LEVEL'],
                                        'STATE' => $row['GUILD_STATE'],
                                        'FLAG' => $row['GUILD_FLAG'],
                                        'PERMITION_JOIN' => $row['GUILD_PERMITION_JOIN'],
                                        'MEMBER_NUM' => $row['GUILD_MEMBERS'],
                                        'DATE' => $row['GUILD_REG_DATE']
                                    ];

                }else
                    self::$guild = [ 'exists' => false ];

                $_SESSION['GUILD'] = self::$guild;

                // Referença da session
                self::$guild = &$_SESSION['GUILD'];
            
            }else {

                $_SESSION['GUILD'] = ['exists' => false];

                // Referença da session
                self::$guild_info = &$_SESSION['GUILD'];
            }
        }

        private static function updateGuildFullInfoInstance($uid) {

            if ($uid > 0) {

                $params = DBManagerSingleton::makeNewInstanceParamsArr($GLOBALS['DatabaseCurrentUsed']);  // Tem que usar novo Objeto de params para não da conflito com outros já que esse faz a consulta em qualquer lugar que o código chamar
                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);

                $params->clear();
                $params->add('i', $uid);

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildInfoComplete ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_GUILD_UID_" as "GUILD_UID", "_GUILD_NAME_" as "GUILD_NAME", "_GUILD_MASTER_" as "GUILD_MASTER", "_GUILD_SUB_MASTER_" as "GUILD_SUB_MASTER", "_GUILD_MEMBERS_" as "GUILD_MEMBERS", "_GUILD_STATE_" as "GUILD_STATE", "_GUILD_FLAG_" as "GUILD_FLAG", "_GUILD_CONDITION_LEVEL_" as "GUILD_CONDITION_LEVEL", "_GUILD_PERMITION_JOIN_" as "GUILD_PERMITION_JOIN", "_GUILD_REG_DATE_" as "GUILD_REG_DATE", "_GUILD_INFO_" as "GUILD_INFO", "_GUILD_NOTICE_" as "GUILD_NOTICE", "_GUILD_MARK_IMG_IDX_" as "GUILD_MARK_IMG_IDX", "_GUILD_NEW_MARK_IDX_" as "GUILD_NEW_MARK_IDX", "_GUILD_INTRO_IMG_" as "GUILD_INTRO_IMG", "_GUILD_CLOSURE_DATE_" as "GUILD_CLOSURE_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildInfoComplete(?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildInfoComplete(?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                    && ($row = $result->fetch_assoc()) != null && isset($row['GUILD_UID'])
                    && isset($row['GUILD_NAME']) && isset($row['GUILD_MASTER']) && isset($row['GUILD_SUB_MASTER'])
                    && isset($row['GUILD_MEMBERS']) && isset($row['GUILD_STATE']) && isset($row['GUILD_CONDITION_LEVEL']) 
                    && isset($row['GUILD_REG_DATE']) && isset($row['GUILD_INFO']) && isset($row['GUILD_NOTICE']) 
                    && isset($row['GUILD_MARK_IMG_IDX']) && key_exists('GUILD_INTRO_IMG', $row) && isset($row['GUILD_NEW_MARK_IDX']) 
                    && isset($row['GUILD_PERMITION_JOIN']) && isset($row['GUILD_FLAG']) && key_exists('GUILD_CLOSURE_DATE', $row)) {

                        self::$guild_info = [   'exists' => true,
                                                'UID' => $row['GUILD_UID'], 
                                                'NAME' => htmlspecialchars(mb_convert_encoding($row['GUILD_NAME'], "UTF-8", "SJIS")), 
                                                'MASTER' => mb_convert_encoding($row['GUILD_MASTER'], "UTF-8", "SJIS"), 
                                                'SUB_MASTER' => mb_convert_encoding($row['GUILD_SUB_MASTER'], "UTF-8", "SJIS"), 
                                                'MEMBERS' => $row['GUILD_MEMBERS'], 
                                                'STATE' => $row['GUILD_STATE'], 
                                                'FLAG' => $row['GUILD_FLAG'],
                                                'CONDITION_LEVEL' => $row['GUILD_CONDITION_LEVEL'], 
                                                'CREATE' => $row['GUILD_REG_DATE'], 
                                                'MESSAGE_INTRO' => $row['GUILD_INFO'], 
                                                'MASTER_COMMENT' => $row['GUILD_NOTICE'],
                                                'MARK_IDX' => $row['GUILD_MARK_IMG_IDX'],
                                                'INTRO_IMG' => $row['GUILD_INTRO_IMG'],
                                                'NEW_MARK_IDX' => $row['GUILD_NEW_MARK_IDX'],
                                                'PERMITION_JOIN' => $row['GUILD_PERMITION_JOIN'],
                                                'CLOSURE_DATE' => $row['GUILD_CLOSURE_DATE']
                                            ];   
                }else
                    self::$guild_info = ['exists' => false];

                $_SESSION['GUILD_INFO'] = self::$guild_info;

                // Referença da session
                self::$guild_info = &$_SESSION['GUILD_INFO'];
            
            }else {

                $_SESSION['GUILD_INFO'] = ['exists' => false];

                // Referença da session
                self::$guild_info = &$_SESSION['GUILD_INFO'];
            }
        }

        public static function updateAllInstance() {

            // Update GUILD
            if (isset($_SESSION['GUILD']))
                self::updateGuildInstance($_SESSION['GUILD']['UID']);

            // Update GUILD FULL INFO
            if (isset($_SESSION['GUILD_INFO']))
                self::updateGuildFullInfoInstance($_SESSION['GUILD_INFO']['UID']);

        }

        public static function &getInstance() {

            //print_r($_SESSION);

            if (self::$guild == null) {

                if (!isset($_SESSION))
                    session_start();

                /* if ((isset($_GET) && isset($_GET['id'])) && (!isset($_SESSION['GUILD']) || !$_SESSION['GUILD']['exists'] || $_SESSION['GUILD']['UID'] != $_GET['id'])) {

                    if (isset($_SESSION['GUILD']))
                        unset($_SESSION['GUILD']);

                    // Update
                    self::updateGuildInstance($_GET['id']);
                
                }else if (isset($_SESSION['GUILD']))
                    self::$guild = &$_SESSION['GUILD']; */

                if (isset($_GET) && isset($_GET['id'])) {

                    if (isset($_SESSION['GUILD']))
                        unset($_SESSION['GUILD']);

                    // Update
                    self::updateGuildInstance($_GET['id']);
                
                }else if (isset($_SESSION['GUILD']) && $_SESSION['GUILD']['exists'] && isset($_SESSION['GUILD']['UID'])) {

                    $id = $_SESSION['GUILD']['UID'];

                    if (isset($_SESSION['GUILD']))
                        unset($_SESSION['GUILD']);

                    // Update
                    self::updateGuildInstance($id);

                }else if (isset($_SESSION['GUILD']))
                    self::$guild = &$_SESSION['GUILD'];

            }

            return self::$guild;
        }

        // Retorna a instância do guild info full
        public static function &getInstanceFullInfo() {

            if (self::$guild_info == null) {

                if (!isset($_SESSION))
                    session_start();

                /* if ((isset($_GET) && isset($_GET['id'])) && (!isset($_SESSION['GUILD_INFO']) || !$_SESSION['GUILD_INFO']['exists'] || $_SESSION['GUILD_INFO']['UID'] != $_GET['id'])) {
                    
                    if (isset($_SESSION['GUILD_INFO']))
                        unset($_SESSION['GUILD_INFO']);

                    // Update
                    self::updateGuildFullInfoInstance($_GET['id']);
                
                }else if (isset($_SESSION['GUILD_INFO']))
                    self::$guild_info = &$_SESSION['GUILD_INFO']; */

                if (isset($_GET) && isset($_GET['id'])) {
                
                    if (isset($_SESSION['GUILD_INFO']))
                        unset($_SESSION['GUILD_INFO']);

                    // Update
                    self::updateGuildFullInfoInstance($_GET['id']);
                
                }else if (isset($_SESSION['GUILD_INFO']) && $_SESSION['GUILD_INFO']['exists'] && isset($_SESSION['GUILD_INFO']['UID'])) {

                    $id = $_SESSION['GUILD_INFO']['UID'];

                    if (isset($_SESSION['GUILD_INFO']))
                        unset($_SESSION['GUILD_INFO']);

                    // Update
                    self::updateGuildFullInfoInstance($id);

                }else if (isset($_SESSION['GUILD_INFO']))
                    self::$guild_info = &$_SESSION['GUILD_INFO'];
            
            }   

            return self::$guild_info;
        }
    }
?>