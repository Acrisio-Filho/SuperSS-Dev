<?php
    // Arquivo club_notice.php
    // Criado em 16/07/2019 as 13:59 por Acrisio
    // Definição e Implementação da classe GuildNotice

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildNotice extends GuildHome {

        private $page = 0;
        private $table = null;

        public function show() {

            // Verifica a autoridade do player, 
            // GM tem acesso a essa página
            if (!(PlayerSingleton::getInstance()['CAP'] & 4/*GM*/))
                $this->checkAuthority(AUTH_LEVEL_REAL_MEMBER);

            $this->checkGet();

            $this->listNotice($this->page);

            $this->begin();

            echo '<title>Guild Notice</title>';

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
                            <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_notice.gif" height="23">
                        </td>
                    </tr>
                    <tr>
                        <td colspan="2" align="center">
                            <table cellspacing="1" cellpadding="0" width="580" bgColor="#fcf8f8">
                                <tr bgColor="#c39f8e">
                                    <td height="2" colspan="3"></td>
                                </tr>
                                <tr bgColor="#efdcd4">
                                    <td height="20" width="341" align="center">
                                        <div style="color: #6b4e17">
                                            Title
                                        </div>
                                    </td>
                                    <td height="20" width="120" align="center">
                                        <div style="color: #6b4e17">
                                            Nickname
                                        </div>
                                    </td>
                                    <td height="20" width="119" align="center">
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

                foreach ($this->table->getCurrentRow() as $notice) {

                    echo '      <tr bgColor="#fffdfd">
                                    <td height="25" width="341" align="left">
                                        &nbsp;&nbsp;
                                        <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/club_notice_view.php?page='.$this->page.'&amp;seq='.($notice['SEQ']).'">
                                            '.($notice['TITLE']).'
                                        </a>
                                    </td>
                                    <td height="20" width="120" align="center">
                                        '.(htmlspecialchars(mb_convert_encoding($notice['NICKNAME'], "UTF-8", "SJIS"))).'
                                    </td>
                                    <td height="20" width="119" align="center">
                                        ('.($notice['REG_DATE']).')
                                    </td>
                                </tr>';
                }

            }else {

                echo '<tr bgColor="#fffdfd">
                        <td height="25" align="center">
                            Not Notice
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
                            <a style="display: inline" id="_FirstPage" href="'.BASE_GUILD_HOME_URL.'/club_notice.php?page='.($this->table->getFirst()).'">
                                <img style="display: inline" src="img/bt_pre01.gif" border="0">
                            </a>
                            &nbsp;
                            <a style="display: inline" id="_PrevPage" href="'.BASE_GUILD_HOME_URL.'/club_notice.php?page='.($this->table->getPrev()).'">
                                <img style="display: inline" src="img/bt_pre02.gif" border="0">
                            </a>
                        </td>
                        <td align="center">
                            &nbsp;&nbsp;
                            '.($this->table->makeListLink(BASE_GUILD_HOME_URL.'/club_notice.php', '')).'
                            &nbsp;&nbsp;
                        </td>
                        <td width="50" align="right">
                            <a style="display: inline" id="_NextPage" href="'.BASE_GUILD_HOME_URL.'/club_notice.php?page='.($this->table->getNext()).'">
                                <img style="display: inline" src="img/bt_next01.gif" border="0">
                            </a>
                            &nbsp; 
                            <a style="display: inline" id="_LastPage" href="'.BASE_GUILD_HOME_URL.'/club_notice.php?page='.($this->table->getLast()).'">
                                <img style="display: inline" src="img/bt_next02.gif" border="0">
                            </a>
                        </td>
                    </tr>
                </table>';

                // Só pode escrever noticia se for o master da guild
                if ($this->isMasterOrSubmaster() && !$this->isBlocked() && !$this->isClosure()) {

                    echo '<div align="center">
                            <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/club_notice_write.php">
                                <img border="0" src="'.BASE_GUILD_HOME_URL.'/img/bbs_new_write.gif">
                            </a>
                          </div>';
                }

            // Fim Links Table Location page
            echo '  </td>
                </tr>';

            echo '</table>';
        }

        private function listNotice($page) {

            $notices = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', GuildSingleton::getInstance()['UID']);
            $params->add('i', $page);       // Page

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildNoticeList ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_SEQ_" as "SEQ", "_TITLE_" as "TITLE", "_NICKNAME_" as "NICKNAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildNoticeList(?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildNoticeList(?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS'])) {

                $linhas = $row['LINHAS'];

                do {

                    if (isset($row['SEQ']) && isset($row['TITLE']) && isset($row['NICKNAME']) && isset($row['REG_DATE'])) {

                        $notices[] = [
                            'SEQ' => $row['SEQ'],
                            'TITLE' => $row['TITLE'],
                            'NICKNAME' => $row['NICKNAME'],
                            'REG_DATE' => $row['REG_DATE']
                        ];
                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(12, $page, $linhas, $notices);

        }
    }

    // Guild Notice
    $guild_notice = new GuildNotice();

    $guild_notice->show();
?>