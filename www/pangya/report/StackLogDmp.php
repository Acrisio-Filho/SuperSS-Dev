<html>
<head></head>
<body>
	<?php
		include_once('../config/save_log.inc');

		$arr = [];

		$arr['id'] = isset($_POST['id']) ? $_POST['id'] : 'unknown';
		$arr['version'] = isset($_POST['version']) ? $_POST['version'] : 'unknown';
		$arr['packet_ver'] = isset($_POST['packetversion']) ? $_POST['packetversion'] : 'unknown';
		$arr['bugs_point_line'] = isset($_POST['BugsPointLine']) ? $_POST['BugsPointLine'] : 'unknown';
		$arr['module_name'] = isset($_POST['ModuleName']) ? $_POST['ModuleName'] : 'unknown';
		
		$file_name = "Stack Log Dmp - ";

		foreach ($_FILES as $key => $value)
			if (isset($value['name']) && isset($value['tmp_name']))
				save_log($value['name'], $value['tmp_name'], $file_name, $arr);

		//file_put_contents("Files.txt", serialize($_FILES));

		//file_put_contents("Bug_log.txt", serialize($_POST));
	?>
	0<br />Success
</body>
</html>