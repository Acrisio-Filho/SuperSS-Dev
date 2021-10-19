<?php
    // Arquivo club_myinfo.php
    // Criado em 16/07/2019 as 14:08 por Acrisio
    // Definição e Implementação da classe GuildMyInfo

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');
    define('BASE_GUILD_NEW_URL', BASE_GUILD_URL.'guild_new');

    class GuildMyInfo extends GuildHome {

        public function show() {

            // Verifica a autoridade do player
            $this->checkAuthority(AUTH_LEVEL_MEMBER);

            $this->checkPost();

            $this->begin();

            echo '<title>Guild My Info</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            $submit = (isset($_POST['submit']) ? $_POST['submit'] : null);

            if (!isset($_SESSION))
                session_start();

            if (!isset($_SESSION['CLUB_MYINFO']))
                $_SESSION['CLUB_MYINFO'] = [];

            if ($submit != null && $submit == 'Withdraw') {

                if (isset($_SESSION['CLUB_MYINFO']['msg']))
                    unset($_SESSION['CLUB_MYINFO']['msg']);

                if (PlayerSingleton::getInstance()['GUILD_UID'] == GuildSingleton::getInstance()['UID']
                        && PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 1/*Master*/ && PlayerSingleton::getInstance()['UID'] == GuildSingleton::getInstance()['MASTER']['UID'])
                    $_SESSION['CLUB_MYINFO']['msg'] = 'You cannot leave the club because you are the master, you have to close the club when it is closed you will be excluded from the club.';
                else {

                    $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                    $params = $db->params;

                    $params->clear();
                    $params->add('i', PlayerSingleton::getInstance()['UID']);
                    $params->add('i', GuildSingleton::getInstance()['UID']);

                    $query = '';

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed']) {

                        if (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 9)
                            $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcGuildCancelaPedido ?, ?';
                        else
                            $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcSaiGuild ?, ?';
                        
                    }else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed']) {
                        
                        if (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 9)
                            $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcGuildCancelaPedido(?, ?)';
                        else
                            $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcSaiGuild(?, ?)';

                    }else {
                        
                        if (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 9)
                            $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGuildCancelaPedido(?, ?)';
                        else
                            $query = 'call '.$db->con_dados['DB_NAME'].'.ProcSaiGuild(?, ?)';
                    }

                    if (($result = $db->db->execPreparedStmt($query, $params->get())) == null || $db->db->getLastError() != 0)
                        $_SESSION['CLUB_MYINFO']['msg'] = 'Failed to leave club.';
                    else {

                        // Update SESSION player
                        $_SESSION['player']['GUILD_UID'] = -1;
                        $_SESSION['player']['MEMBER_STATE_FLAG'] = -1;

                        // Update Guild Singleton
                        GuildSingleton::updateAllInstance();

                        // Redireciona para o index
                        header("Location: ".BASE_GUILD_NEW_URL."/index.php");

                        // sai para o script para o navegador redirecionar a página
                        exit();
                    }
                }
            }

        }

        private function content() {

            // Message Java Script
            if (isset($_SESSION['CLUB_MYINFO']['msg'])) {
                
                echo '  <script>
                            alert(\''.$_SESSION['CLUB_MYINFO']['msg'].'\');
                        </script>';

                // Limpa a msg ela já foi exibida
                unset($_SESSION['CLUB_MYINFO']['msg']);
            }

            echo '<form name="ctl00" method="POST" action="./club_myinfo.php">';

            echo '<table class="text_normal" cellSpacing="0" cellPadding="0" width="95%" border="0">
                        <tr>
                            <td height="45" vAlign="middle" colSpan="2">
                                <img src="'.BASE_GUILD_HOME_URL.'/img/title_myinfo.gif" height="23">
                            </td>
                        </tr>
                        <tr>
                            <td height="140"></td>
                        </tr>
                        <tr>
                            <td align="center">
                                <table cellSpacing="0" cellPadding="0" width="400" bgColor="#fbf1e6" border="0">
                                        <tr>
                                            <td vAlign="top" style="padding: 5px">
                                                <table cellSpacing="0" cellPadding="0" width="100%" border="0">
                                                        <tr>
                                                            <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                                <table cellSpacing="0" cellPadding="0" width="100%" bgColor="#ffffff" border="0">
                                                                        <tr>
                                                                            <td vAlign="top" align="center">
                                                                                <table class="text_normal" cellSpacing="1" cellPadding="0" width="100%" border="0">
                                                                                        <tr>
                                                                                            <td height="40" width="60%" align="right" style="padding: 1px">
                                                                                                <div style="font-size: 12px; color: #333333">
                                                                                                    Do you want to leave the club?
                                                                                                </div>
                                                                                            </td>
                                                                                            <td height="40" width="40%" align="center" style="padding: 1px">
                                                                                                <input id="submit" style="border: rgb(180,160,160) 1px solid;" type="submit" value="Withdraw" name="submit">
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
                </table>';

            echo '</form>';
        }
    }

    // Guild My Info
    $myinfo = new GuildMyInfo();

    $myinfo->show();
?>