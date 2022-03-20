<?php
// Classe site

define("CONST_LOG_DIR", "#Diretorio de Log geral#");

include_once('db_manager_singleton.inc');

include_once('donation/donation_manager.inc');

$global_arr_encode = [ "CP874", "TIS-620", "CP932", "SJIS", "CP936", "EUC-KR", "CP949", "CP950", "CP860" ]; //"EUC-KR,SJIS,TIS-620,EUC-TW,Windows-1252,ISO-8859-11";

function exception_error_e_notice($errno, $errstr, $errfile, $errline) {

	if ($errno == E_NOTICE /*&& stripos($errstr, "Detected an illegal character in input string") == FALSE */
			&& stripos($errstr, "Detected an incomplete multibyte character in input string") == FALSE)
		throw new ErrorException($errstr, 0, $errno, $errfile, $errline);

	return false;
}

function decode($str, $encode_index = 0) {
	global $global_arr_encode;

	$enc = $global_arr_encode[$encode_index % count($global_arr_encode)];

	set_error_handler("exception_error_e_notice", E_NOTICE);

	$try_encode = FALSE;

	if ($enc != FALSE) {

		try {

			if (($new = iconv('UTF-8', $enc, $str)) != FALSE)
				return $new;

		}catch (ErrorException $e) {

			//echo $e->getMessage().PHP_EOL;

			$try_encode = TRUE;
		}

		if ($try_encode == TRUE) {
			$str_tmp = "";
			$str_final = "";

			$count = 0;

			for ($i = 0; $i < strlen($str); $i++) {

				try {

					$str_tmp .= $str[$i];

					if (($new = @iconv('UTF-8', $enc, $str_tmp)) != FALSE) {
						
						$str_final = $new;

						$count = 0;

					}else
						$count++;

				}catch (ErrorException $e) {

					//echo $e->getMessage().PHP_EOL;

					if (($new = decode(substr($str, $i - $count, strlen($str) - $i + $count), $encode_index + 1)) != FALSE)
						return ($str_final.$new);
				}
			}

			if ($str_final != "")
				return $str_final;
		}
	}

	restore_error_handler();

	return FALSE;
}

function str_to_array_char($str) {

	$arr = [];

	$str_final = "";

	$str_final = decode($str);

	if ($str_final == "") {	// Não achou enconder String

		if (($new = @iconv('UTF-8', 'EUC-KR', $str)) != FALSE) {
			$str = $new;
			echo "KR Encode";
		}

		if (($new = @iconv('UTF-8', 'TIS-620', $str)) != FALSE) {
			$str = $new;

			echo "THAI Encode";
		}
		
		if (($new = @iconv('UTF-8', 'SJIS', $str)) != FALSE) {
			$str = $new;
			echo "JP Encode";
		}
		
		if (($new = @iconv('UTF-8', 'Windows-1252')) != FALSE) {
			$str = $new;
			echo "PT Encode";
		}

	}else
		$str = $str_final;

	$arr = unpack("C*", $str);

	return $arr;
}

function verifyAndEncode($str) {

	$_str = str_to_array_char($str);
	$new = "";

	//echo print_r($_str);

	foreach ($_str as $el) {

		/*if (IntlChar::isprint((int)$el)/* || IntlChar::ispunct((int)$el)
			$new .= chr((int)$el);
		else*/
			$new .= "\x".str_pad(strtoupper(dechex((int)$el)), 2, '0', STR_PAD_LEFT);
	}

	//echo $new;

	return $new;
}

function listDir($log_link, $base_dir, $sub_dir) {
	
	$base = new DirectoryIterator(($sub_dir != "" ? $base_dir."/".$sub_dir : $base_dir));

	foreach ($base as $file) {

		if ($file->isDot())
			continue;
		else if ($file->isDir())
			listDir($log_link, $base_dir, ($sub_dir != "" ? $sub_dir."/".$file->getFileName() : $file->getFileName()));
		else if ($file->isFile())
			echo $log_link[0].$sub_dir."/".$file->getFileName().$log_link[1].$sub_dir."/".$file->getFileName().$log_link[2].$file->getSize().$log_link[3];
	}
}

class base_c {

	private $Error = 0;
	private $msg = 0;

	private $logado = false;
	private $uid = -1;
	private $cap = 0;
	private $nick = 'unknown';
	private $id = '';
	private $key = '';
	private $moeda = ['cookie' => 0, 'pang' => 0];
	private $level = -1;
	private $ip_addr = '';

	private $email = '';

	private $pincode = '';

	private $bug_ta = '';

	private $db = null;
	private $params = null;

	private $level_arr = ['-1' => 'unknown', 'Rookie F', 'Rookie E', 'Rookie D', 'Rookie C', 'Rookie B', 'Rookie A',
						  'Beginner E', 'Beginer D', 'Beginer C', 'Beginer B', 'Beginer A', 'Junior E', 'Junior D',
						  'Junior C', 'Junior B', 'Junior A', 'Senior E', 'Senior D', 'Senior C', 'Senior B', 'Senior A',
						  'Amateur E', 'Amateur D', 'Amateur C', 'Amateur B', 'Amateur A', 'Semi-Pro E', 'Semi-Pro D',
						  'Semi-Pro C', 'Semi-Pro B', 'Semi-Pro A', 'Pro E', 'Pro D', 'Pro C', 'Pro B', 'Pro A', 'National E',
						  'National D', 'National C', 'National B', 'National A', 'World Pro E', 'World Pro D', 'World Pro C',
						  'World Pro B', 'World Pro A', 'Master E', 'Master D', 'Master C', 'Master B', 'Master A',
						  'Top Master E', 'Top Master D', 'Top Master C', 'Top Master B', 'Top Master A',
						  'World Master E', 'World Master D', 'World Master C', 'World Master B', 'World Master A',
						  'Legend V', 'Legend IV', 'Legend III', 'Legend II', 'Legend I', 'Infinit Legend V',
						  'Infinit Legend IV', 'Infinit Legend III', 'Infinit Legend II', 'Infinit Legend I'];

	private $argumentos;

