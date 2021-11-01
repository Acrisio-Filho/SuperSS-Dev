<?php
    include_once('config/save_log.inc');
    include_once('config/encrypt_source.inc');

    // Log
    include_once('config/log.inc');

    if (isset($_POST['hl']) && isset($_POST['dummy']) && is_numeric($_POST['dummy'])) {

        if (isset($_FILES) && isset($_FILES['erlfile']) && isset($_FILES['erlfile']['name']) && isset($_FILES['erlfile']['type'])
            && isset($_FILES['erlfile']['tmp_name']) && isset($_FILES['erlfile']['error']) && isset($_FILES['erlfile']['size'])) {
            
            // Salva arquivo
            save_log($_FILES['erlfile']['name'], $_FILES['erlfile']['tmp_name'], "GameGuard NProtect(HL) Log Report - ", null);
        }

        // Resposta
        $code = decryptStringResource($_POST['hl']);

        if (isset($code) && $code != '') {

            $first = explode('|', $code);

            if ($first >= 19) {
                
                $obj = new stdClass();

                $obj->error_name = $first[0];
                $obj->var_error1 = $first[1];
                $obj->game_id = $first[2];
                $obj->null = $first[3];
                $obj->ost = $first[4];
                $obj->osv = $first[5];
                $obj->cpu = $first[6];
                $obj->osc = $first[7];
                $obj->var_error2 = $first[8];
                $obj->moon_type = $first[9];
                $obj->hack_name = $first[10];
                $obj->hack_path = $first[11];
                $obj->pc_name = $first[12];
                $obj->pc_user_name = $first[13];
                $obj->mac = $first[14];
                $obj->date = $first[15];
                $obj->unknown_hash = $first[16];
                $obj->number_unknown = $first[17];   // Pode ser o número de HDs
                $obj->hwid = $first[18];
                $obj->dummy = $_POST['dummy'];
            }

            if (isset($obj) && is_numeric($obj->var_error1) && is_numeric($obj->var_error2)) {

                define("CONSTANT_VALUE_1", 0xFAE9CD);
                define("LIMIT_INT32", 0xFFFFFFFF);

                $crcErrorName = CRCHashStr($obj->error_name); // CRC error
                $crcGameName = CRCHashStr($obj->game_id); // CRC Game

                // Multiplica os números para gerar o hash
                $multi_value1 = $obj->var_error1;
                $multi_value2 = $obj->var_error2;

                // My Value
                // Values que ele compara
                // 1°: 0x013352BA -- gd
                // 2°: 0x01337AEE -- gd x
                // 3°: 0x01241189 -- gd end
                $my_value = 0x01241189; //0x12225;
                // Value 2 is crc from my value

                // Value 1
                $value1 = ((($crcGameName * $multi_value2/*CONSTANT_VALUE_3*/) & LIMIT_INT32) ^ ($my_value ^ ($obj->dummy + 5))) & LIMIT_INT32;

                // CRC from my value
                $crcValue = ((($crcErrorName  * $multi_value1/*CONSTANT_VALUE_2*/) & LIMIT_INT32) ^ ($value1 ^ CONSTANT_VALUE_1)) & LIMIT_INT32;

                // Log
                sLog::getInstance()->putLog("[HL] Value1: ".$value1.", CRCValue: ".$crcValue.", Dummy: ".$obj->dummy.', Multi[0]: '.$multi_value1.', Multi[1]: '.$multi_value2.". Object[".json_encode($obj)."]");

                echo encryptStringResource($value1." ".$crcValue, rand());
            }
        }
    }
?>