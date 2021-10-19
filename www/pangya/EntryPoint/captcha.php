<?php
    // Arquivo captcha.php
    // Criado em 04/12/2020 as 17:43 por Acrisio
    // Definição do sistema que gera a imagem do Captcha

    include_once('source/captcha.inc');

    // Set Header Image type
    header("Content-type: img/png");

    if (!Captcha::isEnable()) {

        http_response_code(404);

        exit();

        return;
    }

    $img = Captcha::makeImageCaptchaSecret();

    if ($img == null) {

        http_response_code(404);

        exit();

        return;
    }

    // Send Image to client
    echo $img;
?>