<?php
    // Arquivo notice_manager.inc
    // Criado em 21/10/2019 as 20:36 por Acrisio
    // Definição e Implementação da classe NoticeManager

    // Endereço do arquivo que guarda as notices
    define("NOTICE_DB_ADDR", "notice.dat");

    // Tipos de Notice
    define("NT_ALL", 0);
    define("NT_NOTICE", 1);
    define("NT_EVENT", 2);
    define("NT_UPDATE", 3);

    class NoticeManager {

        private $notices = [];

        public function __construct() {
            $this->read();
        }

        private function read() {
            
            // Verifica se o arquivo existe, se não cria o arquivo
            if (!file_exists($_SERVER['DOCUMENT_ROOT']."config/".NOTICE_DB_ADDR))
                file_put_contents($_SERVER['DOCUMENT_ROOT']."config/".NOTICE_DB_ADDR, "");
                
            // open file data notices
            $data = file_get_contents($_SERVER['DOCUMENT_ROOT']."config/".NOTICE_DB_ADDR);

            if ($data)
                $this->notices = unserialize($data);
            else
                $this->notices = [];
        }

        private function save() {
            file_put_contents($_SERVER['DOCUMENT_ROOT']."config/".NOTICE_DB_ADDR, serialize($this->notices));
        }

        private function getLastId() {

            if (count($this->notices))
                return end($this->notices)->id;

            return -1;  // +1 da 0
        }

        public function getAllNotices() {
            return $this->notices;
        }

        private function cmp_by_date($a, $b) {

            if ($a->date == $b->date) {
               
                if ($a->id == $b->id)
                    return 0;

                return ($a->id < $b->id) ? 1 : -1;
            }
            
            return ($a->date < $b->date) ? 1 : -1;
        }

        public function getAllNoticesSortByDate() {

            $arr = $this->notices;

            usort($arr, "NoticeManager::cmp_by_date");

            return $arr;
        }

        public function getNoticesByType($type) {

            if ($type == NT_ALL)
                return $this->getAllNotice();

            $ret = [];

            switch ($type) {
                case NT_NOTICE:
                case NT_EVENT:
                case NT_UPDATE: // Garante que é um tipo conhecido
                    foreach($this->notices as $n)
                        if ($n->type == $type)
                            $ret[] = $n;
                    break;
            }

            return $ret;
        }

        public function findNotice($id) {

            $notice = null;

            if (count($this->notices) > 0 && key_exists($id, $this->notices))
                $notice = $this->notices[$id];

            return $notice;
        }

        public function makeNotice($notice) {

            $ret = false;

            if (count($notice) == 4 && key_exists('type', $notice)
                && key_exists('title', $notice) && key_exists('body', $notice) 
                && key_exists('date', $notice)) {

                // Convert array to Object
                $notice = (Object)$notice;

                switch ($notice->type) {
                    case NT_NOTICE:
                    case NT_EVENT:
                    case NT_UPDATE: // Garante que é um tipo conhecido

                        // Ler as notice, "update"
                        $this->read();

                        // notice ID
                        $notice->id = $this->getLastId() + 1;

                        // Add nova notice
                        $this->notices[] = $notice;

                        // Salva
                        $this->save();

                        // Sucesso
                        $ret = true;
                        break;
                }
            }

            return $ret;
        }

        public function makeNoticeFull($type, $title, $body, $date) {
            
            // Add nova notice
            return $this->makeNotice([
                'type' => $type,
                'title' => $title,
                'body' => $body,
                'date' => $date
            ]);
        }

        public function editNotice($notice) {

            $ret = false;

            if (count($notice) == 5 && key_exists('type', $notice) && key_exists('title', $notice)
                && key_exists('body', $notice) && key_exists('date', $notice) && key_exists('id', $notice)) {

                // Convert array para object
                $notice = (Object)$notice;
                
                if ($notice->id >= 0) {

                    // Ler de novo "Update"
                    $this->read();

                    if (key_exists($notice->id, $this->notices)) {

                        switch ($notice->type) {
                            case NT_NOTICE:
                            case NT_EVENT:
                            case NT_UPDATE: // Garante que é um tipo conhecido

                                // Edita notice
                                $this->notices[$notice->id] = $notice;

                                // Salva
                                $this->save();

                                // Sucesso
                                $ret = true;
                                break;
                        }
                    }
                }
            }

            return $ret;
        }

        public function editNoticeFull($id, $type, $title, $body, $date) {

            // Edita a notice
            return $this->editNotice([
                'type' => $type,
                'title' => $title,
                'body' => $body,
                'date' => $date,
                'id' => $id
            ]);
        }

        public function deleteNotice($id) {

            $ret = false;

            if ($id >= 0) {

                // Ler de novo "Update"
                $this->read();

                if (key_exists($id, $this->notices)) {

                    // Deleta do vector
                    unset($this->notices[$id]);

                    $ret = !key_exists($id, $this->notices);

                    // Salve
                    $this->save();
                }
            }

            return $ret;
        }
    }
?>