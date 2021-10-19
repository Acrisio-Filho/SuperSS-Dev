<?php
    // Arquivo Club_New_Ranking.php
    // Criado em 16/07/2019 as 10:56 por Acrisio
    // Definição e Implementação da classe Ranking

    include_once("source/guild_new.inc");

    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once("../source/table.inc");

    define("BASE_GUILD_NEW_URL", BASE_GUILD_URL."guild_new");
    define("BASE_GUILD_HOME_URL", BASE_GUILD_URL."guild_home");

    class Ranking extends GuildNew {

        private $table = null;
        private $update_time = 'NONE';

        public function show() {

            $this->checkPost();

            $this->listRanking($_SESSION['RANKING']['VIEWSTATE']['page'], 
                               $_SESSION['RANKING']['VIEWSTATE']['search']);

            $this->begin();

            echo '<title>Guild Ranking</title>';

            $this->middle();

            $this->content();

            $this->end();

        }

        private function checkPost() {

            //print_r($_POST);
            //print_r($_GET);

            // Inicializa default values
            $this->update_time = date('Y-m-d');

            if (!isset($_SESSION))
                session_start();

            // Cria session LIST_ALL
            if (!isset($_SESSION['RANKING']))
                $_SESSION['RANKING'] = [];
            else if (empty($_GET)) {
                
                // Limpa SESSION RANKING
                unset($_SESSION['RANKING']);
                
                $_SESSION['RANKING'] = [];
            }

            if (!empty($_POST)) {
                // Init and Update VIEWSATE
                $_SESSION['RANKING']['VIEWSTATE'] = [
                    'page' => 0,
                    'search' => isset($_POST['_search']) ? strip_tags($_POST['_search']) : ''
                ];

            }else if (!empty($_GET)) {

                $_SESSION['RANKING']['VIEWSTATE'] = [
                    'page' => isset($_GET['page']) && $_GET['page'] != '' && is_numeric($_GET['page']) ? $_GET['page'] : 0,
                    'search' => isset($_GET['search']) 
                            ? strip_tags($_GET['search']) 
                            : $_SESSION['RANKING']['VIEWSTATE']['search'] ?? ''
                ];                
            
            }else if (!isset($_SESSION['RANKING']['VIEWSTATE'])) {

                $_SESSION['RANKING']['VIEWSTATE'] = [
                    'page' => 0,
                    'search' => ''
                ];
            }
        }

        private function content() {

            echo '<form id="ctl00" method="post" action="./Club_New_Ranking.php">';

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';
            // Linha do title
            echo '<tr>';

            // Title
            echo '  <td height="40" vAlign="top" width="615" align="left">
                        <img src="'.BASE_GUILD_NEW_URL.'/img/title06.gif">
                        <div style="text-align: right">Last update: '.($this->update_time).'&nbsp;</div>
                    </td>';

            echo '</tr>';

            // Conteúdo da tabela
            echo '<tr>
                    <td height="86" colspan="2" align="center">';

            echo '<table bgColor="#ffffff" class="table_listgn" cellspacing="2" cellpadding="0" width="609" align="center" border="0">
                        <tr bgColor="#fff5ee">
                            <th height="33" width="60">Rank</th>
                            <th height="33" width="149">Name</th>
                            <th height="33" width="50">Win</th>
                            <th height="33" width="50">Lose</th>
                            <th height="33" width="50">Draw</th>
                            <th height="33" width="80">Point</th>
                            <th height="33" width="80">Pang</th>
                            <th height="33" width="100">Last rank</th>
                        </tr>
                    </table>
                    <table cellspacing="1" cellpadding="0" width="609" border="0">';
                   
            // Table Result

            if ($this->table != null && !$this->table->isEmpty()) {

                foreach ($this->table->getCurrentRow() as $guild) {

                    echo '<tr bgColor="#fffaf0">
                            <td height="28" width="60" align="center">
                                &nbsp;';

                    if ($guild['RANK'] >= 1 && $guild['RANK'] <= 3)
                        echo '<img style="display: inline" src="'.BASE_GUILD_NEW_URL.'/img/rank_'.str_pad($guild['RANK'], 2, '0', STR_PAD_LEFT).'.gif">';
                    else
                        echo $guild['RANK'];

                    echo '  </td>
                            <td height="28" vAlign="middle" width="149">
                                &nbsp;&nbsp;
                                <img style="display: inline" src="'.BASE_GUILD_UPLOAD_URL.'/mark/'.($guild['GUILD_UID'].'_'.$guild['GUILD_MARK_IMG_IDX']).'.png" width="22" height="20">
                                &nbsp;
                                <a style="display: inline" href="'.BASE_GUILD_HOME_URL.'/index.php?id='.($guild['GUILD_UID']).'">
                                    '.(htmlspecialchars(mb_convert_encoding($guild['GUILD_NAME'], "UTF-8", "SJIS"))).'
                                </a>
                            </td>
                            <td height="28" width="50" align="center">
                                '.($guild['GUILD_WIN']).'
                            </td>
                            <td height="28" width="50" align="center">
                                '.($guild['GUILD_LOSE']).'
                            </td>
                            <td height="28" width="50" align="center">
                                '.($guild['GUILD_DRAW']).'
                            </td>
                            <td height="28" width="80" align="center">
                                '.($guild['GUILD_POINT']).'
                            </td>
                            <td height="28" width="80" align="center">
                                '.($guild['GUILD_PANG']).'
                            </td>
                            <td height="28" width="100" align="center">
                                '.($guild['LAST_RANK'] != -1 ? $guild['LAST_RANK'] : '').' ( '.
                                ($guild['LAST_RANK'] == -1 
                                    ? '<span style="color: #ff55aa">NEW</span>'
                                    : ($guild['LAST_RANK'] == $guild['RANK']
                                        ? '<span style="color: gray">-</span>'
                                        : ($guild['LAST_RANK'] > $guild['RANK']
                                            ? '<span style="color: blue">▲ </span>'.($guild['LAST_RANK'] - $guild['RANK'])
                                            : '<span style="color: red">▼ </span>'.($guild['RANK'] - $guild['LAST_RANK'])
                                        )
                                      )
                                ).' )
                            </td>
                        </tr>
                        <tr bgColor="#dddddd">
                            <td height="1" colspan="8"></td>
                        </t>';
                }
            
            }else {

                echo '  <tr bgColor="#fffaf0">
                            <td height="28" align="center" colspan="8">no clubs found</td>
                        </tr>
                        <tr bgColor="#dddddd">
                            <td height="1" colspan="8"></td>
                        </t>';
            }

            echo '</table>';

            // Pequisa
            echo '<table cellspacing="0" cellpadding="0" width="609" border="0">
                    <tr>
                        <td height="2" colspan="5">
                        </td>
                    </tr>
                    <tr align="right" bgColor="#f6f3e3">
                        <td height="30" colspan="3" align="left">
                            &nbsp;
                            '.('Message').'
                        </td>
                        <td height="30" colspan="5">
                            Search Name
                            <input id="_search" value="'.$_SESSION['RANKING']['VIEWSTATE']['search'].'" style="border: rgb(110,90,35) 1px solid; padding: 0px 3px; background-color: rgb(254,252,252)" maxLength="20" size="25" name="_search" type="text">
                            &nbsp;
                            <input style="border: rgb(110,90,35) 1px solid; background-color: rgb(252,249,232)" type="submit" value="Search" name="ctl01">
                            &nbsp;
                        </td>
                    </tr>
                  </table>';

            echo '  </td>
                </tr>';

            // Links de localização de página da tabela
            echo '<tr>';

            $ParamGetLink = '&search='.$_SESSION['RANKING']['VIEWSTATE']['search'];

            echo '  <td colspan="2" align="center">
                        <table height="40" align="center">
                            <tr>
                                <td width="50" align="left">
                                    <a style="display: inline" id="_FirstPage" href="'.BASE_GUILD_NEW_URL.'/Club_New_Ranking.php?page='.$this->table->getFirst().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_pre01.gif" border="0">
                                    </a>
                                    &nbsp;
                                    <a style="display: inline" id="_PrevPage" href="'.BASE_GUILD_NEW_URL.'/Club_New_Ranking.php?page='.$this->table->getPrev().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_pre02.gif" border="0">
                                    </a>
                                </td>
                                <td align="center">
                                    &nbsp;&nbsp;
                                    '.$this->table->makeListLink(BASE_GUILD_NEW_URL."/Club_New_Ranking.php", $ParamGetLink).'
                                    &nbsp;&nbsp;
                                </td>
                                <td width="50" align="right">
                                    <a style="display: inline" id="_NextPage" href="'.BASE_GUILD_NEW_URL.'/Club_New_Ranking.php?page='.$this->table->getNext().$ParamGetLink.'">
                                        <img style="display: inline" src="img/bt_next01.gif" border="0">
                                    </a>
                                     &nbsp; 
                                    <a style="display: inline" id="_LastPage" href="'.BASE_GUILD_NEW_URL.'/Club_New_Ranking.php?page='.$this->table->getLast().$ParamGetLink.'">
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

        private function listRanking($page, $search) {

            // Convert Search from UTF-8 to SHIFT_JIS
            $search = mb_convert_encoding($search, "SJIS", "UTF-8");

            $ranking = [];
            $linhas = 0;

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;
            
            $params->clear();
            $params->add('i', $page);       // Page
            $params->add('s', $search);     // Search

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'exec '.$db->con_dados['DB_NAME'].'.ProcGetGuildRanking ?, ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_LINHAS_" as "LINHAS", "_REG_DATE_" as "REG_DATE", "_RANK_" as "RANK", "_GUILD_UID_" as "GUILD_UID", "_GUILD_NAME_" as "GUILD_NAME", "_GUILD_WIN_" as "GUILD_WIN", "_GUILD_LOSE_" as "GUILD_LOSE", "_GUILD_DRAW_" as "GUILD_DRAW", "_GUILD_POINT_" as "GUILD_POINT", "_GUILD_PANG_" as "GUILD_PANG", "_LAST_RANK_" as "LAST_RANK", "_GUILD_MARK_IMG_IDX_" as "GUILD_MARK_IMG_IDX" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildRanking(?, ?)';
            else
                $query = 'call '.$db->con_dados['DB_NAME'].'.ProcGetGuildRanking(?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) && $db->db->getLastError() == 0
                && ($row = $result->fetch_assoc()) != null && isset($row['LINHAS']) && isset($row['REG_DATE'])) {

                $linhas = $row['LINHAS'];

                $this->update_time = $row['REG_DATE'];

                do {

                    if (isset($row['RANK']) && isset($row['GUILD_UID']) && isset($row['GUILD_NAME']) && isset($row['GUILD_WIN']) 
                        && isset($row['GUILD_LOSE']) && isset($row['GUILD_DRAW']) && isset($row['GUILD_POINT']) && isset($row['GUILD_PANG']) 
                        && isset($row['LAST_RANK']) && isset($row['GUILD_MARK_IMG_IDX'])) {

                        $ranking[] = [
                            'RANK' => $row['RANK'],
                            'GUILD_UID' => $row['GUILD_UID'],
                            'GUILD_NAME' => $row['GUILD_NAME'],
                            'GUILD_WIN' => $row['GUILD_WIN'],
                            'GUILD_LOSE' => $row['GUILD_LOSE'],
                            'GUILD_DRAW' => $row['GUILD_DRAW'],
                            'GUILD_POINT' => $row['GUILD_POINT'],
                            'GUILD_PANG' => $row['GUILD_PANG'],
                            'LAST_RANK' => $row['LAST_RANK'],
                            'GUILD_MARK_IMG_IDX' => $row['GUILD_MARK_IMG_IDX']
                        ];
                    }

                } while (($row = $result->fetch_assoc()) != null);

            }

            // Create Table
            $this->table = new table(10, $page, $linhas, $ranking);
        }
    }

    // Guild Ranking
    $ranking = new Ranking();

    $ranking->show();
?>