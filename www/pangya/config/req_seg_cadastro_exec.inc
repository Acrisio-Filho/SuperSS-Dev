<?php
    
    if ($this->argumentos['get']['link'] == 7) {

        // Ainda não terminou terminou o de cadastrar, acabou de entrar ou deu erro
        if (!isset($this->argumentos['get']['yes'])) {

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

                    if ($result->num_rows > 0) {

                        if (isset($_SESSION['player_cadastro']))
                            unset($_SESSION['player_cadastro']);

                        $_SESSION['player_cadastro'] = ['index' => $this->argumentos['get']['id'], 'key' => $this->argumentos['get']['key']];

                    }else
                        $this->msg = 104;   // Key and id invalidos

                } else
                    $this->msg = 103;   // Erro na consulta da conta beta
            }
        }

    }else
        $this->msg = 102;   // Link Errado
?>