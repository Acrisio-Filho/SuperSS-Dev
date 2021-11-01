<?php
    // Arquivo club_prejoin_agree.php
    // Criado em 16/07/2019 as 14:16 por Acrisio
    // Definição e Implementação da classe GuildPreJoin

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    class GuildPreJoin extends GuildHome {

        private $msg = null;

        public function show() {

            $this->checkPost();

            $this->begin();

            echo '<title>Guild Pre-Join</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            $check = (isset($_POST['checkbox']) ? $_POST['checkbox'] : false);

            if ($check) {

                // Verifica se o player pode entrar na guild
                if (($this->msg = $this->requestJoinGuild()) == null) {
                    
                    // Update SESSION player
                    $_SESSION['player']['GUILD_UID'] = GuildSingleton::getInstance()['UID'];
                    $_SESSION['player']['MEMBER_STATE_FLAG'] = 9;   // Aguardando o master aceitar o pedido de ingressar na guild

                    // Update Guild Singleton
                    GuildSingleton::updateAllInstance();

                    $this->msg = 'A request has been made is waiting for the club master to approve.';
                }
            }
        }

        private function requestJoinGuild() {

            $msg = null;

            if ($this->isBlocked() || $this->isClosure())
                return 'This club is '.($this->isBlocked() ? ' blocked.' : ' in the process of closing.');

            if (!GuildSingleton::getInstance()['PERMITION_JOIN'] || GuildSingleton::getInstance()['STATE'] == ST_BLOCKED_JOIN)
                return 'This club is closed to new members.';

            if (GuildSingleton::getInstance()['CONDITION_LEVEL'] > PlayerSingleton::getInstance()['LEVEL'])
                return 'You do not have the level required to apply to join this club.';

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);
            $params->add('i', GuildSingleton::getInstance()['UID']);

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcRequestJoinGuild ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_RET_" as "RET" from '.$db->con_dados['DB_NAME'].'.ProcRequestJoinGuild(?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcRequestJoinGuild(?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null || $db->db->getLastError() != 0
                || ($row = $result->fetch_assoc()) == null || !isset($row['RET']) || $row['RET'] == -1)
                $msg = 'Failed to apply to join club.';
            else if ($row['RET'] == -2)
                $msg = 'You have already applied for this club.';
            else if ($row['RET'] == -3)
                $msg = 'The club is closed or locked.';
            else if ($row['RET'] == -4)
                $msg = 'You are already a member of another club, sign out and then apply for that club.';

            return $msg;
        }

        private function Agreement() {

            echo '<form id="ctl00" method="POST" action="./club_prejoin_agree.php">';

            echo '<table width="642" cellspacing="0" cellpadding="0" border="0">';

            // Table Title and Result
            echo '<tr>
                    <td align="center" style="padding: 20px">';

            echo '<table cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td colspan="2" vAlign="top" align="center">
                            <table cellspacing="0" cellpadding="0" width="570" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" align="center" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="1" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td width="556" vAlign="top" align="center">
                                                                <table cellspacing="1" cellpadding="0" width="100%" border="0">
                                                                    <tr>
                                                                        <td height="257" width="557" align="center">
                                                                            <iframe id="agree" height="360" border="0" src="'.BASE_GUILD_URL.'guild_new/html/agreement.html" frameBorder="0" width="557">
                                                                            </iframe>
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
                            </table>
                        </td>
                    </tr>
                    <tr>
                        <td colspan="2">
                            <div align="center">
                                <table>
                                    <tr>
                                        <td height="45" vAlign="bottom" align="center">
                                            <label for="checkbox">
                                                I agree
                                            </label>
                                            <input id="checkbox" type="checkbox" name="checkbox">
                                            &nbsp;&nbsp;&nbsp; 
                                            <input type="image" name="ctl01" align="absMiddle" style="border-width: 0px" src="img/btn_club_reg.gif">
                                        </td>
                                    </tr>
                                </table>
                            </div>
                        </td>
                    </tr>
                </table>';

            echo '</td>
                </tr>';

            echo '</table>';

            echo '</form>';
        }

        private function FinalPage() {
            echo '<table width="642" height="400" cellspacing="0" cellpadding="0" border="0">';

            // Table Title and Result
            echo '<tr>
                    <td align="center" vAlign="top">';

            echo '<table height="100%" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="45" vAlign="middle">
                            <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_edit_main.gif" height="23">
                        </td>
                    </tr>
                    <tr>
                        <td vAlign="middle" align="center">
                            <table cellspacing="0" cellpadding="0" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td align="center" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="1" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" style="padding: 1px">
                                                    <table cellspacing="0" cellpadding="0" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td height="40" align="center">
                                                                <table cellspacing="20" cellpadding="0" border="0">
                                                                    <tr>';

            echo '                                                      <td align="center">'.$this->msg.'</td>';

            // Table bottom
            echo '                                                  </tr>
                                                                </table>
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

            echo '</table>';
        }

        private function content() {

            if ($this->msg == null)
                $this->Agreement();
            else
                $this->FinalPage();
        }
    }

    // Guild Pre-Join
    $pre_join = new GuildPreJoin();

    $pre_join->show();
?>