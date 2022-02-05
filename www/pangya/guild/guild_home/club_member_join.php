<?php
    // Arquivo club_member_join.php
    // Criado em 16/07/2019 as 14:59 por Acrisio
    // Definição e Implementação da classe GuildMemberJoin

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildMemberJoin extends GuildHome {

        private $page = 0;
        private $table = null;

        public function show() {

            // Verifica a autoridade do player
            $this->checkAuthority(AUTH_LEVEL_SUBMASTER);

            $this->checkGetAndPost();

            $this->listMemberJoin($this->page);

            $this->begin();

            echo '<title>Guild Member Join</title>';

            $this->middle();

            Design::menuEdit();

            $this->content();

            $this->end();

        }

        private function checkGetAndPost() {

            // Get
            if (isset($_GET) && !empty($_GET))
                $this->page = (isset($_GET['page']) && is_numeric($_GET['page'])) ? $_GET['page'] : 0;

            if (!isset($_SESSION))
                session_start();

            if (!isset($_SESSION['MEMBER_JOIN']))
                $_SESSION['MEMBER_JOIN'] = [];

            if (isset($_POST) && !empty($_POST)) {

                if (isset($_SESSION['MEMBER_JOIN'])) {

                    if (isset($_SESSION['MEMBER_JOIN']['msg']))
                        unset($_SESSION['MEMBER_JOIN']['msg']);

                    if (isset($_POST['apply']) && is_numeric($_POST['apply'])) {

                        $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                        $params = $db->params;
                        
                        if (isset($_POST['ctl01_x']) && isset($_POST['ctl01_y'])) {

                            // Check soma values
                            if ($this->isBlocked() || $this->isClosure()) {

                                $_SESSION['MEMBER_JOIN']['msg'] = 'Cannot accept new member because club '.($this->isBlocked() ? ' has been blocked by GM.' : ' is in the process of closing.');

                                // Sai da função por que ele não pode aceitar novo membro por que o GM bloqueou a guild ou a guild está em processo de fechamento
                                return;

                            }

                            // Aceita player como membro
                            $params->clear();
                            $params->add('i', PlayerSingleton::getInstance()['UID']);   // Quem Aceitou o membro no Club
                            $params->add('i', $_POST['apply']);
                            $params->add('i', GuildSingleton::getInstance()['UID']);

                            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGuildAceitaPedido ?, ?, ?';
                            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcGuildAceitaPedido(?, ?, ?)';
                            else
                                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGuildAceitaPedido(?, ?, ?)';

                            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null || $db->db->getLastError() != 0)
                                $_SESSION['MEMBER_JOIN']['msg'] = 'Failed to accept player in club.';
                            else
                                GuildSingleton::updateAllInstance();

                        }else if (isset($_POST['ctl02_y']) && isset($_POST['ctl02_y'])) {

                            // Check soma values
                            if ($this->isBlocked() || $this->isClosure()) {

                                $_SESSION['MEMBER_JOIN']['msg'] = 'Cannot refuse request because club '.($this->isBlocked() ? ' has been blocked by GM.' : ' is in the process of closing.');

                                // Sai da função por que ele não pode recusar solicitação por que o GM bloqueou a guild ou a guild está em processo de fechamento
                                return;

                            }

                            // Recusa player
                            $params->clear();
                            $params->add('i', PlayerSingleton::getInstance()['UID']);   // Quem Chutou o membro do Club
                            $params->add('i', $_POST['apply']);
                            $params->add('i', GuildSingleton::getInstance()['UID']);

                            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGuildKickMember ?, ?, ?';
                            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcGuildKickMember(?, ?, ?)';
                            else
                                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGuildKickMember(?, ?, ?)';

                            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null || $db->db->getLastError() != 0)
                                $_SESSION['MEMBER_JOIN']['msg'] = 'Failed to cancel player\'s club order.';
                            else
                                GuildSingleton::updateAllInstance();

                        }
                    
                    }else
                        $_SESSION['MEMBER_JOIN']['msg'] = 'Please select a member.';

                }
            }
            
        }

        private function content() {

            // Message Java Script
            if (isset($_SESSION['MEMBER_JOIN']['msg'])) {
                
                echo '  <script>
                            alert(\''.$_SESSION['MEMBER_JOIN']['msg'].'\');
                        </script>';

                // Limpa a msg ela já foi exibida
                unset($_SESSION['MEMBER_JOIN']['msg']);
            }

            echo '<form id="ctl00" method="POST" action="./club_member_join.php">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="40" vAlign="middle" colspan="2">
                            <img src="'.BASE_GUILD_HOME_URL.'/img/title_member_join.gif">
                        </td>
                    </tr>
                    <tr align="center">
                        <td vAlign="top" colspan="2">
                            <table cellspacing="0" cellpadding="0" width="580" bgColor="#fbf1e6" border="0">
                                <tr>
                                    <td vAlign="top" style="padding: 5px">
                                        <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                            <tr>
                                                <td bgColor="#b39f8e" vAlign="top" style="padding: 1px">
                                                    <table cellspacing="0" cellpadding="0" width="100%" bgColor="#ffffff" border="0">
                                                        <tr>
                                                            <td vAlign="top" align="center">
                                                                <table class="text_normal" cellspacing="1" cellpadding="0" width="100%" border="0">
                                                                    <tr align="center" bgColor="#e7dcd4">
                                                                        <td height="30" width="50%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" height="4" aling="absMiddle">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Nickname</b>
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" width="30%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" height="4" align="absMiddle">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Application date</b>
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" width="20%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" height="4" align="absMiddle">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Selection</b>
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr bgColor="#e7dcd4">
                                                                        <td heigh="1" width="50%" style="padding: 1px"></td>
                                                                        <td heigh="1" width="30%" style="padding: 1px"></td>
                                                                        <td heigh="1" width="20%" style="padding: 1px"></td>
                                                                    </tr>';

            // Table Result
            if ($this->table != null && !$this->table->isEmpty()) {
                
                foreach ($this->table->getCurrentRow() as $member) {

                    echo '                                          <tr bgColor="#fcfcfc">
                                                                        <td height="18" width="50%" style="padding: 1px" align="left">
                                                                            &nbsp;
                                                                            '.htmlspecialchars(mb_convert_encoding($member['NICKNAME'], "UTF-8", "SJIS")).'
                                                                        </td>
                                                                        <td height="18" width="30%" style="padding: 1px" align="center">
                                                                            ('.$member['REG_DATE'].')
                                                                        </td>
                                                                        <td height="18" width="20%" style="padding: 1px" align="center">
                                                                            <input id="apply" value="'.$member['MEMBER_UID'].'" type="radio" name="apply" style="border: 0px; height: 15px">
                                                                        </td>
                                                                    </tr>';

                }
                
                // Linha final
                echo '                                              <tr bgColor="#e7dcd4">
                                                                        <td heigh="1" width="50%" style="padding: 1px"></td>
                                                                        <td heigh="1" width="30%" style="padding: 1px"></td>
                                                                        <td heigh="1" width="20%" style="padding: 1px"></td>
                                                                    </tr>';
            }else {
                
                // Linha final
                echo '                                              <tr bgColor="#e7dcd4">
                                                                        <td heigh="1" width="50%" style="padding: 1px"></td>
                                                                        <td heigh="1" width="30%" style="padding: 1px"></td>
                                                                        <td heigh="1" width="20%" style="padding: 1px"></td>
                                                                    </tr>';

                // Message
                echo '                                              <tr>
                                                                        <td colspan="3" align="center" style="padding: 1px">
                                                                            <span id="_msg">
                                                                                There is no member information currently applied for.
                                                                            </span>
                                                                        </td>
                                                                    </tr>';
            }

            echo '                                              </table>
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


            // Links de Localização página table
            echo '<tr>
                    <td align="center">';

            echo '      <table height="40" align="center">
                            <tr>
                                <td width="50" align="left">
                                    <a style="display: inline" id="_FirstPage" href="'.BASE_GUILD_HOME_URL.'/club_member_join.php?page='.($this->table->getFirst()).'">
                                        <img style="display: inline" src="img/bt_pre01.gif" border="0">
                                    </a>
                                    &nbsp;
                                    <a style="display: inline" id="_PrevPage" href="'.BASE_GUILD_HOME_URL.'/club_member_join.php?page='.($this->table->getPrev()).'">
                                        <img style="display: inline" src="img/bt_pre02.gif" border="0">
                                    </a>
                                </td>
                                <td align="center">
                                    &nbsp;&nbsp;
                                    '.($this->table->makeListLink(BASE_GUILD_HOME_URL.'/club_member_join.php', '')).'
                                    &nbsp;&nbsp;
                                </td>
                                <td width="50" align="right">
                                    <a style="display: inline" id="_NextPage" href="'.BASE_GUILD_HOME_URL.'/club_member_join.php?page='.($this->table->getNext()).'">
                                        <img style="display: inline" src="img/bt_next01.gif" border="0">
                                    </a>
                                    &nbsp; 
                                    <a style="display: inline" id="_LastPage" href="'.BASE_GUILD_HOME_URL.'/club_member_join.php?page='.($this->table->getLast()).'">
                                        <img style="display: inline" src="img/bt_next02.gif" border="0">
                                    </a>
                                </td>
                            </tr>
                        </table>';

            // Fim links de Localização página table
            echo '  </td>
                </tr>';

            // Action
            echo '<tr>
                    <td height="10"></td>
                  </tr>
                  <tr algin="center">
                    <td height="40" vAlign="top" colspan="2" align="right">
                        <input style="border: 0px" src="'.BASE_GUILD_HOME_URL.'/img/member_ok.gif" type="image" align="absMiddle" name="ctl01">
                        &nbsp;
                        <input style="border: 0px" src="'.BASE_GUILD_HOME_URL.'/img/member_no.gif" type="image" align="absMiddle" name="ctl02">
                    </td>
                  </tr>';

            echo '</table>';

            echo '</form>';
        }

        private function listMemberJoin($page) {

            $members = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', GuildSingleton::getInstance()['UID']);
            $params->add('i', $page);       // Page

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList ?, ?, 1, 14';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_MEMBER_UID_" as "MEMBER_UID", "_MEMBER_STATE_FLAG_" as "MEMBER_STATE_FLAG", "_NICKNAME_" as "NICKNAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList(?, ?, 1, 14)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList(?, ?, 1, 14)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS'])) {

                $linhas = $row['LINHAS'];

                do {

                    if (isset($row['MEMBER_UID']) && isset($row['MEMBER_STATE_FLAG']) 
                        && isset($row['NICKNAME']) && isset($row['REG_DATE'])) {

                        // Members line table
                        $members[] = [
                            'MEMBER_UID' => $row['MEMBER_UID'],
                            'MEMBER_STATE_FLAG' => $row['MEMBER_STATE_FLAG'],
                            'NICKNAME' => $row['NICKNAME'],
                            'REG_DATE' => $row['REG_DATE']
                        ];
                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(14, $page, $linhas, $members);
        }
    }

    // Guild Member Join
    $member_join = new GuildMemberJoin();

    $member_join->show();
?>
