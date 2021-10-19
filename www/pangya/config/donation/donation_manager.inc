<?php
    // Arquivo donation_manager.inc
    // Criado em 09/12/2020 as 01:52 por Acrisio
    // Definição da classe DonationManager

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('DONATION_MANAGER', "DONATION_MANAGER");
    define('DONATION_TO_CP', 20);                       // 20 CP por 1 real

    class DonationManager {

        protected static $DateBegin = "2020-08-00T00:00:00.000-03:00";

        protected $db = null;
        protected $params = null;

        public function __construct($db) {

            $this->db = $db;
            $this->params = $this->db->params;
        }

        public function deleteTransaction($id) {

            if ($id < 0) {

                $this->putLog("[Error] Invalid transaction ID", DONATION_MANAGER);

                return 4; // Invalid transaction id
            }

            // Delete transaction by GM
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcDeleteDonationByGM ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$this->db->con_dados['DB_NAME'].'.ProcDeleteDonationByGM(?)';
            else
                $query = 'call '.$this->db->con_dados['DB_NAME'].'.ProcDeleteDonationByGM(?)';

            $this->params->clear();
            $this->params->add('i', $id);

            if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) == null && $this->db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail to exec query proc delete donation by gm. Error code: ".$this->db->db->getLastError(), DONATION_MANAGER);

                return 1;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc delete donation by gm, result empty.", DONATION_MANAGER);

                return 2;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in proc delete donation by gm, id result is invalid", DONATION_MANAGER);

                return 3;
            }

            // Log
            $this->putLog("[Log] Player(GM)[UID=".$_SESSION['player_log']['uid']."] Donation[ID="
                .$row['ID']."] deletada com sucesso.", DONATION_MANAGER);

            return 0; // Sucesso
        }

        public function insertTransaction($transaction) {

            // Registra a doação no banco de dados
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcInsertDonationNew ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$this->db->con_dados['DB_NAME'].'.ProcInsertDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';
            else
                $query = 'call '.$this->db->con_dados['DB_NAME'].'.ProcInsertDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';

            $this->params->clear();
            $this->params->add('i', $transaction->uid);
            $this->params->add('i', $transaction->plataform);
            $this->params->add('s', $transaction->sender->email);
            $this->params->add('s', $transaction->date);
            $this->params->add('s', $transaction->lastEventDate);
            $this->params->add('s', $transaction->code);
            $this->params->add('i', $transaction->type);
            $this->params->add('i', $transaction->status);
            $this->params->add('s', $transaction->reference);
            $this->params->add('i', $transaction->grossAmount);
            $this->params->add('i', $transaction->netAmount);
            $this->params->add('s', $transaction->escrowEndDate);

            if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) == null && $this->db->db->getLastError() != 0) {
                
                $this->putLog("[Error] Fail to exec query proc insert donation. Error code: ".$this->db->db->getLastError(), DONATION_MANAGER);

                return 1;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc insert donation result empty.", DONATION_MANAGER);

                return 2;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in proc insert donation id result is invalid", DONATION_MANAGER);

                return 3;
            }

            // Log
            $this->putLog("[Log] Player(GM)[UID=".$_SESSION['player_log']['uid']."] Donation[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status, ID="
                .$row['ID']."] adicionado com sucesso.", DONATION_MANAGER);

            // MakeEpinAnd Send
            $ret_make_epin = $this->makeEpinAndSend($transaction, [
                'index' => $row['ID'],
                'uid' => $transaction->uid,
                'epin_id' => -1
            ]);

            $epin_id = $ret_make_epin['epin_id'];

            if ($epin_id != -1) {

                // Update Donation
                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcUpdateDonationNew ?, ?, ?, ?, ?, ?, ?, ?, ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_ID_" as "ID" from '.$this->db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';
                else
                    $query = 'call '.$this->db->con_dados['DB_NAME'].'.ProcUpdateDonationNew(?, ?, ?, ?, ?, ?, ?, ?, ?)';

                $update = (new DateTime())->format("Y-m-d H:i:s.u");

                $this->params->clear();
                $this->params->add('s', $transaction->code);
                $this->params->add('i', $transaction->plataform);
                $this->params->add('i', $transaction->uid);
                $this->params->add('i', $transaction->status);
                $this->params->add('s', $update);
                $this->params->add('i', $epin_id);
                $this->params->add('i', $transaction->grossAmount);
                $this->params->add('i', $transaction->netAmount);
                $this->params->add('s', $transaction->escrowEndDate);

                if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) == null && $this->db->db->getLastError() != 0) {

                    $this->putLog("[Error] Fail to exec query proc update donation. Error code: ".$this->db->db->getLastError(), DONATION_MANAGER);

                    return 4;
                }

                if ($result == null || ($row = $result->fetch_assoc()) == null) {

                    $this->putLog("[Error] Fail in proc update donation result empty.", DONATION_MANAGER);

                    return 5;
                }

                if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                    $this->putLog("[Error] Fail in proc update donation id result is invalid", DONATION_MANAGER);

                    return 6;
                }

                // Log
                $this->putLog("[Log] Player(GM)[UID=".$_SESSION['player_log']['uid']."] Donation[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status, ID="
                    .$row['ID'].", EPIN_ID=".$epin_id."] atualizado com sucesso.", DONATION_MANAGER);

            }

            return 0; // Sucesso
        }

        public function updateTransaction($transaction) {

            // Verifica se já tem ela no banco de dados
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT [index], [uid], [status], [update], [epin_id] FROM '.$this->db->con_dados['DB_NAME'].'.pangya_donation_new WHERE [code] = ? and [plataforma] = ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT index, uid, status, update, epin_id FROM '.$this->db->con_dados['DB_NAME'].'.pangya_donation_new WHERE code = ? and plataforma = ?';
            else
                $query = 'SELECT `index`, uid, `status`, `update`, epin_id FROM '.$this->db->con_dados['DB_NAME'].'.pangya_donation_new WHERE `code` = ? and plataforma = ?';

            $this->params->clear();
            $this->params->add('s', $transaction->code);
            $this->params->add('i', $transaction->plataform);

            if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) == null && $this->db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail to consulting[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status] in db. error code: ".$this->db->db->getLastError(), DONATION_MANAGER);

                return 1;

            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in consulting[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status] in db, result empty", DONATION_MANAGER);

                return 2;
            }

            if (!isset($row['index']) || !isset($row['uid']) || !isset($row['epin_id']) || !isset($row['status']) || !key_exists('update', $row) || is_nan($row['status'])) {
                
                $this->putLog("[Error] Failt in query select donation, result is invalid.", DONATION_MANAGER);

                return 3;
            }

            // Update epin_id, só se for maior que 0
            if ($transaction->epin_id > 0)
                $row['epin_id'] = $transaction->epin_id;

            // Make Epin and send
            $ret_make_epin = $this->makeEpinAndSend($transaction, $row);

            $epin_id = $ret_make_epin['epin_id'];

            // Update Donation
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcUpdateDonationNewByGM ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$this->db->con_dados['DB_NAME'].'.ProcUpdateDonationNewByGM(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';
            else
                $query = 'call '.$this->db->con_dados['DB_NAME'].'.ProcUpdateDonationNewByGM(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';

            // Check Update time is null, make now
            if ($transaction->lastEventDate != null)
                $update = $transaction->lastEventDate;
            else
                $update = (new DateTime())->format("Y-m-d H:i:s.u");

            $this->params->clear();
            $this->params->add('i', $transaction->id);
            $this->params->add('s', $transaction->code);
            $this->params->add('i', $transaction->plataform);
            $this->params->add('i', $transaction->uid);
            $this->params->add('i', $transaction->status);
            $this->params->add('s', $update);
            $this->params->add('i', $epin_id);
            $this->params->add('i', $transaction->grossAmount);
            $this->params->add('i', $transaction->netAmount);
            $this->params->add('s', $transaction->escrowEndDate);
            $this->params->add('s', $transaction->sender->email);
            $this->params->add('s', $transaction->date);
            $this->params->add('i', $transaction->type);
            $this->params->add('s', $transaction->reference);

            if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) == null && $this->db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail to exec query proc update donation. Error code: ".$this->db->db->getLastError(), DONATION_MANAGER);

                return 4;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc update donation result empty.", DONATION_MANAGER);

                return 5;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in proc update donation id result is invalid", DONATION_MANAGER);

                return 6;
            }

            // Log
            $this->putLog("[Log] Player(GM)[UID=".$_SESSION['player_log']['uid']."] Donation[CODE=$transaction->code, TYPE=$transaction->type, STATUS=$transaction->status, ID="
                .$row['ID'].", EPIN_ID=".$epin_id."] atualizado com sucesso.", DONATION_MANAGER);

            return 0; // Success
        }

        protected function makeEpinAndSend($transaction, $donation_row) {

            // Verifica se não tem o epin
            if ($donation_row['epin_id'] <= 0)
                $donation_row['epin_id'] = -1;

            // Invalid date Donation
            if ((new DateTime(self::$DateBegin)) > (new DateTime($transaction->date))) {
                
                $this->putLog("Player[UID=".$donation_row['uid']."] donation[ID=".$donation_row['index'].", DATE=".$transaction->date."] date is oldest.", DONATION_MANAGER);
                
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

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcRegisterDonationEpin ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID", "_EPIN_" as epin, "_EMAIL_" as email from '.$this->db->con_dados['DB_NAME'].'.ProcRegisterDonationEpin(?, ?, ?)';
            else
                $query = 'call '.$this->db->con_dados['DB_NAME'].'.ProcRegisterDonationEpin(?, ?, ?)';

            $this->params->clear();
            $this->params->add('i', $donation_row['index']);   // Donation ID
            $this->params->add('i', $donation_row['uid']);     // Player UID
            $this->params->add('i', $value);

            if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) == null && $this->db->db->getLastError() != 0) {

                $this->putLog("[Error] DONATION[ID=".$donation_row['index']."] Player[UID="
                    .$donation_row['uid']."] - Fail to exec proc register donation epin. Error code: ".$this->db->db->getLastError(), DONATION_MANAGER);

                return [
                    'code' => 5,
                    'epin_id' => $donation_row['epin_id']
                ];
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] DONATION[ID=".$donation_row['index']."] Player[UID=".$donation_row['uid']."] - Fail in proc register donation epin, result empty.", DONATION_MANAGER);

                return [
                    'code' => 6,
                    'epin_id' => $donation_row['epin_id']
                ];
            }

            if (!isset($row['ID']) || $row['ID'] == -1 || is_nan($row['ID']) || !isset($row['epin']) || !isset($row['email'])) {

                $this->putLog("[Error] DONATION[ID=".$donation_row['index']."] Player[UID=".$donation_row['uid']."] - Fail in proc register donation epin, result invalid.", DONATION_MANAGER);

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
                    $this->putLog("[Error] Fail to send epin to $email. Error: ".$error, DONATION_MANAGER);

                return false;
            }

            // Log
            $this->putLog("[Log] E-Pin: $epin foi enviado para o e-mail: $email com sucesso.", DONATION_MANAGER);

            return true;
        }

        public function putLog($log, $type) {

            // PHP error log
            error_log("[".$this->getIP()."][".$type."]".$log, 0);
        }

        private function getIP() {

            $ip = ':::?';

            $ip = getenv("REMOTE_ADDR") ?? $ip;

            return $ip;
        }
    }
?>