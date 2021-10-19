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
?>