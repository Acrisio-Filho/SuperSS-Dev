<?php
    // Arquivo design.inc
    // Criado em 28/02/2020 as 06:54 por Acrisio
    // Definição e Implementação da classe Static Design

    include_once('player_singleton.inc');
    include_once('config.inc');
    include_once('debug_log.inc');

    const LINKS = [ 'LOGIN_ERROR' => BASE_LUCIA_ATTENDANCE_URL.'unknown_error.html',
                    'INDEX' => BASE_LUCIA_ATTENDANCE_URL.'index.php',
                    'REWARD' => BASE_LUCIA_ATTENDANCE_URL.'reward.php',
                    'REWARD_NOTICE' => BASE_LUCIA_ATTENDANCE_URL.'reward_notice.php',
                    'UNKNOWN_ERROR' => BASE_LUCIA_ATTENDANCE_URL.'unknown_error.html'];

    class Design {

        static public function checkIE() {
            
            if (stristr($_SERVER['HTTP_USER_AGENT'], 'MSIE 7.0;') == null 
		        || stristr($_SERVER['HTTP_USER_AGENT'], 'compatible;') == null
		        || !preg_match("/Trident\/[345678]\.[01];/", $_SERVER['HTTP_USER_AGENT'])) {

                DebugLog::Log("Bad navigator request. (".$_SERVER['HTTP_USER_AGENT'].")");

                // Redireciona para a página de erro
                header("Location: ".$_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/pangya.php');

                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        static private function checkLogin() {

            // Verifica se o player esta conectado
            if (PlayerSingleton::getInstance() == null || !PlayerSingleton::getInstance()['logged']) {
                
                DebugLog::Log("Not have session, request page without login.");

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

            echo '<meta content="IE=5.0000" http-equiv="X-UA-Compatible">';

            echo '<meta charset="UTF-8">';

            echo '<meta content="no-cache" http-equiv="pragma">';
            echo '<meta content="no-store" http-equiv="cache-control">';
            echo '<meta content="no-cache" http-equiv="cache-control">';

            echo '<link rel="stylesheet" href="include/pangya.css">';

            echo '<script language="javascript" src="include/Stop_KeyFunction.js"></script>';
            
            // New Loading
            echo '<script language="javascript" src="include/loading.js" defer="defer"></script>';
            
        }

        static public function endHead() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</head>';
        }

        static public function beginBody($onload) {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '<body '.($onload ? 'onload="init()"' : '').' background="img/bg.png" style="background-repeat: no-repeat; width: 800px; height: 553px">';

            // Loading
            self::loading();
        }

        static private function loading() {

            // Tela de loading
            echo '<img id="myImg" src="img/legenda.png" style="position:absolute; z-index:1; width:800px; height:553px; background-color: black">';
            echo '<table id="BlockLoading" width="800" height="553" border="0" padding="0" style="position:absolute; z-index:10;">
                    <tr>
                        <td width="50%" align="right" vAlign="middle">
                            <font id="LStrong" style="display:none; color:white; font-weight:bold; font-size: 30px;">
                                Loading
                            </font>
                        </td>
                        <td align="left" vAlign="middle">
                            <div id="LoadingAnim" style="display:inline; color:white; font-weight:bold; font-size: 30px; margin-left: 10px"></div>
                        </td>
                    </tr>
                </table>';
        }

        static public function endBody() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

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