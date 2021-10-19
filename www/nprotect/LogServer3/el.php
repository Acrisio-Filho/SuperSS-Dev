<?php
    include_once('config/save_log.inc');
    include_once('config/encrypt_source.inc');
    
    // Log
    include_once('config/log.inc');

    if (isset($_POST['gid']) && isset($_POST['code']) && isset($_POST['ost']) && isset($_POST['osv']) && isset($_POST['osc'])
        && isset($_POST['cpu']) && isset($_POST['mac']) && isset($_POST['hwid']) && (isset($_POST['mtp']) && ($_POST['mtp'] == '' || isset($_POST['mvs'])))) {
        
        if (isset($_FILES) && isset($_FILES['erlfile']) && isset($_FILES['erlfile']['name']) && isset($_FILES['erlfile']['type'])
            && isset($_FILES['erlfile']['tmp_name']) && isset($_FILES['erlfile']['error']) && isset($_FILES['erlfile']['size'])) {
            
            // Salva arquivo
            save_log($_FILES['erlfile']['name'], $_FILES['erlfile']['tmp_name'], "GameGuard NProtect(EL) Log Report - ", null);
        }

        // Log
        sLog::getInstance()->putLog("[EL] _POST[".json_encode($_POST)."], _FILES[".json_encode($_FILES)."]");

        // Resposta
        echo 'Complete';
    }
?>