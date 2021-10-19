<?php
	include_once('base.inc');

	$dir_log = $_SERVER['DOCUMENT_ROOT'].'LogServer3/log/files/';

	function make_file_name($file_name, $file_tipo_ext) {
		
		$date = getdate();

		return $file_name."date=".$date['mday']."-".$date['mon']."-".$date['year']." ".$date['hours']."h ".$date['minutes']."m ".$date['seconds']."s-salt".rand().$file_tipo_ext;
	};

	function print_arr($arr) {
		$str = "";

		if (is_array($arr))
			foreach ($arr as $key => $value)
				$str .= $key." : ".$value."\xd\xa";

		return $str."\xd\xa";
	};

	function save_log($original_file_name, $tmp_file_name, $file_name, $arr_dados_add) {
		global $dir_log;

		// Verifica se o nome do arquivo tem caracteres proibidos e exclui eles
		$file_name = checkFileName($file_name);

		$tipo_file = ".".after_last('.', $original_file_name);

		// Verifica o tipo do arquivo e ver se é um dos tipos permitidos
		if (checkType($tipo_file)) {

			$new_file_name = $dir_log.make_file_name($file_name, $tipo_file);

			file_put_contents($new_file_name, NULL);

			if (is_array($arr_dados_add) && strcmp(strtoupper($tipo_file), ".LOG") == 0)  // Não pode arquivo dump só log
				file_put_contents($new_file_name, print_arr($arr_dados_add));

			file_put_contents($new_file_name, file_get_contents($tmp_file_name), FILE_APPEND);
		}
	};
?>