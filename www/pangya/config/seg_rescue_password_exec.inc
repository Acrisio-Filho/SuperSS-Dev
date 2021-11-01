<?php
    // Arquivo seg_rescue_password_exec.inc
    // Criado em 31/07/2019 as 20:29 por Acrisio
    // Executar a recuperação da senha(troca)

    $this->msg = 0;

    if ($this->argumentos['post']['rescue_pass'] != $this->argumentos['post']['rescue_confirm_pass'])
        $this->msg = 4;	// Erro a senha está diferente

    if (($result_match = preg_match('/^[0-9A-z@#$%&*!_?=+\'\-]{6,16}$/', $this->argumentos['post']['rescue_pass'])) == 0)
        $this->msg = 11;    // Dados invalidos
    else if ($result_match != 1)
        $this->msg = 6; // Erro no preg match

    if (ctype_digit($this->argumentos['post']['uid']) == FALSE)
        $this->msg = 11;    // Dados invalidos

    if (ctype_digit($this->argumentos['post']['index']) == FALSE)
        $this->msg = 11;    // Dados invalidos

    if ($this->msg == 0) {

        unset($this->argumentos['post']['rescue_confirm_pass']);

        $this->argumentos['post']['rescue_pass'] = strtoupper(md5($this->argumentos['post']['rescue_pass']));

        $this->params->clear();
        $this->params->add('i', $this->argumentos['post']['uid']);
        $this->params->add('s', $this->argumentos['post']['rescue_pass']);

        $query = '';

        if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
            $query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcChangePassword ?, ?';
        else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
            $query = 'select "_RET_" as "RET", "_EMAIL_" as "EMAIL" from '.$this->db->con_dados['DB_NAME'].'.ProcChangePassword(?, ?)';
        else
            $query = 'call ProcChangePassword(?, ?)';

        if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0) {
            
            if (($row = $result->fetch_assoc()) && isset($row['RET'])) {

                switch ($row['RET']) {
                    case 1:		// Trocou a senha com sucesso
                    {
                        $this->msg = 100;

                        $this->params->clear();
                        $this->params->add('i', $this->argumentos['post']['uid']);
                        $this->params->add('i', $this->argumentos['post']['index']);

                        $query = '';

                        if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                            $query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcUpdateRescuePasswordAndID ?, ?';
                        else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                            $query = 'select * from '.$this->db->con_dados['DB_NAME'].'.ProcUpdateRescuePasswordAndID(?, ?)';
                        else
                            $query = 'call ProcUpdateRescuePasswordAndID(?, ?)';

                        if (!(($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0))
                            error_log('Player[UID='.$argumentos['post']['uid'].'] não conseguiu atualizar o state do rescue password[index='.$this->argumentos['post']['index'].']', 0);

                        break;
                    }
                    case -2:	// Já trocou duas vezes de senha em menos de 24 horas
                        $this->msg = 2;
                        break;
                    case -1:	// Erro ao trocar a senha
                    default:
                        $this->msg = 9;
                        break;
                }
            
            }else
                $this->msg = 8;

        }else
            $this->msg = 7;
    }

?>