<?php
    // Arquivo make_guild.inc
    // Criado em 24/07/2019 as 21:19
    // Definição e Implementação da classe MakeGuild

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/base.inc');

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');
    
    include_once("../source/player_singleton.inc");

    class MakeGuild {

        public static function make($guild) {

            // Protected Text from tag HTML
            $guild['name'] = strip_tags($guild['name']);
            $guild['intro'] = strip_tags($guild['intro']);
            $guild['master_comment'] = strip_tags($guild['master_comment']);

            $ret = -1;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcMakeGuild ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_GUILD_UID_" as "GUILD_UID", "_GUILD_NEW_MARK_IDX_" as "GUILD_NEW_MARK_IDX" from '.$db->con_dados['DB_NAME'].'.ProcMakeGuild(?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcMakeGuild(?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('s', $guild['name']);
            $params->add('i', $guild['master']);
            $params->add('i', $guild['lvl_cond']);  // Level Condição tem que colocar depois isso
            $params->add('s', $guild['intro']);
            $params->add('s', $guild['master_comment']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                if (($row = $result->fetch_assoc()) != null && isset($row['GUILD_UID']) && $row['GUILD_UID'] != '') {

                    if ($row['GUILD_UID'] > 0) {
                        
                        if (isset($row['GUILD_NEW_MARK_IDX']) && $row['GUILD_NEW_MARK_IDX'] > 0) {

                            // Update GUILD_UID do player
                            $_SESSION['player']['GUILD_UID'] = $row['GUILD_UID'];
                            $_SESSION['player']['MEMBER_STATE_FLAG'] = 1;   // Master

                            // Make Mark Name
                            $mark_name = checkFileName($row['GUILD_UID'].'_'.$row['GUILD_NEW_MARK_IDX']).'.png';

                            // Copy preview para o mark oficial
                            if (rename($_SERVER['DOCUMENT_ROOT'].'/_Files/guild/mark/preview/'.$guild['mark'], $_SERVER['DOCUMENT_ROOT'].'/_Files/guild/mark/'.$mark_name))
                                $ret = 1;
                        }
                    
                    }else
                        $ret = $row['GUILD_UID'];   // Erro que deu
                }
            }

            return $ret;
        }
    }
?>