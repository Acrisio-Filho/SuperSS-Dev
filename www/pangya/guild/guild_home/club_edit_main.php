<?php
    // Arquivo club_edit_main.php
    // Criado em 16/07/2019 as 14:58 por Acrisio
    // Definição e Implementação da classe GuildEditMain

    include_once("source/guild_home.inc");

    define("BASE_GUILD_HOME_URL", BASE_GUILD_URL."guild_home");

    class GuildEditMain extends GuildHome {

        public function show() {

            // Verifica a autoridade do player
            $this->checkAuthority(AUTH_LEVEL_ADMIN);

            $this->checkPost();

            $this->begin();

            echo '<title>Guild Edit Main</title>';

            $this->middle();

            Design::menuEdit();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            if (!isset($_SESSION))
                session_start();

            if (!isset($_SESSION['EDIT_MAIN']))
                $_SESSION['EDIT_MAIN'] = [];

            if (isset($_POST) && !empty($_POST)) {

                if (isset($_SESSION['EDIT_MAIN'])) {

                    if (isset($_SESSION['EDIT_MAIN']['msg']))
                        unset($_SESSION['EDIT_MAIN']);

                    if (isset($_POST['pre'])) {

                        include_once("../source/check_mark.inc");

                        $ret = ValidMark::checkMark(isset($_FILES['up_file']) ? $_FILES['up_file'] : null);

                        if ($ret['state']) {

                            $filename = PlayerSingleton::getInstance()['ID'].'_prev.png';

                            // salve file prev
                            if (file_put_contents('../../_Files/guild/mark/preview/'.$filename, base64_decode($ret['img']))) {

                                $_SESSION['EDIT_MAIN']['VIEWSTATE'] = ['preview_mark' => $filename];
                            }else
                                $_SESSION['EDIT_MAIN']['msg'] = 'Unable to upload image.';

                        }else if (strcmp($ret['msg'], '1') != 0)
                            $_SESSION['EDIT_MAIN']['msg'] = $ret['msg'];

                    }else if (isset($_POST['mark'])) {

                        if (isset($_SESSION['EDIT_MAIN']['VIEWSTATE']['preview_mark'])) {

                            // Check soma values
                            if ($this->isBlocked() || $this->isClosure()) {

                                $_SESSION['EDIT_MAIN']['msg'] = 'You cannot change the club mark because the club '.($this->isBlocked() ? ' has been blocked by GM.' : ' is in the process of closing.');

                                // Sai da função por que ele não pode alterar o Mark da guild por que o GM bloqueou a guild ou a guild está em processo de fechamento
                                return;

                            }else if (GuildSingleton::getInstanceFullInfo()['NEW_MARK_IDX'] > 0) {

                                $_SESSION['EDIT_MAIN']['msg'] = 'You cannot change the club mark until the GM approves or rejects the waiting mark.';

                                // Sai da função por que ele não pode alterar o Mark da guild até o GM aprova a antiga
                                return;
                            }

                            include_once($_SERVER['DOCUMENT_ROOT']."/config/base.inc");

                            // Pega do Banco de dados o NEW MARK INDEX
                            $new_mark_index = GuildSingleton::getInstanceFullInfo()['MARK_IDX'] + 1;

                            // Make Mark Name
                            $mark_name = checkFileName(GuildSingleton::getInstance()['UID'].'_'.$new_mark_index).'.png';

                            // Copy preview para o mark oficial
                            if (rename($_SERVER['DOCUMENT_ROOT'].'/_Files/guild/mark/preview/'.$_SESSION['EDIT_MAIN']['VIEWSTATE']['preview_mark'], $_SERVER['DOCUMENT_ROOT'].'/_Files/guild/mark/'.$mark_name)) {
                                
                                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                                $params = $db->params;

                                $params->clear();
                                $params->add('i', GuildSingleton::getInstance()['UID']);
                                $params->add('i', PlayerSingleton::getInstance()['UID']);
                                $params->add('i', $new_mark_index);

                                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                    $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildNewMarkIdx ?, ?, ?';
                                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                    $query = 'select "_RET_" as "RET" from '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildNewMarkIdx(?, ?, ?)';
                                else
                                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildNewMarkIdx(?, ?, ?)';

                                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                                    && ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] != '') {

                                        switch ($row['RET']) {
                                            case 1: // Sucesso
                                            {
                                                // Update
                                                GuildSingleton::updateAllInstance();
                                                    
                                                break;
                                            }
                                            case -1:
                                                $_SESSION['EDIT_MAIN']['msg'] = 'Failed to add a new club mark.';
                                                break;
                                            case -2:
                                                $_SESSION['EDIT_MAIN']['msg'] = 'Not enough pangs to add a new club mark.';
                                                break;
                                            case -3:
                                                $_SESSION['EDIT_MAIN']['msg'] = 'You are not allowed to add a new club mark.';
                                                break;
                                        }
                                
                                }else
                                    $_SESSION['EDIT_MAIN']['msg'] = 'Failed to add a new club mark.';

                                // Limpa o preview mark, por que o arquivo já foi trocado de pasta
                                if (isset($_SESSION['EDIT_MAIN']['VIEWSTATE']['preview_mark']))
                                    unset($_SESSION['EDIT_MAIN']['VIEWSTATE']['preview_mark']);
                            
                            }else
                                $_SESSION['EDIT_MAIN']['msg'] = 'Failed to save the club mark.';
                        
                        }else
                            $_SESSION['EDIT_MAIN']['msg'] = 'Click the preview button first.';

                    }else if (isset($_POST['info'])) {

                        // Check soma values
                        if ($this->isBlocked() || $this->isClosure()) {

                            $_SESSION['EDIT_MAIN']['msg'] = 'You cannot change club information because the club '.($this->isBlocked() ? ' has been blocked by GM.' : ' is in the process of closing.');

                            // Sai da função por que ele não pode alterar o Master comentário da guild por que o GM bloqueou a guild ou a guild está em processo de fechamento
                            return;

                        }

                        // MASTER COMMENT
                        if (isset($_POST['text']) /*&& $_POST['text'] != ''*/ && strlen($_POST['text']) <= 50
                            && isset($_POST['dropdown']) && $_POST['dropdown'] != '' && is_numeric($_POST['dropdown'])
                            && isset($_POST['insert']) && $_POST['insert'] != '') {

                            // Remove HTML tags
                            $_POST['text'] = strip_tags($_POST['text']);

                            // PERMISSÃO PARA INGRESSAR NA GUILD
                            $permition = ($_POST['insert'] == 'radiobutton1') ? 1 : 0;

                            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                            $params = $db->params;

                            $params->clear();
                            $params->add('i', GuildSingleton::getInstance()['UID']);
                            $params->add('i', $_POST['dropdown']);      // CONDITION LEVEL
                            $params->add('i', $permition);              // PERMITION
                            $params->add('s', $_POST['text']);          // MASTER COMMENT

                            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildInfo ?, ?, ?, ?';
                            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildInfo(?, ?, ?, ?)';
                            else
                                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateGuildInfo(?, ?, ?, ?)';

                            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {
                                
                                // Inicializa o Guild Info Full
                                GuildSingleton::updateAllInstance();
                            
                            }else
                                $_SESSION['EDIT_MAIN']['msg'] = 'Failed to update club information.';
                        
                        }
                    }

                }
            }

        }

        private function content() {

            // Message Java Script
            if (isset($_SESSION['EDIT_MAIN']['msg'])) {
                
                echo '  <script>
                            alert(\''.$_SESSION['EDIT_MAIN']['msg'].'\');
                        </script>';

                // Limpa a msg ela já foi exibida
                unset($_SESSION['EDIT_MAIN']['msg']);
            }

            echo '<form id="ctl00" enctype="multipart/form-data" method="POST" action="./club_edit_main.php">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="40" vAlign="middle" colspan="2">
                            <img src="'.BASE_GUILD_HOME_URL.'/img/title_info.gif">
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
                                                    <table cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr bgColor="#e7dcd4">
                                                            <td height="30" colspan="2" align="center">
                                                                <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" align="absMiddle" height="4">
                                                                <font style="font-size: 12px; color: #644636">
                                                                    <b>Club mark update</b>
                                                                </font>
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td colspan="2">
                                                                <div style="font-size: 12px; color: #333333; padding-top: 5px; padding-left: 5px;">
                                                                    Club mark image file can be updated.
                                                                    <br>
                                                                    5,000 PP will be consumed for Club mark update.
                                                                    <br>
                                                                    It takes some time for the updated club mark to be reflected in the game.
                                                                    <br>
                                                                    (Format: Horizontal x vertical, Size: 22x20 *.jpg, *.gif, *.png)
                                                                </div>
                                                                <div></div>
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td height="20"></td>
                                                        </tr>
                                                        <tr>
                                                            <td height="30">
                                                                <div style="font-size: 12px; color: #333333">
                                                                    &nbsp;&nbsp;
                                                                    <img style="display: inline" border="1" src="'.((GuildSingleton::getInstanceFullInfo()['MARK_IDX'] <= 0) ? BASE_GUILD_UPLOAD_URL.'/mark/sample.png' : BASE_GUILD_UPLOAD_URL.'/mark/'.(GuildSingleton::getInstance()['UID'].'_'.GuildSingleton::getInstanceFullInfo()['MARK_IDX']).'.png').'" width="22" height="20">
                                                                    &nbsp;
                                                                    Current club mark
                                                                </div>
                                                            </td>
                                                            <td height="30">
                                                                <div style="font-size: 12px; color: #333333">
                                                                    &nbsp;
                                                                    <img style="display: inline" border="1" src="'.(isset($_SESSION['EDIT_MAIN']['VIEWSTATE']['preview_mark']) ? BASE_GUILD_UPLOAD_URL.'/mark/preview/'.$_SESSION['EDIT_MAIN']['VIEWSTATE']['preview_mark'].'?dummy='.rand(15151515, 15151651651651) : '').'" width="22" height="20">
                                                                    &nbsp;
                                                                    Preview
                                                                </div>
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td height="30">
                                                                <div style="font-size: 12px; color: #333333">
                                                                    &nbsp;&nbsp;
                                                                    <img style="display: inline" border="1" src="'.(GuildSingleton::getInstanceFullInfo()['NEW_MARK_IDX'] != '' && GuildSingleton::getInstanceFullInfo()['NEW_MARK_IDX'] > 0 ? BASE_GUILD_UPLOAD_URL.'/mark/'.(GuildSingleton::getInstance()['UID'].'_'.GuildSingleton::getInstanceFullInfo()['NEW_MARK_IDX'].'.png?dummy='.rand(15656818, 457556515151)) : '').'" width="22" height="20">
                                                                    &nbsp;
                                                                    Club mark waiting for change
                                                                </div>
                                                            </td>
                                                            <td height="30">
                                                                <input id="up_file" style="border: rgb(180,160,160) 1px solid" type="file" name="up_file">
                                                                <input id="pre" style="border: rgb(180,160,160) 1px solid" type="submit" value="Preview" name="pre">
                                                                &nbsp;&nbsp;&nbsp;
                                                                <input id="mark" style="border: rgb(180,160,160) 1px solid" type="submit" value="Update" name="mark">
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
                        <td height="20"></td>
                    </tr>
                    <tr>
                        <td align="center">
                            <table cellspacing="0" cellpadding="0" width="580" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr bgColor="#e7dcd4">
                                                            <td height="30" align="center">
                                                                <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" align="absMiddle" height="4">
                                                                <font style="font-size: 12px; color: #644636">
                                                                    <b>Club information update</b>
                                                                </font>
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td style="padding-top: 5px; padding-left: 5px" height="25">
                                                                <div style="font-size: 12px; color: #333333">
                                                                    Club membership requirements：
                                                                    <select id="dropdown" name="dropdown">
                                                                        <option value="0" '.(GuildSingleton::getInstanceFullInfo()['CONDITION_LEVEL'] == '0' ? 'selected' : '').'>
                                                                            No level condition
                                                                        </option>
                                                                        <option value="6" '.(GuildSingleton::getInstanceFullInfo()['CONDITION_LEVEL'] == '6' ? 'selected' : '').'>
                                                                            Beginner or above
                                                                        </option>
                                                                        <option value="11" '.(GuildSingleton::getInstanceFullInfo()['CONDITION_LEVEL'] == '11' ? 'selected' : '').'>
                                                                            Junior or above
                                                                        </option>
                                                                        <option value="16" '.(GuildSingleton::getInstanceFullInfo()['CONDITION_LEVEL'] == '16' ? 'selected' : '').'>
                                                                            Senior or above
                                                                        </option>
                                                                        <option value="21" '.(GuildSingleton::getInstanceFullInfo()['CONDITION_LEVEL'] == '21' ? 'selected' : '').'>
                                                                            Amateur or above
                                                                        </option>
                                                                    </select>
                                                                </div>
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td style="padding: 0px 5px" height="25">
                                                                <div style="font-size: 12px; color: #333333">
                                                                    Membership permission：
                                                                    <input id="radiobutton1" type="radio" value="radiobutton1" name="insert" '.(GuildSingleton::getInstanceFullInfo()['PERMITION_JOIN'] == '1' 
                                                                                                                                                    && !$this->isBlocked() && !$this->isClosure() ? 'checked' : '').'>
                                                                    <label for="radiobutton1">
                                                                        Approval
                                                                    </label>
                                                                    <input id="radiobutton2" type="radio" value="radiobutton2" name="insert" '.(GuildSingleton::getInstanceFullInfo()['PERMITION_JOIN'] == '0' 
                                                                                                                                                    || $this->isBlocked() || $this->isClosure() ? 'checked' : '').'>
                                                                    <label for="radiobutton2">
                                                                        Disapproval
                                                                    </label>
                                                                </div>
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td style="padding-left: 5px" height="40">
                                                                <div style="font-size: 12px; color: #333333">
                                                                    Comments master：
                                                                     
                                                                    <input id="text" value="'.htmlspecialchars(GuildSingleton::getInstanceFullInfo()['MASTER_COMMENT']).'" style="padding-left: 2px; border: rgb(180,160,160) 1px solid" maxLength="50" size="80" name="text">
                                                                    <br>
                                                                    ※Limit characters: 50 characters, HTML tag not available
                                                                </div>
                                                            </td>
                                                        </tr>
                                                        <tr>
                                                            <td height="40" align="center">
                                                                <input id="info" style="border: rgb(180,160,160) 1px solid" type="submit" value=" Update " name="info">
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

    // Guild Edit Main
    $edit_main = new GuildEditMain();

    $edit_main->show();
?>