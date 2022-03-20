<?php
    // Arquivo index.php
    // Criado em 15/07/2019 as 10:55 por Acrisio
    // Definição e Implementação da classe Index (Guild New Index)

    include_once("source/guild_new.inc");
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    define('BASE_GUILD_NEW_URL', BASE_GUILD_URL.'guild_new');

    class Index extends GuildNew {

        private $today = [];
        private $table = null;

        public function show() {

            $this->loadToday();

            $this->listActivity();

            $this->begin();

            echo '<title>Guild New Index</title>';
            
            $this->middle();
            
            $this->content();
            
            $this->end();

        }

        private function loadToday() {

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcGetGuildNewsCreate';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_GUILD_UID_" as "GUILD_UID", "_GUILD_NAME_" as "GUILD_NAME", "_GUILD_INFO_" as "GUILD_INFO", "_GUILD_MARK_IMG_IDX_" as "GUILD_MARK_IMG_IDX", "_GUILD_REG_DATE_" as "GUILD_REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetGuildNewsCreate()';
            else
                $query = 'CALL '.$db->con_dados['DB_NAME'].'.ProcGetGuildNewsCreate()';

            if (($result = $db->db->execPreparedStmt($query, null, 1)) != null && $db->db->getLastError() == 0) {

                $this->today = [];

                while ($row = $result->fetch_assoc())
                    $this->today[] = ['GUILD_UID' => $row['GUILD_UID'],
                                      'GUILD_NAME' => $row['GUILD_NAME'],
                                      'GUILD_INTRO' => $row['GUILD_INFO'],
                                      'GUILD_MARK_IDX' => $row['GUILD_MARK_IMG_IDX'],
                                      'GUILD_REG_DATE' => $row['GUILD_REG_DATE']
                                    ];
            }
        }

        protected function content() {

            echo '<table width="615" cellspacing="0" cellpadding="0" border="0">';

            echo '  <tr>
                        <td>
                            <img src="'.BASE_GUILD_NEW_URL.'/img/title01.gif" width="103" height="29">
                        </td>
                    </tr>
                    <tr>
                        <td height="86">
                            <table class="table_listgn" cellspacing="0" cellpadding="0" width="615" border="0">
                                <tr>
                                    <td width="100">
                                        <img src="'.BASE_GUILD_NEW_URL.'/img/today01.gif" width="100" height="17">
                                    </td>
                                    <td width="479">
                                        <img src="'.BASE_GUILD_NEW_URL.'/img/today02.gif" width="497" height="17">
                                    </td>
                                    <td width="36">
                                        <img src="'.BASE_GUILD_NEW_URL.'/img/today03.gif" width="16" height="17">
                                    </td>
                                </tr>
                                <tr>
                                    <td width="100">
                                        <img src="'.BASE_GUILD_NEW_URL.'/img/today04.gif" width="100" height="39">
                                    </td>
                                    <td bgColor="#ffffff" width="479">
                                        <MARQUEE direction="up" height="39" scrollAmount="1" align="absmiddle">
                                            <dl>';

            if (count($this->today) > 0) {
                
                foreach($this->today as $value) {

                    echo '                      <dt>
                                                    <a style="display: inline" href="'.BASE_GUILD_URL.'guild_home/index.php?id='.$value['GUILD_UID'].'">
                                                        <img style="display: inline" src="'.BASE_GUILD_UPLOAD_URL.'/mark/'.$value['GUILD_UID'].'_'.$value['GUILD_MARK_IDX'].'.png" width="22" height="20" border="0">
                                                    </a>
                                                    &nbsp;
                                                    <a style="display: inline" href="'.BASE_GUILD_URL.'guild_home/index.php?id='.$value['GUILD_UID'].'">
                                                        '.htmlspecialchars(mb_convert_encoding($value['GUILD_NAME'], "UTF-8", "SJIS")).'
                                                        &nbsp;
                                                        <font color="green">
                                                            &nbsp;
                                                            '.htmlspecialchars($value['GUILD_INTRO']).'
                                                        </font>
                                                        &nbsp;
                                                        <font color="blue">
                                                            ('.$value['GUILD_REG_DATE'].')
                                                        </font>
                                                    </a>
                                                </dt>
                                                <dt>&nbsp;</dt>';
                }
            }
                                                
            echo '                          </dl>
                                        </MARQUEE>
                                    </td>
                                    <td width="36">
                                        <img src="'.BASE_GUILD_NEW_URL.'/img/today05.gif" width="16" height="39">
                                    </td>
                                </tr>
                                <tr>
                                    <td width="100">
                                        <img src="'.BASE_GUILD_NEW_URL.'/img/today06.gif" width="100" height="14">
                                    </td>
                                    <td width="479">
                                        <img src="'.BASE_GUILD_NEW_URL.'/img/today07.gif" width="497" height="14">
                                    </td>
                                    <td width="36">
                                        <img src="'.BASE_GUILD_NEW_URL.'/img/today08.gif" width="16" height="14">
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>';

            // Atividade do Player
            echo '<tr vAlign="bottom">
                    <td style="padding-top: 90px">
                        <table cellspacing="0" cellpadding="0" width="100%" border="0">
                            <tr>
                                <td>
                                    <font style="font-size: 20px; font-weight: bold; color: #77ABCA">
                                        Last activity
                                    </font>
                                </td>
                            </tr>
                            <tr>
                                <td style="padding-top: 10px" align="center">
                                    <table cellspacing="0" cellpadding="0" width="100%" border="0" bgColor="#B6D1E2">
                                        <tr>
                                            <td style="padding: 1px">
                                                <table cellspacing="1" cellpadding="0" width="100%" bgColor="#ffffff">
                                                    <thead style="height: 25px">
                                                        <tr bgColor="#B6D1E2" style="font-size: 15px;">
                                                            <td style="padding: 1px">
                                                                <font style="font-size: 15px; font-weight: bold; color: #ffffff">
                                                                    &nbsp;
                                                                    Activity
                                                                </font>
                                                            </td>
                                                            <td align="center" width="30%" style="padding: 1px">
                                                                <font style="font-size: 15px; font-weight: bold; color: #ffffff">
                                                                    Club
                                                                </font>
                                                            </td>
                                                            <td align="center" width="15%" style="padding: 1px">
                                                                <font style="font-size: 15px; font-weight: bold; color: #ffffff">
                                                                    Date
                                                                </font>
                                                            </td>
                                                        </tr>
                                                    </thead>
                                                </table>
                                                <div style="display: block; height: 185px; overflow-y: auto">
                                                    <table cellspacing="1" cellpadding="0" width="100%" bgColor="#ffffff">';

            // Table result
            if ($this->table != null && count($this->table) > 0) {

                foreach ($this->table as $activity) {

                    echo '                              <tr style="height: 20px" bgColor="#f9f9f9">
                                                            <td style="padding: 1px">
                                                                &nbsp;&nbsp;
                                                                '.($this->ACTIVITY_LABEL[$activity['FLAG']]).'
                                                            </td>
                                                            <td align="center" width="30%" style="padding: 1px">
                                                                '.(htmlspecialchars(mb_convert_encoding($activity['GUILD_NAME'], "UTF-8", "SJIS"))).'
                                                            </td>
                                                            <td align="center" width="15%" style="padding: 1px">
                                                                ('.($activity['REG_DATE']).')
                                                            </td>
                                                        </tr>
                                                        <tr bgColor="#B6D1E2">
                                                            <td height="1"></td>
                                                            <td height="1"></td>
                                                            <td height="1"></td>
                                                        </tr>';
                }
            
            }else {
                
                // Mensagem
                echo '<tr bgColor="#f9f9f9">
                        <td align="center" colspan="3">
                            no activity.
                        </td>
                      </tr>';
            }

            echo '                                  </table>
                                                </div>
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

        private function listActivity() {

            $activity = [];

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $params->clear();
            $params->add('i', PlayerSingleton::getInstance()['UID']);

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'EXEC '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildAtividade ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select "_FLAG_" as "FLAG", "_GUILD_NAME_" as "GUILD_NAME", "_REG_DATE_" as "REG_DATE" from '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildAtividade(?)';
            else
                $query = 'CALL '.$db->con_dados['DB_NAME'].'.ProcGetPlayerGuildAtividade(?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) != null && $db->db->getLastError() == 0) {

                while (($row = $result->fetch_assoc()) != null)
                    $activity[] = $row;
            }

            $this->table = $activity;

        }
    }

    // Home
    $index = new Index();

    $index->show();

?>