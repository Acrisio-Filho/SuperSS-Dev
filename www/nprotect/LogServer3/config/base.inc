<?php
	function after ($_this, $inthat)
    {
        if (!is_bool(strpos($inthat, $_this)))
        return substr($inthat, strpos($inthat,$_this)+strlen($_this));
    };

    function after_last ($_this, $inthat)
    {
        if (!is_bool(strrevpos($inthat, $_this)))
        return substr($inthat, strrevpos($inthat, $_this)+strlen($_this));
    };

    function before ($_this, $inthat)
    {
        return substr($inthat, 0, strpos($inthat, $_this));
    };

    function before_last ($_this, $inthat)
    {
        return substr($inthat, 0, strrevpos($inthat, $_this));
    };

    function between ($_this, $that, $inthat)
    {
        return before ($that, after($_this, $inthat));
    };

    function between_last ($_this, $that, $inthat)
    {
     return after_last($_this, before_last($that, $inthat));
    };

	// use strrevpos function in case your php version does not include it
	function strrevpos($instr, $needle)
	{
	    $rev_pos = strpos (strrev($instr), strrev($needle));
	    if ($rev_pos===false) return false;
	    else return strlen($instr) - $rev_pos - strlen($needle);
	};
	
	#substr(".", 0, (strlen(".") - (strpos(REVERSE("."), REVSERSE("str"))) - strlen("str"))

    #substr(".", (strlen(".") - (strpos(REVERSE("."), REVERSE("str"))) - strlen("str")) + strlen(str))
    
    // Verifica se tem tipo invalido no nome do arquivo
    function checkType($type) {

		$type_invalids = '/(php\d|php|html|htm|asp|aspx|vbs|js|css)$/i';

		if (($result_match = preg_match($type_invalids, $type)) == 0)
			return true;
		
		return false;
	}

    // Check File name for diretives of path and clear they
    function checkFileName($name) {

        $search = ['/', '//', '\\', '..'];

        $new = str_replace($search, "", $name);

        // Verifica o tipo do arquivo e vê se é um dos permitidos
        if (checkType('.'.after_last('.', $new)))
            return $new;

        return before_last('.', $new).'.txt';  // Valor padrão se der error, para ele não poder executar nada se tiver outro tipo no nome
    }

    function textBinASCII($text) {
        return array_map('ord', str_split($text));
    }

    function hexBinASCII($text, $count_el = 2) {
        return array_map('hexdec', str_split($text, $count_el));
    }

    function toHex($el) {
        return str_pad(dechex($el), 2, '0', STR_PAD_LEFT);
    }

    function hexstrBinASCII($_strhex) {

        $plain = textBinASCII($_strhex);

        if (isset($_strhex) && !empty($_strhex)) {

            if ($_strhex[0] == '\\')
                $plain = hexBinASCII($_strhex, 4);
            else if ($_strhex[0] == '0' && $_strhex[1] == '2')
                $plain = hexBinASCII($_strhex, 2);
        }

        return $plain;
    }

    function CRCHashStr($_str) {
            
        $hash = 0;

        if (isset($_str) && !empty($_str)) {

            $strBin = textBinASCII($_str);

            if (isset($strBin) && !empty($strBin)) {

                $count = count($strBin);

                $i = 0;
                $c = $strBin[$i++];

                if ($c != 0) {
                    
                    do {
                        $hash = ($c + ((($hash << 5) & 0xFFFFFFFF) - $hash)) & 0xFFFFFFFF;
                    } while ($i < $count && ($c = $strBin[$i++]) != 0);
                }
            }
        }
    
        return $hash;
    }

    function myChr($value) {
        return decbin($value);
    }
?>