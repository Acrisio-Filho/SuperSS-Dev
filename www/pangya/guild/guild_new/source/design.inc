<?php
    // Arquivo design.inc
    // Criado em 16/07/2019 as 06:20 por Acrisio
    // Definição e Implementação da classe do Design

    include_once("../source/player_singleton.inc");

    include_once("../source/config.inc");

    const LINKS = [ 'MNL_LIST_ALL' => BASE_GUILD_URL.'guild_new/Club_New_list_all.php',
                    'MNL_BBS_LIST' => BASE_GUILD_URL.'guild_new/Club_New_bbs_list.php',
                    'MNL_RANKING' => BASE_GUILD_URL.'guild_new/Club_New_Ranking.php',
                    'MNL_FAQ' => BASE_GUILD_URL.'guild_new/Club_New_FAQ.php',
                    'MNL_AGREEMENT' => BASE_GUILD_URL.'guild_new/Club_Agreement.php',
                    'MNL_CREATE' => BASE_GUILD_URL.'guild_new/guild_main.php',
                    'MNL_GUILD_HOME' => BASE_GUILD_URL.'guild_home/index.php',
                    'TOP_INDEX' => BASE_GUILD_URL.'guild_new/index.php',
                    'LOGIN_ERROR' => BASE_GUILD_URL.'unknown_error.html',
                    'UNKNOWN_ERROR' => BASE_GUILD_URL.'unknown_error.html'];

    class Design {

        static private function checkLogin() {

            // Verifica se o player esta conectado
            if (PlayerSingleton::getInstance() == null  || !PlayerSingleton::getInstance()['logged'] ) {
                
                // Redireciona para a página de erro
                header("Location: ".LINKS['LOGIN_ERROR']);

                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        static public function beginHTML() {

            // Verifica se o player esta conectado
            self::checkLogin();
            
            //echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">';
            echo '<!DOCTYPE html>';
            echo '<html lang="pt-br">';
        }

        static public function endHTML() {

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</html>';
        }

        static public function beginHead() {

            // Verifica se o player esta conectado
            self::checkLogin();

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

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</head>';
        }

        static public function beginBody() {

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '<body>';

            // Abre a TAG da tabela principal
            echo '<table id="Table_01" width="800" height="553" cellspacing="0" cellpadding="0" border="0">';

            echo '<tr>';

            self::menuLeft();
            self::menuTop();

            echo '</tr>';
        }

        static public function endBody() {

            // Verifica se o player esta conectado
            self::checkLogin();

            // Fecha a TAG da tabela princiapl
            echo '</table>';

            echo '</body>';
        }

        static public function beginContent() {

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '  <tr>
                        <td width="642" valign="top" height="461" bgcolor="#ffffff" align="center">
                            <div width="642" border="0">';
        }

        static public function endContent() {

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '          </div>
                        </td>
                    <tr>';
        }

        static private function menuLeft() {

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '  <td rowspan="2" width="158" height="553">
                        <table width="158" height="553" cellspacing="0" cellpadding="0" border="0" bgcolor="#ffffff" align="center">
                        <tr>
                            <td colspan="3">
                                <img alt="" src="img/leftmenu_01.gif" width="158" height="183">
                            </td>
                        </tr>
                        <tr>
                            <td rowspan="10">
                                <img alt="" src="img/leftmenu_02.gif" width="14" height="370">
                            </td>
                            <td>
                                <a href="'.LINKS['MNL_LIST_ALL'].'">
                                    <img alt="" src="img/leftmenu_03.gif" width="125" height="23" border="0">
                                </a>
                            </td>
                            <td rowspan="10">
                                <img alt="" src="img/leftmenu_04.gif" width="19" height="370">
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <a href="'.LINKS['MNL_BBS_LIST'].'">
                                    <img alt="" src="img/leftmenu_05.gif" width="125" height="23" border="0">
                                </a>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <a href="'.LINKS['MNL_RANKING'].'">
                                    <img alt="" src="img/leftmenu_06.gif" width="125" height="23" border="0">
                                </a>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <a href="'.LINKS['MNL_FAQ'].'">
                                    <img alt="" src="img/leftmenu_07.gif" width="125" height="24" border="0">
                                </a>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <a href="'.LINKS['MNL_AGREEMENT'].'">
                                    <img alt="" src="img/leftmenu_08.gif" width="125" height="23" border="0">
                                </a>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <img alt="" src="img/leftmenu_09.gif" width="125" height="15">
                            </td>
                        </tr>';
            
            // Só pode criar Guild se for Rookie E pra cima
            if (PlayerSingleton::getInstance()['GUILD_UID'] <= 0 && PlayerSingleton::getInstance()['LEVEL'] >= 1/*Rookie E*/) {

                echo '  <tr>
                            <td>
                                <a href="'.LINKS['MNL_CREATE'].'">
                                    <img alt="" src="img/leftmenu_10_a.gif" width="125" height="25" border="0">
                                </a>
                            </td>
                        </tr>';
                
            }else {

                echo '  <tr>
                            <td>
                                <img alt="" src="img/leftmenu_10_b.gif" width="125" height="25" border="0">
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
                                        <div style="FONT-SIZE: 12px; COLOR: #856c55" align="center">Welcome,&nbsp;<br> </div>
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

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '  <td width="642" height="85">
                        <table id="Table_01" width="642" height="86" cellspacing="0" cellpadding="0" border="0" style="position: relative; top: -1px">
                        <tbody>
                        <tr>
                            <td colspan="2">
                                    <img alt="" src="img/topmenu_01.gif" width="642" height="50">
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <img alt="" src="img/topmenu_02.gif" width="537" height="27">
                            </td>
                            <td>
                                <a href="'.LINKS['TOP_INDEX'].'">
                                    <img alt="" src="img/topmenu_03.gif" width="105" height="27" border="0">
                                </a>
                            </td>
                        </tr>
                        <tr>
                            <td colspan="2">
                                <img alt="" src="img/topmenu_04.gif" width="642" height="9">
                            </td>
                        </tr>
                        </tbody>
                        </table>
                    </td>';
        }
    }

?>