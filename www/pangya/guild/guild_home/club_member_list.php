<?php
    // Arquivo club_member.php
    // Criado em 16/07/2019 as 13:55 por Acrisio
    // Definição e Implementação da classe GuildMember

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildMember extends GuildHome {

        private $page = 0;
        private $table = null;

        public function show() {

            $this->checkGet();

            $this->listMember($this->page);

            $this->begin();

            echo '<title>Guild Member</title>';

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
                            <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_member_list.gif" height="23">
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
                                                                        <td align="center" height="30" width="25%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" align="absMiddle" height="4">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Title</b>
                                                                            </font>
                                                                        </td>
                                                                        <td align="center" height="30" width="45%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" align="absMiddle" height="4">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Nickname</b>
                                                                            </font>
                                                                        </td>
                                                                        <td align="center" height="30" width="30%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" align="absMiddle" height="4">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Joining day</b>
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr bgColor="#e7dcd4">
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                    </tr>';

            // Table Result
            if (!$this->table->isEmpty()) {

                foreach($this->table->getCurrentRow() as $member) {

                    echo '                                          <tr bgColor="#fcf8f8">
                                                                        <td align="center" style="padding: 1px">
                                                                            '.($this->MEMBER_STATE_LABEL[$member['MEMBER_STATE_FLAG']]).'
                                                                        </td>
                                                                        <td style="padding: 1px">
                                                                            &nbsp;&nbsp;'.(htmlspecialchars(mb_convert_encoding($member['NICKNAME'], "UTF-8", "SJIS"))).'
                                                                        </td>
                                                                        <td align="center" style="padding: 1px">
                                                                            ('.($member['REG_DATE']).')
                                                                        </td>
                                                                    </tr>';
                }

                // Linha Final
                echo '                                      <tr bgColor="#e7dcd4">
                                                                <td height="1" style="padding: 1px"></td>
                                                                <td height="1" style="padding: 1px"></td>
                                                                <td height="1" style="padding: 1px"></td>
                                                            </tr>';
            
            }else {

                // Linha Final
                echo '                                      <tr bgColor="#e7dcd4">
                                                                <td height="1" style="padding: 1px"></td>
                                                                <td height="1" style="padding: 1px"></td>
                                                                <td height="1" style="padding: 1px"></td>
                                                            </tr>';

                echo '<tr>
                        <td align="center" style="padding: 1px" colspan="3">
                            No Members
                        </td>
                    </tr>';
            }

            // Table bottom
            echo '                                             </table>
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
                                    <a style="display: inline" id="_FirstPage" href="'.BASE_GUILD_HOME_URL.'/club_member_list.php?page='.($this->table->getFirst()).'">
                                        <img style="display: inline" src="img/bt_pre01.gif" border="0">
                                    </a>
                                    &nbsp;
                                    <a style="display: inline" id="_PrevPage" href="'.BASE_GUILD_HOME_URL.'/club_member_list.php?page='.($this->table->getPrev()).'">
                                        <img style="display: inline" src="img/bt_pre02.gif" border="0">
                                    </a>
                                </td>
                                <td align="center">
                                    &nbsp;&nbsp;
                                    '.($this->table->makeListLink(BASE_GUILD_HOME_URL.'/club_member_list.php', '')).'
                                    &nbsp;&nbsp;
                                </td>
                                <td width="50" align="right">
                                    <a style="display: inline" id="_NextPage" href="'.BASE_GUILD_HOME_URL.'/club_member_list.php?page='.($this->table->getNext()).'">
                                        <img style="display: inline" src="img/bt_next01.gif" border="0">
                                    </a>
                                    &nbsp; 
                                    <a style="display: inline" id="_LastPage" href="'.BASE_GUILD_HOME_URL.'/club_member_list.php?page='.($this->table->getLast()).'">
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

        private function listMember($page) {

            $members = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', GuildSingleton::getInstance()['UID']);
            $params->add('i', $page);       // Page

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList ?, ?, 0, 18';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_MEMBER_UID_" as "MEMBER_UID", "_MEMBER_STATE_FLAG_" as "MEMBER_STATE_FLAG", "_NICKNAME_" as "NICKNAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList(?, ?, 0, 18)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList(?, ?, 0, 18)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS'])) {

                $linhas = $row['LINHAS'];

                do {

                    if (isset($row['MEMBER_UID']) && isset($row['MEMBER_STATE_FLAG']) 
                        && isset($row['NICKNAME']) && isset($row['REG_DATE'])) {

                        // Members line table
                        $members[] = [
                            'MEMBER_UID' => $row['MEMBER_UID'],
                            'MEMBER_STATE_FLAG' => $row['MEMBER_STATE_FLAG'],
                            'NICKNAME' => $row['NICKNAME'],
                            'REG_DATE' => $row['REG_DATE']
                        ];
                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(18, $page, $linhas, $members);

        }
    }

    // Guild Member
    $guild_member = new GuildMember();

    $guild_member->show();
?>