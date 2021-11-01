<?php
    // Arquivo table.inc
    // Criado em 23/05/2020 as 22:27 por Acrisio
    // Definição e Implementação da classe Table

    class table {

        private const max_pages = 10;
        private $max_rows = 12;

        private $rows = 0;
        private $pages = 0;
        
        // Pages
        private $first = 0;
        private $prev = 0;
        private $last = 0;
        private $next = 0;

        private $current_row = [];
        private $page = 0;

        public function __construct($max_rows, $page, $rows, $rows_data) {

            // Max Rows
            $this->max_rows = $max_rows;

            // Init Rows
            $this->rows = $rows;

            // Init Page
            $this->page = $page;

            // Init Rows Data
            $this->current_row = $rows_data;

            if (!$this->isEmpty()) {
               
                // Init Values
                $this->pages = ($this->rows % $this->max_rows == 0) ? (int)($this->rows / $this->max_rows) : (int)($this->rows / $this->max_rows) + 1;

                if ($this->page > ($this->pages - 1))
                    $this->page = $this->pages - 1;

                $this->first = 0;

                if ($this->page <= 1) 
                    $this->prev = 0;
                else
                    $this->prev = $this->page - 1;

                $this->last = $this->pages -1;

                if ($this->page < ($this->pages - 1))
                    $this->next = $this->page + 1;
                else
                    $this->next = $this->pages - 1;
            }
        }

        public function getFirst() {
            return $this->first;
        }

        public function getPrev() {
            return $this->prev;
        }

        public function getNext() {
            return $this->next;
        }

        public function getLast() {
            return $this->last;
        }

        public function getCurrentRow() {
            return $this->current_row;
        }

        public function isEmpty() {
            return $this->rows <= 0 && empty($this->current_row); 
        }

        public function makeListLink($link, $linkGets) {

            $begin = (int)($this->page / table::max_pages) * table::max_pages;
            $end = (($begin + table::max_pages) > $this->pages) ? $this->pages : $begin + table::max_pages; 

            $ret = '';

            if ($begin >= 0 && $end > 0) {
                
                for ($i = $begin; $i < $end; ++$i)
                    $ret .= '<a href="'.$link.'?page='.($i + 1).$linkGets.'">['.($i + 1).']</a>';
            }

            return $ret;
        }
    }
?>