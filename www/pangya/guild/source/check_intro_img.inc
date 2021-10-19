<?php
    // Arquivo check_intro_img.inc
    // Criado em 24/09/2019 as 02:16 por Acrisio
    // Definição e Implementação da classe ValidIntroImg

    class ValidIntroImg {

        private const MAX_SIZE_IMG = 2097152;
        private const WIDTH_IMG = 320;
        private const HEIGHT_IMG = 240;

        private const CHECK_DIMENSION = false;

        public static function checkIntroImg($file) {

            $ret = ['state' => true, 'msg' => '', 'img' => null];

            // Verifica se não carregou a imagem, por que ela é muito grante
            if ($file != null && isset($file['error']) && ($file['error'] == UPLOAD_ERR_INI_SIZE || $file['error'] == UPLOAD_ERR_FORM_SIZE)) {

                $ret['state'] = false;
                $ret['msg'] = 'Failed to upload image, it is too large.';

                return $ret;
            }
			
			// !@ Debug teste
			//error_log('[check_intro_img][File]:  '.$file['tmp_name'], 0);

            $img = ($file != null && !empty($file['tmp_name']) && $file['error'] == 0 && file_exists($file['tmp_name'])) ? getimagesize($file['tmp_name']) : null;

            if ($img != null) {

                // Check Size
                if ($file['size'] > ValidIntroImg::MAX_SIZE_IMG) {
                   
                    $ret['state'] = false;
                    
                    $ret['msg'] = 'Image too big. size: '.$file['size'].' bigger then '.ValidIntroImg::MAX_SIZE_IMG;

                    return $ret;
                }

                // Check dimensões
                if (ValidIntroImg::CHECK_DIMENSION && ($img[0] != ValidIntroImg::WIDTH_IMG || $img[1] != ValidIntroImg::HEIGHT_IMG)) {
                    
                    $ret['state'] = false;
                    
                    $ret['msg'] = 'Image dimensions is not valid. Please use an image with the dimensions '.ValidIntroImg::WIDTH_IMG.'x'.ValidIntroImg::HEIGHT_IMG;

                    return $ret;
                }
                
                // Check Type
                switch ($img[2]) {
                    case IMAGETYPE_BMP:
                        $im = imagecreatefrombmp($file['tmp_name']);

                        if ($im != FALSE) {

                            imagealphablending($im, false);
                            imagesavealpha($im, true);

                            ob_start();

                            imagepng($im);

                            $ret['img'] = base64_encode(ob_get_clean());

                            imagedestroy($im);

                            $img['mime'] = 'image/png';

                        }else {

                            $ret['state'] = false;
                            $ret['msg'] = 'Image type not supported: '.$img['mime'];
                        }

                        break;
                    case IMAGETYPE_PNG:
                        $im = imagecreatefrompng($file['tmp_name']);

                        if ($im != FALSE) {
                            
                            imagealphablending($im, false);
                            imagesavealpha($im, true);

                            ob_start();

                            imagepng($im);
                            
                            $ret['img'] = base64_encode(ob_get_clean());

                            imagedestroy($im);

                            $img['mime'] = "image/png";
                       
                        }else {

                            $ret['state'] = false;
                            $ret['msg'] = 'Image type not supported: '.$img['mime'];
                        }
                        break;
                    case IMAGETYPE_GIF:
                        $im = imagecreatefromgif($file['tmp_name']);

                        if ($im != FALSE) {

                            imagealphablending($im, false);
                            imagesavealpha($im, true);

                            ob_start();
                            
                            imagepng($im);
                            
                            $ret['img'] = base64_encode(ob_get_clean());
                            
                            imagedestroy($im);

                            $img['mime'] = "image/png";
                        
                        }else {

                            $ret['state'] = false;
                            $ret['msg'] = 'Image type not supported: '.$img['mime'];
                        }
                        break;
                    case IMAGETYPE_JPEG:
                        $im = imagecreatefromjpeg($file['tmp_name']);

                        if ($im != FALSE) {

                            imagealphablending($im, false);
                            imagesavealpha($im, true);

                            ob_start();

                            imagepng($im);

                            $ret['img'] = base64_encode(ob_get_clean());

                            imagedestroy($im);
                            
                            $img['mime'] = "image/png";
                        
                        }else {

                            $ret['state'] = false;
                            $ret['msg'] = 'Image type not supported: '.$img['mime'];
                        }
                        break;
                    default:
                        $ret['state'] = false;
                        $ret['msg'] = 'Image type not supported: '.$img['mime'];
                        break;
                }
            
            }else {

                $ret['state'] = false;
                $ret['msg'] = '1';  // Sem imagem para verifica e não mostra mensagem
            }

            return $ret;
        }
    }
?>