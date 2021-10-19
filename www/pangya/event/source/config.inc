<?php
    // Arquivo config.inc
    // Criado em 28/06/2020 as 14:29
    // Definição da configuração do Sistema de Evento

    // Name site or IP
    define('NAME_SITE', 'superss.ga');

    // Debug Enum e config
    abstract class DEBUG_TYPE {
        const DT_SHOW_ALL_ERROR = 0;
        const DT_ONLY_FILE_ERROR = 1;
        const DT_SHOW_ERROR = 2;
    }

    // Debug ERROR TYPE
    define('DEBUG_ERROR', DEBUG_TYPE::DT_ONLY_FILE_ERROR);

    // Base Addr Event
    // Depois trocar o nome da pasta
    define('BASE_EVENT_URL', $_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/event/');

    // Base Folder Event System
    define('BASE_EVENT_DIRECTORY', $_SERVER['DOCUMENT_ROOT'].'/event/');
?>