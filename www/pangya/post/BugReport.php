<html>
<head></head>
<body>
	<?php
		include_once('../config/save_log.inc');

		$arr = [];

		$arr['code'] = isset($_POST['code']) ? $_POST['code'] : 0;
		$arr['userid'] = isset($_POST['userid']) ? $_POST['userid'] : 'unknown';
		$arr['packet_ver'] = isset($_POST['packet_ver']) ? $_POST['packet_ver'] : 'unknown';
		$arr['client_ver'] = isset($_POST['client_ver']) ? $_POST['client_ver'] : 'unknown';
		$arr['comment'] = isset($_POST['comment']) ? $_POST['comment'] : 'unknown';
		
		$file_name = "Projectg Bug report - ";

		foreach ($_FILES as $key => $value)
			if (isset($value['name']) && isset($value['tmp_name']))
				save_log($value['name'], $value['tmp_name'], $file_name, $arr);

		file_put_contents("../report_files/Get_log_".time().".txt", serialize($_GET));

		file_put_contents("../report_files/Files_log_".time().".txt", serialize($_FILES));

		file_put_contents("../report_files/POST_log_".time().".txt", serialize($_POST));
	?>
	0<br />Success
</body>
</html>