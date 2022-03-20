<?php
    // Arquivo login.php
    // Criado em 15/07/2019 as 19:22 por Acrisio
    // Definição e implementação do login da guild web (POST)

    include_once("source/config.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    const LINK_UNKNOWN_ERROR = BASE_GUILD_URL."unknown_error.html";
    const LINK_SUCCESS_LOGIN = BASE_GUILD_URL."guild_new/index.php";

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
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildInfo ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_ID_" as "ID", "_UID_" as "UID", "_NICKNAME_" as "NICKNAME", "_LEVEL_" as "LEVEL", "_IDState_" as "IDState", "_GUILD_UID_" as "GUILD_UID", "_MEMBER_STATE_FLAG_" as "MEMBER_STATE_FLAG" from '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildInfo(?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildInfo(?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                    && ($row = $result->fetch_assoc()) != null) {

                    if (isset($row['ID']) && isset($row['UID']) && isset($row['NICKNAME']) 
                        && isset($row['GUILD_UID']) && isset($row['LEVEL']) 
                        && isset($row['MEMBER_STATE_FLAG']) && isset($row['IDState'])) {

                        $_SESSION['player'] =   [ 'logged' => true, 
                                                    'ID' => $row['ID'], 
                                                    'UID' => $row['UID'], 
                                                    'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"), 
                                                    'LEVEL' => $row['LEVEL'],
                                                    'IDState' => $row['IDState'],
                                                    'GUILD_UID' => ($row['GUILD_UID'] == 0) ? -1 : $row['GUILD_UID'],
                                                    'MEMBER_STATE_FLAG' => $row['MEMBER_STATE_FLAG']
                                                ];
                                                
                        // Success Login
                        header("Location: ".LINK_SUCCESS_LOGIN);
                        //header("Location: ".LINK_UNKNOWN_ERROR);
                    
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