<?php
	
	include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

	define("RSS_VERSION", 0);

    $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
	$params = $db->params;

	$id = (isset($_GET["ID"])) ? $_GET["ID"] : null;
	$key = (isset($_GET["KID"])) ? $_GET["KID"] : null;

	if ($id != null && $key != null && strlen($id) <= 25 && strlen($key) <= 8) {
		
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

		if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

			if (($row = $result->fetch_assoc()) != null
				&& isset($row['uid']) && $row['uid'] > 0) {

				// Get All List Notice Info
				$params->clear();
				$params->add('i', $row['uid']);

				if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildRSSInfo ?';
				else if(DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
					$query = 'select "_RSS_VER_" as "RSS_VER", "_TITLE_" as "TITLE", "_TEXT_" as "TEXT", "_NICKNAME_" as "NICKNAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildRSSInfo(?)';
				else
					$query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildRSSInfo(?)';

				if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0 
					&& ($row = $result->fetch_assoc()) != null && isset($row['RSS_VER'])) {

					echo '<?xml version="1.0" encoding=UTF-8"?>';
					echo '<rss><ver>'.$row['RSS_VER'].'</ver>';
					
					do {

						if (isset($row['TITLE']) && isset($row['TEXT']) 
							&& isset($row['REG_DATE']) && isset($row['NICKNAME'])) {
							
							echo '<notice>';
							echo '<title>'.mb_convert_encoding(htmlspecialchars($row['TITLE']), "SJIS", "UTF-8").'</title>';
							echo '<url>-</url>';
							echo '<content>'.mb_convert_encoding(htmlspecialchars($row['TEXT']), "SJIS", "UTF-8").'</content>';
							echo '<RegDate>'.$row['REG_DATE'].'</RegDate>';
							echo '<Cate>0</Cate>';
							echo '<Userid>'.mb_convert_encoding(htmlspecialchars(mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS")), "SJIS", "UTF-8").'</Userid>';
							echo '</notice>';
						}

					} while (($row = $result->fetch_assoc()) != null);
					
					echo '</rss>';
				
				}else {

					// Erro ao pegar os notices
					echo '<?xml version="1.0" encoding="Shift_JIS"?>
						  <rss><ver>'.RSS_VERSION.'</ver></rss>';
				}

			}else {
				echo '<html lang="PT-BR"><head><meta charset="UTF-8" />
					<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
					<link rel="icon" href="/favicon.ico" type="image/x-icon">
				<title>Guild RSS - Erro</title></head><body><h2>Erro dados incorretos</h2></body></html>';
			}

		}else {
			echo '<html lang="PT-BR"><head><meta charset="UTF-8" />
				<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
				<link rel="icon" href="/favicon.ico" type="image/x-icon">
			<title>Guild RSS - Erro</title></head><body><h2>Erro dados incorretos</h2></body></html>';
		}

	}else {
		echo '<html lang="PT-BR"><head><meta charset="UTF-8" />
			<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
			<link rel="icon" href="/favicon.ico" type="image/x-icon">
		<title>Guild RSS - Erro</title></head><body><h2>Erro dados incorretos</h2></body></html>';
	}
?>