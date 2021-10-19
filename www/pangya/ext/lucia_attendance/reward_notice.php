<?php
    // Arquivo reward_notice.php
    // Criado em 28/02/2020 as 08:45 por Acrisio
    // Definição e Implementação da página que mostra os prêmios que foi enviado para o mail do player

    include_once('source/lucia_attendance.inc');
    include_once('source/itens_data_singleton.inc');
    include_once('source/util.inc');
    include_once('source/debug_log.inc');

    class RewardNotice extends LuciaAttendance {

        public function show() {

            $this->checkHacking();

            $this->begin();

            echo '<link rel="stylesheet" href="include/main.css">';

            echo '<title>Lucia Attendance - Reward notice</title>';

            $this->middle();

            $this->content();

            $this->end();
        }

        private function checkHacking() {

            if (!isset($_SESSION))
                session_start();

            if (!isset($_SESSION['REWARD_ITEM']) || !isset($_SESSION['REWARD_ITEM']['items']) || !isset($_SESSION['REWARD_ITEM']['option'])
                || !is_array($_SESSION['REWARD_ITEM']['items']) || count($_SESSION['REWARD_ITEM']['items']) <= 0 || !is_numeric($_SESSION['REWARD_ITEM']['option'])) {

                // Add ao contando de tentativas de burlar o sistema
                $this->addTryHacking();

                DebugLog::Log("[RewardNotice] [checkHacking] Tentando burlar o sistema.");

                // Redireciona para a página principal já que ele conseguiu pegar o item com sucesso
                header("Location: ".LINKS['UNKNOWN_ERROR']);
                                            
                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        private function content() {

            echo '<table width="800" height="553" border="0" cellspacing="0" cellpadding>
                    <tr height="100">
                        <td></td>
                    </tr>
                    <tr>
                        <td align="center" vAlign="top">
                            <div style="width: 557px; height: 400px">
                                <table width="100%" height="100%" border="0" cellspacing="0" cellpadding="0" cell-colapse="colapse" style="border-top: 2px solid #46cec9">
                                    <tr>
                                        <td id="aba-help-content" colspan="3" bgColor="#46cec9" vAlign="middle" align="center" style="padding-bottom: 2px">
                                            <table width="552" border="0" cellspacing="0" cellpadding="0" style="background-color: #fff">
                                                <tr height="30px">
                                                    <td align="center">
                                                        <h2 style="font-weight: bold; font-size: 20px">Itens</h2>
                                                    </td>
                                                </tr>
                                                <tr>
                                                    <td align="center" vAlign="middle">
                                                        <table width="500" cellspacing="0" cellpadding="0" border="0">
                                                            <tr>
                                                                <td vAlign="top" class="text-reward"><font class="font-reward">'.$this->msg().'</font></td>
                                                                <td align="left" vAlign="top" class="itens-reward">
                                                                    <div class="container-reward">
                                                                        <span>';

            $this->itens();
            
            echo '                                                      </span>
                                                                    </div>
                                                                </td>
                                                                
                                                            </tr>
                                                        </table>
                                                    </td>
                                                </tr>
                                                <tr height="40px">
                                                    <td align="center">
                                                        <button id="btn-back-index" onclick="window.location = \''.LINKS['INDEX'].'?lg=ok\'" class="btn-reward">Go Home</button></a>
                                                    </td>
                                                </tr>
                                            </table>
                                        </td>
                                    </tr>
                                </table>
                            </div>
                        </td>
                    </tr>
                    <tr height="15">
                        <td></td>
                    </tr>
                </table>';
        }

        private function msg() {

            $str = '50CP já foi adicionado para sua conta.<br>
                  E esses são os itens que foram enviado para o seu mail box.';

            return $str;
        }

        private function itens() {

            $tmp = new stdClass();
            $item = null;

            echo '<div style="display: block; overflow-y: auto; overflow-x: hidden; width: 200px; height: 100%">
                    <table width="100%" cellspacing="0" cellpadding="0" border="0">';

            foreach ($_SESSION['REWARD_ITEM']['items'] as $el) {

                $tmp->type = getItemTypeName(getItemGroupType($el->typeid));
                $tmp->_typeid = $el->typeid;

                if ($tmp->type != "Card")
                    $item = findItemByCategory(ItemSingleton::getInstance()->getItens(), $tmp);
                else {

                    $tmp->type = getCardTypeName(getCardGroupType($el->typeid));

                    $item = findCardByCategory(ItemSingleton::getInstance()->getCards(), $tmp);
                }

                if ($item)
                    echo '<tr><td width="100"><img style="display: inline" src="'.BASE_IMG.'item/'.strtolower(urlencode($item->icon)).'.png" alt="'.$item->description.'" /></td><td align="left" vAlign="middle"> Quantidade: '.$el->qntd.'</td></tr>';
            }

            echo '</table></div>';

            // Clear
            unset($_SESSION['REWARD_ITEM']);
            
        }
    }

    $reward_notice = new RewardNotice();

    $reward_notice->show();
?>