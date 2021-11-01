<?php

    echo '<form action="pangya.php?link=7&id='.$_SESSION['player_cadastro']['index'].'&key='.$_SESSION['player_cadastro']['key'].'&yes=1" method="POST" id="fo_ca">
            <div><label class="la_ca">ID: </label><input type="text" name="id" pattern="[A-Za-z0-9]{4,16}" required></div>
            <div><label class="la_ca">Senha: </label><input type="password" name="pass" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required></div>
            <div><label class="la_ca">Confirma Senha: </label><input type="password" name="confirm_pass" title="de 6-16 caracters e não espaços" pattern="[0-9A-z@#$%&*!_?=+\'\-]{6,16}" required></div>
            <div><label class="la_ca">Sexo: </label><div><input type="radio" name="sexo" value="0" checked> Masculino <input type="radio" name="sexo" value="1"> Feminino</div></div>';

    echo '<input type="hidden" name="index" value="'.$this->argumentos['get']['id'].'">';
        
    $ip_cliente = getenv('REMOTE_ADDR');

    echo '<input type="hidden" name="ip" value="'.$ip_cliente.'">
            <input type="submit" value="Enviar">
            </form>';
?>