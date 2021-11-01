<?php
	// Arquivo gacha_result.php
	// Criado em 24/05/2020 as 00:04 por Acrisio
	// Página Gacha Resultados do sorteio

	include_once("source/gacha_system.inc");
	include_once("source/debug_log.inc");
	include_once("source/player_play_singleton.inc");

	class GachaResult extends GachaSystem {

		protected $modo = PLAY_MODO::PM_ONE;
		protected $itens = [];

		public function show() {

			$this->checkPlayed();

			$this->begin();

            echo '<title>Gacha Result</title>';

            $this->middle();

            $this->content();

			$this->end();
			
			$this->clearSession();
		}

		protected function style() {

			echo '<style type="text/css">
					.ui-widget-overlay {
						position: absolute;
						top: 0;
						left: 0;
						width: 100%;
						height: 100%;
						background: #666666;
						opacity: .5;
						filter: Alpha(Opacity=50);
					}
			
					a img {
						border: 0px;
					}
			
					.ticket_count {
						font-size: 14px;
						text-align: right;
					}
			
					.page_count {
						text-align: center;
						padding-top: 16px;
						font-size: 16px
					}
			
					.get_item {
						margin: 0px 0px 10px 10px;
						float: left;
						border: solid 1px #F8C070;
					}
			
					.item_name {
						font-size: 12px;
						text-align: right;
						position: absolute;
						left: 66px;
						top: 10px;
						width: 190px;
						height: 19px;
					}
			
					.get_btn {
						position: absolute;
						left: 104px;
						top: 30px;
						width: 153px;
						height: 22px;
					}
			
					.item_icon {
						position: absolute;
						left: 5px;
						top: 5px;
						width: 50px;
						height: 50px;
					}
			
					.gacha_rare10 {
						background: url(img/item_rare.png) no-repeat bottom;
						width: 112px;
						height: 79px;
						margin: 2px auto;
						float: left;
						position: relative;
						left: -50%;
					}
			
					.gacha_rare10 img {
						margin-top: 25px;
					}
			
					.gacha_nr10 {
						background: url(img/item_nr.png) no-repeat bottom;
						width: 112px;
						height: 79px;
						float: left;
						margin: 2px auto;
						float: left;
						position: relative;
						left: -50%;
					}
			
					.gacha_nr10 img {
						margin-top: 25px;
					}
			
					.rareitem_name {
						font-size: 12px;
						display: inline;
						padding-top: 3px;
						text-decoration: underline;
					}
			
					.list {
						overflow: hidden;
						position: relative;
					}
			
					.item {
						float: left;
						margin: 0;
						padding: 0;
						position: relative;
						left: 50%;
					}
			
					.gacha_frame {
						width: 112px;
						height: 79px;
					}
				</style>';
		}

		protected function clearSession() {

			if (!isset($_SESSION))
				session_start();

			if (isset($_SESSION['player_play']))
				unset($_SESSION['player_play']);

			$itens = [];
		}

		protected function checkPlayed() {

			Design::checkIE();

			Design::checkLogin();

			if (PlayerPlaySingleton::getInstance() == null || PlayerPlaySingleton::getInstance()['created'] == false || PlayerPlaySingleton::getInstance()['SpinningLottery'] == false) {

				DebugLog::Log("Player ainda não jogou para acessar a página resultados.");

				Header("Location: ".LINKS['GACHA_WHATS']);
				
				// exit script para redirecionar para a página no head
				exit();

				// Não sei se ele sai apartir daí então vou colocar o return da função para garantir
				return;
			}

			// Modo
			$this->modo = PlayerPlaySingleton::getInstance()['modo'];

			// Itens
			$this->itens = PlayerPlaySingleton::getInstance()['itens'];
		}

		protected function content() {

			echo '<div align="center" style="position:absolute; left:190px; top:100px; width:610px; height:410px; padding-top:10px;">';

			$this->itemList();

			// Botões de jogar de novo e do Twiter
			echo '<div align="center" style="margin-top:25px;">';

			// Verifica se o player ainda tem ticket para jogar mais 1
			if (PlayerSingleton::checkPlayerHaveTicketToPlay($this->modo)) {
				
				// Form
				echo '<form name="MoreOne" method="POST" action="'.LINKS['GACHA_LOTTERY'].'?count='.$this->modo.'">';

				if ($this->modo == PLAY_MODO::PM_TEN)
					echo '	<input type="image" name="Btn_OneMore" id="Btn_OneMore" src="./img/10btn.png" style="border-width:0px;" />';
				else
					echo '<input type="image" name="Btn_OneMore" id="Btn_OneMore" src="./img/1btn.png" style="border-width:0px;" />';

				echo '</form>';
				// Form end
			}
			
			echo '	<br /><br />
					<span style="font-size:14px; color:#CC0000"><strong>*You can receive the item from "Earned Items" on the left menu.</strong></span>
					<br /><br /><br />
				</div>';

			echo '</div>';
		}

		protected function itemList() {

			echo '<table align="center" width="610" height="228" border="0" cellpadding="0" cellspacing="0" style="background:url(img/item_bg.png) no-repeat;">
					<tr>
						<td width="610" height="40">&nbsp;</td>
					</tr>
					<tr>
						<td align="center" style="padding:0 25px; width:560px; height:188px; text-align:center;">
							<div class="list">
								<div class="item">';

			if (!empty($this->itens)) {

				foreach($this->itens as $item) {

					echo '<div class="gacha_'.($item['RARITY_TYPE'] == RARITY_TYPE::RT_RARE ? 'rare' : 'nr').'10">
							<div class="gacha_frame">';

					foreach($item['ITEM'] as $subItem)
						echo '	<img src="img/th_'.$subItem['TYPEID'].'.gif" width="50" height="50" alt="'.$subItem['NAME'].'/'.$subItem['QNTD'].'" title="'.$subItem['NAME'].'/'.$subItem['QNTD'].'" />';

					echo '	</div>
						</div>';
				}

				echo '<span style="clear:both;"></span>';

			}

			echo '					</div>
								</div>';

			// Modo 1 tem o nome dos itens em baixo
			if ($this->modo == PLAY_MODO::PM_ONE) {

				echo '<div style="font-size:14px; font-family: Arial Black;">';
				
				if (!empty($this->itens)) {

					foreach ($this->itens as $item)
						foreach ($item['ITEM'] as $subItem)
							echo $subItem['NAME'].'/'.$subItem['QNTD'].'<br>';
				
				}
				
				echo '</div>';
			
			}
			
			echo '			</td>
						</tr>
					</table>';
		}
	}

	// Página Resultados
	$gacha_result = new GachaResult();

	$gacha_result->show();
?>