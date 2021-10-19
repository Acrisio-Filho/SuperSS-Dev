<?php
    // Arquivo design.inc
    // Criado em 23/05/2020 as 19:59 por Acrisio
    // Definição e Implementação da classe Static Design

    include_once('player_singleton.inc');
    include_once('config.inc');
    include_once('debug_log.inc');

    // tem que ser sem o https, só a pagina gacha_whats.php, por que se colocar o http o link todo ele reseta a session
    const LINKS = [ 'LOGIN_ERROR' => BASE_GACHA_URL.'unknown_error.html',
                    'CURRENT_INDEX' => BASE_GACHA_URL.'current/gacha_item.php',
                    'LIST_ITEM' => /*BASE_GACHA_URL.*/'list_item.php',
                    'GACHA_WHATS' => /*BASE_GACHA_URL.*/'gacha_whats.php',
                    'GACHA_LOTTERY' => /*BASE_GACHA_URL.*/'gacha_lottery.php',
                    'UNKNOWN_ERROR' => BASE_GACHA_URL.'unknown_error.html'];

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

            echo '<script type="text/javascript" src="./include/jquery-1.8.2.min.js"></script>';
            echo '<script type="text/javascript" src="./include/jquery-ui-1.9.1.custom.min.js"></script>';
            echo '<script type="text/javascript" src="./include/Stop_KeyFunction.js"></script>';
            echo '<script type="text/javascript" src="./include/gacha_common.js"></script>';
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

            echo '<div style="position:absolute; left:0px; top:0px; width:800px; height:555px; background:url(img/gacha_bg.png);">';

            self::menuLeft();
            self::top();
        }

        static public function endBody() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</div>';
            echo '</div>';
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

        static public function menuLeft() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '<div style="position:absolute; left:0px; top:0px; width:190px; height:550px;">
                    <div style="position:absolute; left:0px; top:0px; width:190px; height:550px;">

                        <div style="position:absolute; left:20px; top:20px; width:160px; height:35px;">
                            <a href="'.LINKS['GACHA_WHATS'].'"><img src="img/btn_01.png" width="160" height="35" alt="" /></a>
                        </div>
                        <div style="position:absolute; left:20px; top:65px; width:160px; height:35px;">
                            <a href="'.LINKS['CURRENT_INDEX'].'" target="_blank"><img src="img/btn_02.png"
                                    width="160" height="35" alt="" /></a></div>
                        <div style="position:absolute; left:20px; top:110px; width:160px; height:35px;">
                            <a href="'.LINKS['LIST_ITEM'].'"><img src="img/btn_03.png" width="160" height="35" alt="" /></a>
                        </div>

                        <div class="ticket_count"
                            style="position:absolute; left:100px; top:292px; width:72px; height:17px;">
                            <div id="_NowTicket" class="StatusDIV4">'.PlayerSingleton::getInstance()['TICKET'].'</div>
                        </div>
                        <div class="ticket_count"
                            style="position:absolute; left:100px; top:323px; width:72px; height:17px;">
                            <div id="_NowSubTicket" class="StatusDIV4">'.PlayerSingleton::getInstance()['TICKET_SUB'].'</div>
                        </div>
                        <div class="ticket_count"
                            style="position:absolute; left:100px; top:353px; width:72px; height:17px;">
                            <div id="_RestPlay" class="StatusDIV3">'.PlayerSingleton::getTotalTicketToPlay(PlayerSingleton::getInstance()).'</div>
                        </div>

                        <div style="position:absolute; left:14px; top:380px; width:172px; height:78px;">
                            <input type="image" src="img/btn_12.png" width="172" height="78" alt="" value=""
                                '.(PlayerSingleton::checkPlayerHaveTicketToPlay(PLAY_MODO::PM_TEN) ? 'onclick="javascript:dialogOpen(10); return false;"' : '').' /></div>

                        <div style="position:absolute; left:14px; top:458px; width:172px; height:78px;">
                            <input type="image" src="img/btn_04.png" width="172" height="78" alt="" value=""
                                '.(PlayerSingleton::checkPlayerHaveTicketToPlay(PLAY_MODO::PM_ONE) ? 'onclick="javascript:dialogOpen(1); return false;"' : '').' /></div>

                        <div id="dialogWindow"
                            style="position:fixed; width:400px; height:300px; top:150px; left:200px; z-index:50; display:none;">
                            <div
                                style="position:absolute; left:0px; top:0px; width:400px; height:300px; background:url(img/popup/bg.png) no-repeat; ">
                                <div id="Msg_gachaPlay" align="center"
                                    style="position:absolute; left:30px; top:46px; width:341px; height:58px; display:table-cell; vertical-align:middle; font-size:small; font-family: Arial Black;">
                                </div>
                                <div style="position:absolute; left:0px; top:236px; width:200px; height:48px;">
                                    <input id="Btn_gachaPlay" type="image" src="img/popup/pop_06.png" width="200"
                                        height="48" alt="" value="" style="overflow:hidden; outline:none;"
                                        onclick="javascript:goGacha();" />
                                </div>
                                <div style="position:absolute; left:200px; top:236px; width:200px; height:48px;">
                                    <input id="Btn_gachaNonPlay" type="image" src="img/popup/pop_07.png" width="200"
                                        height="48" alt="" value="" style="overflow:hidden; outline:none;"
                                        onclick="javascript:dialogClose();" />
                                </div>
                            </div>
                        </div>
                    </div>
                    
                </div>';
        }

        static public function top() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '<div style="position:absolute; left:190px; top:0px; width:610px; height:100px;"><img
                        src="img/gacha_title03.png" width="610" height="100" />
                </div>';
        }
    }
?>