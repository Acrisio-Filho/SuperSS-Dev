<?php
    // Arquivo club_match_result.php
    // Criado em 16/07/2019 as 14:05 por Acrisio
    // Definição e Implementação da classe GuildMatchResult

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildMatchResult extends GuildHome {

        private $page = 0;
        private $table = null;

        public function show() {

            $this->checkGet();

            $this->listMatch($this->page);

            $this->begin();

            echo '<title>Guild Match Result</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkGet() {

            $this->page = (isset($_GET['page']) && is_numeric($_GET['page'])) ? $_GET['page'] : 0;

        }

        private function content() {
            echo '<table width="642" cellspacing="0" cellpadding="0" border="0">';

            // Table Title and Result
            echo '<tr>
                    <td align="center">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="45" vAlign="middle">
                            <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_match_result.gif" height="23">
                        </td>
                    </tr>
                    <tr>
                        <td vAlign="top" align="center">
                            <table cellspacing="0" cellpadding="0" width="580" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" align="center" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="1" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td vAlign="top" align="center">
                                                                <table class="text_normal" cellspacing="1" cellpadding="0" width="100%" border="0">
                                                                    <tr bgColor="#e7dcd4">
                                                                        <td align="center" height="30" style="padding: 1px">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Match</b>
                                                                            </font>
                                                                        </td>
                                                                        <td align="center" height="30" width="130" style="padding: 1px">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Date</b>
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr bgColor="#e7dcd4">
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                    </tr>';

            // Table Result
            if ($this->table != null && !$this->table->isEmpty()) {

                foreach($this->table->getCurrentRow() as $match) {

                    // Index Owner Guild
                    $index_owner = ($match['GUILD_1_UID'] == GuildSingleton::getInstance()['UID'] ? 1 : 2);
                    $index_advesary = ($index_owner == 1 ? 2 : 1);

                    echo '                                          <tr bgColor="#fcf8f8">
                                                                        <td height="24" align="center" style="padding: 1px">
                                                                            <table border="0" cellspacing="0" cellpadding="0" width="100%">
                                                                                <tr>
                                                                                    <td align="right" width="120">
                                                                                        <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/index.php?id='.($match['GUILD_'.$index_owner.'_UID']).'">
                                                                                            '.(htmlspecialchars($match['GUILD_'.$index_owner.'_NAME'])).'
                                                                                        </a>
                                                                                        <img align="absMiddle" style="display: inline" border="0" src="'.BASE_GUILD_UPLOAD_URL.'/mark/'.($match['GUILD_'.$index_owner.'_UID'].'_'.$match['GUILD_'.$index_owner.'_MARK_INDEX']).'.png" width="22" height="20">
                                                                                    </td>
                                                                                    <td align="right" width="90">
                                                                                    ('.(str_pad($match['GUILD_'.$index_owner.'_PANG'], 0, '0', STR_PAD_LEFT)).') '.(str_pad($match['GUILD_'.$index_owner.'_POINT'], 0, '0', STR_PAD_LEFT)).'
                                                                                        &nbsp;
                                                                                    </td>
                                                                                    <td align="center">
                                                                                        x
                                                                                    </td>
                                                                                    <td align="left" width="90">
                                                                                        &nbsp;
                                                                                        '.(str_pad($match['GUILD_'.$index_advesary.'_POINT'], 0, '0', STR_PAD_LEFT)).' ('.(str_pad($match['GUILD_'.$index_advesary.'_PANG'], 0, '0', STR_PAD_LEFT)).')
                                                                                    </td>
                                                                                    <td align="left" width="120">
                                                                                        <img align="absMiddle" style="display: inline" border="0" src="'.BASE_GUILD_UPLOAD_URL.'/mark/'.($match['GUILD_'.$index_advesary.'_UID'].'_'.$match['GUILD_'.$index_advesary.'_MARK_INDEX']).'.png" width="22" height="20">
                                                                                        <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/index.php?id='.($match['GUILD_'.$index_advesary.'_UID']).'">
                                                                                            '.(htmlspecialchars($match['GUILD_'.$index_advesary.'_NAME'])).'
                                                                                        </a>
                                                                                    </td>
                                                                                </tr>
                                                                            </table>
                                                                        </td>
                                                                        <td height="24" align="center" width="130" style="padding: 1px">
                                                                            ('.($match['REG_DATE']).')
                                                                        </td>
                                                                    </tr>';
                }

                // Linha Final
                echo '                                              <tr bgColor="#e7dcd4">
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                    </tr>';
            
            }else {

                // Linha Final
                echo '                                              <tr bgColor="#e7dcd4">
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                    </tr>';

                echo '<tr>
                        <td align="center" style="padding: 1px" colspan="2">
                            Not Matchs
                        </td>
                    </tr>';
            }

            // Table bottom
            echo '                                              </table>
                                                            </td>
                                                        </tr>
                                                    </table>
                                                </td>
                                            </tr>
                                        </table>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                </table>';

            // Fim table result
            echo '  </td>
                </tr>';

            // Links de Localização página table
            echo '<tr>
                    <td align="center">';

            echo '      <table height="40" align="center">
                            <tr>
                                <td width="50" align="left">
                                    <a style="display: inline" id="_FirstPage" href="'.BASE_GUILD_HOME_URL.'/club_match_result.php?page='.($this->table->getFirst()).'">
                                        <img style="display: inline" src="img/bt_pre01.gif" border="0">
                                    </a>
                                    &nbsp;
                                    <a style="display: inline" id="_PrevPage" href="'.BASE_GUILD_HOME_URL.'/club_match_result.php?page='.($this->table->getPrev()).'">
                                        <img style="display: inline" src="img/bt_pre02.gif" border="0">
                                    </a>
                                </td>
                                <td align="center">
                                    &nbsp;&nbsp;
                                    '.($this->table->makeListLink(BASE_GUILD_HOME_URL.'/club_match_result.php', '')).'
                                    &nbsp;&nbsp;
                                </td>
                                <td width="50" align="right">
                                    <a style="display: inline" id="_NextPage" href="'.BASE_GUILD_HOME_URL.'/club_match_result.php?page='.($this->table->getNext()).'">
                                        <img style="display: inline" src="img/bt_next01.gif" border="0">
                                    </a>
                                    &nbsp; 
                                    <a style="display: inline" id="_LastPage" href="'.BASE_GUILD_HOME_URL.'/club_match_result.php?page='.($this->table->getLast()).'">
                                        <img style="display: inline" src="img/bt_next02.gif" border="0">
                                    </a>
                                </td>
                            </tr>
                        </table>';

            // Fim links de Localização página table
            echo '  </td>
                </tr>';

            echo '</table>';
        }

        private function listMatch($page) {

            $members = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', GuildSingleton::getInstance()['UID']);
            $params->add('i', $page);       // Page

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildMatchList ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_GUILD_1_UID_" as "GUILD_1_UID", "_GUILD_1_NAME_" as "GUILD_1_NAME", "_GUILD_1_MARK_INDEX_" as "GUILD_1_MARK_INDEX", "_GUILD_1_POINT_" as "GUILD_1_POINT", "_GUILD_1_PANG_" as "GUILD_1_PANG", "_GUILD_2_UID_" as "GUILD_2_UID", "_GUILD_2_NAME_" as "GUILD_2_NAME", "_GUILD_2_MARK_INDEX_" as "GUILD_2_MARK_INDEX", "_GUILD_2_POINT_" as "GUILD_2_POINT", "_GUILD_2_PANG_" as "GUILD_2_PANG", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildMatchList(?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildMatchList(?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS'])) {

                $linhas = $row['LINHAS'];

                do {

                    if (isset($row['GUILD_1_UID']) && isset($row['GUILD_1_NAME']) && isset($row['GUILD_1_MARK_INDEX']) 
                        && isset($row['GUILD_1_POINT']) && isset($row['GUILD_1_PANG']) && isset($row['GUILD_2_UID']) 
                        && isset($row['GUILD_2_NAME']) && isset($row['GUILD_2_MARK_INDEX']) && isset($row['GUILD_2_POINT']) 
                        && isset($row['GUILD_2_PANG']) && isset($row['REG_DATE'])) {

                        $members[] = [
                            'GUILD_1_UID' => $row['GUILD_1_UID'],
                            'GUILD_1_NAME' => $row['GUILD_1_NAME'],
                            'GUILD_1_MARK_INDEX' => $row['GUILD_1_MARK_INDEX'],
                            'GUILD_1_POINT' => $row['GUILD_1_POINT'],
                            'GUILD_1_PANG' => $row['GUILD_1_PANG'],
                            'GUILD_2_UID' => $row['GUILD_2_UID'],
                            'GUILD_2_NAME' => $row['GUILD_2_NAME'],
                            'GUILD_2_MARK_INDEX' => $row['GUILD_2_MARK_INDEX'],
                            'GUILD_2_POINT' => $row['GUILD_2_POINT'],
                            'GUILD_2_PANG' => $row['GUILD_2_PANG'],
                            'REG_DATE' => $row['REG_DATE']
                        ];
                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(13, $page, $linhas, $members);
        }
    }

    // Guild Match Result
    $guild_match = new GuildMatchResult();

    $guild_match->show();
?>