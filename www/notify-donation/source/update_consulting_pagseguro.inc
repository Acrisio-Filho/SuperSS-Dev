<?php
    // Arquivo update_consulting_pagseguro.php
    // Criado em 04/12/2020 as 13:19 por Acrisio
    // Definição e Implementação da classe UpdateConsultingPagSeguro

    include_once('notification_base.inc');

    class ConsultingUpdatePagSeguro extends NotificationBase {

        private static $MODO_PAG_DONA = true; // Modo para consultar e inserir doção no modo de doação, já que o modo convencional não funciona

        private static $PARAMS_CONSULT = [
            'email' => PAG_AUTH_EMAIL,
            'token' => PAG_AUTH_TOKEN,
            'initialDate' => '',
            'finalDate' => '',
            'maxPageResults' => 100
        ];

        private static $URL_CONSULT = "https://ws.pagseguro.uol.com.br/v3/transactions";
        //private static $URL_CONSULT = "https://ws.sandbox.pagseguro.uol.com.br/v3/transactions";

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
            //'sender'
        ];

        private static $VALUES_CONSULT_UPDATE = [
            'date',
            'code',
            //'reference',
            'type',
            'status',
            //'lastEventDate', esse aqui as vezes não tem por que foi gerado a transaction mas ainda não foi mandado nenhum notificação
            'grossAmount',
            'netAmount'
        ];

        public function __construct() {

            $this->checkValidRequest(); // Verifica se foi um request válido

            // Data PagSeguro é horário de Brasilia
            $timezone = new DateTimeZone('America/Sao_Paulo');
            
            $now = new DateTime(null, $timezone);
            $initDate = (new DateTime(null, $timezone))->modify('-30 day');

            // Initializa as date da consulta
            self::$PARAMS_CONSULT['initialDate'] = $initDate->format("Y-m-d\TH:i");
            self::$PARAMS_CONSULT['finalDate'] = $now->format("Y-m-d\TH:i");

            // Valid Request
            $this->state = true;
        }

        // Override
        public function isValid() {
            return $this->state;
        }

        protected function getURLConsult() {
            return self::$URL_CONSULT;
        }

        protected function getParamsConsult() {
            return self::$PARAMS_CONSULT;
        }

        protected function getDataFromAPI() {
            
            if (self::$MODO_PAG_DONA) {

                $copy_params = [
                    'email' => PAG_AUTH_EMAIL,
                    'token' => PAG_AUTH_TOKEN
                ];

                // Data PagSeguro é horário de Brasilia
                $timezone = new DateTimeZone('America/Sao_Paulo');
                
                $now = new DateTime(null, $timezone);
                $initDate = (new DateTime(null, $timezone))->modify('-30 day');

                // Initializa as date da consulta
                $copy_params['initialDate'] = $initDate->format("Y-m-d\TH:i");
                $copy_params['finalDate'] = $now->format("Y-m-d\TH:i");

                return @file_get_contents($this->getURLConsult()."?".http_build_query($copy_params));

            }else
                return @file_get_contents($this->getURLConsult()."/".$this->code."?".http_build_query($this->getParamsConsult()));
        }

        protected function translateConsultModoDonation($consult) {

            $transactions = $this->translateConsultUpdate($consult);

            if ($transactions == null || (!is_array($transactions) && !is_object($transactions)))
                return null;

            if (is_object($transactions) && ($transaction->code != $this->code)) {

                sLogger::getInstance()->putLog("[Error][Consulting] Fail to find transaction[CODE=$this->code, TYPE=$this->type] in result last 30 days.", $this->getPlataformName());

                return null; // Não encontrou a transaction na consulta

            }else {
            
                // Procura a transaction nas transaction retornada
                $it = array_find($transactions, function($el) {
                    return $el->code == $this->code;
                });

                if ($it == null) {

                    sLogger::getInstance()->putLog("[Error][Consulting] Fail to find transaction[CODE=$this->code, TYPE=$this->type] in result last 30 days.", $this->getPlataformName());

                    return null; // Não encontrou a transaction na consulta
                }

                // Founded Transaction
                $transactions = $it;
            }

            return objectToSimpleXML($transactions);
        }

        protected function _translateConsult($consult) {

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

        protected function translateConsultUpdate($consult) {

            if ($consult == null) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting update reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting update reply is invalid.", PAGSEGURO);

                return null;
            }

            if (!sXmlValidator::getInstance()->isXMLContentValid($consult)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting update reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting update reply not is XML valid.", PAGSEGURO);

                return null;
            }

            $obj = simpleXMLToObject($consult);

            if ($obj == null) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consult update fail to convert XML to Object.", PAGSEGURO);

                return null;
            }

            if (!is_object($obj)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting update reply: $consult", PAGSEGURO);
                
                sLogger::getInstance()->putLog("[Error] consult update invalid Object not is object", PAGSEGURO);

                return null;
            }

            // Verifica se ocorreu erro
            if (property_exists($obj, 'error')) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting update reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting update error. $obj->error", PAGSEGURO);

                return null;
            }

            // Verifica se veio uma consulta vazia
            if (property_exists($obj, 'resultsInThisPage') && $obj->resultsInThisPage <= 0) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting update nothing results in reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting update nothing results in reply.", PAGSEGURO);

                return null;
            }

            if (!$this->isValidObjectConsultUpdate($obj)) {

                if (DEBUG_NOTIFY)
                    sLogger::getInstance()->putLog("[Error][DEBUG] consulting update reply: $consult", PAGSEGURO);

                sLogger::getInstance()->putLog("[Error] consulting update is invalid Object.", PAGSEGURO);

                return null;
            }

            return $obj->transactions->transaction;
        }

        protected function translateConsult($consult) {

            // Verifica se está ligado o modo consulta de doação
            if (self::$MODO_PAG_DONA)
                return $this->_translateConsult(
                    $this->translateConsultModoDonation($consult)
                );
            else
                return $this->_translateConsult($consult);
        }

        protected function isValidObjectConsultUpdate($obj) {

            if (!is_object($obj))
                return false;

            if (!property_exists($obj, 'transactions'))
                return false;

            if (!property_exists($obj->transactions, 'transaction'))
                return false;

            if (!is_array($obj->transactions->transaction) && !is_object($obj->transactions->transaction))
                return false;

            if (is_object($obj->transactions->transaction)) {

                // Somente uma transaction
                foreach (self::$VALUES_CONSULT_UPDATE as $prop) {

                    if (!property_exists($obj->transactions->transaction, $prop))
                        return false; // Not exists
                }

                // Date PHP to Date Database
                $obj->transactions->transaction->date = (new DateTime($obj->transactions->transaction->date))->format("Y-m-d H:i:s.u");

                if (property_exists($obj->transactions->transaction, 'lastEventDate') && $obj->transactions->transaction->lastEventDate != null)
                    $obj->transactions->transaction->lastEventDate = (new DateTime($obj->transactions->transaction->lastEventDate))->format("Y-m-d H:i:s.u");
                else
                    $obj->transactions->transaction->lastEventDate = null;

                if (property_exists($obj->transactions->transaction, 'escrowEndDate') && $obj->transactions->transaction->escrowEndDate != null)
                    $obj->transactions->transaction->escrowEndDate = (new DateTime($obj->transactions->transaction->escrowEndDate))->format("Y-m-d H:i:s.u");
                else
                    $obj->transactions->transaction->escrowEndDate = null;

                // Doação não tem código de reference
                if (!property_exists($obj->transactions->transaction, 'reference'))
                    $obj->transactions->transaction->reference = 'PAG-DONATION-2020';

                if ($obj->transactions->transaction->type == 6/*Doação*/)
                    $obj->transactions->transaction->type = 1; // Pagamento para não da erro nos tipos da classe base por causo do PayPal

            }else {

                // Mais de uma transaction
                foreach ($obj->transactions->transaction as $transaction) {

                    foreach (self::$VALUES_CONSULT_UPDATE as $prop) {

                        if (!property_exists($transaction, $prop))
                            return false; // Not exists
                    }

                    // Date PHP to Date Database
                    $transaction->date = (new DateTime($transaction->date))->format("Y-m-d H:i:s.u");

                    if (property_exists($transaction, 'lastEventDate') && $transaction->lastEventDate != null)
                        $transaction->lastEventDate = (new DateTime($transaction->lastEventDate))->format("Y-m-d H:i:s.u");
                    else
                        $transaction->lastEventDate = null;

                    if (property_exists($transaction, 'escrowEndDate') && $transaction->escrowEndDate != null)
                        $transaction->escrowEndDate = (new DateTime($transaction->escrowEndDate))->format("Y-m-d H:i:s.u");
                    else
                        $transaction->escrowEndDate = null;

                    // Doação não tem código de reference
                    if (!property_exists($transaction, 'reference'))
                        $transaction->reference = 'PAG-DONATION-2020';

                    if ($transaction->type == 6/*Doação*/)
                        $transaction->type = 1; // Pagamento para não da erro nos tipos da classe base por causo do PayPal
                }
            }

            return true;
        }

        protected function isValidObjectConsult($obj) {

            if (!is_object($obj))
                return false;

            foreach (self::$VALUES_CONSULT as $prop) {

                if (!property_exists($obj, $prop))
                    return false; // Not exists
            }

            if (self::$MODO_PAG_DONA) {

                // Cria Email inválido para cadastrar a doação por consulta geral
                if (!property_exists($obj, 'sender') || !property_exists($obj->sender, 'email'))
                    $obj->sender = (function () {
                        
                        $new_sender = new stdClass();
                        $new_sender->email = 'email-ivalid-pag-donation-modo@invalid.invalid.com';

                        return $new_sender;
                    })();

            }else if (!property_exists($obj, 'sender') || !property_exists($obj->sender, 'email'))
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

            // Esse não recebe nenhum parametro de fora
            return;
        }

        protected function getPlataformName() {
            return PAGSEGURO;
        }

        protected function getPlataformType() {
            return eTYPE_PLATAFORM[PAGSEGURO];
        }
    }
?>