	public function __construct($args) {

		$this->db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
		
		$this->params = $this->db->params;

		$this->argumentos = $args;

		if (!isset($_SESSION))
			session_start();

		$this->init_session_log();

		$this->init_session_cadastro();

		$this->init_session_bug_report();

		$this->init_session_rescue();

		if (isset($this->argumentos['post'])) {

			// require reCAPTCHA class
			include_once('recaptcha.inc');

			// Loga
			if (isset($this->argumentos['post']['id']) && isset($this->argumentos['post']['key']) && isset($this->argumentos['post']['level'])
				&& reCAPTCHA::checkPostReCAPTCHA_V3($this->argumentos['post'])) {
				
				if ($this->argumentos['post']['id'] != '' && $this->argumentos['post']['id'] != 'unknown' &&
					$this->argumentos['post']['key'] != '' && $this->argumentos['post']['key'] != 'unknown') {

					if (($result_match = preg_match('/^[a-zA-Z0-9]{3,16}$/', $this->argumentos['post']['id'])) == 0)
						$this->Error = 5;	// Dados invalidos
					else if ($result_match != 1)
						$this->Error = 6;	// Erro no preg match

					if (($result_match = preg_match('/^[0-9A-z@#$%&*!_?=+\'\-]{6,16}$/', $this->argumentos['post']['key'])) == 0)
						$this->Error = 5;	// Dados invalidos
					else if ($result_match != 1)
						$this->Error = 6;	// Error no preg match

					// Verifica reCAPTCHA V3 do login
					if (reCAPTCHA::isEnable()) {

						$resposta = reCAPTCHA::checkReCAPTCHA_V3($this->argumentos['post']);
						
						if ($resposta->isSuccess() == 0 || ($resposta->isSuccess() == 1 && ($resposta->getScore() <= 0.3 || !$resposta->checkAction('login'))))
							$this->Error = 15;
					}

					if ($this->Error == 0) {
					
						if ($this->argumentos['post']['level'] == -1)
							$this->argumentos['post']['key'] = strtoupper(md5($this->argumentos['post']['key']));

						if (isset($_SESSION['player_log']))
							unset($_SESSION['player_log']);

						// Loga aqui
						$this->params->clear();
						$this->params->add('s', $this->argumentos['post']['id']);
						$this->params->add('s', $this->argumentos['post']['key']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = "SELECT a.uid, a.password, a.capability as cap, b.[level], a.nick FROM ".$this->db->con_dados['DB_NAME'].".account a, ".$this->db->con_dados['DB_NAME'].".user_info b WHERE a.id = ? AND a.password = ? AND a.uid = b.uid";
						else if (DatabaseConfig::_PSQL_)
							$query = "SELECT a.uid, a.password, a.capability as cap, b.level, a.nick FROM ".$this->db->con_dados['DB_NAME'].".account a, ".$this->db->con_dados['DB_NAME'].".user_info b WHERE a.id = ? AND a.password = ? AND a.uid = b.uid";
						else
							$query = "SELECT a.uid, a.password, a.capability as cap, b.`level`, a.nick FROM account a, user_info b WHERE a.id = ? AND a.password = ? AND a.uid = b.uid";

						if ($result = $this->db->db->execPreparedStmt($query, $this->params->get())) {
							if ($result->num_rows > 0) {
								$row = $result->fetch_assoc();
								
								if (isset($row['uid']) && $row['uid'] >= 0) {
									
									$_SESSION['player_log'] = [
											'uid' => isset($row['uid']) ? $row['uid'] : -1,
											'cap' => isset($row['cap']) ? $row['cap'] : 0,
											'nick' => (isset($row['nick'])) ? mb_convert_encoding($row['nick'], 'UTF-8', 'SJIS') : 'unknown',
											'id' => $this->argumentos['post']['id'],
											'key' => $this->argumentos['post']['key'],
											'level' => (isset($row['level'])) ? $row['level'] : $this->argumentos['post']['level'],
											'cookie' => 0,
											'pang' => 0,
											'ip' => !empty(getenv("REMOTE_ADDR"))  ? getenv("REMOTE_ADDR") : 'unknwon'
									];

									$this->init_session_log();

								}else
									$this->Error = 1;
							}else
								$this->Error = 2;
						}else
							$this->Error = 3;
					}

					if ($this->Error != 0)
						error_log("[Login][Error] Login[ID=".$this->argumentos['post']['id'].", IP=".(!empty(getenv("REMOTE_ADDR"))  
								? getenv("REMOTE_ADDR") 
								: 'unknwon')."] Code: ".$this->Error, 0/*File log somente*/);
					else
					    error_log("[Login][Log] fez Login[ID=".$this->argumentos['post']['id'].", IP=".(!empty(getenv("REMOTE_ADDR"))  
								? getenv("REMOTE_ADDR") 
								: 'unknwon')."]", 0/*File log somente*/);
				}

			}else if (isset($this->argumentos['post']['change_nickname']) && $this->argumentos['post']['change_nickname'] 
					&& isset($this->argumentos['post']['uid']) && $this->argumentos['post']['uid'] >= 0
					&& reCAPTCHA::checkPostReCAPTCHA_V3($this->argumentos['post'])) {	// Change nickname

				if (isset($_SESSION['player_log']) && $_SESSION['player_log']['uid'] == $this->argumentos['post']['uid']) {

					// Verifica reCAPTCHA
					if (reCAPTCHA::isEnable()) {

						$resposta = reCAPTCHA::checkReCAPTCHA_V3($this->argumentos['post']);

						if ($resposta->isSuccess() == 0 || ($resposta->isSuccess() == 1 && ($resposta->getScore() <= 0.3 || !$resposta->checkAction('changenick'))))
							$this->msg = 15;
					}

					if ($this->msg == 0) {

						$this->params->clear();
						$this->params->add('i', $_SESSION['player_log']['uid']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcChangeNickname ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'select "_RET_" as "RET", "_CP_" as cookie from '.$this->db->con_dados['DB_NAME'].'.ProcChangeNickname(?)';
						else
							$query = 'call ProcChangeNickname(?)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0) {

							if (($row = $result->fetch_assoc()) && isset($row['RET']) && isset($row['cookie'])) {

								switch ($row['RET']) {
									case 1:		// Trocou o nickname com sucesso
									{
										$this->msg = 1;

										// Atualiza os cookies point do player na session
										$_SESSION['player_log']['cookie'] = $row['cookie'];

										// Cria o CP Log no banco de dados que o player gastou, trocando o nickname
										$this->params->clear();
										$this->params->add('i', $_SESSION['player_log']['uid']);

										$query = '';

										// Type 3 Change Nickname, 60 CP
										if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
											$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcInsertCPLog ?, 3, -1, 60, 0';
										else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
											$query = 'SELECT "_IDX_" as id FROM '.$this->db->con_dados['DB_NAME'].'.ProcInsertCPLog(?, 3, -1, 60, 0)';
										else
											$query = 'call ProcInsertCPLog(?, 3, -1, 60, 0)';

										// Insert CP Log
										if (($cplog_result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0) {
											
											// Check error and put and file error log of php
											if (!($cplog_row = $cplog_result->fetch_assoc()) || !isset($cplog_row['id']) || $cplog_row['id'] == -1)
												error_log('[CPLog][Error] Player[UID='.$_SESSION['player_log']['uid'].'] Não conseguiu inserir o CP Log de troca de nickname.', 0);
											else
												error_log('[CPLog][Log] Player[UID='.$_SESSION['player_log']['uid'].'] Inseriu o CP Log[ID='.$cplog_row['id'].'] de troca de nickname.', 0);
										
										}else
											error_log('[CPLog][Error] Player[UID='.$_SESSION['player_log']['uid'].'] Não conseguiu inserir o CP Log de troca de nickname. SQL Error: '.$this->db->db->getLastError(), 0);

										break;
									}
									case -2:	// Não tem Cookies Point suficiente
										$this->msg = 3;
										break;
									case -3:	// Não passou um mês desde que ele trocou o nickname
										$this->msg = 2;
										break;
									case -4:	// Já fez o pedido para trocar o nickname
										$this->msg = 4;
										break;
									case -1:	// Não conseguiu fazer o pedido para trocar o nickname
									default:
										$this->msg = 7;
										break;
								}

							}else
								$this->msg = 6;	// Não conseguiu fazer o pedido para trocar o nickname, retorno da chamada do proc é desconhecido

						}else
							$this->msg = 5;	// Não conseguiu fazer o pedido para trocar o nickname
					}
				
				}else
					$this->Error = 5;	// Não está logado

			}else if (isset($this->argumentos['post']['pwd_old']) && isset($this->argumentos['post']['pwd_new']) 
				&& isset($this->argumentos['post']['pwd_new_confirm']) && isset($this->argumentos['post']['uid']) 
				&& $this->argumentos['post']['uid'] >= 0 && reCAPTCHA::checkPostReCAPTCHA_V3($this->argumentos['post'])) { // Change Password

				if (isset($_SESSION['player_log']) && $_SESSION['player_log']['uid'] == $this->argumentos['post']['uid']) {

					if ($this->argumentos['post']['pwd_new'] != $this->argumentos['post']['pwd_new_confirm'])
						$this->msg = 3;	// Senha diferente

					if (($result_match = preg_match('/^[0-9A-z@#$%&*!_?=+\'\-]{6,16}$/', $this->argumentos['post']['pwd_new'])) == 0)
						$this->msg = 5;	// Dados invalidos
					else if ($result_match != 1)
						$this->msg = 6;	// Erro no preg_match

					if (($result_match = preg_match('/^[0-9A-z@#$%&*!_?=+\'\-]{6,16}$/', $this->argumentos['post']['pwd_old'])) == 0)
						$this->msg = 5;	// Dados invalidos
					else if ($result_match != 1)
						$this->msg = 6;	// Erro no preg_match

					if ($this->key != strtoupper(md5($this->argumentos['post']['pwd_old'])))
						$this->msg = 4;	// Senha atual é diferente da fornecida

					// Verifica reCAPTCHA V3
					if (reCAPTCHA::isEnable()) {

						if (isset($this->argumentos['post']))
							$resposta = reCAPTCHA::checkReCAPTCHA_V3($this->argumentos['post']);

						if ($resposta->isSuccess() == 0 || (!$resposta->isSuccess() == 1 && ($resposta->getScore() <= 0.3 || !$resposta->checkAction('changepass'))))
							$this->msg = 15;
					}

					if ($this->msg == 0) {

						unset($this->argumentos['post']['pwd_new_confirm']);

						$this->argumentos['post']['pwd_new'] = strtoupper(md5($this->argumentos['post']['pwd_new']));

						$this->params->clear();
						$this->params->add('i', $_SESSION['player_log']['uid']);
						$this->params->add('s', $this->argumentos['post']['pwd_new']);

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
										$this->msg = 1;

										// Cria o Email e envia aqui nessa parte
										$link = $_SERVER['REQUEST_SCHEME']."://".$_SERVER['HTTP_HOST']."/pangya.php";

										$bodymail = "PangYa SuperSS - Aviso.\n\nA senha da sua conta ID: ".$this->id." foi trocada, se foi você desconsidere esse e-mail.\n\n";
										$bodymail .= "Connection IP: ".getenv('REMOTE_ADDR')."\n\nDate: ".date("d/m/Y H:i:s (e)")."\n\nWebsite: ".$link;

										if (($result_match = preg_match('/^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$/', $row['EMAIL'])) == 1) {

											// Envia mail para o email do player dizendo que a senha dele foi trocada
											require_once('smtp_base.inc');

											// Insira abaixo o email que irá receber a mensagem, o email que irá enviar (o mesmo da variável GUSER), 
											// o nome do email que envia a mensagem, o Assunto da mensagem e por último a variável com o corpo do email.

											if (!smtpmailer($row['EMAIL'], '#Seu email do remetente#', 'PangYa SuperSS', '[Aviso] A senha da sua conta foi trocada', $bodymail))
												error_log('player[UID='.$this->uid.'] Não conseguiu enviar o mail{'.$bodymail.'} de aviso que a senha foi trocada para seu email: '.$row['EMAIL'], 0/*Arqui de log do php*/);
											
										}else
											error_log('player[UID='.$this->uid.'] Não conseguiu enviar o mail{'.$bodymail.'} de aviso de que a senha foi trocada por que ele não tem um email{'.$row['EMAIL'].'} válido.', 0);

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
				
				}else
					$this->Error = 5;	// Não está logado

			}else if (isset($this->argumentos['post']['rescue']) && $this->argumentos['post']['rescue'] && isset($this->argumentos['get']['rescue'])
						&& (isset($this->argumentos['post']['id_rescue']) || isset($this->argumentos['post']['email_rescue']))
						&& reCAPTCHA::checkPostReCAPTCHA_V2($this->argumentos['post'])) {	// Recuperar a senha ou ID
				
				if ($this->argumentos['get']['rescue'] == 1 || $this->argumentos['get']['rescue'] == 2) {

					$query = '';

					if ($this->argumentos['get']['rescue'] == 1 && isset($this->argumentos['post']['id_rescue'])) {
						
						if (($result_match = preg_match('/^[a-zA-Z0-9]{4,16}$/', $this->argumentos['post']['id_rescue'])) == 0)
							$this->msg = 5;		// Dados invalidos
						else if ($result_match != 1)
							$this->msg = 8;		// Erro no preg match

						if ($this->msg == 0) {

							$this->params->clear();
							$this->params->add('s', $this->argumentos['post']['id_rescue']);

							if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
								$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcRescuePassword ?';
							else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
								$query = 'select "_RET_" as "RET", "_UID_" as "UID", "_INDEX_" as "INDEX", "_KEY_" as "KEY", "_EMAIL_" as "EMAIL" from '.$this->db->con_dados['DB_NAME'].'.ProcRescuePassword(?)';
							else
								$query = 'call ProcRescuePassword(?)';
						}

					}else if ($this->argumentos['post']['email_rescue']) {

						if (($result_match = preg_match('/^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$/', $this->argumentos['post']['email_rescue'])) == 0)
							$this->msg = 5;		// Dados invalidos
						else if ($result_match != 1)
							$this->msg = 8;		// Erro no preg match

						if ($this->msg == 0) {

							$this->params->clear();
							$this->params->add('s', $this->argumentos['post']['email_rescue']);

							if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
								$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcRescueID ?';
							else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
								$query = 'select "_RET_" as "RET", "_UID_" as "UID", "_INDEX_" as "INDEX", "_KEY_" as "KEY", "_EMAIL_" as "EMAIL" from '.$this->db->con_dados['DB_NAME'].'.ProcRescueID(?)';
							else
								$query = 'call ProcRescueID(?)';
						}

					}

					// Verifica o reCAPTCHA
					if (reCAPTCHA::isEnable()) {

						$resposta = reCAPTCHA::checkReCAPTCHA_V2($this->argumentos['post']);

						if ($resposta->isSuccess() == 0)
							$this->msg = 15;	// Error no desafio do reCAPTCHA
					}

					if ($this->msg == 0) {

						if ($query != '' && ($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0) {
							
							if (($row = $result->fetch_assoc()) && isset($row['RET'])) {

								switch ($row['RET']) {
									case 1:
									{

										if (isset($row['UID']) && isset($row['INDEX']) && isset($row['KEY']) && isset($row['EMAIL']) && $row['INDEX'] > 0) {

											// Email
											$this->email = $row['EMAIL'];

											// Cria o Email e envia aqui nessa parte
											$link = $_SERVER['REQUEST_SCHEME']."://".$_SERVER['HTTP_HOST']."/pangya.php?link=9&index=".$row['INDEX']."&key=".$row['KEY'];

											$bodymail = "PangYa SuperSS - Recuperação ".($this->argumentos['get']['rescue'] == 1 ? "da senha" : "do id").".\n\n";
											$bodymail .= "Esse email foi enviado para recuperação ".($this->argumentos['get']['rescue'] == 1 ? "da senha" : "do id");
											$bodymail .= ", se não foi você desconsidere esse e-mail e entre em contato atráves do email: supersspangya@gmail.com ou canal do discord.\n\n";
											$bodymail .= "Connection IP: ".getenv('REMOTE_ADDR')."\n\nDate: ".date("d/m/Y H:i:s (e)")."\n\n";
											$bodymail .= "Para recuperar ".($this->argumentos['get']['rescue'] == 1 ? "a senha" : "o id").". clique nesse link: ".$link;
											$bodymail .= "\n\nEssa email expira em uma hora.";

											if (($result_match = preg_match('/^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$/', $row['EMAIL'])) == 1) {

												// Envia mail para o email do player dizendo que a senha dele foi trocada
												require_once('smtp_base.inc');

												// Insira abaixo o email que irá receber a mensagem, o email que irá enviar (o mesmo da variável GUSER), 
												// o nome do email que envia a mensagem, o Assunto da mensagem e por último a variável com o corpo do email.

												if (!smtpmailer($this->email, '#Seu email do remetente#', 'PangYa SuperSS', 'Recuperação '.($this->argumentos['get']['rescue'] == 1 ? 'da senha' : 'do id'), $bodymail)) {

													$this->msg = 6;

													// Log
													error_log('player[UID='.$row['UID'].'] Não conseguiu enviar o mail{'.$bodymail.'} de recuperação '.($this->argumentos['get']['rescue'] == 2 
																? 'do id' 
																: 'da senha').' para seu email: '.$this->email, 0/*Arqui de log do php*/);

												}else
													$this->msg = 1;
												
											}else {
												
												$this->msg = 7;

												// Log
												error_log('player[UID='.$row['UID'].'] Não conseguiu enviar o mail{'.$bodymail.'} de recuperação '.($this->argumentos['get']['rescue'] == 2 
															? 'do id' 
															: 'da senha').', por que ele não tem um email{'.$this->email.'} válido.', 0);
											}

											if ($this->msg == 6 || $this->msg == 7) {

												// Deleta o log de foi criado que não conseguiu enviar o email
												$this->params->clear();
												$this->params->add('i', $row['UID']);
												$this->params->add('i', $row['INDEX']);

												$query = '';

												if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
													$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcDeleteLogRescuePasswordAndID ?, ?';
												else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
													$query = 'select * from '.$this->db->con_dados['DB_NAME'].'.ProcDeleteLogRescuePasswordAndID(?, ?)';
												else
													$query = 'call ProcDeleteLogRescuePasswordAndId(?, ?)';

												if (!(($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0))
													error_log('Player[UID='.$row['UID'].', EMAIL='.$this->email.'] não conseguiu excluir o Rescue password and id[INDEX='.$row['INDEX'].']', 0);
											}
										
										}else
											$this->msg = 13;

										break;
									}
									case -2:
										$this->msg = 2;
										break;
									case -3:
										$this->msg = 3;
										break;
									case -4:
										$this->msg = 4;
										break;
									case -1:
									default:
										$this->msg = 12;
										break;
								}

							}else
								$this->msg = 11;

						}else
							$this->msg = 10;
					}
				
				}else
					$this->msg = 9;

			}else if (isset($this->argumentos['post']['ip']) && isset($this->argumentos['post']['uid'])) {	// Troca o IP
				
				if ($this->argumentos['post']['ip'] != '' && $this->argumentos['post']['uid'] >= 0) {
					
					if (isset($_SESSION['player_log']) && $_SESSION['player_log']['uid'] == $this->argumentos['post']['uid']) {
					
						$this->params->clear();
						$this->params->add('i', $this->argumentos['post']['uid']);

						$query = '';
						
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT uid, change_count, flag_day, CAST(change_date as DATE) as change_date, CAST(getdate() as DATE) as date_now FROM '.$this->db->con_dados['DB_NAME'].'.pangya_player_ip WHERE UID = ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT uid, change_count, flag_day, CAST(change_date as DATE) as change_date, CAST(now() as DATE) as date_now FROM '.$this->db->con_dados['DB_NAME'].'.pangya_player_ip WHERE UID = ?';
						else
							$query = 'SELECT uid, change_count, flag_day, DATE(change_date) as change_date, DATE(NOW()) as date_now FROM pangya_player_ip WHERE UID = ?';

						$result = $this->db->db->execPreparedStmt($query, $this->params->get());

						if ($result && $result->num_rows > 0 && ($row = $result->fetch_assoc())) {
							
							if (isset($row['flag_day']) && isset($row['uid']) && isset($row['change_count']) && isset($row['change_date']) && isset($row['date_now'])) {
								
								if (!$row['flag_day'] || ($row['date_now'] != $row['change_date'] || $row['change_count'] < 3)) {
									
									$count = ($row['change_count'] == 3) ? 1 : $row['change_count'] + 1;

									$this->params->clear();
									$this->params->add('s', $this->argumentos['post']['ip']);
									$this->params->add('i', $count);
									$this->params->add('i', $this->argumentos['post']['uid']);
								
									$query = '';

									if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
										$query = 'UPDATE '.$this->db->con_dados['DB_NAME'].'.pangya_player_ip SET ip = ?, change_count = ?, change_date = getdate() WHERE UID = ?';
									else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
										$query = 'UPDATE '.$this->db->con_dados['DB_NAME'].'.pangya_player_ip SET ip = ?, change_count = ?, change_date = now() WHERE UID = ?';
									else
										$query = 'UPDATE pangya_player_ip SET ip = ?, change_count = ?, change_date = NOW() WHERE UID = ?';

									$this->db->db->execPreparedStmt($query, $this->params->get());

									$this->msg = 1; /*Trocou o ip*/

								}else
									$this->msg = 2;

							}else
								$this->Error = 5;

						}else if ($this->db->db->getLastError() == 0) {	// sem erro na consulta anterior, então ele não tem um ip no db ainda
							
							$this->params->clear();
							$this->params->add('i', $this->argumentos['post']['uid']);
							$this->params->add('s', $this->argumentos['post']['ip']);
							
							$query = '';
							
							if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
								$query = 'INSERT INTO '.$this->db->con_dados['DB_NAME'].'.pangya_player_ip(UID, ip, change_count) VALUES(?, ?, 1)';
							else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
								$query = 'INSERT INTO '.$this->db->con_dados['DB_NAME'].'.pangya_player_ip(UID, ip, change_count) VALUES(?, ?, 1)';
							else
								$query = 'INSERT INTO pangya_player_ip(UID, ip, change_count) VALUES(?, ?, 1)';

							$this->db->db->execPreparedStmt($query, $this->params->get());

							$this->msg = 1; /*Trocou o ip*/
						}
					}
				
				}else
					$this->Error = 5;	// Não está logado

			}else if (isset($this->argumentos['post']['pincode'])) {	// Pin Code do cadastro do beta
				
				$keys = null;
				$key_free = false;

				if (file_exists('config/keys.txt'))
					$keys = unserialize(file_get_contents('config/keys.txt'));

				if ($keys) {
					foreach ($keys as $key => $value) {
						if (isset($value['unique_id']) && isset($value['flag'])) {
							if (!$value['flag'] && $value['unique_id'] == $this->argumentos['post']['pincode']) {
								$key_free = true;
								break;
							}
						}
					}
				}

				if (!$key_free)
					echo "<script>window.addEventListener('load', function() { alert('Chave errada ou já foi utilizada.')});</script>";
				else {
					$this->pincode = $this->argumentos['post']['pincode'];

					if (isset($_SESSION['player_cadastro']))
						unset($_SESSION['player_cadastro']);

					$_SESSION['player_cadastro'] = ['pincode' => $this->pincode];

					$this->init_session_cadastro();
				}

			}else if (count($this->argumentos['post']) == (reCAPTCHA::isEnable() ? 6 : 5) && isset($this->argumentos['post']['nome']) && isset($this->argumentos['post']['sobre_nome']) 
					&& isset($this->argumentos['post']['email']) && isset($this->argumentos['post']['confirm_email'])
					&& reCAPTCHA::checkPostReCAPTCHA_V2($this->argumentos['post'])) {	// Primeira etapa do Cadastro
				
				include("first_cadastro_exec.inc");
			
			}else if (isset($this->argumentos['post']['id']) && isset($this->argumentos['post']['pass']) && isset($this->argumentos['post']['confirm_pass']) 
					&& isset($this->argumentos['post']['ip']) && isset($this->argumentos['post']['index']) && isset($this->argumentos['post']['sexo'])) { // Segunda etapa do Cadastro
				
				include("seg_cadastro_exec.inc");
				
			}else if (isset($this->argumentos['post']['rescue_pass']) && isset($this->argumentos['post']['rescue_confirm_pass']) 
						&& isset($this->argumentos['post']['index']) && isset($this->argumentos['post']['uid'])) {	// Redefinição do password 

				include_once("seg_rescue_password_exec.inc");

			} else if (isset($this->argumentos['post']['bug_report_ta'])) {	// Bug report
				
				if (issset($_SESSION['player_log']) && $this->argumentos['post']['bug_report_ta'] != '') {

					if (isset($_SESSION['bug_report']))
						unset($_SESSION['bug_report']);

					$_SESSION['bug_report'] = ['bug_report_ta' => $this->argumentos['post']['bug_report_ta']];

					$this->init_session_bug_report();
				}

			} else if (isset($this->argumentos['post']['bug_report_ta_confirm'])) {	// Bug report confirm
				
				if (isset($_SESSION['player_log']) && $this->argumentos['post']['bug_report_ta_confirm'] != '') {

					$reports = null;

					if (file_exists('config/bug_report.txt'))
						$reports = unserialize(file_get_contents('config/bug_report.txt'));

					$reports[] = ['uid' => $this->uid, 'id' => $this->id, 'bug_report' => $this->argumentos['post']['bug_report_ta_confirm']];

					file_put_contents('config/bug_report.txt', serialize($reports));

					if (isset($_SESSION['bug_report']))
						unset($_SESSION['bug_report']);

					$this->bug_ta = '';

					$this->msg = 20;	// Bug report enviado com sucesso
				}

			} else if (isset($this->argumentos['post']['ban_min'])) {	// Edit Player State Flag (Ban)
				
				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && isset($_GET['uid'])) {
					
					$state_id = 0;
					$blocktime = 0;
					$is_block = false;
					$player_uid = $_GET['uid'];

					if (isset($this->argumentos['post']['ban_tmp']) && isset($this->argumentos['post']['ban_min'])
							&& $this->argumentos['post']['ban_tmp'] != '' && is_numeric($this->argumentos['post']['ban_tmp'])
							&& $this->argumentos['post']['ban_min'] != '' && is_numeric($this->argumentos['post']['ban_min'])) {

						$state_id |= $this->argumentos['post']['ban_tmp'];
						$blocktime = $this->argumentos['post']['ban_min'];

						$this->params->clear();
						$this->params->add('i', $player_uid);

						$query = '';
						
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT ISNULL(DATEADD(MINUTE, BlockTime, BlockRegDate), NULL) as blockregdate FROM '.$this->db->con_dados['DB_NAME'].'.account WHERE UID = ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = "SELECT COALESCE((BlockRegDate + ('1 minute'::interval * BlockTime)), NULL) as blockregdate FROM ".$this->db->con_dados['DB_NAME'].'.account WHERE UID = ?';
						else
							$query = 'SELECT IFNULL(TIMESTAMPADD(MINUTE, BlockTime, BlockRegDate), -1) as blockregdate FROM account WHERE UID = ?';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $result->num_rows > 0) {

							if (($row = $result->fetch_assoc()) && isset($row['blockregdate'])) {
								
								// Date string to UNIX TIME
								$date_sec = ($row['blockregdate'] == null || $row['blockregdate'] == -1) ? -1 : strtotime($row['blockregdate']);

								//date_default_timezone_set("America/Sao_Paulo");

								$is_block = ($row['blockregdate'] != -1 && $date_sec > time()) ? true : false;
							}
						}
					}

					$state_id |= (isset($this->argumentos['post']['ban_perm'])) ? $this->argumentos['post']['ban_perm'] : 0;
					$state_id |= (isset($this->argumentos['post']['ban_lounge'])) ? $this->argumentos['post']['ban_lounge'] : 0;
					$state_id |= (isset($this->argumentos['post']['ban_sh_lounge'])) ? $this->argumentos['post']['ban_sh_lounge'] : 0;
					$state_id |= (isset($this->argumentos['post']['ban_gift_shop'])) ? $this->argumentos['post']['ban_gift_shop'] : 0;
					$state_id |= (isset($this->argumentos['post']['ban_papel'])) ? $this->argumentos['post']['ban_papel'] : 0;
					$state_id |= (isset($this->argumentos['post']['ban_scratchy'])) ? $this->argumentos['post']['ban_scratchy'] : 0;
					$state_id |= (isset($this->argumentos['post']['ban_ticker'])) ? $this->argumentos['post']['ban_ticker'] : 0;
					$state_id |= (isset($this->argumentos['post']['ban_memorial'])) ? $this->argumentos['post']['ban_memorial'] : 0;
					$state_id |= (isset($this->argumentos['post']['ban_all_ip']) ? $this->argumentos['post']['ban_all_ip'] : 0);
					$state_id |= (isset($this->argumentos['post']['ban_all_mac']) ? $this->argumentos['post']['ban_all_mac'] : 0);
					$state_id |= (isset($this->argumentos['post']['ban_create_guild']) ? $this->argumentos['post']['ban_create_guild'] : 0);
					$state_id |= (isset($this->argumentos['post']['ban_lucia_attendance']) ? $this->argumentos['post']['ban_lucia_attendance'] : 0);

					$query = '';
					
					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'UPDATE '.$this->db->con_dados['DB_NAME'].'.account SET idstate = ?, blocktime = ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'UPDATE '.$this->db->con_dados['DB_NAME'].'.account SET idstate = ?, blocktime = ?';
					else
						$query = 'UPDATE account SET idstate = ?, blocktime = ?';

					if (($state_id & 1) && !$is_block) // troca a data se tiver block por tempo e não está mais bloqueado
						$query .= ', BlockRegDate = '.((DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed']) ? 'getdate()' : 'NOW()');

					$query .= ' WHERE UID = ?';

					$this->params->clear();
					$this->params->add('i', $state_id);
					$this->params->add('i', $blocktime);
					$this->params->add('i', $player_uid);

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0)
						$this->msg = 25;	// Trocou State ID do player com sucesso
				}

			} else if (isset($this->argumentos['post']['cap_edit']) && $this->argumentos['post']['cap_edit'] == 'none') {	// Edit Player Capability

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && isset($_GET['uid'])) {

					$cap = 0;
					$player_uid = $_GET['uid'];

					$cap |= (isset($this->argumentos['post']['cap_block_give_item'])) ? $this->argumentos['post']['cap_block_give_item'] : 0;
					$cap |= (isset($this->argumentos['post']['cap_gm'])) ? $this->argumentos['post']['cap_gm'] : 0;
					$cap |= (isset($this->argumentos['post']['cap_gm_site'])) ? $this->argumentos['post']['cap_gm_site'] : 0;
					$cap |= (isset($this->argumentos['post']['cap_mantle'])) ? $this->argumentos['post']['cap_mantle'] : 0;
					$cap |= (isset($this->argumentos['post']['cap_title_gm'])) ? $this->argumentos['post']['cap_title_gm'] : 0;
					$cap |= (isset($this->argumentos['post']['cap_AI'])) ? $this->argumentos['post']['cap_AI'] : 0;
					
					$query = '';

					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'UPDATE '.$this->db->con_dados['DB_NAME'].'.account SET capability = ? WHERE [UID] = ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'UPDATE '.$this->db->con_dados['DB_NAME'].'.account SET capability = ? WHERE UID = ?';
					else
						$query = 'UPDATE '.$this->db->con_dados['DB_NAME'].'.account SET capability = ? WHERE `UID` = ?';

					$this->params->clear();
					$this->params->add('i', $cap);
					$this->params->add('i', $player_uid);

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0)
						$this->msg = 26;	// Trocou Capability do player com sucesso
				}

			} else if (isset($this->argumentos['post']['rate']) && isset($this->argumentos['post']['qntd']) && isset($this->argumentos['post']['reserved_date']) 
					&& isset($this->argumentos['post']['alvo']) && isset($this->argumentos['get']['adm']) && isset($this->argumentos['get']['aba'])) { // Comando Edite Rate Server
				
				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && $this->argumentos['post']['alvo'] != '' && is_numeric($this->argumentos['post']['alvo'])) {
					
					$reserved_date = null;

					if ($this->argumentos['post']['reserved_date'] != '' && $this->argumentos['post']['reserved_date'] != 0 && is_numeric($this->argumentos['post']['reserved_date'])) {
						//date_default_timezone_set('America/Sao_Paulo');

						$reserved_date = date('Y-m-d H:i:s', time() + ($this->argumentos['post']['reserved_date'] * 60));
					}

					$this->params->clear();
					$this->params->add('i', $this->argumentos['post']['rate']);
					$this->params->add('i', $this->argumentos['post']['qntd']);
					$this->params->add('i', $this->argumentos['post']['alvo']);
					$this->params->add('s', $reserved_date);

					$query = '';

					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcRegisterNewRateServer ?, ?, ?, ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcRegisterNewRateServer(?, ?, ?, ?)';
					else
						$query = 'CALL ProcRegisterNewRateServer(?, ?, ?, ?)';

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0)
						$this->msg = 35;		// Enviou o commando pra trocar o rate do server com sucesso
				}
				
			} else if (isset($this->argumentos['post']['reload']) && isset($this->argumentos['post']['reserved_date']) && isset($this->argumentos['post']['alvo']) 
					&& isset($this->argumentos['get']['adm']) && isset($this->argumentos['get']['aba'])) { // Comando Reload System Server
				
				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && $this->argumentos['post']['alvo'] != '' && is_numeric($this->argumentos['post']['alvo'])) {
					
					$reserved_date = null;

					if ($this->argumentos['post']['reserved_date'] != '' && $this->argumentos['post']['reserved_date'] != 0 && is_numeric($this->argumentos['post']['reserved_date'])) {
						//date_default_timezone_set('America/Sao_Paulo');

						$reserved_date = date('Y-m-d H:i:s', time() + ($this->argumentos['post']['reserved_date'] * 60));
					}

					$this->params->clear();
					$this->params->add('i', $this->argumentos['post']['reload']);
					$this->params->add('i', $this->argumentos['post']['alvo']);
					$this->params->add('s', $reserved_date);

					$query = '';

					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcRegisterReloadSystemServer ?, ?, ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcRegisterReloadSystemServer(?, ?, ?)';
					else
						$query = 'CALL ProcRegisterReloadSystemServer(?, ?, ?)';

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0)
						$this->msg = 40;		// Enviou o commando pra Recarregar Sistema do server com sucesso
				}

			} else if (isset($this->argumentos['post']['ip']) && isset($this->argumentos['post']['mask'])) {

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8)
					&& $this->argumentos['post']['ip'] != '' && $this->argumentos['post']['mask']) {

					if (($result_match = preg_match('/^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$|((?:[0-9]{1,3}\.){3}[0-9]{1,3}\/(?:[0-9]{1,3}\.){3}[0-9]{1,3})$/', $this->argumentos['post']['ip'])) == 0)
						$this->msg = 43;    // IP invalido
					else if ($result_match != 1)
						$this->msg = 47;     // Erro no preg match

					if (($result_match = preg_match('/^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$/', $this->argumentos['post']['mask'])) == 0)
						$this->msg = 44;    // MASK invalido
					else if ($result_match != 1)
						$this->msg = 47;     // Erro no preg match 

					if ($this->msg == 0) {

						$this->params->clear();
						$this->params->add('s', $this->argumentos['post']['ip']);
						$this->params->add('s', $this->argumentos['post']['mask']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcInsertBlockIP ?, ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcInsertBlockIP(?, ?)';
						else
							$query = 'call ProcInsertBlockIP(?, ?)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0)
							$this->msg = 41;
						else
							$this->msg = 46;

					}

				}

			} else if (isset($this->argumentos['post']['mac_address'])) {

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8)
					&& $this->argumentos['post']['mac_address'] != '') {

					if (($result_match = preg_match('/^(?:[a-fA-F0-9]{2}\:){5}[a-fA-F0-9]{2}$/', $this->argumentos['post']['mac_address'])) == 0)
						$this->msg = 50;    // MAC ADDRESS invalido
					else if ($result_match != 1)
						$this->msg = 47;     // Erro no preg match 

					if ($this->msg == 0) {

						$this->params->clear();
						$this->params->add('s', $this->argumentos['post']['mac_address']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcInsertBlockMAC ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcInsertBlockMAC(?)';
						else
							$query = 'call ProcInsertBlockMAC(?)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0)
							$this->msg = 48;
						else
							$this->msg = 51;

					}

				}

			} else if (isset($this->argumentos['post']['index']) && isset($this->argumentos['post']['del_block_ip']) && $this->argumentos['post']['del_block_ip'] == 1) {

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && is_numeric($this->argumentos['post']['index'])) {

					$this->params->clear();
					$this->params->add('i', $this->argumentos['post']['index']);

					$query = '';

					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcDeleteBlockIPFromTable ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcDeleteBlockIPFromTable(?)';
					else
						$query = 'call ProcDeleteBlockIPFromTable(?)';

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0)
						$this->msg = 42;
					else
						$this->msg = 45;

				}

		 	} else if (isset($this->argumentos['post']['index']) && isset($this->argumentos['post']['del_block_mac']) && $this->argumentos['post']['del_block_mac'] == 1) {

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && is_numeric($this->argumentos['post']['index'])) {

					$this->params->clear();
					$this->params->add('i', $this->argumentos['post']['index']);

					$query = '';

					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcDeleteBlockMACFromTable ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcDeleteBlockMACFromTable(?)';
					else
						$query = 'call ProcDeleteBlockMACFromTable(?)';

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0)
						$this->msg = 49;
					else
						$this->msg = 52;

				}

			} else if (isset($this->argumentos['post']['broadcast']) && isset($this->argumentos['post']['replay_count']) && isset($this->argumentos['post']['refresh_time']) 
					&& isset($this->argumentos['post']['reserved_date']) && isset($this->argumentos['post']['alvo'])) {	// Comando Broadcast Mensagem
				
				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && $this->argumentos['post']['alvo'] != '' 
						&& is_numeric($this->argumentos['post']['alvo']) && $this->argumentos['post']['broadcast'] != '') {
					
					$reserved_date = null;

					if ($this->argumentos['post']['reserved_date'] != '' && $this->argumentos['post']['reserved_date'] != 0 && is_numeric($this->argumentos['post']['reserved_date'])) {
						//date_default_timezone_set('America/Sao_Paulo');

						$reserved_date = date('Y-m-d H:i:s', time() + ($this->argumentos['post']['reserved_date'] * 60));
					}

					if ($this->argumentos['post']['replay_count'] == '')
						$this->argumentos['post']['replay_count'] = 1;

					if ($this->argumentos['post']['refresh_time'] == '')
						$this->argumentos['post']['refresh_time'] = 0;

					$msg = verifyAndEncode($this->argumentos['post']['broadcast']);

					$this->params->clear();
					$this->params->add('s', $msg/* $this->argumentos['post']['broadcast'] */);
					$this->params->add('i', $this->argumentos['post']['replay_count']);
					$this->params->add('i', $this->argumentos['post']['refresh_time']);
					$this->params->add('i', $this->argumentos['post']['alvo']);
					$this->params->add('s', $reserved_date);

					$query = '';
					
					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcRegisterNoticeBroadcast ?, ?, ?, ?, ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcRegisterNoticeBroadcast(?, ?, ?, ?, ?)';
					else
						$query = 'CALL ProcRegisterNoticeBroadcast(?, ?, ?, ?, ?)';

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0)
						$this->msg = 36;	// Enviou o comando de broadcast com sucesso
				}

			} else if (isset($this->argumentos['post']['insert-dona']) && $this->argumentos['post']['insert-dona'] == "insert" && isset($this->argumentos['post']['uid']) 
				&& isset($this->argumentos['post']['plataforma']) && isset($this->argumentos['post']['gross_amount']) && isset($this->argumentos['post']['email']) 
				&& isset($this->argumentos['post']['net_amount']) && isset($this->argumentos['post']['status']) && isset($this->argumentos['post']['type']) && isset($this->argumentos['post']['date'])
				&& isset($this->argumentos['post']['update']) && isset($this->argumentos['post']['escrow']) && isset($this->argumentos['post']['reference'])
				&& isset($this->argumentos['post']['code']) && !is_nan($this->argumentos['post']['uid'])) {	// Comando Registra Doação
				
				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8)
					&& !is_nan($this->argumentos['post']['plataforma']) && !is_nan($this->argumentos['post']['type']) && !is_nan($this->argumentos['post']['status'])
					&& !is_nan($this->argumentos['post']['gross_amount']) && !is_nan($this->argumentos['post']['net_amount'] && $this->argumentos['post']['code']) != ''
					&& $this->argumentos['post']['email'] != '' && $this->argumentos['post']['reference'] != '' && $this->argumentos['post']['date'] != ''
					&& $this->argumentos['post']['update'] != '' && $this->argumentos['post']['escrow'] != '') {

					if (strcasecmp($this->argumentos['post']['update'], "NULL") == 0)
						$this->argumentos['post']['update'] = null;

					if (strcasecmp($this->argumentos['post']['escrow'], "NULL") == 0)
						$this->argumentos['post']['escrow'] = null;

					$lambda_register_dona = function() {

						$transaction = new stdClass();
						$transaction->code = $this->argumentos['post']['code'];
						$transaction->plataform = $this->argumentos['post']['plataforma'];
						$transaction->uid = $this->argumentos['post']['uid'];
						$transaction->status = $this->argumentos['post']['status'];
						$transaction->lastEventDate = $this->argumentos['post']['update'];
						$transaction->grossAmount = $this->argumentos['post']['gross_amount'];
						$transaction->netAmount = $this->argumentos['post']['net_amount'];
						$transaction->escrowEndDate = $this->argumentos['post']['escrow'];
						$transaction->date = $this->argumentos['post']['date'];
						$transaction->type = $this->argumentos['post']['type'];
						$transaction->reference = $this->argumentos['post']['reference'];

						$transaction->sender = new stdClass();
						$transaction->sender->email = $this->argumentos['post']['email'];

						$dona = new DonationManager($this->db);

						$reply = $dona->insertTransaction($transaction);

						switch ($reply) {
							case 0:
								return 1101; // Success
							case 1:
								return 1010; // Fail exec query consulting
							case 2:
								return 1011; // Don't have donation in db
							case 3:
								return 1012; // Fail in query consulting
							case 4:
								return 1013; // Fail exec proc update
							case 5:
								return 1014; // Fail in proc update, result empty
							case 6:
								return 1015; // Fail in proc update, result invalid
							default:
								return 1016; // Unknown error
						}
					};

					// Registra doação
					$this->msg = $lambda_register_dona();
				}

			} else if (isset($this->argumentos['post']['edit-dona']) && $this->argumentos['post']['edit-dona'] == 'edit'
					&& isset($this->argumentos['post']['id']) && isset($this->argumentos['post']['type']) && isset($this->argumentos['post']['status'])
					&& isset($this->argumentos['post']['plataforma']) && isset($this->argumentos['post']['date'])
					&& isset($this->argumentos['post']['update']) && isset($this->argumentos['post']['uid']) && isset($this->argumentos['post']['email'])
					&& isset($this->argumentos['post']['reference']) && isset($this->argumentos['post']['code']) && isset($this->argumentos['post']['gross_amount'])
					&& isset($this->argumentos['post']['net_amount']) && isset($this->argumentos['post']['escrow']) && isset($this->argumentos['post']['epin_id']) 
					&& !is_nan($this->argumentos['post']['uid']) && !is_nan($this->argumentos['post']['id'])) {  // Atualiza Donation

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8)
					&& $this->argumentos['post']['code'] != '' && !is_nan($this->argumentos['post']['plataforma']) && !is_nan($this->argumentos['post']['type'])
					&& !is_nan($this->argumentos['post']['status']) && $this->argumentos['post']['update'] != '' && $this->argumentos['post']['escrow'] != ''
					&& $this->argumentos['post']['date'] != '' && !is_nan($this->argumentos['post']['epin_id']) && !is_nan($this->argumentos['post']['gross_amount'])
					&& !is_nan($this->argumentos['post']['net_amount']) && $this->argumentos['post']['email'] != '' && $this->argumentos['post']['reference'] != '') {

					$lambda_update_dona = function() {

						if (strcasecmp($this->argumentos['post']['update'], "NULL") == 0)
							$this->argumentos['post']['update'] = null;

						if (strcasecmp($this->argumentos['post']['escrow'], "NULL") == 0)
							$this->argumentos['post']['escrow'] = null;

						$transaction = new stdClass();
						$transaction->id = $this->argumentos['post']['id'];
						$transaction->code = $this->argumentos['post']['code'];
						$transaction->plataform = $this->argumentos['post']['plataforma'];
						$transaction->uid = $this->argumentos['post']['uid'];
						$transaction->status = $this->argumentos['post']['status'];
						$transaction->lastEventDate = $this->argumentos['post']['update'];
						$transaction->epin_id = $this->argumentos['post']['epin_id'];
						$transaction->grossAmount = $this->argumentos['post']['gross_amount'];
						$transaction->netAmount = $this->argumentos['post']['net_amount'];
						$transaction->escrowEndDate = $this->argumentos['post']['escrow'];
						$transaction->date = $this->argumentos['post']['date'];
						$transaction->type = $this->argumentos['post']['type'];
						$transaction->reference = $this->argumentos['post']['reference'];

						$transaction->sender = new stdClass();
						$transaction->sender->email = $this->argumentos['post']['email'];

						$dona = new DonationManager($this->db);

						$reply = $dona->updateTransaction($transaction);

						switch ($reply) {
							case 0:
								return 1100; // Success
							case 1:
								return 1000; // Fail exec query consulting
							case 2:
								return 1001; // Don't have donation in db
							case 3:
								return 1002; // Fail in query consulting
							case 4:
								return 1003; // Fail exec proc update
							case 5:
								return 1004; // Fail in proc update, result empty
							case 6:
								return 1005; // Fail in proc update, result invalid
							default:
								return 1006; // Unknown error
						}
					};

					// Message
					$this->msg = $lambda_update_dona();
				}

			} else if (isset($this->argumentos['post']['del-dona']) && $this->argumentos['post']['del-dona'] == 'delete'
					&& isset($this->argumentos['post']['id']) && !is_nan($this->argumentos['post']['id'])) { // Deleta Donation
				
					if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8)) {

						$lambda_delete_dona = function() {
	
							$dona = new DonationManager($this->db);
	
							$reply = $dona->deleteTransaction($this->argumentos['post']['id']);
	
							switch ($reply) {
								case 0:
									return 1102; // Success
								case 1:
									return 1020; // Fail exec query delete donation
								case 2:
									return 1021; // Don't have donation in db
								case 3:
									return 1022; // Fail in query delete donation
								case 4:
									return 1023; // Invalid donation id
								default:
									return 1024; // Unknown error
							}
						};

						// Message
						$this->msg = $lambda_delete_dona();
					}

			} else if (isset($this->argumentos['post']['uid']) && isset($this->argumentos['post']['item_typeid_1']) 
				&& isset($this->argumentos['post']['item_typeid_2']) && isset($this->argumentos['post']['item_typeid_3']) && isset($this->argumentos['post']['item_typeid_4'])
				&& isset($this->argumentos['post']['item_typeid_5']) && isset($this->argumentos['post']['item_qntd_1']) && isset($this->argumentos['post']['item_qntd_2'])
				&& isset($this->argumentos['post']['item_qntd_3']) && isset($this->argumentos['post']['item_qntd_4']) && isset($this->argumentos['post']['item_qntd_5'])
				&& isset($this->argumentos['post']['msg'])) {	// Comando Registra GM Gift
				
				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && $this->argumentos['post']['uid'] != '' 
						&& is_numeric($this->argumentos['post']['uid']) && strlen($this->argumentos['post']['msg']) <= 500) {

					if ($this->argumentos['post']['item_typeid_1'] == '')
						$this->argumentos['post']['item_typeid_1'] = 0;

					if ($this->argumentos['post']['item_typeid_2'] == '')
						$this->argumentos['post']['item_typeid_2'] = 0;

					if ($this->argumentos['post']['item_typeid_3'] == '')
						$this->argumentos['post']['item_typeid_3'] = 0;

					if ($this->argumentos['post']['item_typeid_4'] == '')
						$this->argumentos['post']['item_typeid_4'] = 0;

					if ($this->argumentos['post']['item_typeid_5'] == '')
						$this->argumentos['post']['item_typeid_5'] = 0;

					if ($this->argumentos['post']['item_qntd_1'] == '')
						$this->argumentos['post']['item_qntd_1'] = 0;

					if ($this->argumentos['post']['item_qntd_2'] == '')
						$this->argumentos['post']['item_qntd_2'] = 0;

					if ($this->argumentos['post']['item_qntd_3'] == '')
						$this->argumentos['post']['item_qntd_3'] = 0;

					if ($this->argumentos['post']['item_qntd_4'] == '')
						$this->argumentos['post']['item_qntd_4'] = 0;

					if ($this->argumentos['post']['item_qntd_5'] == '')
						$this->argumentos['post']['item_qntd_5'] = 0;

					if ($this->argumentos['post']['msg'] == '')
						$this->argumentos['post']['msg'] = null;

					$this->params->clear();
					$this->params->add('i', $_SESSION['player_log']['uid']);	// ADM que resgistrou o Donation
					$this->params->add('i', $this->argumentos['post']['uid']);
					$this->params->add('s', $this->argumentos['post']['msg']);
					$this->params->add('i', $this->argumentos['post']['item_typeid_1']);
					$this->params->add('i', $this->argumentos['post']['item_typeid_2']);
					$this->params->add('i', $this->argumentos['post']['item_typeid_3']);
					$this->params->add('i', $this->argumentos['post']['item_typeid_4']);
					$this->params->add('i', $this->argumentos['post']['item_typeid_5']);
					$this->params->add('i', $this->argumentos['post']['item_qntd_1']);
					$this->params->add('i', $this->argumentos['post']['item_qntd_2']);
					$this->params->add('i', $this->argumentos['post']['item_qntd_3']);
					$this->params->add('i', $this->argumentos['post']['item_qntd_4']);
					$this->params->add('i', $this->argumentos['post']['item_qntd_5']);

					$query = '';
					
					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcRegisterGMGiftWeb ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcRegisterGMGiftWeb(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';
					else
						$query = 'CALL ProcRegisterGMGiftWeb(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0)
						$this->msg = 53;	// Registrou GM Gift com sucesso
				}

			} else if (isset($this->argumentos['post']['shutdown']) && isset($this->argumentos['post']['reserved_date']) && isset($this->argumentos['post']['alvo'])) { // Comando para desligar o server

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && $this->argumentos['post']['alvo'] != '' 
						&& is_numeric($this->argumentos['post']['alvo']) && $this->argumentos['post']['shutdown'] != '' && is_numeric($this->argumentos['post']['shutdown'])) {
					
					$reserved_date = null;
					$shutdown_date = null;

					//date_default_timezone_set('America/Sao_Paulo');

					$shutdown_date = date('Y-m-d H:i:s.u', time() + ($this->argumentos['post']['shutdown'] * 60) + (!empty($this->argumentos['post']['reserved_date']) ? $this->argumentos['post']['reserved_date'] * 60 : 0));

					if ($this->argumentos['post']['reserved_date'] != '' && $this->argumentos['post']['reserved_date'] != 0 && is_numeric($this->argumentos['post']['reserved_date'])) {
						//date_default_timezone_set('America/Sao_Paulo');

						$reserved_date = date('Y-m-d H:i:s.u', time() + ($this->argumentos['post']['reserved_date'] * 60));
					}

					$this->params->clear();
					$this->params->add('s', $shutdown_date);
					$this->params->add('i', $this->argumentos['post']['alvo']);
					$this->params->add('s', $reserved_date);

					$query = '';
					
					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcRegisterShutdownServer ?, ?, ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'select * from '.$this->db->con_dados['DB_NAME'].'.ProcRegisterShutdownServer(?, ?, ?)';
					else
						$query = 'CALL ProcRegisterShutdownServer(?, ?, ?)';

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0)
						$this->msg = 37;	// Enviou o comando para desligar o server com sucesso
				}

			} else if (isset($this->argumentos['post']['add_notice']) && isset($this->argumentos['post']['type']) && isset($this->argumentos['post']['title']) && isset($this->argumentos['post']['body'])) { // Cria Notice

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && $this->argumentos['post']['type'] != '' 
					&& is_numeric($this->argumentos['post']['type']) && $this->argumentos['post']['title'] != '') {

					include_once("notice_singleton.inc");

					if (NoticeSingleton::getInstance()->makeNoticeFull($this->argumentos['post']['type'], 
																   $this->argumentos['post']['title'],
																   $this->argumentos['post']['body'],
																   time()))
						$this->msg = 600;	// Criou Notice com sucesso
					else
						$this->msg = 602;	// Error ao criar o notice
					
				}
			
			} else if (isset($this->argumentos['post']['edit_notice']) && isset($this->argumentos['post']['id']) && isset($this->argumentos['post']['type'])
				&& isset($this->argumentos['post']['title']) && isset($this->argumentos['post']['body'])) {	// Edita Notice

					if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && $this->argumentos['post']['id'] != ''
						&& is_numeric($this->argumentos['post']['id']) && $this->argumentos['post']['type'] != '' && is_numeric($this->argumentos['post']['type']) && $this->argumentos['post']['title'] != '') {

						include_once("notice_singleton.inc");

						if (NoticeSingleton::getInstance()->editNoticeFull($this->argumentos['post']['id'],
																		$this->argumentos['post']['type'],
																		$this->argumentos['post']['title'],
																		$this->argumentos['post']['body'],
																		time()))
							$this->msg = 604;	// Editou Notice com sucesso
						else
							$this->msg = 605;	// Error ao editar o notice

					}

			} else if (isset($this->argumentos['post']['del_notice']) && isset($this->argumentos['post']['id'])) {	// Deleta Notice

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) 
					&& $this->argumentos['post']['id'] != '' && is_numeric($this->argumentos['post']['id'])) {

					include_once("notice_singleton.inc");

					if (NoticeSingleton::getInstance()->deleteNotice($this->argumentos['post']['id']))
						$this->msg = 601;	// Deletou Notice com sucesso
					else
						$this->msg = 603;	// Error ao deletar o notice
				}

			} else if (isset($this->argumentos['post']['term_use']) && isset($this->argumentos['get']['yes'])) {
				
				if ($this->argumentos['get']['yes']) {
					$this->msg = 200;
				}else {
					if (isset($_SESSION['player_cadastro']))
						unset($_SESSION['player_cadastro']);

					if (isset($_SESSION['player_log']['pincode']))
						unset($_SESSION['player_log']['pincode']);

					$this->pincode = '';
				}

			} else if (isset($this->argumentos['post']['logout'])) { // logout
				$this->logado = false;
				$this->id = '';
				$this->key = '';
				$this->level = -1;
				$this->uid = -1;
				$this->nick = 'unknown';
				$this->cap = 0;
				$this->ip_addr = '';
				$this->pincode = '';

				if (isset($_SESSION['player_log']))
					unset($_SESSION['player_log']);
			}
		}
		
		if (isset($this->argumentos['get'])) {

			if (isset($this->argumentos['get']['link']) && $this->argumentos['get']['link'] == 7) {

				if (isset($this->argumentos['get']['id']) && isset($this->argumentos['get']['key'])) { // REQ FORMULARIO FINISH CADASTRO

					include("req_seg_cadastro_exec.inc");

				}else
					$this->msg = 102;

			}else if (isset($this->argumentos['get']['link']) && $this->argumentos['get']['link'] == 9) {	// REQ formulário de trocar a senha ou id do player que pediu para recuperar

				if (isset($this->argumentos['get']['index']) && isset($this->argumentos['get']['key'])) {

					include_once("req_seg_rescue_password_exec.inc");

				}else
					$this->msg = 102;	// Link invalido

			}else if (isset($this->argumentos['get']['adm']) && isset($this->argumentos['get']['kick']) && isset($this->argumentos['get']['alvo'])) { // Kick = uid um player do server = alvo
				
				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8) && $this->argumentos['get']['kick'] != '' 
						&& is_numeric($this->argumentos['get']['kick']) && $this->argumentos['get']['alvo'] != '' && is_numeric($this->argumentos['get']['alvo'])) {
					
					$this->params->clear();
					$this->params->add('i', $this->argumentos['get']['kick']);
					$this->params->add('i', $this->argumentos['get']['alvo']);

					$query = '';

					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcRegisterKickPlayerCommand ?, ?';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcRegisterKickPlayerCommand(?, ?)';
					else
						$query = 'CALL ProcRegisterKickPlayerCommand(?, ?)';

					if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $this->db->db->getLastError() == 0)
						$this->msg = 38;	// Comando para deslogar o player foi enviado com sucesso
				}
			
			}else if (isset($this->argumentos['get']['guild'])) {	// Guild Manager

				if (isset($_SESSION['player_log']) && ($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8)) {

					if (isset($this->argumentos['get']['accept']) && is_numeric($this->argumentos['get']['accept']) && $this->argumentos['get']['accept'] > 0) {

						$this->params->clear();
						$this->params->add('i', $this->argumentos['get']['accept']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGuildAcceptMark ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT "_RET_" as "RET" FROM '.$this->db->con_dados['DB_NAME'].'.ProcGuildAcceptMark(?)';
						else
							$query = 'CALL ProcGuildAcceptMark(?)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0
								&& ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] != '') {

								switch ($row['RET']) {
									case 1:	// Sucesso
										$this->msg = 501;	// Aceito a guild ou a Mark da guild com sucesso
										break;
									case -1:
										$this->msg = 504;	// Não conseguiu aceitar a guild mark
										break;
									case -2:
										$this->msg = 505;	// Não tem uma guild mark nova para poder aceitar
										break;
								}
						
						}else
							$this->msg = 504;	// Não conseguiu aceitar a guild mark

					}else if (isset($this->argumentos['get']['reject']) && is_numeric($this->argumentos['get']['reject']) && $this->argumentos['get']['reject'] > 0) {

						$this->params->clear();
						$this->params->add('i', $this->argumentos['get']['reject']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGuildRejectMark ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT "_RET_" as "RET" FROM '.$this->db->con_dados['DB_NAME'].'.ProcGuildRejectMark(?)';
						else
							$query = 'CALL ProcGuildRejectMark(?)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0
								&& ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] == 1)
							$this->msg = 502;	// Rejeitou a Mark da guild
						else
							$this->msg = 506;	// Não conseguiu rejeitar a guild mark

					}else if (isset($this->argumentos['get']['closure']) && is_numeric($this->argumentos['get']['closure']) && $this->argumentos['get']['closure'] > 0) {

						$this->params->clear();
						$this->params->add('i', $this->argumentos['get']['closure']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGuildClosure ?, 0';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT "_RET_" as "RET" FROM '.$this->db->con_dados['DB_NAME'].'.ProcGuildClosure(?, 0)';
						else
							$query = 'CALL ProcGuildClosure(?, 0)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0
								&& ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] != '') {
							
							switch ($row['RET']) {
								case 1:
									$this->msg = 503;	// Fechou a Guild com sucesso
									break;
								case -1:
									$this->msg = 507;	// Não conseguiu fechar a guild
									break;
								case -2:
									$this->msg = 511;	// A guild já está fechar
									break;
							}
						}else
							$this->msg = 507;	// Não conseguiu fechar a guild
					
					}else if (isset($this->argumentos['get']['reopen']) && is_numeric($this->argumentos['get']['reopen']) && $this->argumentos['get']['reopen'] > 0) {
						
						$this->params->clear();
						$this->params->add('i', $this->argumentos['get']['reopen']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGuildReOpen ?, 0';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT "_RET_" as "RET" FROM '.$this->db->con_dados['DB_NAME'].'.ProcGuildReOpen(?, 0)';
						else
							$query = 'CALL ProcGuildReOpen(?, 0)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0
								&& ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] != '') {

							switch ($row['RET']) {
								case 1:
									$this->msg = 508;	// Reativou a guild com sucesso
									break;
								case -1:
									$this->msg = 509;	// Não conseguiu reativar a guild
									break;
								case -2:
									$this->msg = 510;	// A guild não está fechada para reabrir ela
									break;
							}
						}else
							$this->msg = 509;	// Não conseguiu reativar a guild
						
					}else if (isset($this->argumentos['get']['block']) && is_numeric($this->argumentos['get']['block']) && $this->argumentos['get']['block'] > 0) {

						$this->params->clear();
						$this->params->add('i', $this->argumentos['get']['block']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGuildBlock ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT "_RET_" as "RET" FROM '.$this->db->con_dados['DB_NAME'].'.ProcGuildBlock(?)';
						else
							$query = 'CALL ProcGuildBlock(?)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0
								&& ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] != '') {

							switch ($row['RET']) {
								case 1:
									$this->msg = 512;	// Bloqueou a guild com sucesso
									break;
								case -1:
									$this->msg = 513;	// Não conseguiu bloquear a guild
									break;
								case -2:
									$this->msg = 514;	// A guild já está bloqueada
									break;
							}

						}else
							$this->msg = 513;	// Não conseguiu bloquear a guild

					}else if (isset($this->argumentos['get']['unblock']) && is_numeric($this->argumentos['get']['unblock']) && $this->argumentos['get']['unblock'] > 0) {

						$this->params->clear();
						$this->params->add('i', $this->argumentos['get']['unblock']);

						$query = '';

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGuildUnblock ?';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT "_RET_" as "RET" FROM '.$this->db->con_dados['DB_NAME'].'.ProcGuildUnblock(?)';
						else
							$query = 'CALL ProcGuildUnblock(?)';

						if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0
							&& ($row = $result->fetch_assoc()) != null && isset($row['RET']) && $row['RET'] != '') {

							switch ($row['RET']) {
								case 1:
									$this->msg = 515;	// Desbloqueou a guild com sucesso
									break;
								case -1:
									$this->msg = 516;	// Não conseguiu desbloquear a guild
									break;
								case -2:
									$this->msg = 517;	// A guild não está bloqueada pra desbloquear
									break;
							}

						}else
							$this->msg = 516;	// Não conseguiu desbloquear a guild
					}
				}

			}
		}
	}

	private function init_session_log() {
		
		if (isset($_SESSION['player_log'])) {
			
			$this->logado = true;

			$this->id = $_SESSION['player_log']['id'];
			$this->key = $_SESSION['player_log']['key'];
			$this->uid = $_SESSION['player_log']['uid'];
			$this->ip_addr = $_SESSION['player_log']['ip'];
			$this->pincode = (isset($_SESSION['player_log']['pincode']) ? $_SESSION['player_log']['pincode'] : '');

			// Atualiza info do player acada atualização de página ou troca de página
			$this->updateInfoPlayer();
		}
	}

	private function updateInfoPlayer() {

		if (isset($_SESSION['player_log']) && $this->logado) {
			
			$this->params->clear();
			$this->params->add('i', $this->uid);

			$query = '';

			if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
				$query = 'SELECT a.nick, a.capability as cap, b.[level], b.cookie, b.pang FROM '.$this->db->con_dados['DB_NAME'].'.account a, '.$this->db->con_dados['DB_NAME'].'.user_info b WHERE a.[UID] = b.[UID] AND a.[UID] = ?';
			else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
				$query = 'SELECT a.nick, a.capability as cap, b.level, b.cookie, b.pang FROM '.$this->db->con_dados['DB_NAME'].'.account a, '.$this->db->con_dados['DB_NAME'].'.user_info b WHERE a.UID = b.UID AND a.UID = ?';
			else
				$query = 'SELECT a.nick, a.capability as cap, b.`level`, b.cookie, b.pang FROM account a, user_info b WHERE a.UID = b.UID AND a.UID = ?';

			if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0
				&& ($row = $result->fetch_assoc()) && isset($row['cookie']) && isset($row['pang']) 
				&& isset($row['nick']) && isset($row['cap']) && isset($row['level'])) {

				$this->level = $_SESSION['player_log']['level'] = $row['level'];
				$this->cap = $_SESSION['player_log']['cap'] = $row['cap'];
				$this->nick = $_SESSION['player_log']['nick'] = mb_convert_encoding($row['nick'], 'UTF-8', 'SJIS');
				$this->moeda['cookie'] = $_SESSION['player_log']['cookie'] = $row['cookie'];
				$this->moeda['pang'] = $_SESSION['player_log']['pang'] = $row['pang'];

			}else {

				$this->level = $_SESSION['player_log']['level'] = -1;
				$this->cap = $_SESSION['player_log']['cap'] = 0;
				$this->nick = $_SESSION['player_log']['nick'] = 'unknown';
				$this->moeda['cookie'] = $_SESSION['player_log']['cookie'] = 0;
				$this->moeda['pang'] = $_SESSION['player_log']['pang'] = 0;
			}
		
		}else {

			$this->level = -1;
			$this->cap = 0;
			$this->nick = 'unknown';
			$this->moeda['cookie'] = 0;
			$this->moeda['pang'] = 0;
		}
	}

	private function init_session_cadastro() {
		if (isset($_SESSION['player_cadastro']) && isset($_SESSION['player_cadastro']['pincode']))
			$this->pincode = $_SESSION['player_cadastro']['pincode'];
	}

	private function init_session_bug_report() {
		if (isset($_SESSION['bug_report']))
			if (isset($_SESSION['bug_report']['bug_report_ta']))
				$this->bug_ta = $_SESSION['bug_report']['bug_report_ta'];
	}

	private function init_session_rescue() {
		//if (isset($_SESSION['player']))
	}

	public function show() {
		// Mostra a pagina
		$this->showHead();
		$this->showConteudo();
		$this->showFooter();
	}

	public function showHead() {
		echo '<!DOCTYPE html>
			<html lang="pt-BR">
				<head>
					<meta charset="UTF-8" />
					<meta name="robots" content="index, follow">
					<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
					<link rel="icon" href="/favicon.ico" type="image/x-icon">
					<meta name="viewport" content="width=device-width, initial-scale=1.0"> 
					<title>PangYa SuperSS</title>';

		include_once('recaptcha.inc');

		echo reCAPTCHA::loadScript_V3();

		$this->style_principal();

		echo '</head>
				<body>';
	}

	public function style_principal() {
		echo '<style>';

		echo '* {
				border: 0;
				padding: 0;
				margin: 0;
			}
			li, a {
				list-style: none;
				display: inline-block;
				text-decoration: none;
				color: black;
			}
			html, body {
				width: 100%;
				background: white;
				font-family: arial;
				font-size: 100%;
			}
			#logo {
				width: 100%;
				height: 350px;
				margin: 0 auto;
				background: url("../img/logo.png") no-repeat;
				background-size: 25% 70%;
				background-position: center;
			}
			#status {
				margin: 4% 0 0 0;
				background: #a4d4e6;
				border-radius: 5px;
			}
			#status span {
				/*display: block-inline;*/
				display: block !important;
				background: #EFEEEC;
				border: 1px solid #B1C4CB;
				border-radius: 5px;
			}
			#onst {
				width: 100%;
				background: linear-gradient(#878787, #FFF) !important;
				/*border: 1px solid black !important;*/
				font-weight: bold;
				text-align: center;
				color: black;
				border-radius: 5px;
			}
			#offst {
				width: 100%;
				background: linear-gradient(#FFF, #878787) !important;
				/*border: 1px solid black !important;*/
				font-weight: bold;
				text-align: center;
				color: black;
				border-radius: 5px;
			}
			#menu {
				/*margin: 2% 0% 5% 8%;*/
				display: block;
				width: 80%;
				margin: 0 auto;
				background-color: #F3F3F3;
				border-radius: 5px;
				/*border: 1px solid #DDD;*/
			}
			.itemlista {
				width: 100%;
				/*border-bottom: 1px solid #DDD;*/
			}
			#head {
				text-align: center;
				display: block;
				background: url("../img/fundohead.png") no-repeat;
				background-size: 100%;
				background-position: center;
				padding: 0.5%;
			}
			#lista > * {
				display: inline-block;
				margin: 1% 3%;
			}
			#lista {
				text-align: center;
				display: block;
				border-radius: 5px;
				background: url("../img/fundohead.png") no-repeat;
				background-size: 100%;
				background-position: center;
				align-items: center;
			}
			#conteudo {
				margin: 5% 10% 5% 1%;
				width: 100%;
				background-color: #F3F3F3;
				border-radius: 5px;
				border: 1px solid #DDD;
				text-align: center;
			}
			article {
				margin: 5% 3%;
				text-align: center;
			}
			#plogin {
				text-align: left;
			}
			#pcadastro {
				text-align: left;
			}
			h3 {
				text-align: center;
			}
			.ipt_cx {
				padding: 0.5%;
				margin-bottom: 1%;
				border: 1px solid #DDD;
			}
			.ipt_btn {
				margin-top: 5%;
				margin-left: 40%;
				padding: 0.8% 2%;
				border-radius: 3px;
				background: linear-gradient(lightblue, #9999EE);
				border: 1px solid #9999DD;
			}
			#rodape {
				text-align: center;
				padding: 5%;
				border-top: 1px solid #ddd;
				background: #CCC;
				clear: both;
			}

			#lgd {
				width: 20%;
				margin: 5% 0 5% 10%;
				overflow: hidden;
			}

			#fo {
				display: flex;
				width: 100%;
				/*background: url("../img/fundohead.png") no-repeat;
				background-position: center;
				background-size: 100%;*/
				border-radius: : 2px;
			}

			#lgd input {
				width: 100%;
				border: 1px solid black;
			}

			#inputd {
				width: 200%;
			}

			#logar {
				margin: 0 0 0 2%;
				width: 100%;
				background: linear-gradient(white, #878787);
				border-radius: 3px;
			}

			#cc1 {
				margin: 5% auto;
				display: flex;
			}

			#aaa1 {
				width: 90%;
				margin: 2% 0 0 2%;
				/*padding: 1%;*/
				display: inline-flex !important;
				font-size: 80%
			}
			.lab1 {
				width: 300%;
			}
			i {
				justify-self: right;
				float: right;
				margin: 1% 0.5% !important;
			}';

		echo '</style>';
	}

	public function showFooter() {
		echo '<footer id="rodape">
						<p>2019 Acrisio SuperSS Dev</p>
						<p>All other trademarks are property of NTREEV SOFT and their respective owners.</p>
					</footer>
				</body>
			</html>';
	}

	public function showLogo() {
		echo '<div id="logo"></div>';
	}

	public function showMenu() {
		echo '<nav id="menu">
					<ul id="lista">
						<li><a href="pangya.php?link=1" class="itemlista">HOME</a></li>';
		if (!$this->logado)
			echo '<li><a href="pangya.php?link=3" class="itemlista">CADASTRO</a></li>';
		else {
			echo '<li><a href="pangya.php?link=2" class="itemlista">DOWNLOAD CLIENT</a>';
		}

		echo '<li><a href="pangya.php?link=5" class="itemlista">FAQ</a></li>';
		echo '<li><a href="pangya.php?link=8" class="itemlista">DOAR</a></li>';

		// icon social links
		echo '<li><a href="https://discord.gg/mFcVvY6" class="itemlista">BUG REPORT</a></li>
				<i><a href="https://www.youtube.com/c/SuperSSDev/"><img src="img/iyoutube.png" alt="icon Youtube" width="30px" hight="30px"></a></i>
				<i><a href="https://www.facebook.com/Acrisio-Superss-Dev-571172483058181/"><img src="img/iface.png" alt="icon Facebook" width="30px" hight="30px"></a></i>
				<i><a href="https://discord.gg/fVKD8zD"><img src="img/idiscord.png" alt="icon Discord" width="30px" hight="30px"></a></i>
					</ul>
				</nav>';
	}

	public function showConteudo() {
		$this->showLogo();
		$this->showMenu();

		echo '<div id="cc1">';

		echo '<div id="lgd">';

		$this->login_view();

		$this->server_status_view();

		echo '</div>';

		echo '<section id="conteudo">';

		// aqui conteudo
		if (isset($this->argumentos['get'])) {

			if (isset($this->argumentos['get']['link'])) {
				switch ($this->argumentos['get']['link']) {
					case '1':	// home
					default:
						$this->home_view();
						break;
					case '2':	// Download cliente
						$this->downlod_view();
						break;
					case '3': 	// Cadastro
						$this->cadastro_view();
						break;
					case '4': 	// Bug Report
						$this->report_view();
						break;
					case '5': 	// FAQ
						$this->faq_view();
						break;
					case '7':	// Finalizar cadastro
						$this->finish_cadastro_view();
						break;
					case '8':	// Doar
						$this->doar();
						break;
					case '9':	// Recuperar senha ou id
						$this->finish_rescue_password_view();
						break;
					case '10':	// Mostra as doações do player
						$this->my_donation_view();
						break;
				}
			} else if (isset($this->argumentos['get']['adm']))
				$this->adm_view();
			else if (isset($this->argumentos['get']['guild']))
				$this->guild_manager_view();
			else if (isset($this->argumentos['get']['notice_manager']))
				$this->notice_manager_view();
			else if (isset($this->argumentos['get']['bug_report_view']))
				$this->bug_report_view();
			else if (isset($this->argumentos['get']['pincode_view']))
				$this->pincode_view();
			else if (isset($this->argumentos['get']['changeip']))
				$this->home_view();
			else
				$this->home_view();
		} else
			$this->home_view();

		echo '</section></div>';
	}

	private function downlod_view() {
		if (!$this->logado)
			echo "<script>window.addEventListener('load', function() {
				alert('Por favor logue primeiro.');
				window.open('pangya.php', '_self');
			});</script>";
		else {
			echo '<style>
					#div_down {
						margin: 2% auto;
					}
					#div_down > * {
						text-align: left;
					}
			     </style>';

			echo '<article id="home">';

			echo '<h3>Download do cliente</h3>';

			echo '<div id="div_down">';

			echo 'Se você não tiver o cliente do Pangya SuperSS baixe ele aqui:<br><br>
					Único arquivo de instação:
					<a target="_blank" href="https://drive.google.com/file/d/1NPbhW0n6S8kJ0NXMRulSRyU3dkKTh4Y0/view?usp=sharing" style="text-decoration: underline; color: blue">link</a>.<br><br>
					Ou baixe ele em partes:
					<a target="_blank" href="https://drive.google.com/drive/folders/1rjFu1FUNPz1htNKD4fMb4WumCkLE6BZo?usp=sharing" style="text-decoration: underline; color: blue">aqui</a>. 
					(Baixe todos os 6 arquivos da pasta do google drive)<br><br><br>';

			echo 'Baixe <a target="_blank" href="https://aka.ms/vs/15/release/VC_redist.x86.exe" style="text-decoration: underline; color: blue">aqui</a>,'; 
			echo ' o Microsoft Visual C++ Redistributable 2017 versão x86 independente da versão do seu sistema operacional. E instale.<br><br>';

			echo 'Execute o update.exe na pasta do PangYa SuperSS, para atualizar o jogo e entrar nele.<br>';
			echo '</div>';

			echo '</article>';
		}
	}

	private function pincode_view() {
		if (!$this->logado)
			echo "<script>window.addEventListener('load', function() {
				alert('Por favor logue primeiro.');
				window.open('pangya.php', '_self');
			});</script>";
		else if (($this->cap & 4) && ($this->cap & 8) /*&& ($this->ip_addr == '127.0.0.1' || $this->ip_addr == '192.168.0.101')*/) { // ADM
			echo '<article id="home">';

			echo '<style>
					#div_principal > * {
						margin: 2% auto;
					}
					.div_interna > * {
						margin: 0.5% auto;
					}
					.used {
						background-color: red;
						color: white;
					}
					.avaliable {
						background-color: green;
						color: white;
					}
				  </style>';

			echo '<h3>Pin Code Viewer</h3>';

			echo '<div id="div_principal" style="text-align: left;">'; // div principal

			if (file_exists('config/keys.txt')) {
				$keys = unserialize(file_get_contents('config/keys.txt'));

				if (isset($keys) && is_array($keys) && count($keys) > 0) {

					for ($i = 0; $i < count($keys); $i++) {
						echo '<div class="div_interna">';

						echo '<h4>Pin code N#'.($i+1).': '.$keys[$i]['unique_id'].' - <span class="'.(($keys[$i]['flag']) ? 'used"> Usado' : 'avaliable"> Disponível').'</span></h4>';

						echo '</div>';
					}
				}
			}

			echo '</div>'; // fecha div principal

			echo '</article>';

		} else {

			// Log
			error_log("Player[UID=".$this->uid.", NICKNAME=".$this->nick.", IP=".$this->ip_addr.", CAPABILITY=".$this->cap."] tentou acessa a pagina pincode_view.", 0);

			echo "<script>window.addEventListener('load', function() {
				window.open('pangya.php', '_self');
			});</script>";
		}
	}

	private function bug_report_view() {
		if (!$this->logado)
			echo "<script>window.addEventListener('load', function() {
				alert('Por favor logue primeiro.');
				window.open('pangya.php', '_self');
			});</script>";
		else if (($this->cap & 4) && ($this->cap & 8) /*&& ($this->ip_addr == '127.0.0.1' || $this->ip_addr == '192.168.0.101')*/) { // ADM
			echo '<article id="home">';

			echo '<style>
					#div_principal > * {
						margin: 2% auto;
					}
					.div_interna > * {
						margin: 0.5% auto;
					}
				  </style>';

			echo '<h3>Bug Report Viewer</h3>';

			echo '<div id="div_principal">'; // div principal

			if (file_exists('config/bug_report.txt')) {
				$bug_report = unserialize(file_get_contents('config/bug_report.txt'));

				if (isset($bug_report) && is_array($bug_report) && count($bug_report) > 0) {
					
					for ($i = 0; $i < count($bug_report); $i++) {
						echo '<div class="div_interna">';	

						echo '<h4>UID: '.$bug_report[$i]['uid'].' ID: '.$bug_report[$i]['id'].'</h4>';
						echo '<textarea name="broadcast" rows="10" cols="100" width="100%" maxlength="1024" readonly style="resize: none">'.$bug_report[$i]['bug_report'].'</textarea>';

						echo '</div>';
					}
				}
			}

			echo '</div>'; // fecha div principal

			echo '</article>';

		} else {

			// Log
			error_log("Player[UID=".$this->uid.", NICKNAME=".$this->nick.", IP=".$this->ip_addr.", CAPABILITY=".$this->cap."] tentou acessa a pagina bug_report_view.", 0);

			echo "<script>window.addEventListener('load', function() {
				window.open('pangya.php', '_self');
			});</script>";
		}
	}

	private function adm_view() {

		if (!$this->logado)
			echo "<script>window.addEventListener('load', function() {
				alert('Por favor logue primeiro.');
				window.open('pangya.php', '_self');
			});</script>";
		else if (($this->cap & 4) && ($this->cap & 8) /*&& ($this->ip_addr == '127.0.0.1' || $this->ip_addr == '192.168.0.101')*/) { // ADM

			if ($this->Error == 0) {
				$str_msg = '';

				if ($this->msg != 0) {
				  	switch ($this->msg) {
				  		case 35: // Enviou o comando para trocar o rate do server com sucesso
				  			$str_msg = 'Enviou o comando para trocar o rate do servidor(s) com sucesso';
				  			break;
				  		case 36: // Enviou o comando de broadcast com sucesso
				  			$str_msg = 'Enviou o comando de broadcast com sucesso';
				  			break;
				  		case 37: // Enviou o comando para desligar o server com sucesso
				  			$str_msg = 'Enviou o comando para desligar o server com sucesso';
				  			break;
				  		case 38: // Enviou o comando para deslogar o player com sucesso
				  			$str_msg = 'Enviou o comando para deslogar o player com sucesso';
							break;
						case 39:	// Registrou Doação com sucesso
							$str_msg = 'Registou Doação com sucesso';
							break;
						case 40:	// Enviou o comando para recarregar sistema do server com sucesso
							$str_msg = "Enviu o comando para recarregar sistema do server com sucesso";
							break;
						case 41:	// Inseriu IP no BLOCK IP TABLE com sucesso
							$str_msg = "Inseriu IP no BLOCK IP TABLE com sucesso";
							break;
						case 42:	// Excluiu IP do BLOCK IP TABLE com sucesso
							$str_msg = "Excluiu IP do BLOCK IP TABLE com sucesso";
							break;
						case 43:	// IP invalid
							$str_msg = "Inserir IP no BLOCK IP TABLE mas o IP é inválido";
							break;
						case 44:	// MASK invalid
							$str_msg = "Inserir IP no BLOCK IP TABLE mas o MASK é inválido";
							break;
						case 45:	// Não conseguiu excluir o IP do BLOCK IP TABLE
							$str_msg = "Não conseguiu excluir o IP do BLOCK IP TABLE";
							break;
						case 46:	// Não conseguiu inserir o IP no BLOCK IP TABLE
							$str_msg = "Não conseguiu inserir o IP no BLOCK IP TABLE";
							break;
						case 47:	// Erro na verificação dos dados fornecidos
							$str_msg = "Erro na verificação dos dados fornecidos";
							break;
						case 48:
							$str_msg = "Inseriu MAC Address BLOCK MAC TABLE com sucesso";
							break;
						case 49:
							$str_msg = "Excluiu MAC Address BLOCK MAC TABLE com sucesso";
							break;
						case 50:
							$str_msg = "Inserir MAC Address no BLOCK MAC TABLE mas o MAC Address é inválido";
							break;
						case 51:
							$str_msg = "Não conseguiu Inserir MAC Address no BLOCK MAC TABLE";
							break;
						case 52:
							$str_msg = "Não conseguiu excluir o MAC Address do BLOCK MAC TABLE";
							break;
						case 53:	// Registrou GM Gift com sucesso
							$str_msg = "Registrou GM Gift com sucesso.";
							break;
						case 1000: // Falha ao executar query de consulta
						case 1001: // Não tem nenhum donation com o code e plataforma fornecido
						case 1002: // Falha na query de consulta
							$str_msg = "Falha ao atualizar doação";
						break;
						case 1003: 	// Falho ao executar a proc de atualização da doação
						case 1004:	// Falho dentro da proc de atualização da doação
						case 1005:	// Falho dentro da verificação da proc de atualização da doação
							$str_msg = "Falha ao atualizar doação.";
						break;
						case 1006: // Erro desconhecido
							$str_msg = "Erro desconhecido";
							break;
						case 1010:	// Falha ao executar a proc de registro de doação
						case 1011:	// Falha detro da proc de registro de doação
						case 1012:	// Falha dentro da verificação da proc de registro de doação
						case 1013:	// Falha ao executar a proc de update de doação
						case 1014:	// Falha dentro da proc de update de doação
						case 1015:	// Falha dentro da verificação da proc de update da doação
							$str_msg = "Falha ao registrar a doação";
						break;
						case 1016: 	// Erro desconhecido
							$str_msg = "Erro desconhecido";
						break;
						case 1020:	// Falha ao executar a proc de deletação de doação
						case 1021:	// Falha dentro da proc de deletação de doação
						case 1022:	// Falha dentro da verificação da proc de deletação de doação
							$str_msg = "Falha ao deletar a doação";
						break;
						case 1023:	// Id da docção inválida
							$str_msg = "Id da docção inválida";
						break;
						case 1024: 	// Erro desconhecido
							$str_msg = "Erro desconhecido";
						break;
						case 1100:	// Doação atualizada com sucesso
							$str_msg = "Doação atualizada com sucesso.";
						break;
						case 1101:	// Doação registrada com sucesso
							$str_msg = "Doação registrada com sucesso";
						break;
						case 1102:	// Doação deletada com sucesso
							$str_msg = "Doação deletada com sucesso";
						break;
				  		default:
				  			$str_msg = 'Erro desconhecido';
				  	} 

					echo "<script>alert('".$str_msg."');</script>";
				}

				echo '<article id="home">';

				echo '<h3>Administração</h3>';

				echo "<script>
						function tab(nome, conteudo) {
							this.nome = nome;
							this.conteudo = conteudo;
						}

						var arr = [];
						arr.push(new tab('player', 'player-c'));
						arr.push(new tab('server', 'server-c'));
						arr.push(new tab('log', 'log-c'));

						function altertab(nome, conteudo) {
							var c = undefined, m = undefined, i = 0;

							for (i = 0; i < arr.length; i++) {
								if (m = document.getElementById(arr[i].nome))
									m.className = 'menu';
								
								if (arr[i].conteudo != 'all') {
									if (c = document.getElementById(arr[i].conteudo))
										c.style.display = 'none';
								}
							}

							if (m = document.getElementById(nome))
								m.className = 'menu-sel';

							if (conteudo == 'all') {
								for (i = 0; i < arr.length; i++) {
									if (arr[i].conteudo != 'all') {
										if (c = document.getElementById(arr[i].conteudo))
											c.style.display = '';
									}
								}
							}else {
								if (c = document.getElementById(conteudo))
									c.style.display = '';
							}
						}
					  </script>";

				echo '<style>
						#tab_aba td {
							border: 1px solid black;
						}
						.menu {
							background-color: white;
							text-align: center;
							cursor: pointer;
						}
						.menu:hover {
							background-color: #777;
						}
						.menu-sel {
							background-color: #555;
							border: 1px solid black;
							border-top: 2px solid black;
							border-bottom: none;
							color: white;
							text-align: center;
						}
						.conteudo-div {
							border: 2px solid black;
							border-radius: 5px;
						}
					  </style>';

				// Span de ON e OFF
				$span = ['<div id="offst">OFF</div>', '<div id="onst">ON</div>'];
				$aba = (isset($_GET['aba']) ? $_GET['aba']: 0);

				// Abas
				echo '<table width="100%" style="border: 0" id="tab_aba">
						<tr>
							<td class="'.(($aba == 0) ? 'menu-sel': 'menu').'" id="player" onclick="altertab(\'player\', \'player-c\')">Players</td>
							<td class="'.(($aba == 1) ? 'menu-sel': 'menu').'" id="server" onclick="altertab(\'server\', \'server-c\')">Servers</td>
							<td class="'.(($aba == 2) ? 'menu-sel': 'menu').'" id="log" onclick="altertab(\'log\', \'log-c\')">Logs</td>
						</tr>
					  </table>';

				if ($aba == 0)
					echo '<div id="player-c" class="conteudo-div">';
				else
					echo '<div id="player-c" class="conteudo-div" style="display: none">';

					// Conteudo do div player
					$query = '';

					echo '<style>
							table {
								border: 1px solid black;
								margin: 2% auto;
							}
							td {
								border: 1px solid black;
							}
							.bt_p {
								border: 1px solid black;
							}
						 </style>';

					echo '<br><div height="100%" width="100%">
									<button class="bt_p" onclick="window.open(\'?adm=1&players_cad_beta=1\', \'_self\')">Cadastro Beta</button>
									<button class="bt_p" onclick="window.open(\'?adm=1&players_unfirst_login=1\', \'_self\')">All Unfirst Login</buton>
									<button class="bt_p" onclick="window.open(\'?adm=1\', \'_self\')">All Players</button>
									<button class="bt_p" onclick="window.open(\'?adm=1&players_ban=1\', \'_self\')">Players Ban</button>
									<button class="bt_p" onclick="window.open(\'?adm=1&players_on=1\', \'_self\')">Only Player ON</button>
								  </div>';

					if (isset($_GET['players_on'])) {
						
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcGetInfoPlayerToWeb 2';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'select "_uid_" as "uid", "_id_" as "id", "_nick_" as "nick", "_level_" as "level", "_idstate_" as "idstate", "_cap_" as "cap", "_pang_" as "pang", "_cookie_" as "cookie", "_channel_" as "channel", "_lobby_" as "lobby", "_room_" as "room", "_place_" as "place", "_logonCount_" as "logonCount", "_counter_" as "counter", "_userip_" as "userip", "_LastLogonTime_" as "LastLogonTime", "_logon_" as "logon", "_game_server_id_" as "game_server_id", "_name_" as "name" from '.$this->db->con_dados['DB_NAME'].'.ProcGetInfoPlayerToWeb(2)';
						else
							$query = 'call ProcGetInfoPlayerToWeb(2)';
					
					}else if (isset($_GET['players_cad_beta'])) {

						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT a.uid, a.nome, a.sobre_nome, a.email, a.key_uniq, a.finish_reg, a.ip_register, a.date_reg FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta a';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT a.uid, a.nome, a.sobre_nome, a.email, a.key_uniq, a.finish_reg, a.ip_register, a.date_reg FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta a';
						else
							$query = 'SELECT a.uid, a.nome, a.sobre_nome, a.email, a.key_uniq, a.finish_reg, a.ip_register, a.date_reg FROM '.$this->db->con_dados['DB_NAME'].'.contas_beta a';

					}else if (isset($_GET['players_unfirst_login'])) {
						
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcGetInfoPlayerToWeb 1';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'select "_uid_" as "uid", "_id_" as "id", "_nick_" as "nick", "_level_" as "level", "_idstate_" as "idstate", "_cap_" as "cap", "_pang_" as "pang", "_cookie_" as "cookie", "_channel_" as "channel", "_lobby_" as "lobby", "_room_" as "room", "_place_" as "place", "_logonCount_" as "logonCount", "_counter_" as "counter", "_userip_" as "userip", "_LastLogonTime_" as "LastLogonTime", "_logon_" as "logon", "_game_server_id_" as "game_server_id", "_name_" as "name" from '.$this->db->con_dados['DB_NAME'].'.ProcGetInfoPlayerToWeb(1)';
						else
							$query = 'call ProcGetInfoPlayerToWeb(1)';
					
					}else if (isset($_GET['players_ban'])) {
					
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcGetInfoPlayerToWeb 4';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'select "_uid_" as "uid", "_id_" as "id", "_nick_" as "nick", "_level_" as "level", "_idstate_" as "idstate", "_cap_" as "cap", "_pang_" as "pang", "_cookie_" as "cookie", "_channel_" as "channel", "_lobby_" as "lobby", "_room_" as "room", "_place_" as "place", "_logonCount_" as "logonCount", "_counter_" as "counter", "_userip_" as "userip", "_LastLogonTime_" as "LastLogonTime", "_logon_" as "logon", "_game_server_id_" as "game_server_id", "_name_" as "name" from '.$this->db->con_dados['DB_NAME'].'.ProcGetInfoPlayerToWeb(4)';
						else
							$query = 'call ProcGetInfoPlayerToWeb(4)';
							
					}else {
						
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcGetInfoPlayerToWeb 0';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'select "_uid_" as "uid", "_id_" as "id", "_nick_" as "nick", "_level_" as "level", "_idstate_" as "idstate", "_cap_" as "cap", "_pang_" as "pang", "_cookie_" as "cookie", "_channel_" as "channel", "_lobby_" as "lobby", "_room_" as "room", "_place_" as "place", "_logonCount_" as "logonCount", "_counter_" as "counter", "_userip_" as "userip", "_LastLogonTime_" as "LastLogonTime", "_logon_" as "logon", "_game_server_id_" as "game_server_id", "_name_" as "name" from '.$this->db->con_dados['DB_NAME'].'.ProcGetInfoPlayerToWeb(0)';
						else
							$query = 'call ProcGetInfoPlayerToWeb(0)';
					}

					if (($result = $this->db->db->execPreparedStmt($query, NULL, 1)) != null && $this->db->db->getLastError() == 0) {
						
						$player_dd = [];
						$rows_count = 0;

						while ($r = $result->fetch_assoc()) {

							$player_dd[$rows_count] = [];

							foreach ($r as $k => $v)
								$player_dd[$rows_count][$k] = $v;

							$rows_count++;
						}

						if ($rows_count > 0) {

							echo "<script>
									function desloga(uid, alvo, only_on) {
										if (only_on)
											window.open('pangya.php?adm=1&players_on=1&kick=' + uid + '&alvo=' + alvo, '_self');
										else
											window.open('pangya.php?adm=1&kick=' + uid + '&alvo=' + alvo, '_self');
									}

									function edit(uid) {
										window.open('adm_edit_player.php?uid=' + uid, 'Editar', 'width:400, height=600');
									}
								</script>";

							echo '<br>Players '.(isset($_GET['players_on']) ? 'online' : '').': '.count($player_dd);

							echo '<table width="98%" cellspacing="0">';

							echo '<tr>';
								
							if (isset($_GET['players_cad_beta'])) {

								echo '	<td>UID</td>
										<td>Nome</td>
										<td>Sobre nome</td>
										<td>Email</td>
										<td>Chave cadastro</td>
										<td>Status</td>
										<td>IP</td>
										<td>Data</td>';

							}else {
							
								echo'<td>UID</td>
									<td>ID</td>
									<td>Nick</td>
									<td>Level</td>
									<td>ID State</td>
									<td>Capcidade</td>
									<td>Moeda</td>
									<td>Location</td>
									<td>Count Logon / Daily</td>
									<td>Last IP</td>
									<td>Last Logon Time</td>
									<td>Status</td>
									<td>Server ID</td>
									<td>Server Name</td>
									<td>ação</td>';
							}

							echo '</tr>';

							foreach ($player_dd as $row) {
								echo '<tr>';

								foreach ($row as $key => $value) {
									if ($key == 'logon')
										echo '<td>'.$span[$value].'</td>';
									else if ($key == 'level')
										printf('<td><img src="../img/level/level_%03d.png"></td>', (($value < 0) ? 000 : ($value+1)));
									else if ($key == 'nick')
										echo '<td>'.htmlspecialchars(mb_convert_encoding($value, 'UTF-8', 'SJIS')).'</td>';
									else if ($key == 'channel' || $key == 'lobby' || $key == 'room' || $key == 'place') {
										
										switch ($key) {
											case 'channel':
												echo '<td>C: '.$value;
												break;
											case 'lobby':
												echo ' L: '.$value;
												break;
											case 'room':
												echo ' R: '.$value;
												break;
											case 'place':
												echo ' P: '.$value.'</td>';
												break;
										}
										
									}else if ($key == 'pang' || $key == 'cookie') {

										if ($key == 'pang')
											echo '<td>PP: '.$value;
										else if ($key == 'cookie')
											echo ', CP: '.$value.'</td>';

									}else if ($key == 'logonCount' || $key == 'counter') {

										if ($key == 'logonCount')
											echo '<td>LC: '.$value;
										else if ($key == 'counter')
											echo ', CD: '.$value.'</td>';

									}else
										echo '<td>'.$value.'</td>';
								}

								if (!isset($_GET['players_cad_beta'])) {
									echo '<td>';

									if ($row['logon'])
										echo '<button onclick="desloga('.$row['uid'].', '.$row['game_server_id'].', '.((isset($_GET['players_on'])) ? 'true' : 'false').')">Deslogar</button> / ';
											
									echo '	  <button onclick="edit('.$row['uid'].')">Editar</button>
										</td>';
								
								}

								echo '</tr>';
							}

							echo '</table>';
						
						}else
							echo '<br> Players '.(isset($_GET['players_on']) ? 'online' : '').': '.$rows_count.'<br><br>';
					
					}else
						echo '<br> Error na consulta. Code: '.$this->db->db->getLastError().'<br><br>';

				echo '</div>';

				if ($aba == 1)
					echo '<div id="server-c" class="conteudo-div">';
				else
					echo '<div id="server-c" class="conteudo-div" style="display: none">';

				// Conteudo do div server
				$server_st = null;

				$query = '';
				
				if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'SELECT a.[uid], a.[type], a.[name], CASE WHEN a.UpdateTime > DATEADD(SECOND, -12, getdate()) THEN 1 ELSE 0 END as [status], a.currUser, ISNULL(b.pangrate, a.pangrate) as PangRate, ISNULL(b.exprate, a.exprate) as ExpRate, ISNULL(b.clubmasteryrate, a.masteryrate) as ClubMastery, ISNULL(b.treasurerate, a.treasurerate) as TreasureRate, ISNULL(b.chuvarate, a.chuvarate) as ChuvaRate, ISNULL(b.scratchyporpointrate, a.scratchrate) as ScratchyRate, ISNULL(b.papelshoprareitemrate, a.rareitemrate) as PapelShopRareItemRate, ISNULL(b.papelshopcookieitemrate, a.cookieitemrate) as PapelShopCookieItemRate, ISNULL(b.memorialshoprate, 0) as MemorialShopRate, ISNULL(b.grandzodiaceventtime, 0) as GrandZodiacEventTime, ISNULL(b.AngelEvent, 0) as AngelEvent, ISNULL(b.GrandPrixEvent, 0) as GrandPrixEvent, ISNULL(b.GoldenTimeEvent, 0) as GoldenTimeEvent, ISNULL(b.LoginRewardEvent, 0) as LoginRewardEvent, ISNULL(b.BotGMEvent, 0) as BotGMEvent, ISNULL(b.SmartCalculator, 0) as SmartCalculator FROM '.$this->db->con_dados['DB_NAME'].'.pangya_server_list a LEFT OUTER JOIN '.$this->db->con_dados['DB_NAME'].'.pangya_config b ON a.[uid] = b.[uid] ORDER BY [type]';
				else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = "SELECT a.uid, a.type, a.name, CASE WHEN a.UpdateTime > (now() + '-12 second'::interval) THEN 1 ELSE 0 END as status, a.currUser, COALESCE(b.pangrate, a.pangrate) as PangRate, COALESCE(b.exprate, a.exprate) as ExpRate, COALESCE(b.clubmasteryrate, a.masteryrate) as ClubMastery, COALESCE(b.treasurerate, a.treasurerate) as TreasureRate, COALESCE(b.chuvarate, a.chuvarate) as ChuvaRate, COALESCE(b.scratchyporpointrate, a.scratchrate) as ScratchyRate, COALESCE(b.papelshoprareitemrate, a.rareitemrate) as PapelShopRareItemRate, COALESCE(b.papelshopcookieitemrate, a.cookieitemrate) as PapelShopCookieItemRate, COALESCE(b.memorialshoprate, 0) as MemorialShopRate, COALESCE(b.grandzodiaceventtime, 0) as GrandZodiacEventTime, COALESCE(b.AngelEvent, 0) as AngelEvent, COALESCE(b.GrandPrixEvent, 0) as GrandPrixEvent, COALESCE(b.GoldenTimeEvent, 0) as GoldenTimeEvent, COALESCE(b.LoginRewardEvent, 0) as LoginRewardEvent, COALESCE(b.BotGMEvent, 0) as BotGMEvent, COALESCE(b.SmartCalculator, 0) as SmartCalculator FROM ".$this->db->con_dados['DB_NAME'].'.pangya_server_list a LEFT OUTER JOIN '.$this->db->con_dados['DB_NAME'].'.pangya_config b ON a.uid = b.uid ORDER BY type';
				else
					$query = 'SELECT a.uid, a.type, a.name, IF(a.UpdateTime > TIMESTAMPADD(SECOND, -12, NOW()), 1, 0) as status, a.currUser, IFNULL(b.pangrate, a.pangrate) as PangRate, IFNULL(b.exprate, a.exprate) as ExpRate, IFNULL(b.clubmasteryrate, a.masteryrate) as ClubMastery, IFNULL(b.treasurerate, a.treasurerate) as TreasureRate, IFNULL(b.chuvarate, a.chuvarate) as ChuvaRate, IFNULL(b.scratchyporpointrate, a.scratchrate) as ScratchyRate, IFNULL(b.papelshoprareitemrate, a.rareitemrate) as PapelShopRareItemRate, IFNULL(b.papelshopcookieitemrate, a.cookieitemrate) as PapelShopCookieItemRate, IFNULL(b.memorialshoprate, 0) as MemorialShopRate, IFNULL(b.grandzodiaceventtime, 0) as GrandZodiacEventTime, IFNULL(b.AngelEvent, 0) as AngelEvent, IFNULL(b.GrandPrixEvent, 0) as GrandPrixEvent, IFNULL(b.GoldenTimeEvent, 0) as GoldenTimeEvent, IFNULL(b.LoginRewardEvent, 0) as LoginRewardEvent, IFNULL(b.BotGMEvent, 0) as BotGMEvent, IFNULL(b.SmartCalculator, 0) as SmartCalculator FROM pangya_server_list a LEFT OUTER JOIN pangya_config b ON a.uid = b.uid ORDER BY `type`';

				if ($result =$this->db->db->execPreparedStmt($query, NULL, 1)) {
					if ($result->num_rows > 0) {
						while($row = $result->fetch_assoc())  {
							if( isset($row['name']))
								$server_st[] = $row;
						}
					}
				}

				if ($server_st) {
					echo '<style>
							table {
								border: 1px solid black;
								margin: 2% auto;
							}
							td {
								border: 1px solid black;
							}
							#form-rate > * {
								border: 1px solid black;
							}
						 </style>';

					echo '<table width="98%" cellspacing="0">';

					echo '<tr>
							<td>UID</td>
							<td>Tipo</td>
							<td>Server Name</td>
							<td>Status</td>
							<td>Players ON</td>
							<td>Pang Rate</td>
							<td>Experiência Rate</td>
							<td>Clubset Mastery Rate</td>
							<td>Treasure Rate</td>
							<td>Chuva Rate</td>
							<td>Scratchy Rate</td>
							<td>Papel Shop Rare Item Rate</td>
							<td>Papel Shop Cookie Item Rate</td>
							<td>Memorial Shop Rate</td>
							<td>Grand Zodiac Event Time</td>
							<td>Angel Event</td>
							<td>GrandPrixEvent</td>
							<td><a href="gm_tools/golden_time.php">GoldenTimeEvent</a></td>
							<td><a href="gm_tools/login_reward.php">LoginRewardEvent</a></td>
							<td><a href="gm_tools/bot_gm_event.php">Bot GM Event</a></td>
							<td>Smart Calculator</td>
						  </tr>';

					for ($i = 0; $i < count($server_st); $i++) {
						echo '<tr>';

						foreach ($server_st[$i] as $key => $value) {
							if ($key == 'status')
								echo '<td>'.$span[$value].'</td>';
							else
								echo '<td>'.$value.'</td>';
						}

						echo '</tr>';
					}

					echo '</table>';

					// Aqui as opções extras
					echo "<script>
							var arr2 = [];
							arr2.push(new tab('rates', 'rates-c'));
							arr2.push(new tab('events', 'events-c'));
							arr2.push(new tab('reload', 'reload-c'));
							arr2.push(new tab('ban_ip', 'ban_ip-c'));
							arr2.push(new tab('broadcast', 'broadcast-c'));
							arr2.push(new tab('gift', 'gift-c'));
							arr2.push(new tab('donate', 'donate-c'));
							arr2.push(new tab('shutdown', 'shutdown-c'));

							function altertab2(nome, conteudo) {
								var c = undefined, m = undefined, i = 0;

								for (i = 0; i < arr2.length; i++) {
									if (m = document.getElementById(arr2[i].nome))
										m.className = 'menu';
									
									if (arr2[i].conteudo != 'all') {
										if (c = document.getElementById(arr2[i].conteudo))
											c.style.display = 'none';
									}
								}

								if (m = document.getElementById(nome))
									m.className = 'menu-sel';

								if (conteudo == 'all') {
									for (i = 0; i < arr2.length; i++) {
										if (arr2[i].conteudo != 'all') {
											if (c = document.getElementById(arr2[i].conteudo))
												c.style.display = '';
										}
									}
								}else {
									if (c = document.getElementById(conteudo))
										c.style.display = '';
								}
							}
						  </script>";

					echo '<style>
							#tab-extra > * {
								width: 100% !important;
							}
							#tab-extra td {
								width: 100% !important;
							}
							#tab-extra tr > * {
								margin: auto 0.3% !important;
							}
							#tab-extra tr {
								display: flex !important;
								width: 100%;
							}
						 </style>';

					// Abas, Rates, Events, Reload System, Broadcast e Shutdown
					echo '<table id="tab-extra" width="99%" style="border: 0; margin: auto 0.5%; display: flex">';

					echo '<tr style="display: flex">
							<td class="menu-sel" id="rates" onclick="altertab2(\'rates\', \'rates-c\')">Rates</td>
							<td class="menu" id="events" onclick="altertab2(\'events\', \'events-c\')">Events</td>
							<td class="menu" id="reload" onclick="altertab2(\'reload\', \'reload-c\')">Reload Systems</td>
							<td class="menu" id="ban_ip" onclick="altertab2(\'ban_ip\', \'ban_ip-c\')">Block IP/MAC Table</td>
							<td class="menu" id="broadcast" onclick="altertab2(\'broadcast\', \'broadcast-c\')">Broadcast</td>
							<td class="menu" id="gift" onclick="altertab2(\'gift\', \'gift-c\')">Gift</td>
							<td class="menu" id="donate" onclick="altertab2(\'donate\', \'donate-c\')">Doação</td>
							<td class="menu" id="shutdown" onclick="altertab2(\'shutdown\', \'shutdown-c\')">Shutdown Server</td>
						 </tr>
						 </table>';

					// conteudo div Rates
					echo '<div id="rates-c" class="conteudo-div" style="width: 98%; margin: 1% auto">';

						echo "<script>
							function change_label(val) {
								var rate = val / 100;

								var c = document.getElementById('rate_x');
								if (c)
									c.innerHTML = 'x' + rate;
							}
						</script>";

						echo '<style>
								#form-rate > * {
									display: block;
									margin: 2% auto;
								}
								#form-rate label {
									border: 0;
									width: 50%;
									text-align: right;
									margin-right: 1%;
								}
								#form-rate div {
									border: 0;
									display: flex;
								}
								#form-rate input {
									display: block;
									border: 1px solid black;
								}
								select {
									display: block;
									border: 1px solid black;
								}
								#rate-div {
									width: 98%;
									margin: auto 1%;
								}
							 </style>';

						echo '<div id="rate-div">';

						// Rates do server
						echo '<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<div><label>Rates: </label><select name="rate">
									<option value="0">Pang</option>
									<option value="1">Experiência</option>
									<option value="2">Clubset Mastery</option>
									<option value="3">Chuva</option>
									<option value="4">Treasure Point</option>
									<option value="5">Scratchy</option>
									<option value="6">Papel Shop Rare Item</option>
									<option value="7">Papel Shop Cookie Item</option>
									<option value="8">Memórial Shop</option>
									</select>
								</div>
								<div>
									<label>Quantidade: <span id="rate_x">x1</span> </label>
									<input type="range" name="qntd" value="100" min="100" max="800" step="100" oninput="change_label(this.value)" style="border: 0">
								</div>
								<div>
									<label>Quantos minutos para executar esse comando: </label><input type="text" name="reserved_date" pattern="[0-9]+" placeholder="tempo em minutos">
								</div>
								<div>
									<label>Alvo: </label><input type="text" name="alvo" required pattern="[0-9]+" placeholder="uid server ou tipo" title="UID server ou tipo EX: 0 Login Server, 1 Game Server, 3 Messager Server e 5 Auth Server">
								</div>
								<input type="submit" value="Trocar Rate" style="margin: 1% auto">
							  </form>';

						echo '</div>';

					echo '</div>'; // Fecha div conteudo Rates "Aba-server-Rate"
					
					// conteudo div Events
					echo '<div id="events-c" class="conteudo-div" style="width: 98%; margin: 1% auto; display: none">';

						echo '<div id="rate-div">';

						// Event do server
						echo '<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<div><label>Events: </label><select name="rate">
									<option value="9">Grand Zodiac Event Time</option>
									<option value="10">Angel Event</option>
									<option value="11">Grand Prix Event</option>
									<option value="12">Golden Time Event</option>
									<option value="13">Login Reward Event</option>
									<option value="14">Bot GM Event</option>
									<option value="15">Smart Calculator</option>
									</select>
								</div>
								<div>
									<label>Enable: </label>
									<input type="number" name="qntd" min="0", max="65535" style="border: 0">
								</div>
								<div>
									<label>Quantos minutos para executar esse comando: </label><input type="text" name="reserved_date" pattern="[0-9]+" placeholder="tempo em minutos">
								</div>
								<div>
									<label>Alvo: </label><input type="text" name="alvo" required pattern="[0-9]+" placeholder="uid server ou tipo" title="UID server ou tipo EX: 0 Login Server, 1 Game Server, 3 Messager Server e 5 Auth Server">
								</div>
								<input type="submit" value="Ativar ou Desativar Evento" style="margin: 1% auto">
							  </form>';

						echo '</div>';

					echo '</div>'; // Fecha div conteudo Events "Aba-server-Events"

					// conteudo div Reload Systems
					echo '<div id="reload-c" class="conteudo-div" style="width: 98%; margin: 1% auto; display: none">';

						echo '<div id="rate-div">';

						// Sistemas do server
						echo '<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<div><label>Sistemas: </label><select name="reload">
									<option value="0">Todos(All)</option>
									<option value="1">Iff</option>
									<option value="2">Card</option>
									<option value="3">Comet Refill</option>
									<option value="4">Papel Shop</option>
									<option value="5">Box</option>
									<option value="6">Memorial Shop</option>
									<option value="7">Cube & Coin</option>
									<option value="8">Treasure Hunter</option>
									<option value="9">Drop</option>
									<option value="10">Attendance Reward</option>
									<option value="11">Map Course</option>
									<option value="12">Approach Mission</option>
									<option value="13">Grand Zodiac Event</option>
									<option value="14">Cube & Coin Location Update</option>
									<option value="15">Golden Time Event</option>
									<option value="16">Login Reward Event</option>
									<option value="17">Bot GM Event</option>
									<option value="18">Smart Calculator</option>
									</select>
								</div>
								<div>
									<label>Quantos minutos para executar esse comando: </label><input type="text" name="reserved_date" pattern="[0-9]+" placeholder="tempo em minutos">
								</div>
								<div>
									<label>Alvo: </label><input type="text" name="alvo" required pattern="[0-9]+" placeholder="uid server ou tipo" title="UID server ou tipo EX: 0 Login Server, 1 Game Server, 3 Messager Server e 5 Auth Server">
								</div>
								<input type="submit" value="Recarregar Sistema" style="margin: 1% auto">
							  </form>';

						echo '</div>';

					echo '</div>'; // Fecha div conteudo Events "Aba-server-Reload Systems"

					// conteudo div Block IP/MAC Table
					echo '<div id="ban_ip-c" class="conteudo-div" style="width: 98%; margin: 1% auto; display: none">';

					echo '<div id="rate-div">';

					$consulta2 = isset($_GET['consult2']) ? $_GET['consult2'] : -1;

					// Consulta IP & MAC Table ou adicionar IP Or MAC
					if ($consulta2 == -1) {

						echo '<br><a href="?adm=1&aba=1&consult2=1">Consulta Block IP/MAC Table</a><br><br>';

						// Form IP Address
						echo '<h3>Block IP</h3>
							<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<div>
									<label style="width: 45%;">IP: </label><input type="text" name="ip" pattern="^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$|((?:[0-9]{1,3}\.){3}[0-9]{1,3}\/(?:[0-9]{1,3}\.){3}[0-9]{1,3})$" required>
								</div>
								<div>
									<label style="width: 45%;">MASK: </label><input type="text" name="mask" pattern="^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$" required>
								</div>
								<input type="submit" value="Bloquear IP ">
							</form>';

						// Form MAC Address
						echo '<h3>Block MAC</h3>
							<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<div>
									<label style="width: 45%">MAC: </label><input type="text" name="mac_address" pattern="^(?:[a-fA-F0-9]{2}\:){5}[a-fA-F0-9]{2}$" required>
								</div>
								<input type="submit" value="Bloquear MAC ">
							</form>';

					}else {

						// Link para voltar para a IP & MAC Table
						echo '<br><a href="?adm=1&aba=1">Back</a><br><br>';

						// IP Address Table
						$query = '';
						
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGetIPTable';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT "_ip_" as "ip", "_mask_" as "mask", "_date_" as "date", "_index_" as "index" FROM '.$this->db->con_dados['DB_NAME'].'.ProcGetIPTable()';
						else
							$query = 'CALL ProcGetIPTable()';

						if ($result = $this->db->db->execPreparedStmt($query, NULL, 1)) {
							
							if ($result->num_rows > 0) {

								echo '<h3>Block IP Table: '.$result->num_rows.'</h3>';

								echo '<table width="98%" cellspacing="0">';

								echo '<tr>
											<td>IP</td>
											<td>MASK</td>
											<td>Date</td>
											<td>Ação</td>
										</tr>';

								while($row = $result->fetch_assoc())  {

									echo '<tr>';

									foreach ($row as $key => $value) {

										if ($key == 'index') {

											echo '<td>
													<form action="?adm=1&aba=1&consult2=1" method="POST">
														<input type="hidden" name="index" value="'.($value).'">
														<input type="hidden" name="del_block_ip" value="1">
														<input type="submit" value="Excluir">
													</form>
												</td>';

										}else
											echo '<td>'.$value.'</td>';
									
									}

									echo '</tr>';

								}

								echo '</table>';
							}
						}

						// MAC Address Table
						$query = '';
						
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGetMACTable';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT "_mac_" as "mac", "_date_" as "date", "_index_" as "index" FROM '.$this->db->con_dados['DB_NAME'].'.ProcGetMACTable()';
						else
							$query = 'CALL ProcGetMACTable()';

						if ($result = $this->db->db->execPreparedStmt($query, NULL, 1)) {
							
							if ($result->num_rows > 0) {

								echo '<h3>Block MAC Table: '.$result->num_rows.'</h3>';

								echo '<table width="98%" cellspacing="0">';

								echo '<tr>
											<td>MAC</td>
											<td>Date</td>
											<td>Ação</td>
										</tr>';

								while($row = $result->fetch_assoc())  {

									echo '<tr>';

									foreach ($row as $key => $value) {

										if ($key == 'index') {

											echo '<td>
													<form action="?adm=1&aba=1&consult2=1" method="POST">
														<input type="hidden" name="index" value="'.($value).'">
														<input type="hidden" name="del_block_mac" value="1">
														<input type="submit" value="Excluir">
													</form>
												</td>';

										}else
											echo '<td>'.$value.'</td>';
									
									}

									echo '</tr>';

								}

								echo '</table>';
							}
						}
					}

					echo '</div>';	// Div rate-div

					echo '</div>'; // Fecha div conteudo Events "Aba-server-Block IP/MAC Table"

					// Conteudo div Broadcast
					echo '<div id="broadcast-c" class="conteudo-div" style="width: 98%; margin: 1% auto; display: none">';

						echo '<div id="rate-div">';

						echo '<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<label style="text-align: center; width: 100%;">Mensagem</label><textarea name="broadcast" rows="10" cols="60" width="98%" maxlength="1024" style="resize: none"></textarea>
								<div>
									<label>Quantas vezes repetir essa mensagem: </label><input type="text" name="replay_count" value="1" pattern="[0-9]+">
								</div>
								<div>
									<label>Intervalor para repetir essa mensagem: </label><input type="text" name="refresh_time" pattern="[0-9]+" placeholder="tempo em minutos">
								</div>
								<div>
									<label>Quantos minutos para executar esse comando: </label><input type="text" name="reserved_date" pattern="[0-9]+" placeholder="tempo em minutos">
								</div>
								<div>
									<label>Alvo: </label><input type="text" name="alvo" required pattern="[0-9]+" placeholder="uid do server ou tipo" title="UID do server ou tipo. Ex: 0 Login Server, 1 Game Server, 3 Messager Server e 5 Auth Server">
								</div>
							 	<input type="submit" value="Enviar comando">
							 </form>';

						echo '</div>';

					echo '</div>'; // Fecha div conteudo Broadcast "Aba-server-Broadcast"

					// Conteudo div Gift
					echo '<div id="gift-c" class="conteudo-div" style="width: 98%; margin: 1% auto; display: none">';

					echo '<div id="rate-div">';

					$consulta = isset($_GET['consult3']) ? $_GET['consult3'] : -1;

					// Consulta Gift ou Presenteia
					if ($consulta == -1) {

						echo '<br><a href="?adm=1&aba=1&consult3=1">Consulta Gift do GM</a><br><br>';

						echo '<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<div>
									<label style="width: 35%;">UID: </label><input type="text" name="uid" pattern="[0-9]+" required>
								</div>
								<div>
									<label style="width: 35%;">Item typeid: </label><input type="text" name="item_typeid_1" pattern="[0-9]+"><label style="width: auto; margin-left: 2%;">Quantidade: </label><input type="number" min="0" name="item_qntd_1">
								</div>
								<div>
									<label style="width: 35%;">Item typeid: </label><input type="text" name="item_typeid_2" pattern="[0-9]+"><label style="width: auto; margin-left: 2%;">Quantidade: </label><input type="number" min="0" name="item_qntd_2">
								</div>
								<div>
									<label style="width: 35%;">Item typeid: </label><input type="text" name="item_typeid_3" pattern="[0-9]+"><label style="width: auto; margin-left: 2%;">Quantidade: </label><input type="number" min="0" name="item_qntd_3">
								</div>
								<div>
									<label style="width: 35%;">Item typeid: </label><input type="text" name="item_typeid_4" pattern="[0-9]+"><label style="width: auto; margin-left: 2%;">Quantidade: </label><input type="number" min="0" name="item_qntd_4">
								</div>
								<div>
									<label style="width: 35%;">Item typeid: </label><input type="text" name="item_typeid_5" pattern="[0-9]+"><label style="width: auto; margin-left: 2%;">Quantidade: </label><input type="number" min="0" name="item_qntd_5">
								</div>
								<label style="text-align: center; width: 100%;">Mensagem</label><textarea name="msg" rows="10" cols="60" width="98%" maxlength="500" style="resize: none"></textarea>
								<input type="submit" value="Presentear">
							</form>';

					}else {

						// Link para voltar para a Gift
						echo '<br><a href="?adm=1&aba=1">Back</a><br><br>';

						$query = '';
				
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGetGMGiftWebInfo';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcGetGMGiftWebInfo()';
						else
							$query = 'CALL ProcGetGMGiftWebInfo()';

						if ($result = $this->db->db->execPreparedStmt($query, NULL, 1)) {
							
							if ($result->num_rows > 0) {

								echo '<h3>Gifts: '.$result->num_rows.'</h3>';

								echo '<table width="98%" cellspacing="0">';

								echo '<tr>
											<td>ADM_UID</td>
											<td>USER_UID</td>
											<td>Count Item</td>
											<td>Mensagem</td>
											<td>Date</td>
										</tr>';

								while($row = $result->fetch_assoc())  {

									echo '<tr>';

									foreach ($row as $key => $value) {

										if ($key == 'message')
											echo '<td>'.htmlspecialchars($value).'</td>';
										else
											echo '<td>'.$value.'</td>';
									
									}

									echo '</tr>';

								}

								echo '</table>';
							}
						}
					}

					echo '</div>';	// Div rate-div

					echo '</div>'; // Fecha div conteudo "Aba-server-Gift"

					// Conteudo div Doação
					echo '<div id="donate-c" class="conteudo-div" style="width: 98%; margin: 1% auto; display: none">';

					echo '<div id="rate-div">';

					$consulta = isset($_GET['consult']) ? $_GET['consult'] : -1;

					// Consulta doações ou faz doação
					if ($consulta == -1) {

						echo '<br><a href="?adm=1&aba=1&consult=1">Consulta doações</a><br><br>';

						echo '<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<input type="hidden" name="insert-dona" value="insert">
								<div>
									<label style="width: 35%;">UID: </label><input type="text" name="uid" required>
									<label style="width: auto; margin-left: 2%;">Email: </label><input type="email" name="email" pattern="^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$" required>
								</div>
								<div>
									<label style="width: 35%;">Gross Amount: </label><input type="text" name="gross_amount" required>
									<label style="width: auto; margin-left: 2%;">Net Amount: </label><input type="text" name="net_amount">
								</div>
								<div>
									<label style="width: 35%;">Type: </label>
									<select name="type">
										<option value="1">Doação</option>
									</select>
									<label style="width: auto; margin-left: 2%;">Plataforma: </label><select name="plataforma">
										<option value="1">Paypal</option>
										<option value="2">PagSeguro</option>
									</select>
								</div>
								<div>
									<label style="width: 35%;">Code: </label><input type="text" name="code">
									<label style="width: auto; margin-left: 2%;">Reference: </label><input type="text" name="reference">
								</div>
								<div>
									<label style="width: 35%">Status: </label>
									<select name="status">
										<option value="1">Aguardando pagamento</option>
										<option value="2">Em analise</option>
										<option value="3">Pago</option>
										<option value="4">Disponível</option>
										<option value="5">Disputa</option>
										<option value="6">Devolvido</option>
										<option value="7">Cancelado</option>
										<option value="8">Cliente pediu para devolver</option>
									</select>
									<label style="width: auto; margin-left: 2%;">Date: </label><input type="text" name="date">
								</div>
								<div>
									<label style="width: 35%;">Update: </label><input type="text" name="update">
									<label style="width: auto; margin-left: 2%;">Escrow: </label><input type="text" name="escrow">
								</div>
								<input type="submit" value="Registrar doação">
							</form>';

					}else {

						// Link para voltar para a doação
						echo '<br><a href="?adm=1&aba=1">Back</a><br><br>';

						$query = '';
				
						if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'EXEC '.$this->db->con_dados['DB_NAME'].'.ProcGetDonationInfo';
						else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
							$query = 'SELECT * FROM '.$this->db->con_dados['DB_NAME'].'.ProcGetDonationInfo()';
						else
							$query = 'CALL ProcGetDonationInfo()';

						if ($result = $this->db->db->execPreparedStmt($query, NULL, 1)) {
							
							if ($result->num_rows > 0) {

								echo '<h3>Doações: '.$result->num_rows.'</h3>';

								echo '<table width="98%" cellspacing="0">';

								echo '<tr>
											<td>ADM_UID</td>
											<td>USER_UID</td>
											<td>Plataforma</td>
											<td>Cash</td>
											<td>CP</td>
											<td>Count Item</td>
											<td>E-mail</td>
											<td>Observação</td>
											<td>Date</td>
										</tr>';

								while($row = $result->fetch_assoc())  {

									echo '<tr>';

									foreach ($row as $key => $value) {

										if ($key == 'plataforma') {

											echo '<td>'.($value == 1 ? 'Paypal' : ($value == 2 ? 'PagSeguro' : 'Unknown')).'</td>';

										}else if ($key == 'obs')
											echo '<td>'.htmlspecialchars($value).'</td>';
										else
											echo '<td>'.$value.'</td>';
									
									}

									echo '</tr>';

								}

								echo '</table>';
							}
						}

						echo '<h3 style="text-align: center; margin-bottom: 20px">Donation New</h3>';

						$lambda = function($player) {

							if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
								$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcGetAllDonationsAndEpin';
							else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
								$query = 'select "_code_" as "code", "_date_" as "date", "_email_" as "email", "_escrow_" as "escrow", "_gross_amount_" as "gross_amount", "_net_amount_" as "net_amount", "_plataforma_" as "plataforma", "_epin_id_" as "epin_id", "_reference_" as "reference", "_reg_date_" as "reg_date", "_status_" as "status", "_type_" as "type", "_update_" as "update", "_uid_" as "uid", "_index_" as "index", "_epin_" as "epin", "_cp_qntd_" as "cp_qntd", "_epin_get_uid_" as "epin_get_uid", "_epin_valid_" as "epin_valid" from '.$this->db->con_dados['DB_NAME'].'.ProcGetAllDonationsAndEpin()';
							else
								$query = 'call '.$this->db->con_dados['DB_NAME'].'.ProcGetAllDonationsAndEpin()';
			
							$player->params->clear();
			
							if (($result = $player->db->db->execPreparedStmt($query, null, 1)) == null && $player->db->db->getLastError() != 0) {
			
								error_log("Player[UID=".$player->uid."] Fail in exec proc get all donations and epin. Error code: ".$player->db->db->getLastError(), 0);
			
								return '<p style="text-align: center">Não conseguiu pegar as suas doações deu erro no sistema. Code: 1000</p>';
							}
			
							if ($result == null || ($row = $result->fetch_assoc()) == null) {
			
								//error_log("Player[UID=".$player->uid."] Fail in proc get all donation and epin by player, result empty.", 0);
			
								return '<p style="text-align: center">Não tem nenhum doação.</p>';
							}
			
							if (!isset($row['code']) || !isset($row['plataforma']) || !isset($row['date']) || !isset($row['gross_amount']) || !isset($row['status'])
								|| !isset($row['email']) || !key_exists('escrow', $row) || !isset($row['net_amount']) || !isset($row['epin_id'])
								|| !isset($row['reference']) || !isset($row['reg_date']) || !isset($row['type']) || !key_exists('update', $row)
								|| !isset($row['uid']) || !isset($row['index']) || !key_exists('epin_get_uid', $row)
								|| !key_exists('epin', $row) || !key_exists('cp_qntd', $row) || !key_exists('epin_valid', $row) || $row['code'] == -1) {
			
								error_log("Player[UID=".$player->uid."] Fail in proc get all donations end epin by, result invalid.", 0);
			
								return '<p style="text-align: center">Não conseguiu pegar as suas doações deu erro no sistema. Code: 1001</p>';
							}

							$str = '<style>
										.dona-table {
											border-collapse: collapse;
										}
										.dona-table td, th {
											padding: 3px 20px;
											text-align: center;
										}
										.dona-table tr:first-child {
											padding-bottom: 100px;
										}
										.dona-table tr:not(:first-child) {
											border-bottom: 1px solid black;
										}
										.edit-dona {
											cursor: pointer;
											text-decoration: underline;
										}
										.edit-cell {
											border: 1px solid black;
											margin: 2px;
										}
									</style>';

							$str .= '<script>
										
										function edit_dona(id) {

											const tr = document.querySelector(".id-" + id);

											if (!tr)
												return;

											const hasTD = document.getElementById("cell-" + id)

											if (hasTD)
												return;

											const td = document.createElement("td");

											td.id = "cell-" + id;
											td.colSpan = tr.cells.length;
											td.align = "center";

											let form = "<form action=\"?adm=1&aba=1&consult=1\" method=\"POST\">";
											
											form += "<input class=\"edit-cell\" type=\"submit\" value=\"Editar\"><button class=\"edit-cell\" onClick=\"dellCell(\'" + td.id + "\')\">fechar</button>";

											form += "<input type=\"hidden\" name=\"edit-dona\" value=\"edit\">";

											Array.prototype.forEach.call(tr.cells, el => {
												if (el.hasAttribute("name") && el.hasAttribute("value"))
													form += "<input class=\"edit-cell\" type=\"" + (el.className != "edit-dona" ? "text" : "hidden") + "\" name=\"" + el.getAttribute("name") + "\" value=\"" 
														+ (el.getAttribute("name") == \'epin_id\' && isNaN(el.getAttribute("value")) ? -1 : el.getAttribute("value")) + "\">";
											});

											form += "</form>";

											form += "<form action=\"?adm=1&aba=1&consult=1\" method=\"POST\">";

											form += "<input type=\"hidden\" name=\"del-dona\" value=\"delete\">"

											form += "<input type=\"hidden\" name=\"id\" value=\"" + id + "\">";

											form += "<input class=\"edit-cell\" type=\"submit\" value=\"Deletar\">";

											form += "</form>";

											td.innerHTML = form;

											tr.parentElement.insertBefore(td, tr.nextSibling);
										}

										function dellCell(id) {

											const td = document.getElementById(id);

											if (!td)
												return false;

											td.parentElement.removeChild(td);

											return false;
										}
									 </script>';

							$str .= '<p style="text-align: center">Quantidade de doações: '.$result->num_rows.'</p>';
			
							$str .= '<table align="center" class="dona-table">
										<tr>
											<th>ID</th>
											<th>TYPE</th>
											<th>Status</th>
											<th>Plataforma</th>
											<th>Date</th>
											<th>Last update</th>
											<th>UID</th>
											<th>Email</th>
											<th>Reference</th>
											<th>Transaction code</th>
											<th>Valor Bruto</th>
											<th>Valor líquido</th>
											<th>Data de liberação</th>
											<th>E-Pin ID</th>
											<th>E-Pin</th>
											<th>Quem usou o E-Pin</th>
											<th>CP Quantidade</th>
											<th>E-Pin Status</th>
											<th>Data de registro</th>
										</tr>';
			
							do {
			
								$str .= '<tr class="id-'.$row['index'].'">';

								$str .= '<td name="id" value="'.$row['index'].'" class="edit-dona" onClick="edit_dona('.$row['index'].')">'.$row['index'].'</td>';

								$str .= '<td name="type" value="'.$row['type'].'">'.$row['type'].'</td>';
								
								$str .= '<td name="status" value="'.$row['status'].'">'.(function() use($row) {
			
									$status = 'Desconhecido';
			
									switch  ($row['status']) {
										case 1:
											$status = 'Aguardando pagamento';
										break;
										case 2:
											$status = 'Em analise';
										break;
										case 3:
											$status = 'Pago';
											break;
										case 4:
											$status = 'Disponível';
										break;
										case 5:
											$status = 'Disputa';
											break;
										case 6:
											$status = 'Devolvido';
											break;
										case 7:
											$status = 'Cancelado';
										break;
										case 8:
											$status = 'Cliente pediu para devolver';
										break;
									}
			
									return $status;
								})().'</td>';
			
								$str .= '<td name="plataforma" value="'.$row['plataforma'].'">'.($row['plataforma'] == 1 ? 'PayPal' : ($row['plataforma'] == 2 ? 'PagSeguro' : 'Desconhecido')).'</td>';
			
								$str .= '<td name="date" value="'.$row['date'].'">'.(new DateTime($row['date']))->format('Y-m-d H:i').'</td>';

								$str .= '<td name="update" value="'.(($row['update'] == '' || $row['update'] == null) ? 'NULL' : $row['update'])
									.'">'.(($row['update'] == '' || $row['update'] == null) ? 'NULL' : (new DateTime($row['update']))->format("Y-m-d H:i")).'</td>';

								$str .= '<td name="uid" value="'.$row['uid'].'">'.$row['uid'].'</td>';

								$str .= '<td name="email" value="'.$row['email'].'">'.$row['email'].'</td>';

								$str .= '<td name="reference" value="'.$row['reference'].'">'.$row['reference'].'</td>';
			
								$str .= '<td name="code" value="'.$row['code'].'">'.$row['code'].'</td>';
			
								$str .= '<td name="gross_amount" value="'.$row['gross_amount'].'">'.$row['gross_amount'].'</td>';

								$str .= '<td name="net_amount" value="'.$row['net_amount'].'">'.$row['net_amount'].'</td>';

								$str .= '<td name="escrow" value="'.(($row['escrow'] == '' || $row['escrow'] == null) ? 'NULL' : $row['escrow']).'">'
									.(($row['escrow'] == '' || $row['escrow'] == null) ? 'NULL' : (new DateTime($row['escrow']))->format("Y-m-d H:i")).'</td>';
			
								if ($row['epin'] == '' || $row['epin'] == null)
									$str .= '<td name="epin_id" value="-1" colspan="5">'.($row['status'] == 6 || $row['status'] == 7 ? 'Doação cancelada' : 'Aguardando a doção ficar em status "Pago" para gerar o E-Pin').'</td>';
								else {
			
									$str .= '<td name="epin_id" value="'.$row['epin_id'].'">'.$row['epin_id'].'</td>';

									$str .= '<td>'.$row['epin'].'</td>';

									$str .= '<td>'.($row['epin_get_uid'] == '' || $row['epin_get_uid'] == null ? 'NULL' : $row['epin_get_uid']).'</td>';
			
									$str .= '<td>'.$row['cp_qntd'].'</td>';
			
									$str .= '<td>'.($row['epin_valid'] == 1 ? 'Válida' : 'Usado').'</td>';
								}

								$str .= '<td>'.(new DateTime($row['reg_date']))->format("Y-m-d H:i").'</td>';
			
							} while (($row = $result->fetch_assoc()) != null);
			
							$str .= '</table>';
			
							return $str;
						};
			
						echo $lambda($this);
					}

					echo '</div>';	// Div rate-div

					echo '</div>'; // Fecha div conteudo "Aba-server-Doação"

					// Conteudo div Shutdown
					echo '<div id="shutdown-c" class="conteudo-div" style="width: 98%; margin: 1% auto; display: none">';

						echo '<div id="rate-div">';

						echo '<form id="form-rate" action="?adm=1&aba=1" method="POST">
								<div>
									<label>Tempo para desligar o server</label><input type="text" name="shutdown" value="0" pattern="[0-9]+" placeholder="tempo em minutos">
								</div>
								<div>
									<label>Quantos minutos para executar esse comando: </label><input type="text" name="reserved_date" pattern="[0-9]+" placeholder="tempo em minutos">
								</div>
								<div>
									<label>Alvo: </label><input type="text" name="alvo" required pattern="[0-9]+" placeholder="uid do server ou tipo" title="UID do Server ou Tipo Ex: 0 Login Server, 1 Game Server, 3 Messager Server e 5 Auth Server">
								</div>
							 	<input type="submit" value="Enviar comando">
							 </form>';

						echo '</div>';

					echo '</div>'; // Fecha div conteudo Shutdown "Aba-server-Shutdown"

					echo '</div>'; // Fecha div conteudo server "Aba-server"
				}

				// Conteúdo da Div Log

				if ($aba == 2)
					echo '<div id="log-c" class="conteudo-div">';
				else
					echo '<div id="log-c" class="conteudo-div" style="display: none">';

					$log_index = isset($_GET['log_index']) ? $_GET['log_index'] : -1;

					// Cria a lista de logas
					if ($log_index == -1) {

						echo '<table id="area-link-log" style="width: 100%; height: 100%;">';	// div area link log

						echo '<span>Logs</span><br><br>';
						
						$log_link = ['<a href="?adm=1&aba=2&log_index=', '" style="text-decoration: underline">', ' / Size: ', '</a><br>'];

						echo '<td style="text-align: left; padding: 2%">';

						listDir($log_link, CONST_LOG_DIR, "");

						echo '</td>';

						echo '</table>';	// Fim de div area link log

					}else {

						// Link para voltar para a lista de arquivos de log
						echo '<a href="?adm=1&aba=2">Back</a><br><br>';
						echo '<span>'.$log_index.'</span><br>';
						
						// Text Area
						echo '<textarea readonly style="resize: none; width: 100%; height: 500px">';

						//echo file_get_contents(CONST_LOG_DIR."/".$log_index);
						echo mb_convert_encoding(file_get_contents(CONST_LOG_DIR."/".$log_index), 'UTF-8', 'SJIS');
						
						echo '</textarea>';
					}

				echo '</div>';	// Fecha Div Log

				echo '</article>';
			}

		} else {

			// Log
			error_log("Player[UID=".$this->uid.", NICKNAME=".$this->nick.", IP=".$this->ip_addr.", CAPABILITY=".$this->cap."] tentou acessa a pagina adm_view.", 0);

			echo "<script>window.addEventListener('load', function() {
				window.open('pangya.php', '_self');
			});</script>";
		}
	}

	private function notice_manager_view() {

		if (!$this->logado)
			echo "<script>window.addEventListener('load', function() {
				alert('Por favor logue primeiro.');
				window.open('pangya.php', '_self');
			});</script>";
		else if (($this->cap & 4) && ($this->cap & 8) /*&& ($this->ip_addr == '127.0.0.1' || $this->ip_addr == '192.168.0.101')*/) { // ADM

			if ($this->Error == 0) {

				// Message
				$str_msg = '';

				if ($this->msg != 0) {
				  	switch ($this->msg) {
						case 600: // Criou notice com sucesso
						  	$str_msg = "Criou notice com sucesso.";
							break;
						case 601:
							$str_msg = "Deletou notice com sucesso.";
							break;
						case 602:	// Error ao criar o notice
							$str_msg = "Error ao criar o notice.";

							// Volta para a página de criação de notice
							$this->argumentos['get']['add'] = 1;
							break;
						case 603:	// Error ao deletar o notice
							$str_msg = "Error ao deletar o notice.";
							break;
						case 604:	// Editou notice com sucesso;
							$str_msg = "Editou notice com sucesso.";
							break;
						case 605:	// Error ao edtiar o notice.
							$str_msg = "Error ao editar o notice";
							break;
				  		default:
				  			$str_msg = 'Erro desconhecido';
				  	} 

					echo "<script>alert('".$str_msg."');</script>";
				}

				// Constants values
				$GLOBALS['NOTICE_TYPE_LABEL'] = [
					'ALL',
					'NOTICE',
					'EVENT',
					'UPDATE'
				];

				include_once("notice_singleton.inc");

				echo '<article id="home">';

				echo '<h2>Notice manager</h2>';

				echo '<div id="notice-manager" class="conteudo-div">';

				// Conteudo do div notice manager
				$query = '';

				echo '<style>
						table {
							border: 1px solid black;
							margin: 2% auto;
						}
						td {
							border: 1px solid black;
						}
						.ta_add {
							resize: none;
							border: 1px solid black;
							width: 100%;
							height: 500px;
						}
						.ttl_add {
							border: 1px solid black;
							width: 100%;
						}
						.slc_add {
							border: 1px solid black;
							width: 100px;
						}
						.btn_add {
							border: 1px solid black;
							cursor: pointer;
						}
						</style>';

				echo '<br><div height="100%" width="100%"><a href="?notice_manager=1&add=1"></a></div>';

				// Add notice
				if (isset($_GET['add'])) {
					
					echo '<form method="POST" action="?notice_manager=1">
						<table style="border: none" width="98%" cellspacing="0" cellpadding="0" border="0">
							<tr>
								<td align="left" width="100" style="border: none">
									Tipo: 
								</td>
								<td align="left" style="border: none">
									<select class="slc_add" name="type">
										<option value="1" selected>Notice</option>
										<option value="2">Event</option>
										<option value="3">Update</option>
									</select>
								</td>
							</tr>
							<tr>
								<td style="border: none" height="10px" colspan="2"></td>
							</tr>
							<tr>
								<td align="left" width="100" style="border: none">
									Título:
								</td>
								<td align="left" style="border: none"> 
									<input class="ttl_add" type="text" name="title">
								</td>
							</tr>
							<tr>
								<td style="border: none" height="20px" colspan="2"></td>
							</tr>
							<tr>
								<td align="center" colspan="2" style="border: none">Conteúdo</td>
							</tr>
							<tr>
								<td align="center" colspan="2" style="border: none">
									<textarea class="ta_add" name="body"></textarea>
								</td>
							</tr>
							<tr>
								<td style="border: none" height="10px" colspan="2"></td>
							</tr>
							<tr>
								<td align="center" colspan="2" style="border: none">
									<table width="100%" cellspacing="0" cellpadding="0" border="0" style="border: none">
										<tr>
											<td with="50%" align="right" style="padding-right: 10px; border: none">
												<a style="border: black solid 1px" href="?notice_manager=1">Cancelar</a>
											</td>
											<td align="left" style="padding-left: 10px; border: none">
												<input class="btn_add" type="submit" value="Criar notice">
											</td>
										</tr>
									</table>
								</td>
							</tr>
						</table>
						<input type="hidden" name="add_notice" value="1">
					</form>';
				
				}else if (isset($_GET['edit']) && $_GET['edit'] != '' && is_numeric($_GET['edit']) 
					&& ($notice = NoticeSingleton::getInstance()->findNotice($_GET['edit'])) != null) {

					echo '<form method="POST" action="?notice_manager=1">
						<table style="border: none" width="98%" cellspacing="0" cellpadding="0" border="0">
							<tr>
								<td align="left" width="100" style="border: none">
									Tipo: 
								</td>
								<td align="left" style="border: none">
									<select class="slc_add" name="type">
										<option value="1" '.($notice->type == NT_NOTICE ? 'selected' : '').'>Notice</option>
										<option value="2" '.($notice->type == NT_EVENT ? 'selected' : '').'>Event</option>
										<option value="3" '.($notice->type == NT_UPDATE ? 'selected' : '').'>Update</option>
									</select>
								</td>
							</tr>
							<tr>
								<td style="border: none" height="10px" colspan="2"></td>
							</tr>
							<tr>
								<td align="left" width="100" style="border: none">
									Título:
								</td>
								<td align="left" style="border: none"> 
									<input class="ttl_add" type="text" name="title" value="'.$notice->title.'">
								</td>
							</tr>
							<tr>
								<td style="border: none" height="20px" colspan="2"></td>
							</tr>
							<tr>
								<td align="center" colspan="2" style="border: none">Conteúdo</td>
							</tr>
							<tr>
								<td align="center" colspan="2" style="border: none">
									<textarea class="ta_add" name="body">'.$notice->body.'</textarea>
								</td>
							</tr>
							<tr>
								<td style="border: none" height="10px" colspan="2"></td>
							</tr>
							<tr>
								<td align="center" colspan="2" style="border: none">
									<table width="100%" cellspacing="0" cellpadding="0" border="0" style="border: none">
										<tr>
											<td with="50%" align="right" style="padding-right: 10px; border: none">
												<a style="border: black solid 1px" href="?notice_manager=1">Cancelar</a>
											</td>
											<td align="left" style="padding-left: 10px; border: none">
												<input class="btn_add" type="submit" value="Editar notice">
											</td>
										</tr>
									</table>
								</td>
							</tr>
						</table>
						<input type="hidden" name="id" value="'.$notice->id.'">
						<input type="hidden" name="edit_notice" value="1">
					</form>';

				}else {
					
					echo '<h4>Notices. Qnty: '.count(NoticeSingleton::getInstance()->getAllNotices()).'</h4>';

					echo '<br><a href="?notice_manager=1&add=1" style="text-decoration: underline">Create notice</a>';

					if (count(NoticeSingleton::getInstance()->getAllNotices()) > 0) {

						echo '<table cellspacing="0" width="98%">
							<tr>
								<td>ID</td>
								<td>TYPE</td>
								<td>TITLE</td>
								<td>DATE</td>
								<td>AÇÃO</td>
							</tr>';

						foreach(NoticeSingleton::getInstance()->getAllNoticesSortByDate() as $n) {

							echo '<tr>';
							
							echo '<td>'.$n->id.'</td>';
							echo '<td width="100px">'.$GLOBALS['NOTICE_TYPE_LABEL'][$n->type].'</td>';

							echo '<td align="left" style="padding-left: 5px">';
								
								if ($n->body != '')
									echo '<a style="text-decoration: underline" href="#" onclick="javascript:window.open(\'notice_view.php?id='.$n->id.'\', \'Notice\', \'width=600,height=400,scrollbars=yes\')">';
								
									echo $n->title;
								
								if ($n->body != '')
									echo '</a>';
							echo '</td>';

							echo '<td>'.date("d/m/y H:i", $n->date).'</td>';
							echo '<td>
									<a href="?notice_manager=1&edit='.$n->id.'" style="display: inline; text-decoration: underline; cursor: pointer; border: 1px solid black">Editar</a>
									/
									<form style="display: inline" action="?notice_manager=1" method="POST">
										<input type="hidden" name="del_notice" value="1">
										<input type="hidden" name="id" value="'.$n->id.'">
										<input class="btn_add" type="submit" value="Deletar">
									</form>
								  </td>';

							echo '</tr>';
						}

						echo '</table>';
					}
				}

				echo '</div>';

				echo '</article>';
			}

		}else {
			
			// Log
			error_log("Player[UID=".$this->uid.", NICKNAME=".$this->nick.", IP=".$this->ip_addr.", CAPABILITY=".$this->cap."] tentou acessa a pagina notice_manager_view.", 0);

			echo "<script>window.addEventListener('load', function() {
				window.open('pangya.php', '_self');
			});</script>";
		}
	}

	private function guild_manager_view() {

		if (!$this->logado)
			echo "<script>window.addEventListener('load', function() {
				alert('Por favor logue primeiro.');
				window.open('pangya.php', '_self');
			});</script>";
		else if (($this->cap & 4) && ($this->cap & 8) /*&& ($this->ip_addr == '127.0.0.1' || $this->ip_addr == '192.168.0.101')*/) { // ADM

			if ($this->Error == 0) {

				// Message
				$str_msg = '';

				if ($this->msg != 0) {
				  	switch ($this->msg) {
						case 501: // Aceita Mark
						  	$str_msg = "Guild Mark foi aceita com sucesso.";
							break;
						case 502: // Rejeita Mark
							$str_msg = "Guild Mark foi rejeitada com sucesso.";
							break;
						case 503: // Fecha Guild
							$str_msg = "Guild foi fechada com sucesso.";
							break;
						case 504:
							$str_msg = "Não conseguiu aceitar a nova guild mark.";
							break;
						case 505:
							$str_msg = "Não tem uma nova guild mark para poder aceitar.";
							break;
						case 506:
							$str_msg = "Não conseguiu rejeitar a guild mark.";
							break;
						case 507:
							$str_msg = "Não conseguiu fechar a guild.";
							break;
						case 508:
							$str_msg = "Reativou a guild com sucesso.";
							break;
						case 509:
							$str_msg = "Não conseguiu reativar a guild.";
							break;
						case 510:
							$str_msg = "A guild não está fechada para reabrir ela.";
							break;
						case 511:
							$str_msg = "A guild já está fechada.";
							break;
						case 512:
							$str_msg = "Bloqueou a guild com sucesso.";
							break;
						case 513:
							$str_msg = "Não conseguiu bloquear a guild.";
							break;
						case 514:
							$str_msg = "A guild já está bloqueada.";
							break;
						case 515:
							$str_msg = "Desbloqueou a guild com sucesso.";
							break;
						case 516:
							$str_msg = "Não conseguiu desbloquear a guild.";
							break;
						case 517:
							$str_msg = "A guild não está bloqueada para desbloquear.";
							break;
				  		default:
				  			$str_msg = 'Erro desconhecido';
				  	} 

					echo "<script>alert('".$str_msg."');</script>";
				}

				// Constants values
			
				// Guild condition level label
				$GLOBALS['CONDITION_LEVEL_LABEL'] = [
					0 => 'No level conditions',
					6 => 'Beginner or above',
					11 => 'Junior or above',
					16 => 'Senior or above',
					21 => 'Amateur or above'
				];
			
				// Guild state
				$GLOBALS['STATE_LABEL'] = [
					0 => 'Approval pending',
					1 => 'Now Open',
					2 => 'Blocked join',
					3 => 'GM has Blocked',
					4 => 'Closure',
					5 => 'GM has Closure'
				];

				// Guild Permition join
				$GLOBALS['PERMITION_JOIN_LABEL'] = [
					0 => 'No',
					1 => 'Yes'
				];

				// Guild Flag
				$GLOBALS['FLAG_LABEL'] =  [
					0 => 'None'
				];

				echo '<article id="home">';

				echo '<h3>Guild Manager</h3>';

				echo '<div id="guild-manager" class="conteudo-div">';

				// Conteudo do div guild manager
				$query = '';

				echo '<style>
						table {
							border: 1px solid black;
							margin: 2% auto;
						}
						td {
							border: 1px solid black;
						}
						.bt_p {
							border: 1px solid black;
						}
						</style>';

				echo '<br><div height="100%" width="100%">
								<button style="width: 45%; display: flex-inline" class="bt_p" onclick="window.open(\'?guild=1\', \'_self\')">All Guild</button>
								<button style="display: flex-inline; width: 45%" class="bt_p" onclick="window.open(\'?guild=1&accept_pending=1\', \'_self\')">Accept Pending</buton>
								</div>';

				if (isset($_GET['accept_pending'])) {
					
					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcGetListGuildWeb 1';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'select "_GUILD_INTRO_IMG_" as "GUILD_INTRO_IMG", "_GUILD_UID_" as "GUILD_UID", "_GUILD_NAME_" as "GUILD_NAME", "_GUILD_MARK_IMG_IDX_" as "GUILD_MARK_IMG_IDX", "_GUILD_NEW_MARK_IDX_" as "GUILD_NEW_MARK_IDX", "_GUILD_MASTER_" as "GUILD_MASTER", "_GUILD_SUB_MASTER_" as "GUILD_SUB_MASTER", "_MEMBERS_" as "MEMBERS", "_GUILD_CONDITION_LEVEL_" as "GUILD_CONDITION_LEVEL", "_GUILD_STATE_" as "GUILD_STATE", "_GUILD_FLAG_" as "GUILD_FLAG", "_GUILD_PERMITION_JOIN_" as "GUILD_PERMITION_JOIN", "_GUILD_POINT_" as "GUILD_POINT", "_GUILD_PANG_" as "GUILD_PANG", "_GUILD_INFO_" as "GUILD_INFO", "_GUILD_NOTICE_" as "GUILD_NOTICE", "_GUILD_REG_DATE_" as "GUILD_REG_DATE", "_GUILD_ACCEPT_DATE_" as "GUILD_ACCEPT_DATE", "_GUILD_CLOSURE_DATE_" as "GUILD_CLOSURE_DATE" from '.$this->db->con_dados['DB_NAME'].'.ProcGetListGuildWeb(1)';
					else
						$query = 'call ProcGetListGuildWeb(1)';
				
				}else {
					
					if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcGetListGuildWeb 0';
					else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
						$query = 'select "_GUILD_INTRO_IMG_" as "GUILD_INTRO_IMG", "_GUILD_UID_" as "GUILD_UID", "_GUILD_NAME_" as "GUILD_NAME", "_GUILD_MARK_IMG_IDX_" as "GUILD_MARK_IMG_IDX", "_GUILD_NEW_MARK_IDX_" as "GUILD_NEW_MARK_IDX", "_GUILD_MASTER_" as "GUILD_MASTER", "_GUILD_SUB_MASTER_" as "GUILD_SUB_MASTER", "_MEMBERS_" as "MEMBERS", "_GUILD_CONDITION_LEVEL_" as "GUILD_CONDITION_LEVEL", "_GUILD_STATE_" as "GUILD_STATE", "_GUILD_FLAG_" as "GUILD_FLAG", "_GUILD_PERMITION_JOIN_" as "GUILD_PERMITION_JOIN", "_GUILD_POINT_" as "GUILD_POINT", "_GUILD_PANG_" as "GUILD_PANG", "_GUILD_INFO_" as "GUILD_INFO", "_GUILD_NOTICE_" as "GUILD_NOTICE", "_GUILD_REG_DATE_" as "GUILD_REG_DATE", "_GUILD_ACCEPT_DATE_" as "GUILD_ACCEPT_DATE", "_GUILD_CLOSURE_DATE_" as "GUILD_CLOSURE_DATE" from '.$this->db->con_dados['DB_NAME'].'.ProcGetListGuildWeb(0)';
					else
						$query = 'call ProcGetListGuildWeb(0)';
				}

				if (($result = $this->db->db->execPreparedStmt($query, NULL, 1)) != null && $this->db->db->getLastError() == 0) {
					
					$guilds = [];
					$rows_count = 0;

					while ($r = $result->fetch_assoc()) {

						$guilds[$rows_count] = [];

						foreach ($r as $k => $v)
							$guilds[$rows_count][$k] = $v;

						$rows_count++;
					}

					if ($rows_count > 0) {

						echo "<script>
								function reply_pending(uid, type) {
									if (type == 0)
										window.open('pangya.php?guild=1&accept=' + uid, '_self');
									else if (type == 1)
										window.open('pangya.php?guild=1&reject=' + uid, '_self');
									else if (type == 2)
										window.open('pangya.php?guild=1&closure=' + uid, '_self');
									else if (type == 3)
										window.open('pangya.php?guild=1&reopen=' + uid, '_self');
									else if (type == 4)
										window.open('pangya.php?guild=1&block=' + uid, '_self');
									else if (type == 5)
										window.open('pangya.php?guild=1&unblock=' + uid, '_self');
								}
							</script>";

						echo '<br>Guilds: '.count($guilds);

						echo '<table width="98%" cellspacing="0">';

						echo '<tr>';
						
						echo'<td>Intro Img</td>
							<td>UID</td>
							<td>NAME</td>
							<td>MARK</td>
							<td>NEW MARK</td>
							<td>MASTER</td>
							<td>SUB MASTER</td>
							<td>MEMBERS</td>
							<td>CONDITION LEVEL</td>
							<td>STATE</td>
							<td>FLAG</td>
							<td>PERMITION JOIN</td>
							<td>POINT</td>
							<td>PANG</td>
							<td>WELCOME MESSAGE</td>
							<td>MASTER COMMENT</td>
							<td>CREATE DATE</td>
							<td>ACCEPT DATE</td>
							<td>CLOSURE DATE</td>
							<td>Ação</td>';

						echo '</tr>';

						foreach ($guilds as $row) {
							echo '<tr>';

							foreach ($row as $key => $value) {
								if ($key == 'GUILD_INTRO_IMG')
									echo '<td><img style="display: block" width="320" height="240" src="../_Files/guild/intro/'.($value == '' ? 'sample.jpg' : $value).'"></td>';
								else if ($key == 'GUILD_MARK_IMG_IDX')
									echo '<td><img width="22" height="20" src="../_Files/guild/mark/'.($value <= 0 ? 'sample.png' : $row['GUILD_UID'].'_'.$value.'.png?dummy='.rand(151515, 278646521)).'"></td>';
								else if ($key == 'GUILD_NEW_MARK_IDX')
									echo '<td><img width="22" height="20" src="../_Files/guild/mark/'.($value <= 0 ? 'sample.png' : $row['GUILD_UID'].'_'.$value.'.png?dummy='.rand(358584, 714551044)).'"></td>';
								else if ($key == 'GUILD_MASTER' || $key == 'GUILD_SUB_MASTER')
									echo '<td>'.htmlspecialchars(mb_convert_encoding($value, "UTF-8", "SJIS")).'</td>';
								else if ($key == 'GUILD_CONDITION_LEVEL')
									echo '<td>'.$GLOBALS['CONDITION_LEVEL_LABEL'][$value].'</td>';
								else if ($key == 'GUILD_STATE')
									echo '<td>'.$GLOBALS['STATE_LABEL'][$value].'</td>';
								else if ($key == 'GUILD_PERMITION_JOIN')
									echo '<td>'.$GLOBALS['PERMITION_JOIN_LABEL'][$value].'</td>';
								else if ($key == 'GUILD_FLAG')
									echo '<td>'.($value == 0 ? $GLOBALS['FLAG_LABEL'][$value] : $value).'</td>';
								else if ($key == 'GUILD_NAME' || $key == 'GUILD_INFO' || $key == 'GUILD_NOTICE')
									echo '<td>'.htmlspecialchars(mb_convert_encoding($value, "UTF-8", "SJIS")).'</td>';
								else
									echo '<td>'.$value.'</td>';
							}

							// Action
							echo '<td>';

							if (isset($_GET['accept_pending'])) {

								echo '	<button style="padding: 10px 0" onclick="reply_pending('.$row['GUILD_UID'].', 0)">Aceita</button>
										<br>/
										<button style="padding: 10px 0" onclick="reply_pending('.$row['GUILD_UID'].', 1)">Rejeita</button>
										<br>/
										<button style="padding: 10px 0" onclick="reply_pending('.$row['GUILD_UID'].', 2)">Fecha</button>';

							}else {
									
								echo '	<button style="padding: 10px 0" onclick="reply_pending('.$row['GUILD_UID'].', 2)">Fecha</button>
										<br>/
										<button style="padding: 10px 0" onclick="reply_pending('.$row['GUILD_UID'].', 3)">ReOpen</button>
										<br>/
										<button style="padding: 10px 0" onclick="reply_pending('.$row['GUILD_UID'].', 4)">Block</button>
										<br>/
										<button style="padding: 10px 0" onclick="reply_pending('.$row['GUILD_UID'].', 5)">Unblock</button>';
							}

							echo '</td>';

							echo '</tr>';
						}

						echo '</table>';
					
					}else
						echo '<br> Guilds: '.$rows_count.'<br><br>';
				
				}else
					echo '<br> Error na consulta. Code: '.$this->db->db->getLastError().'<br><br>';

				echo '</div>';

				echo '</article>';
			}

		}else {
			
			// Log
			error_log("Player[UID=".$this->uid.", NICKNAME=".$this->nick.", IP=".$this->ip_addr.", CAPABILITY=".$this->cap."] tentou acessa a pagina guild_manager_view.", 0);

			echo "<script>window.addEventListener('load', function() {
				window.open('pangya.php', '_self');
			});</script>";
		}
	}

	public function adm_edit_player() {
		
		if (isset($_SESSION['player_log']) && (($_SESSION['player_log']['cap'] & 4) && ($_SESSION['player_log']['cap'] & 8)) && isset($_GET['uid'])) {
			$player_uid = $_GET['uid'];

			echo '<html lang="PT-BR">
					<head>
						<meta charset="UTF-8" />
						<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
						<link rel="icon" href="/favicon.ico" type="image/x-icon">
						<title>ADM - Edit Player</title>';
			
			echo "<script>
					function checado(check) {
						var c = document.getElementById('ban_min');
						if (c) {
							if (check.checked)
								c.style.display = '';
							else
								c.style.display = 'none';
						}
					}
				  </script>";

			echo	'</head>
					<body>
						<div id="dv_principal">';


			if ($this->msg == 0) {

				$this->params->clear();
				$this->params->add('i', $player_uid);

				$query = '';
				
				if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'SELECT id, nick, idstate, blocktime, capability, ISNULL(DATEADD(MINUTE, BlockTime, BlockRegDate), NULL) as blockregdate FROM '.$this->db->con_dados['DB_NAME'].'.account WHERE UID = ?';
				else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = "SELECT id, nick, idstate, blocktime, capability, COALESCE((BlockRegDate + ('1 minute'::interval * BlockTime)), NULL) as blockregdate FROM ".$this->db->con_dados['DB_NAME'].'.account WHERE UID = ?';
				else
					$query = 'SELECT id, nick, idstate, blocktime, capability, IFNULL(TIMESTAMPADD(MINUTE, BlockTime, BlockRegDate), -1) as blockregdate FROM account WHERE UID = ?';

				if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) && $result->num_rows > 0) {

					if (($row = $result->fetch_assoc()) && isset($row['idstate'])) {
						$player_id_state = $row['idstate'];
						$player_id = $row['id'];
						$player_nick = (isset($row['nick']) ? htmlspecialchars(mb_convert_encoding($row['nick'], 'UTF-8', 'SJIS')) : 'unknown');
						$player_blocktime = $row['blocktime'];
						$player_block_reg_date = ($row['blockregdate'] == null || $row['blockregdate'] == -1) ? - 1 : strtotime($row['blockregdate']);	// Date String to UNIX TIME
						$player_cap = $row['capability'];

						echo '<h3>Editando Player. UID: '.$player_uid.'. ID: '.$player_id.'.<t> Nick: '.$player_nick.'</h3>';

						echo '<div id="div_state">
								<form action="?uid='.$player_uid.'" method="POST">';
									
						if ($player_id_state & 1){
							//date_default_timezone_set("America/Sao_Paulo");

							// Se verdade ainda está bloqueado
							$is_block = ($player_block_reg_date != -1 && $player_block_reg_date > time()) ? true : false;
							
							if (!$is_block)
								echo '<h8 style="color: red">Esse player já foi bloqeuado por tempo anteriomente</h8><br>';

							echo '<input type="checkbox" onclick="checado(this)" name="ban_tmp" value='.((($player_id_state & 1) && $is_block) ? '"1" checked' : '"1"').'>Ban Temporario<input type="text" id="ban_min" name="ban_min" placeholder="tempo em minutos" style="display: '.((($player_id_state & 1) && $is_block) ? '" value="'.$player_blocktime.'"' : 'none " value=""').'><br>';
						}else {
							echo '<input type="checkbox" onclick="checado(this)" name="ban_tmp" value="1">Ban Temporario<input type="text" id="ban_min" name="ban_min" placeholder="tempo em minutos" style="display: none" value=""><br>';
						}
						

						echo 	   '<input type="checkbox" name="ban_perm" value='.(($player_id_state & 2) ? '"2" checked' : '"2"').'>Ban permanente<br>
									<input type="checkbox" name="ban_lounge" value='.(($player_id_state & 4) ? '"4" checked' : '"4"').'>Ban Lounge<br>
									<input type="checkbox" name="ban_sh_lounge" value='.(($player_id_state & 8) ? '"8" checked' : '"8"').'>Ban Shop Lounge<br>
									<input type="checkbox" name="ban_gift_shop" value='.(($player_id_state & 16) ? '"16" checked' : '"16"').'>Ban Gift Shop<br>
									<input type="checkbox" name="ban_papel" value='.(($player_id_state & 32) ? '"32" checked' : '"32"').'>Ban Papel Shop<br>
									<input type="checkbox" name="ban_scratchy" value='.(($player_id_state & 64) ? '"64" checked' : '"64"').'>Ban Scratchy<br>
									<input type="checkbox" name="ban_ticker" value='.(($player_id_state & 128) ? '"128" checked' : '"128"').'>Ban Ticker<br>
									<input type="checkbox" name="ban_memorial" value='.(($player_id_state & 256) ? '"256" checked' : '"256"').'>Ban Memorial Shop<br>
									<input type="checkbox" name="ban_all_ip" value='.(($player_id_state & 512) ? '"512" checked' : '"512"').'>Ban All IP<br>
									<input type="checkbox" name="ban_all_mac" value='.(($player_id_state & 1024) ? '"1024" checked' : '"1024"').'>Ban All MAC Addres<br>
									<input type="checkbox" name="ban_create_guild" value='.(($player_id_state & 2048) ? '"2048" checked' : '"2048"').'>Ban Create Giuld<br>
									<input type="checkbox" name="ban_lucia_attendance" value='.(($player_id_state & 4096) ? '"4096" checked' : '"4096"').'>Ban Lucia Attendance<br>
									<input type="submit" value="Enviar">
								</form>
							  </div>';


						echo '<div id="div_cap">
								<form action="?uid='.$player_uid.'" method="POST">';
						

						// Cap 2 dá bug nos itens active quando vai abri a janela no lobby para mexer neles
						echo '<input type="hidden" name="cap_edit" value="none">
							  <input type="checkbox" name="cap_block_give_item" value='.(($player_cap & 16) ? '"16" checked' : '"16"').'>Bloqueia Giveitem/Goldenbell Comandos<br>
							  <input type="checkbox" name="cap_gm" value='.(($player_cap & 4) ? '"4" checked' : '"4"').'>Game Master<br>
							  <input type="checkbox" name="cap_gm_site" value='.(($player_cap & 8) ? '"8" checked' : '"8"').'>GM Edit Site<br>
							  <input type="checkbox" name="cap_mantle" value='.(($player_cap & 1024) ? '"1024" checked' : '"1024"').'>Mantle<br>
							  <input type="checkbox" name="cap_title_gm" value='.(($player_cap & 32768) ? '"32768" checked' : '"32768"').'>Title GM<br>
							  <input type="checkbox" name="cap_AI" value='.(($player_cap & 1) ? '"1" checked' : '"1"').'>A.I Mode<br>
							  <input type="submit" value="Enviar">';

						echo '</form>
							</div>';
					}
				}

				echo		'</div>
						</body>
					</html>';
				}else
					echo "<script>
							alert('O ".(($this->msg == 26) ? "capacidade" : "estado")." do player foi trocado com sucesso');
							window.close();
						  </script>";
		}else {

			// Log
			error_log("Player[UID=".$this->uid.", NICKNAME=".$this->nick.", IP=".$this->ip_addr.", CAPABILITY=".$this->cap."] tentou acessa a pagina adm_edit_player.", 0);

			echo "<script>window.addEventListener('load', function() {
				window.open('pangya.php', '_self');
			});</script>";
		}
	}

	private function faq_view() {
		echo '<article id="home">';

		echo '<h3>FAQ - Perguntas frequentes</h3>';

		echo '<div style="margin: 5% 2% 2% 2%">';

		echo "<script>function show_hide(doc) {
			if (doc.value == undefined || doc.value == 'true') {
				doc.childNodes[1].style.display = 'block'
				doc.value = 'false';
			}else {
				doc.value = 'true';
				doc.childNodes[1].style.display = 'none';
			};
		}</script>";

		// Backup do usar FAQ
		echo '<style>
				.sp_faq {
					display: block;
					margin: 2% 0 0 5%;
					text-align: left;
					color: red;
				}
				.sp_faq2 {
					margin: 1% 0 0 3%;
					color: black !important;
				}
		     </style>';

		/*echo '<span class="sp_faq" onclick="show_hide(this);" style="cursor: pointer;"> 1 - Backup da conta do Pangya USA
				<span class="sp_faq2" style="display: none;">
					- O backup do Pangya USA não vai ser usado em nenhum beta, só no lançamento do server.<br>
				</span>
			  </span>';*/

		echo '<span class="sp_faq" onclick="show_hide(this);" style="cursor: pointer;"> 1 - Cadastro no beta
				<span class="sp_faq2" style="display: none;">
					- Todos os que acessarem esse site e tiver um email valído, poderam cadastrar no beta.<br>
				</span>
			  </span>';
		
		echo '<span class="sp_faq" onclick="show_hide(this);" style="cursor: pointer;"> 2 - Coloqueo a pasta do PangYa SuperSS na exclusão do anti-vírus
				<span class="sp_faq2" style="display: none;">
						- Coloqueo a pasta do PangYa SuperSS na exclusão de pasta do seu anti-vírus, para que o anti-vírus não bloqueie o PangYa SuperSS.<br>
				</span>
			</span>';

		/*echo '<span class="sp_faq" onclick="show_hide(this);" style="cursor: pointer;"> 3 - Erro 100015 no Login Server
				<span class="sp_faq2" style="display: none;">
					- Esse erro é por que o seu ip está diferente, logue no site e troque ele no link que vai aparecer.<br>
					- 3 vezes é o limite que vai poder trocar o ip no mesmo dia.<br>
				</span>
			  </span>';*/
		
		echo '<span class="sp_faq" id="bug_report" onclick="show_hide(this);" style="cursor: pointer;"> 3 - Bug report
				<span class="sp_faq2" style="display: none;">
					- O bug report vai ajudar a tirar os bugs que não consegui achar no desenvolvimento.<br>
					<br>
					- Vou está usando o canal de chat no Discord, para o bug report.<br>
					- Link para o canal no discord. <a href="https://discord.gg/mFcVvY6" style="text-decoration: underline">Bug report</a><br>
				</span>
			  </span>';

		echo '<span class="sp_faq" onclick="show_hide(this);" style="cursor: pointer;"> 4 - Download cliente
				<span class="sp_faq2" style="display: none;">
					<a href="pangya.php?link=2" style="text-decoration: underline">Página de Download do cliente.</a><br>
					- Para baixar o cliente. Faça o download do cliente. Na página de download do cliente.<br>
					- Baixe o MSVC++ redistributable 2017.<br>
					- Instale o MSVC++ redistributable 2017 na versão x86(32bit), mesmo que a versão do seu sistema operacional for outra(x64).<br>
					- Execute o update.exe na pasta do PangYa SuperSS para atualizar o jogo e para entrar nele.<br>
				</span>
			  </span>';

		echo '<span class="sp_faq" onclick="show_hide(this);" style="cursor: pointer;"> 5 - Meu pangya não quer abrir
				<span class="sp_faq2" style="display: none;">
					- Solução -> Sempre executar o update.exe como administrador.<br>
				</span>
			  </span>';

		echo '<span class="sp_faq" onclick="show_hide(this);" style="cursor: pointer;"> 6 - Verificar o comprimento do nickname com o emoticon
				<span class="sp_faq2" style="display: none;">
					- Nesse <a href="check_nickname.php" target="_blank" style="color: blue; text-decoration: underline; cursor: pointer">link</a> voçê pode verifica o comprimento do nickname com o emoticon.<br>
				</span>
			  </span>';

		echo '</div>';

		echo '</article>';
	}

	private function cadastro_view() {
		echo '<article id="home">';

		if (true/*$this->pincode != '' || $this->msg != */) {

			if ($this->msg != 0 && $this->msg != 200) {
				$str_msg = '';

				switch ($this->msg) {
					case 3:
						$str_msg = "Os emails estão diferentes";
						break;
					case 4:
						$str_msg = "As senhas estão diferentes";
						break;
					case 6:
						$str_msg = "Error no sistema. Code: 6";
						break;
					case 8:
						$str_msg = "ID já existe";
						break;
					case 9:
						$str_msg = "email já existe";
						break;
					case 10:
						$str_msg = "email inválido";
						break;
					case 11:
						$str_msg = "Dados inválidos";
						break;
					case 15:	// Erro no desafio reCAPTCHA
						$str_msg = "Erro no desafio reCAPTCHA";
						break;
					case 100:
						$str_msg = "Cadastrado com sucesso";
						break;
					case 150:
						$str_msg = "Foi enviado um email com as instruções da próxima etapa do cadastro para o endereço de email: ";
						$str_msg .= $this->email." Verifique seu Lixo eletrônico ou o spam, se ele não estiver na caixa de entrada. Ele pode demorar um pouco.";
						break;
					case 101:
						$str_msg = "Erro na hora de cadastrar a conta.";
						break;
					case 107:
						$str_msg = "Erro ao enviar o email para o endereço de email: ".$this->email;
						break;
					case 1001:	// Mensagem de mentirar só para a pessoa parar de tentar criar conta
						$str_msg = "Reached smtp limit per month, please contact support";
						break;
					default:
						$str_msg = "Erro desconhecido";
						break;
				}

				echo "<script>window.addEventListener('load', function() {alert('".$str_msg."');";

				if ($this->msg == 100 || $this->msg == 150)
					echo "window.open('pangya.php', '_self');";

				echo "});</script>";
			}


			if ($this->msg == 200 || ($this->msg != 0 && isset($this->argumentos['get']['yes']) && $this->argumentos['get']['yes'] == 1)) {	// Cadastro
				echo '<style>
						#fo_ca {
							width: 100%;
							display: block;
							margin 3% auto;
						}
						#fo_ca > * {
							margin: 1% 0;
						}
						#fo_ca input {
							border: 1px solid black;
						}
						#fo_ca div > * {
							display: inline-block;
							width: 20%
						}
						#fo_ca label {
							text-align: left;
						}
						.la_ca {
							display: block;
						}
					  </style>';

				echo '<h3>Cadastro</h3>';

				include("first_cadastro_frm.inc");

				/*echo '<form action="pangya.php?link=3&yes=1" method="POST" id="fo_ca">
					 <div><label class="la_ca">ID: </label><input type="text" name="id" pattern="[A-Za-z0-9_]{4,16}" required></div>
					 <div><label class="la_ca">Nome: </label><input type="text" name="nome" pattern"[a-zA-Z]{4, 20}" required></div>
					 <div><label class="la_ca">Sobre Nome: </label><input type="text" name="sobre_nome" pattern="[^ _\-.\'][a-zA-Z]{4, 40}" required></div>
					 <div><label class="la_ca">Email: </label><input type="email" name="email" pattern="^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$" required></div>
					 <div><label class="la_ca">Confirme Email: </label><input type="email" name="confirm_email" pattern="^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$" required></div>
					 <div><label class="la_ca">Senha: </label><input type="password" name="pass" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required></div>
					 <div><label class="la_ca">Confirma Senha: </label><input type="password" name="confirm_pass" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required></div>';

				$ip_cliente = getenv('REMOTE_ADDR');

				echo '<input type="hidden" name="ip" value="'.$ip_cliente.'">
					 <input type="submit" value="Enviar">
					 </form>';*/
			}else { // Contrato para participar do beta
				echo '<style>
					#form-term > * {
						margin: 1% 1%;
						border: 1px solid black;
						border-radius: 3px;
					}
					textarea {
						border: 1px solid black;
						margin-top: 1%;
						resize: none;
					}
				 </style>';

				echo '<h3>Cadastro - Termos de uso</h3>';

				echo '<textarea rows="20" cols="50" readonly>	Termos de uso do beta

