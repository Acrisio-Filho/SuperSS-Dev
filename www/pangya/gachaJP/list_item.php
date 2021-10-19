<?php
    // Arquivo list_item.php
    // Criado em 23/05/2020 as 18:30 por Acrisio
    // Página da lista de itens

    include_once("source/gacha_system.inc");
    include_once("source/table.inc");
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    class ListItem extends GachaSystem {

        protected $table = null;
        protected $request_param = [
            'page' => 1,
            'type' => 1,
            'item_type' => 100
        ];

        public function show() {

            $this->checkInValues();

            $this->listItens($this->request_param['page'], $this->request_param['type'], $this->request_param['item_type']);

            $this->begin();

            echo '<title>Gacha List Itens</title>';

            $this->middle();

            $this->content();

            $this->end();
        }

        protected function style() {

            echo '<style type="text/css">
                    .ui-widget-overlay {
                        position: absolute;
                        top: 0;
                        left: 0;
                        width: 100%;
                        height: 100%;
                        background: #666666;
                        opacity: .5;
                        filter: Alpha(Opacity=50);
                    }
            
                    img {
                        border: 0px;
                    }
            
                    .ticket_count {
                        font-size: 14px;
                        text-align: right;
                    }
            
                    .page_count {
                        text-align: center;
                        padding-top: 16px;
                        font-size: 16px
                    }
            
                    .get_item {
                        margin: 0px 0px 10px 10px;
                        float: left;
                        border: solid 1px #F8C070;
                    }
            
                    .item_name {
                        font-size: 12px;
                        text-align: right;
                        position: absolute;
                        left: 66px;
                        top: 10px;
                        width: 190px;
                        height: 19px;
                    }
            
                    .get_btn {
                        position: absolute;
                        left: 104px;
                        top: 30px;
                        width: 153px;
                        height: 22px;
                    }
            
                    .item_icon {
                        position: absolute;
                        left: 5px;
                        top: 5px;
                        width: 50px;
                        height: 50px;
                    }
                </style>';
        }

        protected function checkInValues() {

            Design::checkIE();

            Design::checkLogin();

            // Verifica o _GET e o _POST
            if (isset($_GET)) {

                if (isset($_GET['page']) && is_numeric($_GET['page']))
                    $this->request_param['page'] = $_GET['page'];

                if (isset($_GET['type']) && is_numeric($_GET['type']))
                    $this->request_param['type'] = $_GET['type'];

                if (isset($_GET['item']) && is_numeric($_GET['item']))
                    $this->request_param['item_type'] = $_GET['item'];
            }

            // Send Item to MailBox
            if (isset($_POST)) {

                if (isset($_POST['hidChangeID']) && is_numeric($_POST['hidChangeID'])) {

                    // Envia o item para o player
                    $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                    $params = $db->params;

                    $params->clear();
                    $params->add('i', PlayerSingleton::getInstance()['UID']);
                    $params->add('i', $_POST['hidChangeID']);

                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcSendGachaJPPlayerItemToMail ?, ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcSendGachaJPPlayerItemToMail(?, ?)';
                    else
                        $query = 'call '.$db->con_dados['DB_NAME'].'.ProcSendGachaJPPlayerItemToMail(?, ?)';

                    if (($result = $db->db->ExecPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0)
                        DebugLog::Log("[GachaJP][List Item] Player enviou item[ID=".$_POST['hidChangeID']."] para o mailbox dele.");
                    else {
                        DebugLog::Log("[GachaJP][List Item] nao conseguiu enviar o item[ID=".$_POST['hidChangeID']."] para o mailbox do player. Error DB: ".$db->db->getLastError());

                        Header("Location: ".LINKS['UNKNOWN_ERROR']);

                        exit();

                        return;
                    }
                }
            }
        }

        protected function content() {

            // Script Get Item
            echo '<script type="text/javascript">
                    function doChange(Name, itemid) {
                        res = confirm("Send the " + Name + " to your mailbox?");
                        if (res == true) {
                            document.forms[0]["hidChangeID"].value = itemid;
                            document.getElementById("chgButton").onclick = new Function("return false;");
                            document.forms[0].submit();
                        }
                    }
                </script>';

            echo '<form name="Form1" method="post" action="'.LINKS['LIST_ITEM'].'?page='.$this->request_param['page'].'&type='.$this->request_param['type'].'&item='.$this->request_param['item_type'].'" id="Form1">';
            
            echo '<input type="hidden" name="hidChangeID" id="hidChangeID" value="0" />';

            $this->menuTop();
            $this->menuSubTop();
            $this->itemList();

            echo '</form>';
        }

        protected function menuTop() {

            echo '<div style="position:absolute; left:190px; top:100px; width:610px; height:45px;">
                    <div style="position:absolute; left:30px; top:0px; width:159px; height:35px;">
                        <a href="'.LINKS['LIST_ITEM'].'?type=1"><img src="img/btn_05.png" width="159" height="35" alt="" /></a>
                    </div>
                    <div style="position:absolute; left:225px; top:0px; width:160px; height:35px;">
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=0"><img src="img/btn_06.png" width="160" height="35"
                                alt="" /></a></div>
                    <div style="position:absolute; left:420px; top:0px; width:160px; height:35px;">
                        <a href="'.LINKS['LIST_ITEM'].'?type=3&item=1"><img src="img/btn_07.png" width="160" height="35"
                                alt="" /></a></div>
                </div>';
        }

        protected function menuSubTop() {

            if ($this->request_param['type'] == 2) {

                // characters Rare
                echo '<div id="btnList_rare" align="center"
                        style="position:absolute; left:190px; top:145px; width:610px; height:65px;">
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=0"><img src="img/chara_btn_01.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=1"><img src="img/chara_btn_02.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=2"><img src="img/chara_btn_04.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=3"><img src="img/chara_btn_03.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=4"><img src="img/chara_btn_05.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=5"><img src="img/chara_btn_06.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=6"><img src="img/chara_btn_07.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=7"><img src="img/chara_btn_08.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=8"><img src="img/chara_btn_09.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=9"><img src="img/chara_btn_13.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=10"><img src="img/chara_btn_spika.png" width="40" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=2&item=100"><img src="img/chara_btn_10.png" width="40" height="65"
                                alt="" /></a>
                    </div>';

            }else if ($this->request_param['type'] == 3) {

                // Itens
                echo '<div id="btnList_new" align="center"
                        style="position:absolute; left:190px; top:145px; width:610px; height:65px;">
                        <a href="'.LINKS['LIST_ITEM'].'?type=3&item=1"><img src="img/chara_btn_12.png" width="120" height="65"
                                alt="" /></a>
                        <a href="'.LINKS['LIST_ITEM'].'?type=3&item=2"><img src="img/chara_btn_11.png" width="120" height="65"
                                alt="" /></a>
                    </div>';
            }
        }

        protected function itemList() {

            // Link page
            echo '<div class="page_count" style="position:absolute; left:190px; top:210px; width:610px; height:24px;">';
                
            if ($this->table != null && !$this->table->isEmpty())
                echo $this->table->makeListLink(LINKS['LIST_ITEM'], '&type='.$this->request_param['type'].'&item='.$this->request_param['item_type']);
                
            echo '</div>';

            // Item list
            echo '<div style="position:absolute; left:200px; top:250px; width:580px; height:286px; overflow:scroll; overflow-x:hidden;">';

            // Table Result
            if ($this->table != null && !$this->table->isEmpty()) {

                foreach($this->table->getCurrentRow() as $itens) {

                    echo '<div class="get_item" style="position:relative; width:265px; height:60px;">
                            <div class="item_icon">
                                <img src="img/th_'.$itens['TYPEID'].'.gif" />
                            </div>
                            <div class="item_name">'.$itens['NAME'].' '.$itens['QNTD'].'</div>
                            <div class="get_btn" id="chgButton" onClick="doChange(\''.$itens['NAME'].'\', '.$itens['ID'].')"
                                style="cursor:pointer;">
                                <img src="img/03_09.png" width="153" height="22" alt="" />
                            </div>
                        </div>';                    
                }
            
            }

            echo '</div>';
        }

        protected function listItens($page, $type, $item_type) {

            if ($page > 0)
                $page--;

            $itens = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);   // UID
            $params->add('i', $page);                                   // Page
            $params->add('i', $type);                                   // Type
            $params->add('i', $item_type);                              // Item Type

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetListGachaSystemPlayerItens ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_ID_" as "ID", "_TYPEID_" as "TYPEID", "_QNTD_" as "QNTD", "_NAME_" as "NAME" from '.$db->con_dados['DB_NAME'].'.ProcGetListGachaSystemPlayerItens(?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetListGachaSystemPlayerItens(?, ?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS'])) {

                $linhas = $row['LINHAS'];

                do {

                    if (isset($row['ID']) && isset($row['TYPEID']) && isset($row['QNTD']) && key_exists('NAME', $row)) {

                        $itens[] = [
                            'ID' => $row['ID'],
                            'TYPEID' => $row['TYPEID'],
                            'QNTD' => $row['QNTD'],
                            'NAME' => isset($row['NAME']) ? mb_convert_encoding($row['NAME'], "UTF-8", "SJIS") : 'EMPTY'
                        ];
                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(50, $page, $linhas, $itens);

        }
    }

    // Página de lista de itens
    $list_item = new ListItem();

    $list_item->show();
?>