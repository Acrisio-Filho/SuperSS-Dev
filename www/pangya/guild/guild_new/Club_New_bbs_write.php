<?php
    // Arquivo Club_New_bbs_write.php
    // Criado em 12/09/2019 as 23:25 por Acrisio
    // Club New BBS Write

    include_once("source/guild_new.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_NEW_URL', BASE_GUILD_URL.'guild_new');

    include_once("../source/table.inc");

    class GuildBBSWrite extends GuildNew {

        private $seq = 0;

        public function show() {

            $this->checkPost();

            $this->begin();

            echo '<title>Guild BBS Write</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            // Get
            $this->seq = (isset($_GET['seq']) && is_numeric($_GET['seq'])) ? $_GET['seq'] : 0;

            // Post and Session
            if (!isset($_SESSION))
                session_start();

            // Não tem uma sequência válida de um BBS
            if ($this->seq == 0) {

                header("Location: ".LINKS['UNKNOWN_ERROR']);

                // sai do script para o navegador redirecionar
                exit();
            }

            // Verifica se o player tem o level necessário para escrever um BBS
            if (PlayerSingleton::getInstance()['LEVEL'] < 1/*Rookie E*/) {
                
                // BBS view
                header("Location: ".BASE_GUILD_NEW_URL."/Club_New_bbs_view.php?seq=".$this->seq);

                // sai do script para o navegador redirecionar
                exit();
            }

            if (empty($_POST))
                $_SESSION['BBS_WRITE_VIEWSTATE'] = ['TEXT' => ''];
            else
                $_SESSION['BBS_WRITE_VIEWSTATE'] = [
                                                    'TEXT' => isset($_POST['text']) ? $_POST['text'] : ''
                                                   ];

            if (isset($_POST['up']) && $_POST['up'] = 'Post') {

                if ($_SESSION['BBS_WRITE_VIEWSTATE']['TEXT'] != '' && strlen($_SESSION['BBS_WRITE_VIEWSTATE']['TEXT']) <= 500) {

                    // Criar um novo Guild BBS Comment
                    $this->writeBBS($this->seq);
                
                }else
                    echo "<script>javascript:alert('Failed to comment on BBS.');</script>";
            }
            
        }

        private function writeBBS($seq) {

            // Protect from HTML tags
            $_SESSION['BBS_WRITE_VIEWSTATE']['TEXT'] = strip_tags($_SESSION['BBS_WRITE_VIEWSTATE']['TEXT']);

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);           // PLAYER UID
            $params->add('i', $seq);                                            // BBS SEQ
            $params->add('s', $_SESSION['BBS_WRITE_VIEWSTATE']['TEXT']);        // TEXT

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcWriteGuildBBSReply ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcWriteGuildBBSReply(?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcWriteGuildBBSReply(?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0)
                header("Location: ".BASE_GUILD_NEW_URL."/Club_New_bbs_view.php?seq=".$seq);
            else
                echo "<script>javascript:alert('Failed to comment BBS.');</script>";

        }

        private function content() {

            echo '<form id="ctl00" method="post" action="./Club_New_bbs_write.php?seq='.$this->seq.'">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="45" vAling="middle" colspan="2">
                            <img style="display: block" src="'.BASE_GUILD_NEW_URL.'/img/title_guild_bbs.gif" height="23">
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

    // Guild BBS Write
    $bbs_write = new GuildBBSWrite();

    $bbs_write->show();

?>