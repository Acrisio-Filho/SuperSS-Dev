<?php
    // Arquivo transaction_base.inc
    // Criado em 06/12/2020 as 19:29 por Acrisio
    // Definição da classe TransactionBase

    include_once($_SERVER['DOCUMENT_ROOT'].'/EntryPoint/config/config.inc');
    include_once($_SERVER['DOCUMENT_ROOT'].'/EntryPoint/util/logger.inc');
    include_once($_SERVER['DOCUMENT_ROOT'].'/EntryPoint/util/util.inc');
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once('player_singleton.inc');

    abstract class TransactionBase {

        protected $state = false;
        protected static $DateBegin = "2020-08-00T00:00:00.000-03:00";

        abstract protected function getURLConsult();
        abstract protected function getParamsConsult();
        abstract protected function getDataFromAPI($code);
        abstract protected function translateConsult($consult, $code);
        abstract protected function getPlataformName();
        abstract protected function getPlataformType();
        abstract protected function checkCode($code);

        protected $msg = [];

        public function __construct() {
        }

        public function isValid() {
            return $this->state;
        }

        protected function setError($error, $code) {

            $this->msg = [
                'error' => $error,
                'code' => $code
            ];
        }

        // Verifica se não tem nenhum erro
        protected function notHaveError() {
            return empty($this->msg);
        }

        protected function returnTransactionWithError($transaction) {
            return (Object)[
                'transaction' => $transaction,
                'error' => $this->msg
            ];
        }

        public function consultingCode($code) {

            if (!$this->checkCode($code)) {

                $this->setError("Transaction code invalid", 5000);

                return $this->returnTransactionWithError(null);
            }

            $transaction = $this->getTrasactionFromDB($code);

            // Verifaca se tem erros
            if (!$this->notHaveError())
                return $this->returnTransactionWithError(null);

            if ($transaction != null && !is_object($transaction)) {

                $this->setError("System error", 5001);

                return $this->returnTransactionWithError(null);
            }

            // Procura no banco de dados a transaction
            if ($transaction == null) {
            
                // Call to API
                $reply_xml = $this->getDataFromAPI($code);

                $transaction = $this->translateConsult($reply_xml, $code);

                if ($transaction == null || $transaction->type != 1/*1 é Doação e pagamento*/)
                    return $this->returnTransactionWithError(null);

                // Adiciona a transaction ao banco de dados
                $this->insertTransaction($transaction);

            }else {

                // Atualiza transaction
                $this->updateTransaction($transaction);
            }
            
            return $this->returnTransactionWithError($transaction);
        }

        protected function getTrasactionFromDB($code) {

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT [index], [status], [epin_id], [uid], [date], [gross_amount], [net_amount], [code], [type], [escrow] FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_new WHERE [code] = ? AND [plataforma] = ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT index, status, epin_id, uid, date, gross_amount, net_amount, code, type, escrow FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_new WHERE code = ? AND plataforma = ?';
            else
                $query = 'SELECT `index`, `status`, `epin_id`, `uid`, `date`, `gross_amount`, `net_amount`, `code`, `type`, `escrow` FROM '.$db->con_dados['DB_NAME'].'.pangya_donation_new WHERE `code` = ? AND `plataforma` = ?';

            $plataform = $this->getPlataformType();

            $params->clear();
            $params->add('s', $_POST['code']);
            $params->add('i', $plataform);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                sLogger::getInstance()->putLog("[Error] Fail to exec query select donation by code. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                $this->setError('System error', 30000);

                return null;
            }

            // Not found Transaction in DB, não gera erro só retorna null
            if ($result == null || ($row = $result->fetch_assoc()) == null)
                return null;

            if (!isset($row['index']) || $row['index'] == -1 || is_nan($row['index'])
                || !isset($row['uid']) || is_nan($row['uid']) || !isset($row['status']) || is_nan($row['status'])
                || !isset($row['epin_id']) || is_nan($row['epin_id']) || !isset($row['gross_amount']) || is_nan($row['gross_amount'])
                || !isset($row['net_amount']) || is_nan($row['net_amount']) || !key_exists('escrow', $row)
                || !isset($row['date']) || $row['date'] == '' || !isset($row['code']) || $row['code'] == '' 
                || !isset($row['type']) || is_nan($row['type'])) {

                sLogger::getInstance()->putLog("[Error] Fail in query select donation by code, result invalid.", $this->getPlataformName());

                $this->setError('System error', 30001);

                return null;
            }

            // Make Object of transaction Array
            $obj = new stdClass();

            // DB Dados
            $obj->id = $row['index'];
            $obj->uid = $row['uid'];
            $obj->epin_id = $row['epin_id'];

            // Padrão
            $obj->code = $row['code'];
            $obj->type = $row['type'];
            $obj->status = $row['status'];
            $obj->grossAmount = $row['gross_amount'];
            $obj->netAmount = $row['net_amount'];
            $obj->escrowEndDate = $row['escrow'];
            $obj->date = $row['date'];

            return $obj;
        }

        protected function insertTransaction($transaction) {

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            // Registra a doação no banco de dados
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertDonationNew ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';

            $plataform = $this->getPlataformType();

            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);
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

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {
                
                sLogger::getInstance()->putLog("[Error] Fail to exec query proc insert donation. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                $this->setError('System error', 31000);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                sLogger::getInstance()->putLog("[Error] Fail in proc insert donation result empty.", $this->getPlataformName());

                $this->setError('System error', 31001);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                sLogger::getInstance()->putLog("[Error] Fail in proc insert donation id result is invalid", $this->getPlataformName());

                $this->setError('System error', 31002);

                return;
            }

            // Log
            sLogger::getInstance()->putLog("[Log] Player[UID=".PlayerSingleton::getInstance()['UID']." Donation[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status, ID="
                .$row['ID']."] adicionado com sucesso.", $this->getPlataformName());

            // MakeEpinAnd Send
            $ret_make_epin = $this->makeEpinAndSend($transaction, [
                'index' => $row['ID'],
                'uid' => PlayerSingleton::getInstance()['UID'],
                'epin_id' => -1
            ]);

            $epin_id = $ret_make_epin['epin_id'];

            if ($epin_id != -1) {

                // Update Donation
                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew ?, ?, ?, ?, ?, ?, ?, ?, ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';

                $update = (new DateTime())->format("Y-m-d H:i:s.u");

                $params->clear();
                $params->add('s', $transaction->code);
                $params->add('i', $plataform);
                $params->add('i', PlayerSingleton::getInstance()['UID']);
                $params->add('i', $transaction->status);
                $params->add('s', $update);
                $params->add('i', $epin_id);
                $params->add('i', $transaction->grossAmount);
                $params->add('i', $transaction->netAmount);
                $params->add('s', $transaction->escrowEndDate);

                if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                    sLogger::getInstance()->putLog("[Error] Fail to exec query proc update donation. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                    $this->setError('System error', 32000);

                    return;
                }

                if ($result == null || ($row = $result->fetch_assoc()) == null) {

                    sLogger::getInstance()->putLog("[Error] Fail in proc update donation result empty.", $this->getPlataformName());

                    $this->setError('System error', 32001);

                    return;
                }

                if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                    sLogger::getInstance()->putLog("[Error] Fail in proc update donation id result is invalid", $this->getPlataformName());

                    $this->setError('System error', 32002);

                    return;
                }

                // Log
                sLogger::getInstance()->putLog("[Log] Player[UID=".PlayerSingleton::getInstance()['UID']."] Donation[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status, ID="
                    .$row['ID'].", EPIN_ID=".$epin_id."] atualizado com sucesso.", $this->getPlataformName());

                $msg = 'Doação registrada.<br>';
            
                if ($ret_make_epin['code'] == 0)
                    $msg .= 'O E-Pin foi enviado para o seu email cadastrado.';
                else
                    $msg .= 'O E-Pin foi gerado porém não conseguiu enviar para o email cadastrado. Logue no site e entre em minhas doações para recuperar o E-Pin gerado.';

                // Null == Message, != null Error
                $this->setError($msg, null);

            }else {

                $msg = '';

                if ($ret_make_epin['code'] == 1)
                    $msg = 'Doação registrada.<br>Mas ela é muito antiga para o E-Pin ser gerado.';
                else if ($transaction->status == 1)
                    $msg = 'Doação registrada.<br>Aguardando pagamento, assim que for confirmado o pagamento o E-Pin vai ser enviado para o seu email cadastrado.';
                else if ($transaction->status == 2)
                    $msg = 'Doação registrada.<br>Em analise, assim que for confirmado o E-Pin vai ser enviado para o seu email cadastrado.';
                else if ($transaction->status == 7)
                    $msg = 'Doação registrada, mas ela foi cancelada.';
                else if ($transaction->status == 6 || $transaction->status == 8)
                    $msg = 'Doação registrada, mas ela foi devolvida.';
                else
                    $msg = 'Doação registrada.<br>Aguardando liberação para o E-Pin ser enviado para o seu email cadastrado.';

                // Null == Message, != null Error
                $this->setError($msg, null);
            }

            return;
        }

        protected function updateTransaction($transaction) {

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $plataform = $this->getPlataformType();

            if ($transaction->uid > 0 && $transaction->uid != PlayerSingleton::getInstance()['UID']) {

                sLogger::getInstance()->putLog("[Error] Player[UID=".PlayerSingleton::getInstance()['UID']."] tentando registrar uma doação[ID="
                    .$transaction->id."] de outro player[UID=".$transaction->uid."]", $this->getPlataformName());

                $this->setError('Transaction code invalid', 0);

                return;
            }

            if ($transaction->uid > 0 && $transaction->epin_id > 0) {

                // Log
                sLogger::getInstance()->putLog("[Error] Player[UID=".PlayerSingleton::getInstance()['UID']."] tentando registrar uma doação[ID="
                    .$transaction->id.", CODE=".$transaction->code."] mais de uma vez.", $this->getPlataformName());

                $this->setError('Essa doação já foi registrada anteriormente e o E-Pin já foi gerado. Verifique o seu e-mail ou logue no site e entre em minhas doações para recuperar o E-Pin gerado.', 0);

                return;

            }else if ($transaction->uid > 0 && $transaction->epin_id == -1) {

                // Cria o epin se o status
                $ret_make_epin = $this->makeEpinAndSend($transaction, [
                    'index' => $transaction->id,
                    'uid' => PlayerSingleton::getInstance()['UID'],
                    'epin_id' => -1
                ]);

                $epin_id = $ret_make_epin['epin_id'];

                if ($epin_id != -1) {

                    // Update Donation
                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew ?, ?, ?, ?, ?, ?, ?, ?, ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';
                    else
                        $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';

                    $update = (new DateTime())->format("Y-m-d H:i:s.u");

                    $params->clear();
                    $params->add('s', $transaction->code);
                    $params->add('i', $plataform);
                    $params->add('i', PlayerSingleton::getInstance()['UID']);
                    $params->add('i', $transaction->status);
                    $params->add('s', $update);
                    $params->add('i', $epin_id);
                    $params->add('i', $transaction->grossAmount);
                    $params->add('i', $transaction->netAmount);
                    $params->add('s', $transaction->escrowEndDate);

                    if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                        sLogger::getInstance()->putLog("[Error] Fail to exec query proc update donation. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                        $this->setError('System error', 32000);

                        return;
                    }

                    if ($result == null || ($row = $result->fetch_assoc()) == null) {

                        sLogger::getInstance()->putLog("[Error] Fail in proc update donation result empty.", $this->getPlataformName());

                        $this->setError('System error', 32001);

                        return;
                    }
    
                    if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                        sLogger::getInstance()->putLog("[Error] Fail in proc update donation id result is invalid", $this->getPlataformName());

                        $this->setError('System error', 32002);

                        return;
                    }

                    // Log
                    sLogger::getInstance()->putLog("[Log] Player[UID=".PlayerSingleton::getInstance()['UID']."] Donation[CODE=".$transaction->code.", TYPE="
                        .$transaction->type.", STATUS=".$transaction->status.", ID=".$row['ID'].", EPIN_ID=".$epin_id."] atualizado com sucesso.", $this->getPlataformName());

                    $msg = 'Doação registrada.<br>';
                
                    if ($ret_make_epin['code'] == 0)
                        $msg .= 'O E-Pin foi enviado para o seu email cadastrado.';
                    else
                        $msg .= 'O E-Pin foi gerado porém não conseguiu enviar para o email cadastrado. Logue no site e entre em minhas doações para recuperar o E-Pin gerado.';

                    // Null == Message, != null Error
                    $this->setError($msg, null);
                
                }else {

                    $msg = '';

                    if ($ret_make_epin['code'] == 1)
                        $msg = 'Doação registrada.<br>Mas ela é muito antiga para o E-Pin ser gerado.';
                    else if ($transaction->status== 1)
                        $msg = 'Doação registrada.<br>Aguardando pagamento, assim que for confirmado o pagamento o E-Pin vai ser enviado para o seu email cadastrado.';
                    else if ($transaction->status == 2)
                        $msg = 'Doação registrada.<br>Em analise, assim que for confirmado o E-Pin vai ser enviado para o seu email cadastrado.';
                    else if ($transaction->status == 7)
                        $msg = 'Doação registrada, mas ela foi cancelada.';
                    else if ($transaction->status == 6 || $transaction->status == 8)
                        $msg = 'Doação registrada, mas ela foi devolvida.';
                    else
                        $msg = 'Doação registrada.<br>Aguardando liberação para o E-Pin ser enviado para o seu email cadastrado.';

                    // Null == Message, != null error
                    $this->setError($msg, null);
                }

                return;
            
            }else if ($transaction->uid == -1 && $transaction->epin_id == -1) {

                // Registra Doação para o player

                // Cria o epin se o status
                $ret_make_epin = $this->makeEpinAndSend($transaction, [
                    'index' => $transaction->id,
                    'uid' => PlayerSingleton::getInstance()['UID'],
                    'epin_id' => -1
                ]);

                $epin_id = $ret_make_epin['epin_id'];

                // Update Donation
                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew ?, ?, ?, ?, ?, ?, ?, ?, ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';

                $update = (new DateTime())->format("Y-m-d H:i:s.u");

                $params->clear();
                $params->add('s', $transaction->code);
                $params->add('i', $plataform);
                $params->add('i', PlayerSingleton::getInstance()['UID']);
                $params->add('i', $transaction->status);
                $params->add('s', $update);
                $params->add('i', $epin_id);
                $params->add('i', $transaction->grossAmount);
                $params->add('i', $transaction->netAmount);
                $params->add('s', $transaction->escrowEndDate);

                if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                    sLogger::getInstance()->putLog("[Error] Fail to exec query proc update donation. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                    $this->setError('System error', 32000);

                    return;
                }

                if ($result == null || ($row = $result->fetch_assoc()) == null) {

                    sLogger::getInstance()->putLog("[Error] Fail in proc update donation result empty.", $this->getPlataformName());

                    $this->setError('System error', 32001);

                    return;
                }

                if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                    sLogger::getInstance()->putLog("[Error] Fail in proc update donation id result is invalid", $this->getPlataformName());

                    $this->setError('System error', 32002);

                    return;
                }

                // Log
                sLogger::getInstance()->putLog("[Log] Player[UID=".PlayerSingleton::getInstance()['UID']."] Donation[CODE=".$transaction->code.", TYPE="
                    .$transaction->type.", STATUS=".$transaction->status.", ID=".$row['ID'].", EPIN_ID=".$epin_id."] atualizado com sucesso.", $this->getPlataformName());
                
                if ($epin_id != -1) {
                    
                    $msg = 'Doação registrada.<br>';
                    
                    if ($ret_make_epin['code'] == 0)
                        $msg .= 'O E-Pin foi enviado para o seu email cadastrado.';
                    else
                        $msg .= 'O E-Pin foi gerado porém não conseguiu enviar para o email cadastrado. Logue no site e entre em minhas doações para recuperar o E-Pin gerado.';
                
                    // Null == Message, != null error
                    $this->setError($msg, null);

                }else {

                    $msg = '';

                    if ($ret_make_epin['code'] == 1)
                        $msg = 'Doação registrada.<br>Mas ela é muito antiga para o E-Pin ser gerado.';
                    else if ($transaction->status == 1)
                        $msg = 'Doação registrada.<br>Aguardando pagamento, assim que for confirmado o pagamento o E-Pin vai ser enviado para o seu email cadastrado.';
                    else if ($transaction->status == 2)
                        $msg = 'Doação registrada.<br>Em analise, assim que for confirmado o E-Pin vai ser enviado para o seu email do cadastrado.';
                    else if ($transaction->status == 7)
                        $msg = 'Doação registrada, mas ela foi cancelada.';
                    else if ($transaction->status == 6 || $transaction->status == 8)
                        $msg = 'Doação registrada, mas ela foi devolvida.';
                    else
                        $msg = 'Doação registrada.<br>Aguardando liberação para o E-Pin ser enviado para o seu email cadastrado.';

                    // Null == Message, != null error
                    $this->setError($msg, null);
                }

                return;
            
            }else {

                // Log
                sLogger::getInstance()->putLog("[WARNING] Player[UID=".PlayerSingleton::getInstance()['UID']."] tentou registrar uma doação que foi gerado o E-Pin[ID="
                    .$transaction->epin_id."] mas não tem o UID registrado na doação[ID=".$transaction->id."]", $this->getPlataformName());

                $this->setError('Doação já foi registrada anteriormente.', 0);

                return;
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
                $query = 'select "_ID_" as "ID", "_EPIN_" as epin, "_EMAIL_" as email from '.$db->con_dados['DB_NAME'].'.ProcRegisterDonationEpin(?, ?, ?)';
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
    }
?>