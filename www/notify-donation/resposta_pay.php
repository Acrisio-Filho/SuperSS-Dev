<?php
    // Arquivo resposta_pay.php
    // Criado em 06/12/2020 as 16:01 por Acrisio
    // Implementação do sistema de recebimento de notificações do PayPal

    include_once('source/notification_paypal.inc');

    $pay = new NotificationPayPal();

    if ($pay->isValid() == FALSE)
        $pay->exitAndPutLog("[Error] Fail to initialize Notification PayPal.", PAYPAL);

    $r = $pay->consultingCode();

    if ($r == null)
        $pay->exitAndPutLog("[Error] Fail to translate consulting", PAYPAL);
?>