<?php
	// Arquivo de criação da classe de gerenciamento da conexão com o banco de dados

	define("MYSQL_MANAGER_DEBUG", true);

	include_once('database.inc');

	class StatmentMySQL {
		private $stmt = null;
		private $result_current = null;
        
		private $is_good = false;
        private $is_exec = false;

        public $errno = '';
        public $error = '';

		// Magic Getters And Setters - Begin
		public function __get( $name ) {
			if( method_exists( $this , $method = ( 'get' . ucfirst( $name  ) ) ) )
				return $this->$method();
			else
				throw new Exception( 'Can\'t get property ' . $name );
		}
	
		public function __set( $name , $value ) {
			if( method_exists( $this , $method = ( 'set' . ucfirst( $name  ) ) ) )
				return $this->$method( $value );
			else
				throw new Exception( 'Can\'t set property ' . $name );
		}
	
		public function __isset( $name ) {
			return method_exists( $this , 'get' . ucfirst( $name  ) ) 
				|| method_exists( $this , 'set' . ucfirst( $name  ) );
		}
		// Magic Getters And Setters - End

        public function __construct($stmt) {

            $this->stmt = $stmt;

			$this->is_good = ($this->stmt != null);
        }

		public function getnum_rows() {
			return ($this->result_current != null) ? $this->result_current->num_rows : -1;
		}

		public function setnum_rows($num_rows) {
			
			if ($this->result_current != null)
				$this->result_current->num_rows = $num_rows;
		}

		public function isGood() {
            return $this->is_good;
        }

        public function execute(...$args) {

            if (!$this->is_good) {
                $this->errno = 3;
                $this->error = '[StatmentMySQL][Error] O statment não está inicializado.';
                return false;
            }

            $num_args = count($args);

            if ($num_args > 0 && (!isset($args[0]) || empty($args[0]))) {
                $this->errno = 2;
                $this->error = '[StatmentMySQL][Error] invalid arguments';
                return false;
            }

            if ($num_args > 0) {

				if (!call_user_func_array([$this->stmt, 'bind_param'], $args[0])) {
					$this->errno = 6;
					$this->error = '[StatmentMySQL][Error] fail to bind param. Erro numero : '.$this->stmt->errno.' Erro : '.$this->stmt->error;
					return false;
				}

                if (!$this->stmt->execute()) {
					$this->errno = 6;
					$this->error = '[StatmentMySQL][Error] Pouco parametros para executar o prepared statment.';
                    return false;
                }

            }else if (!$this->stmt->execute()) {
				$this->errno = 5;
				$this->error = '[StatmentMySQL][Error] Para executar a query, é necessário paremetros para executar o prepared statment.';
                return false;
            }

            $this->is_exec = true;

            return true;
        }

        public function get_result() {
           
            if (!$this->is_good) {
                $this->errno = 3;
                $this->error = '[StatmentMySQL][Error] O Statment não está inicializado.';
                return false;
            }

            if (!$this->is_exec) {
                $this->errno = 4;
                $this->error = '[StatmentMySQL][Error] O Statment não foi executado ainda, chame a função execute primeiro.';
                return false;
            }

			if ($this->result_current != null)
				$this->result_current->free();

			$this->result_current = $this->stmt->get_result();

			if ($this->stmt->errno != 0 && ($this->result_current == null || $this->result_current == false)) {
				$this->errno = 8;
                $this->error = '[StatmentMySQL][Error] O falhou em executar o get_result. Error Número: '.$this->stmt->errno.'. Error: '.$this->stmt->error;
                return false;
			}

            return $this;
        }

        public function free() {

            if ($this->is_good && $this->result_current != null)
                $this->result_current->free();

			$this->result_current = null;
        }

        public function close() {

			if ($this->is_good && $this->stmt != null)
				$this->stmt->close();

            $this->stmt = null;
			$this->result_current = null;
            $this->is_good = false;
            $this->is_exec = false;
            $this->error = '';
            $this->errno = '';
        }

        public function next_result() {

            if (!$this->is_good) {
                $this->errno = 3;
                $this->error = '[StatmentMySQL][Error] O Statment não está inicializado.';
                return false;
            }

            if (!$this->is_exec) {
                $this->errno = 4;
                $this->error = '[StatmentMySQL][Error] O Statment não foi executado ainda, chame a função execute primeiro.';
                return false;
            }

            return $this->stmt->more_results() && $this->stmt->next_result();
        }

        public function fetch_assoc() {
            
            if (!$this->is_good) {
                $this->errno = 3;
                $this->error = '[StatmentMySQL][Error] O Statment não está inicializado.';
                return false;
            }

            if (!$this->is_exec) {
                $this->errno = 4;
                $this->error = '[StatmentMySQL][Error] O Statment não foi executado ainda, chame a função execute primeiro.';
                return false;
            }

			if ($this->result_current == null || $this->result_current == false) {
				$this->errno = 7;
                $this->error = '[StatmentMySQL][Error] O Current Result é invalido.';
                return false;
			}
            
            return $this->result_current->fetch_assoc();
        }
    }

	class MysqlManager {
		private $db;
		private $is_connected;
		private $is_clear_consulta;
		private $is_stmted;
		private $result_current;
		private $stmt_current;
		private $debug = MYSQL_MANAGER_DEBUG;
		private $error = 0;

		public function __construct() {
			$this->db = null;
			$this->is_connected = false;
			$this->is_clear_consulta = true;
			$this->is_stmted = false;
			$this->result_current = null;
			$this->stmt_current = null;

			$a = func_get_args();
			$i = func_num_args();
			if (method_exists($this, $f='__construct'.$i)) {
				call_user_func_array([$this, $f], $a);
			}
		}

		public function __construct1($con_dados) {
			$this->init($con_dados);
		}

		public function __destruct() {
			$this->close();
		}

		public function init($con_dados) {
			if (isset($con_dados) && isset($con_dados['IP_ADDR']) && isset($con_dados['User']) && isset($con_dados['pwd']) && isset($con_dados['DB_NAME'])) {
				$this->db = new mysqli($con_dados['IP_ADDR'], $con_dados['User'], $con_dados['pwd'], $con_dados['DB_NAME']);
				
				if (!$this->db->connect_errno) {
					$this->is_connected = true;
				}else {
					if ($this->debug)
						echo "Erro não conexão com o banco de dados Erro numero : ".$this->db->connect_errno." Erro : ".$this->db->connect_error.".";
					else
						error_log("Erro não conexão com o banco de dados Erro numero : ".$this->db->connect_errno." Erro : ".$this->db->connect_error.".", 0);
				}
			}else {
				if ($this->debug)
					echo "Parametro invalido.";
				else
					error_log("Parametro invalido.", 0);
			}
		}

		public function execPreparedStmt($query, $obj_params, $option = 0) {
			if ($this->is_connected) {
				$this->clear_consulta();
				$this->close_stmt();
				$this->error = 0;

				if ($obj_params != null)
					$params = $obj_params->getAll();
				else
					$params = null;

				if (!empty($query) && ((!empty($params) && is_array($params)) || ($params == null && $option == 1))) {
					if (($this->stmt_current = new StatmentMySQL($this->db->prepare($query))) != null && $this->stmt_current->isGood()) {
						$this->is_stmted = true;

						if ($option == 1) {
							if ($this->stmt_current->execute()) {
								$this->result_current = $this->stmt_current->get_result();
								$this->is_clear_consulta = false;
								return $this->result_current;
							}else {
								if ($this->debug)
									echo "erro ao executar o statement. Erro numero : ".$this->db->errno." Erro : ".$this->db->error.". ($query)";
								else
									error_log("erro ao executar o statement. Erro numero : ".$this->db->errno." Erro : ".$this->db->error.". ($query)", 0);
								$this->error = 1;
							}
						}else {
							if ($this->stmt_current->execute($params)) {
								$this->result_current = $this->stmt_current->get_result();
								$this->is_clear_consulta = false;
								return $this->result_current;
							}else {
								if ($this->debug)
									echo "erro ao executar o statement. Erro numero : ".($this->db->errno == 0 ? $this->stmt_current->errno : $this->db->errno)." Erro : ".($this->db->error == '' ? $this->stmt_current->error : $this->db->error).". ($query)";
								else
									error_log("erro ao executar o statement. Erro numero : ".($this->db->errno == 0 ? $this->stmt_current->errno : $this->db->errno)." Erro : ".($this->db->error == '' ? $this->stmt_current->error : $this->db->error).". ($query)", 0);
								$this->error = 2;
							}
						}
					}else {
						if ($this->debug)
							echo "erro ao executar prepared statemant. Erro numero : ".$this->db->errno." Erro : ".$this->db->error.". ($query)";
						else
							error_log("erro ao executar prepared statemant. Erro numero : ".$this->db->errno." Erro : ".$this->db->error.". ($query)", 0);
						$this->error = 3;
					}
				}else {
					if ($this->debug)
						echo "parametros invalidos. ($query)";
					else
						error_log("parametros invalidos. ($query)", 0);
					$this->error = 4;
				}
			}else {
				if ($this->debug)
					echo "não está conectado com o banco de dados. ($query)";
				else
					error_log("não está conectado com o banco de dados. ($query)", 0);
				$this->error = 5;
			}

			return null;
		}

		public function close() {
			if ($this->is_connected) {
				$this->db->kill($this->db->thread_id);
				$this->db->close();
				$this->db = null;
			}

			$this->is_connected = false;

			$this->close_stmt();

			$this->clear_consulta();

			$this->error = 0;
		}

		public function close_stmt() {
			if ($this->is_stmted) {
				$this->stmt_current->close();
			}

			$this->is_stmted = false;
		}

		public function clear_consulta() {
			if (!$this->is_clear_consulta) {
				if ($this->result_current != null)
					$this->result_current->free();

				if ($this->is_stmted) {
					do {
						if ($this->result_current = $this->stmt_current->get_result()) {
							$this->result_current->free();
						}

					}while ($this->stmt_current->next_result());
				}
			}

			$this->is_clear_consulta = true;
		}

		public function getLastError() {
			return $this->error;
		}

		public function is_connected() {
			return $this->is_connected;
		}

		public function is_clear_consulta() {
			return $this->is_clear_consulta;
		}

		public function is_stmted() {
			return $this->is_stmted;
		}
	}

?>