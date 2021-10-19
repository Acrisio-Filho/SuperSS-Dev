<?php
    // Arquivo notification_paypal.inc
    // Criado em 06/12/2020 as 15:15 por Acrisio
    // Definição e Implementação da classe NotificationPayPal

    include_once('notification_base.inc');

    class NotificationPayPal extends NotificationBase {

        private static $PARAMS_CONSULT = [
            'email' => PAY_AUTH_EMAIL,
            'token' => PAY_AUTH_TOKEN
        ];

        //private static $URL_CONSULT = "https://ipnpb.paypal.com/cgi-bin/webscr";
        private static $URL_CONSULT = "https://ipnpb.sandbox.paypal.com/cgi-bin/webscr";

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

        private static $USER_AGENT = 'PHP-IPN-VerificationScript';

        public function __construct() {

            // Set User-Agent
            ini_set('user_agent', self::$USER_AGENT);

            $this->checkValidRequest(); // Verifica se foi um request válido

            // Valid Request
            $this->state = true;
        }

        protected function getURLConsult() {
            return self::$URL_CONSULT;
        }

        protected function getParamsConsult() {
            return self::$PARAMS_CONSULT;
        }

        protected function getDataFromAPI() {
            // PayPal já envia os dados na notificação
            return (empty($_POST) || !is_array($_POST)) ? null : $_POST;
        }

        protected function translateConsultUpdate($consult) {
            
            // Invalid call, this class not translate Consult Update
            return null;
        }

        protected function translateConsult($consult) {

            if ($consult == null || !is_array($consult)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] consulting reply is invalid.", PAYPAL);

                return null;
            }

            $obj = simpleArrayToObject($consult);

            if ($obj == null) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] fail to convert Array to Object.", PAYPAL);

                return null;
            }

            if (!is_object($obj)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);
                
                sLogger::getInstance()->putLog("[Error] invalid Object not is object", PAYPAL);

                return null;
            }

            // Verifica se ocorreu erro
            if (property_exists($obj, 'error')) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] consulting[CODE=$this->code, TYPE=$this->type] error. $obj->error", PAYPAL);

                return null;
            }

            if (!$this->isValidObjectConsult($obj)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] consulting[CODE=$this->code, TYPE=$this->type] is invalid Object.", PAYPAL);

                return null;
            }

            $obj = $this->convertToSupportObjectDatabase($obj);

            if ($obj == null) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: ".json_encode($consult), PAYPAL);

                sLogger::getInstance()->putLog("[Error] consulting[CODE=$this->code, TYPE=$this->type] fail to convert Database Object compatible.", PAYPAL);

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
                        $status = 4; // Disponivel, dinherio voltou
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

        protected function checkValidRequest() {

            $raw_post_data = file_get_contents('php://input');

            if (!$this->checkWithAPINotification($raw_post_data))
                $this->exitAndPutLog("[Error] Fail verify notification. Notification[".urldecode($raw_post_data)."]", PAYPAL);

            if (!isset($_POST))
                $this->exitAndPutLog("[Error] invalid request(1)", PAYPAL);

            if (!isset($_POST['txn_id']) && isset($_POST['parent_txn_id']))
                $_POST['txn_id'] = $_POST['parent_txn_id'];

            if (!isset($_POST['txn_type']) && isset($_POST['parent_txn_id']))
                $_POST['txn_type'] = 'web_accept'; // Coloca esse por que é o do botão do paypal

            if (!isset($_POST['txn_id']) || !isset($_POST['txn_type']) || !isset($_POST['receiver_email']))
                $this->exitAndPutLog("[Error] invalid request(2)", PAYPAL);

            if (strlen($_POST['txn_id']) != 17 && strlen($_POST['txn_id']) != 19)
                $this->exitAndPutLog("[Error] invalid notification code. notify code=".$_POST['txn_id'], PAYPAL);

            if (strtolower(PAY_AUTH_EMAIL) != $_POST['receiver_email'])
                $this->exitAndPutLog("[Error] invalid notification[CODE=".$_POST['txn_id']."] receiver email not match. ["
                    .PAY_AUTH_EMAIL." != ".$_POST['receiver_email']."]", PAYPAL);

            $this->code = $_POST['txn_id'];
            $this->type = $_POST['txn_type'];

            // Log
            sLogger::getInstance()->putLog("[Log] Notification[CODE=$this->code, TYPE=$this->type]", PAYPAL);
        }

        protected function checkWithAPINotification($raw_post_data) {
            
            $raw_post_array = explode('&', $raw_post_data);
            
            $myPost = array();
            
            foreach ($raw_post_array as $keyval) {
                
                $keyval = explode ('=', $keyval);
            
                if (count($keyval) == 2)
                    $myPost[$keyval[0]] = urldecode($keyval[1]);
            }
            
            // read the IPN message sent from PayPal and prepend 'cmd=_notify-validate'
            $req = 'cmd=_notify-validate';
            
            foreach ($myPost as $key => $value) {

                $value = urlencode($value);
                
                $req .= "&$key=$value";
            }

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
            curl_setopt($ch, CURLOPT_CAINFO, $_SERVER['DOCUMENT_ROOT'].'/config/cacert.pem');

            if ( !($res = curl_exec($ch)) ) {

                // Get Errors
                $error = curl_error($ch);
                
                // Close
                curl_close($ch);

                $this->exitAndPutLog("[Error][CURL] Fail to verify notification. Got ".$error." when send IPN data to API.", PAYPAL);
            }

            // Close
            curl_close($ch);
            
            // inspect IPN validation result and act accordingly
            return strcmp ($res, "VERIFIED") == 0;
        }

        protected function getPlataformName() {
            return PAYPAL;
        }

        protected function getPlataformType() {
            return eTYPE_PLATAFORM[PAYPAL];
        }
    }
?>