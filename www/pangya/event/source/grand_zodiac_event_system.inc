<?php
    // Arquivo grand_zodiac_event_system.inc
    // Criado em 28/06/2020 as 14:48 por Acrisio
    // Definição e Implementação da classe GrandZodiacEventSystem

    include_once("config.inc");
    include_once("design.inc");
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once("util.inc");

    abstract class GrandZodiacEventSystem {

        protected $itens = 
        [ 
            ['nome' => 'Neon Comet', 'typeid' => 0x140000C5, 'qntd' => 30, 'price' => 200, 'tipo' => 0],
            ['nome' => 'Chestnut', 'typeid' => 0x1A000035, 'qntd' => 5, 'price' => 200, 'tipo' => 0],
            ['nome' => 'Drops Candy Melon', 'typeid' => 0x1A0000E3, 'qntd' => 3, 'price' => 100, 'tipo' => 0],
            ['nome' => 'Drops Candy Lime', 'typeid' => 0x1A0000E4, 'qntd' => 3, 'price' => 100, 'tipo' => 0],
            ['nome' => 'Drops Candy Strawberry', 'typeid' => 0x1A0000E2, 'qntd' => 3, 'price' => 200, 'tipo' => 0],
            ['nome' => 'Sakura Comet', 'typeid' => 0x1400001E, 'qntd' => 30, 'price' => 200, 'tipo' => 0],
            ['nome' => 'Yam', 'typeid' => 0x1A0000B3, 'qntd' => 5, 'price' => 100, 'tipo' => 0],
            ['nome' => 'Golden Yam', 'typeid' => 0x1A0000B4, 'qntd' => 3, 'price' => 200, 'tipo' => 0],
            ['nome' => 'Safety', 'typeid' => 0x18000028, 'qntd' => 5, 'price' => 100, 'tipo' => 0],
            ['nome' => 'Auto Caliper', 'typeid' => 0x1A000040, 'qntd' => 10, 'price' => 100, 'tipo' => 0],
            ['nome' => 'Obsidian Guardian Wings', 'typeid' => 777, 'itens' => [
                                                                                ['nome' => 'Obsidian Guardian Wings (N)', 'typeid' => 134309900],
                                                                                ['nome' => 'Obsidian Guardian Wings (H)', 'typeid' => 134580236],
                                                                                ['nome' => 'Obsidian Guardian Wings (AZ)', 'typeid' => 134842380],
                                                                                ['nome' => 'Obsidian Guardian Wings (C)', 'typeid' => 135120908],
                                                                                ['nome' => 'Obsidian Guardian Wings (M)', 'typeid' => 135366668],
                                                                                ['nome' => 'Obsidian Guardian Wings (K)', 'typeid' => 135661580],
                                                                                ['nome' => 'Obsidian Guardian Wings (A)', 'typeid' => 135858188],
                                                                                ['nome' => 'Obsidian Guardian Wings (KZ)', 'typeid' => 136194060],
                                                                                ['nome' => 'Obsidian Guardian Wings (L)', 'typeid' => 136398860],
                                                                                ['nome' => 'Obsidian Guardian Wings (NL)', 'typeid' => 136661004],
                                                                                ['nome' => 'Obsidian Guardian Wings (S)', 'typeid' => 136923148],
                                                                                ['nome' => 'Obsidian Guardian Wings (NR)', 'typeid' => 137185286],
                                                                                ['nome' => 'Obsidian Guardian Wings (HR)', 'typeid' => 137447434],
                                                                                ['nome' => 'Obsidian Guardian Wings (CR)', 'typeid' => 138004490],
                                                                            ], 'qntd' => 1, 'price' => 1000, 'tipo' => 1]
        ];

        protected $win_rares = [];

        abstract public function show();

        protected function checkFirstLogin() {

            Design::checkIE();

            // Vamos fazer o login aqui
            if (isset($_GET) && isset($_GET['ID']) && isset($_GET['KID'])) {

                $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                $params = $db->params;

                $id = (isset($_GET["ID"])) ? $_GET["ID"] : null;
                $key = (isset($_GET["KID"])) ? $_GET["KID"] : null;

                if ($id != null && $key != null && strlen($id) <= 25 && strlen($key) <= 8) {

                    // Initialize Session and clean it
                    if (!isset($_SESSION))
                        session_start();

                    if (isset($_SESSION['player_event']))
                        unset($_SESSION['player_event']);

                    // Init With Fail player para previnir de ele passar com algum bug
                    $_SESSION['player_event'] = ['logged' => false];
                    
                    // Check ID and KEY
                    $params->clear();
                    $params->add('s', $id);
                    $params->add('s', $key);

                    $query = '';

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'DECLARE @returnvalue INT = -1 exec '.$db->con_dados['DB_NAME'].'.CheckWeblink_Key$IMPL ?, ?, @returnvalue = @returnvalue OUTPUT SELECT @returnvalue as [uid]';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'select _returnvalue as uid from '.$db->con_dados['DB_NAME'].'.CheckWeblink_Key$IMPL(?, ?)';
                    else
                        $query = 'CALL CheckWeblink_Key(?, ?)';

                    if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0) {
                        
                        $row = $result->fetch_assoc();

                        if (isset($row['uid']) && $row['uid'] > 0) {

                            // Pega as informações iniciais da Guild do player
                            $params->clear();
                            $params->add('i', $row['uid']);

                            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'SELECT b.[UID], b.NICK AS NICKNAME, b.ID, b.IDState, a.PONTOS FROM '.$db->con_dados['DB_NAME'].'.pangya_grand_zodiac_pontos a INNER JOIN '.$db->con_dados['DB_NAME'].'.account b ON a.[UID] = b.[UID] WHERE b.[UID] = ?';
                            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'SELECT b.UID AS "UID", b.NICK AS "NICKNAME", b.ID as "ID", b.IDState as "IDState", a.PONTOS as "PONTOS" FROM '.$db->con_dados['DB_NAME'].'.pangya_grand_zodiac_pontos a INNER JOIN '.$db->con_dados['DB_NAME'].'.account b ON a.UID = b.UID WHERE b.UID = ?';
                            else
                                $query = 'SELECT b.`UID`, b.NICK AS NICKNAME, b.ID, b.IDState, a.PONTOS FROM '.$db->con_dados['DB_NAME'].'.pangya_grand_zodiac_pontos a INNER JOIN '.$db->con_dados['DB_NAME'].'.account b ON a.`UID` = b.`UID` WHERE b.`UID` = ?';

                            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                                && ($row = $result->fetch_assoc()) != null) {

                                if (isset($row['ID']) && isset($row['UID']) && isset($row['NICKNAME'])
                                    && isset($row['IDState']) && isset($row['PONTOS'])) {

                                    $_SESSION['player_event'] = 
                                    [ 
                                        'logged' => true, 
                                        'ID' => $row['ID'], 
                                        'UID' => $row['UID'], 
                                        'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"),
                                        'IDState' => $row['IDState'],
                                        'PONTOS' => $row['PONTOS']
                                    ];
                                                            
                                    // Success Login
                                    DebugLog::Log("[GrandZodiacEvent::checkFirstLogin] Player Logou com sucesso.");
                                    
                                }else
                                    header("Location: ".LINKS['UNKNOWN_ERROR']);
                            
                            }else
                                header("Location: ".LINKS['UNKNOWN_ERROR']);
                        
                        }else
                            header("Location: ".LINKS['UNKNOWN_ERROR']);
                    
                    }else
                        header("Location: ".LINKS['UNKNOWN_ERROR']);
                
                }else
                    header("Location: ".LINKS['UNKNOWN_ERROR']);
            }
        }

        protected function checkGetitem() {

            // Verifica se ele está com o navegador certo e pelo projectg
            Design::checkIE();

            // Verifica se ele está logado
            Design::checkLogin();

            if (isset($_GET) && isset($_GET['item'])) {

                // Verifica aqui se ele pode pegar o item que ele escolheu mesmo
                $choice_item = $_GET['item'];

                // Check if item is valid
                if (!is_numeric($choice_item)) {

                    DebugLog::Log("[GrandZodiacEvent::checkGetItem] Item é inválido, não é um número.");

                    $this->sendError();

                    return;
                }

                $item = $this->findItem($choice_item);

                if ($item == null) {

                    DebugLog::Log("[GrandZodiacEvent::checkGetItem] Não tem o item[TYPEID=".$choice_item."] na lista de itens que pode pegar.");

                    $this->sendError();

                    return;
                }

                if (PlayerSingleton::getInstance()['PONTOS'] < $item['price']) {

                    DebugLog::Log("[GrandZodiacEvent::checkGetItem] Não tem pontos(".PlayerSingleton::getInstance()['PONTOS'].") suficiente para pegar o item[PRICE=".$item['price']."]");

                    $this->sendError();

                    return;
                }

                if ($item['tipo'] == 1 && !$this->isActiveRandomItem()) {

                    DebugLog::Log("[GrandZodiacEvent::checkGetItem] Player já tem todos os itens raros, não pode pegar mais raros.");

                    $this->sendError();

                    return;
                }

                $new_item = $this->initChoiceItem($item);

                if ($new_item == null) {

                    DebugLog::Log("[GrandZodiacEvent::checkGetItem] Não conseguiu inicilizar o item[TYPEID=".$item['typeid']."] escolhido. Bug");

                    $this->sendError();

                    return;
                }

                $this->consomePontos($new_item['price']);

                $this->sendItensToDB($new_item);

                $this->sendResposta($new_item);
            }
        }

        protected function sendError() {

			if (!isset($_SESSION))
				session_start();

            // Desloga
			if (isset($_SESSION['player_event']))
				unset($_SESSION['player_event']);

            $this->win_rares = [];

            // Redireciona para a página de erro
            header("Location: ".LINKS['UNKNOWN_ERROR']);

            // sai do script para o navegador redirecionar para a página
            exit();
        }
        
        protected function initChoiceItem($item) {

            if ($item == null)
                return null;

            $new_item = null;

            if ($item['tipo'] == 1) {       // Raro

                // Raro inválido
                if (!isset($item['itens']) || !is_array($item['itens']) || empty($item['itens']))
                    return null;

                $new_itens = [];

                foreach ($item['itens'] as $rare) {

                    if (find_if($this->win_rares, function($el) use(&$rare) {
                        return $el['TYPEID'] == $rare['typeid'];
                    }) == null)
                        array_push($new_itens, $rare);
                }

                $count = 0;

                // Já tem todos os raros
                if (empty($new_itens) || ($count = count($new_itens)) <= 0)
                    return null;

                // Sorteia new Raro
                $win = $new_itens[(rand(1, $count) - 1)];

                // Novo item
                $new_item =
                [
                    'nome' => $win['nome'],
                    'typeid' => $win['typeid'],
                    'qntd' => $item['qntd'],
                    'price' => $item['price'],
                    'tipo' => $item['tipo']
                ];

            }else if ($item['tipo'] == 0) // Normal
                $new_item = $item;

            return $new_item;
        }

        protected function consomePontos($price) {

            if (!is_numeric($price)) {

                DebugLog::Log("[GrandZodiacEvent::consomePontos] price não é um número.");

                $this->sendError();

                return;
            }

            if (PlayerSingleton::getInstance()['PONTOS'] < $price) {

                DebugLog::Log("[GrandZodiacEvent::consomePontos] Player não tem pontos(".PlayerSingleton::getInstance()['PONTOS'].") suficiente para pegar esse item[PRICE=".$price."]");

                $this->sendError();

                return;
            }

            // Log de quantos pontos tinha antes
            $log = "[PONTOS=".PlayerSingleton::getInstance()['PONTOS']."]";

            $_SESSION['player_event']['PONTOS'] -= $price;

            // Update no banco de dados
			$db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['PONTOS']);
            $params->add('i', PlayerSingleton::getInstance()['UID']);

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = "UPDATE ".$db->con_dados['DB_NAME'].".pangya_grand_zodiac_pontos SET pontos = ? WHERE [UID] = ?";
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = "UPDATE ".$db->con_dados['DB_NAME'].".pangya_grand_zodiac_pontos SET pontos = ? WHERE UID = ?";
            else
                $query = "UPDATE ".$db->con_dados['DB_NAME'].".pangya_grand_zodiac_pontos SET pontos = ? WHERE `UID` = ?";

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null || $db->db->getLastError() != 0) {
                
                DebugLog::Log("[GrandZodiacEvent::consomePontos][Error] Nao conseguiu atualizar os pontos do player");

				$this->sendError();

				return;
            }
            
            // Log
            DebugLog::Log("[GrandZodiacEvent::consomePontos] Consumiu pontos(".$price."), Tinha".$log.", Depois[PONTOS=".PlayerSingleton::getInstance()['PONTOS']."]");
        }

        protected function sendItensToDB($item) {

            if ($item == null) {

                DebugLog::Log("[GrandZodiacEvent::sendItensToDB] item é inválido.");

                $this->sendError();

                return;
            }

            // Insert DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);
            $params->add('i', $item['typeid']);
            $params->add('i', $item['qntd']);
            $params->add('i', $item['tipo']);
            
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'DECLARE @RET INT; EXEC @RET = '.$db->con_dados['DB_NAME'].'.ProcRegisterGrandZodiacEventWebPlayerWin ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_MSG_ID_" as "MSG_ID" from '.$db->con_dados['DB_NAME'].'.ProcRegisterGrandZodiacEventWebPlayerWin(?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcRegisterGrandZodiacEventWebPlayerWin(?, ?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {


                do {

                    while ($row = $result->fetch_assoc()) {

                        if (isset($row['MSG_ID'])) {

                            // Log
                            DebugLog::Log("[GrandZodiacEventSystem::sendItensToDB] Inseriu itens com sucesso. MSG_ID: ".$row['MSG_ID'].". Itens: ".json_encode($item));

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

            DebugLog::Log("[GrandZodiacEvent::setErrorFailInsertItemMail] ERROR: ".$err.". CODE: ".$code);

            $this->sendError();

            return;
		}

        protected function sendResposta($item) {

            if ($item == null) {

                DebugLog::Log("[GrandZodiacEventSystem::sendResposta] item é inválido.");

                $this->sendError();

                return;
            }

            $msg = $item['qntd'].' '.$item['nome'].' foi enviado para seu mail box com sucesso';

            if ((strpos($_SERVER['HTTP_USER_AGENT'], "MSIE") !== false)) // is Internet Explorer
                echo "<script>window.attachEvent('onload', function() { alert('".$msg."')});</script>";
            else
                echo "<script>window.addEventListener('load', function() { alert('".$msg."')});</script>";
        }

        protected function begin() {

            Design::beginHTML();

            Design::beginHead();

            $this->style();

            // Atualiza os itens raro que o player já ganhou, porque ele pode ter pego 1 raro
            // No CheckGetItem, aí tem que atualizar aqui os itens raro que ele pode pegar, vai que ele pegou o ultimo aí tem que bloquear por que não tem mais itens raro
            $this->loadWinRareList();
            
            // JavaScript
            $this->makeContentHead();
        }

        protected function middle() {

            Design::endHead();
            
            Design::beginBody();
            
            Design::beginContent();
        }

        protected function end() {

            Design::endContent();
            
            Design::endBody();
            
            Design::endHTML();
        }

        protected function style() {

            echo '<style type="text/css">
                    img {
                        margin: 0;
                        position: absolute;
                        top: 0;
                        left: 0;
                        background: no-repeat;
                    }
                    .pontos {
                        position: absolute;
                        font-size: 35px;
                        font-weight: bold;
                        width: 30%;
                        top: 417px;
                        left: 215px;
                    }
                    .buttons {
                        position: absolute;
                        background: no-repeat;
                    }
                </style>';
        }

        protected function findItem($typeid) {

            foreach($this->itens as $item)
                if ($item['typeid'] == $typeid)
                    return $item;

            return null;
        }

        // Verifica se ele pode pegar os itens raros
        protected function isActiveRandomItem() {
            return ($this->win_rares !== null && is_array($this->win_rares) && ($item = $this->findItem(777/*Rare*/)) != null && count($this->win_rares) < count($item['itens']));
        }

        protected function makeButt($img, $x, $y, $size, $item_typeid, $state) {
            return "butt_ar.push(new button('$img', $x, $y, $size, $item_typeid, $state));\n";
        }

        // JavaScript
        protected function makeContentHead() {

            echo "<script>
					window.onload = init_all;

					function hidden() {
						document.body.style.overflow = 'hidden';
					}

					function button(img, x, y, size, item_typeid, state) {
						this.img = img;
						this.x = x;
						this.y = y;
						this.size = size;
						this.item_typeid = item_typeid;
						this.state = state;
					}

					var imgs = [{
						name: 'get',
						url: ['../img/button_get_n.png', '../img/button_get_over.png']
					},
					{
						name: 'random',
						url: ['../img/button_random_n.png', '../img/button_random_over.png']
					}];

					var size_get = {
						width: 78,
						height: 25
					};

					var size_random = {
						width: 134,
						height: 34
					};

					var butt_ar = [];\n";

					// Cima - 100
					echo $this->makeButt('get', 93, 212, 'size_get', 0x1A0000E3, ((PlayerSingleton::getInstance()['PONTOS'] >= 100) ? 'true' : 'false'));
					echo $this->makeButt('get', 225, 212, 'size_get', 0x1A0000E4, ((PlayerSingleton::getInstance()['PONTOS'] >= 100) ? 'true' : 'false'));
					echo $this->makeButt('get', 368, 212, 'size_get', 0x1A0000B3, ((PlayerSingleton::getInstance()['PONTOS'] >= 100) ? 'true' : 'false'));
					echo $this->makeButt('get', 500, 212, 'size_get', 0x18000028, ((PlayerSingleton::getInstance()['PONTOS'] >= 100) ? 'true' : 'false'));
					echo $this->makeButt('get', 629, 212, 'size_get', 0x1A000040, ((PlayerSingleton::getInstance()['PONTOS'] >= 100) ? 'true' : 'false'));

					// Baixo - 200
					echo $this->makeButt('get', 93, 323, 'size_get', 0x1400001E, ((PlayerSingleton::getInstance()['PONTOS'] >= 200) ? 'true' : 'false'));
					echo $this->makeButt('get', 225, 323, 'size_get', 0x1A0000E2, ((PlayerSingleton::getInstance()['PONTOS'] >= 200) ? 'true' : 'false'));
					echo $this->makeButt('get', 368, 323, 'size_get', 0x1A0000B4, ((PlayerSingleton::getInstance()['PONTOS'] >= 200) ? 'true' : 'false'));
					echo $this->makeButt('get', 500, 323, 'size_get', 0x1A000035, ((PlayerSingleton::getInstance()['PONTOS'] >= 200) ? 'true' : 'false'));
					echo $this->makeButt('get', 629, 323, 'size_get', 0x140000C5, ((PlayerSingleton::getInstance()['PONTOS'] >= 200) ? 'true' : 'false'));

					// Random - 1000
					echo $this->makeButt('random', 500, 460, 'size_random', 777, (($this->isActiveRandomItem() && PlayerSingleton::getInstance()['PONTOS'] >= 1000) ? 'true' : 'false'));

			echo 	"function get_img(img) {
						for (var i = 0; i < imgs.length; i++)
							if (imgs[i].name == img)
								return imgs[i];
						
						return undefined;
					}

					function back_toggle(_img, el_id, normal_over) {
						var c = document.getElementById(el_id);

						var img = get_img(_img);

						if (c)
							c.src = (img != undefined) ? img.url[normal_over] : '';
					}

					function init_buttons() {
						var img = undefined, c = undefined;

						for (var i = 0; i < butt_ar.length; i++) {
							img = get_img(butt_ar[i].img);

							if (c = createImg('img-' + i, butt_ar[i]))
								c.src = (img != undefined) ? img.url[((butt_ar[i].state) ? 0 : 2)] : '';
						}
					}

					function get_item(button) {
						window.open('?item=' + button.item_typeid, '_self');
					}

					function createImg(id, button) {

                        if (button.state) {
                            
                            var img = document.createElement('img');
                            img.id = id;
                            img.className = 'buttons';
                            
                            if (button.state) {
                                img.onmouseover = function() { back_toggle(button.img, this.id, 1) };
                                img.onmouseout = function() { back_toggle(button.img, this.id, 0) };
                                img.onclick = function() { get_item(button) };
                            
                                img.style.cursor = 'pointer';
                            }

                            position(img, button.x, button.y, button.size);

                            document.body.appendChild(img);

                            return img;

                        }else 
                            return undefined;
					}

					function init_all() {
						hidden();		// escode scroll bar
						init_buttons();	// inicializa os botões
					}

					function position(element, x, y, size) {
						if (element) {
							element.style.top = y.toString() + 'px';
							element.style.left = x.toString() + 'px';
							element.style.width = size.width.toString() + 'px';
							element.style.height = size.height.toString() + 'px';
						}
					}


				 </script>";
        }

        protected function loadWinRareList() {

            // Verifica se ele está com o navegador certo e pelo projectg
            Design::checkLogin();

            // Verifica se ele está logado
            Design::checkLogin();

            // Carrega os itens raros que ele ganhou
            $itens_win = [];
            $item_win = [];

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);   // UID

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT item_typeid FROM '.$db->con_dados['DB_NAME'].'.pangya_gz_event_2016121600_rare_win WHERE [UID] = ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT item_typeid FROM '.$db->con_dados['DB_NAME'].'.pangya_gz_event_2016121600_rare_win WHERE UID = ?';
            else
                $query = 'SELECT item_typeid FROM '.$db->con_dados['DB_NAME'].'.pangya_gz_event_2016121600_rare_win WHERE `UID` = ?';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                while ($row = $result->fetch_assoc()) {

                    if (isset($row['item_typeid'])) {
                    
                            $item_win = [
                                'TYPEID' => $row['item_typeid']
                            ];

                            // Push Item no array
                            $itens_win[] = $item_win;
                    }
                }

            }else
                DebugLog::Log("[GrandZodiacEvent::loadWinRareList] não conseguiu pegar os itens ganhos do player.");

            $this->win_rares = $itens_win;
        }
    }
?>