<?php
    // Arquivo Club_New_FAQ.php
    // Criado em 16/07/2019 as 10:59 por Acrisio
    // Definição e Implementação da classe FAQ

    include_once("source/guild_new.inc");

    class FAQ extends GuildNew {

        public function show() {

            $this->begin();

            echo '<title>Guild FAQ</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function content() {

            echo '<form id="ctl00" method="post" action="./Club_New_FAQ.php">';

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';

            // Title
            echo '  <tr>
                        <td colspan="2">
                            <img src="img/title04.gif">
                        </td>
                    </tr>';

            // Text
            echo '  <tr>
                        <td height="10" colspan="2">
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
                                                <td height="257" width="557" align="center">
                                                    <iframe id="agree" height="360" border="0" src="html/faq0.html" frameBorder="0" width="557">
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

    // Guild FAQ
    $faq = new FAQ();

    $faq->show();
?>