<html>
<head></head>
<body>
	<?php
		include_once('../config/save_log.inc');

		$arr = [];

		$arr['fail_code'] = isset($_POST['FailCode']) ? $_POST['FailCode'] : 0;
		$arr['fail_text'] = isset($_POST['FailText']) ? $_POST['FailText'] : 'unknown';
		$arr['fail_reason'] = isset($_POST['FailReason']) ? $_POST['FailReason'] : 'unknown';
		$arr['ret1'] = isset($_POST['Ret1']) ? $_POST['Ret1'] : 'unknown';
		$arr['ret2'] = isset($_POST['Ret2']) ? $_POST['Ret2'] : 'unknown';
		$arr['update_ver'] = isset($_POST['UpdateVer']) ? $_POST['UpdateVer'] : 'unknown';
		$arr['patch_num'] = isset($_POST['PatchNum']) ? $_POST['PatchNum'] : 0;
		$arr['pre_patch_num'] = isset($_POST['PrePatchNum']) ? $_POST['PrePatchNum'] : 0;
		
		$file_name = "Update Fail Log - ";

		foreach ($_FILES as $key => $value)
			if (isset($value['name']) && isset($value['tmp_name']))
				save_log($value['name'], $value['tmp_name'], $file_name, $arr);

		//file_put_contents("Files.txt", serialize($_FILES));

		//file_put_contents("Bug_log.txt", serialize($_POST));
	?>
	0<br />Success
</body>
</html>