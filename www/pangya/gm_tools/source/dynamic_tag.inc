<?php
    // Arquivo dynamic_tag.inc
    // Criado em 07/11/2020 as 05:39 por Acrisio
    // Difinição e Implementação da classe DynamicTag

    class DynamicTag {

        private static $basic_properts = ['is_single', 'name', 'childs'];

        protected $is_single;
        protected $name;
        protected $childs = [];

        public function __construct($name, $is_single = false) {

            $this->is_single = $is_single;
            $this->name = $name;
        }

        public function addChild($child) {
            
            array_push($this->childs, $child);

            return $this;
        }

        public function toString() {

            $ret = '<'.$this->name;

            foreach ($this as $k => $el) {

                if (!in_array(strtolower($k), self::$basic_properts) && !is_array($el) && !is_object($el))
                    $ret .= ' '.$k.'="'.$el.'"';
            }

            $ret .= '>';

            if ($this->is_single)
                return $ret;

            if (!empty($this->childs)) {

                foreach ($this->childs as $el) {
                    
                    if ($el instanceof DynamicTag)
                        $ret .= $el->toString();
                    else if (is_string($el))
                        $ret .= $el;
                    else if (is_object($el) && property_exists($el, 'toString'))
                        $ret .= $el->toString();
                }
            }

            $ret .= '</'.$this->name.'>';

            return $ret;
        }
    }

?>