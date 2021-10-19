<?php
	// Gera Chaves
	$keys = [];
	$keys_len = 0;
	$id = 0;
	$count = 170;

	$add_new = true;

	while ($count > 0) {
		$keys_len = count($keys);

		$id = md5(uniqid(rand(), true));

		for ($j = 0; $j < $keys_len; $j++) {
			if ($id == $keys[$j]['unique_id']) {
				$add_new = false;
				break;
			}
		}

		if ($add_new) {
			$keys[] = ['unique_id' => $id, 'flag' => 0];
			$count--;
		}
	}

	file_put_contents('config/keys_2.txt', serialize($keys));

	echo 'Terminou';
?>