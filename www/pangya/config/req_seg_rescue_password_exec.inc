<?php
    // Arquivo req_seg_rescue_password_exec.inc
    // Criado em 31/07/2019 as 19:39 por Acrisio
    // Verifica os dados que fornecido para a segundo e final parte do recupera senha ou id

    if ($this->argumentos['get']['link'] == 9) {

        if (($result_match = preg_match('/^[a-fA-F\-0-9]+$/', $this->argumentos['get']['key'])) == 0)
            $this->msg = 11;    // Dados invalidos
        else if ($result_match != 1)
            $this->msg = 6;     // Erro no preg match

        if (ctype_digit($this->argumentos['get']['index']) == FALSE)
            $this->msg = 11;    // Dados invalidos

        if ($this->msg == 0) {

            $this->params->clear();
            $this->params->add('i', $this->argumentos['get']['index']);
            $this->params->add('s', $this->argumentos['get']['key']);

            $query = '';

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcCheckRescuePasswordAndIDKey ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_RET_" as "RET", "_UID_" as "UID", "_TIPO_" as "TIPO" from '.$this->db->con_dados['DB_NAME'].'.ProcCheckRescuePasswordAndIDKey(?, ?)';
            else
                $query = 'call ProcCheckRescuePasswordAndIDKey(?, ?)';

            if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0) {

                if (($row = $result->fetch_assoc()) && isset($row['RET'])) {

                    switch ($row['RET']) {
                        case 1:     // Verificou a chave com sucesso e retornou os valores
                        {

                            if (isset($row['UID']) && isset($row['TIPO']) && $row['UID'] > 0) {
                                
                                $this->msg = 1;

                                if (isset($_SESSION['player_rescue']))
                                    unset($_SESSION['player_rescue']);

                                $_SESSION['player_rescue'] = [ 
                                        'UID' => $row['UID'],
                                        'TIPO' => $row['TIPO'],
                                        'index' => $this->argumentos['get']['index'],
                                        'key' => $this->argumentos['get']['key']
                                ];
                            
                            }else
                                $this->msg = 103;   // Erro nos UID ou nos valores retornado da consulta

                            break;
                        }
                        case -2:    // Chave invalida (não existe, já foi utilizada ou expirou)
                            $this->msg = 104;
                            break;
                        case -1:    // Erro na consulta (pode ser dados invalidos fornecido pelo input do cliente)
                        default:
                            $this->msg = 103;
                            break;
                    }

                }else
                    $this->msg = 103;   // Erro no retorno da consulta

            }else
                $this->msg = 103;   // Erro na consulta
        }

    }else
        $this->msg = 102;   // Link Errado

    if ($this->msg != 1) {

        // Limpa a session se tiver ela por que deu erro
        if (isset($_SESSION['player_rescue']))
            unset($_SESSION['player_rescue']);
    }

?>