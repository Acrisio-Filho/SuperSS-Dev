<?php
    // Arquivo epin_exchange.php
    // Criado em 03/12/2020 as 21:11 por Acrisio
    // Definição e Implementação da classe EpinExchange

    include_once('source/entry_point_system.inc');
    
    //include_once('source/db_manager_singleton.inc');
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    // Captcha
    include_once('source/captcha.inc');

    class EpinExchange extends EntryPointSystem {

        // Confirm epin
        private $epin = null;

        public function __construct() {

            // Verifica se está logado e pelo ProjectG
            Design::checkIE();
            Design::checkLogin();
        }

        public function show() {

            $this->initSession();

            $this->checkGetAndPost();

            $this->begin();

            echo '<title>Entry Point - E-Pin exchange</title>';

            // Load Captcha Script
            echo Captcha::loadScript();

            $this->middle();

            $this->content();

            $this->end();
        }

        public function getPlataformName() {
            return EPIN_EXCHANGE;
        }

        protected function initSession() {

            if (!isset($_SESSION))
                session_start();

            // Verifica se está no ProjectG e Logado
            Design::checkIE();
            Design::checkLogin();

            if (isset($_SESSION['player_epin']))
                $this->epin = $_SESSION['player_epin'];
        }

        protected function checkGetAndPost() {

            if (!isset($_POST) || !isset($_GET))
                return;

            if (isset($_GET['check']) && $this->epin == null) {

                // Verifica Captcha
                if (Captcha::isEnable() && Captcha::hasSecretMaked()) {

                    $resposta = Captcha::checkCaptcha($_POST);

                    // Fail
                    if ($resposta == null || $resposta->isSuccess() == 0) {

                        $this->setError('Captcha code entered as incorrect!', 0);

                        return;
                    }
                }

                // Check E-Pin
                if (!isset($_POST['epin']))
                    return;

                if (strlen($_POST['epin']) != 36) {

                    sLogger::getInstance()->putLog("[Error] E-Pin: [CODE=".$_POST['epin']."], invalid", EPIN_EXCHANGE);

                    $this->setError('E-Pin invalid', 0);

                    return;
                }

                // Check Epin is valid
                $this->checkEpin();

            }else if (isset($_GET['confirm'])) {

                // Confirm
                if (!isset($_POST['epin_id']))
                    return;

                if ($_POST['epin_id'] != $this->epin->id) {

                    sLogger::getInstance()->putLog("[Error] E-Pin: [ID=".$this->epin->id.", SEND=".$_POST['epin_id']."], invalid", EPIN_EXCHANGE);

                    $this->setError('E-Pin invalid', 0);

                    return;
                }

                // Check and exchange Epin
                $this->checkAndExchangeEpin();
            }
        }

        protected function checkEpin() {

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT 
                        a.[index],
                        a.[epin],
                        a.[qntd],
                        b.[gross_amount]
                    FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_epin a 
                        INNER JOIN '.$db->con_dados['DB_NAME'].'.pangya_donation_new b
                        ON a.[UID] = b.[UID] AND a.[index] = b.[epin_id]
                    WHERE a.[epin] = ? AND a.valid = 1 AND a.retrive_uid IS NULL AND b.[status] IN(3, 4)';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT 
                        a.index,
                        a.epin,
                        a.qntd,
                        b.gross_amount
                    FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_epin a 
                        INNER JOIN '.$db->con_dados['DB_NAME'].'.pangya_donation_new b
                        ON a.UID = b.UID AND a.index = b.epin_id
                    WHERE a.epin = ? AND a.valid = 1 AND a.retrive_uid IS NULL AND b.status IN(3, 4)';
            else
                $query = 'SELECT 
                        a.`index`,
                        a.`epin`,
                        a.`qntd`,
                        b.`gross_amount`
                    FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_epin a 
                        INNER JOIN '.$db->con_dados['DB_NAME'].'.pangya_donation_new b
                        ON a.`UID` = b.`UID` AND a.`index` = b.`epin_id`
                    WHERE a.`epin` = ? AND a.valid = 1 AND a.retrive_uid IS NULL AND b.`status` IN(3, 4)';

            $params->clear();
            $params->add('s', $_POST['epin']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                sLogger::getInstance()->putLog("[Error] Fail to exec query select epin. Error code: ".$db->db->getLastError(), EPIN_EXCHANGE);

                $this->setError('System error', 20000);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                sLogger::getInstance()->putLog("[Error] Epin invalid", EPIN_EXCHANGE);

                $this->setError('E-Pin invalid', 0);

                return;
            }

            if (!isset($row['epin']) || !isset($row['index']) || !isset($row['qntd']) || !isset($row['gross_amount'])) {

                sLogger::getInstance()->putLog("[Error] Fail in query select epin.", EPIN_EXCHANGE);

                $this->setError('System error', 20001);

                return;
            }

            $this->epin = new stdClass();
            
            $this->epin->id = $row['index'];
            $this->epin->code = $row['epin'];
            $this->epin->cp_qntd = $row['qntd'];
            $this->epin->donation_value = $row['gross_amount'];

            // Set Epin in session
            $_SESSION['player_epin'] = $this->epin;
        }

        protected function checkAndExchangeEpin() {

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            // Check EPIN is valid
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT 
                        a.[index]
                    FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_epin a 
                        INNER JOIN '.$db->con_dados['DB_NAME'].'.pangya_donation_new b
                        ON a.[UID] = b.[UID] AND a.[index] = b.[epin_id]
                    WHERE a.[index] = ? AND a.valid = 1 AND a.retrive_uid IS NULL AND b.[status] IN(3, 4)';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT 
                        a.index
                    FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_epin a 
                        INNER JOIN '.$db->con_dados['DB_NAME'].'.pangya_donation_new b
                        ON a.UID = b.UID AND a.index = b.epin_id
                    WHERE a.index = ? AND a.valid = 1 AND a.retrive_uid IS NULL AND b.status IN(3, 4)';
            else
                $query = 'SELECT 
                        a.`index`
                    FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_epin a 
                        INNER JOIN '.$db->con_dados['DB_NAME'].'.pangya_donation_new b
                        ON a.`UID` = b.`UID` AND a.`index` = b.`epin_id`
                    WHERE a.`index` = ? AND a.valid = 1 AND a.retrive_uid IS NULL AND b.`status` IN(3, 4)';

            $params->clear();
            $params->add('i', $this->epin->id);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                sLogger::getInstance()->putLog("[Error] Fail to exec query select epin. Error code: ".$db->db->getLastError(), EPIN_EXCHANGE);

                $this->setError('System error', 20000);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                sLogger::getInstance()->putLog("[Error] Epin invalid", EPIN_EXCHANGE);

                $this->setError('E-Pin invalid', 0);

                return;
            }

            if (!isset($row['index'])) {

                sLogger::getInstance()->putLog("[Error] Fail in query select epin.", EPIN_EXCHANGE);

                $this->setError('System error', 20001);

                return;
            }

            // Add exchange CP By EPIN
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcExchangeCookiePointByEpin ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcExchangeCookiePointByEpin(?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcExchangeCookiePointByEpin(?, ?, ?)';

            $params->clear();
            $params->add('i', $this->epin->id);
            $params->add('i', PlayerSingleton::getInstance()['UID']);
            $params->add('i', $this->epin->cp_qntd);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                sLogger::getInstance()->putLog("[Error] Fail to exec proc exchange cookie point by epin. Error code: ".$db->db->getLastError(), EPIN_EXCHANGE);

                $this->setError('System error', 20002);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                sLogger::getInstance()->putLog("[Error] Fail in proc exchange cookie point by epin, result is empty.", EPIN_EXCHANGE);

                $this->setError('System error', 20003);

                return;
            }

            if (!isset($row['ID']) || $row['ID'] == -1 || is_nan($row['ID'])) {

                sLogger::getInstance()->putLog("[Error] Falt in proc exchange cookie point by epin, result is invalid.", EPIN_EXCHANGE);

                $this->setError('System error', 20004);

                return;
            }

            // Log
            sLogger::getInstance()->putLog("[Log] Player[UID=".PlayerSingleton::getInstance()['UID']."] E-Pin[ID=".$row['ID']."] exchange for CP[QNTD=".$this->epin->cp_qntd."] com sucesso.", EPIN_EXCHANGE);

            // Null Msg, != null Error
            $this->setError($this->epin->cp_qntd.' Cookie Point has add to your account. Total CP: '.(PlayerSingleton::getInstance()['COOKIE'] + $this->epin->cp_qntd), null);

            // Add to session COOKIE without update from DB
            $_SESSION['player_etp']['COOKIE'] += $this->epin->cp_qntd;

            // Limpa epin
            $this->epin = null;

            unset($_SESSION['player_epin']);
        }

        protected function content() {

            echo '<p style="font-size: 20px; font-weight: bold; color: goldenrod; margin-top: 35px; text-align: center;">
                    E-Pin exchange
                <p>';

            if ($this->epin == null) {
                
                echo '<table align="center" style="margin-top: 50px;">
                        <tr>
                            <td>
                                <p style="margin-left: 15px">
                                    Digite o E-Pin que foi enviado para o seu e-mail ou logue no site e entre na opção minhas doações para pegar o E-Pin.
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
                                <form '.Captcha::makeAttrEventOnSubmitCheck().' action="?check" method="POST">
                                    <p style="display: inline; font-size: 28px;">E-Pin: </p>
                                    <input class="input input-border" type="text" placeholder="code" name="epin" maxLength="36" value="'.(!isset($_POST['epin']) ? '' : $_POST['epin']).'">
                                    '.Captcha::makeCheckBox()/*Criar o desavio de checkbox do reCAPTCHA*/.'
                                    <input class="button input-border" type="submit" value="Epin exchange">
                                </form>
                            </td>
                        </tr>
                    </table>';
            
            }else {

                echo '<table align="center" style="margin-top: 50px;">
                        <tr>
                            <td>
                                <p style="margin-left: 15px">
                                    Confirme os valores do E-Pin e clique no botão Epin exchange.
                                </p>
                            </td>
                        </tr>
                        <tr>
                            <td height="10"></td>
                        </tr>
                        <tr>
                            <td align="left" height="40">
                                <p style="margin-left: 20px; font-size: 18px; color: gray">
                                    E-Pin: '.$this->epin->code.'<br>
                                    Donation Value: R$ '.$this->epin->donation_value.'<br>
                                    Cookie Point: '.$this->epin->cp_qntd.'
                                </p>
                            </td>
                        </tr>
                        <tr>
                            <td align="center" height="30">'.$this->displayError().'</td>
                        </tr>
                        <tr>
                            <td align="center">
                                <form action="?confirm" method="POST">
                                    <input type="hidden" name="epin_id" value="'.$this->epin->id.'">
                                    <input class="button input-border" type="submit" value="Epin exchange">
                                </form>
                            </td>
                        </tr>
                    </table>';
            }
        }
    }

    $epin_exchange = new EpinExchange();

    $epin_exchange->show();
?>