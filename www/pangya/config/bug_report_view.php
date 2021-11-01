<?php
	// Arquivo que mostra os bug report dos clientes

	if (file_exists('config/bug_report.txt')) {
		$bug_report = unserialize(file_get_contents('config/bug_report.txt'));

		if (isset($bug_report) && is_array($bug_report) && count($bug_report) > 0) {
			echo '<html lang=PT-BR"><head><meta charset="UTF-8" />
				<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
				<link rel="icon" href="/favicon.ico" type="image/x-icon">
			 <title>Bug Report Viewer</title></head><body>';
			
			for ($i = 0; $i < count($bug_report); $i++) {
				echo '<div>';	

				echo '<h4>UID: '.$bug_report[$i]['uid'].' ID: '.$bug_report[$i]['id'].'</h4>';
				echo '<h4>Bug Report: <br><pre>'.$bug_report[$i]['bug_report'].'</pre></h4>';

				echo '</div>';
			}

			echo '</body></html>';
		}
	}
?>