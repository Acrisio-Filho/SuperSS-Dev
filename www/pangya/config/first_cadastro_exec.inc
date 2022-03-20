<?php

    // se true, mostra as msg  de erro do SMTP(send mail)
    $debug = true;

    $this->msg = 0;

    if ($this->argumentos['post']['email'] != $this->argumentos['post']['confirm_email'])
        $this->msg = 3;	// Erro o email está diferente

    if (($result_match = preg_match('/^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$/', $this->argumentos['post']['email'])) == 0)
        $this->msg = 10;    // Email invalido
    else if ($result_match != 1)
        $this->msg = 6;     // Erro no preg match 

    if (($result_match = preg_match('/^[a-zA-Z ]{4,20}$/', $this->argumentos['post']['nome'])) == 0)
        $this->msg = 11;    // Dados invalidos 
    else if ($result_match != 1)
        $this->msg = 6; // Erro no preg match

    if (($result_match = preg_match('/^[a-zA-Z ]{4,40}$/', $this->argumentos['post']['sobre_nome'])) == 0)
        $this->msg = 11;    // Dados invalidos
    else if ($result_match != 1)
        $this->msg = 6; // Erro no preg match

    // Verifica o reCAPTCHA
    include_once('recaptcha.inc');

    if (reCAPTCHA::isEnable()) {

        $resposta = reCAPTCHA::checkReCAPTCHA_V2($this->argumentos['post']);

        if ($resposta->isSuccess() == 0)
            $this->msg = 15;	// Error no desafio do reCAPTCHA
    }

    if ($this->msg == 0) {

        $this->params->clear();
        $this->params->add('s', $this->argumentos['post']['email']);

        $query = '';

        if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
            $query = 'SELECT [index] FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE email = ?';
        else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
            $query = 'SELECT index FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE email = ?';
        else
            $query = 'SELECT `index` FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE email = ?';

        if ($this->msg == 0 && ($resutl = $this->db->db->execPreparedStmt($query, $this->params->get())) && $resutl->num_rows > 0)
            $this->msg = 9;	// email já existe

        if ($this->msg == 0) {

            // Gera uma chave de registro e envia para o email que o cliente forneceu
            unset($this->argumentos['post']['confirm_email']);

            if (strlen($this->argumentos['post']['nome']) <= 50) {

                // Verifica se o email contém palavras que não pode
                include_once("check_mail_first_register.inc");

                $chkMail = new CheckMailFirstRegister();

                // Vai precisar então inicia ela aqui
                if (!isset($_SESSION))
                    session_start();

                if ($chkMail->checkEmail($this->argumentos['post']['email'], getenv("REMOTE_ADDR")) || (isset($_SESSION['REGISTER_ATTACK']) && $_SESSION['REGISTER_ATTACK']['attack'])) {
                    
                    // Adiciona o IP dele para na proxima vez que ele trocar o email, ele seja parado pelo IP
                    $chkMail->addIP(getenv("REMOTE_ADDR"));

                    if (!isset($_SESSION['REGISTER_ATTACK'])) {

                        // Make new SESSION REGISTER ATTACK
                        $_SESSION['REGISTER_ATTACK'] = 
                        [
                            'IP' => getenv("REMOTE_ADDR"), 
                            'email' => $this->argumentos['post']['email'],
                            'attack' => true
                        ];

                    }else {

                        // Update
                        $_SESSION['REGISTER_ATTACK']['IP'] = getenv("REMOTE_ADDR");
                        $_SESSION['REGISTER_ATTACK']['email'] = $this->argumentos['post']['email'];
                        $_SESSION['REGISTER_ATTACK']['attack'] = true;
                    }

                    // Attack register
                    error_log('[first_cadastro_exec.inc][Error] ATTACK REGISTER: IP: '.$_SESSION['REGISTER_ATTACK']['IP'].', EMAIL: '.$_SESSION['REGISTER_ATTACK']['email'], 0);
                    
                    $this->msg = 1001;  // Mostra mensagem dizendo que já chegou no limite do smtp, mas é mentira
                }

                if ($this->msg == 0) {
                
                    $this->params->clear();
                    $this->params->add('s', $this->argumentos['post']['nome']);
                    $this->params->add('s', $this->argumentos['post']['sobre_nome']);
                    $this->params->add('s', $this->argumentos['post']['email']);
                    
                    $this->params->add('s', $this->argumentos['post']['ip']);

                    $query = '';

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcMakeEmailKey ?, ?, ?, ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'SELECT "_INDEX_" as "index", "_KEY_UNIQ_" as "key_uniq" FROM '.$this->db->con_dados['DB_NAME'].'.ProcMakeEmailKey(?, ?, ?, ?)';
                    else
                        $query = 'CALL '.$this->db->con_dados['DB_NAME'].'.ProcMakeEmailKey(?, ?, ?, ?)';

                    if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0) {
                        
                        if (($row = $result->fetch_assoc()) && isset($row['index']) && isset($row['key_uniq'])) {

                            require_once('smtp_base.inc');

                            // Insira abaixo o email que irá receber a mensagem, o email que irá enviar (o mesmo da variável GUSER), 
                            // o nome do email que envia a mensagem, o Assunto da mensagem e por último a variável com o corpo do email.
                            $this->email = $this->argumentos['post']['email'];

                            // Cria o Email e envia aqui nessa parte
                            $link = $_SERVER['REQUEST_SCHEME']."://".$_SERVER['HTTP_HOST']."/pangya.php?link=7&id=".$row['index']."&key=".$row['key_uniq'];

                            $bodymail = "PangYa SuperSS - Bem vindo.\n\nClick no link para terminar o cadastro da conta do Open Beta.  ".$link;

                            if (smtpmailer($this->email, '#Seu email do remetente#', 'PangYa SuperSS', 'Etapa final do cadastro', $bodymail)) {

                                $this->msg = 150;
                            
                            }else {
                                
                                if ($debug && !empty($error))
                                    error_log($error, 0);

                                $this->msg = 107;

                                // Deleta a conta que foi criada agora, por que não conseguiu enviar o email para ele
                                $this->params->clear();
                                $this->params->add('i', $row['index']);

                                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                    $query = 'DELETE FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE [index] = ?';
                                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                    $query = 'DELETE FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE index = ?';
                                else
                                    $query = 'DELETE FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta WHERE `index` = ?';

                                if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0 && $result->num_rows > 0)
                                    $this->msg = 107;   // Conseguiu excluir a conta de acaba de criar
                            }
                        }

                    }else {
                        
                        if ($debug) 
                            error_log('Error na consulta: num rows: '.$result->num_rows, 0);

                        $this->msg = 101;
                    }
                }

            }else
                $this->msg = 1000;	// muito grande o nome
        }
    }
?>