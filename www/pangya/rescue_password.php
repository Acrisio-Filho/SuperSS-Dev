<?php
    // Arquivo rescue_password.php
    // Criado em 31/07/2019 as 14:18 por Acrisio
    // Página que recupera a senha

    include_once("config/base_classe.inc");

	$arr = ['get' => $_GET, 'post' => $_POST];

	$site = new base_c($arr);

	$site->rescue_password_view();

?>