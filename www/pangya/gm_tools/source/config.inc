<?php
    // Arquivo config.inc
    // Criado em 07/11/2020 as 01:56
    // Definição da configuração do Game Master Ferramentas

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

    define('HOME_URL', $_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/pangya.php');

    // Base Addr GM Tools
    // Depois trocar o nome da pasta
    define('BASE_GM_TOOLS_URL', $_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/gm_tools/');

    // Base Folder GM Tools
    define('BASE_GM_TOOL_DIRECTORY', $_SERVER['DOCUMENT_ROOT'].'/gm_tools/');
?>