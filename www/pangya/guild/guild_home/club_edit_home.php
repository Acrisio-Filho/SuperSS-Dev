<?php
    // Arquivo club_edit_home.php
    // Criado em 16/07/2019 as 14:11 por Acrisio
    // Definição e Implementação da classe GuildEditHome

    include_once("source/guild_home.inc");

    include_once("../source/check_img_size.inc");

    define("BASE_GUILD_HOME_URL", BASE_GUILD_URL."guild_home");

    class GuildEditHome extends GuildHome {

        public function show() {

            // Verifica a autoridade do player
            $this->checkAuthority(AUTH_LEVEL_ADMIN);

            $this->checkPost();

            $this->begin();

            echo '<title>Guild Edit Home</title>';

            $this->middle();

            Design::menuEdit();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            if (!isset($_SESSION))
                session_start();

            if (!isset($_SESSION['EDIT_HOME']))
                $_SESSION['EDIT_HOME'] = [];

            // Check uptext ou upfile
            if (isset($_POST) && !empty($_POST)) {

                if (isset($_SESSION['EDIT_HOME'])) {

                    if (isset($_SESSION['EDIT_HOME']['msg']))
                        unset($_SESSION['EDIT_HOME']['msg']);

                    if (isset($_POST['uptext'])) {

                        // Check soma values
                        if ($this->isBlocked() || $this->isClosure()) {

                            $_SESSION['EDIT_HOME']['msg'] = 'You cannot change the welcome message because the club '.($this->isBlocked() ? ' has been blocked by GM.' : ' is in the process of closing.');

                            // Sai da função por que ele não pode alterar o Guild Message Welcome por que o GM bloqueou a guild ou a guild está em processo de fechamento
                            return;

                        }

                        if (isset($_POST['text']) /*&& $_POST['text'] != ''*/ && strlen($_POST['text']) <= 50) {

                            // Remove HTML tags
                            $_POST['text'] = strip_tags($_POST['text']);

                            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                            $params = $db->params;

                            $params->clear();
                            $params->add('i', GuildSingleton::getInstance()['UID']);
                            $params->add('s', $_POST['text']);

                            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildMessageIntro ?, ?';
                            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildMessageIntro(?, ?)';
                            else
                                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildMessageIntro(?, ?)';

                            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                                // Inicializa o GuildSingleton Full Info se ele não estiver inicializado
                                GuildSingleton::updateAllInstance();
                            
                            }else
                                $_SESSION['EDIT_HOME']['msg'] = 'Failed to exchange the welcome message from the club.';

                        }

                    }else if (isset($_POST['upfile'])) {

                        // Check soma values
                        if ($this->isBlocked() || $this->isClosure()) {

                            $_SESSION['EDIT_HOME']['msg'] = 'You can\'t change the club intro image why the club '.($this->isBlocked() ? ' has been blocked by GM.' : ' is in the process of closing.');

                            // Sai da função por que ele não pode alterar o Guild Image Intro por que o GM bloqueou a guild ou a guild está em processo de fechamento
                            return;

                        }

                        include_once("../source/check_intro_img.inc");

                        $ret = ValidIntroImg::checkIntroImg(isset($_FILES['up_file']) ? $_FILES['up_file'] : null);

                        if ($ret['state']) {

                            $filename = GuildSingleton::getInstance()['UID'].'-'.(date("dmy-hi")).'.png';

                            // salve file prev
                            if (file_put_contents($_SERVER['DOCUMENT_ROOT'].'/_Files/guild/intro/'.$filename, base64_decode($ret['img']))) {

                                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                                $params = $db->params;

                                $params->clear();
                                $params->add('i', GuildSingleton::getInstance()['UID']);
                                $params->add('s', $filename);

                                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                    $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildIntroImg ?, ?';
                                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                    $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildIntroImg(?, ?)';
                                else
                                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildIntroImg(?, ?)';

                                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                                    // Inicializa o GuildSingleton Full Info se ele não estiver inicializado
                                    GuildSingleton::updateAllInstance();
                                
                                }else
                                    $_SESSION['EDIT_HOME']['msg'] = 'Unable to change Intro Image.';

                            }else
                                $_SESSION['EDIT_HOME']['msg'] = 'Unable to upload image';

                        }else if (strcmp($ret['msg'], '1') != 0)
                            $_SESSION['EDIT_HOME']['msg'] = $ret['msg'];
                        
                    }
                
                }
            }
        }

        private function content() {

            // Message Java Script
            if (isset($_SESSION['EDIT_HOME']['msg'])) {
                
                echo '  <script>
                            alert(\''.$_SESSION['EDIT_HOME']['msg'].'\');
                        </script>';

                // Limpa a msg ela já foi exibida
                unset($_SESSION['EDIT_HOME']['msg']);
            }

            echo '<form id="ctl00" method="POST" enctype="multipart/form-data" action="./club_edit_home.php">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="40" vAlign="middle" colspan="2">
                            <img src="'.BASE_GUILD_HOME_URL.'/img/title_edit_main.gif">
                        </td>
                    </tr>
                    <tr>
                        <td align="center">
                            <table cellspacing="0" cellpadding="0" width="580" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table height="250" cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td>
                                                                <table style="font-size: 12px; color: #333333" cellspacing="0" width="100%" align="center" border="0">
                                                                    <tr>
                                                                        <td style="padding: 1px" height="35">
                                                                            &nbsp;&nbsp;&nbsp;&nbsp;Club introduction image update
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td style="padding: 1px" align="right">
                                                                            <input id="up_file" style="border: rgb(180,160,160) 1px solid" type="file" name="up_file">
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td style="padding: 1px" align="right">
                                                                            <input id="upfile" style="border: rgb(180,160,160) 1px solid" type="submit" value="&nbsp;Upload" name="upfile">
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td style="padding: 1px" height="10"></td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td style="padding: 1px" align="right">
                                                                            ※Available file formats： jpg, gif, bmp, png
                                                                        </td>
                                                                    </tr>
                                                                </table>
                                                            </td>
                                                            <td vAlign="middle" width="330" align="center">
                                                                <table cellspacing="0" cellpadding="0" bgColor="#ffffff" border="0">
                                                                    <tr>
                                                                        <td style="padding: 5px">
                                                                            <table cellspacing="0" cellpadding="0" border="0">
                                                                                <tr>
                                                                                    <td bgColor="#b39f8e" style="padding: 2px">
                                                                                        <table class="text_normal" cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                                                            <tr>
                                                                                                <td align="center">
                                                                                                    <img '.ValidImgSize::checkImgSize(BASE_GUILD_UPLOAD_URL.'/intro', $_SERVER['DOCUMENT_ROOT'].'_Files/guild/intro/', (GuildSingleton::getInstanceFullInfo()['INTRO_IMG'] == '' ? 'sample.jpg' : GuildSingleton::getInstanceFullInfo()['INTRO_IMG'])).'">
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
                                        </table>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                    <tr>
                        <td height="5"></td>
                    </tr>
                    <tr>
                        <td colspan="2" align="center">
                            <table cellspacing="0" cellpadding="0" width="580" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="1" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td style="padding: 5px">
                                                                <div style="font-size: 12px; color: #333333">
                                                                    Club introduction
                                                                    <br>
                                                                    &nbsp;
                                                                    <input id="text" value="'.htmlspecialchars(GuildSingleton::getInstanceFullInfo()['MESSAGE_INTRO']).'" style="border: rgb(180,160,160) 1px solid; padding: 0px 3px" maxLength="50" size="90" name="text">
                                                                    &nbsp;
                                                                    <input id="uptext" style="border: rgb(180,160,160) 1px solid" type="submit" value="&nbsp;Update" name="uptext">
                                                                    <br>
                                                                    ※Limit characters: 50 characters, HTML tag not available
                                                                </div>
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

            echo '<table>';

            echo '</form>';

        }
    }

    // Guild Edit Home
    $edit_home = new GuildEditHome();

    $edit_home->show();
?>