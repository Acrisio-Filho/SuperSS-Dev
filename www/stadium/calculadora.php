<?php
    // Arquivo calculadora.php
    // Criado em 26/11/2020 as 11:24 por Acrisio
    // Definição e Implementação da API da calculadora Stadium

    include_once("config/db_manager_singleton.inc");

    function makeError($err, $code) {
        return json_encode(
        [
            'error' => 
            [
                'message' => $err, 
                'code' => $code
            ]
        ]);
    }

    if (!isset($_POST)) {

        echo makeError('Invalid POST', 1);

        exit();
    }

    if (!isset($_POST['token'])) {

        echo makeError('Invalid Token', 2);

        exit();
    }

    if ($_POST['token'] != $GLOBALS['TOKEN']) {

        echo makeError('Invalid Token: '.$_POST['token'], 2);

        exit();
    }

    if (!isset($_POST['type'])) {

        echo makeError('Invalid Request', 3);

        exit();
    }

    if ($_POST['type'] == 'create') {
        
        if (!isset($_POST['uid']) || !isset($_POST['permission']) || !isset($_POST['name']) || !isset($_POST['raw'])
            || is_nan($_POST['uid']) || is_nan($_POST['permission'])) {

            echo makeError('Invalid Params to create', 4);
    
            exit();
        }

        $raw_decode = base64_decode($_POST['raw']);

        $db = DBManagerSingleton::getInstanceDB();
        $params = DBManagerSingleton::getInstanceParams();

        $query = 'call create_calculator(?, ?, ?, ?)';

        $params->clear();
        $params->add('i', $_POST['uid']);
        $params->add('i', $_POST['permission']);
        $params->add('s', $_POST['name']);
        $params->add('b', $raw_decode);

        if (($result = $db->execPreparedStmt($query, $params->get())) == null && $db->getLastError() != 0) {

            // Log error
            error_log("Fail to execute query for create calculator. Error code: ".$db->getLastError());

            echo makeError('Fail to exec query create', 5);
    
            exit();
        }

        if (($row = $result->fetch_assoc()) == null || !isset($row['ID'])) {

            echo makeError('Fail in create query', 6);
    
            exit();
        }

        if ($row['ID'] == -1) {

            echo makeError('Fail in create query, calculator['.$_POST['name'].'] already exists.', 7);

            exit();
        }

        // Reply Success
        echo json_encode(['error' => null, 'id' => $row['ID']]);
    
    }else if ($_POST['type'] == 'update') {

        if (!isset($_POST['id']) || !isset($_POST['permission']) || !isset($_POST['raw'])
            || is_nan($_POST['id']) || $_POST['id'] == -1 || is_nan($_POST['permission'])) {

            echo makeError('Invalid Params to create', 4);

            exit();
        }

        $raw_decode = base64_decode($_POST['raw']);

        $db = DBManagerSingleton::getInstanceDB();
        $params = DBManagerSingleton::getInstanceParams();

        $query = 'call update_calculator(?, ?, ?)';

        $params->clear();
        $params->add('i', $_POST['id']);
        $params->add('i', $_POST['permission']);
        $params->add('b', $raw_decode);

        if (($result = $db->execPreparedStmt($query, $params->get())) == null && $db->getLastError() != 0) {

            // Log error
            error_log("Fail to execute query for update calculator. Error code: ".$db->getLastError());

            echo makeError('Fail to exec query update', 5);
    
            exit();
        }

        if (($row = $result->fetch_assoc()) == null || !isset($row['ID'])) {

            echo makeError('Fail in update query', 6);
    
            exit();
        }

        if ($row['ID'] == -1) {

            echo makeError('Fail in update query, calculator['.$_POST['name'].'] not exists.', 7);

            exit();
        }

        // Reply Success
        echo json_encode(['error' => null, 'id' => $row['ID']]);

    }else if ($_POST['type'] == 'delete') {

        if (!isset($_POST['name']) || !isset($_POST['id']) || is_nan($_POST['id']) || $_POST['id'] == -1 || $_POST['name'] == '') {

            echo makeError('Invalid Params to create', 4);

            exit();
        }

        $db = DBManagerSingleton::getInstanceDB();
        $params = DBManagerSingleton::getInstanceParams();

        // Public calculators and private from owner
        $query = 'call delete_calculator(?, ?)';

        $params->clear();
        $params->add('s', $_POST['name']);
        $params->add('i', $_POST['id']);

        if (($result = $db->execPreparedStmt($query, $params->get())) == null && $db->getLastError() != 0) {

           // Log error
           error_log("Fail to execute query for delete calculators. Error code: ".$db->getLastError());

           echo makeError('Fail to exec query delete', 5);
  
          exit();
        }

        if (($row = $result->fetch_assoc()) == null || !isset($row['ID']) || $row['ID'] == -1) {

            echo makeError('Fail in delete query', 6);
    
            exit();
        }

        // Reply
        echo json_encode([
            'error' => null,
            'id' => $row['ID']
        ]);

    }else if ($_POST['type'] == 'load') {

        if (!isset($_POST['name']))  {

            echo makeError('Invalid Params to load', 4);
    
            exit();
        }

        $db = DBManagerSingleton::getInstanceDB();
        $params = DBManagerSingleton::getInstanceParams();

        $query = 'SELECT `index`, `uid`, `owner_uid`, `permission`, `name`, `raw` FROM calculators WHERE `name` = ? LIMIT 1';

        $params->clear();
        $params->add('s', $_POST['name']);

        if (($result = $db->execPreparedStmt($query, $params->get())) == null && $db->getLastError() != 0)  {

             // Log error
             error_log("Fail to execute query for load calculator. Error code: ".$db->getLastError());

             echo makeError('Fail to exec query load', 5);
    
            exit();
        }

        if ($result == null) {

            echo makeError('Not found calculator', 8);

            exit();
        }

        if (($row = $result->fetch_assoc()) == null || !isset($row['index']) || !isset($row['uid'])
            || !isset($row['permission']) || !isset($row['name']) || !isset($row['raw'])) {

            echo makeError('Fail in return values from database', 9);

            exit();
        }

        // Reply Sucess
        echo json_encode([
            'error' => null,
            'id' => $row['index'],
            'uid' => $row['uid'],
            'owner_uid' => $row['owner_uid'],
            'permission' => $row['permission'],
            'name' => $row['name'],
            'raw' => base64_encode($row['raw'])
        ]);

    }else if ($_POST['type'] == 'list') {

        if (!isset($_POST['uid']) || is_nan($_POST['uid']) || !isset($_POST['option']) || $_POST['option'] == '') {

            echo makeError('Invalid Params to list', 4);
    
            exit();
        }

        $db = DBManagerSingleton::getInstanceDB();
        $params = DBManagerSingleton::getInstanceParams();

        // Public calculators and private from owner
        if ($_POST['option'] == 'view')
            $query = 'SELECT `name` FROM calculators WHERE `uid` = ? OR `permission` < 2'; // 0 e 1 pode ver
        else if ($_POST['option'] == 'copy')
            $query = 'SELECT `name` FROM calculators WHERE `uid` = ? OR `permission` = 0'; // Pode copiar e ver
        else
            $query = 'SELECT `name` FROM calculators WHERE `uid` = ?'; // Só as calculadoras dele

        $params->clear();
        $params->add('i', $_POST['uid']);

        if (($result = $db->execPreparedStmt($query, $params->get())) == null && $db->getLastError() != 0)  {

             // Log error
             error_log("Fail to execute query for list calculators. Error code: ".$db->getLastError());

             echo makeError('Fail to exec query list', 5);
    
            exit();
        }

        if ($result == null) {

            echo json_encode([
                'error' => null,
                'list' => []
            ]);

            exit();
        }

        $list = [];

        while (($row = $result->fetch_assoc())) {

            if (!isset($row['name'])) {

                echo makeError('Fail in return values from database', 9);

                exit();
            }

            $list[] = [
                'name' => $row['name']
            ];
        }

        // Reply Sucess
        echo json_encode([
            'error' => null,
            'list' => $list
        ]);

    }else if ($_POST['type'] == 'copy') {

        if (!isset($_POST['name']) || !isset($_POST['copy']) || !isset($_POST['uid']) || !isset($_POST['permission']) 
            || is_nan($_POST['uid']) || is_nan($_POST['permission'])) {

            echo makeError('Invalid Params to list', 4);
    
            exit();
        }

        $db = DBManagerSingleton::getInstanceDB();
        $params = DBManagerSingleton::getInstanceParams();

        // Public calculators and private from owner
        $query = 'call copy_calculator(?, ?, ?, ?)';

        $params->clear();
        $params->add('i', $_POST['uid']);
        $params->add('i', $_POST['permission']);
        $params->add('s', $_POST['name']);
        $params->add('s', $_POST['copy']);

        if (($result = $db->execPreparedStmt($query, $params->get())) == null && $db->getLastError() != 0) {

           // Log error
           error_log("Fail to execute query for copy calculators. Error code: ".$db->getLastError());

           echo makeError('Fail to exec query copy', 5);
  
          exit();
        }

        if (($row = $result->fetch_assoc()) == null || !isset($row['ID']) || $row['ID'] == -1) {

            echo makeError('Fail in copy query', 6);
    
            exit();
        }

        // Reply
        echo json_encode([
            'error' => null,
            'id' => $row['ID']
        ]);

    }else {

        // Tipo desconhecido
        echo makeError('Unknown type', 1000);
    }

?>