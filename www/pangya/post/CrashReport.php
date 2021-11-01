<html>
<head></head>
<body>
	<?php
		include_once('../config/save_log.inc');

		$arr = [];

		$arr['uid'] = isset($_POST['uid']) ? $_POST['uid'] : 0;
		$arr['nation'] = isset($_POST['nation']) ? $_POST['nation'] : 0;

		$file_name = "Crash report - ";

		foreach ($_FILES as $key => $value)
			if (isset($value['name']) && isset($value['tmp_name']))
				save_log($value['name'], $value['tmp_name'], $file_name, $arr);

		//file_put_contents("Files.txt", serialize($_FILES));

		//file_put_contents("Crime_log.txt", serialize($_POST));
	?>
	0<br />Success
</body>
</html>