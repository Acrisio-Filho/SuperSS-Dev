<?php
    // Arquivo config.inc
    // Criado em 02/12/2020 as 20:21 por Acrisio
    // Definição das configuração usado no sistema ETP

    // Name site or IP
    define('NAME_SITE', 'superss.ga');

    // Debug Flag
    define('DEBUG_ETP', false);
    
    define('eTYPE_PLATAFORM_LOG', [
        "ENTRYPOINT" => "[Entry Point] ",
        "DONATION_REGISTER" => "[Donation Register] ",
        "EPIN_EXCHANGE" => "[E-Pin Exchange] ",
        "PAGSEGURO" => "[PagSeguro] ",
        "PAYPAL" => "[PayPal] ",
        "UNKNOWN" => "[UNKNOWN] "
    ]);

    define('eTYPE_PLATAFORM', [
        "PAGSEGURO" => 2,
        "PAYPAL" => 1
    ]);

    define('ENTRYPOINT', "ENTRYPOINT");
    define('DONATION_REGISTER', "DONATION_REGISTER");
    define('EPIN_EXCHANGE', "EPIN_EXCHANGE");
    define('PAGSEGURO', "PAGSEGURO");
    define('PAYPAL', "PAYPAL");
    define('UNKNOWN_LOG', "UNKNOWN");

    define('DONATION_TO_CP', 20); // 20 * 1 real

    // Base Addr Entry Point
    define('BASE_ETP_URL', $_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/EntryPoint/');

    // Base Folder Entry Point System
    define('BASE_ETP_DIRECTORY', $_SERVER['DOCUMENT_ROOT'].'/EntryPoint/');
?>