1 - Mande bug report sempre que achar algum bug, para que o server possa ser melhorado para o lançamento.
2 - Não criei mais de uma conta se não a outra vai ser excluída, se for confirmado que é da mesma pessoa.
3 - Não use hacker, se for reportado e constatado que este usou hacker, a conta vai ser bloqueada permanente.
4 - Não crie conta com GM, Game Master, ADM, Adm, no nome. Por que o nick vai ser trocado e a conta bloqueada por 2 dias.
5 - Não falte com respeito com ninguém, pois tera a conta bloqueada.
6 - Não pode entrar em outra conta que não seja a sua, se for confirmado que alguém entrou na sua conta, ela vai ser banida.
7 - Qualquer programa que faça a leitura ou modificão da memória do pangya está proibido, se eu pegar usando, a conta vai ser banida.
8 - A conta pode ser vendida, mas quem vendeu não joga nunca mais. E quem compra a conta, não pode ter conta já. Se quem vendeu tentar criar conta de novo, a que ele vendeu e a nova que ele criou vão ser banidas para sempre. Quem comprou e já tem conta, a conta que ele comprou e a outra conta dele vão ser banidas para sempre.
9 - A sua conta poderar ser bloqueada ou excluída sem aviso previo, se não respeitar o termos de uso.
10 - Os Termos poderam ser modificado sem aviso previo.

