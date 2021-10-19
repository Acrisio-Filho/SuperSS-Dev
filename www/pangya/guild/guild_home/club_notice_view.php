<?php
    // Arquivo club_notice_view.php
    // Criado em 13/09/2019 as 19:01 por Acrisio
    // Club Notice View

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildNoticeView extends GuildHome {

        private $seq = 0;
        private $page = 0;

        private $notice = null;

        public function show() {

            // Verifica a autoridade do player
            // GM tem acesso a essa página
            if (!(PlayerSingleton::getInstance()['CAP'] & 4/*GM*/))
                $this->checkAuthority(AUTH_LEVEL_REAL_MEMBER);

            $this->checkGet();

            $this->loadNotice($this->seq);

            $this->begin();

            echo '<title>Guild NOTICE View</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkGet() {

            $this->page = (isset($_GET['page']) && is_numeric($_GET['page'])) ? $_GET['page'] : 0;
            $this->seq = (isset($_GET['seq']) && is_numeric($_GET['seq'])) ? $_GET['seq'] : 0;

            // Verifica se a sequência é válida
            if ($this->seq == 0 && !isset($_GET['d'])) {

                header("Location: ".LINKS['GUILD_ERROR']);

                // sai do script para o navegador redirecionar a página
                exit();
            }

            if (!isset($_SESSION))
                session_start();

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            // Delete Notice
            if (isset($_GET['d']) && is_numeric($_GET['d']) && $_GET['d'] > 0) {

                // Delete Notice
                $params->clear();
                $params->add('i', PlayerSingleton::getInstance()['UID']);
                $params->add('i', $_GET['d']);       // seq

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildNotice ?, ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildNotice(?, ?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildNotice(?, ?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {
                    header("Location: ".BASE_GUILD_HOME_URL."/club_notice.php");
                }else
                    echo "<script>javascript:alert('Failed to delete Notice.');</script>";

            }
        }

        private function content() {

            if ($this->notice != null) {

                echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95" border="0">
                        <tr>
                            <td height="45" vAlign="middle" colspan="2">
                                <img style="display: block" src="'.BASE_GUILD_HOME_URL.'/img/title_notice.gif" height="23">
                            </td>
                        </tr>
                        <tr>
                            <td align="center">
                                <span style="overflow: auto; height: 400px; width: 610px">
                                    <table cellspacing="0" cellpadding="0" width="580" border="0">
                                        <tr>
                                            <td>
                                                <table cellspacing="1" cellpadding="0" width="580" bgColor="#fcf8f8" border="0">
                                                    <tr>
                                                        <td bgColor="#b39f8e" height="2" colspan="3"></td>
                                                    </tr>
                                                    <tr class="bbshead">
                                                        <td bgColor="#e7dcd4" height="24" width="580" colspan="3" align="left">
                                                            &nbsp;&nbsp;
                                                            <span style="font-size: 11pt; font-weight: bold; color: #627400">
                                                                '.(htmlspecialchars($this->notice['TITLE'])).'
                                                            </span>
                                                        </td>
                                                    </tr>
                                                    <tr class="bbshead">
                                                        <td bgColor="#e7dcd4" height="24" width="120" align="center">
                                                            <div style="display: inline; font-weight: bold; inline; color: #6b4e17">
                                                                '.(htmlspecialchars(mb_convert_encoding($this->notice['NICKNAME'], "UTF-8", "SJIS"))).'
                                                            </div>
                                                        </td>
                                                        <td bgColor="#e7dcd4" height="24" width="230" align="center">
                                                            <div style="display: inline; color: #6b4e17">
                                                                &nbsp;&nbsp;
                                                                (&nbsp;'.($this->notice['REG_DATE']).'&nbsp;)
                                                            </div>
                                                        </td>
                                                        <td bgColor="#e7dcd4" height="24" width="230" align="left">
                                                            <div style="display: inline; color: #6b4e17">
                                                                &nbsp;&nbsp;Views&nbsp;'.($this->notice['VIEWS']).'
                                                            </div>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td class="bbsline" bgColor="#b39f8e" height="2" colspan="3"></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="bbsmess" width="580" colspan="4">
                                                            <table cellspacing="1" cellpadding="0" width="100%">
                                                                <tr>
                                                                    <td style="word-break: break-all; padding: 4px" bgColor="#ffffff" height="280" vAlign="top" width="580" align="left">
                                                                        '.(htmlspecialchars($this->notice['TEXT'])).'
                                                                    </td>
                                                                </tr>
                                                                <tr>
                                                                    <td style="padding: 4px" align="right">';

                    // Só pode deletar a noticia se for o master ou quem escreveu a noticia
                    if ($this->isMasterOrSubmaster() || PlayerSingleton::getInstance()['UID'] == $this->notice['OWNER_UID']) {

                        echo '                                          <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/club_notice_view.php?d='.($this->notice['SEQ']).'">
                                                                            [&nbsp;Delete&nbsp;]
                                                                        </a>
                                                                        &nbsp;&nbsp;';
                    }

                    echo '                                          </td>
                                                                </tr>
                                                            </table>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td class="bbsline" bgColor="#b39f8e" height="2" colspan="3"></td>
                                                    </tr>
                                                    <tr>
                                                        <td height="20" colspan="3" align="right">
                                                            <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/club_notice.php?page='.$this->page.'">
                                                                [&nbsp;Back to list&nbsp;]
                                                            </a>
                                                            &nbsp;&nbsp;
                                                        </td>
                                                    </tr>
                                                </table>
                                            </td>
                                        </tr>
                                    </table>
                                </span>
                            </td>
                        </tr>';

                echo '</table>';
            
            }else {

                echo "<script>javascript:window.location.href = '".LINKS['GUILD_ERROR']."'</script>";
            }
        }

        private function loadNotice($seq) {

            $notice = [];

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', $seq);       // seq

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildNoticeInfo ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_SEQ_" as "SEQ", "_TITLE_" as "TITLE", "_TEXT_" as "TEXT", "_OWNER_UID_" as "OWNER_UID", "_NICKNAME_" as "NICKNAME", "_VIEWS_" as "VIEWS", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildNoticeInfo(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildNoticeInfo(?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                if ($row = $result->fetch_assoc()) {
                    $notice = $row;
                }

            }

            // Notice
            $this->notice = $notice;

        }
    }

    // Guild Notice View
    $notice_view = new GuildNoticeView();

    $notice_view->show();

?>