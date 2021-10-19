<?php
    // Arquivo check_mail_first_register.inc
    // Criado em 15/11/2019 as 22:22 por Acrisio
    // Definição e Implementação da classe CheckMailFirstRegister

    define('WORD_REGISTER_DB', $_SERVER['DOCUMENT_ROOT'].'config/word_ban_register.dat');
    define('IP_REGISTER_DB', $_SERVER['DOCUMENT_ROOT'].'config/ip_ban_register.dat');

    class CheckMailFirstRegister {

        protected $state = false;
        protected $word = null;
        protected $ip = null;

        public function __construct() {

            if ($this->load() == 0)
                $this->state = true;
        }

        public function isGood() {
            return $this->state;
        }

        public function load() {

            $err = 0;

            // Verifica se tem o arquivo
            if (is_file(WORD_REGISTER_DB)) {

                if (($raw = file_get_contents(WORD_REGISTER_DB)) == FALSE) {

                    $err = 1;

                    error_log('[CheckMailFirstRegister::load][Error] Ao ler o '.WORD_REGISTER_DB, 0);   
                
                }else if (($raw = unserialize($raw)) === FALSE) {

                    $err = 2;

                    error_log('[CheckMailFirstRegister::load][Error] Não consguiu unserialize o arquivo '.WORD_REGISTER_DB, 0);
                
                }else
                    $this->word = $raw;

            }else {

                $this->word = ['xkx.me', 'tuofs.com', 'chacuo.net', 'idxue.com', 'qq.com', 'foxmail.com', '027168.com', 'linshiyouxiang.net', 'sina.com', '163.com', 'yopmail.com', 'gomen-da.com', 'protonmail.com'];

                // Cria o arquivo
                file_put_contents(WORD_REGISTER_DB, serialize($this->word));
            }

            if ($err == 0) {

                // Verifica se tem o arquivo
                if (is_file(IP_REGISTER_DB)) {

                    if (($raw = file_get_contents(IP_REGISTER_DB)) == FALSE) {

                        $err = 1;

                        error_log('[CheckMailFirstRegister::load][Error] Ao ler o '.IP_REGISTER_DB, 0);

                    }else if (($raw = unserialize($raw)) === FALSE) {

                        $err = 2;

                        error_log('[CheckMailFirstRegister::load][Error] Não consguiu unserialize o arquivo '.IP_REGISTER_DB, 0);

                    }else
                        $this->ip = $raw;

                }else {
                  
                    $this->ip = [];

                    // Cria o arquivo
                    file_put_contents(IP_REGISTER_DB, serialize($this->ip));
                }
            }

            return $err;
        }

        public function save() {

            $err = 0;

            if (file_put_contents(WORD_REGISTER_DB, serialize($this->word)) == FALSE) {
                
                $err = 1;

                error_log('[CheckMailFirstRegister::save][Error] Não conseguiu salvar o WORD array. Filename: '.WORD_REGISTER_DB, 0);
            }

            if (file_put_contents(IP_REGISTER_DB, serialize($this->ip)) == FALSE) {
                
                $err += 2;

                error_log('[CheckMailFirstRegister::save][Error] Não conseguiu salvar o IP array. Filename: '.IP_REGISTER_DB, 0);
            }

            return $err;
        }

        public function addIP($ip) {

            if ($ip == '')
                return 2;

            $this->ip[] = $ip;

            // Salva
            if ($this->save()) {

                error_log('[CheckMailFirstRegister::addIP][Error] Não conseguiu salvar.', 0);

                return 3;
            }
            
            // OK
            error_log('[CheckMailFirstRegister::addIP][Log] Adicionou IP: '.$ip.' a lista de IP banidos do registro.', 0);

            return 0;
        }

        public function addWord($word) {

            if ($word == '')
                return 2;

            $this->word[] = $word;

            // Salva
            if ($this->save()) {

                error_log('[CheckMailFirstRegister::addWord][Error] Não conseguiu salvar.', 0);

                return 3;
            }
            
            // OK
            error_log('[CheckMailFirstRegister::addWord][Log] Adicionou Word: '.$word.' a lista de Word banidos do registro.', 0);

            return 0;
        }

        public function editWord($index, $word) {

            if ($index < 0 || $index >= count($this->word))
                return 1;

            if ($word == '')
                return 2;

            $old = $this->word[$index];

            $this->word[$index] = $word;

            // Salva
            if ($this->save()) {

                error_log('[CheckMailFirstRegister::editWord][Error] Não conseguiu salvar.', 0);

                return 3;
            }
            
            // OK
            error_log('[CheckMailFirstRegister::editWord][Log] Editou Word: '.$old.' -> '.$word.' da lista de Word banidos do registro.', 0);

            return 0;
        }

        public function editIP($index, $ip) {

            if ($index < 0 || $index >= count($this->word))
                return 1;

            if ($ip == '')
                return 2;

            $old = $this->ip[$index];

            $this->ip[$index] = $ip;

            // Salva
            if ($this->save()) {

                error_log('[CheckMailFirstRegister::editIP][Error] Não conseguiu salvar.', 0);

                return 3;
            }
            
            // OK
            error_log('[CheckMailFirstRegister::editIP][Log] Editou IP: '.$old.' -> '.$ip.' da lista de IP banidos do registro.', 0);

            return 0;
        }

        public function removeWord($index) {

            if ($index < 0 || $index >= count($this->word))
                return 1;

            $word = $this->word[$index];

            array_splice($this->word, $index, 1);

            // Salva
            if ($this->save()) {

                error_log('[CheckMailFirstRegister::removeWord][Error] Não conseguiu salvar.', 0);

                return 3;
            }
            
            // OK
            error_log('[CheckMailFirstRegister::removeWord][Log] Removeu Word: '.$word.' da lista de Word banidos do registro.', 0);

            return 0;
        }

        public function removeIP($index) {

            if ($index < 0 || $index >= count($this->ip))
                return 1;

            $ip = $this->ip[$index];

            array_splice($this->ip, $index, 1);

            // Salva
            if ($this->save()) {

                error_log('[CheckMailFirstRegister::removeIP][Error] Não conseguiu salvar.', 0);

                return 3;
            }
            
            // OK
            error_log('[CheckMailFirstRegister::removeIP][Log] Removeu IP: '.$ip.' da lista de IP banidos do registro.', 0);

            return 0;
        }

        public function getAllWord() {
            return $this->word;
        }

        public function getAllIP() {
            return $this->ip;
        }

        public function checkEmail($email, $_ip) {

            $err = false;

            // Não conseguiu inicializar direito
            if (!$this->state)
                return true;

            if ($email != '' && $_ip != '') {

                // Verifica se tem palavras que não pode no email
                foreach ($this->word as $word) {
                    if (strpos($email, $word) !== FALSE)
                        return true;
                }

                // Verifica se está na lista de IP Bloqueados
                foreach ($this->ip as $ip)
                    if ($ip == $_ip)
                        return true;
            
            }else
                $err = true;    // Invalids parameters

            return $err;
        }
    }
?>