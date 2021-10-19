<?php
    // Arquivo club_private_bbs_list.php
    // Criado em 16/07/2019 as 14:02 por Acrisio
    // Definição e Implementação da classe GuildPrivateBBSList

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildPrivateBBSList extends GuildHome {

        private $page = 0;
        private $table = null;

        public function show() {

            $this->checkGet();

            $this->listBBS($this->page);

            $this->begin();

            echo '<title>Guild Private BBS List</title>';

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

            echo '<table width="95%" cellspacing="0" cellpadding="0" border="0">
                    <tr>
                        <td height="45" vAlign="middle" colspan="2">
                            <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_member_bbs.gif" height="23">
                        </td>
                    </tr>
                    <tr>
                        <td colspan="2" align="center">
                            <table cellspacing="1" cellpadding="0" width="580" bgColor="#fcf8f8">
                                <tr bgColor="#c39f8e">
                                    <td height="2" colspan="3"></td>
                                </tr>
                                <tr bgColor="#efdcd4">
                                    <td height="20" width="340" align="center">
                                        <div style="color: #6b4e17">
                                            Title
                                        </div>
                                    </td>
                                    <td height="20" width="120" align="center">
                                        <div style="color: #6b4e17">
                                            Nickname
                                        </div>
                                    </td>
                                    <td height="20" width="120" align="center">
                                        <div style="color: #6b4e17">
                                            Date
                                        </div>
                                    </td>
                                </tr>
                                <tr bgColor="#c39f8e">
                                    <td height="2" colspan="3"></td>
                                </tr>
                            </table>
                            <table cellspacing="1" cellpadding="0" width="580" bgColor="#e7dcd4">';

            // Table Result
            if ($this->table != null && !$this->table->isEmpty()) {

                foreach ($this->table->getCurrentRow() as $bbs) {

                    echo '      <tr bgColor="#fffdfd">
                                    <td height="20" width="341" align="left">
                                        &nbsp;&nbsp;
                                        <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/club_private_bbs_view.php?page='.$this->page.'&amp;seq='.($bbs['SEQ']).'">
                                            '.($bbs['TITLE']).'
                                            &nbsp;&nbsp;
                                            <span class="bbsrescount">
                                                '.($bbs['RES_COUNT'] > 0 ? '[Re:'.$bbs['RES_COUNT'].']' : '').'
                                            </span>
                                        </a>
                                    </td>
                                    <td height="20" width="120" align="center">
                                        '.(htmlspecialchars(mb_convert_encoding($bbs['NICKNAME'], "UTF-8", "SJIS"))).'
                                    </td>
                                    <td height="20" width="120" align="center">
                                        ('.($bbs['REG_DATE']).')
                                    </td>
                                </tr>';
                }

            }else {

                echo '<tr bgColor="#fffdfd">
                        <td height="20" align="center">
                            No BBS
                        </td>
                      </tr>';
            }

            echo '          </table>
                            <table cellspacing="1" cellpadding="0" width="580" bgColor="#e7dcd4">
                                <tr bgColor="#f5eadf">
                                    <td height="10" align="right">
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                  </table>';

            // Fim Table Result
            echo '  </td>
                </tr>';

            // Links Table Location page
            echo '<tr>
                    <td align="center">';

            echo '      <table height="40" align="center">
                    <tr>
                        <td width="50" align="left">
                            <a style="display: inline" id="_FirstPage" href="'.BASE_GUILD_HOME_URL.'/club_private_bbs_list.php?page='.($this->table->getFirst()).'">
                                <img style="display: inline" src="img/bt_pre01.gif" border="0">
                            </a>
                            &nbsp;
                            <a style="display: inline" id="_PrevPage" href="'.BASE_GUILD_HOME_URL.'/club_private_bbs_list.php?page='.($this->table->getPrev()).'">
                                <img style="display: inline" src="img/bt_pre02.gif" border="0">
                            </a>
                        </td>
                        <td align="center">
                            &nbsp;&nbsp;
                            '.($this->table->makeListLink(BASE_GUILD_HOME_URL.'/club_private_bbs_list.php', '')).'
                            &nbsp;&nbsp;
                        </td>
                        <td width="50" align="right">
                            <a style="display: inline" id="_NextPage" href="'.BASE_GUILD_HOME_URL.'/club_private_bbs_list.php?page='.($this->table->getNext()).'">
                                <img style="display: inline" src="img/bt_next01.gif" border="0">
                            </a>
                            &nbsp; 
                            <a style="display: inline" id="_LastPage" href="'.BASE_GUILD_HOME_URL.'/club_private_bbs_list.php?page='.($this->table->getLast()).'">
                                <img style="display: inline" src="img/bt_next02.gif" border="0">
                            </a>
                        </td>
                    </tr>
                </table>';

                // Só pode criar BBS guild se for membro
                if ($this->isMember() && !$this->isBlocked() && !$this->isClosure()) {

                    echo '<div align="center">
                            <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/club_private_bbs_new_write.php">
                                <img border="0" src="'.BASE_GUILD_HOME_URL.'/img/bbs_new_write.gif">
                            </a>
                          </div>';
                }

            // Fim Links Table Location page
            echo '  </td>
                </tr>';

            echo '</table>';
        }

        private function listBBS($page) {

            $bbs = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', GuildSingleton::getInstance()['UID']);
            $params->add('i', $page);       // Page

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildPrivateBBSList ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_SEQ_" as "SEQ", "_RES_COUNT_" as "RES_COUNT", "_TITLE_" as "TITLE", "_NICKNAME_" as "NICKNAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildPrivateBBSList(?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildPrivateBBSList(?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS'])) {

                $linhas = $row['LINHAS'];

                do {

                    if (isset($row['SEQ']) && isset($row['RES_COUNT']) && isset($row['TITLE']) 
                        && isset($row['NICKNAME']) && isset($row['REG_DATE'])) {

                        $bbs[] = [
                            'SEQ' => $row['SEQ'],
                            'RES_COUNT' => $row['RES_COUNT'],
                            'TITLE' => $row['TITLE'],
                            'NICKNAME' => $row['NICKNAME'],
                            'REG_DATE' => $row['REG_DATE']
                        ];
                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(15, $page, $linhas, $bbs);

        }
    }

    // Guild Private BBS List
    $bbs_list = new GuildPrivateBBSList();

    $bbs_list->show();
?>