<?php
    // Arquivo design.inc
    // Criado em 28/06/2020 as 14:36 por Acrisio
    // Definição e Implementação da classe Static Design

    include_once('player_singleton.inc');
    include_once('config.inc');
    include_once('debug_log.inc');

    // tem que ser sem o https, só a pagina gacha_whats.php, por que se colocar o http o link todo ele reseta a session
    const LINKS = [ 'LOGIN_ERROR' => BASE_EVENT_URL.'unknown_error.html',
                    'UNKNOWN_ERROR' => BASE_EVENT_URL.'unknown_error.html'];

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

        static public function checkLogin() {

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

            echo '<meta content="IE=7.0000" http-equiv="X-UA-Compatible">';

            echo '<meta charset="UTF-8">';

            echo '<script type="text/javascript" src="./include/Stop_KeyFunction.js"></script>';
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

            echo '<img src="../img/event_27062020.jpg">';

            echo '<span class="pontos">'.PlayerSingleton::getInstance()['PONTOS'].'</span>';
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