<?php
    // Arquivo index.php
    // Criado em 16/07/2019 as 12:09 por Acrisio
    // Definição e Implementação da classe Index

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once("../source/check_img_size.inc");

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    class Index extends GuildHome {

        public function show() {

            $this->load_info();

            $this->begin();

            echo '<title>Guild Home Index</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function load_info() {

            // Error
            if (GuildSingleton::getInstanceFullInfo() == null || !GuildSingleton::getInstanceFullInfo()['exists']) {
                
                // redireciona para a página de erro
                header("Location: ".LINKS['GUILD_ERROR']);

                // sai do script para o navegador redirecionar a página
                exit();
            }
        }

        private function content() {

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" border="0" width="95%">
                    <tr>
                        <td height="45" vAlign="middle">
                            <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_main.gif" height="23">
                        </td>
                    </tr>
                    <tr>
                        <td vAlign="top" align="center">
                            <table cellspacing="0" width="580" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" style="padding: 5px">
                                        <table cellspacing="0" width="570" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table cellspacing="0" cellpadding="0" width="568" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td vAlign="top" align="center">
                                                                <table class="text_normal" cellspacing="0" width="568" border="0">
                                                                    <tr align="left" bgColor="#e7dcd4">
                                                                        <td height="30" colspan="3" style="padding: 2px">
                                                                            <b>
                                                                                &nbsp;&nbsp;
                                                                                <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" align="absMiddle" height="4">
                                                                                <font style="font-size: 12px; color: #644636">
                                                                                    Detailed information
                                                                                </font>
                                                                            </b>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                        <td bgColor="#fbf1e6" height="30" width="83" style="padding: 2px">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Mark
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" width="143" align="left" style="padding: 2px">
                                                                            &nbsp;&nbsp;
                                                                            <img style="display: inline" src="'.((GuildSingleton::getInstanceFullInfo()['MARK_IDX'] <= 0) ? BASE_GUILD_UPLOAD_URL.'/mark/sample.png' : BASE_GUILD_UPLOAD_URL.'/mark/'.(GuildSingleton::getInstanceFullInfo()['UID']).'_'.(GuildSingleton::getInstanceFullInfo()['MARK_IDX']).'.png').'" width="22" height="20">
                                                                        </td>
                                                                        <td height="250" width="330" align="center" vAlign="middle" rowspan="15" style="padding: 2px">
                                                                            <table cellspacing="0" bgColor="#fbf1e6" border="0">
                                                                                <tr>
                                                                                    <td style="padding: 5px">
                                                                                        <table cellspacing="0" width="100%" border="0">
                                                                                            <tr>
                                                                                                <td bgColor="#b39f8e" style="padding: 1px">
                                                                                                    <table class="text_normal" cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                                                                        <tr>
                                                                                                            <td align="center">
                                                                                                                <img '.ValidImgSize::checkImgSize(BASE_GUILD_UPLOAD_URL.'/intro', $_SERVER['DOCUMENT_ROOT'].'_Files/guild/intro/', (GuildSingleton::getInstanceFullInfo()['INTRO_IMG'] == '' ? 'sample.jpg' : GuildSingleton::getInstanceFullInfo()['INTRO_IMG'])).'"> <!-- uid-anomesdia-horamin.jpg -->
                                                                                                            </td>
                                                                                                        </tr>
                                                                                                    </table>
                                                                                                </td>
                                                                                            </tr>
                                                                                        </table>
                                                                                    </td>
                                                                                </tr>
                                                                            </table>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1"></td>
                                                                        <td height="1" align="left"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                        <td bgColor="#fbf1e6" height="30">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Name
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" align="left">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.(htmlspecialchars(GuildSingleton::getInstanceFullInfo()['NAME'])).'
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1"></td>
                                                                        <td height="1" align="left"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                         <td bgColor="#fbf1e6" height="30">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Master
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" align="left">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.(htmlspecialchars(GuildSingleton::getInstanceFullInfo()['MASTER'])).'
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1"></td>
                                                                        <td height="1" align="left"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                         <td bgColor="#fbf1e6" height="30">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Submaster
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" align="left">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.(htmlspecialchars(GuildSingleton::getInstanceFullInfo()['SUB_MASTER'])).'
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1"></td>
                                                                        <td height="1" align="left"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                         <td bgColor="#fbf1e6" height="30">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Members
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" align="left">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.(GuildSingleton::getInstanceFullInfo()['MEMBERS']).'
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1"></td>
                                                                        <td height="1" align="left"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                         <td bgColor="#fbf1e6" height="30">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                State
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" align="left">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.($GLOBALS['STATE_LABEL'][GuildSingleton::getInstanceFullInfo()['STATE']]).'
                                                                            </font>
                                                                            
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1"></td>
                                                                        <td height="1" align="left"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                         <td bgColor="#fbf1e6" height="30">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Condition
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" align="left">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.(((GuildSingleton::getInstance()['STATE'] == ST_APPROVAL_PEDNDING || GuildSingleton::getInstance()['STATE'] == ST_NOW_OPEN) 
                                                                                    && GuildSingleton::getInstance()['PERMITION_JOIN'] == 0)
                                                                                        ? 'Closed to join'
                                                                                        : $GLOBALS['CONDITION_LEVEL_LABEL'][GuildSingleton::getInstanceFullInfo()['CONDITION_LEVEL']]).'
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1" colspan="2"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                         <td bgColor="#fbf1e6" height="30">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Creation
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" align="left" colspan="2">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.(GuildSingleton::getInstanceFullInfo()['CREATE']).'
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1" colspan="3"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                         <td bgColor="#fbf1e6" height="40">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Welcome
                                                                            </font>
                                                                        </td>
                                                                        <td height="40" align="left" colspan="2">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.(htmlspecialchars(GuildSingleton::getInstanceFullInfo()['MESSAGE_INTRO'])).'
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td bgColor="#fbf1e6" height="1" colspan="3"></td>
                                                                    </tr>
                                                                    <tr align="center">
                                                                         <td bgColor="#fbf1e6" height="40">
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                Master Msg
                                                                            </font>
                                                                        </td>
                                                                        <td height="40" align="left" colspan="2">
                                                                            &nbsp;&nbsp;
                                                                            <font style="font-size: 12px; color: #333333">
                                                                                '.(htmlspecialchars(GuildSingleton::getInstanceFullInfo()['MASTER_COMMENT'])).'
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                </table>
                                                            </td>
                                                        </tr>
                                                    </table>
                                                </td>
                                            </tr>
                                        </table>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>';


            echo '</table>';
        }
    }

    // Guild Home Index
    $index = new Index();

    $index->show();
?>