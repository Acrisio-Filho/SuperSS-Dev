<?php
    // Arquivo change_password.php
    // Criado em 30/07/2019 as 23:24 por Acrisio
    // página de troca de senha

    include_once("config/base_classe.inc");

	$arr = ['get' => $_GET, 'post' => $_POST];

	$site = new base_c($arr);

	$site->change_password_view();

?>