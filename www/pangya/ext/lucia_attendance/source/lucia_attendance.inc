<?php
    // Arquivo lucia_attendance.inc
    // Criado em 28/02/2020 as 05:38 por Acrisio
    // Definição e Implementação da class LuciaAttendance

    include_once('design.inc');
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once('debug_log.inc');

    abstract class LuciaAttendance {

        private const LIMIT_TRY_HACKING = 3;
        protected const BLOCKED_BY_ADM_FLAG = 0x1000; // 4096
        protected $blocked = false;

        public function __construct() {

            Design::checkIE();
            
            $this->checkBlock();
        }

        abstract public function show();

        protected function begin() {

            Design::beginHTML();

            Design::beginHead();
        }

        protected function middle($onload = false) {

            Design::endHead();

            Design::beginBody($onload);

            Design::beginContent();

        }

        protected function end() {

            Design::endContent();

            Design::endBody();

            Design::endHTML();

        }

        protected function isEnableDay() {
            
            $p = PlayerSingleton::getInstance();

            if ($p['logged']) {

                if ($p['LAST_DAY_ATTENDANCE'] == null)
                    return true;

                $date = new DateTime($p['LAST_DAY_ATTENDANCE']);
                $date = DateTime::createFromFormat("!Y-m-d", $date->format("Y-m-d")); // Pega apenas a Date
                $diff = (new DateTime(date("Y-m-d")))->diff($date);

                if ($diff->invert == 1/*Negativo*/ && $diff->days >= 1)
                    return true;
            }

            return false;
        }

        protected function isGettedItem() {

            $p = PlayerSingleton::getInstance();

            if ($p['logged']) {

                if ($p['LAST_DAY_GET_ITEM'] == null || $p['COUNT_DAY'] != 0)
                    return false;

                $date = new DateTime($p['LAST_DAY_GET_ITEM']);
                $date = DateTime::createFromFormat("!Y-m-d", $date->format("Y-m-d"));   // Pega apenas a data
                $diff = (new DateTime(date("Y-m-d")))->diff($date);

                // Ainda está no dia que ele pegou o item
                if ($diff->invert == 0/*Positivo*/ && $diff->days == 0)
                    return true;
            }

            return false;
        }

        protected function checkBlock() {

            $p = PlayerSingleton::getInstance();

            if ($p['logged']) {

                // Verifica block de tentativa de hacking
                switch ($p['BLOCK_TYPE']) {
                    case 1: // 1 Dia Block
                    case 2: // 1 Semana Block
                    case 3: // 1 Mês Block
                    {

                        // Block por tempo, Verifica se o tempo já passou
                        if ($p['BLOCK_END_DATE'] != null) {

                            $date = new DateTime($p['BLOCK_END_DATE']);
                            $diff = (new DateTime())->diff($date);
                            
                            if ($diff->invert == 0/*Positivo ainda está bloqueado*/)
                                $this->blocked = true;

                        }
                        break;
                    }
                    case 4: // Permanente
                        $this->blocked = true;
                        break;
                }

                // Verifica block do adm
                if ($p['IDState'] & LuciaAttendance::BLOCKED_BY_ADM_FLAG)
                    $this->blocked = true;
                
            }else
                $this->blocked = true;
        }

        protected function addCountDay() {

            $p = PlayerSingleton::getInstance();

            if ($p['logged']) {

                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                $params = $db->params;

                if (++$p['COUNT_DAY'] <= 7) {

                    $params->clear();
                    $params->add('i', $p['UID']);
                    $params->add('i', $p['COUNT_DAY']);

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcUpdateCountDayLuciaAttendance ?, ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcUpdateCountDayLuciaAttendance(?, ?)';
                    else
                        $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateCountDayLuciaAttendance(?, ?)';

                    if ($db->db->execPreparedStmt($query, $params->get()) == null || $db->db->getLastError())
                        DebugLog::Log("[LuciaAttendance][addCountDay] nao conseguiu atualiza o count day. DB ERROR: ".$db->db->getLastError());

                    // Log
                    DebugLog::Log("[LuciaAttendance][addCountDay] marcou presença com sucesso.");

                }else {

                    DebugLog("[LuciaAttendance][addCountDay] Count day é maior que 7 dias. Value: ".$p['COUNT_DAY']);

                }

                // Update
                PlayerSingleton::updateInstance();
            }
        }

        protected function addTryHacking() {

            $p = PlayerSingleton::getInstance();

            if ($p['logged']) {

                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                $params = $db->params;

                if (++$p['TRY_HACKING_COUNT'] >= LuciaAttendance::LIMIT_TRY_HACKING) {

                    // Bloqueia o player
                    $p['BLOCK_TYPE']++; // soma mais uma a flag de block, quando chegar no 4 ele é bloqueado permanente

                    $params->clear();
                    $params->add('i', $p['UID']);
                    $params->add('i', $p['BLOCK_TYPE']);

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcBlockPlayerLuciaAttendance ?, ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcBlockPlayerLuciaAttendance(?, ?)';
                    else
                        $query = 'call '.$db->con_dados['DB_NAME'].'.ProcBlockPlayerLuciaAttendance(?, ?)';

                    if ($db->db->execPreparedStmt($query, $params->get()) == null || $db->db->getLastError() != 0)
                        DebugLog::Log("[LuciaAttendance][addTryHacking] nao conseguiu bloquear o player. DB ERROR: ".$db->db->getLastError());

                }else {

                    // Update no banco de dados
                    $params->clear();
                    $params->add('i', $p['UID']);
                    $params->add('i', $p['TRY_HACKING_COUNT']);

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcUpdateTryHackingPlayerLuciaAttendance ?, ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcUpdateTryHackingPlayerLuciaAttendance(?, ?)';
                    else
                        $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateTryHackingPlayerLuciaAttendance(?, ?)';

                    if ($db->db->execPreparedStmt($query, $params->get()) == null || $db->db->getLastError() != 0)
                        DebugLog::Log("[LuciaAttendance][addTryHacking] nao conseguiu atualizar o try_hacking_count. DB ERROR: ".$db->db->getLastError());
                }

                // Update
                PlayerSingleton::updateInstance();
            }
        }
    }
?>