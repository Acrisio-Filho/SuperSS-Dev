<?php

    $this->msg = 0;

    if (isset($_SESSION['player_cadastro']) && isset($_SESSION['player_cadastro']['index']) && isset($_SESSION['player_cadastro']['key'])) {

        if ($this->argumentos['get']['link'] == 7) {

            // Ainda não terminou terminou o de cadastrar, acabou de entrar ou deu erro
            if (isset($this->argumentos['get']['yes'])) {

                if (($result_match = preg_match('/^[a-fA-F\-0-9]+$/', $this->argumentos['get']['key'])) == 0)
                    $this->msg = 11;    // Dados invalidos
                else if ($result_match != 1)
                    $this->msg = 6;     // Erro no preg match

                if (ctype_digit($this->argumentos['get']['id']) == FALSE)
                    $this->msg = 11;    // Dados invalidos

                if ($this->msg == 0) {

                    $this->params->clear();
                    $this->params->add('i', $this->argumentos['get']['id']);
                    $this->params->add('s', $this->argumentos['get']['key']);

                    $query = '';

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'SELECT key_uniq FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE [index] = ? AND key_uniq = ? AND finish_reg = 0';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'SELECT key_uniq FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE index = ? AND key_uniq = ? AND finish_reg = 0';
                    else
                        $query = 'SELECT key_uniq FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE `index` = ? AND key_uniq = ? AND finish_reg = 0';

                    if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0) {

                        if ($result->num_rows <= 0)
                            $this->msg = 104;   // Key and id invalidos

                    } else
                        $this->msg = 103;   // Erro na consulta da conta beta
                }
                
            }else
                $this->msg = 1007;  // Tentou finalizar o cadastro sem ir pela primeira parte

        }else
            $this->msg = 102;   // Link Errado

        if ($this->msg == 0) {

            if ($this->argumentos['post']['pass'] != $this->argumentos['post']['confirm_pass'])
                $this->msg = 4;	// Erro a senha está diferente

            if (($result_match = preg_match("/^[A-Za-z0-9]{4,16}$/", $this->argumentos['post']['id'])) == 0)
                $this->msg = 5;  // Tem caracteres invalidos no id
            else if ($result_match != 1)
                $this->msg = 6; // erro de sistema (não conseguiu executar o preg_match)

            if (($result_match = preg_match('/^[0-9A-z@#$%&*!_?=+\'\-]{6,16}$/', $this->argumentos['post']['pass'])) == 0)
                $this->msg = 11;    // Dados invalidos
            else if ($result_match != 1)
                $this->msg = 6; // Erro no preg match

            if (ctype_digit($this->argumentos['post']['sexo']) == FALSE)
                $this->msg = 11;    // Dados invalidos

            if (ctype_digit($this->argumentos['post']['index']) == FALSE)
                $this->msg = 11;    // Dados invalidos

            if ($_SESSION['player_cadastro']['index'] != $this->argumentos['post']['index'])
                $this->msg = 11;    // Dados invalidos

            if ($this->msg == 0) {

                $this->params->clear();
                $this->params->add('s', $this->argumentos['post']['id']);

                $query = '';

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'SELECT uid FROM '.$this->db->con_dados['DB_NAME'].'.account WHERE id = ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'SELECT uid FROM '.$this->db->con_dados['DB_NAME'].'.account WHERE id = ?';
                else
                    $query = 'SELECT uid FROM account WHERE id = ?';

                if (($resutl = $this->db->db->execPreparedStmt($query, $this->params->get())) && $resutl->num_rows > 0)
                    $this->msg = 8;	// ID já existe
                
                if ($this->msg == 0) {
                    
                    $this->argumentos['post']['pass'] = strtoupper(md5($this->argumentos['post']['pass']));

                    unset($this->argumentos['post']['confirm_pass']);

                    $this->params->clear();
                    $this->params->add('s', $this->argumentos['post']['id']);
                    $this->params->add('s', $this->argumentos['post']['pass']);
                    $this->params->add('s', $this->argumentos['post']['index']);
                    $this->params->add('i', $this->argumentos['post']['sexo']);
                    
                    $this->params->add('s', $this->argumentos['post']['ip']);

                    $query = '';

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcMakeUserBetaWithMD5 ?, ?, ?, ?, ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'SELECT "_UID_" as "UID" FROM '.$this->db->con_dados['DB_NAME'].'.ProcMakeUserBetaWithMD5(?, ?, ?, ?, ?)';
                    else
                        $query = 'CALL ProcMakeUserBetaWithMD5(?, ?, ?, ?, ?)';

                    if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0) {
                        
                        if (($row = $result->fetch_assoc()) != null && isset($row['UID']) && $row['UID'] > 0) {
                            
                            // Concluiu com sucesso, libera a session cadastro
                            if ($_SESSION['player_cadastro'])
                                unset($_SESSION['player_cadastro']);

                            $this->msg = 100;

                        }else
                            $this->msg = 101;   // Erro na criação da conta, pode ser o index inválido
                        
                    }else
                        $this->msg = 101;
                }
            }
        }

    }else
        $this->msg = 11; // "Dados inválidos" Tentando burla o sistema de cadastro

    if ($this->msg != 100 && $this->msg != 0) {

        if (isset($this->argumentos['get']['yes']))
            unset($this->argumentos['get']['yes']);
    }

    if ($this->msg != 100)
        error_log("[Segunda Etapa Cadastro][Error] Cliente IP: ".getenv('REMOTE_ADDR')." Code: ".$this->msg);
?>