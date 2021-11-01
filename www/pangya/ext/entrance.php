<?php

	// Arquivo entrance.php
	// Criado em 28/02/2020 as 05:35 por Acrisio
	// Redireciona temporário para o Sistema Lucia Attendance

	include_once('source/config.inc');

	if (stristr($_SERVER['HTTP_USER_AGENT'], 'MSIE 7.0;') == null 
		|| stristr($_SERVER['HTTP_USER_AGENT'], 'compatible;') == null
		|| !preg_match("/Trident\/[345678]\.[01];/", $_SERVER['HTTP_USER_AGENT'])) {

		error_log("[Entrance][Error] [IP=".(!empty(getenv("REMOTE_ADDR"))  ? getenv("REMOTE_ADDR") : 'unknwon')."] Bad navigator request. (".$_SERVER['HTTP_USER_AGENT'].")", 0);

		// Redireciona para a página de erro
		header("Location: ".$_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/pangya.php');

		// sai do script para o navegador redirecionar para a página
		exit();
	}

	// Redireciona para o Sistema Lucia Attendance
	header("Location: ".$_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/ext/lucia_attendance/login.php'.(!empty($_SERVER['QUERY_STRING']) ? '?'.$_SERVER['QUERY_STRING'] : ''));

?>
<?php
	
	/*echo '<html lang="PT-BR"><head><meta charset="UTF-8" />
		<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
		<link rel="icon" href="/favicon.ico" type="image/x-icon">
	<title>Entrance - Em construção</title></head><body><h2>Em construção</h2></body></html>';
	/*include "../config/config.inc";

	if (_MSSQL_)
		include "../config/mssql_odbc_manager.inc";
	else
		include "../config/MysqlManager.inc";

	$db = null;

	if (_MSSQL_)
		$db = new MSSqlODBCManager($con_dados);
	else
		$db = new MysqlManager($con_dados);

	$params = new paramsArr();

	$id = (isset($_GET["ID"])) ? $_GET["ID"] : null;
	$key = (isset($_GET["KID"])) ? $_GET["KID"] : null;

	if ($id != null && $key != null) {
		$params->clear();
		$params->add('s', $id);
		$params->add('s', $key);

		$query = '';

		if (_MSSQL_)
			$query = 'DECLARE @returnvalue INT = -1 exec '.$db->con_dados['DB_NAME'].'.CheckWeblink_Key$IMPL ?, ?, @returnvalue = @returnvalue OUTPUT SELECT @returnvalue as [uid]';
		else
			$query = 'SELECT CheckWeblink_Key(?, ?) as UID';

		if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0) {
			$row = $result->fetch_assoc();

			if (isset($row['uid']) && $row['uid'] >= 0) {

				echo '<html lang="PT-BR">
		<head><meta charset="UTF-8" />
		<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
		<link rel="icon" href="/favicon.ico" type="image/x-icon">
	<title>Entrance</title></head><body><h2>Welcome Entrance</h2></body></html>';
			}else {
				echo '<html lang="PT-BR"><head><meta charset="UTF-8" />
		<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
		<link rel="icon" href="/favicon.ico" type="image/x-icon">
	<title>Entrance - Erro</title></head><body><h2>Erro dados incorretos</h2></body></html>';
			}
		}else {
			echo '<html lang="PT-BR"><head><meta charset="UTF-8" />
		<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
		<link rel="icon" href="/favicon.ico" type="image/x-icon">
	<title>Entrance - Erro</title></head><body><h2>Erro dados incorretos</h2></body></html>';
		}
	}else {
		echo '<html lang="PT-BR"><head><meta charset="UTF-8" />
		<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
		<link rel="icon" href="/favicon.ico" type="image/x-icon">
	<title>Entrance - Erro</title></head><body><h2>Erro dados incorretos</h2></body></html>';
	}*/
?>