<?php
    include_once('config/save_log.inc');
    include_once('config/encrypt_source.inc');

    // Log
    include_once('config/log.inc');

    file_put_contents("log/BN_Get_log_".time()."-salt".rand().".txt", serialize($_GET));

    file_put_contents("log/BN_Files_log_".time()."-salt".rand().".txt", serialize($_FILES));

    file_put_contents("log/BN_POST_log_".time()."-salt".rand().".txt", serialize($_POST));

    // Log
    sLog::getInstance()->putLog("[BN] _POST[".json_encode($_POST)."], _FILES[".json_encode($_FILES)."]");

    // Resposta
    echo 'Complete';
?>