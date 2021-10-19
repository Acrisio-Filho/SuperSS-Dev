<?php
    // Arquivo notification_pagseguro.inc
    // Criado em 02/12/2020 as 12:49 por Acrisio
    // Definição e Implementação da classe NotificationPagSeguro

    include_once('notification_base.inc');

    class NotificationPagSeguro extends NotificationBase {

        private static $PARAMS_CONSULT = [
            'email' => PAG_AUTH_EMAIL,
            'token' => PAG_AUTH_TOKEN
        ];

        private static $URL_CONSULT = "https://ws.pagseguro.uol.com.br/v3/transactions/notifications";
        //private static $URL_CONSULT = "https://ws.sandbox.pagseguro.uol.com.br/v3/transactions/notifications";

        private static $VALUES_CONSULT = [
            'date',
            'code',
            //'reference',
            'type',
            'status',
            //'lastEventDate', esse aqui as vezes não tem por que foi gerado a transaction mas ainda não foi mandado nenhum notificação
            'grossAmount',
            'netAmount',
            //'escrowEndDate', esse só aparece quando foi pago, é a data que vai liberar o no pagseguro para usar
            'sender'
        ];

        public function __construct() {

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
            return @file_get_contents($this->getURLConsult()."/".$this->code."?".http_build_query($this->getParamsConsult()));
        }

        protected function translateConsultUpdate($consult) {
            
            // Invalid call, this class not translate Consult Update
            return null;
        }

        protected function translateConsult($consult) {

            if ($consult == null) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting reply is invalid.", PAGSEGURO);

                return null;
            }

            if (!sXmlValidator::getInstance()->isXMLContentValid($consult)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting reply not is XML valid.", PAGSEGURO);

                return null;
            }

            $obj = simpleXMLToObject($consult);

            if ($obj == null) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] fail to convert XML to Object.", PAGSEGURO);

                return null;
            }

            if (!is_object($obj)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: $consult", PAGSEGURO);
                
                sLogger::getInstance()->putLog("[Error] invalid Object not is object", PAGSEGURO);

                return null;
            }

            // Verifica se ocorreu erro
            if (property_exists($obj, 'error')) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting[CODE=$this->code, TYPE=$this->type] error. $obj->error", PAGSEGURO);

                return null;
            }

            if (!$this->isValidObjectConsult($obj)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting[CODE=$this->code, TYPE=$this->type] is invalid Object.", PAGSEGURO);

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

            if (!property_exists($obj->sender, 'email'))
                return false;

            // Date PHP to Date Database
            $obj->date = (new DateTime($obj->date))->format("Y-m-d H:i:s.u");

            if (property_exists($obj, 'lastEventDate') && $obj->lastEventDate != null)
                $obj->lastEventDate = (new DateTime($obj->lastEventDate))->format("Y-m-d H:i:s.u");
            else
                $obj->lastEventDate = null;

            if (property_exists($obj, 'escrowEndDate') && $obj->escrowEndDate != null)
                $obj->escrowEndDate = (new DateTime($obj->escrowEndDate))->format("Y-m-d H:i:s.u");
            else
                $obj->escrowEndDate = null;

			// Doação não tem código de reference
            if (!property_exists($obj, 'reference'))
                $obj->reference = 'PAG-DONATION-2020';

			if ($obj->type == 6/*Doação*/)
                $obj->type = 1; // Pagamento para não da erro nos tipos da classe base por causo do PayPal

            return true;
        }

        protected function checkValidRequest() {

            if (!isset($_POST))
                $this->exitAndPutLog("[Error] invalid request(1)", PAGSEGURO);

            if (!isset($_POST['notificationCode']) || !isset($_POST['notificationType']))
                $this->exitAndPutLog("[Error] invalid request(2)", PAGSEGURO);

            if (strlen($_POST['notificationCode']) != 39)
                $this->exitAndPutLog("[Error] invalid notification code. notify code=".$_POST['notificationCode'], PAGSEGURO);

            $this->code = $_POST['notificationCode'];
            $this->type = $_POST['notificationType'];

            // Log
            sLogger::getInstance()->putLog("[Log] Notification[CODE=$this->code, TYPE=$this->type]", PAGSEGURO);
        }

        protected function getPlataformName() {
            return PAGSEGURO;
        }

        protected function getPlataformType() {
            return eTYPE_PLATAFORM[PAGSEGURO];
        }
    }

?>