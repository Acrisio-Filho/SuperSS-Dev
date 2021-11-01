<?php
    // Arquivo club_notice_write.php
    // Criado em 13/09/2019 as 18:31 por Acrisio
    // Club Notice Write

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildNoticeWrite extends GuildHome {

        private $seq = 0;

        public function show() {

            // Verifica a autoridade do player
            $this->checkAuthority(AUTH_LEVEL_SUBMASTER);

            // Verifica se a guild está bloqueada ou fecha, aí não pode escrever mais nenhum Notice
            if ($this->isBlocked() || $this->isClosure()) {

                header("Location: ".LINKS['GUILD_ERROR']);

                // sai do script para o navegador redirecionar
                exit();
            }

            $this->checkPost();

            $this->begin();

            echo '<title>Guild Notice Write</title>';

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

            if (empty($_POST))
                $_SESSION['NOTICE_WRITE_VIEWSTATE'] = ['TITLE' => '', 'TEXT' => ''];
            else
                $_SESSION['NOTICE_WRITE_VIEWSTATE'] = ['TITLE' => isset($_POST['title']) ? $_POST['title'] : '',
                                                    'TEXT' => isset($_POST['text']) ? $_POST['text'] : ''
                                                   ];

            if (isset($_POST['up']) && $_POST['up'] = 'Post') {

                if ($_SESSION['NOTICE_WRITE_VIEWSTATE']['TITLE'] != '' && strlen($_SESSION['NOTICE_WRITE_VIEWSTATE']['TITLE']) <= 50
                    && $_SESSION['NOTICE_WRITE_VIEWSTATE']['TEXT'] != '' && strlen($_SESSION['NOTICE_WRITE_VIEWSTATE']['TEXT']) <= 500) {
                    
                    // Criar um novo Guild Notice
                    $this->writeNotice();
                
                }else
                    echo "<script>javascript:alert('Failed to create Notice.');</script>";
            }
            
        }

        private function writeNotice() {

            // Protected from HTML tags
            $_SESSION['NOTICE_WRITE_VIEWSTATE']['TITLE'] = strip_tags($_SESSION['NOTICE_WRITE_VIEWSTATE']['TITLE']);
            $_SESSION['NOTICE_WRITE_VIEWSTATE']['TEXT'] = strip_tags($_SESSION['NOTICE_WRITE_VIEWSTATE']['TEXT']);

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', GuildSingleton::getInstance()['UID']);                // GUILD UID
            $params->add('i', PlayerSingleton::getInstance()['UID']);               // PLAYER UID
            $params->add('s', $_SESSION['NOTICE_WRITE_VIEWSTATE']['TITLE']);        // TITLE
            $params->add('s', $_SESSION['NOTICE_WRITE_VIEWSTATE']['TEXT']);         // TEXT

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcWriteGuildNotice ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_SEQ_" as "SEQ" from '.$db->con_dados['DB_NAME'].'.ProcWriteGuildNotice(?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcWriteGuildNotice(?, ?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {
              
                if (($row = $result->fetch_assoc()) != null && isset($row['SEQ']) && $row['SEQ'] > 0)
                    header("Location: ".BASE_GUILD_HOME_URL."/club_notice_view.php?seq=".$row['SEQ']);
                else
                    echo "<script>javascript:alert('Failed to create Notice.');</script>";

            }else
                echo "<script>javascript:alert('Failed to create Notice.');</script>";

        }

        private function content() {

            echo '<form id="ctl00" method="post" action="./club_notice_write.php">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="45" vAling="middle" colspan="2">
                            <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_notice.gif" height="23">
                        </td>
                    </tr>
                    <tr>
                        <td vAlign="middle" colspan="2" align="center">
                            <table cellspacing="0" cellpadding="0" width="580" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table cellspacing="5" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td align="right" style="padding: 5px">
                                                                <div style="color: #6b4e17">
                                                                    Title:
                                                                </div>
                                                            </td>
                                                            <td style="padding: 5px">
                                                                <input id="title" value="'.($_SESSION['NOTICE_WRITE_VIEWSTATE']['TITLE']).'" style="border: rgb(180, 160, 160) 1px solid; padding: 1px 5px" maxLength="49" size="60" name="title">
                                                                &nbsp;
                                                                ※Limited to 50 characters
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td align="right" style="padding: 5px">
                                                                <div style="color: #6b4e17">
                                                                    Text:
                                                                </div>
                                                            </td>
                                                            <td style="padding: 5px">
                                                                <textarea id="text" style="border: rgb(180, 160, 160) 1px solid; padding: 3px 5px" rows="16" cols="90" name="text">'.($_SESSION['NOTICE_WRITE_VIEWSTATE']['TEXT']).'</textarea>
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td style="padding: 0px 30px" colspan="2" align="right">
                                                                ※Limited to 500 characters, HTML tags cannot be used
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td colspan="2" align="center" style="padding: 5px">
                                                                <input id="up" style="border: rgb(180, 160, 160) 1px solid" type="submit" value="Post Notice" name="up">
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

    // Guild Notice Write
    $notice_write = new GuildNoticeWrite();

    $notice_write->show();

?>