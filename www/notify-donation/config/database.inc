<?php
    // Arquivo database.inc
    // Criado em 01/08/2021 as 13:44 por Acrisio
    // Definição das classe basa para usar com database

    class paramsArr {
		private $arr = [];
		private $tipo = '';
        private $ModelType = 1;

        public function __construct($model_type) {
            $this->ModelType = $model_type;
        }

		public function __destruct() {
			$this->clear();
		}

		public function add($tipo, &$valor) {

            if ($this->ModelType == 1) {

                if ($valor === null)    // True Null Value
                    $this->arr[] = null;
                else
                    $this->arr[] = &$valor;
                
            }else
                $this->arr[] = &$valor;

			$this->tipo .= $tipo;
		}

        public function get() {
            return $this;
        }

		public function getAll() {
			return array_merge([$this->tipo], $this->arr);
        }
        
        public function getValues() {
            return $this->arr;
        }

        public function getTipo() {
            return [$this->tipo];
        }

		public function clear() {
			if (!empty($this->arr)) {
				$this->arr = [];
			}

			if (!empty($this->tipo)) {
				$this->tipo = '';
			}
		}
	}

    class Statment {
        private $conn = false;
        private $query = '';

        private $is_good = false;
        private $is_exec = false;
        private $stmt = false;
        
        public $num_rows = -1;

        public $errno = '';
        public $error = '';

        public function __construct($conn, $query) {

            if (!$conn || !isset($query) || empty($query)) {
                $is_good = false;

                $this->errno = 1;

                if (!$conn)
                    $this->error = '[Statment][Error] conexão invalida.';
                else
                    $this->error = '[Statment][Error] query invalida';

                return;
            }
            
            $this->conn = $conn;
            $this->query = $query;

            $this->init();
        }

        public function isGood() {
            return $this->is_good;
        }

        public function initError() {
            $this->errno = odbc_error($this->conn);
            $this->error = odbc_errormsg($this->conn);
        }

        public function init() {

            if ($this->is_good) {
                $this->errno = 20;
                $this->error = '[Statment][Error] O statment já está inicializado.';
                return;
            }

            @$stmt = odbc_prepare($this->conn, $this->query);

            if (!$stmt) {
                $this->initError();
                return;
            }

            $this->stmt = $stmt;

            $this->is_good = true;
        }

        public function execute(...$args) {

            if (!$this->is_good) {
                $this->errno = 3;
                $this->error = '[Statment][Error] O statment não está inicializado.';
                return false;
            }

            $num_args = count($args);

            if ($num_args > 0 && (!isset($args[0]) || empty($args[0]))) {
                $this->errno = 2;
                $this->error = '[Statment][Error] invalid arguments';
                return false;
            }

            if ($num_args > 0) {
                if (@!odbc_execute($this->stmt, $args[0])) {
                    $this->initError();
                    if (empty($this->errno)) {
                        $this->errno = 6;
                        $this->error = '[Statment][Error] Pouco parametros para executar o prepared statment.';
                    }
                    return false;
                }
            }else if (@!odbc_execute($this->stmt)) {
                $this->initError();
                if (empty($this->errno)) {
                    $this->errno = 5;
                    $this->error = '[Statment][Error] Para executar a query, é necessário paremetros para executar o prepared statment.';
                }
                return false;
            }

            @$this->num_rows = odbc_num_rows($this->stmt);

            /*if ($this->num_rows == -1) {
                $this->initError();
                if ($this->errno != '01000') { // Aviso de STORED PROCEDURE que usa o SET NOCOUNT ON
                    if (empty($this->errno)) {
                        $this->errno = 7;
                        $this->error = '[Statment][Error] Muitos parametros passado para executar o prepared statment.';
                    }
                    return false;
                }
            }*/

            $this->is_exec = true;

            return true;
        }

        public function get_result() {
           
            if (!$this->is_good) {
                $this->errno = 3;
                $this->error = '[Statment][Error] O Statment não está inicializado.';
                return false;
            }

            if (!$this->is_exec) {
                $this->errno = 4;
                $this->error = '[Statment][Error] O Statment não foi executado ainda, chame a função execute primeiro.';
                return false;
            }

            return $this;
        }

        public function free() {
            if ($this->is_good)
                odbc_free_result($this->stmt); // Sempre retorna TRUE

           $this->close();
        }

        public function close() {
            $this->stmt = null;
            $this->is_good = false;
            $this->is_exec = false;
            $this->error = '';
            $this->errno = '';
        }

        public function next_result() {

            if (!$this->is_good) {
                $this->errno = 3;
                $this->error = '[Statment][Error] O Statment não está inicializado.';
                return false;
            }

            if (!$this->is_exec) {
                $this->errno = 4;
                $this->error = '[Statment][Error] O Statment não foi executado ainda, chame a função execute primeiro.';
                return false;
            }

            return odbc_next_result($this->stmt);
        }

        public function fetch_assoc() {
            
            if (!$this->is_good) {
                $this->errno = 3;
                $this->error = '[Statment][Error] O Statment não está inicializado.';
                return false;
            }

            if (!$this->is_exec) {
                $this->errno = 4;
                $this->error = '[Statment][Error] O Statment não foi executado ainda, chame a função execute primeiro.';
                return false;
            }

            // propria função da ODBC no PHP, mas ela não está funcionando comigo
            @$arr = odbc_fetch_array($this->stmt);

            if (!is_array($arr) && !$arr) {
                $this->initError();
                return false;
            }

            /*$arr = [];
            $error = 0;
            $num_cols = 0;
            $i = 0;

            while (($error = @odbc_fetch_row($this->stmt)) == TRUE) {

                if (($num_cols = @odbc_num_fields($this->stmt)) == -1) {
                    $this->initError();
                    return false;
                }

                for ($i = 1; $i <= $num_cols; ++$i) {
                    
                    if (!($field_name = @odbc_field_name($this->stmt, $i))) {
                        $this->initError();
                        return false;
                    }

                    $arr[$field_name] = @odbc_result($this->stmt, $i);

                    if ($arr[$field_name] === TRUE) {
                        $this->errno = 50;
                        $this->error = '[Statment][Error] odbc_result retornou dados binário.';
                        return false;
                    }else if ($arr[$field_name] == FALSE) {
                        $this->initError();
                        return false;
                    }
                }
            }

            if ($error) {
                $this->initError(); // Verifica se deu erro no odbc_fetch_row
                
                if (!empty($this->errno))    // Só envia erro se tiver error no odbc_fetch_row
                    return false;
            }*/
            
            return $arr;
        }
    }

    class DataBase {
        public $connect_errno = '';
        public $connect_error = '';

        private $is_good = false;
        private $db = false;

        public function __construct($ip_addr, $user, $pwd) {
            
            @$db = odbc_connect($ip_addr, $user, $pwd);

            if (!$db) {
                $this->initError();
                return;
            }

            $this->db = $db;

            $this->is_good = true;
        }

        public function isGood() {
            return $this->is_good;
        }

        public function close() {
           
            if ($this->is_good)
                odbc_close($this->db);  // Não retorna nada
        }

        private function initError() {
            $this->connect_errno = odbc_error();
            $this->connect_error = odbc_errormsg();
        }

        public function prepare($query) {

            if (!$this->is_good) {
                $this->connect_errno = 2;
                $this->connect_error = '[Database][Error] O db nao esta conectado';
                return false;
            }

            if (!isset($query) || empty($query)) {
                $this->connect_errno = 3;
                $this->connect_error = '[Database][Error] query invalida';
                return false;
            }

            $prepare = new Statment($this->db, $query);

            if (!$prepare->isGood()) {
                $this->connect_errno = $prepare->errno;
                $this->connect_error = $prepare->error;
                return false;
            }

            return $prepare;
        }
    }
?>