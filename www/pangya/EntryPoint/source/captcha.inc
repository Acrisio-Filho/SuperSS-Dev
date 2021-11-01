<?php
    // Arquivo captcha.inc
    // Criado em 04/12/2020 as 16:54 por Acrisio
    // Definição e Implementação da classe Captcha

    include_once($_SERVER['DOCUMENT_ROOT'].'/EntryPoint/config/config.inc');

    define('CAPTCHA_SECRET', "captcha-secret");
    define('CAPTCHA_RESPONSE', "captcha-response");

    $GLOBALS['TABLE_SECRET'] = '0123456789abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';

    // Gera aleatóriamente um Alpha displyable
    function genChar() {

        srand(time());

        $GLOBALS['TABLE_SECRET'] = str_shuffle($GLOBALS['TABLE_SECRET']);

        srand(time());

        $choice = rand(0, strlen($GLOBALS['TABLE_SECRET']) - 1);

        return $GLOBALS['TABLE_SECRET'][$choice];
    }

    class Captcha {

        private static $enable = true;

        public static function isEnable() {
            return self::$enable;
        }

        // Verifica se o secreto já foi criado
        public static function hasSecretMaked() {
            return Captcha::getCaptchaSecret() !== null;
        }

        public static function getCaptchaSecret() {

            // initialize session
            if (!isset($_SESSION))
                session_start();

            if (!isset($_SESSION[CAPTCHA_SECRET]) || $_SESSION[CAPTCHA_SECRET] == '')
                return null;
            
            return $_SESSION[CAPTCHA_SECRET];
        }

        public static function clearSecret() {

            // initialize session
            if (!isset($_SESSION))
                session_start();

            if (isset($_SESSION[CAPTCHA_SECRET]))
                unset($_SESSION[CAPTCHA_SECRET]);
        }

        public static function checkPostCaptcha($post) {

            if (!self::$enable)
                return true;

            if (!isset($post) || !isset($post[CAPTCHA_RESPONSE]) || $post[CAPTCHA_RESPONSE] == '')
                return false;

            return true;
        }

        public static function checkCaptcha($post) {

            if (!self::$enable)
                return (Object)['success' => true, 'enable' => self::$enable];

            if (!Captcha::checkPostCaptcha($post)) {
                
                // Clear Secret
                Captcha::clearSecret();

                return null;
            }

            $reply = new Resposta($post[CAPTCHA_RESPONSE], '1.0');

            // Clear Secret
            Captcha::clearSecret();

            return $reply;
        }

        public static function loadScript() {

            if (!self::$enable)
                return '';

            return '<script type="text/javascript">
                        var on_time_click = false;

                        function checkCaptcha(form) {

                            captcha_input = document.getElementById(\'captcha-code-input\');
                            
                            if(!captcha_input || !captcha_input.value.match(/^[\\d\\w]{5}$/)) {
                                
                                alert(\'Please enter the CAPTCHA digits in the box provided\');
                                
                                captcha_input.focus();
                                
                                return false;
                            }

                            if (on_time_click === false) {

                                on_time_click = true;

                                return true;
                            }

                            return false;
                        }

                        function refreshCaptcha() {

                            document.getElementById(\'captcha\').src = \'/EntryPoint/captcha.php?\' + Math.random();
                            document.getElementById(\'captcha-code-input\').value = \'\';

                            return false;
                        }

                        function handlerInput(el) {
                            el.value = el.value.replace(/[^\d\w]+/g, \'\');
                        }
                    </script>
                    <style rel="StyleSheet" type="text/css">
                        .captcha-refresh {
                            text-decoration: none;
                            line-height: 41px;
                            height: 41px;
                            width: 30px;
                            display:inline-block;
                            background-color: #777;
                            text-align: center;
                            border: 2px solid darkgray;
                            margin-left: 3px;
                        }

                        .captcha-refresh small {
                            font-family: Lucida Sans Unicode
                            color: white;
                            font-size: 28px;
                            padding-top: 15px;
                        }

                        .captcha-refresh small:hover {
                            color: white;
                        }

                        #captcha-code-input {
                            width: 150px;
                            height: 39px;
                            line-height: 39px;
                            vertical-align: text-bottom;
                            font-size: 32px;
                            margin-left: 15px;
                        }

                        #captcha {
                            vertical-align: bottom;
                        }
                    </style>';
        } 

        public static function makeCheckBox() {

            if (!self::$enable)
                return '';

            //>
            return '
                <table align="center" cellspacing="0" cellpadding="0" cellcolapse="0" border="0" style="margin-top: 15px; margin-bottom: 10px; line-height: 100%">
                    <tr>
                        <td>
                            <img id="captcha" src="/EntryPoint/captcha.php" width="160" height="45" border="0" alt="CAPTCHA">
                        </td>
                        <td>
                            <a class="captcha-refresh" href="#" onclick="return refreshCaptcha()"><small>&#x27F3;</small></a>
                        </td vAlign="bottom">
                        <td>
                            <input id="captcha-code-input" class="input input-border" id="captcha_code_input" type="text" name="'.CAPTCHA_RESPONSE.'" size="6" maxlength="5" onkeyup="handlerInput(this)">
                        </td>
                    </tr>
                    <tr>
                        <td height="30" align="center" colspan="3">
                            <small>copy the digits and letters from the image into this box</small>
                        </td>
                    </tr>
                </table>
            ';
            //<?
        }

        public static function makeAttrEventOnSubmitCheck() {

            if (!self::$enable)
                return '';

            return 'onsubmit="return checkCaptcha(this)"';
        }

        // Make Captcha Secret
        public static function makeCaptchaSecret() {

            if (!self::$enable)
                return;

            // initialize session
            if (!isset($_SESSION))
                session_start();

            $secret = '';

            for ($i = 0; $i < 5; $i++)
                $secret .= genChar();

            $_SESSION[CAPTCHA_SECRET] = $secret;
        }

        // Make Image from Captcha Secret
        public static function makeImageCaptchaSecret() {

            if (!self::$enable)
                return null;

            // Make secret
            Captcha::makeCaptchaSecret();

            // pega secret gerada
            $secret = Captcha::getCaptchaSecret();

            if ($secret == null)
                return null;

            // Adapted for The Art of Web: www.the-art-of-web.com
            // Please acknowledge use of this code by including this header.

            // initialise image with dimensions of 160 x 45 pixels
            $image = @imagecreatetruecolor(160, 45);
            
            if (!$image) {

                error_log("[Captcha] Cannot Initialize new GD image stream", 0);

                return null;
            }

            // set background and allocate drawing colours
            $background = imagecolorallocate($image, 0x66, 0xCC, 0xFF);
            imagefill($image, 0, 0, $background);
            $linecolor = imagecolorallocate($image, 0x33, 0x99, 0xCC);
            $textcolor1 = imagecolorallocate($image, 0x00, 0x00, 0x00);
            $textcolor2 = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);

            // draw random lines on canvas
            for($i=0; $i < 8; $i++) {
                imagesetthickness($image, rand(1,3));
                imageline($image, rand(0,160), 0, rand(0,160), 45, $linecolor);
            }

            // using a mixture of TTF fonts
            $fonts = [];
            $fonts[] = BASE_ETP_DIRECTORY."font/EastSeaDokdo-Regular.ttf";
            $fonts[] = BASE_ETP_DIRECTORY."font/IndieFlower-Regular.ttf";
            $fonts[] = BASE_ETP_DIRECTORY."font/Orbitron-VariableFont_wght.ttf";
            $fonts[] = BASE_ETP_DIRECTORY."font/Rajdhani-Medium.ttf";
            $fonts[] = BASE_ETP_DIRECTORY."font/SyneMono-Regular.ttf";
            $fonts[] = BASE_ETP_DIRECTORY."font/YeonSung-Regular.ttf";

            // add secret to canvas using random black/white colour
            for($x = 10, $i = 0; $x <= 130; $x += 30) {
                $textcolor = (rand() % 2) ? $textcolor1 : $textcolor2;
                imagettftext($image, 20, rand(-30,30), $x, rand(20, 42), $textcolor, $fonts[array_rand($fonts)], $secret[$i++]);
            }
            
            // Image to Buffer
            ob_start();
            
            imagepng($image);

            $img = ob_get_clean();

            imagedestroy($image);

            return $img;
        }
    }

    // Class auxiliar resposta
    class Resposta {

        private $reply = null;
        private $version = '1.0';   // 2.2 Check Box
        private $state = 'pendenting';

        public function __construct($reply, $version) {

            $this->reply = $reply;
            $this->version = $version;

            // Initialize reply
            $this->init();
        }

        private function init() {

            // Check
            if (($this->reply == null || $this->reply == '') && !Captcha::isEnable())
                $this->state = 'disabled';

            // Não tem resposta
            if ($this->reply == null || $this->reply == '')
                $this->state = 'error';

            $secret = Captcha::getCaptchaSecret();

            if ($secret == null)
                $this->state = 'error';

            if ($this->state == 'pendenting' && $this->version == '1.0' && $this->reply != $secret)
                $this->state = 'error';

            if ($this->state == 'pendenting')
                $this->state = 'success';
        }

        public function isSuccess() {

            if ($this->state == 'success')
                return 1;
            else if ($this->state == 'disabled')
                return 2;

            return 0;
        }
    }
?>