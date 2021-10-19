<?php
    // Arquivo encrypt_source.inc
    // Criado em 15/03/2020 as 19:58 por Acrisio
    // Definição e Implementação das funções que usa para cifra a resposta para o cliente

    include_once('base.inc');

    // function Encrypt Resource
    function encryptStringResource($_plained, $seed) {

        $str = textBinASCII($_plained);

        $data = &$str;
        $plain = $str;

        $salt = ($seed & 0xFF);

        $tmp = 0;
        $tmp2 = 0;

        $data[1] = ($salt & 0xFF);

        $salt = ($salt * 8) - $salt;
        $salt = $salt & 0xFFFFFFFF;

        $tmp2 = $salt + 0x46;

        $salt++;

        $salt = ($salt * 8) - $salt;
        $salt = $salt & 0xFFFFFFFF;

        $tmp = $salt + 0x46;

        $salt++;

        $data[0] = 2;

        $i = 0;

        while ($i < count($plain)) {

            $salt = ($salt * 8) - $salt;

            $salt = $salt & 0xFFFFFFFF;

            $data[$i + 4] = ($plain[$i] ^ (($salt + 0x46) & 0xFF)) & 0xFF;

            $salt++;
            $i++;
        }

        $data[2] = ((count($plain) >> 8) ^ ($tmp2 & 0xFF)) & 0xFF;
        $data[3] = (($tmp & 0xFF) ^ (count($plain) & 0xFF)) & 0xFF;

        //$data[$i + 4] = 0;

        return implode('', array_map('toHex', $str)).' ';
    }

    function decryptStringResource($_ciphered) {

        $data = hexstrBinASCII($_ciphered);

        if (isset($data) && count($data) < 4) {

            //echo 'String invalid';

            return "";
        }

        $cpy = $data;

        $tmp = 0;
        $tmp2 = 0;
        $tmp3 = 0;

        $tmp = $data[1];

        $tmp = ($tmp * 8) - $tmp;

        $tmp = $tmp & 0xFFFFFFFF;

        $data[2] ^= (($tmp & 0xFF) + 0x46) & 0xFF;

        ++$tmp;

        $tmp = ($tmp * 8) - $tmp;

        $tmp = $tmp & 0xFFFFFFFF;

        $data[3] ^= (($tmp & 0xFF) + 0x46) & 0xFF;

        $tmp2 = ($data[2] << 8) | $data[3];

        $tmp++;

        if (count($cpy) < $tmp2) {

            //echo 'Decrypted String size is invalid. '.count($cpy).' != '.$tmp2;

            return "";
        }

        $i = 0;

        while ($i < $tmp2) {

            $tmp = ($tmp * 8) - $tmp;

            $tmp = $tmp & 0xFFFFFFFF;

            $tmp3 = $data[$i + 4] ^ ((($tmp & 0xFF) + 0x46) & 0xFF);

            ++$i;

            $data[$i - 1] = $tmp3;

            $tmp++;

        }

        $data[$tmp2] = 0;
        
        return implode('', array_map('chr', $data));
     }
?>