<?php
    // Arquivo club_private_bbs_write.php
    // Criado em 12/09/2019 as 13:38 por Acrisio
    // Guild Private BBS Write

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildPrivateBBSWrite extends GuildHome {

        private $seq = 0;

        public function show() {

            // Verifica a autoridade do player
            $this->checkAuthority(AUTH_LEVEL_MEMBER);

            // Verifica se a guild está bloqueada ou fecha, aí não pode escrever mais nenhum Notice
            if ($this->isBlocked() || $this->isClosure()) {

                header("Location: ".LINKS['GUILD_ERROR']);

                // sai do script para o navegador redirecionar
                exit();
            }

            $this->checkPost();

            $this->begin();

            echo '<title>Guild Private BBS Write</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            // Get
            $this->seq = (isset($_GET['seq']) && is_numeric($_GET['seq'])) ? $_GET['seq'] : 0;

            // Não tem uma sequência válida de um BBS
            if ($this->seq == 0) {

                header("Location: ".LINKS['GUILD_ERROR']);

                // sai do script para o navegador redirecionar
                exit();
            }

            // Post and Session
            if (!isset($_SESSION))
                session_start();

            if (empty($_POST))
                $_SESSION['BBS_WRITE_VIEWSTATE'] = ['TEXT' => ''];
            else
                $_SESSION['BBS_WRITE_VIEWSTATE'] = [
                                                    'TEXT' => isset($_POST['text']) ? $_POST['text'] : ''
                                                   ];

            if (isset($_POST['up']) && $_POST['up'] = 'Post') {

                if ($_SESSION['BBS_WRITE_VIEWSTATE']['TEXT'] != '' && strlen($_SESSION['BBS_WRITE_VIEWSTATE']['TEXT']) <= 500) {

                    // Criar um novo Guild Private BBS
                    $this->writeBBS($this->seq);
                
                }else
                    echo "<script>javascript:alert('Failed to comment on BBS.');</script>";
            }
            
        }

        private function writeBBS($seq) {

            // Protected from HTML tags
            $_SESSION['BBS_WRITE_VIEWSTATE']['TEXT'] = strip_tags($_SESSION['BBS_WRITE_VIEWSTATE']['TEXT']);

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);           // PLAYER UID
            $params->add('i', $seq);                                            // BBS SEQ
            $params->add('s', $_SESSION['BBS_WRITE_VIEWSTATE']['TEXT']);        // TEXT

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcWriteGuildPrivateBBSReply ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcWriteGuildPrivateBBSReply(?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcWriteGuildPrivateBBSReply(?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0)
                header("Location: ".BASE_GUILD_HOME_URL."/club_private_bbs_view.php?seq=".$seq);
            else
                echo "<script>javascript:alert('Failed to comment on BBS.');</script>";

        }

        private function content() {

            echo '<form id="ctl00" method="post" action="./club_private_bbs_write.php?seq='.$this->seq.'">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="45" vAling="middle" colspan="2">
                            <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_member_bbs.gif" height="23">
                        </td>
                    </tr>
                    <tr>
                        <td vAlign="middle" colspan="2" align="center">
                            <table cellspacing="0" cellpadding="0" width="550" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="middle" style="padding: 1px">
                                                    <table height="200" cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td vAlign="middle" align="center">
                                                                <textarea id="text" style="border: rgb(180, 160, 160) 1px solid; padding: 2px 3px" rows="8" cols="90" name="text">'.($_SESSION['BBS_WRITE_VIEWSTATE']['TEXT']).'</textarea>
                                                                <br><br>
                                                                <input id="up" style="border: rgb(180, 160, 160) 1px solid" type="submit" value="Comment" name="up">
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

    // Guild Private BBS Write
    $bbs_write = new GuildPrivateBBSWrite();

    $bbs_write->show();

?>