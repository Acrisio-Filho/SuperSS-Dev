<?php
    // Arquivo transaction_paypal.inc
    // Criado em 06/12/2020 as 22:04 por Acrisio
    // Definição e Implementação da classe TransactionPayPal

    include_once('transaction_base.inc');
    include_once($_SERVER['DOCUMENT_ROOT'].'/EntryPoint/config/auth.inc');
    include_once('player_singleton.inc');

    class TransactionPayPal extends TransactionBase {

        private static $PARAMS_CONSULT = [
            'email' => PAY_AUTH_EMAIL,
            'token' => PAY_AUTH_TOKEN
        ];

        // !@ Teste real
        //private static $URL_CONSULT = "https://www.sandbox.paypal.com/cgi-bin/webscr";
        private static $URL_CONSULT = "https://www.paypal.com/cgi-bin/webscr";

        private static $VALUES_CONSULT_DB_CAMPATIBLE = [
            'date',
            'code',
            'reference',
            'type',
            'status',
            //'lastEventDate', esse aqui as vezes não tem por que foi gerado a transaction mas ainda não foi mandado nenhum notificação
            'grossAmount',
            'netAmount',
            //'escrowEndDate', esse só aparece quando foi pago, é a data que vai liberar o no pagseguro para usar
            'sender'
        ];

        private static $VALUES_CONSULT = [
            'payment_date',     // date
            'txn_id',           // code
            'txn_type',         // type
            'payment_status',   // status
            'payment_type',     // [reference] - Colocar no lugar da reference
            'mc_gross',         // grossAmount
            //'mc_fee',         // mc_fee - mc_groos = netAmount (esse só tem quando o status está completo)
            'mc_currency',      // usa para ver se é USD ou BRL
            //'exchange_rate', esse aqui é só quando tem troca de moeda (Usa no Convert Coin)
            //'settle_amount, esse aqui é só quando tem troca de moeda (Usa no Convert Coin)
            'payer_email'       // sender->email
        ];

        private static $USER_AGENT = 'PHP-PDT-SyncScript';

        public function __construct() {

            $this->state = true;
        }

        protected function getURLConsult() {
            return self::$URL_CONSULT;
        }

        protected function getParamsConsult() {
            return self::$PARAMS_CONSULT;
        }

        protected function getDataFromAPI($code) {
            
            $params = [
                'tx' => $code,
                'at' => self::$PARAMS_CONSULT['token']
            ];

            // Get info Transaction
            $req = 'cmd=_notify-synch&'.http_build_query($params);

            // Step 2: POST IPN data back to PayPal to validate
            $ch = curl_init($this->getURLConsult());

            // Options
            curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
            curl_setopt($ch, CURLOPT_POST, 1);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $req);
            curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 1);
            curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 2);
            curl_setopt($ch, CURLOPT_FORBID_REUSE, 1);
            curl_setopt($ch, CURLOPT_USERAGENT, self::$USER_AGENT);
            curl_setopt($ch, CURLOPT_HTTPHEADER, array('Connection: Close'));
            // In wamp-like environments that do not come bundled with root authority certificates,
            // please download 'cacert.pem' from "https://curl.haxx.se/docs/caextract.html" and set
            // the directory path of the certificate as shown below:
            curl_setopt($ch, CURLOPT_CAINFO, $_SERVER['DOCUMENT_ROOT'].'/EntryPoint/config/cacert.pem');

            if ( !($res = curl_exec($ch)) ) {

                // Get Errors
                $error = curl_error($ch);
                
                // Close
                curl_close($ch);

                sLogger::getInstance()->putLog("[Error][CURL] Fail to verify notification. Got ".$error." when send IPN data to API.", PAYPAL);

                $this->setError("System error", 6000);

                // Fail
                return null;
            }

            // Close
            curl_close($ch);
            
            // Verifica se falho
            if (strncmp($res, "FAIL", 4) == 0 || strncmp($res, "SUCCESS", 7) != 0) {

                sLogger::getInstance()->putLog("[Error] Fail to get transaction info. Reply: ".$res, PAYPAL);

                if (strstr($res, "\n") != null)
                    $res_error = str_replace("\n", "&", substr($res, 5));
                else
                    $res_error = $res;

                // Verifica se tem o Error
                if (strncasecmp($res_error, "Error", 5) == 0) {

                    $arr = explode(':', $res_error);

                    if (count($arr) == 2 && !is_nan($arr[1]) && $arr[1] == 4011) {

                        // Código de transação válido, porém não posso recuperar os dados com conta personal do paypal
                        // Aí não da para verifica se é uma doação verdadeira

                        // Cria uma transaction falsa para guarda no banco de dados com status de pending para eu verificar ela depois e deletar ou atualiza para enviar o email

                        // 'payment_date',     // date
                        // 'txn_id',           // code
                        // 'txn_type',         // type
                        // 'payment_status',   // status
                        // 'payment_type',     // [reference] - Colocar no lugar da reference
                        // 'mc_gross',         // grossAmount
                        // 'mc_fee',           // mc_fee - mc_groos = netAmount
                        // 'mc_currency',      // usa para ver se é USD ou BRL
                        // //'exchange_rate', esse aqui é só quando tem troca de moeda (Usa no Convert Coin)
                        // //'settle_amount, esse aqui é só quando tem troca de moeda (Usa no Convert Coin)
                        // 'payer_email'       // sender->email

                        // Log
                        sLogger::getInstance()->putLog("[WARNING] Player[UID=".PlayerSingleton::getInstance()['UID']."] Donation transaction code[CODE="
                            .$code."] don't have database more it is a code valid. Add to DB with status Pending to manul verify.", PAYPAL);

                        return [
                            'txn_type' => 'web_accept',
                            'txn_id' => $code,
                            'payment_status' => 'Pending',
                            'payment_date' => (new DateTime())->format('c'),
                            'payment_type' => 'Suspeita',
                            'mc_gross' => 0.0,
                            'mc_fee' => 0.0,
                            'mc_currency' => 'BRL',
                            'payer_email' => 'teste.teste.teste123@gmail.com' // Não tem como saber o email dele, se não consultar no banco de dados
                        ];
                    }
                
                }

                $this->setError("Transaction code invalid", 0);

                // Envia null por que ela é inválida
                return null;
            }

            $transaction_r = [];

            if (strstr($res, "\n") != null)
                $res_replace = str_replace("\n", "&", substr($res, 8));
            else
                $res_replace = $res;

            parse_str($res_replace, $transaction_r);

            if (!is_array($transaction_r) || empty($transaction_r)) {

                sLogger::getInstance()->putLog("[Error] Fail to decode transaction reply from API. Reply: ".$res, PAYPAL);

                $this->setError("System error", 6001);

                return null;
            }

            if (!isset($transaction_r['receiver_email'])) {

                sLogger::getInstance()->putLog("[Error] invalid transaction decoded array. Reply: ".json_encode($transaction_r), PAYPAL);

                $this->setError("System error", 6002);

                return null;
            }
            
            if (strtolower(PAY_AUTH_EMAIL) != $transaction_r['receiver_email']) {

                sLogger::getInstance()->putLog("[Error] receiver email not match. [".PAY_AUTH_EMAIL." != ".$transaction_r['receiver_email']."]", PAYPAL);

                $this->setError("System error", 6003);

                return null;
            }

            return $transaction_r;
        }

        protected function translateConsult($consult, $code) {

            if ($consult == null || !is_array($consult)) {

                if (DEBUG_ETP)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] consulting reply is invalid.", PAYPAL);

                return null;
            }

            $obj = simpleArrayToObject($consult);

            if ($obj == null) {

                if (DEBUG_ETP)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] fail to convert Array to Object.", PAYPAL);

                return null;
            }

            if (!is_object($obj)) {

                if (DEBUG_ETP)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);
                
                sLogger::getInstance()->putLog("[Error] invalid Object not is object", PAYPAL);

                return null;
            }

            // Verifica se ocorreu erro
            if (property_exists($obj, 'error')) {

                if (DEBUG_ETP)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] consulting error. $obj->error", PAYPAL);

                return null;
            }

            if (!$this->isValidObjectConsult($obj)) {

                if (DEBUG_ETP)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] consulting[CODE=$code] is invalid Object.", PAYPAL);

                return null;
            }

            $obj = $this->convertToSupportObjectDatabase($obj);

            if ($obj == null) {

                if (DEBUG_ETP)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] consulting[CODE=$code] fail to convert Database Object compatible.", PAYPAL);

                return null;
            }

            return $obj;
        }

        protected function isValidObjectConsult($obj) {

            if (!is_object($obj))
                return false;

            foreach (self::$VALUES_CONSULT as $prop) {

                if (!property_exists($obj, $prop))
                    return false; // Not exists
            }

            // Date PHP to Date Database
            $obj->payment_date = (new DateTime($obj->payment_date))->format("Y-m-d H:i:s.u");

            // mc_fee só tem quando a transaction está completa
            if (!property_exists($obj, 'mc_fee'))
                $obj->mc_fee = 0.0; // Por que ainda não está completa a transaction            

            // parent_txn_id (canceled, refound, devolving, canceled devolving)
            if (!property_exists($obj, 'parent_txn_id'))
                $obj->parent_txn_id = null;

            // Settle exchange coin
            // exchange_rate
            if (!property_exists($obj, 'exchange_rate'))
                $obj->exchange_rate = null;

            // settle_amount
            if (!property_exists($obj, 'settle_amount'))
                $obj->settle_amount = null;

            // settle_currency
            if (!property_exists($obj, 'settle_currency'))
                $obj->settle_currency = null;

            return true;
        }

        protected function convertToSupportObjectDatabase($obj) {

            if (!is_object($obj))
                return null;

            // 'payment_date',     // date
            // 'txn_id',           // code
            // 'txn_type',         // type
            // 'payment_status',   // status
            // 'payment_type',     // [reference] - Colocar no lugar da reference
            // 'mc_gross',         // grossAmount
            // 'mc_fee',           // mc_fee - mc_groos = netAmount
            // 'mc_currency',      // usa para ver se é USD ou BRL
            // //'exchange_rate', esse aqui é só quando tem troca de moeda (Usa no Convert Coin)
            // //'settle_amount, esse aqui é só quando tem troca de moeda (Usa no Convert Coin)
            // 'payer_email'       // sender->email

            $obj->date = $obj->payment_date;
            $obj->code = $obj->txn_id;
            $obj->type = ($obj->txn_type == 'send_money' || $obj->txn_type == 'web_accept') ? 1 : 2;

            // Usar no reference, esse vai da o valore, eCheck ou instant
            $obj->reference = $obj->payment_type;
            $obj->grossAmount = $obj->mc_gross;
            $obj->netAmount = ($obj->mc_gross - $obj->mc_fee);
            
            $obj->status = (function() use($obj) {

                $status = 1; // Aguardando pagamento

                switch ($obj->payment_status) {
                    case 'Pending':
                        $status = 2; // em analise
                    break;
                    case 'Completed':
                        $status = 4; // Disponivel
                    break;
                    case 'Refunded':
                        $status = 6; // Devolvido
                    break;
                    case 'Reversed':
                        $status = 8; // Cliente pediu para devolver(banco)
                    break;
                    case 'Canceled_Reversal':
                        $status = 4; // Disponivel, dinheiro voltou
                    break;
                    case 'Created':
                        $status = 1;
                    break;
                    case 'Denied':
                        $status = 7; // Cancelado
                    break;
                    case 'Expired':
                        $status = 7; // Cancelado
                    break;
                    case 'Failed':
                        $status = 7; // Cancelado
                    break;
                    case 'Processed':
                        $status = 1; // Aguardando pagamento
                    break;
                    case 'Voided':
                        $status = 1; // Aguardando pagamento
                    break;
                }

                return $status;
            })();

            if (!property_exists($obj, 'lastEventDate'))
                $obj->lastEventDate = (new DateTime())->format("Y-m-d H:i:s.u");

            if (!property_exists($obj, 'escrowEndDate')) {

                if ($obj->payment_status == "Completed")
                    $obj->escrowEndDate = (new DateTime())->format("Y-m-d H:i:s.u");
            }

            // Email
            $obj->sender = new stdClass();
            $obj->sender->email = $obj->payer_email;

            // Verifica se Currency é USD e faz as modificações
            if ($obj->mc_currency == 'USD' && $obj->payment_status == 'Completed') {

                if ($obj->exchange_rate != null && $obj->settle_amount != null) {

                    $obj->netAmount = $obj->settle_amount;

                    // Gross Amount em BRL por que o CP é baseado em BRL
                    $obj->grossAmount = $obj->exchange_rate * $obj->mc_gross;
                }
            }

            // Verifica mudança de estados em uma transaction por outra transaction
            if ($obj->parent_txn_id != null) {

                switch ($obj->payment_status) {
                case 'Refunded':
                    $obj->code = $obj->parent_txn_id;
                    $obj->status = 6; // Devolvido
                break;
                case 'Reversed':
                    $obj->code = $obj->parent_txn_id;
                    $obj->status = 8; // Cliente pediu para devolver(banco)
                break;
                case 'Canceled_Reversal':
                    $obj->code = $obj->parent_txn_id;
                    $obj->status = 4; // Disponivel, dinherio voltou
                break;
                }

            }

            return $obj;
        }
        
        protected function getPlataformName() {
            return PAYPAL;
        }

        protected function getPlataformType() {
            return eTYPE_PLATAFORM[PAYPAL];
        }

        protected function checkCode($code) {
            return strlen($code) == 17 || strlen($code) == 19;
        }
    }
?>