<?php
    include_once('config/save_log.inc');
    include_once('config/encrypt_source.inc');

    // Log
    include_once('config/log.inc');

    if (isset($_GET['gd']) && !empty($_GET['gd']) && isset($_GET['mtp']) && !empty($_GET['mtp']) && isset($_GET['dm']) && is_numeric($_GET['dm'])) {

        $code = decryptStringResource($_GET['gd']);

        if (isset($code) && $code != '') {

            define("CONST_VALUE", 0xA7F24B);
            define("INT32_LIMIT", 0xFFFFFFFF);

            if (ord($code) != 0)
                $crcGameName = CRCHashStr($code);
            else
                $crcGameName = 0;

            // My Values returns
            $my_value1 = 0; //0x40000000;
            $my_value2 = 0;
            $my_value3 = 0;
            $my_value4 = 0; //0xE8D5F2F;
            // Value 5 is CRC dos valores
            $my_value6 = ""; //"ggexp.des";
            $my_value7 = ""; //"SuperSS";
            $my_value8 = ""; //"SS";
            // Value9 é o Dummy

            // ---------- Encrypt Values -----------

            // Encrypt Value 1
            $value1 = (((($crcGameName - 2) & INT32_LIMIT) ^ (($_GET['dm'] + 5) & INT32_LIMIT)) ^ $my_value1) & INT32_LIMIT;

            // Encrypt Value 2
            $value2 = ((($crcGameName - 4) & INT32_LIMIT) ^ $value1) & INT32_LIMIT;
            $value2 = (($value2 ^ (($_GET['dm'] + 3) & INT32_LIMIT)) ^ $my_value2) & INT32_LIMIT;

            // Encrypt Value 3
            $value3 = ((($crcGameName - 6) & INT32_LIMIT) ^ $value2) & INT32_LIMIT;
            $value3 = (($value3 ^ (($_GET['dm'] + 9) & INT32_LIMIT)) ^ $my_value3) & INT32_LIMIT;

            // Encrypt Value 4
            $value4 = ((($value2 - 14) & INT32_LIMIT) ^ (($value1 - 9) & INT32_LIMIT)) & INT32_LIMIT;
            $value4 = ($value4 ^ (($crcGameName - 15) & INT32_LIMIT)) & INT32_LIMIT;
            $value4 = ($value4 ^ (($_GET['dm'] + 22) & INT32_LIMIT)) & INT32_LIMIT;
            $value4 = ($value4 ^ $my_value4) & INT32_LIMIT;

            // CRC My Values reply
            $crc_reply = ($value1 ^ $value2 ^ $value3 ^ $value4 ^ CONST_VALUE ^ $crcGameName);

            // ----------------- End ---------------
            
            // Constroi resposta
            $reply = $value1.' '.$value2.' '.$value3.' '.$value4.' '.$crc_reply.' '.$my_value6.' '.$my_value7.' '.$my_value8.' '.$_GET['dm'];

            // Log
            sLog::getInstance()->putLog("[Service] Reply: ".$reply);

            echo encryptStringResource($reply, rand());
        }
    }
?>