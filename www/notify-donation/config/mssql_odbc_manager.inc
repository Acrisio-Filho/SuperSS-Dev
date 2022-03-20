<?php
    // Arquivo mssql_odbc_manager.inc
    // Criado em 11/07/2018 as 19:28 por Acrisio
    // Definição e Implementação da classe MSSqlODBCManager

    // Contante de debug do MSSQL Manager
    define("MSSQL_MANAGER_DEBUG", true);

    include_once('database.inc');

    class MSSqlODBCManager {
        private $db;
		private $is_connected;
		private $is_clear_consulta;
		private $is_stmted;
		private $result_current;
		private $stmt_current;
        private $debug = MSSQL_MANAGER_DEBUG;
        private $last_error = 0;

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
				$this->db = new DataBase($con_dados['IP_ADDR'], $con_dados['User'], $con_dados['pwd']);
				
				if (empty($this->db->connect_errno)) {
					$this->is_connected = true;
				}else {
					if ($this->debug)
                        echo "[MSSqlODBCManager::init][Error] não conexão com o banco de dados Erro numero : ".$this->db->connect_errno." Erro : ".$this->db->connect_error.".";
                    else
                        error_log("[MSSqlODBCManager::init][Error] não conexão com o banco de dados Erro numero : ".$this->db->connect_errno." Erro : ".$this->db->connect_error.".", 0);
				}
			}else {
				if ($this->debug)
                    echo "[MSSqlODBCManager::init][Error] Parametro invalido.";
                else
                    error_log("[MSSqlODBCManager::init][Error] Parametro invalido.", 0);
			}
		}

		public function execPreparedStmt($query, $obj_params, $option = 0) {
			if ($this->is_connected) {
				$this->clear_consulta();
                $this->close_stmt();
                $this->last_error = 0;

				if ($obj_params != null)
					$params = $obj_params->getValues();
				else
					$params = null;

				if (!empty($query) && ((!empty($params) && is_array($params)) || ($params == null && $option == 1))) {
					if (($this->stmt_current = $this->db->prepare($query))) {
						$this->is_stmted = true;

						if ($option == 1) {
							if ($this->stmt_current->execute()) {
								$this->result_current = $this->stmt_current->get_result();
								$this->is_clear_consulta = false;
								return $this->result_current;
							}else {
								if ($this->debug)
                                    echo "[MSSqlODBCManager::execPreparedStmt][Error] ao executar o statement. Erro numero : ".$this->stmt_current->errno." Erro : ".$this->stmt_current->error.". ($query)";
                                else
                                    error_log("[MSSqlODBCManager::execPreparedStmt][Error] ao executar o statement. Erro numero : ".$this->stmt_current->errno." Erro : ".$this->stmt_current->error.". ($query)", 0);
                                $this->last_error = 1;
							}
                        }else {
                            if ($this->stmt_current->execute($params)) {
                                $this->result_current = $this->stmt_current->get_result();
                                $this->is_clear_consulta = false;
                                return $this->result_current;
                            }else {
                                if ($this->debug)
                                    echo "[MSSqlODBCManager::execPreparedStmt][Error] ao executar o statement. Erro numero : ".$this->stmt_current->errno." Erro : ".$this->stmt_current->error.". ($query)";
                                else
                                    error_log("[MSSqlODBCManager::execPreparedStmt][Error] ao executar o statement. Erro numero : ".$this->stmt_current->errno." Erro : ".$this->stmt_current->error.". ($query)", 0);
                                $this->last_error = 2;
                            }
                        }
					}else {
						if ($this->debug)
                            echo "[MSSqlODBCManager::execPreparedStmt][Error] ao executar prepared statemant. Erro numero : ".$this->db->connect_errno." Erro : ".$this->db->connect_error.". ($query)";
                        else
                            error_log("[MSSqlODBCManager::execPreparedStmt][Error] ao executar prepared statemant. Erro numero : ".$this->db->connect_errno." Erro : ".$this->db->connect_error.". ($query)", 0);
                        $this->last_error = 3;
					}
				}else {
					if ($this->debug)
                        echo "[MSSqlODBCManager::execPreparedStmt][Error] parametros invalidos. ($query)";
                    else
                        error_log("[MSSqlODBCManager::execPreparedStmt][Error] parametros invalidos. ($query)", 0);
                    $this->last_error = 4;
				}
			}else {
				if ($this->debug)
                    echo "[MSSqlODBCManager::execPreparedStmt][Error] não está conectado com o banco de dados. ($query)";
                else
                    error_log("[MSSqlODBCManager::execPreparedStmt][Error] não está conectado com o banco de dados. ($query)", 0);
                $this->last_error = 5;
			}

			return null;
        }
        
        public function getLastError() {
            return $this->last_error;
        }

		public function close() {
			if ($this->is_connected) {
				$this->db->close();
				$this->db = null;
			}

			$this->is_connected = false;

			$this->close_stmt();

            $this->clear_consulta();
            
            $this->last_error = 0;
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