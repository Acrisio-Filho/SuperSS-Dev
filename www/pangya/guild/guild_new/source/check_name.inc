<?php
    // Arquivo check_name.inc
    // Criado em 24/07/2019 as 19:06 por Acrisio
    // Definição e Implementação da classe ValidName

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    class ValidName {

        public static function checkName($name) {

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcCheckGuildName ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcCheckGuildName(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcCheckGuildName(?)';

            $params->clear();
            $params->add('s', $name);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                // Guild Name disponível
                if ($result->num_rows == 0)
                    return true;
            }

            return false;
        }
    }
?>