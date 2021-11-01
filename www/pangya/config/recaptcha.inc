<?php
    // Arquivo recaptcha.inc
    // Criado em 01/08/2019 as 20:53 por Acrisio
    // Definição dos dados do reCAPTCHA do Google

    class reCAPTCHA {

        // Habilita true, false desabilita
        private static $enable = false;

        // Keys reCAPTCHA V2 Checkbox
        private static $pubKey_V2 = "sua chave public do reCAPTCHA Versão 2 aqui";
        private static $privKey_V2 = "sua chave privada do reCAPTCHA Versão 2 aqui";

        // Keys reCAPTCH V3
        private static $pubKey_V3 = "sua chave public do reCAPTCHA Versão 3 aqui";
        private static $privKey_V3 = "sua chave privada do reCAPTCHA Versão 3 aqui";

        public static function getPublicKey_V2() {
            return self::$pubKey_V2;
        }

        public static function getPublicKey_V3() {
            return self::$pubKey_V3;
        }

        public static function isEnable() {
            return self::$enable;
        }

        public static function loadScript_V2() {

            if (!self::$enable)
                return '';

            return '<script src="https://www.google.com/recaptcha/api.js?" async defer></script>';
        }

        public static function loadScript_V3() {
            
            if (!self::$enable)
                return '';

            return '<script src="https://www.google.com/recaptcha/api.js?render='.self::$pubKey_V3.'"></script>';
        }

        public static function loadScript_V2_from_V3() {

            if (!self::$enable)
                return '';

            return  '<script>
                    grecaptcha.ready(() => {
                        grecaptcha.render(\'reCAPTCHA_V2\', {
                            \'sitekey\' : \''.self::$pubKey_V2.'\'
                        });
                    });
                </script>';
        }

        public static function checkPostReCAPTCHA_V2($post) {

            if (!self::$enable)
                return true;

            if (!isset($post) || !isset($post['g-recaptcha-response']) || $post['g-recaptcha-response'] == '')
                return false;

            return true;
        }

        public static function checkPostReCAPTCHA_V3($post) {

            if (!self::$enable)
                return true;

            if (!isset($post) || !isset($post['g-recaptcha-response3']) || $post['g-recaptcha-response3'] == '')
                return false;

            return true;
        }

        public static function checkReCAPTCHA_V2($post) {

            if (!self::$enable)
                return (Object)['success' => true, 'enable' => self::$enable];
            
            if (!reCAPTCHA::checkPostReCAPTCHA_V2($post))
                return null;

            $reply = new Resposta(json_decode(file_get_contents("https://www.google.com/recaptcha/api/siteverify?secret=".self::$privKey_V2."&response=".$post['g-recaptcha-response'])), '2.2');

            return $reply;
        }

        public static function checkReCAPTCHA_V3($post) {

            if (!self::$enable)
                return (Object)['success' => true, 'enable' => self::$enable];

            if (!reCAPTCHA::checkPostReCAPTCHA_V3($post))
                return null;

            $reply = new Resposta(json_decode(file_get_contents("https://www.google.com/recaptcha/api/siteverify?secret=".self::$privKey_V3."&response=".$post['g-recaptcha-response3'])), '3');

            return $reply;
        }

        public static function makeCheckBox_V2() {

            if (!self::$enable)
                return '';

            return '<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td align="center"><div id="reCAPTCHA_V2" class="g-recaptcha" data-sitekey="'.self::$pubKey_V2.'"></div></td></tr></table>';
        }

        public static function makeAttrEventOnSubmitCheck_V2() {

            if (!self::$enable)
                return '';

            return 'onsubmit="return (function(){ 
                if (grecaptcha.getResponse() == \'\') {
                    alert(\'Por favor faça o reCAPTCHA primeiro.\');
                    return false;
                }

                return true;
            })()"';
        }

        public static function makeAttrEventOnSubmitCheck_V3($action) {

            if (!self::$enable)
                return '';

            if ($action == '' || !isset($action))
                $action = 'homepage';

            return 'onsubmit="return (function(element){
                    grecaptcha.ready(() => {
                        grecaptcha.execute(\''.self::$pubKey_V3.'\', { action: \''.$action.'\' }).then((token) => {
                            var el = document.createElement(\'input\');
                            el.type = \'hidden\';
                            el.value = token;
                            el.name = \'g-recaptcha-response3\';
                            element.appendChild(el);
                            element.submit();
                        });
                    });
                    return false;
                })(this)"';
        }
    }

    // Class auxiliar resposta
    class Resposta {

        private $reply = null;
        private $version = '2.2';   // 2.2 Check Box
        private $state = 'pendenting';

        public function __construct($reply, $version) {

            $this->reply = $reply;
            $this->version = $version;

            // Initialize reply
            $this->init();
        }

        private function init() {

            // Check
            if (($this->reply == null || $this->reply == '' ) && !reCAPTCHA::isEnable())
                $this->state = 'disabled';

            // Não tem resposta
            if ($this->reply == null || $this->reply == '')
                $this->state = 'error';

            if ($this->state == 'pendenting' && (!property_exists($this->reply, 'success') || !$this->reply->success))
                $this->state = 'error';

            if ($this->state == 'pendenting' && $this->version == '3' 
                    && (!property_exists($this->reply, 'action') || !property_exists($this->reply, 'score')))
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

        public function getScore() {

            if ($this->state == 'success' && $this->version == '3')
                return $this->reply->score;

            return '0.0';
        }

        public function checkAction($action) {

            if ($this->state == 'success' && $this->version == '3')
                return $this->reply->action == $action;

            return false;
        }
    }
?>