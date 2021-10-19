<?php
    // Arquivo Club_Agreement.php
    // Criado em 16/07/2019 as 11:03 por Acrisio
    // Definição e Implementação da classe Agreement

    include_once("source/guild_new.inc");

    class Agreement extends GuildNew {

        public function show() {

            $this->begin();

            echo '<title>Guild Agreement</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function content() {

            echo '<form id="ctl00" method="post" action="./Club_Agreement.php">';

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';

            // Title
            echo '  <tr>
                        <td colspan="2">
                            <img src="img/title05.gif">
                        </td>
                    </tr>';

            // Text
            echo '  <tr>
                        <td height="10">
                        </td>
                    </tr>';

            // Termos
            echo '  <tr align="center">
                        <td colspan="2">
                            <table width="570" style="border-collapse: collapse" borderColor="#CCCCCC" cellspacing="0" cellpadding="5" border="1">
                                <tr>
                                    <td width="556">
                                        <table cellspacing="0" cellpadding="10" width="577" bgColor="#f3f3f3" border="0">
                                            <tr>
                                                <td height="360" width="557" align="center">
                                                    <iframe id="agree" height="360" border="0" src="html/agreement.html" frameBorder="0" width="557">
                                                    </iframe>
                                                </td>
                                            </tr>
                                        </table>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>';

            // Fecha table
            echo '</table>';

            // Fecha form
            echo '</form>';
        }
    }

    // Guild Agreement
    $agreement = new Agreement();

    $agreement->show();
?>