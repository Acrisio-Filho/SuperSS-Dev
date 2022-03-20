<?php
    // Arquivo login.php
    // Criado em 28/02/2020 as 06:20 por Acrisio
    // Definição e Implementão do login do Lucia Attendance

    include_once("source/config.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    const LINK_UNKNOWN_ERROR = BASE_LUCIA_ATTENDANCE_URL."unknown_error.html";
    const LINK_SUCCESS_LOGIN = BASE_LUCIA_ATTENDANCE_URL."index.php";

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

                // Pega as informações iniciais da Lucia Attendance do player
                $params->clear();
                $params->add('i', $row['uid']);

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetPlayerLuciaAttendanceInfo ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select "_UID_" as "UID", "_ID_" as "ID", "_NICKNAME_" as "NICKNAME", "_IDState_" as "IDState", "_count_day_" as "count_day", "_last_day_attendance_" as "last_day_attendance", "_last_day_get_item_" as "last_day_get_item", "_try_hacking_count_" as "try_hacking_count", "_block_type_" as "block_type", "_block_end_date_" as "block_end_date" from '.$db->con_dados['DB_NAME'].'.ProcGetPlayerLuciaAttendanceInfo(?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetPlayerLuciaAttendanceInfo(?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                    && ($row = $result->fetch_assoc()) != null) {

                    if (isset($row['ID']) && isset($row['UID']) && isset($row['NICKNAME']) 
                        && isset($row['IDState']) && isset($row['count_day']) && key_exists('last_day_attendance', $row) 
                        && isset($row['try_hacking_count']) && isset($row['block_type']) && key_exists('block_end_date', $row)
                        && key_exists('last_day_get_item', $row)) {

                        $_SESSION['player'] =   [ 'logged' => true, 
                                                    'ID' => $row['ID'], 
                                                    'UID' => $row['UID'], 
                                                    'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"),
                                                    'IDState' => $row['IDState'],
                                                    'COUNT_DAY' => $row['count_day'],
                                                    'LAST_DAY_ATTENDANCE' => $row['last_day_attendance'],
                                                    'LAST_DAY_GET_ITEM' => $row['last_day_get_item'],
                                                    'TRY_HACKING_COUNT' => $row['try_hacking_count'],
                                                    'BLOCK_TYPE' => $row['block_type'],
                                                    'BLOCK_END_DATE' => $row['block_end_date']
                                                ];
                                                
                        // Success Login
                        header("Location: ".LINK_SUCCESS_LOGIN.'?lg=ok');
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