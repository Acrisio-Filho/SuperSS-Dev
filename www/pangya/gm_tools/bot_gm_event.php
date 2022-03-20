<?php
    // Arquivo bot_gm_event.php
    // Criado em 10/12/2020 as 22:17 por Acrisio
    // Ferramenta do Bot GM Event

    include_once("source/gm_tools_base.inc");
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once('source/dynamic_tag.inc');

    class BotGMEvent extends GMTools {

        private $events = [
            'times' => [],
            'rewards' => []
        ];

        protected function getName() {
            return "Bot GM Event";
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

            if (isset($_POST['edit-time']) && $_POST['edit-time'] == 'edit') {

                $this->editTime();

            }else if (isset($_POST['add-time']) && $_POST['add-time'] == 'add') {

                $this->addTime();

            }else if (isset($_POST['edit-reward']) && $_POST['edit-reward'] == 'edit') {

                $this->editReward();

            }else if (isset($_POST['add-reward']) && $_POST['add-reward'] == 'add') {

                $this->addReward();

            }else if (isset($_POST['remove-time']) && $_POST['remove-time'] == 'remove') {

                $this->removeTime();
            
            }else if (isset($_POST['remove-reward']) && $_POST['remove-reward'] == 'remove') {

                $this->removeReward();

            }
        }

        protected function editTime() {

            if (!isset($_POST['id']) || !isset($_POST['inicio_time']) || !isset($_POST['fim_time']) 
                || !isset($_POST['channel_id']) || !isset($_POST['valid']) || $_POST['id'] == ''
                || $_POST['inicio_time'] == '' || $_POST['fim_time'] == '' || $_POST['channel_id'] == ''
                || $_POST['valid'] == '' || is_nan($_POST['id']) || is_nan($_POST['channel_id']) || is_nan($_POST['valid'])) {

                $this->putLog("[Edit Time] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateBotGMEventTime ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ == $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateBotGMEventTime(?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateBotGMEventTime(?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('i', $_POST['id']);
            $params->add('s', $_POST['inicio_time']);
            $params->add('s', $_POST['fim_time']);
            $params->add('i', $_POST['channel_id']);
            $params->add('i', $_POST['valid']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc update bot gm event time. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_BOT_GM_EVENT_TIME);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc update bot gm event time, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_BOT_GM_EVENT_TIME);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc update bot gm event time, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_BOT_GM_EVENT_TIME);

                return;
            }

            // OK
            $this->setError('Bot GM Event Time editado com sucesso', null);
        }

        protected function addTime() {

            if (!isset($_POST['inicio_time']) || !isset($_POST['fim_time']) || !isset($_POST['channel_id']) 
                || !isset($_POST['valid']) || $_POST['inicio_time'] == '' || $_POST['fim_time'] == '' || $_POST['channel_id'] == ''
                || $_POST['valid'] == ''|| is_nan($_POST['channel_id']) || is_nan($_POST['valid'])) {

                $this->putLog("[Add Time] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertBotGMEventTime ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertBotGMEventTime(?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertBotGMEventTime(?, ?, ?, ?)';

            $params->clear();
            $params->add('s', $_POST['inicio_time']);
            $params->add('s', $_POST['fim_time']);
            $params->add('i', $_POST['channel_id']);
            $params->add('i', $_POST['valid']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc insert bot gm event time. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_BOT_GM_EVENT_TIME);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc insert bot gm event time, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_INSERT_BOT_GM_EVENT_TIME);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc insert bot gm event time, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_BOT_GM_EVENT_TIME);

                return;
            }

            // OK
            $this->setError('Bot GM Event Time adicionado com sucesso', null);
        }

        protected function editReward() {
            
            if (!isset($_POST['id']) || !isset($_POST['typeid']) || !isset($_POST['qntd']) || !isset($_POST['qntd_time'])
                || !isset($_POST['rate']) || !isset($_POST['valid']) || $_POST['id'] == '' || $_POST['typeid'] == ''
                || $_POST['qntd'] == '' || $_POST['qntd_time'] == '' || $_POST['rate'] == '' || $_POST['valid'] == ''
                || is_nan($_POST['id']) || is_nan($_POST['typeid']) || is_nan($_POST['qntd']) || is_nan($_POST['qntd_time'])
                || is_nan($_POST['rate']) || is_nan($_POST['valid'])) {

                $this->putLog("[Edit Reward] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateBotGMEventReward ?, ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateBotGMEventReward(?, ?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateBotGMEventReward(?, ?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('i', $_POST['id']);
            $params->add('i', $_POST['typeid']);
            $params->add('i', $_POST['qntd']);
            $params->add('i', $_POST['qntd_time']);
            $params->add('i', $_POST['rate']);
            $params->add('i', $_POST['valid']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc update bot gm event reward. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_BOT_GM_EVENT_REWARD);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc update bot gm event reward, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_BOT_GM_EVENT_REWARD);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc update bot gm event reward, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_BOT_GM_EVENT_REWARD);

                return;
            }

            // OK
            $this->setError('Bot GM Event Reward editado com sucesso', null);
        }

        protected function addReward() {

            if (!isset($_POST['typeid']) || !isset($_POST['qntd']) || !isset($_POST['qntd_time'])
                || !isset($_POST['rate']) || !isset($_POST['valid']) || $_POST['typeid'] == ''
                || $_POST['qntd'] == '' || $_POST['qntd_time'] == '' || $_POST['rate'] == '' || $_POST['valid'] == ''
                || is_nan($_POST['typeid']) || is_nan($_POST['qntd']) || is_nan($_POST['qntd_time'])
                || is_nan($_POST['rate']) || is_nan($_POST['valid'])) {

                $this->putLog("[Add Reward] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertBotGMEventReward ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertBotGMEventReward(?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertBotGMEventReward(?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('i', $_POST['typeid']);
            $params->add('i', $_POST['qntd']);
            $params->add('i', $_POST['qntd_time']);
            $params->add('i', $_POST['rate']);
            $params->add('i', $_POST['valid']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc insert bot gm event reward. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_BOT_GM_EVENT_REWARD);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc insert bot gm event reward, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_INSERT_BOT_GM_EVENT_REWARD);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc insert bot gm event reward, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_BOT_GM_EVENT_REWARD);

                return;
            }

            // OK
            $this->setError('Bot GM Event Reward editado com sucesso', null);
        }

        protected function removeTime() {
            
            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id'])) {

                $this->putLog("[Remove Time] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteBotGMEventTime ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcDeleteBotGMEventTime(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteBotGMEventTime(?)';

            $params->clear();
            $params->add('i', $_POST['id']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc delete bot gm event time. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_BOT_GM_EVENT_TIME);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc delete bot gm event time, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_DELETE_BOT_GM_EVENT_TIME);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc delete bot gm event time, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_BOT_GM_EVENT_TIME);

                return;
            }

            // OK
            $this->setError('Bot GM Event Time deletado com sucesso', null);
        }

        protected function removeReward() {

            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id'])) {

                $this->putLog("[Remove Reward] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteBotGMEventReward ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcDeleteBotGMEventReward(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteBotGMEventReward(?)';

            $params->clear();
            $params->add('i', $_POST['id']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc delete bot gm event reward. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_BOT_GM_EVENT_REWARD);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc delete bot gm event reward, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_DELETE_BOT_GM_EVENT_REWARD);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc delete bot gm event reward, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_BOT_GM_EVENT_REWARD);

                return;
            }

            // OK
            $this->setError('Bot GM Event Reward deletado com sucesso', null);
        }

        public function show() {

            // Document
            $document = new DynamicTag('html');
            $document->lang = "pt_br";

            // Head
            $head = new DynamicTag('head');
            $title = new DynamicTag('title');

            $title->addChild("Bot GM Event Tool");

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

                .title-table-time {
                    margin-top: 1em;
                    display: grid;
                    justify-content: center;
                    grid-template-columns: 20px 150px 150px 120px 70px 200px;
                    
                }

                .title-table-time > text + text {
                    padding-left: 0.7em;
                }

                .title-table-reward {
                    margin-top: 1em;
                    display: grid;
                    justify-content: center;
                    grid-template-columns: 20px 120px 90px 110px 80px 80px 210px;
                    
                }

                .title-table-reward > text + text {
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
                <font size="20em">Bot GM Event Tool</font>
            </div>
            <div class="table">
                <div style="display: flex; justify-content: center; font-size: 1.5em; font-weight: bold;">Times</div>
                <div class="div-message">'.$this->showError().'</div>
                <div class="title-table-time" style="margin-left: 0.7em; justify-content: flex-start;">
                    <text>ID</text>
                    <text>INICIO TIME</text>
                    <text>FIM TIME</text>
                    <text>CHANNEL ID</text>
                    <text>VALID</text>
                    <text>REGISTRY DATE</text>
                </div>'.(function(){

                    $ret = '';

                    foreach ($this->events['times'] as $ev) {

                        $ret .= '<div class="event">
                            <div class="title-table-time event-title g2">
                                <text>'.$ev->id.'</text>
                                <text>'.$ev->inicio_time.'</text>
                                <text>'.$ev->fim_time.'</text>
                                <text>'.$ev->channel_id.'</text>
                                <text>'.($ev->valid ? 'SIM' : 'NÃO').'</text>
                                <text>'.($ev->reg_date->format('Y-m-d H:i:s.u')).'</text>
                            </div>
                            <div class="g4 event-title edit-event-btn noselect" onClick="editToggle(this)">Edit</div>
                            <div class="g6 event-title remove-btn noselect">
                                <form onsubmit="return confirm(\'Do you want remove this time?\')" action="?" method="POST">
                                    <input type="hidden" name="remove-time" value="remove">
                                    <input type="hidden" name="id" value="'.$ev->id.'">
                                    <input type="submit" value="Remove">
                                </form>
                            </div>
                            <div class="g5 edit-event" style="display: none">
                                <form action="?" method="POST">
                                    <div>
                                        <input type="hidden" name="edit-time" value="edit">
                                        <input type="hidden" name="id" value="'.$ev->id.'">
                                    </div>
                                    <div>
                                        <text>INICIO TIME</text><input type="text" name="inicio_time" value="'.$ev->inicio_time.'">
                                    </div>
                                    <div>
                                        <text>FIM TIME</text><input type="text" name="fim_time" value="'.$ev->fim_time.'">
                                    </div>
                                    <div>
                                        <text>CHANNEL ID</text><input type="text" name="channel_id" value="'.$ev->channel_id.'">
                                    </div>
                                    <div>
                                        <text>VALID</text>
                                        <select name="valid">
                                            <option value="0" '.(!$ev->valid ? 'selected' : '').'>NÃO</option>
                                            <option value="1" '.($ev->valid ? 'selected' : '').'>SIM</option>
                                        </select>
                                    </div>
                                    <div>
                                        <input type="submit" value="Editar"></input>
                                    </div>
                                </form>
                            </div>
                        </div>';
                    }

                    return $ret;
                })().'</div>
                    <div style="display: flex; justify-content: center; margin-top: 1.5em; margin-bottom: 1.5em;">
                        <button onClick="addEvent(this)" class="btn-add-event">Adicionar Time</button>
                    </div>
                    <div style="display: none; justify-content: center; margin-bottom: 1.5em;">
                        <div class="edit-event" style="border: none">
                            <div style="display: flex; justify-content: center; font-size: 1.5em; font-weight: bold; margin-bottom: 1em;">Adicionar Time</div>
                            <form action="?" method="POST">
                                <div>
                                    <input type="hidden" name="add-time" value="add">
                                </div>
                                <div>
                                    <text>INICIO TIME</text><input type="text" name="inicio_time" value="">
                                </div>
                                <div>
                                    <text>FIM TIME</text><input type="text" name="fim_time" value="">
                                </div>
                                <div>
                                    <text>CHANNEL ID</text><input type="text" name="channel_id" value="">
                                </div>
                                <div>
                                    <text>VALID</text>
                                    <select name="valid">
                                        <option value="0">NÃO</option>
                                        <option value="1" selected>SIM</option>
                                    </select>
                                </div>
                                <div>
                                    <input type="submit" value="Adicionar"></input>
                                </div>
                            </form>
                        </div>
                    </div>
                    <div class="table">
                <div style="display: flex; justify-content: center; font-size: 1.5em; font-weight: bold;">Reward</div>
                <div class="title-table-reward" style="margin-left: 0.7em; justify-content: flex-start;">
                    <text>ID</text>
                    <text>TYPEID</text>
                    <text>QNTD</text>
                    <text>QNTD TIME</text>
                    <text>RATE</text>
                    <text>VALID</text>
                    <text>REGISTRY DATE</text>
                </div>'.(function(){

                    $ret = '';

                    foreach ($this->events['rewards'] as $ev) {

                        $ret .= '<div class="event">
                            <div class="title-table-reward event-title g2">
                                <text>'.$ev->id.'</text>
                                <text>'.$ev->typeid.'</text>
                                <text>'.$ev->qntd.'</text>
                                <text>'.$ev->qntd_time.'</text>
                                <text>'.$ev->rate.'</text>
                                <text>'.($ev->valid ? 'SIM' : 'NÃO').'</text>
                                <text>'.($ev->reg_date->format('Y-m-d H:i:s.u')).'</text>
                            </div>
                            <div class="g4 event-title edit-event-btn noselect" onClick="editToggle(this)">Edit</div>
                            <div class="g6 event-title remove-btn noselect">
                                <form onsubmit="return confirm(\'Do you want remove this reward?\')" action="?" method="POST">
                                    <input type="hidden" name="remove-reward" value="remove">
                                    <input type="hidden" name="id" value="'.$ev->id.'">
                                    <input type="submit" value="Remove">
                                </form>
                            </div>
                            <div class="g5 edit-event" style="display: none">
                                <form action="?" method="POST">
                                    <div>
                                        <input type="hidden" name="edit-reward" value="edit">
                                        <input type="hidden" name="id" value="'.$ev->id.'">
                                    </div>
                                    <div>
                                        <text>TYPEID</text><input type="text" name="typeid" value="'.$ev->typeid.'">
                                    </div>
                                    <div>
                                        <text>QNTD</text><input type="text" name="qntd" value="'.$ev->qntd.'">
                                    </div>
                                    <div>
                                        <text>QNTD TIME</text><input type="text" name="qntd_time" value="'.$ev->qntd_time.'">
                                    </div>
                                    <div>
                                        <text>RATE</text><input type="text" name="rate" value="'.$ev->rate.'">
                                    </div>
                                    <div>
                                        <text>VALID</text>
                                        <select name="valid">
                                            <option value="0" '.(!$ev->valid ? 'selected' : '').'>NÃO</option>
                                            <option value="1" '.($ev->valid ? 'selected' : '').'>SIM</option>
                                        </select>
                                    </div>
                                    <div>
                                        <input type="submit" value="Editar"></input>
                                    </div>
                                </form>
                            </div>
                        </div>';
                    }

                    return $ret;
                })().'</div>
                    <div style="display: flex; justify-content: center; margin-top: 1.5em; margin-bottom: 1.5em;">
                        <button onClick="addEvent(this)" class="btn-add-event">Adicionar Reward</button>
                    </div>
                    <div style="display: none; justify-content: center; margin-bottom: 1.5em;">
                        <div class="edit-event" style="border: none">
                            <div style="display: flex; justify-content: center; font-size: 1.5em; font-weight: bold; margin-bottom: 1em;">Adicionar Reward</div>
                            <form action="?" method="POST">
                                <div>
                                    <input type="hidden" name="add-reward" value="add">
                                </div>
                                <div>
                                    <text>TYPEID</text><input type="text" name="typeid" value="">
                                </div>
                                <div>
                                    <text>QNTD</text><input type="text" name="qntd" value="">
                                </div>
                                <div>
                                    <text>QNTD TIME</text><input type="text" name="qntd_time" value="">
                                </div>
                                <div>
                                    <text>RATE</text><input type="text" name="rate" value="">
                                </div>
                                <div>
                                    <text>VALID</text>
                                    <select name="valid">
                                        <option value="0">NÃO</option>
                                        <option value="1" selected>SIM</option>
                                    </select>
                                </div>
                                <div>
                                    <input type="submit" value="Adicionar"></input>
                                </div>
                            </form>
                        </div>
                    </div>
                </div>
            ');

            $document->addChild($body);

            echo '<!DOCTYPE html>'.$document->toString();
        }

        protected function loadEvents($page) {

            $num_events = self::$LIMIT_EVENTS_PER_PAGE * $page;
            
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $query = '';

            // Times Event - Bot GM Event
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT [index], [inicio_time], [fim_time], [channel_id], [valid], [reg_date] FROM '
                    .$db->con_dados['DB_NAME'].'.pangya_bot_gm_event_time ORDER BY [index] DESC OFFSET ? ROWS FETCH NEXT '.self::$LIMIT_EVENTS_PER_PAGE.' ROWS ONLY';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT index, inicio_time, fim_time, channel_id, valid, reg_date FROM '
                    .$db->con_dados['DB_NAME'].'.pangya_bot_gm_event_time ORDER BY index DESC OFFSET ? ROWS FETCH NEXT '.self::$LIMIT_EVENTS_PER_PAGE.' ROWS ONLY';
            else
                $query = 'SELECT `index`, inicio_time, fim_time, channel_id, valid, reg_date FROM '.$db->con_dados['DB_NAME'].'.pangya_bot_gm_event_time ORDER BY `index` DESC LIMIT ?, '.self::$LIMIT_EVENTS_PER_PAGE; // MYSQL

            $params->clear();
            $params->add('i', $num_events);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0) {

                while ($row = $result->fetch_assoc()) {

                    if (isset($row['index']) && isset($row['inicio_time']) && isset($row['fim_time']) 
                        && isset($row['channel_id']) && isset($row['valid']) && isset($row['reg_date'])) {
                        
                        $new_obj = new stdClass();

                        $new_obj->id = $row['index'];
                        $new_obj->inicio_time = $row['inicio_time'];
                        $new_obj->fim_time = $row['fim_time'];
                        $new_obj->channel_id = $row['channel_id'];
                        $new_obj->valid = $row['valid'];
                        $new_obj->reg_date = new DateTime($row['reg_date']);

                        array_push($this->events['times'], $new_obj);
                    }
                }
            } // Error
            else
                $this->putLog("[Error] Não conseguiu pegar os time events do Bot GM Event. Error code(".$db->db->getLastError().")");

            // Rewards - Bot GM Event
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT [index], [typeid], [qntd], [qntd_time], [rate], [valid], [reg_date] FROM '
                    .$db->con_dados['DB_NAME'].'.pangya_bot_gm_event_reward ORDER BY [index] DESC OFFSET ? ROWS FETCH NEXT '.self::$LIMIT_EVENTS_PER_PAGE.' ROWS ONLY';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT index, typeid, qntd, qntd_time, rate, valid, reg_date FROM '
                    .$db->con_dados['DB_NAME'].'.pangya_bot_gm_event_reward ORDER BY index DESC OFFSET ? ROWS FETCH NEXT '.self::$LIMIT_EVENTS_PER_PAGE.' ROWS ONLY';
            else
                $query = 'SELECT `index`, typeid, qntd, qntd_time, rate, valid, reg_date FROM '.$db->con_dados['DB_NAME'].'.pangya_bot_gm_event_reward ORDER BY `index` DESC LIMIT ?, '.self::$LIMIT_EVENTS_PER_PAGE; // MYSQL

            $params->clear();
            $params->add('i', $num_events);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0) {

                while ($row = $result->fetch_assoc()) {
                    
                    if (isset($row['index']) && isset($row['typeid']) && isset($row['qntd']) && isset($row['qntd_time']) 
                        && isset($row['rate']) && isset($row['valid']) && isset($row['reg_date'])) {
                        
                        $new_obj = new stdClass();

                        $new_obj->id = $row['index'];
                        $new_obj->typeid = $row['typeid'];
                        $new_obj->qntd = $row['qntd'];
                        $new_obj->qntd_time = $row['qntd_time'];
                        $new_obj->rate = $row['rate'];
                        $new_obj->valid = $row['valid'];
                        $new_obj->reg_date = new DateTime($row['reg_date']);

                        array_push($this->events['rewards'], $new_obj);
                    }
                }
            } // Error
            else
                $this->putLog("[Error] Não conseguiu pegar os rewards do Bot GM Event. Error code(".$db->db->getLastError().")");
        }
    }

    // Bot GM Event
    $bot = new BotGMEvent();
    $bot->show();
?>