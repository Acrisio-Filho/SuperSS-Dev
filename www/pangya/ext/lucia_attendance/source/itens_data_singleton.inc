<?php
    // Arquivo itens_data_singleton.inc
    // Criado em 28/02/2020 as 05:43 por Acrisio
    // Definição do singleton da class itens

    include_once('debug_log.inc');

    // Itens
    class Itens {

        private $item = null;
        private $card = null;
        private $state = false;

        public function __construct() {
            $this->init();
        }

        private function init() {

            if (!$this->state) {

                $this->item = json_decode(file_get_contents(BASE_LUCIA_ATTENDANCE_DIRECTORY."/source/items.json"));
                $this->card = json_decode(file_get_contents(BASE_LUCIA_ATTENDANCE_DIRECTORY."/source/cards.json"));

                if (!$this->item)
                    DebugLog::Log("Não conseguiu carregar os itens do arquivo json.");

                if (!$this->card)
                    DebugLog::Log("Não conseguiu carregar os cards do arquivo json.");

                $this->state = ($this->item && $this->card);

            }else
                DebugLog::Log("Class Itens já está inicializada.");
        }

        private function close() {

            if ($this->item)
                $this->item = null;

            if ($this->card)
                $this->card = null;

            $this->state = false;
        }

        public function reload() {

            if ($this->state)
                $this->close();

            $this->init();
        }

        public function getItens() {
            return $this->item;
        }

        public function getCards() {
            return $this->card;
        }
    }

    // Singleton
    class ItemSingleton {

        private static $item = null;

        public static function &getInstance() {

            if (self::$item == null)
                self::$item = new Itens();

            return self::$item;
        }
    }
?>