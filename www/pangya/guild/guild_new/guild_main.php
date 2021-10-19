<?php
    // Arquivo guild_main.php
    // Criado em 16/07/2019 as 11:07 por Acrisio
    // Definição e Implementação da classe GuildCreate

    include_once("source/guild_new.inc");
    include_once("../source/player_singleton.inc");

    define("BASE_GUILD_NEW_URL", BASE_GUILD_URL."guild_new");

    define("GUILD_NAME_MAX_LENGTH", 16);

    class GuildCreate extends GuildNew {

        public function show() {

            if (PlayerSingleton::getInstance()['GUILD_UID'] > 0) {
                
                header("Location: ".LINKS['UNKNOWN_ERROR']);

                // Sai do script para o navegador redirecionar para a página
                exit();
            }

            $this->begin();

            echo '<title>Guild Create</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            if (!isset($_SESSION))
                session_start();

            if (isset($_POST) && !empty($_POST)) {

                if (isset($_SESSION['CREATE'])) {
                    
                    // Limpa msg se ela existir
                    if (isset($_SESSION['CREATE']['msg']))
                        unset($_SESSION['CREATE']['msg']);
                    
                    switch ($_SESSION['CREATE']['page']) {
                        case 0: // Termos
                            if (isset($_POST['ctl01_x']) && isset($_POST['ctl01_y'])
                                && isset($_POST['checkbox']) && $_POST['checkbox'] == 'on') {

                                // Verifica se o player pode Criar guild
                                if (PlayerSingleton::getInstance()['IDState'] == 2048/*Bloqueado para criar guild*/) {

                                    // Redireciona para o index, por que o player não pode criar guild
                                    echo "<script>alert('You can\'t create a club');window.document.location.href = '".BASE_GUILD_NEW_URL."/index.php'</script>";

                                    // sai do script para o navegador redirecionar para a página
                                    exit();
                                }

                                // Verifica se o player é menor que Iniciante E (Rookie E)
                                if (PlayerSingleton::getInstance()['LEVEL'] < 1/*Rookie E*/) {

                                    // Mostra a mensagem e redireciona para a página de index
                                    echo "<script>alert('You do not have the level required to create a club');window.document.location.href = '".BASE_GUILD_NEW_URL."/index.php'</script>";

                                    // sai do script para o navegador redirecionar para a página
                                    exit();
                                }

                                // Player pode criar guild
                                $_SESSION['CREATE']['page'] = 1;

                                // Clear
                                if (isset($_SESSION['CREATE']['verify']))
                                    unset($_SESSION['CREATE']['verify']);

                                if (isset($_SESSION['CREATE']['name']))
                                    unset($_SESSION['CREATE']['name']);
                            }
                            break;
                        case 1: // Nome e Intro Guild
                            // VIEW STATE
                            $_SESSION['CREATE']['VIEWSTATE'] = [
                                'gName' => isset($_POST['gName']) ? mb_convert_encoding(htmlspecialchars($_POST['gName']), "SJIS", "UTF-8") : '',
                                'dropdown' => isset($_POST['dropdown']) && is_numeric($_POST['dropdown']) ? $_POST['dropdown'] : 0,
                                'gDoor' => isset($_POST['gDoor']) ? $_POST['gDoor'] : '',
                                'gIntro' => isset($_POST['gIntro']) ? $_POST['gIntro'] : ''
                            ];

                            if (isset($_POST['ctl01'])) {

                                // Default value
                                $_SESSION['CREATE']['verify'] = 'FAIL';

                                if (isset($_POST['gName']) && $_POST['gName'] != '') {

                                    // Convert para SHIFT_JIS
                                    $_POST['gName'] = mb_convert_encoding(htmlspecialchars($_POST['gName']), "SJIS", "UTF-8");

                                    if (strlen($_POST['gName']) <= GUILD_NAME_MAX_LENGTH) {

                                        include_once("source/check_name.inc");

                                        // !@ verifica no banco de dados se o nome da guild já existe
                                        if (ValidName::checkName($_POST['gName'])) {

                                            if (isset($_SESSION['CREATE']['name']))
                                                unset($_SESSION['CREATE']['name']);

                                            // Verificou o Nickname
                                            $_SESSION['CREATE']['verify'] = 'OK';

                                            $_SESSION['CREATE']['msg_nick'] = 'Club name "'.mb_convert_encoding($_POST['gName'], "UTF-8", "SJIS").'" is available.';
                                        
                                        }else
                                            $_SESSION['CREATE']['msg_nick'] = 'Club name "'.mb_convert_encoding($_POST['gName'], "UTF-8", "SJIS").'" not available.';
                                    
                                    }else
                                        $_SESSION['CREATE']['msg_nick'] = 'Club name "'.mb_convert_encoding($_POST['gName'], "UTF-8", "SJIS").'" it\'s too long.';
                                }

                            }else if (isset($_POST['ctl03_x']) && isset($_POST['ctl03_y'])) {
                                
                                if (isset($_SESSION['CREATE']['verify']) && $_SESSION['CREATE']['verify'] == 'OK' && isset($_POST['gName']) && $_POST['gName'] != '') {

                                    // Convert para SHIFT_JIS
                                    $_POST['gName'] = mb_convert_encoding(htmlspecialchars($_POST['gName']), "SJIS", "UTF-8");
                                    
                                    if (strlen($_POST['gName']) <= GUILD_NAME_MAX_LENGTH) {
                                        
                                        include_once("source/check_name.inc");

                                        // !@ verifica no banco de dados se o nome da guild já existe
                                        if (ValidName::checkName($_POST['gName'])) {
                                            
                                            $_SESSION['CREATE']['name'] = $_POST['gName'];
                                            $_SESSION['CREATE']['lvl_cond'] = (is_numeric($_POST['dropdown'])) ? $_POST['dropdown'] : 0;
                                            $_SESSION['CREATE']['intro'] = isset($_POST['gDoor']) ? $_POST['gDoor'] : '';
                                            $_SESSION['CREATE']['master_comment'] = isset($_POST['gIntro']) ? $_POST['gIntro'] : '';

                                            // Next Page
                                            $_SESSION['CREATE']['page'] = 2;

                                        }else
                                            $_SESSION['CREATE']['msg'] = 'Club name "'.mb_convert_encoding($_POST['gName'], "UTF-8", "SJIS").'" not available.';
                                    
                                    }else
                                        $_SESSION['CREATE']['msg'] = 'Club name "'.mb_convert_encoding($_POST['gName'], "UTF-8", "SJIS").'" it\'s too long.';
                                
                                }else
                                    $_SESSION['CREATE']['msg'] = 'Please check the club name first by clicking the "Check" button.';
                            
                            }else if (isset($_POST['ctl02_x']) && isset($_POST['ctl02_y']))
                                $_SESSION['CREATE']['page'] = 0;
                            break;
                        case 2: // Esculdo Guild
                            if (isset($_POST['ctl01'])) {

                                include_once("../source/check_mark.inc");

                                $ret = ValidMark::checkMark(isset($_FILES['up_File']) ? $_FILES['up_File'] : null);

                                if ($ret['state']) {

                                    $filename = PlayerSingleton::getInstance()['ID'].'_prev.png';

                                    // salve file prev
                                    if (file_put_contents('../../_Files/guild/mark/preview/'.$filename, base64_decode($ret['img']))) {

                                        $_SESSION['CREATE']['mark'] = $filename;
                                    }else
                                        $_SESSION['CREATE']['msg'] = 'could not upload the image.';

                                }else if (strcmp($ret['msg'], '1') != 0)
                                    $_SESSION['CREATE']['msg'] = $ret['msg'];

                            }else if (isset($_POST['ctl03_x']) && isset($_POST['ctl03_y'])) {

                                if (isset($_SESSION['CREATE']['mark'])) {

                                    include_once("source/check_name.inc");

                                    // !@ verifica no banco de dados se o nome da guild já existe
                                    if (ValidName::checkName($_SESSION['CREATE']['name'])) {
                                        
                                        $_SESSION['CREATE']['master'] = PlayerSingleton::getInstance()['UID'];

                                        include_once("source/make_guild.inc");

                                        // Cria a guild aqui no DB
                                        if (($ret = MakeGuild::make($_SESSION['CREATE'])) > 0) {

                                            $_SESSION['CREATE']['page'] = 3;/*Termina*/
                                        
                                        }else {

                                            switch ($ret) {
                                                case -1:
                                                    $_SESSION['CREATE']['msg'] = 'Failed to create club.';
                                                    break;
                                                case -2:
                                                    $_SESSION['CREATE']['msg'] = 'You cannot create the club, it has not been 1 day since you created another club.';
                                                    break;
                                                case -3:
                                                    $_SESSION['CREATE']['msg'] = 'You do not have enough pangs to create the club.';
                                                    break;
                                                case -4:
                                                    $_SESSION['CREATE']['msg'] = 'You cannot create the club, you are already a member of another club, leave it so you can create another club.';
                                                    break;
                                            }

                                            // Back to last page
                                            if (isset($_SESSION['CREATE']['verify']))
                                                unset($_SESSION['CREATE']['verify']);

                                            if (isset($_SESSION['CREATE']['name']))
                                                unset($_SESSION['CREATE']['name']);

                                            $_SESSION['CREATE']['page'] = 1;    // volta para a página que escolhe o nome de novo
                                        }
                                    
                                    }else {

                                        // Back to last page
                                        if (isset($_SESSION['CREATE']['verify']))
                                            unset($_SESSION['CREATE']['verify']);

                                        if (isset($_SESSION['CREATE']['name']))
                                            unset($_SESSION['CREATE']['name']);

                                        $_SESSION['CREATE']['msg'] = 'Club name "'.mb_convert_encoding($_SESSION['CREATE']['name'], "UTF-8", "SJIS").'" not available.';

                                        $_SESSION['CREATE']['page'] = 1;    // volta para a página que escolhe o nome de novo
                                    }
                                
                                }else
                                    $_SESSION['CREATE']['msg'] = 'Please upload an image to be the club mark.';
                            
                            }else if (isset($_POST['ctl02_x']) && isset($_POST['ctl02_y'])) {

                                // Back to last page
                                if (isset($_SESSION['CREATE']['verify']))
                                    unset($_SESSION['CREATE']['verify']);

                                if (isset($_SESSION['CREATE']['name']))
                                    unset($_SESSION['CREATE']['name']);
                                
                                // Page 1, check name of club and information of club
                                $_SESSION['CREATE']['page'] = 1;
                            }
                            break;
                        default:
                            break;
                    }
                
                }else {
                    
                    $_SESSION['CREATE'] = ['page' => (isset($_POST['ctl01']) ? 1 : 0)];
                }

            }else {

                if (isset($_SESSION['CREATE']))
                    unset($_SESSION['CREATE']);
                
                $_SESSION['CREATE'] = ['page' => 0];
            }
        }

        private function pageAgreement() {

            echo '<form id="ctl00" method="post" action="./guild_main.php">';

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';

            // Title
            echo '  <tr>
                        <td colspan="2">
                            <img src="img/title_apply.gif" width="110" height="29">
                        </td>
                    </tr>';

            // Text
            echo '  <tr>
                        <td height="20" width="419">
                            <img src="img/text1.gif" width="419" height="85">
                        </td>
                        <td height="20" width="196">
                            &nbsp;
                        </td>
                    </tr>';

            // Termos
            echo '  <tr align="center">
                        <td colspan="2">
                            <table width="570" style="border-collapse: collapse" borderColor="#CCCCCC" cellspacing="0" cellpadding="5" border="1">
                                <tr>
                                    <td width="556">
                                        <table cellspacing="0" cellpadding="10" width="577" bgColor="#f3f3f3" border="0">
                                            <tr>
                                                <td height="257" width="557" align="center">
                                                    <iframe id="agree" height="257" border="0" src="html/agreement.html" frameBorder="0" width="557">
                                                    </iframe>
                                                </td>
                                            </tr>
                                        </table>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>';

            // Butão next
            echo '  <tr>
                        <td colspan="2">
                            <div align="center">
                                <table>
                                    <tr>
                                        <td height="50" vAlign="bottom" align="center">
                                            <label for="checkbox">
                                                I agree
                                            </label>
                                            <input id="checkbox" type="checkbox" name="checkbox">
                                            &nbsp;&nbsp;&nbsp; 
                                            <input type="image" name="ctl01" align="absMiddle" style="border-width: 0px" src="img/guild2_03.gif">
                                        </td>
                                    </tr>
                                </table>
                            </div>
                        </td>
                    </tr>';

            // Fecha table
            echo '</table>';

            // Fecha form
            echo '</form>';

        }

        private function pageNameIntro() {

            // Message Java Script
            if (isset($_SESSION['CREATE']['msg'])) {
                
                echo '  <script>
                            alert(\''.$_SESSION['CREATE']['msg'].'\');
                        </script>';

                // Já mostrou a mensagem limpa o SESSION msg
                if (isset($_SESSION['CREATE']['msg']))
                    unset($_SESSION['CREATE']['msg']);
            }

            echo '<form id="ctl00" method="post" action="./guild_main.php">';

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';

            // Title Name Guild
            echo '  <tr>
                        <td align="center">
                            <img src="img/s_title02.gif" width="608" height="31">
                        </td>
                    </tr>';

            echo '  <tr>
                        <td style="padding-bottom: 5px; padding-top: 10px; padding-left: 50px" height="130" width="100%" vAlign="top" colspan="2">
                            <table style="border-collapse: collapse" borderColor="#CCCCCC" height="90%" width="330" cellspacing="0" cellpadding="5" border="1">
                                <tr>
                                    <td>
                                        <table height="100%" cellspacing="0" cellpadding="5" width="100%" bgColor="#f3f3f3" border="0">
                                            <tr>
                                                <td>
                                                    <input id="gName" maxLength="16" value="'.(isset($_SESSION['CREATE']['name']) 
                                                            ? htmlspecialchars(mb_convert_encoding($_SESSION['CREATE']['name'], "UTF-8", "SJIS")) 
                                                            : (isset($_SESSION['CREATE']['VIEWSTATE']) ? htmlspecialchars(mb_convert_encoding($_SESSION['CREATE']['VIEWSTATE']['gName'], "UTF-8", "SJIS")) : '')).'" name="gName" style="font-size: 15px; border: rgb(200, 200, 200) 1px solid">
                                                     &nbsp;
                                                    <input type="submit" name="ctl01" value="Check" style="border: rgb(231, 203, 219) 1px solid;">
                                                    <br>';

            // Mensagem
            if (isset($_SESSION['CREATE']['msg_nick'])) {
                
                echo '                              <div style="font-size: 12px; color: #f02222">
                                                        '.$_SESSION['CREATE']['msg_nick'].'
                                                    </div>';

                // Mensagem já foi mostrada
                if (isset($_SESSION['CREATE']['msg_nick']))
                    unset($_SESSION['CREATE']['msg_nick']);
            }

            echo '                                  <br>
                                                    <div style="font-size: 12px; color: #333333">
                                                        ※ Maximum 16 characters
                                                    </div>
                                                </td>
                                            </tr>
                                        </table>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>';

            // Title Condition
            echo '  <tr>
                        <td align="center">
                            <img src="img/s_title06.gif" width="608" height="31">
                        </td>
                    </tr>';

            echo '  <tr>
                        <td style="padding-bottom: 15px; padding-top: 15px; padding-left: 50px" height="33" width="100%" valign="top">
                            <select id="dropdown" name="dropdown" style="font-size: 12px; font-family: Ms Gothic; border: rgb(231, 203, 219) 1px solid; background-color: rgb(219, 219, 250)">
                                <option value="0" '.(isset($_SESSION['CREATE']['lvl_cond']) && $_SESSION['CREATE']['lvl_cond'] == 0 || (isset($_SESSION['CREATE']['VIEWSTATE']) ? $_SESSION['CREATE']['VIEWSTATE']['dropdown'] == 0 : false) ? 'selected' : '').'>No level condition</option>
                                <option value="6" '.(isset($_SESSION['CREATE']['lvl_cond']) && $_SESSION['CREATE']['lvl_cond'] == 6 || (isset($_SESSION['CREATE']['VIEWSTATE']) ? $_SESSION['CREATE']['VIEWSTATE']['dropdown'] == 6 : false) ? 'selected' : '').'>Beginner or above</option>
                                <option value="11" '.(isset($_SESSION['CREATE']['lvl_cond']) && $_SESSION['CREATE']['lvl_cond'] == 11 || (isset($_SESSION['CREATE']['VIEWSTATE']) ? $_SESSION['CREATE']['VIEWSTATE']['dropdown'] == 11 : false) ? 'selected' : '').'>Junior or above</option>
                                <option value="16" '.(isset($_SESSION['CREATE']['lvl_cond']) && $_SESSION['CREATE']['lvl_cond'] == 16 || (isset($_SESSION['CREATE']['VIEWSTATE']) ? $_SESSION['CREATE']['VIEWSTATE']['dropdown'] == 16 : false) ? 'selected' : '').'>Senior or above</option>
                                <option value="21" '.(isset($_SESSION['CREATE']['lvl_cond']) && $_SESSION['CREATE']['lvl_cond'] == 21 || (isset($_SESSION['CREATE']['VIEWSTATE']) ? $_SESSION['CREATE']['VIEWSTATE']['dropdown'] == 21 : false) ? 'selected' : '').'>Amateur or above</option>
                            </select>
                        </td>
                    </tr>';

            // Title Info Guild Introdulção
            echo '  <tr>
                        <td align="center">
                            <img src="img/s_title07.gif" width="608" height="31">
                        </td>
                    </tr>';

            echo '  <tr>
                        <td style="padding-bottom: 10px; padding-top: 10px; padding-left: 50px" height="33" width="100%" valign="top" colspan="2">
                            <table style="border-collapse: collapse" borderColor="#CCCCCC" cellspacing="0" cellpadding="5" border="1" width="388">
                                <tr>
                                    <td width="420">
                                        <table cellspacing="0" cellpadding="5" width="394" bgColor="#f3f3f3" border="0">
                                            <tr>
                                                <td width="400">
                                                    Club Introduction
                                                    <input id="gDoor" name="gDoor" value="'.(isset($_SESSION['CREATE']['intro']) ? htmlspecialchars($_SESSION['CREATE']['intro']) : (isset($_SESSION['CREATE']['VIEWSTATE']) ? htmlspecialchars($_SESSION['CREATE']['VIEWSTATE']['gDoor']) : '')).'" maxLength="50" size="100" style="border: rgb(200, 200, 200) 1px solid"
                                                    <br>
                                                </td>
                                            </tr>
                                            <tr>
                                                <td width="400">
                                                    Club master comment
                                                    <input id="gIntro" name="gIntro" value="'.(isset($_SESSION['CREATE']['master_comment']) ? htmlspecialchars($_SESSION['CREATE']['master_comment']) : (isset($_SESSION['CREATE']['VIEWSTATE']) ? htmlspecialchars($_SESSION['CREATE']['VIEWSTATE']['gIntro']) : '')).'" maxLength="50" size="100" style="border: rgb(200, 200, 200) 1px solid"
                                                    <br>
                                                </td>
                                            </tr>
                                            <tr>
                                                <td width="400">
                                                    <div style="font-size: 12px; color: #333333">
                                                        ※ Limited to 50 characters. Can be changed after club creation
                                                    </div>
                                                </td>
                                            </tr>
                                        </table>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>';

            echo '  <tr>
                        <td>
                            <p align="center">
                                &nbsp;
                                <input type="image" name="ctl02" align="absMiddle" alt="Voltar" src="img/guild2_04.gif" style="border-width: 0px">
                                &nbsp;&nbsp;
                                <input type="image" name="ctl03" align="absMiddle" alt="Próximo" src="img/guild2_03.gif" style="border-width: 0px">
                            </p>
                        </td>
                    </tr>';

            // Fecha table
            echo '</table>';

            // Fecha form
            echo '</form>';

        }

        private function pageMark() {

            // Message Java Script
            if (isset($_SESSION['CREATE']['msg'])) {

                echo '  <script>
                            alert(\''.$_SESSION['CREATE']['msg'].'\');
                        </script>';

                // Já mostrou a mensagem limpa o SESSION msg
                if (isset($_SESSION['CREATE']['msg']))
                    unset($_SESSION['CREATE']['msg']);
            }

            echo '<form id="ctl00" method="post" enctype="multipart/form-data" action="./guild_main.php">';

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';

            // Title
            echo '  <tr>
                        <td align="center">
                            <img src="img/s_title01.gif" width="608" height="31">
                        </td>
                    </tr>';

            // Text
            echo '  <tr>
                        <td align="center">
                            <table cellspacing="0" cellpadding="5" width="600" border="0">
                                <tr>
                                    <td>
                                        <img src="img/regstertext01.gif">
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>';

            // Principal
            echo '  <tr>
                        <td style="padding-bottom: 10px; padding-left: 20px" height="31" width="600" vAlign="top">
                            <table cellspacing="5" cellpadding="6" width="590" border="0">
                                <tr>
                                    <td style="border: silver 1px solid; padding: 10px" bgColor="#f7f7f7">
                                        <table cellspacing="1" cellpadding="6" width="570" bgColor="#f7f7f7" border="0">
                                            <tr bgColor="#ffffff">
                                                <td vAlign="middle" width="160" align="right">
                                                    <b>
                                                        <span style="font-size: 9pt">
                                                            <font color="gray" face="MS Gothic">
                                                                File :&nbsp;
                                                            </font>
                                                        </span>
                                                    </b>
                                                </td>
                                                <td>
                                                    &nbsp;
                                                    <input id="up_file" type="file" name="up_File" style="border: rgb(231, 203, 219) 1px solid">
                                                    &nbsp;
                                                    &nbsp;
                                                    <input type="submit" name="ctl01" value="File upload" style="border: rgb(231, 203, 219) 1px solid">
                                                </td>
                                            </tr>
                                            <tr bgColor="#ffffff">
                                                <td>
                                                    <b>
                                                        <span style="font-size: 9pt">
                                                            <font color="gray" face="MS Gothic">
                                                                &nbsp;
                                                                Preview:
                                                            </font>
                                                        </span>
                                                    </b>
                                                </td>
                                                <td>
                                                    <table cellspacing="0" cellpadding="0" border="0">
                                                        <tr>
                                                            <td vAlign="top">
                                                                <table height="127" cellspacing="0" cellpadding="0" width="127" background="img/preview01.gif">
                                                                    <tr>
                                                                        <td colspan="2">
                                                                            &nbsp;
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td height="72" rowspan="2" width="70">
                                                                            &nbsp;
                                                                        </td>
                                                                        <td heigh="41" vAlign="bottom" width="57">
                                                                            <img width="22" height="20" border="0" src="'.((isset($_SESSION['CREATE']['mark'])) ? '../../_Files/guild/mark/preview/'.$_SESSION['CREATE']['mark'].'?dummy='.rand(15656818, 457556515151) : '../../_Files/guild/mark/sample.png').'">
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td height="36" width="57">
                                                                            &nbsp;
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td width="70">
                                                                            &nbsp;
                                                                        </td>
                                                                        <td width="57">
                                                                            &nbsp;
                                                                        </td>
                                                                    </tr>
                                                                </table>
                                                            </td>
                                                            <td width="10">
                                                                &nbsp;
                                                            </td>
                                                            <td>
                                                                <table height="127" cellspacing="0" cellpadding="0" width="305" border="0">
                                                                    <tr>
                                                                        <td vAlign="top" background="img/preview02.gif">
                                                                            <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                                                                <tr>
                                                                                    <td height="5" width="48%">
                                                                                    </td>
                                                                                    <td height="5" width="20%">
                                                                                    </td>
                                                                                    <td height="5" width="34%">
                                                                                    </td>
                                                                                </tr>
                                                                                <tr>
                                                                                    <td height="21" width="47%">
                                                                                        &nbsp;
                                                                                    </td>
                                                                                    <td height="21" width="20%">
                                                                                        <img width="22" height="20" border="0" src="'.((isset($_SESSION['CREATE']['mark'])) ? '../../_Files/guild/mark/preview/'.$_SESSION['CREATE']['mark'].'?dummy='.rand(15656818, 457556515151) : '../../_Files/guild/mark/sample.png').'">
                                                                                    </td>
                                                                                    <td height="21" width="34%">
                                                                                        &nbsp;
                                                                                    </td>
                                                                                </tr>
                                                                                <tr>
                                                                                    <td width="46%">
                                                                                        &nbsp;
                                                                                    </td>
                                                                                    <td width="20%">
                                                                                        &nbsp;
                                                                                    </td>
                                                                                    <td width="34%">
                                                                                        &nbsp;
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
                    </tr>';
            
            // Botões Prev - Next
            echo '  <tr>
                        <td>
                            <p align="center">
                                &nbsp;
                                <input type="image" name="ctl02" alt="Volta" align="absMiddle" src="img/guild2_04.gif" style="border-width: 0px">
                                &nbsp;
                                &nbsp;
                                <input type="image" name="ctl03" alt="Registrar" align="absMiddle" src="img/guild2_05.gif" style="border-width: 0px">
                            </p>
                        </td>
                    </tr>';

            // Fecha table
            echo '</table>';

            // Fecha form
            echo '</form>';

        }

        private function pageFinal() {

            echo '<table width="642" height="461" cellspacing="0" cellpadding="0" border="0">
                    <tr>
                        <td width="642" heigh="461" vAlign="middle" algin="center" bgColor="#ffffff">';

            echo '<table width="100%" cellspacing="0" cellpadding="0" border="0">';

            echo '  <tr>
                        <td height="86">
                            <div style="font-size: 12px; color: #333333" align="center">
                                Club registration is complete.
                                <br>
                                <br>
                                The club mark in the game is displayed after receiving club approval.
                                <br>
                                <br>
                                (Club approval requires at least 1 club members.)
                                <br>
                                <br>
                                <a href="'.LINKS['MNL_LIST_ALL'].'">
                                    <img border="0" src="img/btn_list.gif">
                                </a>
                            </div>
                        </td>
                    </tr>';

            // Fecha table
            echo '</table>';

            // Table 2
            echo '      </td>
                    </tr>
                </table>';
        }

        private function content() {

            // Verifica os post controle
            $this->checkPost();

            if (isset($_SESSION['CREATE'])) {

                switch ($_SESSION['CREATE']['page']) {
                    case 0:
                        $this->pageAgreement();
                        break;
                    case 1:
                        $this->pageNameIntro();
                        break;
                    case 2:
                        $this->pageMark();
                        break;
                    case 3:
                        $this->pageFinal();
                        break;
                    default:
                        echo '<h1>Error: Page Invalid<h1>';
                        break;
                }

            }else
                echo '<h1>Error: Session invalid</h1>';
        }
    }

    // Guild Create
    $create = new GuildCreate();

    $create->show();
?>