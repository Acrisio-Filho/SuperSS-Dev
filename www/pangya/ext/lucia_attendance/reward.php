<?php
    // Arquivo reward.php
    // Criado em 28/02/2020 as 06:26 por Acrisio
    // Definição e Implementação da página Reward

    include_once('source/lucia_attendance.inc');
    include_once('source/itens_data_singleton.inc');
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once('source/debug_log.inc');
    include_once('source/util.inc');

    abstract class ERROR_TYPE {

        const ET_INVALID_PARAMETERS             = 10020;

        // Option 1 Errors
        const ET_OPT1_NUM_ITENS_SEND_INVALID    = 10070;
        const ET_OPT1_NUM_ITENS_VERIFY_INVALID  = 10071;
        const ET_OPT1_2ND_ITEM_NOT_CARD         = 10072;
        const ET_OPT1_1ST_ITEM_QNTD_INVALID     = 10073;
        const ET_OPT1_2ND_ITEM_QNTD_INVALID     = 10074;
        const ET_OPT1_PLAYER_HAD_1ST_ITEM       = 10075;
        const ET_OPT1_CARD_EXCLUDED             = 10076;

        // Options 2 Errors
        const ET_OPT2_CARD_TYPES_NUM_INVALID    = 10090;
        const ET_OPT2_NOT_CARD_OR_CARD_EXCLUDED = 10091;

        // INSERT ITEM MAIL ERROR
        const ET_INSERT_ITEM_MAIL_ERROR         = 10100;
        const ET_FAIL_FINISH_ATTENDANCE_REWARD  = 10101;
    }

    class Reward extends LuciaAttendance {

        private $have_itens = [];
        private $is_loaded_have_itens = false;

        private $error = "";

        private $input_id = [];
        private $entry =    [   
            "OPTIONS" => [
                'TYPE-1' => [],
                'TYPE-2' => []
            ],
            "INPUTS" => [
                'TYPE-1' => [],
                'TYPE-2' => []
            ]
        ];

        private const CARD_EXCLUDED = [112, 122, 123, 124, 125, 126, 127, 229, 230, 231, 232];

        public function show() {

            // Player está bloqueado redireciona para o index
            if ($this->blocked) {

                DebugLog::Log("[Reward] [show-0] tentando burlar o sistema, ele já está bloqueado.");

                // Redireciona para a página principal por que ele está bloqueado
                header("Location: ".LINKS['INDEX']);
                                        
                // sai do script para o navegador redirecionar para a página
                exit();
                
            }

            // Verifica se o player está tentando burlar o sistema
            $this->checkHacking();

            $this->initArrHaveItens();

            $this->checkPost();

            // Verifica aqui se ele não foi bloqueado
            $this->checkBlock();

            if ($this->blocked) {

                DebugLog::Log("[Reward] [show-1] tentando burlar o sistema, ele já está bloqueado.");

                // Redireciona para a página principal por que ele está bloqueado
                header("Location: ".LINKS['INDEX']);
                                        
                // sai do script para o navegador redirecionar para a página
                exit();
            }

            $this->begin();

            echo '<link rel="stylesheet" href="include/main.css">';
            echo '<script language="javascript" src="include/main.js"></script>';

            echo '<title>Lucia Attendance - Reward</title>';

            $this->middle();

            $this->content();

            $this->end();
        }

        private function checkHacking() {
            
            if (!$this->isEnableDay() || PlayerSingleton::getInstance()['COUNT_DAY'] != 6) {

                // Add ao contando de tentativas de burlar o sistema
                $this->addTryHacking();

                DebugLog::Log("[Reward] [checkHacking] Tentando burlar o sistema.");

                // Redireciona para a página principal já que ele conseguiu pegar o item com sucesso
                header("Location: ".LINKS['UNKNOWN_ERROR']);
                                        
                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        private function checkPost() {

            if (isset($_POST) && isset($_POST['check']) && !empty($_POST['check'])) {

                if ($_POST['check'] != 'true') {

                    // Add ao contando de tentativas de burlar o sistema
                    $this->addTryHacking();

                    DebugLog::Log("[Reward] [checkPost] [check] Tentando burlar o sistema.");

                    // Redireciona para a página principal já que ele conseguiu pegar o item com sucesso
                    header("Location: ".LINKS['UNKNOWN_ERROR']);
                                            
                    // sai do script para o navegador redirecionar para a página
                    exit();
                
                }

            }else if (isset($_POST['item']) && is_array($_POST['item']) && count($_POST['item']) > 0) {
        
                $item = ItemSingleton::getInstance()->getItens();
                $card = ItemSingleton::getInstance()->getCards();
        
                $chk_item = new stdClass();
        
                $arr_item = [];
        
                foreach ($_POST['item'] as $el) {
        
                    if (isset($el)) {
        
                        $tmp = json_decode($el);
                        
                        // Bug gerava erros no log, por que não verifica se o $tmp foi preenchido com uma classe json
                        // por que se falhar o json_decode ele retorna null
                        // eu estava setando os membros do objeto sem verificar aí, se tentar ternar membro em uma variável null ele gera avisos, mais cria o objeto ainda,
                        // coisa que eu não queria, eu verifica depois que setava esse membro se o $tmp era válido mas aí não tinha como por que ele criou um novo objeto com avisos.
                        if (isset($tmp) && $tmp) {

                            $tmp->qntd = 1;
                            $tmp->have = false;
                            $tmp->rarity = -1;
                        
                            // Check itens
                            $chk_item->type = getItemTypeName(getItemGroupType($tmp->typeid));
                            $chk_item->_typeid = $tmp->typeid;
        
                            $tmp->type = $chk_item->type;
        
                            if ($chk_item->type != "Card") {
        
                                if (($citem = findItemByCategory($item, $chk_item)) != null) {
            
                                    $tmp->have = $this->checkHaveItem($citem);
                                    $tmp->id = $citem->id;
            
                                    if (($cc = find_if($arr_item, function($el) use(&$tmp) {
                                        return $el->typeid == $tmp->typeid;
                                    })) != null)
                                        $cc->qntd++;
                                    else
                                        array_push($arr_item, $tmp);
                                }
        
                            }else {
        
                                $chk_item->type = getCardTypeName(getCardGroupType($tmp->typeid));
                                $chk_item->_typeid = $tmp->typeid;
        
                                $ccard = findCardByCategory($card, $chk_item);
        
                                if ($ccard) {
        
                                    $tmp->rarity = $ccard->rarity;
                                    $tmp->id = $ccard->id;
        
                                    if (($cc = find_if($arr_item, function($el) use(&$tmp) {
                                        return $el->typeid == $tmp->typeid;
                                    })) != null)
                                        $cc->qntd++;
                                    else
                                        array_push($arr_item, $tmp);
                                }
                            }
                        }
                    }
                }

                if (isset($arr_item) && is_array($arr_item) && count($arr_item) >= 2) {
                    
                    if ($_POST['type'] == 1) {
        
                        if (count($_POST['item']) == 2 && count($arr_item) == 2 && $arr_item[1]->type == "Card" 
                            && $arr_item[0]->qntd == 1 && $arr_item[1]->qntd == 1 && !$arr_item[0]->have && !in_array($arr_item[1]->id, Reward::CARD_EXCLUDED)) {
                            
                            // Finish Item Reward
                            $this->reward_finish($arr_item, 1);
                                    
                        }else {

                            if (count($_POST['item']) != 2) {

                                $err = "Número de itens enviado é invalido para a option 1. Num Itens: ".count($_POST['item']);
                                $code = ERROR_TYPE::ET_OPT1_NUM_ITENS_SEND_INVALID;

                            }else if (count($arr_item) != 2) {

                                $err = "Número de itens depois da verificação é diferente de 2. Num Itens: ".count($arr_item);;
                                $code = ERROR_TYPE::ET_OPT1_NUM_ITENS_VERIFY_INVALID;
                            
                            }else if ($arr_item[1]->type != "Card") {

                                $err = "Segundo item não é um card. TYPE: ".$arr_item[1]->type;
                                $code = ERROR_TYPE::ET_OPT1_2ND_ITEM_NOT_CARD;
                            
                            }else if ($arr_item[0]->qntd != 1) {

                                $err = "Primeiro item quantidade diferente de 1. Quantidade: ".$arr_item[0]->qntd;
                                $code = ERROR_TYPE::ET_OPT1_1ST_ITEM_QNTD_INVALID;

                            }else if ($arr_item[1]->qntd != 1) {

                                $err = "Segundo item quantidade diferente de 1. Quantidade: ".$arr_item[1]->qntd;
                                $code = ERROR_TYPE::ET_OPT1_2ND_ITEM_QNTD_INVALID;

                            }else if ($arr_item[0]->have) {

                                $err = "O player já tem o primeiro item. TYPEID: ".$arr_item[0]->typeid." ID: ".$arr_item[0]->id;
                                $code = ERROR_TYPE::ET_OPT1_PLAYER_HAD_1ST_ITEM;

                            }else if (in_array($arr_item[1]->id, Reward::CARD_EXCLUDED)) {

                                $err = "Tentou pegar um card excluído. TYPEID: ".$arr_item[1]->typeid." ID:".$arr_item[1]->id;
                                $code = ERROR_TYPE::ET_OPT1_CARD_EXCLUDED;
                            }

                            // Add ao contando de tentativas de burlar o sistema
                            $this->addTryHacking();

                            DebugLog::Log("[Reward] Tentando burlar o sistema. ERROR: ".$err.". Code: ".$code);

                            // Display Error
                            $this->error = "Error code: ".$code;
                        }
        
                    }else if($_POST['type'] == 2) {
        
                        if (count($_POST['item']) == 11 && count($arr_item) >= 4) {
        
                            $obj = new stdClass();
                            $obj->error = false;
                            $obj->normal = 0;
                            $obj->rare = 0;
                            $obj->super_rare = 0;
                            $obj->secret = 0;
        
                            foreach ($arr_item as $el) {
        
                                if ($el && $el->type == "Card" && !in_array($el->id, Reward::CARD_EXCLUDED)) {
        
                                    switch ($el->rarity) {
                                        case 0:
                                            $obj->normal += $el->qntd;
                                            break;
                                        case 1:
                                            $obj->rare += $el->qntd;
                                            break;
                                        case 2:
                                            $obj->super_rare += $el->qntd;
                                            break;
                                        case 3:
                                            $obj->secret += $el->qntd;
                                            break;
                                    }
                                
                                }else {
        
                                    $obj->error = true;
        
                                    break;
                                }
                            }
        
                            if (!$obj->error) {
        
                                if ($obj->normal != 5)
                                    $obj->error = true;
        
                                if ($obj->rare != 3)
                                    $obj->error = true;
        
                                if ($obj->super_rare != 2)
                                    $obj->error = true;
        
                                if ($obj->secret != 1)
                                    $obj->error = true;
        
                                if (!$obj->error) {

                                    // Finish Card Reward
                                    $this->reward_finish($arr_item, 2);
                                    
                                }else {

                                    // Add ao contando de tentativas de burlar o sistema
                                    $this->addTryHacking();

                                    $err = "A quantidade de item de cada tipo está errada. [N=".$obj->normal.", R=".$obj->rare.", SR=".$obj->super_rare.", SC=".$obj->secret."]";
                                    $code = ERROR_TYPE::ET_OPT2_CARD_TYPES_NUM_INVALID;

                                    DebugLog::Log("[Reward] [checkPost] Tentando burlar o sistema. ERROR: ".$err." CODE: ".$code);

                                    // Display Error
                                    $this->error = "Error code: ".$code;
                                }
        
                            }else {
                                
                                // Add ao contando de tentativas de burlar o sistema
                                $this->addTryHacking();

                                $err = "Um dos itens não é card ou é um card excluído. Itens: ".json_encode($arr_item);
                                $code = ERROR_TYPE::ET_OPT2_NOT_CARD_OR_CARD_EXCLUDED;

                                DebugLog::Log("[Reward] [checkPost] Tentando burlar o sistema. ERROR: ".$err." CODE: ".$code);

                                // Display Error
                                $this->error = "Error code: ".$code;
                            }
                            
                        }
                    }
        
                }else {

                    // Add ao contando de tentativas de burlar o sistema
                    $this->addTryHacking();

                    $err = "Parametros que o post recebe estão inválidos";
                    $code = ERROR_TYPE::ET_INVALID_PARAMETERS;

                    DebugLog::Log("[Reward] [checkPost] Tentando burlar o sistema. ERROR: ".$err." CODE: ".$code);

                    // Display Error
                    $this->error = "Error code: ".$code;
                }
            
            }else {

                // Add ao contando de tentativas de burlar o sistema
                $this->addTryHacking();

                DebugLog::Log("[Reward] [checkPost] Tentando burlar o sistema.");

                // Redireciona para a página principal já que ele conseguiu pegar o item com sucesso
                header("Location: ".LINKS['UNKNOWN_ERROR']);
                                        
                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        private function setErrorFailInsertItemMail($items, $db) {

            $err = "Não conseguiu inserir os itens no mail do player. DB ERROR: ".$db->db->getLastError().". Itens: ".json_encode($items);
            $code = ERROR_TYPE::ET_INSERT_ITEM_MAIL_ERROR;

            DebugLog::Log("[reward_finish] ERROR: ".$err.". CODE: ".$code);

            // Display Error
            $this->error = $code;
        }

        private function reward_finish($items, $opt) {
            
            if (!isset($_SESSION))
                session_start();

            //----------- Adiciona os itens e o CP do player -----------
            
            // Gera Itens de itens

            $str = '';
            $b = false;

            foreach ($items as $el) {

                if (!$b)
                    $b = true;
                else
                    $str .= '|';

                $str .= $el->typeid.';'.$el->qntd;
            }

            $count_item = count($items);

            // Insert DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);
            $params->add('i', $count_item);
            $params->add('s', $str);
            
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'DECLARE @RET INT; EXEC @RET = '.$db->con_dados['DB_NAME'].'.ProcRegisterLuciaAttendanceReward ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_MSG_ID_" as "MSG_ID", "_LOG_ID_" as "LOG_ID" from '.$db->con_dados['DB_NAME'].'.ProcRegisterLuciaAttendanceReward(?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcRegisterLuciaAttendanceReward(?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                do {

                    while ($row = $result->fetch_assoc()) {

                        if (isset($row['MSG_ID']) && isset($row['LOG_ID'])) {

                            // Log
                            DebugLog::Log("[reward_finish] Inseriu itens com sucesso. MSG_ID: ".$row['MSG_ID']." LOG_ID: ".$row['LOG_ID'].". Itens: ".json_encode($items));

                            // Aqui set o dia que o player pegou o ultimo item
                            $params->clear();
                            $params->add('i', PlayerSingleton::getInstance()['UID']);

                            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcFinishLuciaAttendanceReward ?';
                            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcFinishLuciaAttendanceReward(?)';
                            else
                                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcFinishLuciaAttendanceReward(?)';

                            if ($db->db->execPreparedStmt($query, $params->get()) != null && $db->db->getLastError() == 0) {

                                // Passa Para o Reward Notice
                                if (isset($_SESSION['REWARD_ITEM']))
                                    unset($_SESSION['REWARD_ITEM']);

                                $_SESSION['REWARD_ITEM'] = ['items' => $items, 'option' => $opt];

                                // Redireciona para a página de reward notice já que ele conseguiu pegar o item com sucesso
                                header("Location: ".LINKS['REWARD_NOTICE']);
                                                        
                                // sai do script para o navegador redirecionar para a página
                                exit();
                            
                            }else {

                                $err = "Não conseguiu finalizar o Lucia Attendance reward. MSG_ID: ".$row['MSG_ID']." LOG_ID: ".$row['LOG_ID'].". DB ERROR: ".$db->db->getLastError().". Itens: ".json_encode($items);
                                $code = ERROR_TYPE::ET_FAIL_FINISH_ATTENDANCE_REWARD;

                                DebugLog::Log("[reward_finish] ERROR: ".$err.". CODE: ".$code);

                                // Display Error
                                $this->error = $code;

                                return; // Segue com o app para mostrar o error
                            }
                        }
                    }

                } while ($result->next_result() && ($result->get_result()) && $db->db->getLastError() == 0);

                // Error;
                $this->setErrorFailInsertItemMail($items, $db);
                
            }else   // Error
                $this->setErrorFailInsertItemMail($items, $db);

            //--------------------- END SAVE ITENS ---------------------
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
        
            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0
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
                if ($item->type != 'Setitem') {

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
                                            if ($el['typeid'] == $el2)
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
                                            if ($el['typeid'] == $el2)
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
                                            if($el['typeid'] == $el2)
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

        private function makeOptions() {

            // TYPE-1
            $this->makeOptionsData(1, 1, "item", "Item", "Item");
            $this->makeOptionsData(2, 1, "card", "ALL", "Card");

            // TYPE-2
            $this->makeOptionsData(1, 2, "card", "Normal", "Normal");
            $this->makeOptionsData(2, 2, "card", "Normal");
            $this->makeOptionsData(3, 2, "card", "Normal");
            $this->makeOptionsData(4, 2, "card", "Normal");
            $this->makeOptionsData(5, 2, "card", "Normal");
            $this->makeOptionsData(6, 2, "card", "Rare", "Rare");
            $this->makeOptionsData(7, 2, "card", "Rare");
            $this->makeOptionsData(8, 2, "card", "Rare");
            $this->makeOptionsData(9, 2, "card", "Super Rare", "Super Rare");
            $this->makeOptionsData(10, 2, "card", "Super Rare");
            $this->makeOptionsData(11, 2, "card", "Secret", "Secret");

            if (is_array($this->input_id) && count($this->input_id) > 0) {
                
                echo '<script>
                    // add id ao arr_opts
                    ';

                foreach ($this->input_id as $el)
                    echo 'arr_opts.push(\''.$el.'\');
                    ';

                echo '</script>';
            
            }else {

                DebugLog::Log("[makeOptions] Não conseguiu criar os options e os inputs.");

                return false;
            }

            return true;
        }

        private function error_msg() {

            $ret = '';

            if (!empty($this->error))
                $ret = '<font id="Error-Msg" style="font-size: 20px; font-weight: bold; color: red;">Error: '.$this->error.'</font>';

            return $ret;
        }

        private function content() {

            // Make Options
            if ($this->makeOptions()) {

                echo '<table width="800" height="553" border="0" cellspacing="0" cellpadding>
                        <tr height="70">
                            <td></td>
                        </tr>
                        <tr>
                            <td align="center" vAlign="middle">
                                <div style="width: 557px;">
                                    <table width="100%" height="100%" border="0" cellspacing="0" cellpadding="0" cell-colapse="colapse">
                                        <tr valign="bottom" style="font-weight: bold;">
                                            <td id="aba-1" class="aba ab-left" align="left" onclick="changeAba(this, 1)">Item</td>
                                            <td id="aba-2" class="aba ab-right" align="left" onclick="changeAba(this, 2)">Card</td>
                                            <td width="397">
                                                <table height="100%" width="100%" style="border-bottom: 2px solid #46cec9">
                                                    <tr>
                                                        <td align="right">
                                                            <table width="80" height="100%" cellspacing="0" cellpadding="0" border="0">
                                                                <tr>
                                                                    <td id="aba-help" class="aba-help" align="center" vAlign="middle" onclick="toggleHelp(this)">
                                                                        ?
                                                                    </td>
                                                                </tr>
                                                            </table>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td id="aba-help-content" colspan="3" bgColor="#46cec9" vAlign="middle" align="center" style="padding-bottom: 2px">
                                                <table width="552" border="0" cellspacing="0" cellpadding="0" style="background-color: #fff">
                                                    <tr height="30px">
                                                        <td align="center">
                                                            <h2 style="font-weight: bold; font-size: 20px">Condições de prêmiação</h2>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td align="center" vAlign="middle">
                                                            <table width="500" cellspacing="0" cellpadding="0" border="0">
                                                                <tr>
                                                                    <td align="left" vAlign="top">
                                                                        <div class="container-opt">
                                                                            <span>
                                                                                '.$this->error_msg().'
                                                                                <br><br>
                                                                                <h2>Prêmios que vai ganhar:</h2><br>
                                                                                - 50CP;<br>
                                                                                - 1 item e 1 card a escolha da lista OU 11 cards(*) a escolha da lista.<br>
                                                                                <br>
                                                                                * Tipos e quantidade de cada card:<br>
                                                                                &nbsp;&nbsp;&nbsp; - 1 Card Secret;<br>
                                                                                &nbsp;&nbsp;&nbsp; - 2 Card Super Rare;<br>
                                                                                &nbsp;&nbsp;&nbsp; - 3 Card Rare;<br>
                                                                                &nbsp;&nbsp;&nbsp; - 5 Card Normal.<br>
                                                                            </span>
                                                                            <span>
                                                                                <br><br>
                                                                                <h2>Pesquisa</h2><br>
                                                                                Item e Card:<br>
                                                                                - Nome;<br>
                                                                                - Categória;<br>
                                                                                - Número(ID).<br>
                                                                                <br>
                                                                                Card:<br>
                                                                                - Volume;<br>
                                                                                - Raridade.<br>
                                                                            </span>
                                                                        </div>
                                                                    </td>
                                                                </tr>
                                                            </table>
                                                        </td>
                                                    </tr>
                                                    <tr height="40px">
                                                        <td align="center">
                                                            <button id="btn-help" class="btn-reward" onclick="btnHelp(this)">Ok</button>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </td>
                                            <td id="aba-content-1" colspan="3" bgColor="#46cec9" vAlign="middle" align="center" style="padding-bottom: 2px">
                                                <table width="552" border="0" cellspacing="0" cellpadding="0" style="background-color: #fff">
                                                    <tr height="30px">
                                                        <td align="center">
                                                            <h2 style="font-weight: bold; font-size: 20px">Escolha seu prêmio</h2>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td align="center" vAlign="middle">
                                                            <table width="500" cellspacing="0" cellpadding="0" border="0">
                                                                <tr>
                                                                    <td align="center" vAlign="top">
                                                                        <div class="container-opt">';

                // Options Type 1
                if (isset($this->entry['OPTIONS']['TYPE-1']) && is_array($this->entry['OPTIONS']['TYPE-1']) && count($this->entry['OPTIONS']['TYPE-1']) > 0) {

                    foreach ($this->entry['OPTIONS']['TYPE-1'] as $el)
                        echo $el;
                }
                                                                
                echo '													</div>
                                                                    </td>
                                                                </tr>
                                                            </table>
                                                        </td>
                                                    </tr>
                                                    <tr height="40px">
                                                        <td align="center">
                                                            <form method="POST">
                                                                <input type="hidden" name="type" value="1">';
                                                                
                // Inputs Type 1
                if (isset($this->entry['INPUTS']['TYPE-1']) && is_array($this->entry['INPUTS']['TYPE-1']) && count($this->entry['INPUTS']['TYPE-1']) > 0) {

                    foreach ($this->entry['INPUTS']['TYPE-1'] as $el)
                        echo $el;
                }
                
                echo '                                          <input class="btn-reward" type="submit" name="getReward" onclick="return btnReward(this, 1)" value="Get item reward">
                                                            </form>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </td>
                                            <td id="aba-content-2" colspan="3" bgColor="#46cec9" vAlign="middle" align="center" style="padding-bottom: 2px">
                                                <table width="552" border="0" cellspacing="0" cellpadding="0" style="background-color: #fff">
                                                    <tr height="30px">
                                                        <td align="center">
                                                            <h2 style="font-weight: bold; font-size: 20px">Escolha seu prêmio</h2>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td align="center" vAlign="middle">
                                                            <table width="500" cellspacing="0" cellpadding="0" border="0">
                                                                <tr>
                                                                    <td align="center" vAlign="top">
                                                                        <div class="container-opt">';
                
                // Options Type 2
                if (isset($this->entry['OPTIONS']['TYPE-2']) && is_array($this->entry['OPTIONS']['TYPE-2']) && count($this->entry['OPTIONS']['TYPE-2']) > 0) {

                    foreach ($this->entry['OPTIONS']['TYPE-2'] as $el)
                        echo $el;
                }

                echo '													</div>
                                                                    </td>
                                                                </tr>
                                                            </table>
                                                        </td>
                                                    </tr>
                                                    <tr height="40px">
                                                        <td align="center">
                                                            <form method="POST">
                                                                <input type="hidden" name="type" value="2">';

                // Inputs Type 2
                if (isset($this->entry['INPUTS']['TYPE-2']) && is_array($this->entry['INPUTS']['TYPE-2']) && count($this->entry['INPUTS']['TYPE-2']) > 0) {

                    foreach ($this->entry['INPUTS']['TYPE-2'] as $el)
                        echo $el;
                }
                
                echo '                                          <input class="btn-reward" type="submit" name="getReward" onclick="return btnReward(this, 2)" value="Get card reward">
                                                            </form>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </td>
                                        </tr>
                                    </table>
                                </div>
                            </td>
                        </tr>
                        <tr height="15">
                            <td></td>
                        </tr>
                    </table>';

                // Init
                echo '<script>init()</script>';
            }
        }

        private function makeOptionsData($id, $opt, $name, $filter_rarity = "ALL", $title = null) {

            $str_option = '<div id="drop-'.$id.'-'.$opt.'" class="dropOpt '.($id == 1 ? 'show' : '').'">
            '.($title != null ? '<h2 style="font-size: 20px" align="left">'.$title.'</h2>' : '').'
            <div class="dropdown">
            <div id="sel-'.$id.'-'.$opt.'" onclick="myFunction(\''.$id.'-'.$opt.'\')" class="dropbtn">Select '.$name.'</div>
            <div id="myDropdown-'.$id.'-'.$opt.'" class="dropdown-content '.($id == 1 ? 'show' : '').'">
                <span class="search" id="myInput-'.$id.'-'.$opt.'-SPAN"><input type="text" class="search-inpt" id="myInput-'.$id.'-'.$opt.'" placeholder="Search..." onkeyup="filterFunction(this, \''.$id.'-'.$opt.'\')"></span>
                <fieldset id="fieldSetlist-'.$id.'-'.$opt.'" class="content" onclick="myClick(event, \''.$id.'-'.$opt.'\')">';
                
            if ($name == "item") {
                
                // Itens
                $str_option .= $this->itemArr();
    
            }else if ($name == "card") {
                
                // Cards
                $str_option .= $this->cardArr($filter_rarity);
    
            }
        
            $str_option .= '</fieldset>
            </div>
            </div>
            </div>';

            $str_input = '<input id="item-'.$id.'-'.$opt.'" type="hidden" name="item[]" value="undefined">';

            // Optins
            array_push($this->entry['OPTIONS']['TYPE-'.$opt], $str_option);

            // Inputs
            array_push($this->entry['INPUTS']['TYPE-'.$opt], $str_input);
            
            // Inputs Id
            array_push($this->input_id, $id.'-'.$opt);
        }

        private function itemArr() {

            $item = ItemSingleton::getInstance()->getItens();
        
            $d = false;

            $str = '';
        
            foreach ($item as $el) {
        
                $d = false;
        
                foreach($el as $k => $ell) {
        
                    if (!$d) {
        
                        $d = true;
        
                        $str .= '<span name="'.$k.'" class="category">'.$k.'</span>';
                    }
        
                    foreach($ell as $el2) {
                        
                        $el2->have = $this->checkHaveItem($el2);
                        
                        $str .= '<span name="'.$el2->name.'" id="'.$el2->_typeid.'" category="'.$k.'" class="lid'.($el2->have ? ' have' : '').'"><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td align="center" vAlign="middle"><table width="100%" border="0" cellspacing="2" cellpadding="0"><tr><td style="width: 25px; padding-left: 5px;">'.$el2->id.'</td>'.($el2->have ? '<td width="30">&#10060;</td>' : '').'<td align="left" width="20%"><img src="'.BASE_IMG.'item/'.strtolower(urlencode($el2->icon)).'.png" alt="'.$el2->description.'" /></td><td>'.$el2->name.'</td></tr></table></td></tr></table></span>';
                    }
                }
            }

            return $str;
        }
        
        private function cardArr($type_rarity_filter = "ALL") {
        
            $card = ItemSingleton::getInstance()->getCards();
        
            $d = false;
            $e = false;

            $str = '';
        
            foreach ($card as $el) {
        
                $d = false;
        
                foreach ($el as $k => $ell) {
        
                    if ($k == $type_rarity_filter || $type_rarity_filter == "ALL") {
        
                        if (!$d) {
        
                            $d = true;
        
                            $str .= '<span name="'.$k.'" class="rarity">'.$k.'</span>';
                        }
        
                        foreach ($ell as $elll) {
        
                            $e = false;
        
                            foreach ($elll as $kk => $el3) {
        
                                if (!$e) {
        
                                    $e = true;
                    
                                    $str .= '<span name="'.$kk.'" class="category">'.$kk.'</span>';
                                }
        
                                foreach ($el3 as $el2)
                                    if (!in_array($el2->id, Reward::CARD_EXCLUDED))
                                        $str .= '<span name="'.$el2->name.'" id="'.$el2->_typeid.'" category="'.$kk.', '.$k.'" class="lid"><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td align="center" vAlign="middle"><table width="100%" border="0" cellspacing="2" cellpadding="0"><tr><td style="width: 25px; padding-left: 5px;">'.$el2->id.'</td><td align="left" width="20%"><img src="'.BASE_IMG.'item/'.strtolower(urlencode($el2->icon)).'.png" alt="'.$el2->description.'" /></td><td width="40" align="left"> Vol.'.$el2->volume.'</td><td width="25">'.getRarityBref($el2->rarity).'</td><td>'.$el2->name.'</td></tr></table></td></tr></table></span>';
                            }
                        }
                    }
                }
            }

            return $str;
        }
    }

    // Reward
    $reward = new Reward();

    $reward->show();
?>