<?php
    // Arquivo Club_New_list_all.php
    // Criado em 16/07/2019 as 10:13 por Acrisio
    // Dinifição e Implementação da classe GuildListAll

    include_once("source/guild_new.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once("../source/table.inc");

    class GuildListAll extends GuildNew {

        private $table = null;

        public function show() {

            $this->checkPost();

            $this->listGuild($_SESSION['LIST_ALL']['VIEWSTATE']['page'], 
                             $_SESSION['LIST_ALL']['VIEWSTATE']['category_select'], 
                             $_SESSION['LIST_ALL']['VIEWSTATE']['search'], 
                             ($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort']) == '-' 
                                ? 6/*REG_DATE*/ 
                                : $_SESSION['LIST_ALL']['VIEWSTATE']['category_sort']);

            $this->begin();

            echo '<title>Guild List All</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            //print_r($_POST);
            //print_r($_GET);

            if (!isset($_SESSION))
                session_start();

            // Cria session LIST_ALL
            if (!isset($_SESSION['LIST_ALL']))
                $_SESSION['LIST_ALL'] = [];
            else if (empty($_GET)) {
                
                // Limpa SESSION LIST_ALL
                unset($_SESSION['LIST_ALL']);
                
                $_SESSION['LIST_ALL'] = [];
            }

            if (!empty($_POST)) {
                // Init and Update VIEWSATE
                $_SESSION['LIST_ALL']['VIEWSTATE'] = [
                    'page' => 0,
                    'category_sort' => isset($_POST['category_sort']) && $_POST['category_sort'] != '' && is_numeric($_POST['category_sort']) ? $_POST['category_sort'] : '-',
                    'category_select' => isset($_POST['category_select']) && $_POST['category_select'] != '' && is_numeric($_POST['category_select']) ? $_POST['category_select'] : 0,
                    'search' => isset($_POST['search']) ? strip_tags($_POST['search']) : ''
                ];

            }else if (!empty($_GET)) {

                $_SESSION['LIST_ALL']['VIEWSTATE'] = [
                    'page' => isset($_GET['page']) && $_GET['page'] != '' && is_numeric($_GET['page']) ? $_GET['page'] : 0,
                    'category_sort' => isset($_GET['sort']) && $_GET['sort'] != '' && is_numeric($_GET['sort']) 
                            ? $_GET['sort'] 
                            : $_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] ?? '-',
                    'category_select' => isset($_GET['category']) && $_GET['category'] != '' && is_numeric($_GET['category'])
                            ? $_GET['category'] 
                            : $_SESSION['LIST_ALL']['VIEWSTATE']['category_select'] ?? 0,
                    'search' => isset($_GET['search']) 
                            ? strip_tags($_GET['search']) 
                            : $_SESSION['LIST_ALL']['VIEWSTATE']['search'] ?? ''
                ];                
            
            }else if (!isset($_SESSION['LIST_ALL']['VIEWSTATE'])) {

                $_SESSION['LIST_ALL']['VIEWSTATE'] = [
                    'page' => 0,
                    'category_sort' => '-',
                    'category_select' => 0,
                    'search' => ''
                ];
            }
        }

        private function content() {

            echo '<form id="ctl00" method="post" action="./Club_New_list_all.php">';

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';

            /* Table Column Heads ( Name, 
                                    Master"nickname", 
                                    Members"Numbers", 
                                    Level condition"none, beginner, junior, senior, amateur", 
                                    State"waiting approv, opened, closed",
                                    Flag"Desativado",
                                    Create Date
                                   ) 
            */

            // Linha do title
            echo '<tr>';

            // Title
            echo '  <td height="40" vAlign="top" width="150">
                        <img src="img/title02.gif">
                    </td>';

            echo '  <td vAlign="middle" width="465" align="right">
                        <select id="category_sort" name="category_sort" style="border: rgb(110, 90, 35) 1px solid; background-color: rgb(252, 249, 232)">
                            <option value="-" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] == '-') ? 'selected' : '').'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;---------</option>
                            <option value="0" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] == '0') ? 'selected' : '').'>Club name</option>
                            <option value="1" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] == '1') ? 'selected' : '').'>Club master</option>
                            <option value="2" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] == '2') ? 'selected' : '').'>Club members</option>
                            <option value="3" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] == '3') ? 'selected' : '').'>Club Conditions</option>
                            <option value="4" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] == '4') ? 'selected' : '').'>Club State</option>
                            <!-- Desativado <option value="5">Número de bandeiras</option> -->
                            <option value="6" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] == '6') ? 'selected' : '').'>Club create date</option>
                        </select>
                        <input width="100" name="ctl01" type="submit" value="Order" style="width: 25%; border: rgb(110, 90, 35) 1px solid; background-color: rgb(252, 249, 232)">
                    </td>';

            echo '</tr>';

            // Conteúdo da tabela
            echo '<tr>';

            echo '  <td height="86" colspan="2">
                        <table width="609" cellspacing="0" cellpadding="0" border="0" align="center">
                            <tr>
                                <td height="30">
                                    <a href="'.LINKS['MNL_LIST_ALL'].'?sort=0">
                                        <img src="img/list_bbs01.gif" width="153" height="33" border="0">
                                    </a>
                                </td>
                                <td height="30">
                                    <a href="'.LINKS['MNL_LIST_ALL'].'?sort=1">
                                        <img src="img/list_bbs02.gif" width="100" height="33" border="0">
                                    </a>
                                </td>
                                <td height="30">
                                    <a href="'.LINKS['MNL_LIST_ALL'].'?sort=2">
                                        <img src="img/list_bbs03.gif" width="69" height="33" border="0">
                                    </a>
                                </td>
                                <td height="30">
                                    <a href="'.LINKS['MNL_LIST_ALL'].'?sort=3">
                                        <img src="img/list_bbs04.gif" width="71" height="33" border="0">
                                    </a>
                                </td>
                                <td height="30">
                                    <a href="'.LINKS['MNL_LIST_ALL'].'?sort=4">
                                        <img src="img/list_bbs05.gif" width="67" height="33" border="0">
                                    </a>
                                </td>
                                <td height="30">
                                    <!-- Desativado <a href="'.LINKS['MNL_LIST_ALL'].'?sort=5"> -->
                                    <img src="img/list_bbs06.gif" width="70" height="33" border="0">
                                    <!-- </a> -->
                                </td>
                                <td height="30">
                                    <a href="'.LINKS['MNL_LIST_ALL'].'?sort=6">
                                        <img src="img/list_bbs07.gif" width="79" height="33" border="0">
                                    </a>
                                </td>
                            </tr>
                            <tr>
                                <td height="2" colspan="7"></td>
                            </tr>';

            // Linhas
            echo '          <tr bgColor="#f2eccd">
                                <td colspan="7">
                                    <table class="table_listgn" cellspacing="1" cellpadding="1" width="609" border="0">';
            
            //<!-- Linhas Result -->
            if (!$this->table->isEmpty()) {

                foreach ($this->table->getCurrentRow() as $guild) {

                    echo '<tr bgColor="#ffffffff">';
                    
                    echo '  <td height="25" width="154">
                                &nbsp;
                                <img src="'.(($guild['GUILD_MARK_IMG_IDX'] <= 0) ? BASE_GUILD_UPLOAD_URL.'/mark/sample' : BASE_GUILD_UPLOAD_URL.'/mark/'.$guild['GUILD_UID'].'_'.$guild['GUILD_MARK_IMG_IDX']).'.png" style="display: inline" width="22" height="20">
                                &nbsp;
                                <a href="'.LINKS['MNL_GUILD_HOME'].'?id='.$guild['GUILD_UID'].'" style="display: inline;">
                                    '.htmlspecialchars(mb_convert_encoding($guild['GUILD_NAME'], "UTF-8", "SJIS")).'
                                </a>
                            </td>';

                    echo '  <td height="25" width="100" align="center">
                                '.htmlspecialchars(mb_convert_encoding($guild['GUILD_MASTER'], "UTF-8", "SJIS")).'
                            </td>
                            <td height="25" width="69" align="center">
                                '.$guild['MEMBERS'].'
                            </td>
                            <td height="25" width="71" align="center">
                                <!-- Guild Conditions -->
                                <img src="img/list_level'.$GLOBALS['CONDITION_LEVEL_IMG_IDX'][$guild['GUILD_CONDITION_LEVEL']].'.gif">
                            </td>
                            <td height="25" width="67" align="center">
                                '.$GLOBALS['STATE_LABEL'][$guild['GUILD_STATE']].'
                            </td>
                            <td height="25" width="70" align="center">
                                <!-- Número de bandeiras -->
                                '.($guild['GUILD_FLAG'] == 0 ? '-' : $guild['GUILD_FLAG']).'
                            </td>
                            <td height="25" width="78" align="center">
                                ('.$guild['GUILD_REG_DATE'].')
                            </td>';

                    echo '</tr>';
                }
            
            }else {

                echo '  <tr bgColor="#ffffffff">
                    	    <td height="25" align="center">no clubs found</td>
                        </tr>';
            }
            
            // Fecha Tabela Result
            echo '                  </table>
                                </td>
                            </tr>';

            // Pesquisa
            echo '          <tr>
                                <td height="2" colspan="7"></td>
                            </tr>
                            <tr align="right" bgColor="#fcf9e8">
                                <td height="30" colspan="7">
                                    <select id="category_select" name="category_select" style="border: rgb(110, 90, 35) 1px solid; background-color: rgb(254, 252, 252)">
                                        <option value="0" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_select'] == 0) ? 'selected' : '').'>Club name</option>
                                        <option value="1" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category_select'] == 1) ? 'selected' : '').'>Club master</option>
                                    </select>
                                    &nbsp; 
                                    <input id="search" name="search" type="text" value="'.$_SESSION['LIST_ALL']['VIEWSTATE']['search'].'" maxLength="20" size="25" style="border: rgb(110, 90, 35) 1px solid; background-color: rgb(254, 252, 252)">
                                    &nbsp;
                                    <input type="submit" name="ctl02" value=" Search " style="border: rgb(110, 90, 35) 1px solid; background-color: rgb(252, 249, 232)">
                                    &nbsp;
                                </td>
                            </tr>
                        </table>
                    </td>';

            echo '</tr>';

            // Links de localização de página da tabela
            echo '<tr>';

            $ParamGetLink = ($_SESSION['LIST_ALL']['VIEWSTATE']['search'] == '&category=&search' ? '' : '&category='.$_SESSION['LIST_ALL']['VIEWSTATE']['category_select'].'&search='.$_SESSION['LIST_ALL']['VIEWSTATE']['search']).'&sort='.($_SESSION['LIST_ALL']['VIEWSTATE']['category_sort'] == '-' ? '' : $_SESSION['LIST_ALL']['VIEWSTATE']['category_sort']);

            echo '  <td colspan="2" align="center">
                        <table height="40" align="center">
                            <tr>
                                <td width="50" align="left">
                                    <a style="display: inline" id="_FirstPage" href="'.LINKS['MNL_LIST_ALL'].'?page='.$this->table->getFirst().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_pre01.gif" border="0">
                                    </a>
                                    &nbsp;
                                    <a style="display: inline" id="_PrevPage" href="'.LINKS['MNL_LIST_ALL'].'?page='.$this->table->getPrev().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_pre02.gif" border="0">
                                    </a>
                                </td>
                                <td align="center">
                                    &nbsp;&nbsp;
                                    '.$this->table->makeListLink(LINKS['MNL_LIST_ALL'], $ParamGetLink).'
                                    &nbsp;&nbsp;
                                </td>
                                <td width="50" align="right">
                                    <a style="display: inline" id="_NextPage" href="'.LINKS['MNL_LIST_ALL'].'?page='.$this->table->getNext().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_next01.gif" border="0">
                                    </a>
                                     &nbsp; 
                                    <a style="display: inline" id="_LastPage" href="'.LINKS['MNL_LIST_ALL'].'?page='.$this->table->getLast().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_next02.gif" border="0">
                                    </a>
                                </td>
                            </tr>
                        </table>
                    </td>';
            
            echo '</tr>';

            // Fecha table
            echo '</table>';

            // Fech Form
            echo '</form>';
        }

        private function listGuild($page, $category, $search, $sort) {

            // Convert Search from UTF-8 to SHIFT_JIS
            $search = mb_convert_encoding($search, "SJIS", "UTF-8");

            $guilds = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', $page);       // Page
            $params->add('i', $category);   // Category
            $params->add('s', $search);     // Search
            $params->add('i', $sort);       // Sort

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetListGuild ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_GUILD_UID_" as "GUILD_UID", "_GUILD_NAME_" as "GUILD_NAME", "_GUILD_MASTER_" as "GUILD_MASTER", "_MEMBERS_" as "MEMBERS", "_GUILD_CONDITION_LEVEL_" as "GUILD_CONDITION_LEVEL", "_GUILD_STATE_" as "GUILD_STATE", "_GUILD_FLAG_" as "GUILD_FLAG", "_GUILD_MARK_IMG_IDX_" as "GUILD_MARK_IMG_IDX", "_GUILD_REG_DATE_" as "GUILD_REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetListGuild(?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetListGuild(?, ?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS'])) {

                $linhas = $row['LINHAS'];

                do {

                    if (isset($row['GUILD_UID']) && isset($row['GUILD_NAME']) && isset($row['GUILD_MASTER']) 
                        && isset($row['MEMBERS']) && isset($row['GUILD_CONDITION_LEVEL']) && isset($row['GUILD_STATE']) 
                        && isset($row['GUILD_FLAG']) && isset($row['GUILD_MARK_IMG_IDX']) && isset($row['GUILD_REG_DATE'])) {
                        
                        // add linha table guild
                        $guilds[] = [
                            'GUILD_UID' => $row['GUILD_UID'],
                            'GUILD_NAME' => $row['GUILD_NAME'],
                            'GUILD_MASTER' => $row['GUILD_MASTER'],
                            'MEMBERS' => $row['MEMBERS'],
                            'GUILD_CONDITION_LEVEL' => $row['GUILD_CONDITION_LEVEL'],
                            'GUILD_STATE' => $row['GUILD_STATE'],
                            'GUILD_FLAG' => $row['GUILD_FLAG'],
                            'GUILD_MARK_IMG_IDX' => $row['GUILD_MARK_IMG_IDX'],
                            'GUILD_REG_DATE' => $row['GUILD_REG_DATE']
                        ];

                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(12, $page, $linhas, $guilds);
        }
    }

    // Guild List All
    $list = new GuildListAll();

    $list->show();
?>