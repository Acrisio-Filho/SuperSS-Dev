<?php
    // Arquivo config.inc, tem as configurações de autenticação da API do Pagseguro e Paypal

	// Debug Flag
	define('DEBUG_NOTIFY', false);

	#Configura o banco de dados do mysql
	include_once('database_config.inc');

	$GLOBALS['DatabaseConfigInstance'] = new DatabaseConfig();

	// MySQL
	$GLOBALS['DatabaseConfigInstance']->addConfig(DatabaseConfig::_MYSQL_, "127.0.0.1:3308", "pangya", "pangya", "pangya");

	// MSSQL
	$GLOBALS['DatabaseConfigInstance']->addConfig(DatabaseConfig::_MSSQL_, "pangyaPHP", "pangya", "pangyaPHP", "pangya");

	// PostgreSQL
	$GLOBALS['DatabaseConfigInstance']->addConfig(DatabaseConfig::_PSQL_, "pangyaPHP_pg", "pangya", "pangyaPHP", "pangya");

	// Server Database current in use
	$GLOBALS['DatabaseCurrentUsed'] = DatabaseConfig::_MYSQL_;

	// log types
	define('eTYPE_PLATAFORM_LOG', [
        "PAYPAL" => "[PayPal] ",
        "PAGSEGURO" => "[PagSeguro] ",
        "UNKNOWN" => "[Unknown] "
	]);
	
	define('eTYPE_PLATAFORM', [
		"PAGSEGURO" => 2,
		"PAYPAL" => 1
	]);

    define('PAGSEGURO', "PAGSEGURO");
	define('PAYPAL', "PAYPAL");
	
	define('DONATION_TO_CP', 20); // 20 * 1 real

	// Base directory
	define('BASE_NOTIFY_DIRECTORY', $_SERVER['DOCUMENT_ROOT'].'/');

    // PagSeguro dados
    define('PAG_AUTH_EMAIL', "seu email do pagseguro");
	define('PAG_AUTH_TOKEN', 'seu token do pagseguro');
	
	// PayPal dados
	define('PAY_AUTH_EMAIL', "seu email do paypal");
	define('PAY_AUTH_TOKEN', "seu email do paypal")
?>