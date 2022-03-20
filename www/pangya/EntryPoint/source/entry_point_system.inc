<?php
    // Arquivo entry_point_system.inc
    // Criado em 28/06/2020 as 14:48 por Acrisio
    // Definição e Implementação da classe EntryPointSystem

    include_once($_SERVER['DOCUMENT_ROOT']."/EntryPoint/config/config.inc");
    include_once($_SERVER['DOCUMENT_ROOT']."/EntryPoint/util/util.inc");
    include_once($_SERVER['DOCUMENT_ROOT']."/EntryPoint/util/logger.inc");
    include_once("design.inc");
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    abstract class EntryPointSystem {

        // Abstract
        abstract public function show();
        abstract public function getPlataformName();

        protected $msg = [];

        protected function checkFirstLogin() {

            Design::checkIE();

            // Verifica se ele já está logado
            if (empty($_GET) || (!isset($_GET['ID']) && !isset($_GET['KID']))) {

                // Check Login
                Design::checkLogin();

                return;
            }

            // Vamos fazer o login aqui
            if (!isset($_GET) || !isset($_GET['ID']) || !isset($_GET['KID'])) {

                sLogger::getInstance()->putLog("[Error] Invalid request, ID or KID not sended.", $this->getPlataformName());

                $this->sendError(); // And exit();
            }

            $db = DBManagerSingleton::getInstanceDB($GLOBALS['DatabaseCurrentUsed']);
            $params = $db->params;

            $id = (isset($_GET["ID"])) ? $_GET["ID"] : null;
            $key = (isset($_GET["KID"])) ? $_GET["KID"] : null;

            if ($id == null || $key == null || strlen($id) > 25 || strlen($id) <= 0 || strlen($key) > 8 || strlen($key) <= 0) {
                
                sLogger::getInstance()->putLog("[Error] Invalid request, ID or KID invalid.", $this->getPlataformName());
                
                $this->sendError(); // And exit();
            }

            // Initialize Session and clean it
            if (!isset($_SESSION))
                session_start();

            if (isset($_SESSION['player_etp']))
                unset($_SESSION['player_etp']);

            // Init With Fail player para previnir de ele passar com algum bug
            $_SESSION['player_etp'] = ['logged' => false];
            
            // Check ID and KEY
            $params->clear();
            $params->add('s', $id);
            $params->add('s', $key);

            $query = '';

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'DECLARE @returnvalue INT = -1 exec '.$db->con_dados['DB_NAME'].'.CheckWeblink_Key$IMPL ?, ?, @returnvalue = @returnvalue OUTPUT SELECT @returnvalue as [uid]';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'select _returnvalue as uid from '.$db->con_dados['DB_NAME'].'.CheckWeblink_Key$IMPL(?, ?)';
            else
                $query = 'CALL CheckWeblink_Key(?, ?)';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                sLogger::getInstance()->putLog("[Error] Fail to exec proc check Web link key. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                $this->sendError(); // And exit();
            }
                
            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                sLogger::getInstance()->putLog("[Error] Fail in proc check web link key, result empty.", $this->getPlataformName());

                $this->sendError(); // And exit();
            }

            if (!isset($row['uid']) || is_nan($row['uid']) || $row['uid'] <= 0) {

                sLogger::getInstance()->putLog("[Error] Fail in proc check web link key, result invalid.", $this->getPlataformName());

                $this->sendError(); // And exit();
            }

            // Pega as informações iniciais da Guild do player
            $params->clear();
            $params->add('i', $row['uid']);

            if (DatabaseConfig::_MSSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT b.[UID], b.NICK AS NICKNAME, b.ID, b.IDState, a.EMAIL, c.COOKIE FROM '
                        .$db->con_dados['DB_NAME'].'.contas_beta a INNER JOIN '
                        .$db->con_dados['DB_NAME'].'.account b ON a.[UID] = b.[UID] INNER JOIN '
                        .$db->con_dados['DB_NAME'].'.user_info c ON b.[UID] = c.[UID] WHERE b.[UID] = ?';
            else if (DatabaseConfig::_PSQL_ === $GLOBALS['DatabaseCurrentUsed'])
                $query = 'SELECT b.UID AS "UID", b.NICK AS "NICKNAME", b.ID as "ID", b.IDState as "IDState", a.EMAIL as "EMAIL", c.COOKIE as "COOKIE" FROM '
                    .$db->con_dados['DB_NAME'].'.contas_beta a INNER JOIN '
                    .$db->con_dados['DB_NAME'].'.account b ON a.UID = b.UID INNER JOIN '
                    .$db->con_dados['DB_NAME'].'.user_info c ON b.UID = c.UID WHERE b.UID = ?';
            else
                $query = 'SELECT b.`UID`, b.NICK AS NICKNAME, b.ID, b.IDState, a.EMAIL, c.COOKIE FROM '
                    .$db->con_dados['DB_NAME'].'.contas_beta a INNER JOIN '
                    .$db->con_dados['DB_NAME'].'.account b ON a.`UID` = b.`UID` INNER JOIN '
                    .$db->con_dados['DB_NAME'].'.user_info c ON b.`UID` = c.`UID` WHERE b.`UID` = ?';

            if (($result = $db->db->execPreparedStmt($query, $params->get())) == null && $db->db->getLastError() != 0) {

                sLogger::getInstance()->putLog("[Error] Fail exec query select info player. Error code: ".$db->db->getLastError(), $this->getPlataformName());

                $this->sendError(); // And exit();
            }
            
            if ($result == null || ($row = $result->fetch_assoc()) == null) {

                sLogger::getInstance()->putLog("[Error] Fail in query select info player, result empty.", $this->getPlataformName());

                $this->sendError(); // And exit();
            }

            if (!isset($row['ID']) || !isset($row['UID']) || !isset($row['NICKNAME']) || !isset($row['IDState']) || !isset($row['EMAIL']) || !isset($row['COOKIE'])) {

                sLogger::getInstance()->putLog("[Error] Fail in query select info player, result invalid.", $this->getPlataformName());

                $this->sendError(); // And exit();
            }

            // Login with success
            $_SESSION['player_etp'] = 
            [ 
                'logged' => true, 
                'ID' => $row['ID'], 
                'UID' => $row['UID'], 
                'NICKNAME' => mb_convert_encoding($row['NICKNAME'], "UTF-8", "SJIS"),
                'IDState' => $row['IDState'],
                'COOKIE' => $row['COOKIE'],
                'EMAIL' => $row['EMAIL']
            ];
                                    
            // Success Login
            sLogger::getInstance()->putLog("[Log] Player Logou com sucesso.", $this->getPlataformName());
        }

        protected function setError($error, $code) {

            $this->msg = [
                'error' => $error,
                'code' => $code
            ];
        }

        protected function displayError() {

            $error = '';

            if (!empty($this->msg) && isset($this->msg['error']) && key_exists('code', $this->msg)) {
                
                $error = '<span class="'.($this->msg['code'] !== null ? 'error' : 'success').'">'.$this->msg['error'];
                
                if ($this->msg['code'] !== null && $this->msg['code'] != 0)
                    $error .= ' - Code: '.$this->msg['code'];

                $error .= '</span>';
            }

            return $error;
        }

        protected function sendError() {

			if (!isset($_SESSION))
				session_start();

            // Desloga
			if (isset($_SESSION['player_etp']))
				unset($_SESSION['player_etp']);

            // Redireciona para a página de erro
            header("Location: ".LINKS['UNKNOWN_ERROR']);

            // sai do script para o navegador redirecionar para a página
            exit();

            return;
        }

        protected function begin() {

            Design::beginHTML();

            Design::beginHead();
        }

        protected function middle() {

            Design::endHead();
            
            Design::beginBody();
            
            Design::beginContent();
        }

        protected function end() {

            Design::endContent();
            
            Design::endBody();
            
            Design::endHTML();
        }
    }
?>