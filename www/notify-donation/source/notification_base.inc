<?php
    // Arquivo notification_base.inc
    // Criado em 02/12/2020 as 12:28 por Acrisio
    // Definição da classe base NotificationBase

    //include_once('db_manager_singleton.inc');
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/util/logger.inc');
    include_once($_SERVER['DOCUMENT_ROOT'].'/util/util.inc');

    abstract class NotificationBase {

        protected $state = false;
        protected $code = null;
        protected $type = null;
        protected static $DateBegin = "2020-08-00T00:00:00.000-03:00";

        abstract protected function checkValidRequest();
        abstract protected function getURLConsult();
        abstract protected function getParamsConsult();
        abstract protected function getDataFromAPI();
        abstract protected function translateConsult($consult);
        abstract protected function translateConsultUpdate($consult);
        abstract protected function getPlataformName();
        abstract protected function getPlataformType();

        public function __construct() {
        }

        public function isValid() {
            return $this->state && $this->code != null && $this->type != null;
        }

        public function consultingCode() {
            
            // Call to API
            $reply_xml = $this->getDataFromAPI();

            $transaction = $this->translateConsult($reply_xml);

            if ($transaction == null || $transaction->type != 1/*1 é Doação e pagamento*/)
                return null;

            $this->updateTransaction($transaction);
            
            return $transaction;
        }

        public function consultingUpdate() {

            $reply_xml = @file_get_contents($this->getURLConsult()."?".http_build_query($this->getParamsConsult()));

            $transactions = $this->translateConsultUpdate($reply_xml);

            if ($transactions == null || (!is_array($transactions) && !is_object($transactions)))
                return null;

            if (is_object($transactions)) {

                // Somente uma transaction
                $this->code = $transactions->code;
                $this->type = $transactions->type;

                // Consulting individual transaction
                $transaction_ret = $this->consultingCode();

                if ($transaction_ret == null)
                    sLogger::getInstance()->putLog("[Error][consulting update] Fail to consulting code [CODE=$this->code, TYPE=$this->type]", $this->getPlataformName());

            }else {
            
                // Mais de uma transaction
                foreach ($transactions as $transaction) {

                    $this->code = $transaction->code;
                    $this->type = $transaction->type;

                    // Consulting individual transaction
                    $transaction_ret = $this->consultingCode();

                    if ($transaction_ret == null)
                        sLogger::getInstance()->putLog("[Error][consulting update] Fail to consulting code [CODE=$this->code, TYPE=$this->type]", $this->getPlataformName());
                }
            }
            
            return $transactions;
        }

        protected function updateTransaction($transaction) {

            // Verifica se já tem ela no banco de dados
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT [index], [uid], [status], [update], [epin_id] FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_new WHERE [code] = ? and [plataforma] = ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT index, uid, status, update, epin_id FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_new WHERE code = ? and plataforma = ?';
            else
                $query = 'SELECT `index`, `uid`, `status`, `update`, `epin_id` FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_new WHERE `code` = ? and `plataforma` = ?';

            $plataform = $this->getPlataformType();

            $params->clear();
            $params->add('s', $transaction->code);
            $params->add('i', $plataform);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0)
                $this->exitAndPutLog("[Error] Fail to consulting[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status] in db. error code: ".$db->db->getLastError(), $this->getPlataformName());

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                // Adiciona
                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertDonationNew ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';

                $uid = -1; // Não registra o UID, por que quem notifica é o pagseguro

                $params->clear();
                $params->add('i', $uid);
                $params->add('i', $plataform);
                $params->add('s', $transaction->sender->email);
                $params->add('s', $transaction->date);
                $params->add('s', $transaction->lastEventDate);
                $params->add('s', $transaction->code);
                $params->add('i', $transaction->type);
                $params->add('i', $transaction->status);
                $params->add('s', $transaction->reference);
                $params->add('i', $transaction->grossAmount);
                $params->add('i', $transaction->netAmount);
                $params->add('s', $transaction->escrowEndDate);

                if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0)
                    $this->exitAndPutLog("[Error] Fail to exec query proc insert donation. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                if ($result == null || ($row = $result->fetch_assoc()) == null)
                    $this->exitAndPutLog("[Error] Fail in proc insert donation result empty.", $this->getPlataformName());

                if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1)
                    $this->exitAndPutLog("[Error] Fail in proc insert donation id result is invalid", $this->getPlataformName());

                // Log
                sLogger::getInstance()->putLog("[Log] Donation[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status, ID=".$row['ID']."] adicionado com sucesso.", $this->getPlataformName());

            }else {

                if (!isset($row['index']) || !isset($row['uid']) || !isset($row['epin_id']) || !isset($row['status']) || !key_exists('update', $row) || is_nan($row['status']))
                    $this->exitAndPutLog("[Error] Failt in query select donation, result is invalid.", $this->getPlataformName());
                
                // Atualiza
                $update = null;
                $lastEventDate = null;

                if ($row['update'] != null)
                    $update = new DateTime($row['update']);
                
                if ($transaction->lastEventDate != null)
                    $lastEventDate = new DateTime($transaction->lastEventDate);

                if ($transaction->status != $row['status'] || ($update == null && $lastEventDate != null) || ($update != null && $lastEventDate != null && $update < $lastEventDate)) {

                    // Make Epin and send
                    $ret_make_epin = $this->makeEpinAndSend($transaction, $row);

                    $epin_id = $ret_make_epin['epin_id'];

                    // Update Donation
                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew ?, ?, ?, ?, ?, ?, ?, ?, ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';
                    else
                        $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';

                    // Check Update time is null, make now
                    if ($transaction->lastEventDate != null)
                        $update = $transaction->lastEventDate;
                    else
                        $update = (new DateTime())->format("Y-m-d H:i:s.u");

                    $params->clear();
                    $params->add('s', $transaction->code);
                    $params->add('i', $plataform);
                    $params->add('i', $row['uid']);
                    $params->add('i', $transaction->status);
                    $params->add('s', $update);
                    $params->add('i', $epin_id);
                    $params->add('i', $transaction->grossAmount);
                    $params->add('i', $transaction->netAmount);
                    $params->add('s', $transaction->escrowEndDate);

                    if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0)
                        $this->exitAndPutLog("[Error] Fail to exec query proc update donation. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                    if ($result == null || ($row = $result->fetch_assoc()) == null)
                        $this->exitAndPutLog("[Error] Fail in proc update donation result empty.", $this->getPlataformName());
    
                    if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1)
                        $this->exitAndPutLog("[Error] Fail in proc update donation id result is invalid", $this->getPlataformName());

                    // Log
                    sLogger::getInstance()->putLog("[Log] Donation[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status, ID=".$row['ID'].", EPIN_ID=".$epin_id."] atualizado com sucesso.", $this->getPlataformName());
                }
            }

        }

        protected function makeEpinAndSend($transaction, $donation_row) {

            // Verifica se não tem o epin
            if ($donation_row['epin_id'] <= 0)
                $donation_row['epin_id'] = -1;

            // Invalid date Donation
            if ((new DateTime(self::$DateBegin)) > (new DateTime($transaction->date))) {
                
                sLogger::getInstance()->putLog("Player[UID=".$donation_row['uid']."] donation[ID=".$donation_row['index'].", DATE=".$transaction->date."] date is oldest.", $this->getPlataformName());
                
                return [
                    'code' => 1,
                    'epin_id' => $donation_row['epin_id']
                ];
            }

            // Make EPIN and Send it to mail of player
            if ($donation_row['epin_id'] != -1 || $donation_row['uid'] <= 0)
                return [
                    'code' => 2,
                    'epin_id' => $donation_row['epin_id']
                ];

            if ($transaction->status != 3/*Pago*/ && $transaction->status != 4/*Disponível*/)
                return [
                    'code' => 3,
                    'epin_id' => $donation_row['epin_id']
                ];

            $value = (int)($transaction->grossAmount * DONATION_TO_CP);

            if ($value <= 0)
                return [
                    'code' => 4,
                    'epin_id' => $donation_row['epin_id']
                ];

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcRegisterDonationEpin ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID", "_EPIN_" as "epin", "_EMAIL_" as "email" from '.$db->con_dados['DB_NAME'].'.ProcRegisterDonationEpin(?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcRegisterDonationEpin(?, ?, ?)';

            $params->clear();
            $params->add('i', $donation_row['index']);   // Donation ID
            $params->add('i', $donation_row['uid']);     // Player UID
            $params->add('i', $value);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                sLogger::getInstance()->putLog("[Error] DONATION[ID=".$donation_row['index']."] Player[UID="
                    .$donation_row['uid']."] - Fail to exec proc register donation epin. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                return [
                    'code' => 5,
                    'epin_id' => $donation_row['epin_id']
                ];
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                sLogger::getInstance()->putLog("[Error] DONATION[ID=".$donation_row['index']."] Player[UID=".$donation_row['uid']."] - Fail in proc register donation epin, result empty.", $this->getPlataformName());

                return [
                    'code' => 6,
                    'epin_id' => $donation_row['epin_id']
                ];
            }

            if (!isset($row['ID']) || $row['ID'] == -1 || is_nan($row['ID']) || !isset($row['epin']) || !isset($row['email'])) {

                sLogger::getInstance()->putLog("[Error] DONATION[ID=".$donation_row['index']."] Player[UID=".$donation_row['uid']."] - Fail in proc register donation epin, result invalid.", $this->getPlataformName());

                return [
                    'code' => 7,
                    'epin_id' => $donation_row['epin_id']
                ];
            }

            // Send Mail
            $send_mail_ret = $this->sendMail($row['email'], $row['epin']);

            // Sucess create E-Pin
            return [
                'code' => ($send_mail_ret == false ? 8/*Fail send Mail*/ : 0),
                'epin_id' => $row['ID']
            ];
        }

        protected function sendMail($email, $epin) {
            global $error;

            require_once($_SERVER['DOCUMENT_ROOT'].'/config/smtp_base.inc');

            // Cria o Email e envia aqui nessa parte
            $bodymail = "PangYa SuperSS - Donation E-pin.\n\nPara pegar o Cookie Point tem que trocar o E-Pin dentro do jogo. Entre no jogo e no menu recharge lá vai ter a opção de troca do E-Pin por Cookie Point.\n\n";
            $bodymail .= "E-Pin: ".$epin;

            if (!smtpmailer($email, '#Seu email do remetente#', 'PangYa SuperSS', 'Donation E-Pin', $bodymail)) {
                
                if (!empty($error))
                    sLogger::getInstance()->putLog("[Error] Fail to send epin to $email. Error: ".$error, $this->getPlataformName());

                return false;
            }

            // Log
            sLogger::getInstance()->putLog("[Log] E-Pin: $epin foi enviado para o e-mail: $email com sucesso.", $this->getPlataformName());

            return true;
        }

        public function exitAndPutLog($log, $type) {

            sLogger::getInstance()->putLog($log, $type);

            exit();

            return;
        }
    }
?>