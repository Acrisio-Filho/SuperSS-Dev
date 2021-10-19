<?php
    // Arquivo club_member_admin.php
    // Criado em 16/07/2019 as 15:00 por Acrisio
    // Definição e Implementação da classe GuildMemberAdm

    include_once("source/guild_home.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_HOME_URL', BASE_GUILD_URL.'guild_home');

    include_once("../source/table.inc");

    class GuildMemberAdm extends GuildHome {

        private $page = 0;
        private $table = null;

        public function show() {
            
            // Verifica a autoridade do player
            $this->checkAuthority(AUTH_LEVEL_ADMIN);

            $this->checkGetAndPost();

            $this->listMember($this->page);

            $this->begin();

            echo '<title>Guild Member Admin</title>';

            $this->middle();

            Design::menuEdit();

            $this->content();

            $this->end();

        }

        private function checkGetAndPost() {

            // Get
            if (isset($_GET) && isset($_GET['page']))
                $this->page = (is_numeric($_GET['page'])) ? $_GET['page'] : 0;
            else
                $this->page = 0;

            if (!isset($_SESSION))
                session_start();

            if (!isset($_SESSION['MEMBER_ADM']))
                $_SESSION['MEMBER_ADM'] = [];

            // Post
            if (isset($_POST) && !empty($_POST)) {

                if (isset($_SESSION['MEMBER_ADM'])) {

                    if (isset($_SESSION['MEMBER_ADM']['msg']))
                        unset($_SESSION['MEMBER_ADM']['msg']);

                    $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
                    $params = $db->params;

                    // Muda level(authority of member of Guild)
                    if (isset($_POST['ctl01_x']) && isset($_POST['ctl01_y'])) {
                        
                        if (isset($_POST['apply']) && is_numeric($_POST['apply'])) {

                            if ($_POST['apply'] != GuildSingleton::getInstance()['MASTER']['UID']) {
                                
                                if (isset($_POST['level_change']) && is_numeric($_POST['level_change'])) {

                                    // Check soma values
                                    if ($this->isBlocked() || $this->isClosure()) {

                                        $_SESSION['MEMBER_ADM']['msg'] = 'Cannot change member authority because club '.($this->isBlocked() ? ' has been blocked by the GM.' : ' is in the process of closing.');

                                        // Sai da função por que ele não pode trocar autoridade do membro por que o GM bloqueou a guild ou a guild está em processo de fechamento
                                        return;

                                    }

                                    $params->clear();
                                    $params->add('i', GuildSingleton::getInstance()['UID']);
                                    $params->add('i', $_POST['apply']);
                                    $params->add('i', $_POST['level_change']);

                                    if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                        $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGuildChangeAuthorityMember ?, ?, ?';
                                    else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                        $query = 'select "_RET_" as "RET" from '.$db->con_dados['DB_NAME'].'.ProcGuildChangeAuthorityMember(?, ?, ?)';
                                    else
                                        $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGuildChangeAuthorityMember(?, ?, ?)';

                                    if (($result = $db->db->execPreparedStmt($query, $params->get())) == null || $db->db->getLastError() != 0
                                        || ($row = $result->fetch_assoc()) == null || !isset($row['RET']) || $row['RET'] < 0) {
                                        
                                        switch ($row['RET']) {
                                            case -1:
                                                $_SESSION['MEMBER_ADM']['msg'] = 'Failed to change member authority level.';
                                                break;
                                            case -2:
                                                $_SESSION['MEMBER_ADM']['msg'] = 'This member is already '.($_POST['level_change'] == 2 ? 'submaster' : ' a member');
                                                break;
                                        }
                                    
                                    }else
                                        GuildSingleton::updateAllInstance();
                                }
                        
                            }else
                                $_SESSION['MEMBER_ADM']['msg'] = 'You cannot change the authority level of the club master.';

                        }else
                            $_SESSION['MEMBER_ADM']['msg'] = 'Select a member to change their level.';
                    
                    }else if (isset($_POST['ctl02_x']) && isset($_POST['ctl02_y'])) {

                        if (isset($_POST['apply']) && is_numeric($_POST['apply'])) {

                            if ($_POST['apply'] != GuildSingleton::getInstance()['MASTER']['UID']) {

                                // Check soma values
                                if ($this->isBlocked() || $this->isClosure()) {

                                    $_SESSION['MEMBER_ADM']['msg'] = 'Cannot expel member because the club '.($this->isBlocked() ? ' has been blocked by the GM.' : ' is in the process of closing.');

                                    // Sai da função por que ele não pode expulsar membro por que o GM bloqueou a guild ou a guild está em processo de fechamento
                                    return;

                                }

                                $params->clear();
                                $params->add('i', PlayerSingleton::getInstance()['UID']);
                                $params->add('i', $_POST['apply']);
                                $params->add('i', GuildSingleton::getInstance()['UID']);

                                if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                    $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGuildKickMember ?, ?, ?';
                                else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                                    $query = 'select * from '.$db->con_dados['DB_NAME'].'.ProcGuildKickMember(?, ?, ?)';
                                else
                                    $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGuildKickMember(?, ?, ?)';

                                if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0)
                                    $_SESSION['MEMBER_ADM']['msg'] = 'Failed to expel member.';
                                else
                                    GuildSingleton::updateAllInstance();
                            
                            }else
                                $_SESSION['MEMBER_ADM']['msg'] = 'You cannot expel the club master.';

                        }else
                            $_SESSION['MEMBER_ADM']['msg'] = 'Select a member to expel him.';
                    }

                }
            }

        }

        private function content() {

            // Message Java Script
            if (isset($_SESSION['MEMBER_ADM']['msg'])) {
                
                echo '  <script>
                            alert(\''.$_SESSION['MEMBER_ADM']['msg'].'\');
                        </script>';

                // Limpa a msg ela já foi exibida
                unset($_SESSION['MEMBER_ADM']['msg']);
            }

            echo '<form id="ctl00" method="POST" action="./club_member_admin.php">';

            echo '<table class="text_normal" cellspacing="0" cellpadding="0" width="95%" border="0">
                    <tr>
                        <td height="40" vAling="middle" colspan="2">
                            <img src="'.BASE_GUILD_HOME_URL.'/img/title_member_admin.gif">
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
                                                                        <td height="30" width="25%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" height="4" align="absMiddle">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Position</b>
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" width="40%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" height="4" align="absMiddle">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Nickname</b>
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" width="20%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" height="4" align="absMiddle">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Join date</b>
                                                                            </font>
                                                                        </td>
                                                                        <td height="30" width="15%" style="padding: 1px">
                                                                            <img style="display: inline" src="'.BASE_GUILD_HOME_URL.'/img/icon_brown.gif" width="4" height="4" align="absMiddle">
                                                                            <font style="font-size: 12px; color: #644636">
                                                                                <b>Selection</b>
                                                                            </font>
                                                                        </td>
                                                                    </tr>
                                                                    <tr bgColor="#e7dcd4">
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                    </tr>';

            // Table Result
            if ($this->table != null & !$this->table->isEmpty()) {

                foreach ($this->table->getCurrentRow() as $member) {

                    echo '                                          <tr bgColor="#fcfcfc">
                                                                        <td height="20" align="center" style="padding: 1px">
                                                                            '.($this->MEMBER_STATE_LABEL[$member['MEMBER_STATE_FLAG']]).'
                                                                        </td>
                                                                        <td height="20" style="padding: 1px">
                                                                            &nbsp;&nbsp;
                                                                            '.(htmlspecialchars(mb_convert_encoding($member['NICKNAME'], "UTF-8", "SJIS"))).'
                                                                        </td>
                                                                        <td height="20" align="center" style="padding: 1px">
                                                                            ('.($member['REG_DATE']).')
                                                                        </td>
                                                                        <td height="20" align="center" style="padding: 1px">
                                                                            '.($member['MEMBER_UID'] == GuildSingleton::getInstance()['MASTER']['UID'] ? '' : '<input id="apply" name="apply" type="radio" value="'.$member['MEMBER_UID'].'">').'
                                                                        </td>
                                                                    </tr>';
                }

                // Linha Final
                echo '                                              <tr bgColor="#e7dcd4">
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                    </tr>';
            }else {

                // Linha Final
                echo '                                              <tr bgColor="#e7dcd4">
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                        <td height="1" style="padding: 1px"></td>
                                                                    </tr>';

                echo '<tr>
                        <td align="center" colspan="4">
                            has no member to change authority level
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
                                    <a style="display: inline" id="_FirstPage" href="'.BASE_GUILD_HOME_URL.'/club_member_admin.php?page='.($this->table->getFirst()).'">
                                        <img style="display: inline" src="img/bt_pre01.gif" border="0">
                                    </a>
                                    &nbsp;
                                    <a style="display: inline" id="_PrevPage" href="'.BASE_GUILD_HOME_URL.'/club_member_admin.php?page='.($this->table->getPrev()).'">
                                        <img style="display: inline" src="img/bt_pre02.gif" border="0">
                                    </a>
                                </td>
                                <td align="center">
                                    &nbsp;&nbsp;
                                    '.($this->table->makeListLink(BASE_GUILD_HOME_URL.'/club_member_admin.php', '')).'
                                    &nbsp;&nbsp;
                                </td>
                                <td width="50" align="right">
                                    <a style="display: inline" id="_NextPage" href="'.BASE_GUILD_HOME_URL.'/club_member_admin.php?page='.($this->table->getNext()).'">
                                        <img style="display: inline" src="img/bt_next01.gif" border="0">
                                    </a>
                                    &nbsp; 
                                    <a style="display: inline" id="_LastPage" href="'.BASE_GUILD_HOME_URL.'/club_member_admin.php?page='.($this->table->getLast()).'">
                                        <img style="display: inline" src="img/bt_next02.gif" border="0">
                                    </a>
                                </td>
                            </tr>
                        </table>';

            // Fim links de Localização página table
            echo '  </td>
                </tr>';

            // Action
            echo '<tr align="center">
                    <td height="40" vAlign="top" colspan="2" align="right">
                        <select align="absMiddle" id="level_change" name="level_change">
                            <option value="3" selected>Club members</option>
                            <option value="2">Submaster</option>
                        </select>
                        <input style="border: 0px" src="'.BASE_GUILD_HOME_URL.'/img/btn_edit_level.gif" type="image" algin="absMiddle" name="ctl01">
                    </td>
                  </tr>
                  <tr>
                    <td height="10"></td>
                  </tr>
                  <tr>
                    <td align="right">
                        <input style="border: 0px" src="'.BASE_GUILD_HOME_URL.'/img/btn_out.gif" type="image" align="absMiddle" name="ctl02">
                    </td>
                  </tr>';

            echo '</table>';

            echo '</form>';
        }

        private function listMember($page) {

            $members = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', GuildSingleton::getInstance()['UID']);
            $params->add('i', $page);       // Page

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList ?, ?, 2, 10';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_MEMBER_UID_" as "MEMBER_UID", "_MEMBER_STATE_FLAG_" as "MEMBER_STATE_FLAG", "_NICKNAME_" as "NICKNAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList(?, ?, 2, 10)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildMemberList(?, ?, 2, 10)';

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
            $this->table = new table(10, $page, $linhas, $members);

        }
    }

    // Guild Member Admin
    $member_adm = new GuildMemberAdm();

    $member_adm->show();
?>