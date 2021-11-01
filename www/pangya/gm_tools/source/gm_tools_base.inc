<?php
    // Arquivo gm_tools_base.inc
    // Criado em 07/11/2020 as 02:40 por Acrisio
    // Definição e Implementação da classe Base GM Tools

    include_once("source/player_singleton.inc");
    include_once("source/debug_log.inc");

    // Golden Time
    const SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_GOLDEN_TIME_EVENT          = 20000;
    const SYSTEM_ERROR_FAIL_IN_PROC_INSERT_GOLDEN_TIME_EVENT            = 20001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_GOLDEN_TIME_EVENT  = 20002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_GOLDEN_TIME_ROUND          = 21000;
    const SYSTEM_ERROR_FAIL_IN_PROC_INSERT_GOLDEN_TIME_ROUND            = 21001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_GOLDEN_TIME_ROUND  = 21002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_GOLDEN_TIME_ITEM           = 22000;
    const SYSTEM_ERROR_FAIL_IN_PROC_INSERT_GOLDEN_TIME_ITEM             = 22001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_GOLDEN_TIME_ITEM   = 22002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_GOLDEN_TIME_EVENT          = 30000;
    const SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_GOLDEN_TIME_EVENT            = 30001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_GOLDEN_TIME_EVENT  = 30002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_GOLDEN_TIME_ROUND          = 31000;
    const SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_GOLDEN_TIME_ROUND            = 31001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_GOLDEN_TIME_ROUND  = 31002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_GOLDEN_TIME_ITEM           = 32000;
    const SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_GOLDEN_TIME_ITEM             = 32001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_GOLDEN_TIME_ITEM   = 32002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_GOLDEN_TIME_EVENT          = 40000;
    const SYSTEM_ERROR_FAIL_IN_PROC_DELETE_GOLDEN_TIME_EVENT            = 40001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_GOLDEN_TIME_EVENT  = 40002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_GOLDEN_TIME_ROUND          = 41000;
    const SYSTEM_ERROR_FAIL_IN_PROC_DELETE_GOLDEN_TIME_ROUND            = 41001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_GOLDEN_TIME_ROUND  = 41002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_GOLDEN_TIME_ITEM           = 42000;
    const SYSTEM_ERROR_FAIL_IN_PROC_DELETE_GOLDEN_TIME_ITEM             = 42001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_GOLDEN_TIME_ITEM   = 42002;

    // Login Reward
    const SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_LOGIN_REWARD_EVENT         = 50000;
    const SYSTEM_ERROR_FAIL_IN_PROC_INSERT_LOGIN_REWARD_EVENT           = 50001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_LOGIN_REWARD_EVENT = 50002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_LOGIN_REWARD_EVENT         = 60000;
    const SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_LOGIN_REWARD_EVENT           = 60001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_LOGIN_REWARD_EVENT = 60002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_LOGIN_REWARD_EVENT         = 70000;
    const SYSTEM_ERROR_FAIL_IN_PROC_DELETE_LOGIN_REWARD_EVENT           = 70001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_LOGIN_REWARD_EVENT = 70002;

    // Bot GM Event
    const SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_BOT_GM_EVENT_TIME              = 80000;
    const SYSTEM_ERROR_FAIL_IN_PROC_INSERT_BOT_GM_EVENT_TIME                = 80001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_BOT_GM_EVENT_TIME      = 80002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_INSERT_BOT_GM_EVENT_REWARD            = 81000;
    const SYSTEM_ERROR_FAIL_IN_PROC_INSERT_BOT_GM_EVENT_REWARD              = 81001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_INSERT_BOT_GM_EVENT_REWARD    = 81002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_BOT_GM_EVENT_TIME              = 90000;
    const SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_BOT_GM_EVENT_TIME                = 90001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_BOT_GM_EVENT_TIME      = 90002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_UPDATE_BOT_GM_EVENT_REWARD            = 91000;
    const SYSTEM_ERROR_FAIL_IN_PROC_UPDATE_BOT_GM_EVENT_REWARD              = 91001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_UPDATE_BOT_GM_EVENT_REWARD    = 91002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_BOT_GM_EVENT_TIME              = 100000;
    const SYSTEM_ERROR_FAIL_IN_PROC_DELETE_BOT_GM_EVENT_TIME                = 100001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_BOT_GM_EVENT_TIME      = 100002;
    const SYSTEM_ERROR_FAIL_EXEC_PROC_DELETE_BOT_GM_EVENT_REWARD            = 101000;
    const SYSTEM_ERROR_FAIL_IN_PROC_DELETE_BOT_GM_EVENT_REWARD              = 101001;
    const SYSTEM_ERROR_FAIL_IN_VERIFY_OF_PROC_DELETE_BOT_GM_EVENT_REWARD    = 101002;

    abstract class GMTools {

        protected static $LIMIT_EVENTS_PER_PAGE = 20;

        abstract protected function getName();
        abstract public function show();

        protected $msg = [];
        protected $page = 0;

        protected function checkLogin() {

            if (PlayerSingleton::getInstance() == null || !PlayerSingleton::getInstance()['logged']) {

                // Log
                DebugLog::Log("Not logged, tryed enter GM Tools(".$this->getName().")");

                header("Location: ".HOME_URL);

                exit();
            }
        }

        protected function checkCapacity() {

            if (!((PlayerSingleton::getInstance()['cap'] & 4) == 4 && (PlayerSingleton::getInstance()['cap'] & 8) == 8)) {

                // Log
                DebugLog::Log("Not have capability(".PlayerSingleton::getInstance()['cap'].") to access the page GM Tools(".$this->getName().").");

                header("Location: ".HOME_URL);

                exit();
            }
        }

        protected function checkLoginAndCapacity() {

            $this->checkLogin();

            $this->checkCapacity();
        }

        protected function setError($error, $code) {

            $this->msg = [
                'error' => $error,
                'code' => $code
            ];
        }

        protected function showError() {

            $ret = '';

            if (empty($this->msg) || !isset($this->msg['error']) || !key_exists('code', $this->msg) || $this->msg['error'] == '')
                return $ret;

            if ($this->msg['code'] === null)
                $ret = '<text class="message-label">'.$this->msg['error'].'</text>';
            else if ($this->msg['code'] == 0)
                $ret = '<text class="error-label">'.$this->msg['error'].'</text>';
            else
                $ret = '<text class="error-label">'.$this->msg['error'].'. Code: '.$this->msg['code'].'</text>';

            if ($ret != '')
                $ret .= '<i class="message-cross" onClick="this.parentElement.style.display = \'none\'">x</i>';

            return $ret;
        }

        protected function putLog($log) {

            DebugLog::Log("[{$this->getName()}]".$log);
        }
    }
?>