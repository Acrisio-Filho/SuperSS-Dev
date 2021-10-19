<?php
    // Arquivo lucia_attendance.php
    // Criado em 28/02/2020 as 05:39 por Acrisio
    // Página principal do Sistem Lucia Attendance

    include_once('source/lucia_attendance.inc');
    include_once('source/debug_log.inc');

    class Index extends LuciaAttendance {

        private const NUM_KEYS = 6;
        private const KEY_POSITIONS = ["X" => [201, 278, 355, 432, 509, 586], 
                                       "Y" => [259, 259, 259, 259, 259, 259]];
        
        public function show() {

            $this->checkPost();

            $this->begin();

            echo '<title>Lucia Attendance - Index</title>';

            $this->middle();

            $this->content();

            $this->end();
        }

        private function checkPost() {

            if (isset($_POST) && !empty($_POST) && isset($_POST['check']) && !empty($_POST['check']) && $_POST['check'] == 'true') {

                if ($this->isEnableDay()) {

                    // Marca Presença
                    $this->addCountDay();
                
                }else {

                    // Add ao contando de tentativas de burlar o sistema
                    $this->addTryHacking();

                    DebugLog::Log("[Index] tentou burlar o sistema");
                }
            
            }else if (!isset($_GET) || empty($_GET) || !isset($_GET['lg']) || $_GET['lg'] != 'ok') {

                // Add ao contando de tentativas de burlar o sistema
                $this->addTryHacking();

                DebugLog::Log("[Index] tentou burlar o sistema");
            }
        }

        private function textLucia() {

            // Verifica se o player está bloqueado
            $this->checkBlock();

            $enable = $this->isEnableDay();
            $getted_item = $this->isGettedItem();

            if ($this->blocked) {

                if (PlayerSingleton::getInstance()['IDState'] == LuciaAttendance::BLOCKED_BY_ADM_FLAG) {

                    $text = 'Hum..., o Administrador bloqueou você. Entre em contato com o Administrador.';

                }else {

                    $text = 'Hum..., você foi bloqueado por tentativa de hacking ';

                    switch (PlayerSingleton::getInstance()['BLOCK_TYPE']) {
                        case 1:
                            $text .= 'por um dia. <br>Vai libera em: '.(new DateTime(PlayerSingleton::getInstance()['BLOCK_END_DATE']))->format('Y-m-d H:m:s').' (GMT).';
                            break;
                        case 2:
                            $text .= 'por uma semana. <br>Vai leberar em: '.(new DateTime(PlayerSingleton::getInstance()['BLOCK_END_DATE']))->format('Y-m-d H:m:s').' (GMT)';
                            break;
                        case 3:
                            $text .= 'por um mês. <br>Vai liberar em: '.(new DateTime(PlayerSingleton::getInstance()['BLOCK_END_DATE']))->format('Y-m-d H:m:s').' (GMT)';
                            break;
                        case 0:
                        case 4: // Permanente
                            $text .= 'por tempo indeterminado.';
                            break;
                    }
                    
                }

            }else {

                if ($getted_item)
                    $text = 'Hum...., já vi que você já pegou o prêmio de hoje, volte amanhã para marca presença.';
                else if (!$enable) {

                    if ((PlayerSingleton::getInstance()['COUNT_DAY'] / 6) > 0.5) {

                        if (PlayerSingleton::getInstance()['COUNT_DAY'] == 6)
                            $text = 'Você já marcou presença hoje, volte amanhã para escolher um prêmio.';
                        else
                            $text = 'Você já marcou presença hoje, volte amanhã que falta poucos dias para você escolhe seu prêmio.';

                    }else
                        $text = 'Você já marcou presença hoje, volte amanhã.';
                    
                }else {

                    // Tempo que não entra no sistema
                    $diff = (new DateTime(date("Y-m-d")))->diff(DateTime::createFromFormat("!Y-m-d", (new DateTime(PlayerSingleton::getInstance()['LAST_DAY_ATTENDANCE']))->format("Y-m-d")), true);

                    $text = '';

                    // Mensagem pelo tempo sem entrar
                    if ($diff->days >= 3 && $diff->days < 5)
                        $text .= "Faz alguns dias que eu não te vejo por aqui.<br><br>";
                    else if ($diff->days >= 5 && $diff->days < 7)
                        $text .= "Nossa não veio me ver já faz um tempo.<br><br>";
                    else if ($diff->days >= 7 && $diff->days < 15)
                        $text .= "Mais de uma semana sem me ver, assim vou ficar triste. Sob!<br><br>";
                    else if ($diff->days >= 15 && $diff->days < 30)
                        $text .= "Um bom tempo você ficou sem vir aqui, assim vou te bloquear no orkut. Ungh!<br><br>";
                    else if ($diff->days >= 30 && $diff->days < 360)
                        $text .= "Grrr! Estou de mau de você já faz mais de um mês que você não vem me ver.<br><br>";
                    else if ($diff->days >= 360)
                        $text .= "Não quero mais te ver aqui, não veio me ver já faz mais de um ano. Grrr!<br><br>";

                    // Mensagem pelo dias da chave
                    if (PlayerSingleton::getInstance()['COUNT_DAY'] == 6)
                        $text .= 'Chegou o grande dia!. Clique no cube para escolher seu prêmio.';
                    else if ((PlayerSingleton::getInstance()['COUNT_DAY'] / 6) > 0.5) {
                        
                        if (PlayerSingleton::getInstance()['COUNT_DAY'] == 5)
                            $text .= 'Clique na chave para marca presença, falta só um dia para você escolher um prêmio.';
                        else
                            $text .= 'Clique na chave para marca presença, falta só alguns dias para você escolher um prêmio.';
                    }else
                        $text .= 'Clique na chave para marca presença.';

                }

            }

            // Mensagem padrão da Lucia
            echo '<font style="color: red; font-size: 20px">Lucia: </font>
                    Bem viando <font style="color: greenyellow; font-size: 20px">'.htmlspecialchars(PlayerSingleton::getInstance()['NICKNAME']).'.</font><br><br>';
                    
            // Mensagens dinâmicas
            echo $text;
        }

        private function content() {

            // Verifica se o player foi bloqueado
            $this->checkBlock();

            echo '	<table width="800" height="553" border="0" cellspacing="0" cellpadding="0">
                        <tr height="380">
                            <td>
                                <table height="380" border="0" cellspacing="0" cellpadding="0">
                                    <tr height="190">
                                        <td></td>
                                    </tr>
                                    <tr>
                                        <td width="200"></td>';
		
            $this->makeKeys();
            $this->makeCube();

            echo '					</tr>
                                </table>
                            </td>
                        </tr>
                        <tr>
                            <td vAlign="top" align="center">
                                <table width="786" height="166" style="background: url(img/legenda.png);" border="0" cellspacing="0" cellpadding="0">
                                    <tr>
                                        <td align="center" vAlign="middle">
                                            <table height="166" border="0" cellspacing="0" cellpadding="0">
                                                <tr height="7">
                                                    <td></td>
                                                </tr>
                                                <tr>
                                                    <td width="7px"></td>
                                                    <td vAlign="top" align="left" width="100%" style="padding-left: 10px">
                                                        <font style="FONT-SIZE: 20px; FONT-WEIGHT: BOLD; Color: #fff; overflow: hidden">';
            
            $this->textLucia();
        
            echo '                                      </font>
                                                    </td>
                                                    <td width="7px"></td>
                                                </tr>
                                                <tr height="7">
                                                    <td></td>
                                                </tr>
                                            </table>
                                        </td>
                                    </tr>
                                </table>
                            </td>
                        </tr>
                    </table>';
        }

        private function makeKeys() {

            $getted_item = $this->isGettedItem();
            $enable = ($this->isEnableDay() && !$getted_item);

            for ($i = 0; $i < Index::NUM_KEYS; ++$i) {
        
                echo '<td vAlign="middle">';
        
                echo '	<div class="bg-key" style="background: url(img/bg-'.(PlayerSingleton::getInstance()['COUNT_DAY'] == $i && $enable ? 'key.png' : 'inative-key.png').') no-repeat; width: 81px; height: 82px; position: absolute; left: '.Index::KEY_POSITIONS['X'][$i].'px; top: '.Index::KEY_POSITIONS['Y'][$i].'px">';
                
                if (PlayerSingleton::getInstance()['COUNT_DAY'] == $i && $enable && !$this->blocked)
                    echo '	<form method="POST" style="cursor: pointer">
                                <input type="hidden" name="check" value="true">
                                <input border="0" type="image" style="position: absolute; left: 4px; top: 10px" class="key" src="img/key.png"'.(PlayerSingleton::getInstance()['COUNT_DAY'] == $i && $enable ? ' onmouseover="this.src=\'img/key-over.png\'" onmouseout="this.src=\'img/key.png\'">' : '>');
                else
                    echo '<img style="border="0" class="key" style="position: absolute; left: 4px; top: 10px" src="img/key.png">';
                
                if (PlayerSingleton::getInstance()['COUNT_DAY'] > $i || $this->blocked || $getted_item)
                    echo '		<img border="0" style="position: absolute; left: -4px; top: -5px" class="checked" src="img/key-'.($this->blocked ? 'blocked' : 'checked').'.png">';
                
                if (PlayerSingleton::getInstance()['COUNT_DAY'] == $i && $enable && !$this->blocked)
                    echo '	</form>';
        
                echo '	</div>';
        
                echo '</td>';
            }
        }
        
        private function makeCube() {

            $enable = $this->isEnableDay();
            $getted_item = $this->isGettedItem();
        
            echo '	<td vAlign="center" style="overflow: hidden">';
            echo '		<div style="width: 146px; height: 146px; background: url(img/bg-'.(PlayerSingleton::getInstance()['COUNT_DAY'] == 6 && $enable || $getted_item ? 'cube.png' : 'inative-cube.png').'); position: absolute; left: 660px; top: 223px">';
        
            if (PlayerSingleton::getInstance()['COUNT_DAY'] == 6 && $enable && !$this->blocked)
                echo '		<form action="'.LINKS['REWARD'].'" method="POST" style="cursor: pointer">
                                <input type="hidden" name="check" value="true">
                                <input type="image" src="img/cube.png" onmouseover="this.src=\'img/cube-over.png\'" onmouseout="this.src=\'img/cube.png\'" style="position: absolute; left: 18px; top: 22px;">';
            else if (PlayerSingleton::getInstance()['COUNT_DAY'] == 0 && $getted_item)
                echo '			<img src="img/opened-cube.png" style="position: absolute; left: 18px; top: 22px;">';
            else
                echo '			<img src="img/cube.png" style="position: absolute; left: 18px; top: 22px;">';
        
            if ($this->blocked)
                echo '			<img border="0" style="position: absolute; left: -3px; top: -5px" width="105%" height="105%" class="checked" src="img/cube-blocked.png">';
        
            if (PlayerSingleton::getInstance()['COUNT_DAY'] == 6 && $enable && !$this->blocked)
                echo '		</form>';
        
            echo '		</div>';
            echo '	</td>';
        }
    }

    // Home
    $index = new Index();

    $index->show();
?>