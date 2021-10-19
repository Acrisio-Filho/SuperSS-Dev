<?php
    // Arquivo config.inc
    // Criado em 16/07/2019 as 08:11 por Acrisio
    // Definição da configurção do Sistem de Guild
    
    $NAME_SITE = 'ss3.sytes.net';

    define('BASE_GUILD_URL', $_SERVER['REQUEST_SCHEME'].'://'.$NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/guild/');
    define('BASE_GUILD_UPLOAD_URL', $_SERVER['REQUEST_SCHEME'].'://'.$NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/_Files/guild');
    define('BASE_GUILD_DIRECTORY', $_SERVER['DOCUMENT_ROOT'].'/guild/');

    // Guild condition level label
    $GLOBALS['CONDITION_LEVEL_LABEL'] = [
        0 => 'No level condition',
        6 => 'Beginner or above',
        11 => 'Junior or above',
        16 => 'Senior or above',
        21 => 'Amateur or above'
    ];

    // Guild condition level imagem index
    $GLOBALS['CONDITION_LEVEL_IMG_IDX'] = [
        0 => '01',
        6 => '02',
        11 => '03',
        16 => '04',
        21 => '05'
    ];

    // Constantes de STATE da Guild
    define("ST_APPROVAL_PEDNDING", 0);
    define("ST_NOW_OPEN", 1);
    define("ST_BLOCKED_JOIN", 2);
    define("ST_GM_BLOCKED", 3);
    define("ST_CLOSURE", 4);
    define("ST_GM_CLOSURE", 5);

    // Guild state
    $GLOBALS['STATE_LABEL'] = [
        0 => 'Approval pending',
        1 => 'Now Open',
        2 => 'Blocked join',
        3 => 'GM has Blocked',
        4 => 'Closure',
        5 => 'GM has Closure'
    ];

?>
