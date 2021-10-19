<?php
	include_once("../config/base.inc");

	include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

	$db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
	$params = $db->params;

	$uccuid = (isset($_POST['selfDesignUID'])) ? $_POST['selfDesignUID'] : 0;
	$item_id = (isset($_POST['selfDesignItemID'])) ? $_POST['selfDesignItemID'] : 0;
	$key = (isset($_POST['selfDesignKey'])) ? $_POST['selfDesignKey'] : "";

	$query = '';

	if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
		$query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcCheckSecurityKey ?, ?, ?';
	else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
		$query = 'SELECT "_UID_" as "UID" FROM '.$db->con_dados['DB_NAME'].'.ProcCheckSecurityKey(?, ?, ?)';
	else
		$query = 'CALL ProcCheckSecurityKey(?, ?, ?)';

	$params->clear();
	$params->add('i', $uccuid);
	$params->add('i', $item_id);
	$params->add('s', $key);

	if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

		$row = $result->fetch_assoc();
		
		if($row != null && isset($row['UID']) && $uccuid != 0 && $uccuid == $row['UID']) {
			$UCCIDX = between_last("_", ".", $_FILES['selfDesignFileName']['name']);

			$params->clear();
			$params->add('i', $uccuid);
			$params->add('s', $UCCIDX);

			$query = '';

			if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
				$query = 'SELECT [UID] FROM '.$db->con_dados['DB_NAME'].'.TU_UCC WHERE [UID] = ? AND UCCIDX = ?';
			else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
				$query = 'SELECT UID as "UID" FROM '.$db->con_dados['DB_NAME'].'.TU_UCC WHERE UID = ? AND upper(UCCIDX) = upper(?)';
			else
				$query = 'SELECT UID FROM TU_UCC WHERE UID = ? AND UCCIDX = ?';

			if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

				$row = $result->fetch_assoc();

				if($row != null && isset($row['UID']) && $uccuid != 0 && $uccuid == $row['UID']) {
					$atual_dir = getcwd();

					$dir = $_SERVER['DOCUMENT_ROOT'].'/_Files/SelfDesign/';

					if(isset($_POST['selfDesignFolder'])){
						$dir .= $_POST['selfDesignFolder'].'/';

						if(!is_dir($dir))
							mkdir($dir);

						$dir .= checkFileName($_FILES['selfDesignFileName']['name']);

						move_uploaded_file($_FILES['selfDesignFileName']['tmp_name'], $dir);

						/*$fop = fopen($atual_dir."\\teste.txt", "w");

						fwrite($fop, $dir);

						fclose($fop);*/

						//file_put_contents($atual_dir."\\Files.txt", serialize($_FILES));

						//file_put_contents($atual_dir."\\SD_log.txt", serialize($_POST));

						#PANGYA_UPLOAD_ERR UNKNOWN
						#PANGYA_SRVVAR     UNKNOWN
						#PANGYA_UPLOAD_OK  0

						echo "0";
					}else{
						echo "3";
					}
				}else{
					echo "12"; # UCCIDX Errado
				}
			}else {
				echo "13"; # Erro no execPreparedStmt
			}
		}else{
			echo "1";
		}
	}else {
		echo "13"; # Erro no execPreparedStmt
	}
?>