<?php
    // Arquivo resposta_pag.php
    // Criado em 01/12/2020 as 13:12 por Acrisio
    // Definição e Implementação do sistema que recebe as notificações do PagSeguro

    include_once('source/notification_pagseguro.inc');

    $pag = new NotificationPagSeguro();

    if ($pag->isValid() == FALSE)
        $pag->exitAndPutLog("[Error] Fail to initialize Notification PagSeguro.", PAGSEGURO);

    $r = $pag->consultingCode();

    if ($r == null)
        $pag->exitAndPutLog("[Error] Fail to translate consulting", PAGSEGURO);
?>