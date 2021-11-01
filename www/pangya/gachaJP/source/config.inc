<?php
    // Arquivo config.inc
    // Criado em 23/05/2020 as 17:39
    // Definição da configuração do Sistema Gacha

    // Name site or IP
    define('NAME_SITE', 'superss.ga');

    // Debug Enum e config
    abstract class DEBUG_TYPE {
        const DT_SHOW_ALL_ERROR = 0;
        const DT_ONLY_FILE_ERROR = 1;
        const DT_SHOW_ERROR = 2;
    }

    // Modos
	abstract class PLAY_MODO {
        const PM_ONE = 1;
        const PM_TEN = 10;
    }

    // Rarity Type Item
    abstract class RARITY_TYPE {
        const RT_RARE = 0;
        const RT_AZTEC = 1;
        const RT_NORMAL = 2;
    }

    // Debug ERROR TYPE
    define('DEBUG_ERROR', DEBUG_TYPE::DT_ONLY_FILE_ERROR);

    // Base Addr Gacha System
    // Depois trocar o nome da pasta
    define('BASE_GACHA_URL', $_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/gachaJP/');

    // Base Folder Gacha System
    define('BASE_GACHA_DIRECTORY', $_SERVER['DOCUMENT_ROOT'].'/gachaJP/');
?>