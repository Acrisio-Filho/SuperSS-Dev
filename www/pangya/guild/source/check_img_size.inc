<?php
    // Arquivo check_img_size.inc
    // Criado em 24/09/2019 as 03:53 por Acrisio
    // ValidImgSize

    class ValidImgSize {

        private const WIDTH_SIZE = 320;
        private const HEIGHT_SIZE = 240;

        public static function checkImgSize($link, $base, $file) {

            $ret = '';

            if ($file != null && $file != '' && $base != null && $base != '' && $link != null && $link != '') {
			
				// !@ Debug teste
				//error_log('[check_img_size][File]:  '.$base.$file, 0);
                
				if (file_exists($base.$file) && ($img = getimagesize($base.$file)) != null)
					$ret = 'width="'.($img[0] > ValidImgSize::WIDTH_SIZE ? ValidImgSize::WIDTH_SIZE : $img[0]).'" height="'.($img[1] > ValidImgSize::HEIGHT_SIZE ? ValidImgSize::HEIGHT_SIZE : $img[1]).'" src="'.$link.'/'.$file.'"';
            }

            return $ret;
        }
    }

?>