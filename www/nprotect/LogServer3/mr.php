<?php
    include_once('config/save_log.inc');
    include_once('config/encrypt_source.inc');

    // Log
    include_once('config/log.inc');

    file_put_contents("log/MR_Get_log_".time()."-salt".rand().".txt", serialize($_GET));

    file_put_contents("log/MR_Files_log_".time()."-salt".rand().".txt", serialize($_FILES));

    file_put_contents("log/MR_POST_log_".time()."-salt".rand().".txt", serialize($_POST));

    // Log
    sLog::getInstance()->putLog("[MR] _POST[".json_encode($_POST)."], _FILES[".json_encode($_FILES)."]");

    // Respota
    echo 'Complete';
?>