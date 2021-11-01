<?php
    // Arquivo Club_New_bbs_view.php
    // Criado em 12/09/2019 as 23:10 por Acrisio
    // Club New BBS View

    include_once("source/guild_new.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_NEW_URL', BASE_GUILD_URL.'guild_new');

    include_once("../source/table.inc");

    class GuildBBSView extends GuildNew {

        private $seq = 0;
        private $page = 0;

        private $bbs_all = [
                            'bbs' => null, 're-bss' => []
                           ];

        public function show() {

            $this->checkGet();

            $this->loadBBS($this->seq);

            $this->begin();

            echo '<title>Guild BBS View</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkGet() {

            $this->page = (isset($_GET['page']) && is_numeric($_GET['page'])) ? $_GET['page'] : 0;
            $this->seq = (isset($_GET['seq']) && is_numeric($_GET['seq'])) ? $_GET['seq'] : 0;

            // Verifica se a sequência do BBS é válida
            if ($this->seq == 0 && !isset($_GET['dp']) && !isset($_GET['dc'])) {

                header("Location: ".LINKS['UNKNOWN_ERROR']);

                // sai do script para o navegador redirecionar a página
                exit();
            }

            if (!isset($_SESSION))
                session_start();

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            // Delete BBS Or Comment
            if (isset($_GET['dp']) && is_numeric($_GET['dp']) && $_GET['dp'] > 0) {

                // Delete BBS
                $params->clear();
                $params->add('i', PlayerSingleton::getInstance()['UID']);
                $params->add('i', $_GET['dp']);       // seq

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildBBS ?, ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildBBS(?, ?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildBBS(?, ?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {
                    header("Location: ".BASE_GUILD_NEW_URL."/Club_New_bbs_list.php");
                }else
                    echo "<script>javascript:alert('Failed to delete BBS.');</script>";

            }else if (isset($_GET['dc']) && is_numeric($_GET['dc']) && $_GET['dc'] > 0) {

                // Delete BBS Comment
                $params->clear();
                $params->add('i', PlayerSingleton::getInstance()['UID']);
                $params->add('i', $_GET['dc']);       // seq

                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildBBSReply ?, ?';
                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                    $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildBBSReply(?, ?)';
                else
                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcDeleteGuildBBSReply(?, ?)';

                if (($result = $db->db->execPreparedStmt($query, $params->get())) == null || $db->db->getLastError() != 0)
                    echo "<script>javascript:alert('Failed to delete BBS Comment.');</script>";
            }
        }

        private function content() {

            if ($this->bbs_all['bbs'] != null && !empty($this->bbs_all['bbs'])) {

                echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95" border="0">
                        <tr>
                            <td height="45" vAlign="middle" colspan="2">
                                <img style="display: block" src="'.BASE_GUILD_NEW_URL.'/img/title_guild_bbs.gif" height="23">
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
                                                        <td vAlign="middle" align="center" height="24" width="120" bgColor="#e7dcd4">
                                                            <img align="absMiddle" border="0" src="'.BASE_GUILD_NEW_URL.'/img/bbs_icon'.str_pad($this->bbs_all['bbs']['TYPE'], 2, '0', STR_PAD_LEFT).'.gif">
                                                        </td>
                                                        <td style="padding: 2px 3px" bgColor="#e7dcd4" height="24" width="510" colspan="2" align="left">
                                                            &nbsp;&nbsp;
                                                            <span style="font-size: 11pt; font-weight: bold; color: #627400">
                                                                '.(htmlspecialchars($this->bbs_all['bbs']['TITLE'])).'
                                                            </span>
                                                        </td>
                                                    </tr>
                                                    <tr class="bbshead">
                                                        <td bgColor="#e7dcd4" height="24" width="120" align="center">
                                                            <div style="display: inline; font-weight: bold; inline; color: #6b4e17">
                                                                '.(htmlspecialchars(mb_convert_encoding($this->bbs_all['bbs']['NICKNAME'], "UTF-8", "SJIS"))).'
                                                            </div>
                                                        </td>
                                                        <td bgColor="#e7dcd4" height="24" width="230" align="center">
                                                            <div style="display: inline; color: #6b4e17">
                                                                &nbsp;&nbsp;
                                                                (&nbsp;'.($this->bbs_all['bbs']['REG_DATE']).'&nbsp;)
                                                            </div>
                                                        </td>
                                                        <td bgColor="#e7dcd4" height="24" width="230" align="left">
                                                            <div style="display: inline; color: #6b4e17">
                                                                &nbsp;&nbsp;Views&nbsp;'.($this->bbs_all['bbs']['VIEWS']).'
                                                            </div>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td class="bbsline" bgColor="#b39f8e" height="2" colspan="3"></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="bbsmess" width="580" colspan="4">
                                                            <table width="100%">
                                                                <tr>
                                                                    <td style="word-break: break-all; padding: 2px 3px" bgColor="#ffffff" vAlign="top" width="580" align="left">
                                                                        '.(htmlspecialchars($this->bbs_all['bbs']['TEXT'])).'
                                                                    </td>
                                                                </tr>
                                                                <tr>
                                                                    <td align="right">';

                    // Só pode deletar o BBS quem escreveu
                    if (PlayerSingleton::getInstance()['UID'] == $this->bbs_all['bbs']['OWNER_UID']) {

                        echo '                                          <a style="display: inline" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_view.php?dp='.($this->bbs_all['bbs']['SEQ']).'">
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
                                                            <a style="display: inline" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_list.php?page='.$this->page.'">
                                                                [&nbsp;Back to list&nbsp;]
                                                            </a>';

                    // Verifica se o player tem o level necessário para comentar no BBS de outro player
                    if (PlayerSingleton::getInstance()['LEVEL'] >= 1/*Rookie E*/) {
                        
                        echo '                              &nbsp;&nbsp;&nbsp;
                                                            <a style="display: inline" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_write.php?seq='.($this->bbs_all['bbs']['SEQ']).'">
                                                                [&nbsp;Reply to BBS&nbsp;]
                                                            </a>';
                    }

                    echo '                                  &nbsp;&nbsp;
                                                        </td>
                                                    </tr>
                                                </table>';

                // BBS Reply
                if (!empty($this->bbs_all['re-bbs'])) {
                    
                    foreach ($this->bbs_all['re-bbs'] as $index => $rbbs) {
                   
                        echo '                  <br>
                                                <span id="_ResBlind'.($index + 1).'"></span>
                                                <table cellspacing="0" cellpadding="0" width="580" bgColor="#f0f0f0">
                                                    <tr>
                                                        <td style="padding: 2px">
                                                            <table cellspacing="2" cellpadding="0" width="100%" bgColor="#ffffff">
                                                                <tr>
                                                                    <td>
                                                                        <table cellspacing="0" cellpadding="0" width="100%" align="center" border="0">
                                                                            <tr bgColor="#f0f0f0">
                                                                                <td style="font-size: 11pt; color: #666666; padding: 0 0 0 10px" width="60%" align="left">
                                                                                    '.(htmlspecialchars(mb_convert_encoding($rbbs['NICKNAME'], "UTF-8", "SJIS"))).'
                                                                                </td>
                                                                                <td style="font-size: 9pt; color: #627400; padding: 0 10px 0 0" vAlign="bottom" with="40%" align="right">
                                                                                    ('.($rbbs['REG_DATE']).')
                                                                                </td>
                                                                            </tr>
                                                                            <tr>
                                                                                <td style="word-break: break-all; padding: 2px 3px" vAlign="top" width="100%" colspan="2">
                                                                                    '.(htmlspecialchars($rbbs['TEXT'])).'
                                                                                </td>
                                                                            </tr>
                                                                            <tr>
                                                                                <td colspan="2" align="right">';

                        // Só pode deletar o BBS Reply que escreveu
                        if (PlayerSingleton::getInstance()['UID'] == $rbbs['OWNER_UID']) {

                            echo '                                                  <a style="display: inline" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_view.php?seq='.($this->bbs_all['bbs']['SEQ']).'&dc='.($rbbs['SEQ']).'">
                                                                                        [&nbsp;Delete&nbsp;]
                                                                                    </a>
                                                                                    &nbsp;&nbsp;';
                        }

                        echo '                                                  </td>
                                                                            </tr>
                                                                        </table>
                                                                        <br>
                                                                    </td>
                                                                </tr>
                                                            </table>
                                                        </td>
                                                    </tr>
                                                </table>';
                    }
                }

                echo '                      </td>
                                        </tr>
                                    </table>
                                </span>
                            </td>
                        </tr>';

                echo '</table>';
            
            }else {

                echo "<script>javascript:window.location.href = '".LINKS['UNKNOWN_ERROR']."'</script>";
            }
        }

        private function loadBBS($seq) {

            $bbs = [];
            $re_bbs = [];

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', $seq);       // seq

            // BBS
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildBBSInfo ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_SEQ_" as "SEQ", "_TITLE_" as "TITLE", "_TEXT_" as "TEXT", "_TYPE_" as "TYPE", "_OWNER_UID_" as "OWNER_UID", "_NICKNAME_" as "NICKNAME", "_VIEWS_" as "VIEWS", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildBBSInfo(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildBBSInfo(?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                if ($row = $result->fetch_assoc()) {
                    $bbs = $row;
                }

            }

            // BSS Reply
            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildBBSReplyInfo ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_SEQ_" as "SEQ", "_TEXT_" as "TEXT", "_OWNER_UID_" as "OWNER_UID", "_NICKNAME_" as "NICKNAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildBBSReplyInfo(?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildBBSReplyInfo(?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                while (($row = $result->fetch_assoc())) {
                    $re_bbs[] = $row;
                }
            }

            // BBS
            $this->bbs_all['bbs'] = $bbs;
            $this->bbs_all['re-bbs'] = $re_bbs;

        }
    }

    // Guild BBS View
    $bbs_view = new GuildBBSView();

    $bbs_view->show();
?>