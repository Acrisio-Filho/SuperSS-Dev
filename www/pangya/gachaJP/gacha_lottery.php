<?php
    // Arquivo gacha_lottery.php
    // Criado em 23/05/2020 as 17:32 por Acrisio
    // Sistema do Gacha Lottery principal

    include_once("source/gacha_lottery_system.inc");
    include_once("source/debug_log.inc");
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once("source/itens_data_singleton.inc");
    include_once("source/util.inc");

    class GachaLottery extends GachaLotterySystem {

        private $have_itens = [];
        private $is_loaded_have_itens = false;

        protected $modo = PLAY_MODO::PM_ONE;

        public function show() {

            $this->checkInValues();

            $this->initArrHaveItens();

            $this->initLotteryToPlayerAndModo();

            $this->begin();

            echo '<title>Gacha Lottery</title>';

            $this->middle();

            $this->content();

            $this->end();
        }

        protected function checkInValues() {

            DesignLottery::checkIE();

            DesignLottery::checkLogin();

            if (!isset($_GET['count']) || !is_numeric($_GET['count']) || ($_GET['count'] != PLAY_MODO::PM_ONE && $_GET['count'] != PLAY_MODO::PM_TEN)) {

                DebugLog::Log("Forneceu ou não forneceu um valor do count(modo) invalido.");

                Header("Location: ".LINKS['UNKNOWN_ERROR']);

                // exit, para o script para redirecionar a página
                exit();

                return;
            }

            $this->modo = $_GET['count'];

            if (!PlayerSingleton::checkPlayerHaveTicketToPlay($this->modo)) {

                DebugLog::Log("Não tem quantidade de ticket suficiente para jogar o modo. TICKET[".$this->getAllTicketPlay()."], MODO[".$this->modo."]");

                Header("Location: ".LINKS['UNKNOWN_ERROR']);

                // exit, para o script para redirecionar a página
                exit();

                return;
            }
        }

        protected function initLotteryToPlayerAndModo() {

            if (!isset($_SESSION))
                session_start();

            // Pegar os itens que o player pode ganhar do banco de dados

            $itens = [];
            $item = [];

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);   // UID

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetPlayerCanWinGachaJPItens ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_index_" as "index", "_gacha_num_" as "gacha_num", "_rarity_type_" as "rarity_type", "_typeid_1_" as "typeid_1", "_qnty_1_" as "qnty_1", "_name_1_" as "name_1", "_char_type_1_" as "char_type_1", "_typeid_2_" as "typeid_2", "_qnty_2_" as "qnty_2", "_name_2_" as "name_2", "_char_type_2_" as "char_type_2" from '.$db->con_dados['DB_NAME'].'.ProcGetPlayerCanWinGachaJPItens(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetPlayerCanWinGachaJPItens(?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                while (($row = $result->fetch_assoc()) != null) {

                    if (isset($row['index']) && isset($row['gacha_num']) && isset($row['rarity_type'])
                        && isset($row['typeid_1']) && isset($row['qnty_1']) && isset($row['name_1']) && isset($row['char_type_1'])
                        && key_exists('typeid_2', $row) && key_exists('qnty_2', $row) && key_exists('name_2', $row) && key_exists('char_type_2', $row)) {
                    
                            $item = [
                                'ID' => $row['index'],
                                'GACHA_NUM' => $row['gacha_num'],
                                'RARITY_TYPE' => $row['rarity_type'],
                                'ITEM' => [
                                    [
                                        'TYPEID' => $row['typeid_1'],
                                        'QNTD' => $row['qnty_1'],
                                        'NAME' => mb_convert_encoding($row['name_1'], "UTF-8", "SJIS")
                                    ]
                                ]
                            ];

                            if ($row['typeid_2'] != null && $row['typeid_2'] != '')
                                $item['ITEM'][] = [
                                    'TYPEID' => $row['typeid_2'],
                                    'QNTD' => $row['qnty_2'],
                                    'NAME' => mb_convert_encoding($row['name_2'], "UTF-8", "SJIS")
                                ];

                            // Push Item no array
                            $itens[] = $item;
                    }
                }

            }

            // Aqui verifica se já tem o item no banco de dados
            $const_db_item = ItemSingleton::getInstance()->getItens();

            $chk_item = new stdClass();

            foreach ($itens as $kkey => &$el_gacha_item) {

                foreach ($el_gacha_item['ITEM'] as $el_item) {

                    // Check itens
                    $chk_item->type = getItemTypeName(getItemGroupType($el_item['TYPEID']));
                    $chk_item->_typeid = $el_item['TYPEID'];

                    if ($chk_item->type != "Card") {
                
                        if (($citem = findItemByCategory($const_db_item, $chk_item)) != null) {

                            if ($this->checkHaveItem($citem)) {

                                // remove item do array, o player já tem o item
                                unset($itens[$kkey]);
                            }
                        }
                    }
                }
            }

            // Verifica se tem itens para sortear, se não dá error
            if (empty($itens)) {

                DebugLog::Log("Não conseguiu pegar os itens do banco de dos para sortear");

                Header("Location: ".LINKS['UNKNOWN_ERROR']);

                exit();

                return;
            }

            // Cria a Session de player play, para o lottery sortear os itens e confirma que o player jogou direito
            if (isset($_SESSION['player_play']))
                unset($_SESSION['player_play']);

            $_SESSION['player_play'] = [
                'created' => true,
                'SpinningLottery' => false, // Se já sorteou um prêmio, chamou o lottery.php
                'modo' => $this->modo,
                'itens' => $itens
            ];
        }

        private function initArrHaveItens() {

            $this->is_loaded_have_itens = false;
        
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);       // ID
            
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcGetAllItemByPlayer ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_TYPEID_" as "TYPEID", "_TYPE_" as "TYPE" from '.$db->con_dados['DB_NAME'].'.ProcGetAllItemByPlayer(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetAllItemByPlayer(?)';
        
            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['TYPEID']) && isset($row['TYPE'])) {
        
                do {

                    if (isset($row['TYPEID']) && isset($row['TYPE'])) {

                        switch ($row['TYPE']) {
                            case 0: // Caddie
                                $this->have_itens["Caddie"][] = ["typeid" => $row['TYPEID']];
                                break;
                            case 1: // Mascot
                                $this->have_itens['Mascot'][] = ["typeid" => $row['TYPEID']];
                                break;
                            case 2: // Warehouse
                                $this->have_itens['Warehouse'][] = ["typeid" => $row['TYPEID']];
                                break;
                            case 3: // Dolphin_Locker
                                $this->have_itens['Dolphin_Locker'][] = ["typeid" => $row['TYPEID']];
                                break;
                            case 4: // Mail_Box
                                $this->have_itens['Mail_Box'][] = ["typeid" => $row['TYPEID']];
                                break;
                        }
                    }

                } while (($row = $result->fetch_assoc()) != null);
                
                $this->is_loaded_have_itens = true;

            }else
                DebugLog::Log("[initArrHaveItens] Error ao pegar os itens que o player já tem. DB ERROR: ".$db->db->getLastError());
            
        }

        private function checkHaveItem($item) {
        
            if ($this->is_loaded_have_itens && isset($item) && $item) {

                // Dolphin locker não guarda set item, ele guarda os itens do set
                if ($item->type != "Setitem") {

                    // Dolphin Locker
                    if (isset($this->have_itens['Dolphin_Locker']) && is_array($this->have_itens['Dolphin_Locker']) && count($this->have_itens['Dolphin_Locker']) > 0) {
                        
                        foreach ($this->have_itens['Dolphin_Locker'] as $el)
                            if (isset($el['typeid']) && $el['typeid'] == $item->_typeid)
                                return true;
                    }
                }
        
                switch ($item->type) {
                    case "Caddie":
                    {
                        
                        if (isset($this->have_itens['Caddie']) && is_array($this->have_itens['Caddie']) && count($this->have_itens['Caddie']) > 0) {
                            
                            foreach ($this->have_itens['Caddie'] as $el)
                                if (isset($el['typeid']) && $el['typeid'] == $item->_typeid)
                                    return true;
                        }

                        // Mail Box
                        if (isset($this->have_itens['Mail_Box']) && is_array($this->have_itens['Mail_Box']) && count($this->have_itens['Mail_Box']) > 0) {
                            
                            foreach ($this->have_itens['Mail_Box'] as $el)
                                if (isset($el['typeid']) && $el['typeid'] == $item->_typeid)
                                    return true;
                        }
                    }
                    case "Mascot":
                    {
                        if (isset($this->have_itens['Mascot']) && is_array($this->have_itens['Mascot']) && count($this->have_itens['Mascot']) > 0) {
                            
                            foreach ($this->have_itens['Mascot'] as $el)
                                if (isset($el['typeid']) && $el['typeid'] == $item->_typeid)
                                    return true;
                        }

                        // Mail Box
                        if (isset($this->have_itens['Mail_Box']) && is_array($this->have_itens['Mail_Box']) && count($this->have_itens['Mail_Box']) > 0) {
                            
                            foreach ($this->have_itens['Mail_Box'] as $el)
                                if (isset($el['typeid']) && $el['typeid'] == $item->_typeid)
                                    return true;
                        }
                    }
                    case "Setitem":
                    {
                        // Dolphin Locker
                        if (isset($this->have_itens['Dolphin_Locker']) && is_array($this->have_itens['Dolphin_Locker']) && count($this->have_itens['Dolphin_Locker']) > 0) {
                            
                            foreach ($this->have_itens['Dolphin_Locker'] as $el) {

                                if (isset($el['typeid'])) {

                                    if ($el['typeid'] == $item->_typeid)
                                        return true;
                                    else if ($item->is_setitem && count($item->set_item) > 0) {

                                        // SetItem
                                        foreach ($item->set_item as $el2)
                                            if (isset($el2) && property_exists($el2, 'typeid') && $el['typeid'] == $el2->typeid)
                                                return true;
                                    }
                                }
                            }
                        }

                        if (isset($this->have_itens['Warehouse']) && is_array($this->have_itens['Warehouse']) && count($this->have_itens['Warehouse']) > 0) {
                            
                            foreach ($this->have_itens['Warehouse'] as $el) {
            
                                if (isset($el['typeid'])) {
                                    
                                    if ($el['typeid'] == $item->_typeid)
                                        return true;
                                    else if ($item->is_setitem && count($item->set_item) > 0) {
                                        
                                        // SetItem
                                        foreach ($item->set_item as $el2)
                                            if (isset($el2) && property_exists($el2, 'typeid') && $el['typeid'] == $el2->typeid)
                                                return true;
                                    }
                                }
                            }
                        }

                        // Mail Box
                        if (isset($this->have_itens['Mail_Box']) && is_array($this->have_itens['Mail_Box']) && count($this->have_itens['Mail_Box']) > 0) {

                            foreach ($this->have_itens['Mail_Box'] as $el) {
                                
                                if (isset($el['typeid'])) {

                                    if ($el['typeid'] == $item->_typeid)
                                        return true;
                                    else if ($item->is_setitem && count($item->set_item) > 0) {

                                        foreach ($item->set_item as $el2)
                                            if (isset($el2) && property_exists($el2, 'typeid') && $el['typeid'] == $el2->typeid)
                                                return true;
                                    }
                                }
                            }
                        }
                    }
                    case "Clubset":
                    case "Item":
                    case "Ring":
                    case "Outfit":
                    {
                        if (isset($this->have_itens['Warehouse']) && is_array($this->have_itens['Warehouse']) && count($this->have_itens['Warehouse']) > 0) {
                            
                            foreach ($this->have_itens['Warehouse'] as $el)
                                if (isset($el['typeid']) && $el['typeid'] == $item->_typeid)
                                    return true;
                        }

                        // Mail Box
                        if (isset($this->have_itens['Mail_Box']) && is_array($this->have_itens['Mail_Box']) && count($this->have_itens['Mail_Box']) > 0) {
                            
                            foreach ($this->have_itens['Mail_Box'] as $el)
                                if (isset($el['typeid']) && $el['typeid'] == $item->_typeid)
                                    return true;
                        }
                    }
                }
            }
        
            return false;
        }

        protected function content() {

            echo '<table cellSpacing="0" cellPadding="0">
                    <tr>
                        <td style="BACKGROUND-COLOR: #eeeeee" width="800">
                            <table style="MARGIN-RIGHT: 10px" height="54" cellSpacing="0" cellPadding="0" width="790" align="right">
                                <tr>
                                    <td align="right">
                                        <div class="StatusDIV1">Nickname</div>
                                    </td>
                                    <td align="right">
                                        <div id="_NickName" class="StatusDIV2">'.(PlayerSingleton::getInstance()['NICKNAME']/*Saitama(にや)*/).'</div>
                                    </td>
                                    <td align="right">
                                        <div class="StatusDIV1">Modo</div>
                                    </td>
                                    <td align="right">
                                        <div id="_PlayMode" class="StatusDIV3">'.$this->modo.' Play</div>
                                    </td>
                                    <td align="right">
                                        <div class="StatusDIV1">Number of remaining plays</div>
                                    </td>
                                    <td align="right">
                                        <div id="_RestPlay" class="StatusDIV3">'.((int)(PlayerSingleton::getInstance()['TICKET_SUB'] / 10) + PlayerSingleton::getInstance()['TICKET']).'</div>
                                    </td>
                                    <td align="right">
                                        <div class="StatusDIV1">Ticket / Partial ticket</div>
                                    </td>
                                    <td align="right">
                                        <div id="_NowTicket" class="StatusDIV4">'.PlayerSingleton::getInstance()['TICKET'].' / '.PlayerSingleton::getInstance()['TICKET_SUB'].'</div>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                    <tr>
                        <td style="HEIGHT: 1px; WIDTH: 800px; BACKGROUND-COLOR: #3581ff"></td>
                    </tr>
                    <tr>
                        <td>
                            <table height="500" cellSpacing="0" cellPadding="0" width="800"
                                background="./img/bg1.gif">
                                    <tr>
                                        <td id="Content-Anim">
                                            <script type="text/javascript">';

                // Depois de 11/02/2021 o flash player foi encerrado
                $final_flash_player = new DateTime('2021-02-11');
                $now = new DateTime();
                $now->settime(0,0,0);

                if ($now <= $final_flash_player)
                    echo 'writeGacha();';
                else {

                    echo 'var divf = document.createElement("div");
                          divf.innerHTML = \'<div id="animation_container" style="background-color:rgba(255, 255, 255, 1.00); width:800px; height:500px">\
                                                <canvas id="canvas" width="800" height="500" style="position: absolute; display: none; background-color:rgb(0, 0, 0);"></canvas>\
                                                <canvas id="canvas2" width="800" height="500" style="position: absolute; display: block; background-color:rgb(0, 0, 0);"></canvas>\
                                                <div id="dom_overlay_container" style="pointer-events:none; overflow:hidden; width:800px; height:500px; position: absolute; left: 0px; top: 0px; display: block;">\
                                                </div>\
                                            </div>\';
                        document.getElementById("Content-Anim").appendChild(divf);

                        // Init
                        init();
                    ';

                    // Temporário até eu fazer a animação no javascript, está quase só falta tirar os bugs e compactar
                    /*echo 'if (!XMLHttpRequest.DONE)
                            XMLHttpRequest.DONE = 4;
                        
                        function play_no_flash() {
                            play.detachEvent("onclick", play_no_flash);

                            play.innerHTML = "playing...";
                            var xhttp = new XMLHttpRequest();

                            xhttp.open("GET", "./lottery.php");
                            xhttp.setRequestHeader("Content-Type", "plain/text");
                            
                            xhttp.onreadystatechange = (function(button) {
                                return function() {

                                    if (this.readyState === XMLHttpRequest.DONE) {
                                        
                                        if (this.status === 200 && this.responseText != null && this.responseText != "" && !isNaN(this.responseText)) {

                                            var ganhou = Number(this.responseText.substr(5).charAt(0)) % 3;

                                            button.play.innerHTML = (ganhou == 0 ? "Lucky you" : (ganhou == 1 ? "Hmm, something good" : "Luckless")) + ", Click to see the result.";
                                            button.play.attachEvent("onclick", f_result);
                                        }else {

                                            button.play.innerHTML = "Error: fail to play.";
                                            button.play.attachEvent("onclick", f_error);
                                        }
                                    }
                                };
                            })({ play: play });

                            xhttp.send();
                        }

                        function f_error() {
                            document.location.href = "./gacha_result.php";
                        }

                        function f_result() {
                            document.location.href = "./gacha_result.php";
                        }

                        var play = document.createElement("button");
                        play.attachEvent("onclick", play_no_flash);
                        play.style.width = "100%";
                        play.style.height = "100%";
                        play.style.lineHeight = "100px";
                        play.style.fontSize = "30px";
                        play.innerHTML = "Play";
                        document.getElementById("Content-Anim").appendChild(play);
                    ';*/
                }
                
                echo '                      </script>
                                        </td>
                                    </tr>
                            </table>
                        </td>
                    </tr>
                </table>';
        }
    }
  
    // Gacha Lottery
    $gacha_lottery = new GachaLottery();

    $gacha_lottery->show();

?>