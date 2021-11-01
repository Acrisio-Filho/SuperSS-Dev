<?php
	// Arquivo de criação da classe de gerenciamento da conexão com o banco de dados

	define("MYSQL_MANAGER_DEBUG", true);

	include_once('database.inc');

	/*class paramsArr {
		private $arr = [];
		private $tipo = '';

		public function __destruct() {
			$this->clear();
		}

		public function add($tipo, &$valor) {
			$this->arr[] = &$valor;
			$this->tipo .= $tipo;
		}

		public function get() {
			return array_merge([$this->tipo], $this->arr);
		}

		public function clear() {
			if (!empty($this->arr)) {
				$this->arr = [];
			}

			if (!empty($this->tipo)) {
				$this->tipo = '';
			}
		}
	}*/

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

		public function execPreparedStmt($query, $params, $option = 0) {
			if ($this->is_connected) {
				$this->clear_consulta();
				$this->close_stmt();

				if (!empty($query) && ((!empty($params) && is_array($params)) || ($params == null && $option == 1))) {
					if ($this->stmt_current = $this->db->prepare($query)) {
						$this->is_stmted = true;

						if ($option == 1) {
							if ($this->stmt_current->execute()) {
								$this->result_current = $this->stmt_current->get_result();
								$this->is_clear_consulta = false;
								return $this->result_current;
							}else {
								if ($this->debug)
									echo "erro ao executar o statement. ($query)";
								else
									error_log("erro ao executar o statement. ($query)", 0);
								$this->error = 1;
							}
						}else if (call_user_func_array([$this->stmt_current, 'bind_param'], $params)) {
							if ($this->stmt_current->execute()) {
								$this->result_current = $this->stmt_current->get_result();
								$this->is_clear_consulta = false;
								return $this->result_current;
							}else {
								if ($this->debug)
									echo "erro ao executar o statement. ($query)";
								else
									error_log("erro ao executar o statement. ($query)", 0);
								$this->error = 2;
							}
						}else {
							if ($this->debug)
								echo "erro ao executar o bind param. ($query)";
							else
								error_log("erro ao executar o bind param. ($query)", 0);
							$this->error = 3;
						}
					}else {
						if ($this->debug)
							echo "erro ao executar prepared statemant. ($query)";
						else
							error_log("erro ao executar prepared statemant. ($query)", 0);
						$this->error = 4;
					}
				}else {
					if ($this->debug)
						echo "parametros invalidos. ($query)";
					else
						error_log("parametros invalidos. ($query)", 0);
					$this->error = 5;
				}
			}else {
				if ($this->debug)
					echo "não está conectado com o banco de dados. ($query)";
				else
					error_log("não está conectado com o banco de dados. ($query)", 0);
				$this->error = 6;
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

					}while ($this->stmt_current->more_results() && $this->stmt_current->next_result());
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