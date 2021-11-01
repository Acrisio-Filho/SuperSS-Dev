<?php
    // Arquivo etp.php
    // Criado em 02/12/2020 as 18:11 por Acrisio
    // Definição e Implementação do sistema Entry Point

    include_once('source/entry_point_system.inc');

    class EntryPoint extends EntryPointSystem {

        public function show() {

            $this->checkFirstLogin();

            $this->begin();

            echo '<title>Entry Point</title>';

            $this->middle();

            $this->content();

            $this->end();
        }

        public function getPlataformName() {
            return ENTRYPOINT;
        }

        protected function content() {

            echo '<p style="font-size: 20px; font-weight: bold; color: goldenrod; margin-top: 35px; text-align: center;">
                    Doe para ajudar a manter o PangYa SuperSS online e ganhe CP
                <p>
                <br>
                <div style="margin-left: 15px; margin-top: 70px">
                    Para fazer uma doação clique nesse <a class="link" href="'.$_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/pangya.php?link=8'.'" target="_blank">link</a> que tem os botões de doação.
                    <br>
                    <br>
                    Depois que fizer a doação registre ela no menu "Donation register"<br> para receber um E-Pin para trocar por Cookie Point no menu "E-Pin exchange".
                    <br>
                    <br>
                    - A cada 1 Real de doação você recebe 20 Cookie Point.
                </div>';
        }
    }

    // Entry Point
    $entry_point = new EntryPoint();

    $entry_point->show();
?>