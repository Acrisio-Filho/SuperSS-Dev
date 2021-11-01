<?php
	// Arquivo lottery.php
	// Criado em 22/05/2020 as 06:30 por Acrisio
	// Faz o sorteio do player
	
	// Para expira essa página em 1 segundo, para sempre buscar do server, 
	// por que o internet explore não busca de novo ele pega do cache
	Header('Cache-Control: max-age=1');

	include_once('source/config.inc');
	include_once('source/util.inc');
	include_once('source/player_play_singleton.inc');
	include_once('source/player_singleton.inc');
	include_once('source/debug_log.inc');
	include_once('source/design.inc');
	
	//include_once('source/db_manager_singleton.inc');
	include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

	class Lottery {

		protected $itens = [];
		protected $type = RARITY_TYPE::RT_NORMAL;
		protected $config = null;

		public function __construct() {

			Design::checkIE();

			Design::checkLogin();

			$this->checkAllValues();

			$this->loadConfigGacha();

			$this->lottery();

		}

		protected function getAllTicketPlay() {
			return PlayerSingleton::getTotalTicketToPlay(PlayerSingleton::getInstance());
		}

		protected function sendError() {

			echo 'ERROR';

			if (!isset($_SESSION))
				session_start();

			if (isset($_SESSION['player_play']))
				unset($_SESSION['player_play']);

			$this->itens = [];
			$this->config = null;

			exit();
		}

		protected function checkAllValues() {

			if (PlayerPlaySingleton::getInstance() == null || PlayerPlaySingleton::getInstance()['created'] == false) {

				DebugLog::Log("Nao chamou o Gacha Lottery Antes, esta tentando burlar o gacha");

				$this->sendError();

				return;
			}

			if (PlayerPlaySingleton::getInstance()['SpinningLottery'] == true) {

				DebugLog::Log("Já jogou, está tentando jogar de novo sem ver o resultado.");

				$this->sendError();

				return;
			}

			if (!PlayerSingleton::checkPlayerHaveTicketToPlay(PlayerPlaySingleton::getInstance()['modo'])) {

				DebugLog::Log("[checkAllValues] Player nao tem a quantidades de ticket suficiente para jogar. TICKET[".$this->getAllTicketPlay()."], MODO[".PlayerPlaySingleton::getInstance()['modo']."]");

				$this->sendError();

				return;
			}

			if (empty(PlayerPlaySingleton::getInstance()['itens'])) {

				DebugLog::Log("Nao tem itens para sortear, algum bug");

				$this->sendError();

				return;
			}

			// Itens
			$this->itens = PlayerPlaySingleton::getInstance()['itens'];
		}

		protected function loadConfigGacha() {

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);

			if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
            	$query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGachaJPConfig';
			else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
            	$query = 'select "_rate_normal_" as "rate_normal", "_rate_rare_" as "rate_rare", "_gacha_num_" as "gacha_num" from '.$db->con_dados['DB_NAME'].'.ProcGetGachaJPConfig()';
			else
            	$query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGachaJPConfig()';

            if (($result = $db->db->execPreparedStmt($query, null, 1)) && $db->db->getLastError() == 0) {

                while ($row = $result->fetch_assoc()) {

					if (isset($row['rate_normal']) && isset($row['rate_rare']) && isset($row['gacha_num'])) {
						$this->config = [
							'GACHA_NUM' => $row['gacha_num'],
							'RT_NORMAL' => $row['rate_normal'],
							'RT_RARE' => $row['rate_rare']
						];
					}
				}
			}

			if ($this->config == null) {

				DebugLog::Log("Nao conseguiu pegar informações do gacha no banco de dados");

				exit();

				return;
			}
		}

		protected function lottery() {

			$normal = [];
			$rare = [];

			foreach ($this->itens as $item) {

				if ($item['RARITY_TYPE'] == RARITY_TYPE::RT_RARE)
					$rare[] = $item;
				else
					$normal[] = $item;
			}

			// Reseta itens para colocar os que o player ganhou
			$this->itens = [];

			for ($i = 0; $i < PlayerPlaySingleton::getInstance()['modo']; $i++)
				$this->SpinningLottery($normal, $rare);

			// Resete itens win
			$_SESSION['player_play']['itens'] = [];
			$_SESSION['player_play']['itens'] = $this->itens;

			// Sorteou os itens
			$_SESSION['player_play']['SpinningLottery'] = true;

			// Consome o(s) ticket(s)
			$this->consomeTicket();

			// Salva itens no DB
			$this->sendItensToDB();

			// Lottery Seed Result
			echo putSeedIntoRandom($this->type);
		}

		protected function SpinningLottery(&$normal, &$rare) {

			// SRAND seed to shuffle
			srand((float)microtime()*1000009);

			// Padrão
			$Lottery = &$normal;

			if (!empty($rare)) {

				$_1 = $this->config['RT_RARE'] * 300;
				$_2 = $this->config['RT_NORMAL'] * 700;

				if (rand(0, $_1) >= rand(0, $_2))
					$Lottery = &$rare;
				
			}

			$count = count($Lottery);

			shuffle($Lottery);

			$dice = rand(0, $count-1);
					
			$item = $Lottery[$dice];

			// Remove o item da lista se ele for raro, para não tirar ele novamente
			if ($item['RARITY_TYPE'] == RARITY_TYPE::RT_RARE)
				array_splice($Lottery, $dice, 1);

			if ($this->type > $item['RARITY_TYPE'])
				$this->type = $item['RARITY_TYPE'];

			// Push no Array
			$this->itens[] = $item;
		}

		protected function sendItensToDB() {

			// Gera String de Itens para enviar para o banco de dados
			$str = '';
			$b = false;
			$count_item = 0;

			foreach ($this->itens as $item) {

				foreach ($item['ITEM'] as $el) {

					if (!$b)
						$b = true;
					else
						$str .= '|';

					$str .= $el['TYPEID'].';'.$el['QNTD'].';'.$item['RARITY_TYPE'].';'.$item['GACHA_NUM'];

					$count_item++;
				}
			}
			
			// Insert DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
			$params = $db->params;

            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);
            $params->add('i', $count_item);
			$params->add('s', $str);
            
			if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
            	$query = 'DECLARE @RET INT; EXEC @RET = '.$db->con_dados['DB_NAME'].'.ProcRegisterGachaJPPlayerWin ?, ?, ?';
			else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
            	$query = 'select "_FIRST_ID_" as "FIRST_ID" from '.$db->con_dados['DB_NAME'].'.ProcRegisterGachaJPPlayerWin(?, ?, ?)';
			else
            	$query = 'call '.$db->con_dados['DB_NAME'].'.ProcRegisterGachaJPPlayerWin(?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {


                do {

                    while ($row = $result->fetch_assoc()) {

                        if (isset($row['FIRST_ID'])) {

                            // Log
                            DebugLog::Log("[GachaJP][Lottery] Inseriu itens com sucesso. FIRST ID: ".$row['FIRST_ID'].". Itens: ".json_encode($this->itens));

							// Sai da função que deu tudo certo
                            return;
                        }
                    }

                } while ($result->next_result() && ($result->get_result()) && $db->db->getLastError() == 0);

                // Error;
                $this->setErrorFailInsertItemMail($this->itens, $db);
                
            }else   // Error
                $this->setErrorFailInsertItemMail($this->itens, $db);

		}

		protected function setErrorFailInsertItemMail($items, $db) {

			$err = "Não conseguiu inserir os itens no mail do player. DB ERROR: ".$db->db->getLastError().". Itens: ".json_encode($items);
            $code = 1005;

            DebugLog::Log("[GachaJP][Lottery] ERROR: ".$err.". CODE: ".$code);
		}

		protected function consomeTicket() {

			if (!PlayerSingleton::checkPlayerHaveTicketToPlay(PlayerPlaySingleton::getInstance()['modo'])) {

				DebugLog::Log("[consomeTicket] Player nao tem a quantidades de ticket suficiente para jogar. TICKET[".$this->getAllTicketPlay()."], MODO[".PlayerPlaySingleton::getInstance()['modo']."]");

				$this->sendError();

				return;
			}

			$ticket_used = PlayerSingleton::getNumTicketByPlayModo(PlayerSingleton::getInstance(), PlayerPlaySingleton::getInstance()['modo']);

			if ($ticket_used == -1) {

				DebugLog::Log("[consomeTicket] erro para pegar o numero de ticket(s) usado pelo modo[".PlayerPlaySingleton::getInstance()['modo']."]");

				$this->sendError();

				return;
			}

			// Log string
			$log = "[TICKET=".PlayerSingleton::getInstance()['TICKET'].", TICKET_SUB=".PlayerSingleton::getInstance()['TICKET_SUB']."]";
			$log_ticket_used = "[TICKET_USED=".$ticket_used.", MODO=".(PlayerPlaySingleton::getInstance()['modo'] == PLAY_MODO::PM_TEN ? "10" : "1")."]";

			if (PlayerSingleton::getInstance()['TICKET'] >= $ticket_used)
				$_SESSION['player']['TICKET'] -= $ticket_used;
			else if (PlayerSingleton::getInstance()['TICKET'] > 0) {

				$rest = $ticket_used - PlayerSingleton::getInstance()['TICKET'];

				$_SESSION['player']['TICKET'] = 0;
				$_SESSION['player']['TICKET_SUB'] -= ($rest * 10);
			}else
				$_SESSION['player']['TICKET_SUB'] -= ($ticket_used * 10);

			// Update no banco de dados
			$db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
			$params = $db->params;

			$params->clear();
			$params->add('i', PlayerSingleton::getInstance()['UID']);
			$params->add('i', PlayerSingleton::getInstance()['TICKET']);
			$params->add('i', PlayerSingleton::getInstance()['TICKET_ID']);
			$params->add('i', PlayerSingleton::getInstance()['TICKET_SUB']);
			$params->add('i', PlayerSingleton::getInstance()['TICKET_SUB_ID']);

			if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
				$query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateGachaJPTicketPlayer ?, ?, ?, ?, ?';
			else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
				$query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcUpdateGachaJPTicketPlayer(?, ?, ?, ?, ?)';
			else
				$query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateGachaJPTicketPlayer(?, ?, ?, ?, ?)';

			if (($result = $db->db->execPreparedStmt($query, $params->get())) == null || $db->db->getLastError() != 0) {
				DebugLog::Log("[Lottery::consomeTicket][Error] Nao conseguiu atualizar os ticket do player");

				$this->sendError();

				return;
			}

			// Log
			DebugLog::Log("[Lottery::consomeTicket] Consumiu ticket".$log_ticket_used.", Tinha".$log.", Depois[TICKET=".PlayerSingleton::getInstance()['TICKET'].", TICKET_SUB=".PlayerSingleton::getInstance()['TICKET_SUB']."]");
		}
	}

	// Lottery
	$lottery = new Lottery();
?>