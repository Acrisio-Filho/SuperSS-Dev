<html>
<head></head>
<body>
	<?php
		include_once('../config/save_log.inc');

		$arr = [];

		$arr['id'] = isset($_POST['id']) ? $_POST['id'] : 'unknown';
		$arr['tipo'] = isset($_POST['type']) ? $_POST['type'] : 0;

		$file_name = "Crime report - player id=".$arr['id'].", tipo= ".$arr['tipo']." - ";

		foreach ($_FILES as $key => $value)
			if (isset($value['name']) && isset($value['tmp_name']))
				save_log($value['name'], $value['tmp_name'], $file_name, $arr);

		//file_put_contents("ssFiles.txt", serialize($_FILES));

		//file_put_contents("ssCrime_log.txt", serialize($_POST));
	?>
	0<br />Success
</body>
</html>