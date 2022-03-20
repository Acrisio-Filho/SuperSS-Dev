<?php
    // Arquivo login_reward.php
    // Criado em 10/12/2020 as 19:20 por Acrisio
    // Ferramenta do Login Reward Event

    include_once("source/gm_tools_base.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');
    
    include_once('source/dynamic_tag.inc');

    class LoginReward extends GMTools {

        private $events = [];

        protected function getName() {
            return "Login Reward";
        }

        public function __construct() {

            $this->checkLoginAndCapacity();

            $this->checkGetAndPost();

            $this->loadEvents($this->page);
        }

        protected function checkGetAndPost() {

            if (!isset($_POST) || !isset($_GET))
                return;

            if (isset($_GET['page']) && !is_nan($_GET['page']))
                $this->page = $_GET['page'];

            if (isset($_POST['edit-event']) && $_POST['edit-event'] == 'edit') {

                $this->editEvent();

            }else if (isset($_POST['add-event']) && $_POST['add-event'] == 'add') {

                $this->addEvent();

            }else if (isset($_POST['remove-event']) && $_POST['remove-event'] == 'remove') {

                $this->removeEvent();
            
            }
        }

        protected function editEvent() {

            if (!isset($_POST['id']) || !isset($_POST['name']) || !isset($_POST['type']) || !isset($_POST['days_to_gift']) || !isset($_POST['n_times_gift'])
                || !isset($_POST['item_typeid']) || !isset($_POST['item_qntd']) || !isset($_POST['item_qntd_time']) || !isset($_POST['is_end'])
                || !isset($_POST['end_date']) || $_POST['id'] == '' || $_POST['name'] == '' || $_POST['type'] == '' || $_POST['days_to_gift'] == ''
                || $_POST['n_times_gift'] == '' || $_POST['item_typeid'] == '' || $_POST['item_qntd'] == '' || $_POST['item_qntd_time'] == ''
                || $_POST['is_end'] == '' || $_POST['end_date'] == '' || is_nan($_POST['id']) || is_nan($_POST['type']) 
                || is_nan($_POST['days_to_gift']) || is_nan($_POST['n_times_gift']) || is_nan($_POST['item_typeid']) || is_nan($_POST['item_qntd']) 
                || is_nan($_POST['item_qntd_time']) || is_nan($_POST['is_end']) || $_POST['days_to_gift'] < 1 || strlen($_POST['name']) > 50) {


                $this->putLog("[Edit Event] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (strcasecmp($_POST['end_date'], 'NULL') == 0)
                $_POST['end_date'] = null;

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateLoginRewardEvent ?, ?, ?, ?, ?, ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateLoginRewardEvent(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateLoginRewardEvent(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('i', $_POST['id']);
            $params->add('s', $_POST['name']);
            $params->add('i', $_POST['type']);
            $params->add('i', $_POST['days_to_gift']);
            $params->add('i', $_POST['n_times_gift']);
            $params->add('i', $_POST['item_typeid']);
            $params->add('i', $_POST['item_qntd']);
            $params->add('i', $_POST['item_qntd_time']);
            $params->add('i', $_POST['is_end']);
            $params->add('s', $_POST['end_date']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc update login reward event. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_LOGIN_REWARD_EVENT);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc update login reward event, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_LOGIN_REWARD_EVENT);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc update login reward event, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_LOGIN_REWARD_EVENT);

                return;
            }

            // OK
            $this->setError('Login Reward editado com sucesso', null);
        }

        protected function addEvent() {

            if (!isset($_POST['name']) || !isset($_POST['type']) || !isset($_POST['days_to_gift']) || !isset($_POST['n_times_gift'])
                || !isset($_POST['item_typeid']) || !isset($_POST['item_qntd']) || !isset($_POST['item_qntd_time']) || !isset($_POST['is_end'])
                || !isset($_POST['end_date']) || $_POST['name'] == '' || $_POST['type'] == '' || $_POST['days_to_gift'] == ''
                || $_POST['n_times_gift'] == '' || $_POST['item_typeid'] == '' || $_POST['item_qntd'] == '' || $_POST['item_qntd_time'] == ''
                || $_POST['is_end'] == '' || $_POST['end_date'] == '' || is_nan($_POST['type']) || is_nan($_POST['days_to_gift'])
                || is_nan($_POST['n_times_gift']) || is_nan($_POST['item_typeid']) || is_nan($_POST['item_qntd']) 
                || is_nan($_POST['item_qntd_time']) || is_nan($_POST['is_end']) || $_POST['days_to_gift'] < 1 || strlen($_POST['name']) > 50) {


                $this->putLog("[Add Event] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (strcasecmp($_POST['end_date'], 'NULL') == 0)
                $_POST['end_date'] = null;

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertLoginRewardEvent ?, ?, ?, ?, ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertLoginRewardEvent(?, ?, ?, ?, ?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertLoginRewardEvent(?, ?, ?, ?, ?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('s', $_POST['name']);
            $params->add('i', $_POST['type']);
            $params->add('i', $_POST['days_to_gift']);
            $params->add('i', $_POST['n_times_gift']);
            $params->add('i', $_POST['item_typeid']);
            $params->add('i', $_POST['item_qntd']);
            $params->add('i', $_POST['item_qntd_time']);
            $params->add('i', $_POST['is_end']);
            $params->add('s', $_POST['end_date']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc insert login reward event. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_LOGIN_REWARD_EVENT);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc insert login reward event, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_INSERT_LOGIN_REWARD_EVENT);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc insert login reward event, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_LOGIN_REWARD_EVENT);

                return;
            }

            // OK
            $this->setError('Login Reward adicionado com sucesso', null);
        }

        protected function removeEvent() {
         
            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id'])) {

                $this->putLog("[Remove Event] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteLoginRewardEvent ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcDeleteLoginRewardEvent(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteLoginRewardEvent(?)';

            $params->clear();
            $params->add('i', $_POST['id']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc delete login reward event. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_LOGIN_REWARD_EVENT);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc delete login reward event, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_DELETE_LOGIN_REWARD_EVENT);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc delete login reward event, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_LOGIN_REWARD_EVENT);

                return;
            }

            // OK
            $this->setError('Login Reward deletado com sucesso', null);
        }

        public function show() {

            // Document
            $document = new DynamicTag('html');
            $document->lang = "pt_br";

            // Head
            $head = new DynamicTag('head');
            $title = new DynamicTag('title');

            $title->addChild("Login Reward Tool");

            $head->addChild($title);

            $meta = new DynamicTag('meta');
            $meta->charset = "UTF-8";

            $head->addChild($meta);

            $script = new DynamicTag('script');
            $script->addChild("
                function toggle(el) {

                    if (el) {

                        el_parent = el.parentElement;

                        if (el_parent) {

                            let content = el_parent.querySelector('.event-content');

                            if (content) {

                                if (content.style.display == 'none') {

                                    el.innerText = '▲';

                                    content.style.display = 'block';

                                }else {

                                    el.innerText = '▼';

                                    content.style.display = 'none';
                                }
                            }
                        }
                    }
                }

                function editToggle(el) {

                    const next = el.parentElement.querySelector('.edit-event');

                    if (next) {

                        if (next.style.display) {

                            if (next.style.display == 'none') {

                                next.style.display = 'flex';

                                el.innerText = 'Close';

                            }else {

                                next.style.display = 'none';

                                el.innerText = 'Edit';
                            }
                        }
                    }
                }

                function editItem(el) {

                    const next = el.parentElement.nextElementSibling;

                    if (next) {

                        if (next.style.display) {

                            if (next.style.display == 'none') {

                                next.style.display = 'flex';

                                const i = el.querySelector('i');

                                if (i)
                                    i.innerText = '▲';

                            }else {

                                next.style.display = 'none';

                                const i = el.querySelector('i');

                                if (i)
                                    i.innerText = '▼';
                            }
                        }
                    }
                }

                function addItem(el, type) {

                    const next = el.parentElement.parentElement.querySelector('.' + type);

                    if (next) {

                        if (next.style.display) {

                            if (next.style.display == 'none') {

                                next.style.display = 'flex';

                                if (next.nextElementSibling) {

                                    if (next.nextElementSibling.style.display && next.nextElementSibling.style.display != 'none')
                                        next.nextElementSibling.style.display = 'none';
                                
                                }else if (next.previousElementSibling) {

                                    if (next.previousElementSibling.style.display && next.previousElementSibling.style.display != 'non')
                                        next.previousElementSibling.style.display = 'none';

                                }

                            }else {

                                next.style.display = 'none';
                            }
                        }
                    }
                }

                function addEvent(el) {

                    const next = el.parentElement.nextElementSibling;

                    if (next) {

                        if (next.style.display) {

                            if (next.style.display == 'none') {

                                next.style.display = 'flex';

                            }else {

                                next.style.display = 'none';
                            }
                        }
                    }
                }

                function addSubItem(el, type) {

                    const copy = el.parentElement.parentElement.parentElement.querySelectorAll('.' + type);
    
                    if (copy && copy.length > 0) {
    
                        const last = copy[copy.length -1];
    
                        if (last) {
                            
                            const clone = last.cloneNode(true);
    
                            // Clear
                            Array.prototype.forEach.call(clone.querySelectorAll('input'), el => {
    
                                if (el.value)
                                    el.value = '';
                            });
    
                            el.parentElement.parentElement.parentElement.insertBefore(clone, last.nextElementSibling);
                        }
                    }
                }
    
                function removeSubItem(el, type) {
    
                    const copy = el.parentElement.parentElement.parentElement.querySelectorAll('.' + type);
    
                    if (copy && copy.length > 1) {
    
                        const last = copy[copy.length -1];
    
                        if (last) {
    
                            el.parentElement.parentElement.parentElement.removeChild(last);
                        }
                    }
                }
            ");

            $head->addChild($script);

            $style = new DynamicTag('style');
            $style->addChild("
                * {
                    border: 0;
                    margin: 0;
                    padding: 0;
                    box-sizing: border-box;
                }

                .content {
                    display: flex;
                    flex-direction: column;
                    justify-content: center;
                }

                .intro {
                    margin-top: 2em;
                    margin-bottom: 2em;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                }

                .event {
                    display: grid;
                    justify-content: center;
                    grid-template-areas: 
                    'seta nome edit-btn remove-btn'
                    'edit edit edit edit'
                    'content content content content';
                }

                .event-title {
                    background: rgb(41,41,41);
                    background: linear-gradient(0deg, rgba(41,41,41,1) 0%, rgba(101,118,125,1) 0%, rgba(169,183,222,1) 52%);
                    padding: 0.2em 0.7em;
                }

                .table {
                    display: inline-block;
                    margin: auto;
                }

                .title-table {
                    margin-top: 1em;
                    display: grid;
                    justify-content: center;
                    grid-template-columns: 20px 100px 100px 100px 200px;
                    
                }

                .title-table > text + text {
                    padding-left: 0.7em;
                }

                .round {
                    margin-top: 1em;
                    margin-left: 2em;
                    margin-right: 2em;
                    padding-bottom: 1.0em;
                    border-bottom: 2px ridge black;
                }

                .title-round {
                    margin-top: 1.2em;
                    display: grid;
                    justify-content: flex-start;
                    align-items: center;
                    grid-template-columns: 150px 200px auto auto;
                }

                .title-round > text + text {
                    padding-left: 0.7em;
                }

                .item {
                    margin-top: 1em;
                    margin-left: 2em;
                    margin-right: 2em;
                    padding-bottom: 1.0em;
                    border-bottom: 2px ridge black;
                }

                .title-item {
                    margin-top: 1.2em;
                    display: grid;
                    justify-content: flex-start;
                    align-items: center;
                    grid-template-columns: 100px 60px 100px 60px 200px auto auto;
                }

                .title-item > text + text {
                    padding-left: 0.7em;
                }

                .anchor {
                    margin-top: 1em;
                    margin-right: 0.3em;
                    padding: 0.2em;
                    cursor: pointer;
                }

                .edit-event-btn {
                    margin-top: 1em;
                    margin-left: 0.3em;
                    padding: 0.2em;
                    cursor: pointer;
                    font-weight: bold;
                    min-width: 50px;
                    text-align: center;
                }

                .remove-btn {
                    margin-top: 1em;
                    margin-left: 0.3em;
                    padding: 0.2em;
                    min-width: 70px;
                    cursor: pointer;
                }

                .g1 {
                    grid-area: seta;
                }
                .g2 {
                    grid-area: nome;
                }
                .g3 {
                    grid-area: content;
                }
                .g4 {
                    grid-area: edit-btn;
                }
                .g5 {
                    grid-area: edit;
                }
                .g6 {
                    grid-area: remove-btn;
                }

                .title-content {
                    color: cornflowerblue;
                    font-weight: bolder;
                    font-size: 1.5em;
                }

                .noselect {
                    -webkit-touch-callout: none; /* iOS Safari */
                    -webkit-user-select: none; /* Safari */
                    -khtml-user-select: none; /* Konqueror HTML */
                    -moz-user-select: none; /* Old versions of Firefox */
                    -ms-user-select: none; /* Internet Explorer/Edge */
                    user-select: none; /* Non-prefixed version, currently
                                        supported by Chrome, Edge, Opera and Firefox */
                }

                .btn_add {
                    display: flex;
                    justify-content: space-evenly;
                    margin-top: 1.0em;
                }

                .btn_add button {
                    border: 2px inset lightgray;
                    background-color: cornflowerblue;
                    padding: 0.3em 0.6em;
                    cursor: pointer;
                    color: white;
                    font-weight: bold;
                }

                .btn_add button:hover {
                    background-color: rgb(65, 122, 228);
                }

                .edit-event {
                    justify-content: center;
                    margin-top: 0.7em;
                    border-bottom: 2px ridge black;
                    padding-bottom: 1.2em;
                }

                .edit-event form {
                    display: grid;
                }

                .edit-event div {
                    justify-content: center;
                    display: flex;
                }

                .edit-event div > text, input, select {
                    flex: 1 0 100%;
                    margin: 0.5em;
                }

                .edit-event div > input, select {
                    padding-left: 0.2em;
                    border: 1px solid darkgray;
                }

                .edit-event div > input[type=submit] {
                    border: 2px inset lightgray;
                    background-color: cornflowerblue;
                    color: white;
                    font-weight: bold;
                    cursor: pointer;
                    padding: 0.1em 0;
                }

                .edit-event div > input[type=submit]:hover {
                    background-color: rgb(65, 122, 228);
                }

                .edit-item-btn {
                    cursor: pointer;
                    margin-left: 0.5em;
                    border-left: 1px solid black;
                }

                .remove-item-btn input {
                    margin: 0;
                    padding: 0;
                    background-color: transparent;
                    font-size: 0.9em;
                    border-left: 1px solid black;
                    padding-left: 0.5em;
                    cursor: pointer;
                }

                .remove-btn form {
                    display: flex;
                    justify-content: center;
                    cursor: pointer;
                }

                .remove-btn form > input {
                    margin: 0;
                    padding: 0;
                    background-color: transparent;
                    max-width: 70px;
                    font-weight: bold;
                    cursor: pointer;
                }

                .add-event {
                    display: grid;
                    justify-content: center;
                    margin-top: 1.5em;
                    text-align: center;
                }

                .title-add {
                    display: grid;
                    grid-auto-flow: column;
                    margin-top: 1.5em;
                }

                .add-event input, select {
                    border: 1px solid darkgray;
                    padding: 0.1em 0.2em;
                }

                .add-event input[type=submit] {
                    border: 2px inset lightgray;
                    color: white;
                    background-color: cornflowerblue;
                    cursor: pointer;
                    padding: 0.6em 3em;
                    margin-top: 1.5em;
                    font-weight: bold;
                }

                .add-event select {
                    padding: 0 0.2em;
                    min-width: 148px;
                }

                .add-event input[type=submit]:hover {
                    background-color: rgb(65, 122, 228);
                }

                .sub-title-item {
                    font-size: 1.7em;
                    font-weight: bold;
                    display: block;
                    color: cornflowerblue;
                    text-align: left;
                    margin-left: 0.3em;
                    margin-top: 0.5em;
                }

                .sub-title-item-plus {
                    font-size: 1.9em;
                    font-weight: 900;
                    color: cornflowerblue;
                    text-align: left;
                    margin-right: 0.3em;
                    margin-top: 0.5em;
                    cursor: pointer;
                    display: flex;
                    flex-direction: row;
                    justify-content: space-around;
                    flex-wrap: wrap;
                    min-width: 50px;
                }

                .sub-title-item-plus i:hover {
                    color: rgb(65, 122, 228);
                }

                .sub-title-item-plus :first-child {
                    position: relative;
                    top: -3px;
                }

                .sub-title {
                    display: flex;
                    justify-content: space-between;
                }

                .btn-add-event {
                    border: 2px inset lightgray;
                    background-color: cornflowerblue;
                    padding: 0.3em 0.6em;
                    cursor: pointer;
                    color: white;
                    font-weight: bold;
                }

                .btn-add-event:hover {
                    background-color: rgb(65, 122, 228);
                }

                .div-message {
                    display: flex;
                    justify-content: space-between;
                    background-color: white;
                    font-size: 1.5em;
                    font-weight: bold;
                }

                .div-message > text, i {
                    padding: 0.7em;
                }

                .message-label {
                    color: green;
                }

                .error-label {
                    color: red;
                }

                .message-cross {
                    cursor: pointer;
                    color: darkgrey;
                }

                .message-cross:hover {
                    color: grey;
                }

                .label-event {
                    display: flex;
                    justify-content: center;
                    margin-top: 0.7em;
                    border-bottom: 2px ridge black;
                    padding-bottom: 1.2em;
                }

                .label-event form {
                    display: grid;
                    width: 80%;
                }

                .label-event div {
                    justify-content: space-between;
                    display: grid;
                    grid-template-columns: auto auto;
                }

                .label-event div > text {
                    margin: 0.5em;
                }
            ");

            $head->addChild($style);

            $document->addChild($head);

            // Body
            $body = new DynamicTag('body');
            $body->style = "width: 100%; height: 100%; background-color: #777";

            $body->addChild('<div class="content">
            <div class="intro">
                <font size="20em">Login Reward Tool</font>
            </div>
            <div class="table">
                <div class="div-message">'.$this->showError().'</div>
                <div class="title-table" style="margin-left: 2.2em; justify-content: flex-start;">
                    <text>ID</text>
                    <text>TYPE</text>
                    <text>IS END</text>
                    <text>END DATE</text>
                    <text>REGISTRY DATE</text>
                </div>'.(function(){

                    $ret = '';

                    foreach ($this->events as $ev) {

                        $ret .= '<div class="event">
                            <i class="anchor event-title g1 noselect" onClick="toggle(this)">▼</i>
                            <div class="title-table event-title g2">
                                <text>'.$ev->id.'</text>
                                <text>'.(function() use($ev) {
                                    
                                    $ret_1 = 'N TIME';

                                    switch ($ev->type) {
                                        case 0:
                                        default:
                                            $ret_1 = 'N TIME';
                                        break;
                                        case 1:
                                            $ret_1 = 'FOREVER';
                                        break;
                                    }

                                    return $ret_1;
                                })().'</text>
                                <text>'.($ev->is_end ? 'SIM' : 'NÃO').'</text>
                                <text>'.($ev->end_date == null ? 'NULL' : $ev->end_date->format('Y-m-d')).'</text>
                                <text>'.($ev->reg_date->format('Y-m-d H:i:s.u')).'</text>
                            </div>
                            <div class="g4 event-title edit-event-btn noselect" onClick="editToggle(this)">Edit</div>
                            <div class="g6 event-title remove-btn noselect">
                                <form onsubmit="return confirm(\'Do you want remove this event?\')" action="?" method="POST">
                                    <input type="hidden" name="remove-event" value="remove">
                                    <input type="hidden" name="id" value="'.$ev->id.'">
                                    <input type="submit" value="Remove">
                                </form>
                            </div>
                            <div class="g5 edit-event" style="display: none">
                                <form action="?" method="POST">
                                    <div>
                                        <input type="hidden" name="edit-event" value="edit">
                                        <input type="hidden" name="id" value="'.$ev->id.'">
                                    </div>
                                    <div>
                                        <text>NAME</text><input type="text" name="name" value="'.$ev->name.'">
                                    </div>
                                    <div>
                                        <text>TYPE</text>
                                        <select name="type">
                                            <option value="0" '.($ev->type == 0 ? 'selected' : '').'>N TIME</option>
                                            <option value="1" '.($ev->type == 1 ? 'selected' : '').'>FOREVER</option>
                                        </select>
                                    </div>
                                    <div>
                                        <text>DAYS TO GIFT</text><input type="text" name="days_to_gift" value="'.$ev->days_to_gift.'">
                                    </div>
                                    <div>
                                        <text>N TIMES GIFT</text><input type="text" name="n_times_gift" value="'.$ev->n_times_gift.'">
                                    </div>
                                    <div>
                                        <text>ITEM TYPEID</text><input type="text" name="item_typeid" value="'.$ev->item_typeid.'">
                                    </div>
                                    <div>
                                        <text>ITEM QNTD</text><input type="text" name="item_qntd" value="'.$ev->item_qntd.'">
                                    </div>
                                    <div>
                                        <text>ITEM QNTD TIME</text><input type="text" name="item_qntd_time" value="'.$ev->item_qntd_time.'">
                                    </div>
                                    <div>
                                        <text>IS END</text>
                                        <select name="is_end">
                                            <option value="0" '.(!$ev->is_end ? 'selected' : '').'>NÃO</option>
                                            <option value="1" '.($ev->is_end ? 'selected' : '').'>SIM</option>
                                        </select>
                                    </div>
                                    <div>
                                        <text>END DATE</text><input type="text" name="end_date" value="'.($ev->end_date == null ? 'NULL' : $ev->end_date->format('Y-m-d H:i:s.u')).'">
                                    </div>
                                    <div>
                                        <input type="submit" value="Editar"></input>
                                    </div>
                                </form>
                            </div>
                            <div class="event-content g3" style="display: none">'.
                            (function() use($ev) {
                                
                                return '<div class="label-event">
                                    <form action="?" method="POST">
                                        <div>
                                            <text>ID</text><text>'.$ev->id.'</text>
                                        </div>
                                        <div>
                                            <text>NAME</text><text>'.$ev->name.'</text>
                                        </div>
                                        <div>
                                            <text>TYPE</text>
                                            <text>'.(function() use($ev) {
                                    
                                                $ret_1 = 'N TIME';
            
                                                switch ($ev->type) {
                                                    case 0:
                                                    default:
                                                        $ret_1 = 'N TIME';
                                                    break;
                                                    case 1:
                                                        $ret_1 = 'FOREVER';
                                                    break;
                                                }
            
                                                return $ret_1;
                                            })().'</text>
                                        </div>
                                        <div>
                                            <text>DAYS TO GIFT</text><text>'.$ev->days_to_gift.'</text>
                                        </div>
                                        <div>
                                            <text>N TIMES GIFT</text><text>'.$ev->n_times_gift.'</text>
                                        </div>
                                        <div>
                                            <text>ITEM TYPEID</text><text>'.$ev->item_typeid.'</text>
                                        </div>
                                        <div>
                                            <text>ITEM QNTD</text><text>'.$ev->item_qntd.'</text>
                                        </div>
                                        <div>
                                            <text>ITEM QNTD TIME</text><text>'.$ev->item_qntd_time.'</text>
                                        </div>
                                        <div>
                                            <text>IS END</text>
                                            <text>'.($ev->is_end ? 'SIM' : 'NÃO').'</text>
                                        </div>
                                        <div>
                                            <text>END DATE</text><text>'.($ev->end_date == null ? 'NULL' : $ev->end_date->format('Y-m-d H:i:s.u')).'</text>
                                        </div>
                                    </form>
                                </div>';
                            }
                            )().'
                            </div>
                        </div>';
                    }

                    return $ret;
                })().'</div>
                    <div style="display: flex; justify-content: center; margin-top: 1.5em; margin-bottom: 1.5em;">
                        <button onClick="addEvent(this)" class="btn-add-event">Adicionar evento</button>
                    </div>
                    <div style="display: none; justify-content: center; margin-bottom: 1.5em;">
                        <div class="edit-event" style="border: none">
                            <div style="display: flex; justify-content: center; font-size: 1.5em; font-weight: bold; margin-bottom: 1em;">Adicionar evento</div>
                            <form action="?" method="POST">
                                <div>
                                    <input type="hidden" name="add-event" value="add">
                                </div>
                                <div>
                                    <text>NAME</text><input type="text" name="name" value="">
                                </div>
                                <div>
                                    <text>TYPE</text>
                                    <select name="type">
                                        <option value="0">N TIME</option>
                                        <option value="1">FOREVER</option>
                                    </select>
                                </div>
                                <div>
                                    <text>DAYS TO GIFT</text><input type="text" name="days_to_gift" value="">
                                </div>
                                <div>
                                    <text>N TIMES GIFT</text><input type="text" name="n_times_gift" value="">
                                </div>
                                <div>
                                    <text>ITEM TYPEID</text><input type="text" name="item_typeid" value="">
                                </div>
                                <div>
                                    <text>ITEM QNTD</text><input type="text" name="item_qntd" value="">
                                </div>
                                <div>
                                    <text>ITEM QNTD TIME</text><input type="text" name="item_qntd_time" value="">
                                </div>
                                <div>
                                    <text>IS END</text>
                                    <select name="is_end">
                                        <option value="0">NÃO</option>
                                        <option value="1">SIM</option>
                                    </select>
                                </div>
                                <div>
                                    <text>END DATE</text><input type="text" name="end_date" value="">
                                </div>
                                <div>
                                    <input type="submit" value="Adicionar"></input>
                                </div>
                            </form>
                        </div>
                    </div>
                </div>');

            $document->addChild($body);

            echo '<!DOCTYPE html>'.$document->toString();
        }

        protected function loadEvents($page) {

            $num_events = self::$LIMIT_EVENTS_PER_PAGE * $page;
            
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $query = '';

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT [index], [name], [type], [days_to_gift], [n_times_gift], [item_typeid], [item_qntd], [item_qntd_time], [is_end], [end_date], [reg_date] FROM '
                    .$db->con_dados['DB_NAME'].'.pangya_login_reward ORDER BY [index] DESC OFFSET ? ROWS FETCH NEXT '.self::$LIMIT_EVENTS_PER_PAGE.' ROWS ONLY';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT index, name, type, days_to_gift, n_times_gift, item_typeid, item_qntd, item_qntd_time, is_end, end_date, reg_date FROM '
                    .$db->con_dados['DB_NAME'].'.pangya_login_reward ORDER BY index DESC OFFSET ? ROWS FETCH NEXT '.self::$LIMIT_EVENTS_PER_PAGE.' ROWS ONLY';
            else
                $query = 'SELECT `index`, `name`, `type`, days_to_gift, n_times_gift, item_typeid, item_qntd, item_qntd_time, is_end, end_date, reg_date FROM '
                    .$db->con_dados['DB_NAME'].'.pangya_login_reward ORDER BY `index` DESC LIMIT ?, '.self::$LIMIT_EVENTS_PER_PAGE; // MYSQL

            $params->clear();
            $params->add('i', $num_events);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0) {

                while ($row = $result->fetch_assoc()) {
                    
                    if (isset($row['index']) && isset($row['name']) && isset($row['type']) && isset($row['n_times_gift']) && isset($row['item_typeid'])
                        && isset($row['item_qntd']) && isset($row['item_qntd_time']) && key_exists('end_date', $row) 
                        && isset($row['days_to_gift']) && isset($row['is_end']) && isset($row['reg_date'])) {
                        
                        $new_obj = new stdClass();

                        $new_obj->id = $row['index'];
                        $new_obj->name = $row['name'];
                        $new_obj->type = $row['type'];
                        $new_obj->days_to_gift = $row['days_to_gift'];
                        $new_obj->n_times_gift = $row['n_times_gift'];
                        $new_obj->item_typeid = $row['item_typeid'];
                        $new_obj->item_qntd = $row['item_qntd'];
                        $new_obj->item_qntd_time = $row['item_qntd_time'];
                        $new_obj->end_date = $row['end_date'] == '' ? null : new DateTime($row['end_date']);
                        $new_obj->is_end = $row['is_end'] == 1 ? true : false;
                        $new_obj->reg_date = new DateTime($row['reg_date']);

                        array_push($this->events, $new_obj);
                    }
                }
            } // Error
            else
                $this->putLog("[Error] Não conseguiu pegar os eventos do Login Reward. Error code(".$db->db->getLastError().")");
        }
    }

    // Login Reward
    $login = new LoginReward();
    $login->show();

?>