Data de funcionamento. Tempo inderterminado.

2019 Acrisio SuperSS Dev.
</textarea>';

				echo '<form id="form-term">';
					echo '<input type="hidden" name="term_use" value="1">';
					echo '<input type="submit" formaction="pangya.php?link=3&yes=1" formmethod="POST" value="sim">';
					echo '<input type="submit" formaction="pangya.php?link=3&yes=0" formmethod="POST" value="não">';
				echo '</form>';
			}
		}else {
			echo '<style>
					.binput {
						border: 1px solid black;
						border-radius: 3px;
					}
				 </style>';

			echo '<h3>Cadastro - Pincode</h3>';

			echo '<form action="pangya.php?link=3" method="POST" style="margin: 3%;">
				 <label> Chave do beta fechado:</label>
				 <input type="text" class="binput" name="pincode" pattern="[a-zA-A0-9]{32}" required placeholder="chave do beta fechado">
				 <input type="submit" class="binput" value="enviar">
				 </form>';
		}

		echo '</article>';
	}

	private function finish_cadastro_view() {
		echo '<article id="home">';

		if ($this->msg != 0) {
			$str_msg = '';

			switch ($this->msg) {
				case 3:
					$str_msg = "Os emails estão diferentes";
					break;
				case 4:
					$str_msg = "As senhas estão diferentes";
					break;
				case 5:
					$str_msg = "Tem caracteres inválidos no id";
					break;
				case 6:
					$str_msg = "Error no sistema. Code: 6";
					break;
				case 8:
					$str_msg = "ID já existe";
					break;
				case 9:
					$str_msg = "email já existe";
					break;
				case 10:
					$str_msg = "email inválido";
					break;
				case 11:
					$str_msg = "Dados inválidos";
					break;
				case 100:
					$str_msg = "Cadastrado com sucesso";
					break;
				case 101:
					$str_msg = "Error na hora de cadastrar a conta.";
					break;
				case 102:
					$str_msg = "Link desconhecido.";
					break;
				case 103:
					$str_msg = "Error nao consulta com o banco de dados.";
					break;
				case 104:
					$str_msg = "Chave invalida.";
					break;
				default:
					$str_msg = "Erro desconhecido";
					break;
			}

			echo "<script>window.addEventListener('load', function() {alert('".$str_msg."');";

			if ($this->msg == 100 || $this->msg == 104 || $this->msg == 102)
				echo "window.open('pangya.php', '_self');";

			echo "});</script>";
		}

		if (isset($_SESSION['player_cadastro']) && isset($_SESSION['player_cadastro']['index']) && isset($_SESSION['player_cadastro']['key'])) {

			// Cadastro
			echo '<style>
					#fo_ca {
						width: 100%;
						display: block;
						margin 3% auto;
					}
					#fo_ca > * {
						margin: 1% 0;
					}
					#fo_ca input {
						border: 1px solid black;
					}
					#fo_ca div > * {
						display: inline-block;
						width: 20%
					}
					#fo_ca label {
						text-align: left;
					}
					.la_ca {
						display: block;
					}
					</style>';

			echo '<h3>Cadastro - Etapa Final</h3>';

			include("seg_cadastro_frm.inc");

		}else if (!($this->msg == 100 || $this->msg == 104 || $this->msg == 102))
			echo "<script>window.open('pangya.php', '_self')</script>";

		echo '</article>';
	}

	private function doar() {
		echo '<article id="home">';

		echo '<h1>Quer ajudar o projeto?</h1><br>';
		echo '<h7>Se sentir o desejo e quiser contribuir com o meu trabalho, faça uma doação de qualquer valor, desta forma você vai está me ajudando a manter e a adquirir novas ferramentas para o desenvolvimento deste projeto. Desde já muito obrigado pela sua contribuição.</h7><br>';
		echo '<br><h5>Você pode doar qualquer quantia para ajudar no VPS para manter o Beta mais tempo possível online.</h5>';

		echo '<br><br><form style="display: inline; margin-right: 10px;" action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
			<input type="hidden" name="cmd" value="_s-xclick">
			<input type="hidden" name="encrypted" value="-----BEGIN PKCS7-----MIIHPwYJKoZIhvcNAQcEoIIHMDCCBywCAQExggEwMIIBLAIBADCBlDCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20CAQAwDQYJKoZIhvcNAQEBBQAEgYC9EALb6WdPFciKcnODIwe5ZroGjh13ytlQoUoxpG0Fs9fexjGts46e1qvSXmIKLnXWN/zrigGXUR5OPs5r5FtfeTgNsUBgsQFmN2Liguq98zI3ZcxvsuMjofpeXXMCSuWvE+rlP4h1Q7nwcWR8bWA8WOHTUwHOaoFTFn908Hv0lTELMAkGBSsOAwIaBQAwgbwGCSqGSIb3DQEHATAUBggqhkiG9w0DBwQItFrp9VlAgDGAgZg1Z2UMBjrtN6grtc55VUnXrUYKPTDcTT4MIiOpjdVDo35xotDgFXXMROMaTeZvIZEqc7FLBZ3hq4O2fhmHBu0EyK7yc0J6VMZlWrf+lGkkSd7VaRFyjnNVkHso+/f1I2izyXjcoYnQrEb3zHgvnEwYjAEGhmQxowfPkddGAOyIWEg5x8Fyx+DpqY/VcWSatU0YFHa+Ek/Z8qCCA4cwggODMIIC7KADAgECAgEAMA0GCSqGSIb3DQEBBQUAMIGOMQswCQYDVQQGEwJVUzELMAkGA1UECBMCQ0ExFjAUBgNVBAcTDU1vdW50YWluIFZpZXcxFDASBgNVBAoTC1BheVBhbCBJbmMuMRMwEQYDVQQLFApsaXZlX2NlcnRzMREwDwYDVQQDFAhsaXZlX2FwaTEcMBoGCSqGSIb3DQEJARYNcmVAcGF5cGFsLmNvbTAeFw0wNDAyMTMxMDEzMTVaFw0zNTAyMTMxMDEzMTVaMIGOMQswCQYDVQQGEwJVUzELMAkGA1UECBMCQ0ExFjAUBgNVBAcTDU1vdW50YWluIFZpZXcxFDASBgNVBAoTC1BheVBhbCBJbmMuMRMwEQYDVQQLFApsaXZlX2NlcnRzMREwDwYDVQQDFAhsaXZlX2FwaTEcMBoGCSqGSIb3DQEJARYNcmVAcGF5cGFsLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAwUdO3fxEzEtcnI7ZKZL412XvZPugoni7i7D7prCe0AtaHTc97CYgm7NsAtJyxNLixmhLV8pyIEaiHXWAh8fPKW+R017+EmXrr9EaquPmsVvTywAAE1PMNOKqo2kl4Gxiz9zZqIajOm1fZGWcGS0f5JQ2kBqNbvbg2/Za+GJ/qwUCAwEAAaOB7jCB6zAdBgNVHQ4EFgQUlp98u8ZvF71ZP1LXChvsENZklGswgbsGA1UdIwSBszCBsIAUlp98u8ZvF71ZP1LXChvsENZklGuhgZSkgZEwgY4xCzAJBgNVBAYTAlVTMQswCQYDVQQIEwJDQTEWMBQGA1UEBxMNTW91bnRhaW4gVmlldzEUMBIGA1UEChMLUGF5UGFsIEluYy4xEzARBgNVBAsUCmxpdmVfY2VydHMxETAPBgNVBAMUCGxpdmVfYXBpMRwwGgYJKoZIhvcNAQkBFg1yZUBwYXlwYWwuY29tggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQADgYEAgV86VpqAWuXvX6Oro4qJ1tYVIT5DgWpE692Ag422H7yRIr/9j/iKG4Thia/Oflx4TdL+IFJBAyPK9v6zZNZtBgPBynXb048hsP16l2vi0k5Q2JKiPDsEfBhGI+HnxLXEaUWAcVfCsQFvd2A1sxRr67ip5y2wwBelUecP3AjJ+YcxggGaMIIBlgIBATCBlDCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20CAQAwCQYFKw4DAhoFAKBdMBgGCSqGSIb3DQEJAzELBgkqhkiG9w0BBwEwHAYJKoZIhvcNAQkFMQ8XDTE3MDExMTE2NDMxMFowIwYJKoZIhvcNAQkEMRYEFIK1E44iVp4kaKPXBIIz2/A+U4WPMA0GCSqGSIb3DQEBAQUABIGAPTWOw4VNJfaZhOtYStECFNYFq4JCdbzVboULUac8GPKr8j4gD0ToZ3DdOCtsyL/FJNllY9dVvb8+KEt4D42nCGjvUq5OsUFQVmnOhzL5Vip8YuarzxD5cVfQ7W3po7OuJRByTHCgnWqkYvAIHEB+vL6VU+dIaLO1gxAADujMVqo=-----END PKCS7-----
			">
			<input type="image" src="https://www.paypalobjects.com/pt_BR/BR/i/btn/btn_donateCC_LG.gif" border="0" name="submit" alt="PayPal - A maneira fácil e segura de enviar pagamentos online!">
			<img alt="" border="0" src="https://www.paypalobjects.com/pt_BR/i/scr/pixel.gif" width="1" height="1">
			</form>
		
				<!-- INICIO FORMULARIO BOTAO PAGSEGURO -->
			<form style="display: inline;" action="https://pagseguro.uol.com.br/checkout/v2/donation.html" method="post">
			<!-- NÃO EDITE OS COMANDOS DAS LINHAS ABAIXO -->
			<input type="hidden" name="currency" value="BRL" />
			<input type="hidden" name="receiverEmail" value="jack2_299@hotmail.com" />
			<input type="hidden" name="iot" value="button" />
			<input type="image" src="https://stc.pagseguro.uol.com.br/public/img/botoes/doacoes/209x48-doar-assina.gif" name="submit" alt="Pague com PagSeguro - é rápido, grátis e seguro!" />
			</form>
			<!-- FINAL FORMULARIO BOTAO PAGSEGURO -->
		
			<div id="paypal_USD">
		
			<br /><p style="display: inline;" class="ui-footer-p">Donation to non-Brazilians</p><br /><br />
			<form style="display: inline;" action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
				<input type="hidden" name="cmd" value="_s-xclick">
				<input type="hidden" name="encrypted" value="-----BEGIN PKCS7-----MIIHPwYJKoZIhvcNAQcEoIIHMDCCBywCAQExggEwMIIBLAIBADCBlDCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20CAQAwDQYJKoZIhvcNAQEBBQAEgYAPF0lSTjofXuUWkKcCxQ7ftFl0vEmN4gBvAFIfhSG8GqL3PuuNEnuURft1H1VuKUtlEfJnSj11Q1rq6v0zeYVSr8UGmuGdqi3ogGfYH+0OfYEu9adorJEWQezlfIdIzE1VC2rn+zOLHqVMIVm7SabIwFtc5ALFBzPRQKhkS27hmjELMAkGBSsOAwIaBQAwgbwGCSqGSIb3DQEHATAUBggqhkiG9w0DBwQIy6XqjokavJmAgZhfDqrULhMfN+eEdUCSHG0MlUKtoJFonoN71EGpCuaOIf3Mnqhh+r+y5ZD2MACB5l3z52YrMp8AFwwGJbxXzmPQxD6rxE1hNBQf7i99yVy4QNtlArq0WdC3CKC2Mf8QDs40OCXHUm0122Mln6kZT1WdeBK9k4FWH8beWP7FK44NpMB6Ak3Fif2X/vNIdHPwH/LvzUPQV4oNMaCCA4cwggODMIIC7KADAgECAgEAMA0GCSqGSIb3DQEBBQUAMIGOMQswCQYDVQQGEwJVUzELMAkGA1UECBMCQ0ExFjAUBgNVBAcTDU1vdW50YWluIFZpZXcxFDASBgNVBAoTC1BheVBhbCBJbmMuMRMwEQYDVQQLFApsaXZlX2NlcnRzMREwDwYDVQQDFAhsaXZlX2FwaTEcMBoGCSqGSIb3DQEJARYNcmVAcGF5cGFsLmNvbTAeFw0wNDAyMTMxMDEzMTVaFw0zNTAyMTMxMDEzMTVaMIGOMQswCQYDVQQGEwJVUzELMAkGA1UECBMCQ0ExFjAUBgNVBAcTDU1vdW50YWluIFZpZXcxFDASBgNVBAoTC1BheVBhbCBJbmMuMRMwEQYDVQQLFApsaXZlX2NlcnRzMREwDwYDVQQDFAhsaXZlX2FwaTEcMBoGCSqGSIb3DQEJARYNcmVAcGF5cGFsLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAwUdO3fxEzEtcnI7ZKZL412XvZPugoni7i7D7prCe0AtaHTc97CYgm7NsAtJyxNLixmhLV8pyIEaiHXWAh8fPKW+R017+EmXrr9EaquPmsVvTywAAE1PMNOKqo2kl4Gxiz9zZqIajOm1fZGWcGS0f5JQ2kBqNbvbg2/Za+GJ/qwUCAwEAAaOB7jCB6zAdBgNVHQ4EFgQUlp98u8ZvF71ZP1LXChvsENZklGswgbsGA1UdIwSBszCBsIAUlp98u8ZvF71ZP1LXChvsENZklGuhgZSkgZEwgY4xCzAJBgNVBAYTAlVTMQswCQYDVQQIEwJDQTEWMBQGA1UEBxMNTW91bnRhaW4gVmlldzEUMBIGA1UEChMLUGF5UGFsIEluYy4xEzARBgNVBAsUCmxpdmVfY2VydHMxETAPBgNVBAMUCGxpdmVfYXBpMRwwGgYJKoZIhvcNAQkBFg1yZUBwYXlwYWwuY29tggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQADgYEAgV86VpqAWuXvX6Oro4qJ1tYVIT5DgWpE692Ag422H7yRIr/9j/iKG4Thia/Oflx4TdL+IFJBAyPK9v6zZNZtBgPBynXb048hsP16l2vi0k5Q2JKiPDsEfBhGI+HnxLXEaUWAcVfCsQFvd2A1sxRr67ip5y2wwBelUecP3AjJ+YcxggGaMIIBlgIBATCBlDCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20CAQAwCQYFKw4DAhoFAKBdMBgGCSqGSIb3DQEJAzELBgkqhkiG9w0BBwEwHAYJKoZIhvcNAQkFMQ8XDTE3MTAyMTEzMzkyMFowIwYJKoZIhvcNAQkEMRYEFDTW31rA+kKVe8LwbS9Y+O72WPdgMA0GCSqGSIb3DQEBAQUABIGAcbNb4JJUfwIHxTjZjSl+zgdCZEgA9Altw3j9zv0A104JII6BAixPUQOKvPAAzl6RnA1ZfXPMoDOcFuXwYn6CTknCEWBwqv49Y5WTansNuiVEXMxuqlvUu47+s8D8egeOMts4z/24zpPIX+rlPLViNHWuCIb01qz4X00TlsqBnsw=-----END PKCS7-----">
				<input type="image" src="https://www.paypalobjects.com/pt_BR/BR/i/btn/btn_donateCC_LG.gif" border="0" name="submit" alt="PayPal - A maneira fácil e segura de enviar pagamentos online!">
				<img alt="" border="0" src="https://www.paypalobjects.com/pt_BR/i/scr/pixel.gif" width="1" height="1">
			</form>
			</div>';

		echo '</article>';
	}

	public function changeip_view() {

		echo '<html lang="PT-BR">
				<head>
					<meta charset="UTF-8" />
					<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
					<link rel="icon" href="/favicon.ico" type="image/x-icon">
					<title>Change IP</title>
				</head>
				<body>';

		if ($this->logado) {
			if ($this->msg == 0 && $this->Error == 0) {
				$this->params->clear();
				$this->params->add('i', $this->uid);

				$ip = '';

				$query = '';
				
				if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'SELECT ip FROM '.$this->db->con_dados['DB_NAME'].'.pangya_player_ip WHERE UID = ?';
				else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'SELECT ip FROM '.$this->db->con_dados['DB_NAME'].'.pangya_player_ip WHERE UID = ?';
				else
					$query = 'SELECT ip FROM pangya_player_ip WHERE UID = ?';

				if ($result = $this->db->db->execPreparedStmt($query, $this->params->get()))
					if ($result->num_rows > 0 && ($row = $result->fetch_assoc()) && isset($row['ip']))
						$ip = $row['ip'];

				echo '<form action="changeip.php" method="POST" style="margin: 20% 27%;">
					  <h5 style="color: drakgreen;">Só pode trocar o IP 3 vezes no mesmo dia</h5>
					 IP: <input type="text" name="ip" style="border: 1px solid black;" pattern="^([0-9]{1,3}[.]{1}){3}[0-9]{1,3}$" required placeholder="IP Address" value="'.$ip.'" title="Ex: 200.200.200.5" autofocus>
					 <input type="hidden" name="uid" value="'.$this->uid.'">
					 <input type="submit" value="Trocar IP" width="100%" style="border-radius: 3px; border: 1px solid black;">
					 </form>';
			}else {
				$str_msg = '';

				switch ($this->msg) {
					case 1: // Trocou com sucesso
						$str_msg = 'IP address trocado com sucesso.';
						break;
					case 2: // Já trocou o ip 3 vezes no mesmo dia
						$str_msg = 'Você já trocou o IP 3 vezes hoje, tente trocar ele amanhã.';
						break;
					case 5:
					default:
						$str_msg = 'Erro ao trocar o IP address.';
						break;
				}

				echo "<script>window.addEventListener('load', function() {alert('".$str_msg."'); window.close();});</script>";
			}


			echo '</body></html>';
		}else
			echo "<script>window.open('pangya.php', '_self')</script>";
	}

	public function change_nickname_view() {

		include_once('recaptcha.inc');

		echo '<html lang="PT-BR">
				<head>
					<meta charset="UTF-8" />
					<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
					<link rel="icon" href="/favicon.ico" type="image/x-icon">
					<title>Change Nickname</title>
					'.reCAPTCHA::loadScript_V3().'
			<style>
				body, head, html, form {
					border: none;
					padding: 0;
					margin: 0;
				}
			</style></head><body>';

		if ($this->logado) {
			
			if ($this->msg == 0 && $this->Error == 0) {

				echo '<form '.reCAPTCHA::makeAttrEventOnSubmitCheck_V3('changenick').' action="change_nickname.php" method="POST">
					  	<table width="500px" height="400px" cellpadding="0" cellspacing="10" align="center" valign="center" border="0">
						 	<tr valign="center" align="center">
						  		<td padding="20" colspan="2" align="left">';

				// Texto
				echo '				<span style="font-size: 15; line-height: 20px">
										<h3>Troca de Nickname</h3>
										- Para trocar o nickname vai custar 60 Cookie Point(CP).<br>
										- Depois que você confirmar que quer trocar o nickname, <br>
											 se estiver online no jogo relogue para poder trocar o nickname.<br>
										- Assim que você fizer o login no jogo, vai pedir para trocar o nickname.<br>
										<br>
										<br>
										<i style="color: green; font-size: 18">
											* Nickname só pode ser trocado 1 vez por mês
										</i>
									</span>';

				echo '			</td>
							</tr>';

				// Dica para verificar o comprimeiro do nickname
				echo '		<tr>
								<td padding="20" colspan="2" align="left">
									Você pode verificar o comprimento do nickname nesse <a href="check_nickname.php" target="_blank">link</a>.<br>
									Para não ocorrer nenhum bug do cliente na hora de escolher o nickname.
								</td>
							</tr>';


				// Inputs
				echo '		<tr height="100px">	
								<td align="center">
									<div style="display: block; margin-bottom: 20px; margin-top: 20px">
										<input type="checkbox" name="change_nickname" style="border: 1px solid black;" autofocus> Quero trocar o nickname
									</div>

									<input type="hidden" name="uid" value="'.$this->uid.'">
								</td>
								<td>
									<input type="submit" value="Trocar Nickname" width="100%" style="border: 1px solid black;">
								</td>
							</tr>
						</table>
					</form>';

			}else {
				$str_msg = '';

				switch ($this->msg) {
					case 1: // Trocou com sucesso
						$str_msg = 'Pedido de troca de nickname feito com sucesso.';
						break;
					case 2: // Ainda não deu um mês da ultima vez que trocou o nickname
						$str_msg = 'Ainda não completou um mês desde a ultima vez que você trocou o nickname.';
						break;
					case 3:	// Não tem CP suficiente para troca o nickname
						$str_msg = 'Não tem Cookie Point(CP) suficiente para trocar o nickname.';
						break;
					case 4: // Já pediu para trocar o login, faça o login no joga para trocar o nickname
						$str_msg = 'Você já fez o pedido para trocar o nickname. Faça o login no jogo para trocar o nickname.';
						break;
					case 15:	// Erro no reCAPTCHA
						$str_msg = 'Erro no reCAPTCHA';
						break;
					case 5:
					default:
						$str_msg = 'Erro ao trocar o Nickname.';
						break;
				}

				echo "<script>window.addEventListener('load', function() {alert('".$str_msg."'); window.close();});</script>";
			}


			echo '</body></html>';
		}else
			echo "<script>window.open('pangya.php', '_self')</script>";
	}

	public function change_password_view() {

		include_once('recaptcha.inc');

		echo '<html lang="PT-BR">
				<head>
					<meta charset="UTF-8" />
					<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
					<link rel="icon" href="/favicon.ico" type="image/x-icon">
					<title>Change Password</title>
					'.reCAPTCHA::loadScript_V3().'
			<style>
				body, head, html, form {
					border: none;
					padding: 0;
					margin: 0;
				}
			</style></head><body>';

		if ($this->logado) {
			
			if ($this->msg != 0 || $this->Error != 0) {
				
				$str_msg = '';

				switch ($this->msg) {
					case 1: // Trocou a senha com sucesso
						$str_msg = 'Trocou a senha com sucesso.';
						break;
					case 2: // Já trocou a senha 2 vezes em 24 horas
						$str_msg = 'Você já trocou a senha duas vezes em 24 horas.';
						break;
					case 3:	// Nova senha é diferente
						$str_msg = 'A senha nova e a confirmação são diferentes.';
						break;
					case 4:	// Senha atual está diferente
						$str_msg = 'A senha atual está errada.';
						break;
					case 5:	// Dados invalidos
						$str_msg = 'Dados inválidos.';
						break;
					case 15:	// Erro no reCAPTCHA
						$str_msg = 'Erro no reCAPTCHA';
						break;
					case 6:
					default:
						$str_msg = 'Erro ao trocar a senha.';
						break;
				}

				echo "<script>window.addEventListener('load', function() {alert('".$str_msg."'); ".($this->msg == 2 || $this->msg == 1 || $this->msg >= 6 || $this->Error != 0 ? "window.close();" : "")."});</script>";
			}

			if ($this->Error == 0 && ($this->msg == 0 || $this->msg == 3 || $this->msg == 4 || $this->msg == 5)) {

				echo '<form '.reCAPTCHA::makeAttrEventOnSubmitCheck_V3('changepass').' action="change_password.php" method="POST">
					  	<table width="500px" height="400px" cellpadding="0" cellspacing="10" align="center" valign="center" border="0">
						 	<tr height="150px" valign="center" align="center">
						  		<td padding="20" colspan="3" align="left">';

				// Texto
				echo '				<span style="font-size: 15; line-height: 20px">
										<h3>Troca de senha</h3>
										<i style="color: green; font-size: 18">
											* A senha só pode ser trocada 2 vez a cada 24 horas, contando da primeira vez que foi trocado
										</i>
									</span>';

				echo '			</td>
							</tr>';

				// Inputs
				echo '		</tr>
							<tr height="25" align="left">	
								<td>
									Senha atual: 
								</td>
								<td>
									<input name="pwd_old" type="password" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required>
								</td>
							</tr>
							<tr height="25" align="left">	
								<td>
									Nova senha: 
								</td>
								<td>
									<input name="pwd_new" type="password" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required>
								</td>
							</tr>
							<tr height="25" align="left">	
								<td>
									Confirma nova senha: 
								</td>
								<td>
									<input name="pwd_new_confirm" type="password" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required>
								</td>
							</tr>
							<tr height="2">
								<td colspan="3">
									<input type="hidden" name="uid" value="'.$this->uid.'">
								</td>
							</tr>
							<tr height="50" align="center" valign="top">
								<td colspan="3">
									<input type="submit" value="Trocar Senha" width="100%" style="border: 1px solid black;">
								</td>
							</tr>
							<tr>
								<td></td>
							</tr>
						</table>
					</form>';

			}

			echo '</body></html>';
		}else
			echo "<script>window.open('pangya.php', '_self')</script>";
	}

	public function rescue_password_view() {

		$type = isset($this->argumentos['get']['rescue']) ? $this->argumentos['get']['rescue'] : 1;

		$content = ($type == 2 ? 'o ID' : 'a Senha');

		include_once('recaptcha.inc');

		echo '<!DOCTYPE html>
				<html lang="PT-BR">
				<head>
					<meta charset="UTF-8" />
					<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
					<link rel="icon" href="/favicon.ico" type="image/x-icon">
					<title>Recuperar '.$content.'</title>
					'.reCAPTCHA::loadScript_V2().'
					<style>
						body, head, html, form {
							border: none;
							padding: 0;
							margin: 0;
						}
					</style>
				</head>
				<body>';

		// Camufla o email se ele não for vazio
		if ($this->email != '') {

			include_once('config/base.inc');

			$before = before_last('@', $this->email);
			$last = after_last('@', $this->email);

			$count = strlen($before);

			if ($count > 5) {

				$before = preg_replace('/(^[a-z0-9._%+-]{3})(.*)/', '$1'.str_repeat('*', $count-3), $before);

			}else if ($count > 3) {

				$before = preg_replace('/(^[a-z0-9._%+-]{2})(.*)/', '$1'.str_repeat('*', $count-2), $before);

			}else if ($count > 1) {

				$before = preg_replace('/(^[a-z0-9._%+-]{1})(.*)/', '$1'.str_repeat('*', $count-1), $before);
			}

			$this->email = $before.'@'.$last;
		}

			
		if ($this->msg != 0 || $this->Error != 0) {
			
			$str_msg = '';

			switch ($this->msg) {
				case 1: // Um email foi eviado para o endereço de email com as instrução para recuparar a senha ou id
					$str_msg = 'Um email foi eviado para o endereço de email: <span style="color: red">'.$this->email.'</span> com as instrução de recuparar '.$content;
					break;
				case 2: // Esse id não existe
					$str_msg = 'Esse id não existe.';
					break;
				case 3:	// Esse endereço de email não existe
					$str_msg = ($type == 2 ? 'Esse endereço de email não existe.' : 'Esse id não tem um endereço de email válido.');
					break;
				case 4:	// Já foi enviado um mail para o endereço de email da conta
					$str_msg = 'Já foi enviado um mail com as instrução de recuperação '.($type == 2 ? 'do id' : 'da senha').' para o endereço de email da conta. Verifique o lixo eletrônico ou spam se não estiver na caixa de entrada. Ele pode demorar um pouco.';
					break;
				case 5:	// Dados invalidos
					$str_msg = 'Dados inválidos.';
					break;
				case 6:	// Não conseguiu enviar o email
					$str_msg = 'Não conseguiu enviar o mail para o endereço de email: '.$this->email;
					break;
				case 7:	// Esse conta não tem um endereço de email valido
					$str_msg = 'Esse conta não tem um endereço de email válido';
					break;
				case 15:	// Erro no desafio do reCAPTCHA
					$str_msg = 'Erro no desafio do reCAPTCHA';
					break;
				case 8:
				default:
					$str_msg = 'Erro desconhecido. Code('.$this->msg.').';
					break;
			}

			if ($this->msg != 1)
				echo "<script>window.addEventListener('load', function() { alert('".$str_msg."'); });</script>";
		}

		if ($this->Error == 0 && $this->msg != 1) {

			include_once('recaptcha.inc');

			// reCAPTCHA do google evento que verifica se o desafio foi feito
			echo '<form '.reCAPTCHA::makeAttrEventOnSubmitCheck_V2().' action="rescue_password.php?rescue='.($type == 2 ? 2 : 1).'" method="POST">
					<table width="500px" height="400px" cellpadding="0" cellspacing="10" align="center" valign="center" border="0">
						<tr height="120px" valign="center" align="center">
							<td padding="20" colspan="3" align="left">';

			// Texto
			echo '				<span style="font-size: 15; line-height: 20px">
									<h3>Recuperar '.$content.'</h3>
								</span>';

			echo '			</td>
						</tr>';

			// Inputs
			echo '<tr height="25" align="left">';
			
			if ($type == 2) {

				echo '		<td>
								E-mail: 
							</td>
							<td>
								<input name="email_rescue" type="email" pattern="^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$" required>
							</td>';

			}else {
			
				echo '		<td>
								ID: 
							</td>
							<td>
								<input name="id_rescue" type="text" pattern="^[a-zA-Z0-9]{4,16}$" title="de 4-16 caracters e não espaços" required>
							</td>';
			}

			// reCAPTCHA do google My Key
			
			echo '		</tr>
						
						<tr height="100">
							<td align="center" colspan="2">
								<input type="hidden" name="rescue" value="true">
								'.reCAPTCHA::makeCheckBox_V2().'
							</td>
						</tr>
						<tr height="50" align="center" valign="top">
							<td>
								<a href="?rescue='.($type == 2 ? 1 : 2).'">Esqueceu '.($type == 2 ? 'a Senha' : 'o ID').'?</a>
							</td>
							<td colspan="2" align="left">
								<input type="submit" value="Recuperar '.$content.'" width="100%" style="border: 1px solid black;">
							</td>
						</tr>
						<tr>
							<td></td>
						</tr>
					</table>
				</form>';

		}else if ($this->msg == 1) {

			$str_msg = 'Um email foi eviado para o endereço de email: <span style="color: red">'.$this->email.'</span><br> com as instrução para recuparar '.$content.'.';

			echo '	<table width="550px" height="400px" cellpadding="0" cellspacing="10" align="center" valign="center" border="0">
						<tr align="center">
							<td>';
			
			echo '				<span style="font-size: 18px">
									<i>'.$str_msg.'</i>
									<br>
									<br>
									Verifique o lixo eletrônico ou spam se não estiver na caixa de entrada. Ele pode demorar um pouco.
								</span>';

			echo '			</td>
						</tr>
						<tr height="100px" valign="top">
							<td align="center">
								<a href="#" onclick="javascript:window.close()" style="text-decoration: underline">Voltar para página home</a>
							</td>
						</tr>
					</table>';
		}

		echo '</body></html>';
	}

	private function finish_rescue_password_view() {

		echo '<article id="home">';

		if ($this->msg != 0 && $this->msg != 1) {
			$str_msg = '';

			switch ($this->msg) {
				case 2:
					$str_msg = "Já trocou a senha duas vezes em 24 horas.";
					break;
				case 4:
					$str_msg = "As senhas estão diferentes.";
					break;
				case 11:
					$str_msg = "Dados inválidos.";
					break;
				case 100:
					$str_msg = "Redefiniu a senha com sucesso.";
					break;
				case 102:
					$str_msg = "Link desconhecido.";
					break;
				case 103:
					$str_msg = "Error nao consulta com o banco de dados.";
					break;
				case 104:
					$str_msg = "Chave inválida.";
					break;
				case 6:
				default:
					$str_msg = "Erro desconhecido. Code(".$this->msg.")";
					break;
			}

			echo "<script>window.addEventListener('load', function() {alert('".$str_msg."');";

			if ($this->msg == 100 || $this->msg == 104 || $this->msg == 102)
				echo "window.open('pangya.php', '_self');";

			echo "});</script>";
		}

		if (isset($_SESSION['player_rescue']) && isset($_SESSION['player_rescue']['index']) && isset($_SESSION['player_rescue']['key']) 
			&& isset($_SESSION['player_rescue']['UID']) && isset($_SESSION['player_rescue']['TIPO'])) {

			// Redefinição de senha
			echo '<style>
					#fo_ca {
						width: 100%;
						display: block;
						margin 3% auto;
					}
					#fo_ca > * {
						margin: 1% 0;
					}
					#fo_ca input {
						border: 1px solid black;
					}
					#fo_ca div > * {
						display: inline-block;
						width: 20%
					}
					#fo_ca label {
						text-align: left;
					}
					.la_ca {
						display: block;
					}
					</style>';
			
			if ($_SESSION['player_rescue']['TIPO'] == 1) {

				echo '<h3>Redefinição da senha</h3>';

				echo '<form action="pangya.php?link=9&index='.$_SESSION['player_rescue']['index'].'&key='.$_SESSION['player_rescue']['key'].'" method="POST" id="fo_ca">
						<div><label class="la_ca">Nova senha: </label><input type="password" name="rescue_pass" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required></div>
						<div><label class="la_ca">Confirma nova senha: </label><input type="password" name="rescue_confirm_pass" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required></div>
						<div>
							<input type="hidden" name="uid" value="'.$_SESSION['player_rescue']['UID'].'">
							<input type="hidden" name="index" value="'.$_SESSION['player_rescue']['index'].'">
						</div>';

				echo '<input type="submit" value="Redefinir">
					</form>';
			
			}else {

				// Faz a pesquisa no banco de dados e retorno o ID e NICK do player que esqueceu o ID
				$player_rescue = ['id' => 'unknown', 'nick' => 'unknown'];


				$this->params->clear();
				$this->params->add('i', $_SESSION['player_rescue']['UID']);

				$query = '';

				if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'SELECT id, nick FROM '.$this->db->con_dados['DB_NAME'].'.account WHERE [UID] = ?';
				else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'SELECT id, nick FROM '.$this->db->con_dados['DB_NAME'].'.account WHERE UID = ?';
				else
					$query = 'SELECT id, nick FROM account WHERE `UID` = ?';

				if (($result = $this->db->db->execPreparedStmt($query, $this->params->get())) != null && $this->db->db->getLastError() == 0) {

					if (($row = $result->fetch_assoc()) && isset($row['id']) && isset($row['nick'])) {
						
						$player_rescue['id'] = $row['id'];
						$player_rescue['nick'] = mb_convert_encoding($row['nick'], 'UTF-8', "SJIS");
					
					}else
						$this->msg = 202;
				
				}else
					$this->msg = 201;

				if ($this->msg != 0) {
					
					echo '<h3>Reperação do ID</h3>';
					
				echo '	<table width="100%" cellspacing="0" cellpadding="0" border="0" style="margin-top: 10px">
							<tr>
								<td align="center">
									<table cellspacing="0" cellpadding="0" border="0">
										<tr>
											<td align="left">
												Seu ID: &nbsp;&nbsp;
											</td>
											<td align="left">
												'.$player_rescue['id'].'
											</td>
										</tr>
										<tr>
											<td align="left">
												Seu Nickname: &nbsp;&nbsp;
											</td>
											<td align="left">
												'.htmlspecialchars($player_rescue['nick']).'
											</td>
										</tr>
									</table>
								</td>
							</tr>
						</table>';

				}else {
					
					echo "<script>window.addEventListener('load', function() {alert('Error na resposta do banco de dados. Code(".$this->msg.")');";
				}
			}

		}else if (!($this->msg == 100 || $this->msg == 104 || $this->msg == 102))
			echo "<script>window.open('pangya.php', '_self')</script>";

		echo '</article>';
	}

	private function my_donation_view() {
		
		if (!$this->logado)
			echo "<script>window.addEventListener('load', function() {
				alert('Por favor logue primeiro.');
				window.open('pangya.php', '_self');
			});</script>";
		else {

			echo '<style>
					#div_dona {
						margin: 2% auto;
						display: flex;
						justify-content: center;
					}
					#div_dona > * {
						text-align: left;
					}
					.dona-table {
						border-collapse: collapse;
					}
					.dona-table td, th {
						padding: 3px 20px;
						text-align: center;
					}
					.dona-table tr:first-child {
						padding-bottom: 100px;
					}
					.dona-table tr:not(:first-child) {
						border-bottom: 1px solid black;
					}
			     </style>';

			echo '<article id="home">';

			echo '<h3>Minhas doações</h3>';

			echo '<div id="div_dona">';

			$lambda = function($player) {

				if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'exec '.$this->db->con_dados['DB_NAME'].'.ProcGetAllDonationAndEpinByPlayer ?';
				else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'select _code_ as code, _plataforma_ as plataforma, _date_ as date, _gross_amount_ as gross_amount, _status_ as status, _epin_ as epin, _cp_qntd_ as cp_qntd, _epin_valid_ as epin_valid from '.$this->db->con_dados['DB_NAME'].'.ProcGetAllDonationAndEpinByPlayer(?)';
				else
					$query = 'call '.$this->db->con_dados['DB_NAME'].'.ProcGetAllDonationAndEpinByPlayer(?)';

				$player->params->clear();
				$player->params->add('i', $player->uid);

				if (($result = $player->db->db->execPreparedStmt($query, $player->params->get())) == null && $player->db->db->getLastError() != 0) {

					error_log("Player[UID=".$player->uid."] Fail in exec proc get all donation and epin by player. Error code: ".$player->db->db->getLastError(), 0);

					return '<p style="text-align: center">Não conseguiu pegar as suas doações deu erro no sistema. Code: 1000</p>';
				}

				if ($result == null || ($row = $result->fetch_assoc()) == null) {

					//error_log("Player[UID=".$player->uid."] Fail in proc get all donation and epin by player, result empty.", 0);

					return '<p style="text-align: center">Você não tem nenhum doação.</p>';
				}

				if (!isset($row['code']) || !isset($row['plataforma']) || !isset($row['date']) || !isset($row['gross_amount']) || !isset($row['status'])
					|| !key_exists('epin', $row) || !key_exists('cp_qntd', $row) || !key_exists('epin_valid', $row) || $row['code'] == -1) {

					error_log("Player[UID=".$player->uid."] Fail in proc get all donation end epin by player, result invalid.", 0);

					return '<p style="text-align: center">Não conseguiu pegar as suas doações deu erro no sistema. Code: 1001</p>';
				}

				$str = '<table align="center" class="dona-table">
							<tr>
								<th>Status</th>
								<th>Plataforma</th>
								<th>Date</th>
								<th>Transaction code</th>
								<th>Valor</th>
								<th>E-Pin</th>
								<th>CP Quantidade</th>
								<th>E-Pin Status</th>
							</tr>';

				do {

					$str .= '<tr>';
					
					$str .= '<td>'.(function() use($row) {

						$status = 'Desconhecido';

						switch  ($row['status']) {
							case 1:
								$status = 'Aguardando pagamento';
							break;
							case 2:
								$status = 'Em analise';
							break;
							case 3:
							case 4:
								$status = 'Pago';
							break;
							case 7:
							case 6:
								$status = 'Cancelado';
							break;
							case 5:
							case 8:
								$status = 'Aguardando liberação';
							break;
						}

						return $status;
					})().'</td>';

					$str .= '<td>'.($row['plataforma'] == 1 ? 'PayPal' : ($row['plataforma'] == 2 ? 'PagSeguro' : 'Desconhecido')).'</td>';

					$str .= '<td>'.(new DateTime($row['date']))->format('Y-m-d H:i').'</td>';

					$str .= '<td>'.$row['code'].'</td>';

					$str .= '<td>'.$row['gross_amount'].'</td>';

					if ($row['epin'] == '' || $row['epin'] == null)
						$str .= '<td colspan="3">'.($row['status'] == 6 || $row['status'] == 7 ? 'Doação cancelada' : 'Aguardando a doção ficar em status "Pago" para gerar o E-Pin').'</td>';
					else {

						$str .= '<td>'.$row['epin'].'</td>';

						$str .= '<td>'.$row['cp_qntd'].'</td>';

						$str .= '<td>'.($row['epin_valid'] == 1 ? 'Válida' : 'Usado').'</td>';
					}

				} while (($row = $result->fetch_assoc()) != null);

				$str .= '</table>';

				return $str;
			};

			echo $lambda($this);

			echo '</div>';

			echo '</article>';
		}
	}

	private function login_view() {

		if ($this->logado) {
			echo '<style type="text/css">
						#log_out {
							border: 2px linear-gradient(darkblue, lightblue);
							background: linear-gradient(white, #838383);
							border-radius: 3px;
							width: 96%;
							margin: 2% 2% 2% 2%;
						}
						.las {
							display: block; 
							margin: 2% 2% 2% 2%;
						}
					</style>';

			printf('<div style="width: 100%%; display: block; background: #a4d4e6; border-radius: 5px">
					<label class="las">Nick: %s</label>
					<label class="las">Level: <img src="../img/level/level_%03d.png"></label>', htmlspecialchars($this->nick), (($this->level < 0) ? 000 : ($this->level+1)));

			// Moedas
			echo '<label class="las">Pang: '.$this->moeda['pang'].'</label>
				  <label class="las">Cookie: '.$this->moeda['cookie'].'</label>';

			// Troca de nick
			echo '<a class="las" href="#" onclick="{ var win = window.open(\'change_nickname.php?changenickname=1\', \'Change Nickname\', \'width=600,height=400\');
							var timer = setInterval(function() {
								if (win.closed) {
									clearInterval(timer);
									window.location.href = window.location.protocol + \'//\' + window.location.host + window.location.pathname + window.location.search
								}
							}, 1000);
						}" style="text-decoration: underline"> Change Nickname</a>';

			// Troca de senha
			echo '<a class="las" href="#" onclick="javascript:window.open(\'change_password.php?changepwd=1\', \'Change Password\', \'width=600,height=400\')" style="text-decoration: underline"> Change Password</a>';

			// Verificar o comprimento do nickname
			echo '<a class="las" href="#" onclick="javascript:window.open(\'check_nickname.php\', \'Check nickname\', \'width=700,height=500\')" style="text-decoration: underline"> Check nickname</a>';

			// Mostra as doações do player
			echo '<a class="las" href="pangya.php?link=10" style="text-decoration: underline">Minhas doações</a>';

			//echo '<a class="las" href="#" onclick="javascript:window.open(\'changeip.php?changeip=1\', \'Change IP\', \'width=600,height=400\')" style="text-decoration: underline">Change IP Address</a>';

			if (($this->cap & 4) && ($this->cap & 8) /*&& ($this->ip_addr != '' && ($this->ip_addr == '127.0.0.1' || $this->ip_addr == '192.168.0.101'))*/) {
				echo '<a class="las" href="pangya.php?adm=1" style="text-decoration: underline">Go adm page</a>';
				//echo '<a class="las" href="pangya.php?bug_report_view=1" style="text-decoration: underline">Bug Report Viewer</a>';
				//echo '<a class="las" href="pangya.php?pincode_view=1" style="text-decoration: underline">Pin Code Viewer</a>';

				echo '<a class="las" href="pangya.php?guild=1" style="text-decoration: underline">Go Guild manager page</a>';
				echo '<a class="las" href="pangya.php?notice_manager=1" style="text-decoration: underline">Go Notice manager page</a>';
				echo '<a class="las" href="gm_tools/stream_monitor.php" style="text-decoration: underline">Go Stream Monitor</a>';
				echo '<a class="las" href="gm_tools/block_word_register.php" style="text-decoration: underline">Go Block Word Register</a>';
			}

			echo '<form action="pangya.php" method="POST" id="post_logout">
						<input type="hidden" name="logout" value="1">
					</form>
					<button onclick="document.getElementById(\'post_logout\').submit()" id="log_out">Logout</button>
			      </div>';
		}else {
			if ($this->Error != 0)
				//echo "Erro ao logar.";
				echo "<script>window.addEventListener('load', function() { alert('Erro ao logar.'); } );</script>";

			include_once('recaptcha.inc');

			echo '<form '.reCAPTCHA::makeAttrEventOnSubmitCheck_V3('login').' action="pangya.php" id="fo" method="POST">
				  <div id="inputd">
				  	<input type="text" placeholder="ID" pattern="^[a-zA-Z0-9]{3,16}$" title="de 2-16 caracters e não espaços" required name="id">
				  	<input type="password" placeholder="password" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" title="de 6-16 caracters e não espaços" required name="key">
					<input type="text" value="-1" name="level" style="display:none;">
				  </div>
				  <input type="submit" id="logar" value="logar">
				 </form>
				 <table width="100%" cellspacing="0" cellpadding="0" border="0">
				 <tr>
				 	<td style="border: 0">
				 		<a href="#" onclick="javascript:window.open(\'rescue_password.php?rescue=1\', \'Rescue Password\', \'width=600,height=400\')" style="font-size: 15px; text-decoration: underline; margin-top: 5px">Esqueceu a senha?</a>
					</td>
					<td align="right" style="border: 0">
						<a href="#" onclick="javascript:window.open(\'check_nickname.php\', \'Check nickname\', \'width=700,height=500\')" style="font-size: 15px; text-decoration: underline; margin-top: 5px">Verificar nickname</a>
					</td>
				 </tr>
				 </table>';
		}
	}

	private function server_status_view() {

		// CSS
		echo "<style>
				.list_player {
					display: none;
				}
				.seta li {
					display: flex;
					margin-top: 1%;
				}
				.seta f {
					display: block;
					width: 100%;
				}
				.seta ul, nav {
					display: block;
					margin-left: 5%;
					margin-top: 3%;
				}
				label j {
					margin-right: 1px;
				}
				label x {
					visibility: hidden;
				}
				label j, x {
					display: inline;
					position: relative;
					top: 2px;
					margin-left: 2px;
					transition: transform 0.3s linear;
				}
				label j:before {
					content: '\\2228';
				}
				label x:before {
					content: '\\2205';
				}
				.seta input:checked ~ label j {
					-webkit-transform: rotate(-180deg);
					-moz-transform: rotate(-180deg);
					-ms-transform: rotate(-180deg);
					-o-transform: rotate(-180deg);
					transform: rotate(-180deg);
					position: relative;
					top: 5px;
				}
				.seta input:checked ~ nav.list_player {
					display: block;
					animation: fade 0.5s linear forwards;
				}
				@keyframes fade {
					from {
						opacity: 0;
						visibility: hidden;
					}
					to { 
						opacity: 1;
						visibility: visible;
					}
				}
			  </style>";

		$span = ['<div id="offst">OFF</div>', '<div id="onst">ON</div>'];

		$login_st = null;
		$server_st = null;
		$player_list = [];

		$query = '';
		
		if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
			$query = 'SELECT TOP(1) [name], CASE WHEN UpdateTime > DATEADD(SECOND, -12, getdate()) THEN 1 ELSE 0 END as [status] FROM '.$this->db->con_dados['DB_NAME'].'.pangya_server_list WHERE [type] = 0 ORDER BY UpdateTime DESC';
		else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
			$query = "SELECT name, CASE WHEN UpdateTime > (now() + '-12 second'::interval) THEN 1 ELSE 0 END as status FROM ".$this->db->con_dados['DB_NAME'].'.pangya_server_list WHERE type = 0 ORDER BY UpdateTime DESC LIMIT 1';
		else
			$query = 'SELECT name, IF(UpdateTime > TIMESTAMPADD(SECOND, -12, NOW()), 1, 0) as status FROM pangya_server_list WHERE `type` = 0 ORDER BY UpdateTime DESC LIMIT 1';

		if ($result =$this->db->db->execPreparedStmt($query, NULL, 1)) {
			if ($result->num_rows > 0 && ($row = $result->fetch_assoc()) && isset($row['name']))
				$login_st = ['status' => $row['status'], 'name' => $row['name']];
		}

		$query = '';
		
		if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
			$query = 'SELECT [UID], [name], CASE WHEN UpdateTime > DATEADD(SECOND, -5, getdate()) THEN 1 ELSE 0 END as [status], property FROM '.$this->db->con_dados['DB_NAME'].'.pangya_server_list WHERE [type] = 1';
		else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
			$query = "SELECT UID, name, CASE WHEN UpdateTime > (now() + '-5 second'::interval) THEN 1 ELSE 0 END as status, property FROM ".$this->db->con_dados['DB_NAME'].'.pangya_server_list WHERE type = 1';
		else
			$query = 'SELECT uid, name, IF(UpdateTime > TIMESTAMPADD(SECOND, -5, NOW()), 1, 0) as status, property FROM pangya_server_list WHERE `type` = 1';

		if (($result = $this->db->db->execPreparedStmt($query, NULL, 1)) != null && $this->db->db->getLastError() == 0) {
			while(($row = $result->fetch_assoc()) != null)  {
				if( isset($row['name']))
					$server_st[] = ['uid' => $row['uid'], 'status' => $row['status'], 'name' => $row['name'], 'property' => $row['property']];
			}
		}

		// Load list of player online on Server
		if ($server_st != null) {
			
			$query = '';

			if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
				$query = 'SELECT a.[nick], a.capability, b.[level] FROM '.$this->db->con_dados['DB_NAME'].'.account a, '.$this->db->con_dados['DB_NAME'].'.user_info b, '.$this->db->con_dados['DB_NAME'].'.pangya_server_list c WHERE c.[UID] = ? AND a.[UID] = b.[UID] AND a.game_server_id = c.[UID] AND a.logon = 1';
			else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
				$query = 'SELECT a.nick, a.capability, b.level FROM '.$this->db->con_dados['DB_NAME'].'.account a, '.$this->db->con_dados['DB_NAME'].'.user_info b, '.$this->db->con_dados['DB_NAME'].'.pangya_server_list c WHERE c.UID = ? AND a.UID = b.UID AND a.game_server_id::bigint = c.UID AND a.logon = 1';
			else
				$query = 'SELECT a.nick, a.capability, b.level FROM account a, user_info b, pangya_server_list c WHERE c.UID = ? AND a.UID = b.UID AND a.game_server_id = c.UID AND a.logon = 1';
			
			foreach ($server_st as $key => $value) {

				$this->params->clear();
				$this->params->add('i', $value['uid']);

				if ($result = $this->db->db->execPreparedStmt($query, $this->params->get())) {
					if ($result->num_rows > 0) {
						while ($row = $result->fetch_assoc()) {
							if (isset($row['nick']))
								$player_list[$value['uid']][] = ['nick' => (isset($row['nick']) ? htmlspecialchars(mb_convert_encoding($row['nick'], 'UTF-8', 'SJIS')) : 'unknown'), 'cap' => $row['capability'], 'level' => $row['level']];
						}
					}
				}
			}
		}

		echo '<div id="status">
				<span style="text-align: center;">SERVER STATUS</span>';
		echo '<label style="display: flex;"><x class="i-seta"></x><div id="aaa1"><label class="lab1"> LOGIN SERVER:</label>'.(($login_st == null) ? $span[0] : $span[$login_st['status']]).'</div></label>';

		if ($server_st != null) {
			foreach ($server_st as $key => $value)
				if (!($value['property'] & 16)/*Mantle só gm ou player que tem a cap de ver mantle pode ver esse server*/ 
					|| ((($this->cap & 4) || (($this->cap & 1024) == 1024)))) {
						
						$player_list_exist = array_key_exists($value['uid'], $player_list);

						if ($player_list_exist && (count($player_list[$value['uid']]) != 1 || (!($player_list[$value['uid']][0]['cap'] & 4) || ($this->cap & 4)))) {

							$rash = str_replace(' ', '', $value['name']).rand().rand();

							echo '<div class="seta">';

							echo '<input id="seta-'.$rash.'" type="checkbox" style="display: none;"><label for="seta-'.$rash.'" style="display: flex;"><j class="i-seta"></j>';
						
						}else
							echo '<label style="display: flex;"><x class="i-seta"></x>';

						echo '<div id="aaa1">';	// Abre div server
						echo '<label class="lab1">'.$value['name'].' SERVER:';

						// Quantidade Players online
						if ($player_list_exist && (count($player_list[$value['uid']]) != 1 || (!($player_list[$value['uid']][0]['cap'] & 4) || ($this->cap & 4))))
							echo ' '.count($player_list[$value['uid']]);

						echo '</label>'.$span[$value['status']];
						echo '</div>';	// Fecha div server
						
						if ($player_list_exist && (count($player_list[$value['uid']]) != 1 || (!($player_list[$value['uid']][0]['cap'] & 4) || ($this->cap & 4)))) {
							
							echo '</label>';	// Fecha label seta
							echo '<nav class="list_player"><ul>';	// Abre nav, ul
							
							foreach ($player_list[$value['uid']] as $p_key => $player) {
								
								if (!($player['cap'] & 4) || ($this->cap & 4)) {

									echo '<li>'; // Abre li

									echo '<f>'.sprintf('<img src="../img/level/level_%03d.png">', (($player['level'] < 0) ? 000 : ($player['level'] + 1))).'</f>';
									echo '<f>'.$player['nick'].'</f>';
									
									echo '</li>';	// Fecha li
								}
							}

							echo '</ul></nav>';	// Fecha ul, nav
							echo '</div>';	// Fecha Div seta
						
						}else
							echo '</label>';
				}
		}else
			echo '<div id="aaa1"><label class="lab1">GAME SERVER:</label>'.$span[0].'</div>';
		
		echo '</div>';
	}

	private function home_view() {
		echo '<article id="home">';

		echo '<style>
				.p1, .p2 {
					border: 0 !important;
					margin: 1% 1% 1% 1%;
					white-space: nowrap;
					display: inline-block !important;
					width: 100%;
				}
				.p2 {
					margin: 0 2% 0 1%;
					text-align: right;
				}
				.conteudo table {
					width: 100%;
				}
				.conteudo table .p1 {
					width: 45% !important;
					text-align: left;
				}
				.conteudo table .p2 {
					width: 45% !important;
				}
				#div_title h1 {
					font-size: 2em !important;
				}
				#tab-menu > * {
					width: 100% !important;
				}
				#tab-menu {
					margin: auto 1.5% !important;
					width: 96% !important;
					display: flex !important;
				}
				#tab-menu tr {
					display: flex !important;
				}
				#tab-menu tr > * {
					margin: auto 0.3%;
				}
				#tab-menu td {
					width: 100% !important;
				}
			  </style>';

		include_once("notice.php");

		/*echo '<h3>PangYa SuperSS</h3>';

		echo '<br><p>O beta fechado começa dia 25 de dezembro de 2016</p>';*/
		
		echo '</article>';
	}

	private function report_view() {
		echo '<article id="home">';

		if ($this->logado) {
			if ($this->msg == 0) {
				echo '<style>
					  	#div_bug_r {
							display: block;
							width: 100%;
						}
						#div_bug_r form > * {
							margin: 1% auto;
						}
						#div_bug_r textarea {
							resize: none;
							display: block;
							width: 90%;
							height: 50%;
							border: 1px solid black;
						}
						#div_bug_r input {
							border: 1px solid black;
						}
						.tooltip {
							position: relative;
							display: inline-block;
							border-bottom: 1px dotted black;
						}
						.tooltip .tooltiptext {
							visibility: hidden;
							width: 300px;
							background-color: #555;
							color: #fff;
							text-align: center;
							border-radius: 6px;
							padding: 5px 0;
							position: absolute;
							z-index: 1;
							bottom: 125%;
							left: 50%;
							margin-left: -150px;
							opacity: 0;
							transition: opacity 1s;
						}
						.tooltip .tooltiptext::after {
							content: "";
							position: absolute;
							top: 100%;
							left: 50%;
							margin-left: -5px;
							border-width: 5px;
							border-style: solid;
							border-color: #555 transparent transparent transparent;
						}
						.tooltip:hover .tooltiptext {
							visibility: visible;
							opacity: 1;
						}
				     </style>';

				echo '<h3>Bug Report 
					  	<sup class="tooltip">?
					  	<span class="tooltiptext">Bug report pode colocar imagem e vídeo nele, mas só o link de onde eles foram upados. Ex: youtube.com, imgur.com <a href="pangya.php?link=5#bug_report" style="text-decoration: underline">Vejá com mais detalhes no FAQ</a></span>
					 	</sup>
					 </h3>';

				if ($this->bug_ta == '') {
					echo '<div id="div_bug_r">
						 	<form action="pangya.php?link=4" method="POST">
						 		<textarea name="bug_report_ta" maxlength="8192" rows="20" autofocus></textarea>
						 		<input type="submit" value="Enviar">
						 	</form>
						 </div>';
				}else {
					echo '<div id="div_bug_r">
						 	<form action="pangya.php?link=4" method="POST">
						 		<textarea id="bug_report_ta_id" name="bug_report_ta_confirm" maxlength="8192" rows="20" disabled>'.$this->bug_ta.'</textarea>
						 		<div>
						 			<span onclick="document.getElementById(\'bug_report_ta_id\').removeAttribute(\'disabled\');" style="border: 1px solid black">Editar</span>
						 			<input onclick="document.getElementById(\'bug_report_ta_id\').removeAttribute(\'disabled\');" type="submit" value="Confirmar">
						 		</div>
						 	</form>
					</div>';
				}
			}else
				echo "<script>window.addEventListener('load', function() {
					alert('Bug report foi enviado com sucesso.');
					window.open('pangya.php', '_self');
				});</script>";
		}else
			echo "<script>window.addEventListener('load', function() {
				alert('Logue por favor.');
				window.open('pangya.php', '_self');
			});</script>";

		echo '</article>';
	}
}

?>