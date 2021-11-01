<?php
    // Arquivo check_mark.inc
    // Criado em 24/07/2019 as 17:05 por Acrisio
    // Definição e Implementação da classe ValidMark

    class ValidMark {

        private const MAX_SIZE_IMG = 5120;
        private const WIDTH_IMG = 22;
        private const HEIGHT_IMG = 20;

        public static function checkMark($file) {

            $ret = ['state' => true, 'msg' => '', 'img' => null];

            // Verifica se não carregou a imagem, por que ela é muito grante
            if ($file != null && isset($file['error']) && ($file['error'] == UPLOAD_ERR_INI_SIZE || $file['error'] == UPLOAD_ERR_FORM_SIZE)) {

                $ret['state'] = false;
                $ret['msg'] = 'Failed to upload image, it is too large.';

                return $ret;
            }
			
			// !@ Debug teste
			//error_log('[check_mark][File]:  '.$file['tmp_name'], 0);

            $img = ($file != null && !empty($file['tmp_name']) && $file['error'] == 0 && file_exists($file['tmp_name'])) ? getimagesize($file['tmp_name']) : null;

            if ($img != null) {

                // Check Size
                if ($file['size'] > ValidMark::MAX_SIZE_IMG) {
                   
                    $ret['state'] = false;
                    
                    $ret['msg'] = 'Image too big. size: '.$file['size'].' bigger then '.ValidMark::MAX_SIZE_IMG;
                }

                // Check dimensões
                if ($img != null && ($img[0] != ValidMark::WIDTH_IMG || $img[1] != ValidMark::HEIGHT_IMG)) {
                    
                    $ret['state'] = false;
                    
                    $ret['msg'] = 'Image dimensions is not valid. Please use an image with the dimensions '.ValidMark::WIDTH_IMG.'x'.ValidMark::HEIGHT_IMG;
                }
                
                // Check Type
                switch ($img[2]) {
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