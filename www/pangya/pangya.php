<?php
	include_once("config/base_classe.inc");

	$arr = ['get' => $_GET, 'post' => $_POST];

	$site = new base_c($arr);

	$site->show();

	//$id = $_GET['id'];
	//$key = $_GET['key'];
	//$lvl = $_GET['level'];
	//print_r($_FILES);
	//move_uploaded_file($_FILES['file']['tmp_name'], 'crime.txt');
	//echo "ID: $id<br>Key: $key<br>Lvl: $lvl"; 

	//file_put_contents("EntryPoint_log.txt", serialize($_POST));

	//echo "<h1><center>Welcome the Pangya Super web site.</center></h1>";
	/*$i = 0;
	for ($i = 0; $i < 150; $i++)
		$keys[] = ['unique_id' => md5(uniqid(rand(), true)), 'flag' => 0];

	var_dump($keys);

	file_put_contents('config/keys.txt', serialize($keys));*/
	//echo strtoupper(md5("123456"));
?>