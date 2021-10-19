<?php
    // Arquivo change_nickname.php
    // Criado em 30/07/2019 as 17:35 por Acrisio
    // página de troca de nickname

    include_once("config/base_classe.inc");

	$arr = ['get' => $_GET, 'post' => $_POST];

	$site = new base_c($arr);

	$site->change_nickname_view();

?>