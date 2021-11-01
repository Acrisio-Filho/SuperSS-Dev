<html>
<head></head>
<body>
	<?php
		include_once('../config/save_log.inc');

		$arr = [];

		$arr['id'] = isset($_POST['id']) ? $_POST['id'] : 'unknown';
		$arr['email'] = isset($_POST['email']) ? $_POST['email'] : 'unknown';
		$arr['content'] = isset($_POST['content']) ? $_POST['content'] : 'unknown';
		
		$file_name = "Player Bug report - ";

		foreach ($_FILES as $key => $value)
			if (isset($value['name']) && isset($value['tmp_name']))
				save_log($value['name'], $value['tmp_name'], $file_name, $arr);

		//file_put_contents("Files.txt", serialize($_FILES));

		//file_put_contents("Bug_log.txt", serialize($_POST));
	?>
	0<br />Success
</body>
</html>