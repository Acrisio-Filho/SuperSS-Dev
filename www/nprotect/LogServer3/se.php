<?php
    include_once('config/save_log.inc');
    include_once('config/encrypt_source.inc');

    // Log
    include_once('config/log.inc');

    $post_code = '';

    if (isset($_POST['code']))
        $post_code = $_POST['code'];
    else if (isset($_POST['tp']))
        $post_code = $_POST['tp'];
    else if (isset($_POST['btp']))
        $post_code = $_POST['btp'];

    // Resposta
    if ($post_code != '')
        $code = decryptStringResource($post_code);

    if (isset($code) && $code != '' && isset($_POST['dummy']) && is_numeric($_POST['dummy'])) {

        define("CONST_VALUE", 0x73171913);

        $first = explode('|', $code);

        $hash = CRCHashStr($first[0]);

        $hash_do_dummy = (($_POST['dummy'] ^ $hash) ^ CONST_VALUE);

        // Log
        sLog::getInstance()->putLog("[SE] Hash Dummy: ".$hash_do_dummy);

        echo $hash_do_dummy;
    }
?>