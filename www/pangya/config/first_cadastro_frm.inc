<?php

        // Criar o desavio de checkbox do reCAPTCHA
        include_once('recaptcha.inc');

        echo reCAPTCHA::loadScript_V2_from_V3();

        echo '<form '.reCAPTCHA::makeAttrEventOnSubmitCheck_V2().' action="pangya.php?link=3&yes=1" method="POST">
                <div id="fo_ca">
                <div><label class="la_ca">Nome: </label><input type="text" name="nome" pattern="[a-zA-Z ]{4,20}" required></div>
                <div><label class="la_ca">Sobre Nome: </label><input type="text" name="sobre_nome" pattern="[a-zA-Z ]{4,40}" required></div>
                <div><label class="la_ca">Email: </label><input type="email" name="email" pattern="^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$" required></div>
                <div><label class="la_ca">Confirme Email: </label><input type="email" name="confirm_email" pattern="^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,4}$" required></div>
                </div>';
                
        $ip_cliente = getenv('REMOTE_ADDR');

        // Criar o desavio de checkbox do reCAPTCHA
        echo reCAPTCHA::makeCheckBox_V2();

        echo '<input type="hidden" name="ip" value="'.$ip_cliente.'">
                <input style="border: 1px solid black; margin-top: 20px" type="submit" value="Enviar">
                </form>';

?>