<?php

    require_once($_SERVER['DOCUMENT_ROOT']."/phpmailer/class.phpmailer.php");

    define('GUSER', '#');	// <-- Insira aqui o seu GMail
    define('GPWD', '#');		// <-- Insira aqui a senha do seu GMail

    function smtpmailer($para, $de, $de_nome, $assunto, $corpo) { 
        global $error;

        $mail = new PHPMailer();
        $mail->IsSMTP();		// Ativar SMTP
        $mail->SMTPDebug = 0;		// Debugar: 1 = erros e mensagens, 2 = mensagens apenas
        $mail->SMTPAuth = true;		// Autenticação ativada
        $mail->SMTPSecure = 'tls';	// SSL REQUERIDO pelo GMail, tls se o ssl não for
        $mail->Host = 'smtp.gmail.com';	// SMTP utilizado
        $mail->Port = 587;  		// A porta 587 deverá estar aberta em seu servidor
        $mail->Username = GUSER;
        $mail->Password = GPWD;
        $mail->SetFrom($de, $de_nome);
        $mail->Subject = $assunto;
        $mail->Body = $corpo;
        $mail->AddAddress($para);
        if(!$mail->Send()) {
            $error = 'Mail error: '.$mail->ErrorInfo; 
            return false;
        } else {
            $error = 'Mensagem enviada!';
            return true;
        }
    }
?>