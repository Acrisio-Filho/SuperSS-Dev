<?php
    // Arquivo design.inc
    // Criado em 28/06/2020 as 14:36 por Acrisio
    // Definição e Implementação da classe Static Design

    include_once('player_singleton.inc');
    include_once($_SERVER['DOCUMENT_ROOT'].'/EntryPoint/config/config.inc');
    include_once($_SERVER['DOCUMENT_ROOT'].'/EntryPoint/util/logger.inc');

    // LINKS
    const LINKS = [ 'LOGIN_ERROR' => BASE_ETP_URL.'unknown_error.html',
                    'UNKNOWN_ERROR' => BASE_ETP_URL.'unknown_error.html'];

    class Design {

        static public function checkIE() {
            
            if (stristr($_SERVER['HTTP_USER_AGENT'], 'MSIE 7.0;') == null 
                || stristr($_SERVER['HTTP_USER_AGENT'], 'compatible;') == null
                || !preg_match("/Trident\/[345678]\.[01];/", $_SERVER['HTTP_USER_AGENT'])) {

                sLogger::getInstance()->putLog("[Error] Bad navigator request. (".$_SERVER['HTTP_USER_AGENT'].")", ENTRYPOINT);

                // Redireciona para a página de erro
                header("Location: ".$_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/pangya.php');

                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        static public function checkLogin() {

            // Verifica se o player esta conectado
            if (PlayerSingleton::getInstance() == null || !PlayerSingleton::getInstance()['logged']) {
                
                sLogger::getInstance()->putLog("[Error] Not have session, request page without login.", ENTRYPOINT);

                // Redireciona para a página de erro
                header("Location: ".LINKS['LOGIN_ERROR']);

                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        static public function beginHTML() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();
            
            echo '<!DOCTYPE html>';
            echo '<html lang="pt-br">';
        }

        static public function endHTML() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</html>';
        }

        static public function beginHead() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '<head>';

            echo '<meta content="IE=7.0000" http-equiv="X-UA-Compatible">';

            echo '<meta charset="UTF-8">';

            echo '<script type="text/javascript" src="./include/Stop_KeyFunction.js"></script>';
            echo '<link rel="stylesheet" type="text/css" href="./include/main.css'.(
                // Debug
                (DEBUG_ETP) ? '?'.rand() : ''
            ).'" media="screen" />';
        }

        static public function endHead() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</head>';
        }

        static public function beginBody() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '<body style="background-color:#FFFFFF;">';

            // Background
            echo '<table width="800" height="553" cellspacing="0" cellpadding="0" border="0" background="img/bg-etp.jpg">
                    <tr>
                        <td>';

            // Top bar older color #0bac76

            // Content Design
            echo '<table width="800" height="553" cellspacing="0" cellpadding="0" border="0">
                    <tr>
                        <td height="30" vAlign="top">
                            <table width="100%" height="100%" cellspacing="0" cellpadding="0" border="0" class="backColor">
                                <tr>
                                    <td align="left">
                                        <div style="margin-left: 10px">
                                            <font size="4em" color="goldenrod" weight="bold">Nickname: </font>
                                            <font size="4em" color="greenyellow" weight="bold">'.PlayerSingleton::getInstance()['NICKNAME'].'</font>
                                        </div>
                                    </td>
                                    <td align="right">
                                        <div style=" margin-right: 10px;">
                                            <font size="4em" color="goldenrod" weight="bold">Cookie Point: </font>
                                            <font size="4em" color="greenyellow" weight="bold">'.PlayerSingleton::getInstance()['COOKIE'].'</font>
                                        </div>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                    <tr>
                        <td height="30"></td>
                    </tr>
                    <tr>
                        <td>
                            <table width="100%" height="493" cellspacing="0" cellpadding="0" border="0">
                                <tr>
                                    <td width="150">
                                        <table align="center" width="150" height="400" cellspacing="0" cellpadding="0" border="0" class="menu">
                                            <tr>
                                                <td>
                                                    <table style="margin-left: 5px">
                                                        <tr>
                                                            <td height="30"><a class="menuItem" href="./etp.php">About</a></td>
                                                        </tr>
                                                        <tr>
                                                            <td height="30"><a class="menuItem" href="./donation_register.php">Donation register</a></td>
                                                        </tr>
                                                        <tr>
                                                            <td height="30"><a class="menuItem" href="./epin_exchange.php">E-Pin exchange</a></td>
                                                        </tr>
                                                    </table>
                                                </td>
                                            </tr>
                                        </table>
                                    </td>
                                    <td width="30"></td>
                                    <td>
                                        <table width="100%" height="400" cellspacing="0" cellpadding="0" border="0">
                                            <tr>
                                                <td width="100%" height="100%" align="center" class="backColor content">
                                                    <table width="100%" height="100%" cellspacing="0" cellpadding="0" border="0">
                                                        <tr>
                                                            <td vAlign="top" align="center">
                                                                <div class="divContent">';
        }

        static public function endBody() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '                                              </div>
                                                            </td>
                                                        </tr>
                                                    </table>
                                                </td>
                                            </tr>
                                        </table>
                                    </td>
                                    <td width="30"></td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                </table>';

            echo '      </td>
                    </tr>';

            echo '</body>';
        }

        static public function beginContent() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();
        }

        static public function endContent() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();
        }
    }
?>