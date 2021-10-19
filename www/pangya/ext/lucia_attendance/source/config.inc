<?php
    // Arquivo config.inc
    // Criado em 28/02/2020 as 05:32 por Acrisio
    // Definição das configurações do Sistem Lucia Attendance

    include_once('../source/config.inc');

    // Debug Enum e config
    abstract class DEBUG_TYPE {
        const DT_SHOW_ALL_ERROR = 0;
        const DT_ONLY_FILE_ERROR = 1;
        const DT_SHOW_ERROR = 2;
    }

    // Debug ERROR TYPE
    define('DEBUG_ERROR', DEBUG_TYPE::DT_ONLY_FILE_ERROR);

    // Base Addr Lucia Attendance
    define('BASE_LUCIA_ATTENDANCE_URL', $_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/ext/lucia_attendance/');

    // Base Addr IMG
    define('BASE_IMG', $_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/img/');

    // Base Folder Lucia Attendance
    define('BASE_LUCIA_ATTENDANCE_DIRECTORY', $_SERVER['DOCUMENT_ROOT'].'/ext/lucia_attendance/');
?>