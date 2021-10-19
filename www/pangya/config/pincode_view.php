<?php
	// Esse arquivo visualizar os pincode que ainda não foi usado
	if (file_exists('config/keys.txt')) {
		$keys = unserialize(file_get_contents('config/keys.txt'));

		if (isset($keys) && is_array($keys) && count($keys) > 0) {
			echo '<html lang=PT-BR"><head><meta charset="UTF-8" />
				<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
				<link rel="icon" href="/favicon.ico" type="image/x-icon">
			 <title>Pin code Viewer</title></head><body>';

			echo '<style>
					.used {
						background-color: red;
						color: white;
					}
					.avaliable {
						background-color: green;
						color: white;
					}
				  </style>';

			for ($i = 0; $i < count($keys); $i++) {
				echo '<div>';

				echo '<h4>Pin code: '.$keys[$i]['unique_id'].' - <span class="'.(($keys[$i]['flag']) ? 'used"> Usado' : 'avaliable"> Disponível').'</span></h4>';

				echo '</div>';
			}

			echo '</body></html>';
		}
	}
?>