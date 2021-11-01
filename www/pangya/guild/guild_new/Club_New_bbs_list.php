<?php
    // Arquivo Club_New_bbs_list.php
    // Criado em 16/07/2019 as 10:53 por Acrisio
    // Definição e Implementação da classe BBSList

    include_once("source/guild_new.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once("../source/table.inc");

    define('BASE_GUILD_NEW_URL', BASE_GUILD_URL.'guild_new');

    class BBSList extends GuildNew {

        private $table = null;

        public function show() {

            $this->checkPost();

            $this->listBBS($_SESSION['LIST_ALL']['VIEWSTATE']['page'], 
                             ($_SESSION['LIST_ALL']['VIEWSTATE']['category'] == '' ? 10 : $_SESSION['LIST_ALL']['VIEWSTATE']['category']), 
                             $_SESSION['LIST_ALL']['VIEWSTATE']['search'], 
                             ($_SESSION['LIST_ALL']['VIEWSTATE']['sort'] == 'radiobutton1' || $_SESSION['LIST_ALL']['VIEWSTATE']['sort'] == '' 
                                ? 6/*REG_DATE BBS*/ 
                                : 0/*REG_DATE LAST BBS REPLY*/));

            $this->begin();

            echo '<title>Guild BBS List</title>';

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
                    'sort' => isset($_POST['sort']) && $_POST['sort'] != '' ? $_POST['sort'] : '',
                    'category' => isset($_POST['category']) && $_POST['category'] != '' && is_numeric($_POST['category']) ? $_POST['category'] : '',
                    'search' => isset($_POST['search']) ? $_POST['search'] : ''
                ];

            }else if (!empty($_GET)) {

                $_SESSION['LIST_ALL']['VIEWSTATE'] = [
                    'page' => isset($_GET['page']) && $_GET['page'] != '' && is_numeric($_GET['page']) ? $_GET['page'] : 0,
                    'sort' => isset($_GET['sort']) && $_GET['sort'] != '' 
                            ? $_GET['sort'] 
                            : $_SESSION['LIST_ALL']['VIEWSTATE']['sort'] ?? '',
                    'category' => isset($_GET['category']) && $_GET['category'] != '' && is_numeric($_GET['category'])
                            ? $_GET['category'] 
                            : $_SESSION['LIST_ALL']['VIEWSTATE']['category'] ?? '',
                    'search' => isset($_GET['search']) 
                            ? $_GET['search'] 
                            : $_SESSION['LIST_ALL']['VIEWSTATE']['search'] ?? ''
                ];                
            
            }else if (!isset($_SESSION['LIST_ALL']['VIEWSTATE'])) {

                $_SESSION['LIST_ALL']['VIEWSTATE'] = [
                    'page' => 0,
                    'sort' => '',
                    'category' => '',
                    'search' => ''
                ];
            }
        }

        private function content() {

            echo '<form id="ctl00" method="post" action="./Club_New_bbs_list.php">';

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';

            echo '  <tr>
                        <td height="40" vAlign="top" width="165">
                            <img src="'.BASE_GUILD_NEW_URL.'/img/title03.gif" width="125" height="29">
                        </td>
                        <td width="450" align="right">
                            <span style="border: 0px solid; background-color: rgb(252,249,232)">
                                <input id="radiobutton1" type="radio" value="radiobutton1" name="sort" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['sort'] == 'radiobutton1' || $_SESSION['LIST_ALL']['VIEWSTATE']['sort'] == '') ? 'checked' : '').'>
                                <label for="radiobutton1">
                                    New Article
                                </label>
                            </span>
                            <span style="border: 0px solid; background-color: rgb(252,249,232)">
                                <input id="radiobutton2" type="radio" value="radiobutton2" name="sort" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['sort'] == 'radiobutton2') ? 'checked' : '').'>
                                <label for="radiobutton2">
                                    Last Reply
                                </label>
                            </span>
                            &nbsp;&nbsp;
                            <select id="category" style="border: rgb(110,90,35) 1px solid; background-color: rgb(252,249,232)" name="category">
                                <option value="10" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category'] == '10' || $_SESSION['LIST_ALL']['VIEWSTATE']['category'] == '') ? 'selected' : '').'>Show all</option>
                                <option value="0" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category'] == '0') ? 'selected' : '').'>Members Wanted</option>
                                <option value="1" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category'] == '1') ? 'selected' : '').'>Opponent search</option>
                                <option value="2" '.(($_SESSION['LIST_ALL']['VIEWSTATE']['category'] == '2') ? 'selected' : '').'>Chat & Question</option>
                            </select>
                            <input style="border: rgb(110,90,35) 1px solid; background-color: rgb(252,249,232)" type="submit" name="ctl01" value="Change category">
                        </td>
                    </tr>
                    <tr>
                        <td colspan="2" align="center">
                            <table cellspacing="0" cellpadding="0" width="610">
                                <tr>
                                    <td></td>
                                </tr>
                            </table>
                            <table cellspacing="1" cellpadding="0" width="610" bgColor="#ffffe3">
                                <tr bgColor="#b39f8e">
                                    <td height="2" colspan="5"></td>
                                </tr>
                                <tr bgColor="#f2eccd">
                                    <td height="20" width="50" align="center">
                                        <div style="color: #6b4e17">Number</div>
                                    </td>
                                    <td height="20" width="70" align="center">
                                        <div style="color: #6b4e17">Category</div>
                                    </td>
                                    <td height="20" width="274" align="center">
                                        <div style="color: #6b4e17">Title</div>
                                    </td>
                                    <td height="20" width="100" align="center">
                                        <div style="color: #6b4e17">Nickname</div>
                                    </td>
                                    <td height="20" width="100" align="center">
                                        <div style="text-align: center; color: #6b4e17">Date</div>
                                    </td>
                                </tr>
                                <tr bgColor="#b39f8e">
                                    <td height="2" colspan="5"></td>
                                </tr>
                            </table>
                            <table cellspacing="1" cellpadding="0" width="610" bgColor="#f7f2d3">';

            // Table Result
            if ($this->table != null && !$this->table->isEmpty()) {

                foreach($this->table->getCurrentRow() as $bbs) {

                    echo '      <tr bgColor="#ffffff">
                                    <td height="20" width="50" align="center" style="padding: 1px">
                                        '.($bbs['SEQ']).'
                                    </td>
                                    <td height="20" width="70" align="center" style="padding: 1px">
                                        <img border="0" src="'.BASE_GUILD_NEW_URL.'/img/bbs_icon'.(str_pad($bbs['TYPE'], 2, '0', STR_PAD_LEFT)).'.gif" width="67" height="16">
                                    </td>
                                    <td height="20" width="280" align="left" style="padding: 1px">
                                        &nbsp; 
                                        <a style="display: inline" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_view.php?page='.$_SESSION['LIST_ALL']['VIEWSTATE']['page'].'&amp;seq='.($bbs['SEQ']).'&amp;category='.$_SESSION['LIST_ALL']['VIEWSTATE']['category'].'&amp;sort='.$_SESSION['LIST_ALL']['VIEWSTATE']['sort'].'&amp;search='.$_SESSION['LIST_ALL']['VIEWSTATE']['search'].'">
                                            '.($bbs['TITLE']).'
                                            &nbsp;&nbsp;
                                            <span class="bbsrescount">
                                                '.(($bbs['RES_COUNT'] > 0) ? '[Re:'.$bbs['RES_COUNT'].']' : '').'
                                            </span>
                                        </a>
                                    </td>
                                    <td height="20" width="100" align="center" style="padding: 1px">
                                        '.(htmlspecialchars(mb_convert_encoding($bbs['NICKNAME'], "UTF-8", "SJIS"))).'
                                    </td>
                                    <td height="20" width="100" align="center" style="padding: 1px">
                                        ('.($bbs['REG_DATE']).')
                                    </td>
                                </tr>';
                }
            
            }else {

                echo '          <tr bgColor="#ffffff">
                                    <td height="20" align="center" style="padding: 1px">
                                        Not Found BBS
                                    </td>
                                </tr>';
            }

            echo '          </table>
                            <table cellspacing="1" cellpadding="0" width="610" bgColor="#f7f2d3">
                                <tr bgColor="#fcf9e8">
                                    <td height="25" align="right">
                                        Title search
                                        &nbsp;
                                        <input id="search" type="text" value="'.$_SESSION['LIST_ALL']['VIEWSTATE']['search'].'" style="border: rgb(110,90,35) 1px solid; padding: 0px 3px; background-color: rgb(254,252,252)" maxLength="20" size="25" name="search">
                                        &nbsp; 
                                        <input style="border: rgb(110,90,35) 1px solid; background-color: rgb(252,249,232)" type="submit" name="ctl02" value="Search">
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>';

            // Links de localização de página da tabela
            echo '<tr>';

            $ParamGetLink = ($_SESSION['LIST_ALL']['VIEWSTATE']['search'] == '&category=&search' ? '' : '&category='.$_SESSION['LIST_ALL']['VIEWSTATE']['category'].'&search='.$_SESSION['LIST_ALL']['VIEWSTATE']['search']).'&sort='.$_SESSION['LIST_ALL']['VIEWSTATE']['sort'];

            echo '  <td colspan="2" align="center">
                        <table height="40" align="center">
                            <tr>
                                <td width="50" align="left">
                                    <a style="display: inline" id="_FirstPage" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_list.php'.'?page='.$this->table->getFirst().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_pre01.gif" border="0">
                                    </a>
                                    &nbsp;
                                    <a style="display: inline" id="_PrevPage" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_list.php'.'?page='.$this->table->getPrev().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_pre02.gif" border="0">
                                    </a>
                                </td>
                                <td align="center">
                                    &nbsp;&nbsp;
                                    '.$this->table->makeListLink(BASE_GUILD_NEW_URL.'/Club_New_bbs_list.php', $ParamGetLink).'
                                    &nbsp;&nbsp;
                                </td>
                                <td width="50" align="right">
                                    <a style="display: inline" id="_NextPage" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_list.php'.'?page='.$this->table->getNext().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_next01.gif" border="0">
                                    </a>
                                     &nbsp; 
                                    <a style="display: inline" id="_LastPage" href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_list.php'.'?page='.$this->table->getLast().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_next02.gif" border="0">
                                    </a>
                                </td>
                            </tr>
                        </table>';

            // Verifica se o player tem o level necessário para escrever um BBS
            if (PlayerSingleton::getInstance()['LEVEL'] >= 1/*Rookie E*/) {
                
                echo '  <div align="center">
                            <a href="'.BASE_GUILD_NEW_URL.'/Club_New_bbs_new_write.php">
                                <img border="0" src="'.BASE_GUILD_NEW_URL.'/img/bbs_new_write.gif">
                            </a>
                        </div>';
            }

            echo '  </td>';
            
            echo '</tr>';

            // Fecha table
            echo '</table>';

            // Fech Form
            echo '</form>';
        }

        private function listBBS($page, $category, $search, $sort) {

            $bbs = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', $page);       // Page
            $params->add('i', $category);   // Category
            $params->add('s', $search);     // Search
            $params->add('i', $sort);       // Sort

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetListGuildBBS ?, ?, ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_SEQ_" as "SEQ", "_TYPE_" as "TYPE", "_TITLE_" as "TITLE", "_RES_COUNT_" as "RES_COUNT", "_NICKNAME_" as "NICKNAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetListGuildBBS(?, ?, ?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetListGuildBBS(?, ?, ?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS'])) {

                $linhas = $row['LINHAS'];

                do {

                    if (isset($row['SEQ']) && isset($row['TYPE']) && isset($row['TITLE']) && isset($row['RES_COUNT']) 
                        && isset($row['NICKNAME']) && isset($row['REG_DATE'])) {

                        $bbs[] = [
                            'SEQ' => $row['SEQ'],
                            'TYPE' => $row['TYPE'],
                            'TITLE' => $row['TITLE'],
                            'RES_COUNT' => $row['RES_COUNT'],
                            'NICKNAME' => $row['NICKNAME'],
                            'REG_DATE' => $row['REG_DATE']
                        ];
                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(14, $page, $linhas, $bbs);
        }
    }

    // Guild BBS List
    $bbs_list = new BBSList();

    $bbs_list->show();
?>