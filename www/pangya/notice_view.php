<?php
    // Arquivo notice_view.php
    // Criado em 22/10/2019 as 00:20 por Acrisio
    // Definição e Implementação da página que visualiza as noticias

    // inicializa as variáveis
    include_once("config/notice_singleton.inc");

    $error = 0;

    if (isset($_GET['id']) && $_GET['id'] != '' && is_numeric($_GET['id'])
        && ($n = NoticeSingleton::getInstance()->findNotice($_GET['id'])) != null) {

        $title = $n->title;
        $date = date("d/m/Y H:i", $n->date);
        $body = $n->body;

    }else {
        $title = "Error";
        $body = "Unknown error";
        $error = 1;
    }
?>

<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
    <link rel="icon" href="/favicon.ico" type="image/x-icon">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title><?php echo $title ?></title>
</head>
<body>
    <h1><?php echo $title; ?></h1>

    <?php

        if ($error == 0) {

            // Data
            echo '<h5>Date: '.$date.'</h5><br>';

            // Body
            echo $body;

        }else   // Error
            echo '<h3>'.$body.'</h3>';
    ?>

</body>
</html>