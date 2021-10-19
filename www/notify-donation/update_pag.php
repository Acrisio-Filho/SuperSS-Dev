<?php
    // Arquivo update_pag.php
    // Criado em 04/12/2020 as 13:19 por Acrisio
    // Definição e Implementação da consulta de atualizações nas transações do PagSeguro

    include_once('source/update_consulting_pagseguro.inc');

    // Consulting update
    $update = new ConsultingUpdatePagSeguro();

    if ($update->isValid() == FALSE)
        $update->exitAndPutLog("[Error] Fail to initialize Consulting Update PagSeguro.", PAGSEGURO);

    $r = $update->consultingUpdate();

    if ($r == null)
        $update->exitAndPutLog("[Error] Fail to translate consulting update", PAGSEGURO);
?>