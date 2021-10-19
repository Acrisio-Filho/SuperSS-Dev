<?php
    // Arquivo club_closed.php
    // Criado em 16/07/2019 as 15:02 por Acrisio
    // Definição e Implementação da classe GuildClosed

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    class GuildClosed extends GuildHome {

        public function show() {

            // Verifica a autoridade do player
            $this->checkAuthority(AUTH_LEVEL_ADMIN);

            $this->checkPost();

            $this->begin();

            echo '<title>Guild Closed</title>';

            $this->middle();

            Design::menuEdit();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            if (!isset($_SESSION))
                session_start();

            if (!isset($_SESSION['CLOSED']))
                $_SESSION['CLOSED'] = [];

            if (isset($_POST) && !empty($_POST)) {

                if (isset($_SESSION['CLOSED'])) {

                    if (isset($_SESSION['CLOSED']['msg']))
                        unset($_SESSION['CLOSED']['msg']);

                    // Verifica post
                    if (isset($_POST['submit']) && $_POST['submit'] == 'Execute') {

                        $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                        $params = $db->params;

                        if ($this->isClosure()) {

                            // Reabrir a guild se não foi o GM quem fechou
                            if (GuildSingleton::getInstanceFullInfo()['STATE'] == ST_GM_CLOSURE) {

                                $_SESSION['CLOSED']['msg'] = 'You can\'t reactivate the club because it was GM who closed it.';

                                // Sai da função por que o player não pode reativar a guild por que foi o GM quem fechou ela
                                return;
                            }

                            // Reativa a Guild aqui
                            $params->clear();
                            $params->add('i', GuildSingleton::getInstance()['UID']);
                            $params->add('i', PlayerSingleton::getInstance()['UID']);

                            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcGuildReOpen ?, ?';
                            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'select "_RET_" as "RET" from '.$db->con_dados['DB_NAME'].'.ProcGuildReOpen(?, ?)';
                            else
                                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGuildReOpen(?, ?)';

                            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                                && ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] == 1) {

                                // Atualiza a instância da guild por que reativou a guild
                                GuildSingleton::updateAllInstance();

                            }else
                                $_SESSION['CLOSED']['msg'] = 'Failed to reactivate the club.';

                        }else {

                            //Atualiza a instância da guild para ter certeza do números de membros
                            GuildSingleton::updateAllInstance();

                            // Aqui verifica se todos membros foram expulsos para pode fechar a guild
                            if (GuildSingleton::getInstanceFullInfo()['MEMBERS'] > 1) {

                                $_SESSION['CLOSED']['msg'] = 'You cannot close the club before removing all club members except yourself.';

                                // Sai da função por que o player ainda não expulso todos os membros para poder fechar a guild
                                return;
                            }

                            // Fechar a Guild
                            $params->clear();
                            $params->add('i', GuildSingleton::getInstance()['UID']);
                            $params->add('i', PlayerSingleton::getInstance()['UID']);

                            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcGuildClosure ?, ?';
                            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'select "_RET_" as "RET" from '.$db->con_dados['DB_NAME'].'.ProcGuildClosure(?, ?)';
                            else
                                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGuildClosure(?, ?)';

                            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                                && ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] != '') {
                                
                                switch ($row['RET']) {
                                    case 1:
                                        // Atualiza a instância da guild por que fechou a guild
                                        GuildSingleton::updateAllInstance();
                                        break;
                                    case -1:
                                        $_SESSION['CLOSED']['msg'] = 'Failed to close the club.';
                                        break;
                                    case -2:
                                        $_SESSION['CLOSED']['msg'] = 'The club has been blocked by GM, you can\'t close it.';
                                        break;
                                }
                            
                            }else
                                $_SESSION['CLOSED']['msg'] = 'Failed to close the club.';
                        }
                    }

                }
            }
        }

        private function content() {

            // Message Java Script
            if (isset($_SESSION['CLOSED']['msg'])) {
                
                echo '  <script>
                            alert(\''.$_SESSION['CLOSED']['msg'].'\');
                        </script>';

                // Limpa a msg ela já foi exibida
                unset($_SESSION['CLOSED']['msg']);
            }

            echo '<form id="ctl00" method="POST" action="./club_closed.php">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="40" vAlign="middle" colspan="2">
                            <img src="'.BASE_GUILD_HOME_URL.'/img/title_closed.gif">
                        </td>
                    </tr>
                    <tr align="center">
                        <td height="340" vAlign="middle" colspan="2">
                            <table cellspacing="0" cellpadding="0" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td vAlign="top" align="center" style="padding: 15px">
                                                                <table class="text_normal" cellspacing="0" cellpadding="0" border="0">
                                                                    <tr>
                                                                        <td colspan="2">
                                                                            <div style="font-size: 12px; color: #333333">';

            // Texto de fechamento ou reabertura da Guild, ou Mensagem que o GM que Fechou a Guild
            if ($this->isClosure() && GuildSingleton::getInstanceFullInfo()['CLOSURE_DATE'] != '') {

                if (GuildSingleton::getInstanceFullInfo()['STATE'] == ST_CLOSURE) {

                    // Player que fechou a guild
                    echo 'The club will be deleted in '.GuildSingleton::getInstanceFullInfo()['CLOSURE_DATE'].'
                          <br>
                          <br>
                          Do you want to reactivate the club?';

                }else {
                    
                    // GM que fechou a guild
                    echo 'GM closed his club, he will be deleted in '.GuildSingleton::getInstanceFullInfo()['CLOSURE_DATE'];
                }

            }else {
                
                // Fechar a Guild
                echo '                                                          The club cannot be closed unless all club members are removed from the club administration.
                                                                                <br>
                                                                                There is a 7-day hold period after closing the club page.
                                                                                <br>
                                                                                After that, all club information will be deleted.
                                                                                <br>
                                                                                The club master can cancel the club closure application at any time during the hold period.
                                                                                <br>';
            }

            echo '                                                          </div>
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td height="60" vAlign="bottom" align="center">
                                                                            '.(GuildSingleton::getInstanceFullInfo()['STATE'] == ST_GM_CLOSURE && GuildSingleton::getInstanceFullInfo()['CLOSURE_DATE'] != ''
                                                                                ? '' 
                                                                                : '<input id="submit" style="border: rgb(180,160,160) 1px solid" type="submit" value="Execute" name="submit">').'
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
                    </tr>';

            echo '</table>';

            echo '</form>';
        }
    }

    // Guild Closed
    $closed = new GuildClosed();

    $closed->show();
?>