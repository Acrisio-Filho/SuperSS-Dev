<?php
    // Arquivo golden_time.php
    // Criado em 07/11/2020 as 01:55 por Acrisio
    // Ferramenta do Golden Time Event

    include_once("source/gm_tools_base.inc");
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once('source/dynamic_tag.inc');

    class GoldenTime extends GMTools {

        private $events = [];

        protected function getName() {
            return "Golden Time";
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

            }else if (isset($_POST['edit-round']) && $_POST['edit-round'] == 'edit') {

                $this->editRound();

            }else if (isset($_POST['add-round']) && $_POST['add-round'] == 'add') {

                $this->addRound();

            }else if (isset($_POST['edit-item']) && $_POST['edit-item'] == 'edit') {

                $this->editItem();

            }else if (isset($_POST['add-item']) && $_POST['add-item'] == 'add') {

                $this->addItem();

            }else if (isset($_POST['remove-event']) && $_POST['remove-event'] == 'remove') {

                $this->removeEvent();
            
            }else if (isset($_POST['remove-round']) && $_POST['remove-round'] == 'remove') {

                $this->removeRound();

            }else if (isset($_POST['remove-item']) && $_POST['remove-item'] == 'remove') {

                $this->removeItem();
            }
        }

        protected function editEvent() {

            if (!isset($_POST['id']) || !isset($_POST['type']) || !isset($_POST['begin']) || !isset($_POST['end']) || !isset($_POST['rate'])
                || !isset($_POST['is_end']) || $_POST['id'] == '' || $_POST['type'] == '' || $_POST['begin'] == '' || $_POST['end'] == ''
                || $_POST['rate'] == '' || $_POST['is_end'] == '' || is_nan($_POST['type']) || is_nan($_POST['rate']) || is_nan($_POST['is_end'])
                || is_nan($_POST['id'])) {

                // Log
                $this->putLog('[Edit Event] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (strcasecmp($_POST['end'], 'NULL') == 0)
                $_POST['end'] = null;

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeEvent ?, ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeEvent(?, ?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeEvent(?, ?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('i', $_POST['id']);
            $params->add('i', $_POST['type']);
            $params->add('s', $_POST['begin']);
            $params->add('s', $_POST['end']);
            $params->add('i', $_POST['rate']);
            $params->add('i', $_POST['is_end']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {


                $this->putLog('[Error] Fail exec proc update golden time event. Error code: '.$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_GOLDEN_TIME_EVENT);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog('[Error] Fail in proc update golden time event, result empty.');

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_GOLDEN_TIME_EVENT);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog('[Error] Fail in verify of proc update golden time event, result invalid');

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_GOLDEN_TIME_EVENT);

                return;
            }

            // OK
            $this->setError('Golden Time Event editado com sucesso', null);
        }

        protected function addEvent() {

            if (!isset($_POST['type']) || !isset($_POST['begin']) || !isset($_POST['end']) || !isset($_POST['rate'])
                || !isset($_POST['is_end']) || !isset($_POST['time']) || !isset($_POST['typeid']) || !isset($_POST['qntd'])
                || !isset($_POST['qntd_time']) || !isset($_POST['rate_item']) || empty($_POST['time'])
                || empty($_POST['typeid']) || empty($_POST['qntd']) || empty($_POST['qntd_time']) || empty($_POST['rate_item'])
                || !is_array($_POST['time']) || !is_array($_POST['typeid']) || !is_array($_POST['qntd'])
                || !is_array($_POST['qntd_time']) || !is_array($_POST['rate_item'])
                || $_POST['type'] == '' || $_POST['rate'] == '' || $_POST['is_end'] == ''
                || is_nan($_POST['type']) || is_nan($_POST['rate']) || is_nan($_POST['is_end'])
                || $_POST['begin'] == '' || $_POST['end'] == '') {

                // Log
                $this->putLog('[Add Event] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            // Check input time[]
            foreach ($_POST['time'] as $el) {

                if (!isset($el) || $el == '') {

                    // Log
                    $this->putLog('[Add Event] invalid parameters time');

                    $this->setError('Please insert all fields', 0);

                    return;
                }
            }
            
            // Check input typeid[]
            foreach ($_POST['typeid'] as $el) {

                if (!isset($el) || $el == '') {

                    // Log
                    $this->putLog('[Add Event] invalid parameters typeid');

                    $this->setError('Please insert all fields', 0);

                    return;
                }
            }

            // Check input qntd[]
            foreach ($_POST['qntd'] as $el) {

                if (!isset($el) || $el == '') {

                    // Log
                    $this->putLog('[Add Event] invalid parameters qntd');

                    $this->setError('Please insert all fields', 0);

                    return;
                }
            }

            // Check input qntd_time[]
            foreach ($_POST['qntd_time'] as $el) {

                if (!isset($el) || $el == '') {

                    // Log
                    $this->putLog('[Add Event] invalid parameters qntd_time');

                    $this->setError('Please insert all fields', 0);

                    return;
                }
            }

            // Check input rate_item[]
            foreach ($_POST['rate_item'] as $el) {

                if (!isset($el) || $el == '') {

                    // Log
                    $this->putLog('[Add Event] invalid parameters rate_item');

                    $this->setError('Please insert all fields', 0);

                    return;
                }
            }

            // Verifica se os array itens tem a mesma quantidade
            $check_qntd = count($_POST['typeid']);

            if ($check_qntd != count($_POST['qntd']) || $check_qntd != count($_POST['qntd_time']) || $check_qntd != count($_POST['rate_item'])) {

                $this->putLog('[Add Event] invalid parameters array item quantity.');

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (strcasecmp($_POST['end'], 'NULL') == 0)
                $_POST['end'] = null;

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeEvent ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeEvent(?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeEvent(?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('i', $_POST['type']);
            $params->add('s', $_POST['begin']);
            $params->add('s', $_POST['end']);
            $params->add('i', $_POST['rate']);
            $params->add('i', $_POST['is_end']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog('[Error] Fail exec proc insert golden time event. Error code: '.$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_GOLDEN_TIME_EVENT);
                
                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog('[Error] Fail in proc insert golden time event, result empty.');

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_INSERT_GOLDEN_TIME_EVENT);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog('[Error] Fail in verify of proc insert golden time event, result invalid');

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_GOLDEN_TIME_EVENT);

                return;
            }

            // Adiciona o(s) round(s) e o(s) item(ns) ao golden time event criado
            $golden_time_id = $row['ID'];

            // Round
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeRound ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeRound(?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeRound(?, ?)';

            foreach ($_POST['time'] as $time) {

                $params->clear();
                $params->add('i', $golden_time_id);
                $params->add('s', $time);

                if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                    $this->putLog("[Error][GOLDEN_TIME_ID={$golden_time_id}] Fail exec proc insert golden time round. Error code: ".$db->db->getLastError());

                    $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_GOLDEN_TIME_ROUND + 100);

                    return;
                }

                if ($result == null || ($row = $result->fetch_assoc()) == null) {

                    $this->putLog("[Error][GOLDEN_TIME_ID={$golden_time_id}] Fail in proc insert golden time round, result empty");

                    $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_INSERT_GOLDEN_TIME_ROUND + 100);

                    return;
                }

                if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                    $this->putLog("[Error][GOLDEN_TIME_ID={$golden_time_id}] Fail in verify of proc insert golden time round, result invalid");

                    $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_GOLDEN_TIME_ROUND + 100);

                    return;
                }
            }

            // Item
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeItem ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeItem(?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeItem(?, ?, ?, ?, ?)';

            for ($i = 0; $i < count($_POST['typeid']); $i++) {

                $params->clear();
                $params->add('i', $golden_time_id);
                $params->add('i', $_POST['typeid'][$i]);
                $params->add('i', $_POST['qntd'][$i]);
                $params->add('i', $_POST['qntd_time'][$i]);
                $params->add('i', $_POST['rate_item'][$i]);

                if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                    $this->putLog("[Error][GOLDEN_TIME_ID={$golden_time_id}] Fail exec proc insert golden time item. Error code: ".$db->db->getLastError());

                    $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_GOLDEN_TIME_ITEM + 100);

                    return;
                }

                if ($result == null || ($row = $result->fetch_assoc()) == null) {

                    $this->putLog("[Error][GOLDEN_TIME_ID={$golden_time_id}] Fail in proc insert golden time item, result empty");

                    $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_INSERT_GOLDEN_TIME_ITEM + 100);

                    return;
                }

                if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                    $this->putLog("[Error][GOLDEN_TIME_ID={$golden_time_id}] Fail in verify of proc insert golden time item, result invalid");

                    $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_GOLDEN_TIME_ITEM + 100);

                    return;
                }
            }

            // OK
            $this->setError('Golden Time Event adicionado com sucesso', null);
        }

        protected function editRound() {

            if (!isset($_POST['id']) || !isset($_POST['time']) || $_POST['id'] == '' || $_POST['time'] == '' || is_nan($_POST['id'])) {

                // Log
                $this->putLog('[Edit Round] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeRound ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeRound(?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeRound(?, ?)';

            $params->clear();
            $params->add('i', $_POST['id']);
            $params->add('s', $_POST['time']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc update golden time round. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_GOLDEN_TIME_ROUND);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc update golden time round, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_GOLDEN_TIME_ROUND);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc update golden time round, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_GOLDEN_TIME_ROUND);

                return;
            }

            // OK
            $this->setError('Golden Time Round editado com sucesso', null);
        }

        protected function addRound() {

            if (!isset($_POST['event_id']) || !isset($_POST['time']) || $_POST['event_id'] == '' || $_POST['time'] == '' || is_nan($_POST['event_id'])) {

                // Log
                $this->putLog('[Add Round] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeRound ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeRound(?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeRound(?, ?)';

            $params->clear();
            $params->add('i', $_POST['event_id']);
            $params->add('s', $_POST['time']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc insert golden time round. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_GOLDEN_TIME_ROUND);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc insert golden time round, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_INSERT_GOLDEN_TIME_ROUND);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc insert golden time round, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_GOLDEN_TIME_ROUND);

                return;
            }

            // OK
            $this->setError('Golden Time Round adicionado com sucesso', null);
        }

        protected function editItem() {

            if (!isset($_POST['id']) || !isset($_POST['typeid']) || !isset($_POST['qntd'])
                || !isset($_POST['qntd_time']) || !isset($_POST['rate']) || $_POST['id'] == ''
                || $_POST['typeid'] == '' || $_POST['qntd'] == '' || $_POST['qntd_time'] == '' || $_POST['rate'] == ''
                || is_nan($_POST['id']) || is_nan($_POST['typeid']) || is_nan($_POST['qntd'])
                || is_nan($_POST['qntd_time']) || is_nan($_POST['rate'])) {

                // Log
                $this->putLog('[Edit Item] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeItem ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeItem(?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcUpdateGoldenTimeItem(?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('i', $_POST['id']);
            $params->add('i', $_POST['typeid']);
            $params->add('i', $_POST['qntd']);
            $params->add('i', $_POST['qntd_time']);
            $params->add('i', $_POST['rate']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc update golden time item. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_GOLDEN_TIME_ITEM);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc update golden time item, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_GOLDEN_TIME_ITEM);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc update golden time item, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_GOLDEN_TIME_ITEM);

                return;
            }

            // OK
            $this->setError('Golden Time Item editado com sucesso', null);
        }

        protected function addItem() {

            if (!isset($_POST['event_id']) || !isset($_POST['typeid']) || !isset($_POST['qntd'])
                || !isset($_POST['qntd_time']) || !isset($_POST['rate']) || $_POST['event_id'] == ''
                || $_POST['typeid'] == '' || $_POST['qntd'] == '' || $_POST['qntd_time'] == '' || $_POST['rate'] == ''
                || is_nan($_POST['event_id']) || is_nan($_POST['typeid']) || is_nan($_POST['qntd'])
                || is_nan($_POST['qntd_time']) || is_nan($_POST['rate'])) {

                // Log
                $this->putLog('[Add Item] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeItem ?, ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeItem(?, ?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcInsertGoldenTimeItem(?, ?, ?, ?, ?)';

            $params->clear();
            $params->add('i', $_POST['event_id']);
            $params->add('i', $_POST['typeid']);
            $params->add('i', $_POST['qntd']);
            $params->add('i', $_POST['qntd_time']);
            $params->add('i', $_POST['rate']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc insert golden time item. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_GOLDEN_TIME_ITEM);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc insert golden time item, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_INSERT_GOLDEN_TIME_ITEM);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc insert golden time item, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_GOLDEN_TIME_ITEM);

                return;
            }

            // OK
            $this->setError('Golden Time Item adicionado com sucesso', null);
        }

        protected function removeEvent() {

            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id'])) {

                // Log
                $this->putLog('[Remove Event] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeEvent ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeEvent(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeEvent(?)';

            $params->clear();
            $params->add('i', $_POST['id']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError()) {

                $this->putLog("[Error] Fail exec proc delete golden time event. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_GOLDEN_TIME_EVENT);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc delete golden time event, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_DELETE_GOLDEN_TIME_EVENT);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify proc delete golden time event, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_GOLDEN_TIME_EVENT);

                return;
            }

            // OK
            $this->setError('Golden Time Event deletado com sucesso', null);
        }

        protected function removeRound() {

            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id'])) {

                // Log
                $this->putLog('[Remove Round] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeRound ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeRound(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeRound(?)';

            $params->clear();
            $params->add('i', $_POST['id']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc delete golden time round. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_GOLDEN_TIME_ROUND);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc delete golden time round, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_DELETE_GOLDEN_TIME_ROUND);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc delete golden time round, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_GOLDEN_TIME_ROUND);

                return;
            }

            // OK
            $this->setError('Golden Time Round deletado com sucesso', null);
        }

        protected function removeItem() {

            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id'])) {

                // Log
                $this->putLog('[Reomve Item] invalid parameters');

                $this->setError('Please insert all fields', 0);

                return;
            }

            // DB
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeItem ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_ID_" as "ID" from '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeItem(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteGoldenTimeItem(?)';

            $params->clear();
            $params->add('i', $_POST['id']);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                $this->putLog("[Error] Fail exec proc delete golden time item. Error code: ".$db->db->getLastError());

                $this->setError('System error', SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_GOLDEN_TIME_ITEM);

                return;
            }

            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                $this->putLog("[Error] Fail in proc delete golden time item, result empty");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_PROC_DELETE_GOLDEN_TIME_ITEM);

                return;
            }

            if (!isset($row['ID']) || is_nan($row['ID']) || $row['ID'] == -1) {

                $this->putLog("[Error] Fail in verify of proc delete golden time item, result invalid");

                $this->setError('System error', SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_GOLDEN_TIME_ITEM);

                return;
            }

            // OK
            $this->setError('Golden Time Item deletado com sucesso', null);
        }

        public function show() {

            // Document
            $document = new DynamicTag('html');
            $document->lang = "pt_br";

            // Head
            $head = new DynamicTag('head');
            $title = new DynamicTag('title');

            $title->addChild("Golden Time Tool");

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
                    grid-template-columns: 20px 100px 100px 100px 60px 80px 200px;
                    
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

                .edit-event div > input {
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
            ");

            $head->addChild($style);

            $document->addChild($head);

            // Body
            $body = new DynamicTag('body');
            $body->style = "width: 100%; height: 100%; background-color: #777";

            $body->addChild('<div class="content">
            <div class="intro">
                <font size="20em">Golden Time Tool</font>
            </div>
            <div class="table">
                <div class="div-message">'.$this->showError().'</div>
                <div class="title-table" style="margin-left: 2.2em; justify-content: flex-start;">
                    <text>ID</text>
                    <text>TYPE</text>
                    <text>BEGIN</text>
                    <text>END</text>
                    <text>RATE</text>
                    <text>IS END</text>
                    <text>REGISTRY DATE</text>
                </div>'.(function(){

                    $ret = '';

                    foreach ($this->events as $ev) {

                        $ret .= '<div class="event">
                            <i class="anchor event-title g1 noselect" onClick="toggle(this)">▼</i>
                            <div class="title-table event-title g2">
                                <text>'.$ev->id.'</text>
                                <text>'.(function() use($ev) {
                                    
                                    $ret_1 = 'ONE DAY';

                                    switch ($ev->type) {
                                        case 0:
                                        default:
                                            $ret_1 = 'ONE DAY';
                                        break;
                                        case 1:
                                            $ret_1 = 'INTERVAL';
                                        break;
                                        case 2:
                                            $ret_1 = 'FOREVER';
                                        break;
                                    }

                                    return $ret_1;
                                })().'</text>
                                <text>'.$ev->begin->format('Y-m-d').'</text>
                                <text>'.($ev->end == null ? 'NULL' : $ev->end->format('Y-m-d')).'</text>
                                <text>'.$ev->rate.'</text>
                                <text>'.($ev->is_end ? 'SIM' : 'NÃO').'</text>
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
                                        <text>TYPE</text>'.(function() use($ev) {
                                            
                                            return '<select name="type">
                                                        <option value="0" '.($ev->type == 0 ? 'selected' : '').'>ONE DAY</option>
                                                        <option value="1" '.($ev->type == 1 ? 'selected' : '').'>INTERVAL</option>
                                                        <option value="2" '.($ev->type == 2 ? 'selected' : '').'>FOREVER</option>
                                                    </select>';
                                        })().'
                                    </div>
                                    <div>
                                        <text>BEGIN</text><input type="text" name="begin" value="'.$ev->begin->format('Y-m-d').'">
                                    </div>
                                    <div>
                                        <text>END</text><input type="text" name="end" value="'.($ev->end == null ? 'NULL' : $ev->end->format('Y-m-d')).'">
                                    </div>
                                    <div>
                                        <text>RATE</text><input type="text" name="rate" value="'.$ev->rate.'">
                                    </div>
                                    <div>
                                        <text>IS END</text>
                                        <select name="is_end">
                                            <option value="0" '.(!$ev->is_end ? 'selected' : '').'>NÃO</option>
                                            <option value="1" '.($ev->is_end ? 'selected' : '').'>SIM</option>
                                        </select>
                                    </div>
                                    <div>
                                        <input type="submit" value="Editar"></input>
                                    </div>
                                </form>
                            </div>
                            <div class="event-content g3" style="display: none">'.
                            ((function($ev) {

                                $ret_1 = '';

                                if (count($ev->rounds) <= 0)
                                    return $ret_1;

                                $ret_1 .= '<div class="round">
                                            <div class="title-content">ROUND</div>
                                            <div style="display: flex; align-items: center; flex-direction: column;">
                                                <div>
                                                    <div class="title-round">
                                                        <text>TIME</text>
                                                        <text>REGISTRY DATE</text>
                                                    </div>';

                                foreach ($ev->rounds as $round) {

                                    $ret_1 .= '<div class="title-round">
                                                    <text>'.$round->time.'</text>
                                                    <text>'.$round->reg_date->format('Y-m-d H:i:s.u').'</text>
                                                    <text class="noselect edit-item-btn" onClick="editItem(this)">Edit <i>▼</i></text>
                                                    <text class="noselect remove-item-btn">
                                                        <form onSubmit="return confirm(\'do you want remove this round?\')" action="?" method="POST">
                                                            <input type="hidden" name="remove-round" value="remove">
                                                            <input type="hidden" name="id" value="'.$round->id.'">
                                                            <input type="submit" value="Remove">
                                                        </form>
                                                    </text>
                                                </div>
                                                <div class="edit-item edit-event" style="display: none">
                                                    <form action="?" method="POST">
                                                        <input type="hidden" name="edit-round" value="edit">
                                                        <input type="hidden" name="id" value="'.$round->id.'">
                                                        <div>
                                                            <text>TIME</text><input type="text" name="time" value="'.$round->time.'">
                                                        </div>
                                                        <div>
                                                            <input type="submit" value="Editar">
                                                        </div>
                                                    </form>
                                                </div>';
                                }

                                $ret_1 .= '</div></div></div>';

                                return $ret_1;
                            })($ev)).
                            ((function($ev) {

                                $ret_1 = '';

                                if (count($ev->itens) <= 0)
                                    return $ret_1;

                                $ret_1 .= '<div class="item">
                                            <div class="title-content">ITEM</div>
                                            <div style="display: flex; align-items: center; flex-direction: column;">
                                                <div>
                                                    <div class="title-item">
                                                        <text>TYPEID</text>
                                                        <text>QNTD</text>
                                                        <text>QNTD TIME</text>
                                                        <text>RATE</text>
                                                        <text>REGISTRY DATE</text>
                                                    </div>';

                                foreach ($ev->itens as $item) {

                                    $ret_1 .= '<div class="title-item">
                                                    <text>'.$item->typeid.'</text>
                                                    <text>'.$item->qntd.'</text>
                                                    <text>'.$item->qntd_time.'</text>
                                                    <text>'.$item->rate.'</text>
                                                    <text>'.$item->reg_date->format('Y-m-d H:i:s.u').'</text>
                                                    <text class="noselect edit-item-btn" onClick="editItem(this)">Edit <i>▼</i></text>
                                                    <text class="noselect remove-item-btn">
                                                        <form onSubmit="return confirm(\'do you want remove this item?\')" action="?" method="POST">
                                                            <input type="hidden" name="remove-item" value="remove">
                                                            <input type="hidden" name="id" value="'.$item->id.'">
                                                            <input type="submit" value="Remove">
                                                        </form>
                                                    </text>
                                                </div>
                                                <div class="edit-item edit-event" style="display: none">
                                                    <form action="?" method="POST">
                                                        <input type="hidden" name="edit-item" value="edit">
                                                        <input type="hidden" name="id" value="'.$item->id.'">
                                                        <div>
                                                            <text>TYPEID</text><input type="text" name="typeid" value="'.$item->typeid.'">
                                                        </div>
                                                        <div>
                                                            <text>QNTD</text><input type="text" name="qntd" value="'.$item->qntd.'">
                                                        </div>
                                                        <div>
                                                            <text>QNTD TIME</text><input type="text" name="qntd_time" value="'.$item->qntd_time.'">
                                                        </div>
                                                        <div>
                                                            <text>RATE</text><input type="texte" name="rate" value="'.$item->rate.'">
                                                        </div>
                                                        <div>
                                                            <input type="submit" value="Editar">
                                                        </div>
                                                    </form>
                                                </div>';
                                }

                                $ret_1 .= '</div></div></div>';

                                return $ret_1;
                            })($ev)).
                            '<div class="btn_add">
                                <button onClick="addItem(this, \'round-add\')">Adicionar Round</button>
                                <button onClick="addItem(this, \'item-add\')">Adicionar Item</button>
                            </div>
                            <div class="add_form round-add edit-event" style="display: none;">
                                <form action="?" method="POST">
                                    <div>
                                        <input type="hidden" name="add-round" value="add">
                                        <input type="hidden" name="event_id" value="'.$ev->id.'"
                                        <text style="font-size: 1.5em; font-weight: bold; text-align: center;">Adicionar round</text>
                                    </div>
                                    <div>
                                        <text>TIME</text><input type="text" name="time" value="">
                                    </div>
                                    <div>
                                        <input type="submit" value="Inserir">
                                    </div>
                                </form>
                            </div>
                            <div class="add_form item-add edit-event" style="display: none;">
                                <form action="?" method="POST">
                                    <div>
                                        <input type="hidden" name="add-item" value="add">
                                        <input type="hidden" name="event_id" value="'.$ev->id.'"
                                        <text style="font-size: 1.5em; font-weight: bold; text-align: center;">Adicionar time</text>
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
                                        <text>RATE</text><input type="texte" name="rate" value="">
                                    </div>
                                    <div>
                                        <input type="submit" value="Inserir">
                                    </div>
                                </form>
                            </div>
                        </div>
                    </div>';
                    }

                    return $ret;
                })().'</div>
                    <div style="display: flex; justify-content: center; margin-top: 1.5em; margin-bottom: 1.5em;">
                        <button onClick="addEvent(this)" class="btn-add-event">Adicionar evento</button>
                    </div>
                    <div style="display: none; justify-content: center; margin-bottom: 1.5em;">
                        <form action="?" method="POST" class="add-event">
                            <input type="hidden" name="add-event" value="add">
                            <div style="font-size: 2em; display: block;">Adicionar evento</div>
                            <div class="title-add">
                                <text>TYPE</text>
                                <text>BEGIN</text>
                                <text>END</text>
                                <text>RATE</text>
                                <text>IS END</text>
                            </div>
                            <div>
                                <select name="type">
                                    <option value="0">ONE DAY</option>
                                    <option value="1">INTERVAL</option>
                                    <option value="2">FOREVER</option>
                                </select>
                                <input type="text" name="begin" value="">
                                <input type="text" name="end" value="">
                                <input type="text" name="rate" value="">
                                <select name="is_end">
                                    <option value="0">NÃO</option>
                                    <option value="1">SIM</option>
                                </select>
                            </div>
                            <div class="sub-title">
                                <div class="sub-title-item">Round</div>
                                <div class="sub-title-item-plus noselect"><i onClick="removeSubItem(this, \'cround\')">-</i><i onClick="addSubItem(this, \'cround\')">+</i></div>
                            </div>
                            <div class="title-add">
                                <text>TIME</text>
                            </div>
                            <div class="cround">
                                <input type="text" name="time[]" value="">
                            </div>
                            <div class="sub-title">
                                <div class="sub-title-item">Item</div>
                                <div class="sub-title-item-plus noselect"><i onClick="removeSubItem(this, \'citem\')">-</i><i onClick="addSubItem(this, \'citem\')">+</i></div>
                            </div>
                            <div class="title-add" style="justify-content: space-evenly;">
                                <text>TYPEID</text>
                                <text>QNTD</text>
                                <text>QNTD TIME</text>
                                <text>RATE</text>
                            </div>
                            <div class="citem">
                                <input type="text" name="typeid[]" value="">
                                <input type="text" name="qntd[]" value="">
                                <input type="text" name="qntd_time[]" value="">
                                <input type="text" name="rate_item[]" value="">
                            </div>
                            <div>
                                <input type="submit" value="Adicionar">
                            </div>
                        </form>
                    </div>
                </div>');

            $document->addChild($body);

            echo '<!DOCTYPE html>'.$document->toString();
        }

        private function loadEvents($page) {

            $num_events = self::$LIMIT_EVENTS_PER_PAGE * $page;
            
            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $query = '';

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT [index], [type], [begin], [end], [rate], [is_end], [reg_date] FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_info ORDER BY [index] DESC OFFSET ? ROWS FETCH NEXT '.self::$LIMIT_EVENTS_PER_PAGE.' ROWS ONLY';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT index, type, begin, "end", rate, is_end, reg_date FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_info ORDER BY index DESC OFFSET ? ROWS FETCH NEXT '.self::$LIMIT_EVENTS_PER_PAGE.' ROWS ONLY';
            else
                $query = 'SELECT `index`, `type`, `begin`, `end`, rate, is_end, reg_date FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_info ORDER BY `index` DESC LIMIT ?, '.self::$LIMIT_EVENTS_PER_PAGE; // MYSQL

            $params->clear();
            $params->add('i', $num_events);

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0) {

                while ($row = $result->fetch_assoc()) {
                    
                    if (isset($row['index']) && isset($row['type']) && isset($row['begin']) 
                        && key_exists('end', $row) && isset($row['rate']) && isset($row['is_end']) && isset($row['reg_date'])) {
                        
                        $new_obj = new stdClass();

                        $new_obj->id = $row['index'];
                        $new_obj->type = $row['type'];
                        $new_obj->begin = new DateTime($row['begin']);
                        $new_obj->end = $row['end'] == '' ? null : new DateTime($row['end']);
                        $new_obj->rate = $row['rate'];
                        $new_obj->is_end = $row['is_end'] == 1 ? true : false;
                        $new_obj->reg_date = new DateTime($row['reg_date']);

                        array_push($this->events, $new_obj);
                    }
                }
            } // Error
            else
                $this->putLog("[Error] Não conseguiu pegar os eventos do Golden Time. Error code(".$db->db->getLastError().")");

            if (!empty($this->events)) {

                foreach ($this->events as &$ev) {

                    // Itens
                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'SELECT [index], [typeid], [qntd], [qntd_time], [rate], [reg_date] FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_item WHERE [golden_time_id] = ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'SELECT index, typeid, qntd, qntd_time, rate, reg_date FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_item WHERE golden_time_id = ?';
                    else
                        $query = 'SELECT `index`, `typeid`, `qntd`, `qntd_time`, `rate`, `reg_date` FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_item WHERE `golden_time_id` = ?';

                    $params->clear();
                    $params->add('i', $ev->id);

                    if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0) {

                        while ($row = $result->fetch_assoc()) {

                            if (isset($row['index']) && isset($row['typeid']) && isset($row['qntd']) 
                                && isset($row['qntd_time']) && isset($row['rate']) && isset($row['reg_date'])) {

                                    $new_obj = new stdClass();

                                    $new_obj->id = $row['index'];
                                    $new_obj->typeid = $row['typeid'];
                                    $new_obj->qntd = $row['qntd'];
                                    $new_obj->qntd_time = $row['qntd_time'];
                                    $new_obj->rate = $row['rate'];
                                    $new_obj->reg_date = new DateTime($row['reg_date']);

                                    $ev->itens[] = $new_obj;
                            }
                        }

                    } //Error
                    else
                        $this->putLog("[Error] Não conseguiu pegar os itens do Golden Time Event[ID=".$ev->id."]. Error code(".$db->db->getLastError().")");

                    // Rounds
                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'SELECT [index], [time], [reg_date] FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_round WHERE [golden_time_id] = ?';
                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                        $query = 'SELECT index, time, reg_date FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_round WHERE golden_time_id = ?';
                    else
                        $query = 'SELECT `index`, `time`, `reg_date` FROM '.$db->con_dados['DB_NAME'].'.pangya_golden_time_round WHERE `golden_time_id` = ?';

                    $params->clear();
                    $params->add('i', $ev->id);

                    if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0) {

                        while ($row = $result->fetch_assoc()) {

                            if (isset($row['index']) && isset($row['time']) && isset($row['reg_date'])) {

                                $new_obj = new stdClass();

                                $new_obj->id = $row['index'];
                                $new_obj->time = $row['time'];
                                $new_obj->reg_date = new DateTime($row['reg_date']);

                                $ev->rounds[] = $new_obj;
                            }
                        }

                    } // Error
                    else
                        $this->putLog("[Error] Não conseguiu pegar os rounds do Golden Time Event[ID=".$ev->id."]. Erro code(".$db->db->getLastError().")");

                }
            }
        }
    }

    // Golden Time
    $golden_time = new GoldenTime();
    $golden_time->show();
?>