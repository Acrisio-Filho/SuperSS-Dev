<?php
    // Arquivo design.inc
    // Criado em 16/07/2019 as 12:12 por Acrisio
    // Definição e Implementação da classe do Design

    include_once("../source/player_singleton.inc");

    include_once("../source/config.inc");

    include_once("source/guild_singleton.inc");

    // Links
    const LINKS =   [ 'MNL_MEMBER_LIST' => BASE_GUILD_URL.'guild_home/club_member_list.php',
                        'MNL_NOTICE' => BASE_GUILD_URL.'guild_home/club_notice.php',
                        'MNL_PRIVATE_BBS_LIST' => BASE_GUILD_URL.'guild_home/club_private_bbs_list.php',
                        'MNL_MATCH_LIST' => BASE_GUILD_URL.'guild_home/club_match_result.php',
                        'MNL_MYINFO' => BASE_GUILD_URL.'guild_home/club_myinfo.php',
                        'MNL_EDIT_HOME' => BASE_GUILD_URL.'guild_home/club_edit_home.php',
                        'MN_EDIT_MAIN' => BASE_GUILD_URL.'guild_home/club_edit_main.php',
                        'MN_MEMBER_JOIN' => BASE_GUILD_URL.'guild_home/club_member_join.php',
                        'MN_MEMBER_ADM' => BASE_GUILD_URL.'guild_home/club_member_admin.php',
                        'MN_MOVE_MASTER' => BASE_GUILD_URL.'guild_home/club_move_master.php',
                        'MN_CLOSED' => BASE_GUILD_URL.'guild_home/club_closed.php',
                        'MNL_JOIN'  => BASE_GUILD_URL.'guild_home/club_prejoin_agree.php',
                        'MNL_GUILD_HOME' => BASE_GUILD_URL.'guild_home/index.php',
                        'TOP_INDEX' => BASE_GUILD_URL.'guild_new/index.php',
                        'LOGIN_ERROR' => BASE_GUILD_URL.'unknown_error.html',
                        'GUILD_ERROR' => BASE_GUILD_URL.'unknown_error.html'
                    ];

    class Design {

        static private function checkAll() {

            self::checkLogin();
            self::checkGuild();
        }

        static private function checkLogin() {

            // Verifica se o player esta conectado
            if (PlayerSingleton::getInstance() == null  || !PlayerSingleton::getInstance()['logged']) {
                
                // Redireciona para a página de erro
                header("Location: ".LINKS['LOGIN_ERROR']);
                
                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        static private function checkGuild() {

            // Verifica se a guild existe
            if (GuildSingleton::getInstance() == null || !GuildSingleton::getInstance()['exists']) {
                header("Location: ".LINKS['GUILD_ERROR']);
            }
        }

        static public function beginHTML() {

            // Verifica valores GET e POST
            self::checkAll();
            
            //echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 2.0 Transitional//EN">';
            echo '<!DOCTYPE html>';
            echo '<html lang="pt-br">';
        }

        static public function endHTML() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '</html>';
        }

        static public function beginHead() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '<head>';

            echo '<meta content="IE=5.0000" http-equiv="X-UA-Compatible">';

            echo '<meta charset="UTF-8">';

            echo '<meta content="no-cache" http-equiv="pragma">';
            echo '<meta content="no-store" http-equiv="cache-control">';
            echo '<meta content="no-cache" http-equiv="cache-control">';

            echo '<link rel="stylesheet" href="include/pangya.css">';

            echo '<script language="javascript" src="../include/Stop_KeyFunction.js"></script>';
        }

        static public function endHead() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '</head>';
        }

        static public function beginBody() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '<body>';

            // Abre a TAG da tabela principal
            echo '<table id="Table_01" width="800" height="553" cellspacing="0" cellpadding="0" border="0">';

            echo '<tr>';

            self::menuLeft();
            self::menuTop();

            echo '</tr>';
        }

        static public function endBody() {

            // Verifica valores GET e POST
            self::checkAll();

            // Fecha a TAG da tabela princiapl
            echo '</table>';

            echo '</body>';
        }

        static public function beginContent() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '  <tr>
                        <td width="642" valign="top" height="461" bgcolor="#ffffff" align="center">
                            <div width="642" border="0">';
        }

        static public function endContent() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '          </div>
                        </td>
                    <tr>';
        }

        static public function menuEdit() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '  <table width="100%" valign="top" height="20" cellspacing="0" cellpadding="0" border="0" bgcolor="#ffffff" align="center">
                        <tr>
                            <td height="20" vAlign="middle" align="center">
                                <div style="FONT-SIZE: 12px; COLOR: #666666; display: block" align="center">
                                    &lt;';
            
            // Só o Master e Submaster pode mexer aqui
            if (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] > 0 && PlayerSingleton::getInstance()['GUILD_UID'] == GuildSingleton::getInstance()['UID']) {

                if (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 1/*MASTER*/) {
                
                    echo '              <a style="COLOR: #cc6600; display: inline" href="'.LINKS['MNL_EDIT_HOME'].'">Home edit</a>　|　
                                        <a style="COLOR: #cc6600; display: inline" href="'.LINKS['MN_EDIT_MAIN'].'">Basic setting</a>　|　
                                        <a style="COLOR: #cc6600; display: inline" href="'.LINKS['MN_MEMBER_JOIN'].'">Accept member</a>　|　
                                        <a style="COLOR: #cc6600; display: inline" href="'.LINKS['MN_MEMBER_ADM'].'">Authority change</a>　|　
                                        <a style="COLOR: #cc6600; display: inline" href="'.LINKS['MN_MOVE_MASTER'].'">Master authority delegation</a>　|　
                                        <a style="COLOR: #cc6600; display: inline" href="'.LINKS['MN_CLOSED'].'">Club closed</a>　';
                
                }else if (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 2/*SUBMASTER*/) {

                    echo '              <a style="COLOR: #cc6600; display: inline" href="'.LINKS['MN_MEMBER_JOIN'].'">Accept member</a>';
                }
            }
            
            echo '                  &gt;
                                </div>
                            </td>
                        </tr>
                    </table>';
        }

        static private function menuLeft() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '  <td rowspan="2" width="158" height="553">
                        <table width="158" height="553" cellspacing="0" cellpadding="0" border="0" bgcolor="#ffffff" align="center">
                        <tr>
                            <td colspan="3">
                                <img alt="" src="img/leftmenu_01.gif" width="158" height="186">
                            </td>
                        </tr>
                        <tr>
                            <td rowspan="10">
                                <img alt="" src="img/leftmenu_02.gif" width="14" height="367">
                            </td>
                            <td height="110" width="125">
                                <table cellspacing="0" cellpadding="0" border="0">
                                    <tr>
                                        <td>
                                            <a href="'.LINKS['MNL_MEMBER_LIST'].'">
                                                <img alt="" src="img/leftmenu_homemenu01.gif" width="125" height="22" border="0">
                                            </a>
                                        </td>
                                    </tr>';

            // GM pode ver as noticias de outra guild, só os real membros podem ver os notices
            if ((PlayerSingleton::getInstance()['CAP'] & 4/*GM*/) || 
                (PlayerSingleton::getInstance()['GUILD_UID'] > 0 && PlayerSingleton::getInstance()['GUILD_UID'] == GuildSingleton::getInstance()['UID'] 
                    && (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] > 0 && PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] < 9))) {
                
                echo '              <tr>
                                        <td>
                                            <a href="'.LINKS['MNL_NOTICE'].'">
                                                <img alt="" src="img/leftmenu_homemenu02.gif" width="125" height="22" border="0">
                                            </a>
                                        </td>
                                    </tr>';
            }

            echo '                  <tr>
                                        <td>
                                            <a href="'.LINKS['MNL_PRIVATE_BBS_LIST'].'">
                                                <img alt="" src="img/leftmenu_homemenu03.gif" width="125" height="22" border="0">
                                            </a>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <a href="'.LINKS['MNL_MATCH_LIST'].'">
                                                <img alt="" src="img/leftmenu_homemenu04.gif" width="125" height="22" border="0">
                                            </a>
                                        </td>
                                    </tr>';

            if (PlayerSingleton::getInstance()['GUILD_UID'] > 0 && PlayerSingleton::getInstance()['GUILD_UID'] == GuildSingleton::getInstance()['UID']) {
                
                echo '              <tr>
                                        <td>
                                            <a href="'.LINKS['MNL_MYINFO'].'">
                                                <img alt="" src="img/leftmenu_homemenu05.gif" width="125" height="22" border="0">
                                            </a>
                                        </td>
                                    </tr>';
            }

            echo '              </table>
                            </td>
                            <td rowspan="10">
                                <img alt="" src="img/leftmenu_04.gif" width="19" height="367">
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <img alt="" src="img/leftmenu_09.gif" width="125" height="18">
                            </td>
                        </tr>';
                        
            if (PlayerSingleton::getInstance()['GUILD_UID'] <= 0 && GuildSingleton::getInstance()['PERMITION_JOIN'] == 1 
                && PlayerSingleton::getInstance()['LEVEL'] >= GuildSingleton::getInstance()['CONDITION_LEVEL']
                && (GuildSingleton::getInstance()['STATE'] == ST_APPROVAL_PEDNDING || GuildSingleton::getInstance()['STATE'] == ST_NOW_OPEN)) {

                echo '  <tr>
                            <td>
                                <a href="'.LINKS['MNL_JOIN'].'">
                                    <img alt="" src="img/leftmenu_prejoin.gif" width="125" height="25" border="0">
                                </a>
                            </td>
                        </tr>';
                
            }else if (PlayerSingleton::getInstance()['GUILD_UID'] == GuildSingleton::getInstance()['UID'] 
                && (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 2/*SUBMASTER*/ || 
                    (PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 1/*Master*/ && PlayerSingleton::getInstance()['UID'] == GuildSingleton::getInstance()['MASTER']['UID']))) {

                // Master ou Submaster
                echo '  <tr>
                            <td>
                                <a href="'.(PlayerSingleton::getInstance()['MEMBER_STATE_FLAG'] == 2/*SUBMASTER*/ ? LINKS['MN_MEMBER_JOIN'] : LINKS['MNL_EDIT_HOME']).'">
                                    <img alt="" src="img/leftmenu_master.gif" width="125" height="25" border="0">
                                </a>
                            </td>
                        </tr>';

            }else {

                echo '  <tr>
                            <td>
                                <img alt="" src="img/leftmenu_other.gif" width="125" height="25" border="0">
                            </td>
                        </tr>';
            }

            echo '      <tr>
                            <td>
                                <img alt="" src="img/leftmenu_11.gif" width="125" height="15">
                            </td>
                        </tr>
                        <tr>
                            <td width="125" height="85" align="center">
                                <table width="109" height="82" cellspacing="0" cellpadding="0" border="0" align="center">
                                <tr>
                                    <td valign="middle">
                                        <div style="FONT-SIZE: 12px; COLOR: #856c55" align="center">Welcome&nbsp;<br> </div>
                                        <div style="FONT-WEIGHT: bold; COLOR: #856c55" align="center">'.htmlspecialchars(PlayerSingleton::getInstance()['NICKNAME']).' !</div>
                                    </td>
                                </tr>';

            // Link MyClub (My Guild)
            if (PlayerSingleton::getInstance()['GUILD_UID'] > 0) {
                
                echo '          <tr>
                                    <td valign="middle">
                                        <a href="'.LINKS['MNL_GUILD_HOME'].'?id='.PlayerSingleton::getInstance()['GUILD_UID'].'">
                                            <img src="img/leftmenu_myclub.gif" width="109" height="26" border="0">
                                        </a> 
                                    </td>
                                </tr>';
            }

            echo '              </table>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <img alt="" src="img/leftmenu_13.gif" width="125" height="114">
                            </td>
                        </tr>
                        </table>
                    </td>';

        }

        static private function menuTop() {

            // Verifica valores GET e POST
            self::checkAll();

            echo '  <td width="642" height="85">
                        <table id="Table_01" width="642" height="86" cellspacing="0" cellpadding="0" border="0" style="position: relative; top: -1px">
                            <tr>
                                <td colspan="2" width="642" height="50" valign="bottom" background="img/topmenu2_01.gif">
                                    <table width="537" border="0">
                                        <tr>
                                            <td width="58"></td>
                                            <td width="579">
                                                <!-- Nome Guild -->
                                                <div 
                                                    style="FONT-SIZE: 20px; FONT-WEIGHT: bold; COLOR: #ffffff">
                                                    '.htmlspecialchars(GuildSingleton::getInstance()['NAME']).'
                                                </div>
                                            </td>
                                        </tr>
                                    </table>
                                </td>
                            </tr>
                            <tr>
                                <td height="27" width="537" background="img/topmenu2_02.gif">
                                    <div align="center">
                                        <table width="537">
                                            <tr>
                                                <td width="36"></td>
                                                <td width="200">
                                                    <!-- Guild Master (Nickname) -->
                                                    <div style="COLOR: #ffffff">CLUB MASTER：'.htmlspecialchars(GuildSingleton::getInstance()['MASTER']['NICKNAME']).'</div>
                                                </td>
                                                <td width="115">
                                                    <!-- Número de membros -->
                                                    <div style="COLOR: #ffffff">MEMBERS：'.GuildSingleton::getInstance()['MEMBER_NUM'].'</div>
                                                </td>
                                                <td width="150">
                                                    <!-- Data de criação -->
                                                    <div style="COLOR: #ffffff">CREATION：'.GuildSingleton::getInstance()['DATE'].'</div>
                                                </td>
                                                <td width="36"></td>
                                            </tr>
                                        </table>
                                    </div>
                                </td>
                                <td>
                                    <a href="'.LINKS['TOP_INDEX'].'">
                                        <img alt="" src="img/topmenu2_03.gif" width="105" height="27" border="0">
                                    </a>
                                </td>
                            </tr>
                            <tr>
                                <td colspan="2">
                                    <img alt="" src="img/topmenu_04.gif" width="642" height="9">
                                </td>
                            </tr>
                        </table>
                    </td>';
        }
    }

?>