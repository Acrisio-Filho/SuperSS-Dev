<?php
    // Arquivo login.php
    // Criado em 23/05/2020 as 17:37 por Acrisio
    // Página de Login

    include_once("source/config.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    const LINK_UNKNOWN_ERROR = BASE_GACHA_URL."unknown_error.html";
    
    // tem que ser sem o https, só a pagina gacha_whats.php, por que se colocar o http o link todo ele reseta a session
    const LINK_SUCCESS_LOGIN = /*BASE_GACHA_URL.*/"gacha_whats.php";

    $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
    $params = $db->params;

	$id = (isset($_GET["ID"])) ? $_GET["ID"] : null;
	$key = (isset($_GET["KID"])) ? $_GET["KID"] : null;

	if ($id != null && $key != null && strlen($id) <= 25 && strlen($key) <= 8) {

        // Initialize Session and clean it
        if (!isset($_SESSION))
            session_start();

        if (isset($_SESSION['player']))
            unset($_SESSION['player']);

        // Init With Fail player para previnir de ele passar com algum bug
        $_SESSION['player'] = ['logged' => false];
        
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
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGachaSystemInfo ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_UID_" as "UID", "_ID_" as "ID", "_NICKNAME_" as "NICKNAME", "_LEVEL_" as "LEVEL", "_capability_" as "capability", "_IDState_" as "IDState", "_TICKET_" as "TICKET", "_TICKET_ID_" as "TICKET_ID", "_TICKET_SUB_" as "TICKET_SUB", "_TICKET_SUB_ID_" as "TICKET_SUB_ID" from '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGachaSystemInfo(?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGachaSystemInfo(?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                    && ($row = $result->fetch_assoc()) != null) {

                    if (isset($row['ID']) && isset($row['UID']) && isset($row['NICKNAME']) 
                        && isset($row['TICKET']) && isset($row['LEVEL']) 
                        && isset($row['TICKET_SUB']) && isset($row['IDState'])
                        && isset($row['TICKET_ID']) && isset($row['TICKET_SUB_ID'])) {

                        $_SESSION['player'] =   [ 'logged' => true, 
                                                    'ID' => $row['ID'], 
                                                    'UID' => $row['UID'], 
                                                    'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"), 
                                                    'LEVEL' => $row['LEVEL'],
                                                    'IDState' => $row['IDState'],
                                                    'TICKET' => $row['TICKET'],
                                                    'TICKET_ID' => $row['TICKET_ID'],
                                                    'TICKET_SUB' => $row['TICKET_SUB'],
                                                    'TICKET_SUB_ID' => $row['TICKET_SUB_ID']
                                                ];
                                                
                        // Success Login
                        header("Location: ".LINK_SUCCESS_LOGIN);
                        
                    }else
                        header("Location: ".LINK_UNKNOWN_ERROR);
                
                }else
                    header("Location: ".LINK_UNKNOWN_ERROR);
            
            }else
                header("Location: ".LINK_UNKNOWN_ERROR);
        
        }else
            header("Location: ".LINK_UNKNOWN_ERROR);
    
    }else
        header("Location: ".LINK_UNKNOWN_ERROR);
?>