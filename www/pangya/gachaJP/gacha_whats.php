<?php
    // Arquivo gacha_whats.php
    // Criado em 23/05/2020 as 18:36 por Acrisio
    // Página index do Sistema de Gacha

    include_once("source/gacha_system.inc");

    class GachaWhats extends GachaSystem {

        public function show() {

            $this->begin();

            echo '<title>Gacha Whats</title>';

            $this->middle();

            $this->content();

            $this->end();
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
            
                    img {
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
                </style>';
        }

        protected function content() {

            echo '<div style="position:absolute; left:190px; top:100px; width:610px; height:450px;">
                    <img src="img/gacha_top.png" width="610" height="450" alt="" />
                </div>';
        }
    }

    // Gacha Whats index
    $gacha_whats = new GachaWhats();

    $gacha_whats->show();
?>