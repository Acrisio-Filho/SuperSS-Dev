<?php
	#Configura o banco de dados do mysql
	include_once('database_config.inc');

	$GLOBALS['DatabaseConfigInstance'] = new DatabaseConfig();

	// MySQL
	$GLOBALS['DatabaseConfigInstance']->addConfig(DatabaseConfig::_MYSQL_, "127.0.0.1", "pangya", "pangya", "pangya");

	// MSSQL
	$GLOBALS['DatabaseConfigInstance']->addConfig(DatabaseConfig::_MSSQL_, "pangyaPHP", "pangya", "pangyaPHP", "pangya");

	// PostgreSQL
	$GLOBALS['DatabaseConfigInstance']->addConfig(DatabaseConfig::_PSQL_, "pangyaPHP", "pangya", "pangyaPHP", "pangya");
	//$GLOBALS['DatabaseConfigInstance']->addConfig(DatabaseConfig::_PSQL_, "pangyaPHP_pgpi", "pangya", "pangyaPHP", "pangya");

	// Server Database current in use
	$GLOBALS['DatabaseCurrentUsed'] = DatabaseConfig::_MYSQL_;

?>