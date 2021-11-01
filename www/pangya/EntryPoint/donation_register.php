<?php
    // Arquivo donation_register.php
    // Criado em 03/12/2020 as 21:02 por Acrisio
    // Definição e Implementação da classe DonationRegister

    include_once('source/entry_point_system.inc');
    include_once('source/captcha.inc');
    include_once('source/transaction_pagseguro.inc');
    include_once('source/transaction_paypal.inc');

    class DonationRegister extends EntryPointSystem {

        public function __construct() {

            // Verifica se está logado e pelo ProjectG
            Design::checkIE();
            Design::checkLogin();
        }

        public function show() {

            $this->checkGetAndPost();

            $this->begin();

            echo '<title>Entry Point - Donation register</title>';

            // load script Captcha
            echo Captcha::loadScript();

            $this->middle();

            $this->content();

            $this->end();
        }

        public function getPlataformName() {
            return DONATION_REGISTER;
        }

        protected function checkGetAndPost() {

            if (!isset($_POST) || !isset($_GET))
                return;

            if (!isset($_POST['code']))
                return;

            if (!isset($_POST['type']) || ($_POST['type'] != PAGSEGURO && $_POST['type'] != PAYPAL))
                return;

            if (Captcha::isEnable() && Captcha::hasSecretMaked()) {

                $resposta = Captcha::checkCaptcha($_POST);

                // Fail - Incorrect Captcha
                if ($resposta == null || $resposta->isSuccess() == 0) {

                    $this->setError("Captcha code entered as incorrect!", 0);

                    return;
                }
            }

            switch ($_POST['type']) {
                case PAGSEGURO:
                {

                    if (strlen($_POST['code']) != 36) {

                        sLogger::getInstance()->putLog("[Error][PagSeguro] Transaction code: [CODE=".$_POST['code']."], invalid", DONATION_REGISTER);

                        $this->setError('Transaction code invalid', 0);

                        return;
                    }

                    // Verifica e registra doação do PagSeguro
                    $this->checkAndRegisterDonationPagSeguro();

                    break;
                }
                case PAYPAL:
                {

                    if (strlen($_POST['code']) != 17 && strlen($_POST['code']) != 19) {

                        sLogger::getInstance()->putLog("[Error][PagSeguro] Transaction code: [CODE=".$_POST['code']."], invalid", DONATION_REGISTER);

                        $this->setError('Transaction code invalid', 0);

                        return;
                    }

                    // Verifica e registra doação do PayPal
                    $this->checkAndRegisterDonationPayPal();

                    break;
                }
            }
        }

        protected function checkAndRegisterDonationPayPal() {

            $pay = new TransactionPayPal();

            if (!$pay->isValid()) {

                sLogger::getInstance()->putLog("[Error] Fail to initialize Transaction PayPal object.", DONATION_REGISTER);

                $this->setError("System error", 4000);

                return;
            }

            $reply = $pay->consultingCode($_POST['code']);

            if ($reply == null || !is_object($reply)) {

                sLogger::getInstance()->putLog("[Error] Reply from Transaction PayPal is not a object. Reply: ".($reply != null ? json_encode($reply) : 'NULL'));

                $this->setError("System error", 4001);

                return;
            }

            // Set Error
            if (!empty($reply->error))
                $this->setError($reply->error['error'], $reply->error['code']);
            else if ($reply->transaction == null)
                $this->setError("System Error", 4002);

            // Verifica se não deu nenhum error
            if ($reply->transaction != null && (empty($reply->error) || $reply->error['code'] == null))
                unset($_POST['code']);  // Clear Transaction code
        }

        protected function checkAndRegisterDonationPagSeguro() {

            $pag = new TransactionPagSeguro();

            if (!$pag->isValid()) {

                sLogger::getInstance()->putLog("[Error] Fail to initialize Transaction PagSeguro object.", DONATION_REGISTER);

                $this->setError("System error", 4000);

                return;
            }

            $reply = $pag->consultingCode($_POST['code']);

            if ($reply == null || !is_object($reply)) {

                sLogger::getInstance()->putLog("[Error] Reply from Transaction PagSeguro is not a object. Reply: ".($reply != null ? json_encode($reply) : 'NULL'));

                $this->setError("System error", 4001);

                return;
            }

            // Set Error
            if (!empty($reply->error))
                $this->setError($reply->error['error'], $reply->error['code']);
            else if ($reply->transaction == null)
                $this->setError("System Error", 4002);

            // Verifica se não deu nenhum error
            if ($reply->transaction != null && (empty($reply->error) || $reply->error['code'] == null))
                unset($_POST['code']);  // Clear Transaction code
        }

        protected function content() {

            echo '<p style="font-size: 20px; font-weight: bold; color: goldenrod; margin-top: 35px; text-align: center;">
                    Donation register
                <p>';

            if (empty($_GET) || (!isset($_GET['pag']) && !isset($_GET['pay']))) {

                echo '<table>
                        <tr>
                            <td height="20"></td>
                        </tr>
                        <tr>
                            <td align="center">
                                <p style="font-size: 18px">Clique na imagem de qual plataforma foi feito a doação</p>
                            </td>
                        </tr>
                        <tr>
                            <td height="40"></td>
                        </tr>
                            <tr>
                                <td>';

                // Select Donation Plataform
                echo '<div class="choicePlataform">
                        <a class="buttonChoice buttonPag" href="?pag"><img src="./img/pag_mark_74x46.png" border="0" alt="PagSeguro"></a>
                        <a class="buttonChoice buttonPay" href="?pay"><img src="./img/pp_cc_mark_74x46.png" border="0" alt="PayPal"></a>
                     </div>';

                echo '      </td>
                        </tr>
                    </table>';

            }else {

                echo '<table align="center" style="margin-top: 50px;">
                    <tr>
                        <td>
                            <p style="margin-left: 15px">
                                Digite o Transaction code da doação que o '.(isset($_GET['pag']) ? 'PagSeguro' : 'PayPal').' gerou.
                            </p>
                        </td>
                    </tr>
                    <tr>
                        <td height="10"></td>
                    </tr>
                    <tr>
                        <td align="center" height="30">'.$this->displayError().'</td>
                    </tr>
                    <tr>
                        <td align="center">
                            <form '.Captcha::makeAttrEventOnSubmitCheck().' action="?'.(isset($_GET['pag']) ? 'pag' : 'pay').'" method="POST">
                                <input type="hidden" name="type" value="'.(isset($_GET['pag']) ? PAGSEGURO : PAYPAL).'">
                                <p style="display: inline; font-size: 28px;">Trasaction code: </p>
                                <input class="input input-border" type="text" placeholder="code" name="code" maxLength="'.(isset($_GET['pag']) ? '36' : '19').'" value="'.(!isset($_POST['code']) ? '' : $_POST['code']).'">
                                '.Captcha::makeCheckBox()/*Criar o desavio de checkbox do reCAPTCHA*/.'
                                <input class="button input-border" type="submit" value="Donation register">
                            </form>
                        </td>
                    </tr>
                </table>';
            }
        }
    }

    $donation_register = new DonationRegister();

    $donation_register->show();
?>