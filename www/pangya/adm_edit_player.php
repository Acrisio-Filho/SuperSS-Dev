<?php
	include_once("config/base_classe.inc");

	$arr = ['get' => $_GET, 'post' => $_POST];

	$site = new base_c($arr);

	$site->adm_edit_player();

?>