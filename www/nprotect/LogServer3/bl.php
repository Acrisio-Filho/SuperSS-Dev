<?php
    include_once('config/save_log.inc');
    include_once('config/encrypt_source.inc');

    // Log
    include_once('config/log.inc');

    // Verifica se é uma requisição válida
    if (isset($_POST['gid']) && isset($_POST['type']) && isset($_POST['binfo']) && isset($_POST['binfo2']) && isset($_POST['md5'])
        && isset($_POST['ost']) && isset($_POST['osv']) && isset($_POST['osc']) && isset($_POST['cpu']) && isset($_POST['mac'])
        && isset($_POST['mvs']) && isset($_POST['hwid'])) {


            // Enviou arquivo de log
            if (isset($_FILES) && isset($_FILES['erlfile']) && isset($_FILES['erlfile']['name']) && isset($_FILES['erlfile']['type'])
                && isset($_FILES['erlfile']['tmp_name']) && isset($_FILES['erlfile']['error']) && isset($_FILES['erlfile']['size'])) {
                
                $file_name = "GameGuard NProtect(BL) Log Report - ";

                foreach ($_FILES as $key => $value)
                    if (isset($value['name']) && isset($value['tmp_name']))
                        save_log($value['name'], $value['tmp_name'], $file_name, $arr);

                file_put_contents("log/BL_Files_log_".time()."-salt".rand().".txt", serialize($_FILES));
            }

            // Log
            sLog::getInstance()->putLog("[BL] _POST[".json_encode($_POST)."], _FILES[".json_encode($_FILES)."]");

            // Não é essa a reposta depois eu vejo, mais para frente
            // Resposta
            echo 'Complete';
    }else
        echo 'Success';  // Pensar que ele enviou tudo certo, enganar quem está tentando hackear
    
    
?>