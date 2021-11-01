<?php
    // Arquivo block_word_register.php
    // Criado em 09/05/2021 as 17:24 por Acrisio
    // Ferramenta para gerenciar o bloqueia de palavras no e-mail de registro

    define('BASE_CONFIG_PANGYA', $_SERVER['DOCUMENT_ROOT'].'/config/');

    include_once("source/gm_tools_base.inc");
    
    include_once($_SERVER['DOCUMENT_ROOT'].'/config/db_manager_singleton.inc');

    include_once('source/dynamic_tag.inc');
    include_once(BASE_CONFIG_PANGYA.'check_mail_first_register.inc');

    class BlockWordRegister extends GMTools {

        private $checker = null;

        protected function getName() {
            return "Block Word Register";
        }

        public function __construct() {

            $this->checkLoginAndCapacity();

            $this->checker = new CheckMailFirstRegister();

            $this->checkGetAndPost();
        }

        protected function checkGetAndPost() {

            if (!isset($_POST) || !isset($_GET))
                return;

            if (isset($_GET['page']) && !is_nan($_GET['page']))
                $this->page = $_GET['page'];

            if (isset($_POST['edit-word']) && $_POST['edit-word'] == 'edit') {

                $this->editWord();

            }else if (isset($_POST['add-word']) && $_POST['add-word'] == 'add') {

                $this->addWord();

            }else if (isset($_POST['remove-word']) && $_POST['remove-word'] == 'remove') {

                $this->removeWord();
            
            }else if (isset($_POST['edit-ip']) && $_POST['edit-ip'] == 'edit') {

                $this->editIP();

            }else if (isset($_POST['add-ip']) && $_POST['add-ip'] == 'add') {

                $this->addIP();

            }else if (isset($_POST['remove-ip']) && $_POST['remove-ip'] == 'remove') {

                $this->removeIP();
            
            }
        }

        protected function editWord() {

            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id']) || !isset($_POST['word']) || $_POST['word'] == '') {


                $this->putLog("[Edit Word] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (!$this->checker->isGood()) {

                $this->putLog("[Error] Não conseguiu inicializar o Check mail first register.");

                $this->setError('System error', 15000);

                return;
            }

            $ret = 0;

            if (($ret = $this->checker->editWord($_POST['id'], $_POST['word'])) != 0) {

                $this->putLog("[Error] Não conseguiu editar o word[".$_POST['id']."]: ".$_POST['word'].". Code: ".$ret);

                $this->setError('System error', 15001);

                return;
            }

            // OK
            $this->setError('Word editado com sucesso', null);
        }

        protected function editIP() {

            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id']) || !isset($_POST['ip']) || $_POST['ip'] == '') {

                $this->putLog("[Edit IP] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (!$this->checker->isGood()) {

                $this->putLog("[Error] Não conseguiu inicializar o Check mail first register.");

                $this->setError('System error', 15000);

                return;
            }

            $ret = 0;

            if (($ret = $this->checker->editIP($_POST['id'], $_POST['ip'])) != 0) {

                $this->putLog("[Error] Não conseguiu editar o ip[".$_POST['id']."]: ".$_POST['ip'].". Code: ".$ret);

                $this->setError('System error', 15002);

                return;
            }

            // OK
            $this->setError('IP editado com sucesso', null);
        }

        protected function addWord() {

            if (!isset($_POST['word']) || $_POST['word'] == '') {

                $this->putLog("[Add Word] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (!$this->checker->isGood()) {

                $this->putLog("[Error] Não conseguiu inicializar o Check mail first register.");

                $this->setError('System error', 15000);

                return;
            }

            $ret = 0;

            if (($ret = $this->checker->addWord($_POST['word'])) != 0) {

                $this->putLog("[Error] Não conseguiu adicionar o word: ".$_POST['word'].". Code: ".$ret);

                $this->setError('System error', 15003);

                return;
            }

            // OK
            $this->setError('Word adicionado com sucesso', null);
        }

        protected function addIP() {

            if (!isset($_POST['ip']) || $_POST['ip'] == '') {

                $this->putLog("[Add IP] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (!$this->checker->isGood()) {

                $this->putLog("[Error] Não conseguiu inicializar o Check mail first register.");

                $this->setError('System error', 15000);

                return;
            }

            $ret = 0;

            if (($ret = $this->checker->addIP($_POST['ip'])) != 0) {

                $this->putLog("[Error] Não conseguiu adicionar o ip: ".$_POST['ip'].". Code: ".$ret);

                $this->setError('System error', 15004);

                return;
            }

            // OK
            $this->setError('IP adicionado com sucesso', null);
        }

        protected function removeWord() {
         
            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id'])) {

                $this->putLog("[Remove Word] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (!$this->checker->isGood()) {

                $this->putLog("[Error] Não conseguiu inicializar o Check mail first register.");

                $this->setError('System error', 15000);

                return;
            }

            $ret = 0;

            if (($ret = $this->checker->removeWord($_POST['id'])) != 0) {

                $this->putLog("[Error] Não conseguiu remover o word[".$_POST['id']."]. Code: ".$ret);

                $this->setError('System error', 15005);

                return;
            }

            // OK
            $this->setError('Word deletado com sucesso', null);
        }

        protected function removeIP() {

            if (!isset($_POST['id']) || $_POST['id'] == '' || is_nan($_POST['id'])) {

                $this->putLog("[Remove IP] invalid parameters");

                $this->setError('Please insert all fields', 0);

                return;
            }

            if (!$this->checker->isGood()) {

                $this->putLog("[Error] Não conseguiu inicializar o Check mail first register.");

                $this->setError('System error', 15000);

                return;
            }

            $ret = 0;

            if (($ret = $this->checker->removeIP($_POST['id'])) != 0) {
             
                $this->putLog("[Error] Não conseguiu remover o ip[".$_POST['id']."]. Code: ".$ret);

                $this->setError('System error', 15006);

                return;
            }

            // OK
            $this->setError('IP deletado com sucesso', null);
        }

        public function show() {

            // Document
            $document = new DynamicTag('html');
            $document->lang = "pt_br";

            // Head
            $head = new DynamicTag('head');
            $title = new DynamicTag('title');

            $title->addChild("Block Word Register Tool");

            $head->addChild($title);

            $meta = new DynamicTag('meta');
            $meta->charset = "UTF-8";

            $head->addChild($meta);

            $script = new DynamicTag('script');
            $script->addChild("
                function toggle(el) {

                    if (el) {

                        el_parent = el.parentElement;

                        if (el_parent) {

                            let content = el_parent.querySelector('.event-content');

                            if (content) {

                                if (content.style.display == 'none') {

                                    el.innerText = '▲';

                                    content.style.display = 'block';

                                }else {

                                    el.innerText = '▼';

                                    content.style.display = 'none';
                                }
                            }
                        }
                    }
                }

                function editToggle(el) {

                    const next = el.parentElement.querySelector('.edit-event');

                    if (next) {

                        if (next.style.display) {

                            if (next.style.display == 'none') {

                                next.style.display = 'flex';

                                el.innerText = 'Close';

                            }else {

                                next.style.display = 'none';

                                el.innerText = 'Edit';
                            }
                        }
                    }
                }

                function editItem(el) {

                    const next = el.parentElement.nextElementSibling;

                    if (next) {

                        if (next.style.display) {

                            if (next.style.display == 'none') {

                                next.style.display = 'flex';

                                const i = el.querySelector('i');

                                if (i)
                                    i.innerText = '▲';

                            }else {

                                next.style.display = 'none';

                                const i = el.querySelector('i');

                                if (i)
                                    i.innerText = '▼';
                            }
                        }
                    }
                }

                function addItem(el, type) {

                    const next = el.parentElement.parentElement.querySelector('.' + type);

                    if (next) {

                        if (next.style.display) {

                            if (next.style.display == 'none') {

                                next.style.display = 'flex';

                                if (next.nextElementSibling) {

                                    if (next.nextElementSibling.style.display && next.nextElementSibling.style.display != 'none')
                                        next.nextElementSibling.style.display = 'none';
                                
                                }else if (next.previousElementSibling) {

                                    if (next.previousElementSibling.style.display && next.previousElementSibling.style.display != 'non')
                                        next.previousElementSibling.style.display = 'none';

                                }

                            }else {

                                next.style.display = 'none';
                            }
                        }
                    }
                }

                function addEvent(el) {

                    const next = el.parentElement.nextElementSibling;

                    if (next) {

                        if (next.style.display) {

                            if (next.style.display == 'none') {

                                next.style.display = 'flex';

                            }else {

                                next.style.display = 'none';
                            }
                        }
                    }
                }

                function addSubItem(el, type) {

                    const copy = el.parentElement.parentElement.parentElement.querySelectorAll('.' + type);
    
                    if (copy && copy.length > 0) {
    
                        const last = copy[copy.length -1];
    
                        if (last) {
                            
                            const clone = last.cloneNode(true);
    
                            // Clear
                            Array.prototype.forEach.call(clone.querySelectorAll('input'), el => {
    
                                if (el.value)
                                    el.value = '';
                            });
    
                            el.parentElement.parentElement.parentElement.insertBefore(clone, last.nextElementSibling);
                        }
                    }
                }
    
                function removeSubItem(el, type) {
    
                    const copy = el.parentElement.parentElement.parentElement.querySelectorAll('.' + type);
    
                    if (copy && copy.length > 1) {
    
                        const last = copy[copy.length -1];
    
                        if (last) {
    
                            el.parentElement.parentElement.parentElement.removeChild(last);
                        }
                    }
                }
            ");

            $head->addChild($script);

            $style = new DynamicTag('style');
            $style->addChild("
                * {
                    border: 0;
                    margin: 0;
                    padding: 0;
                    box-sizing: border-box;
                }

                .content {
                    display: flex;
                    flex-direction: column;
                    justify-content: center;
                }

                .intro {
                    margin-top: 2em;
                    margin-bottom: 2em;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                }

                .event {
                    display: grid;
                    justify-content: center;
                    grid-template-areas: 
                    'seta nome edit-btn remove-btn'
                    'edit edit edit edit'
                    'content content content content';
                }

                .event-title {
                    background: rgb(41,41,41);
                    background: linear-gradient(0deg, rgba(41,41,41,1) 0%, rgba(101,118,125,1) 0%, rgba(169,183,222,1) 52%);
                    padding: 0.2em 0.7em;
                }

                .table {
                    display: inline-block;
                    margin: auto;
                }

                .title-table {
                    margin-top: 1em;
                    display: grid;
                    justify-content: center;
                    grid-template-columns: 20px 100px 100px 100px 200px;
                    
                }

                .title-table > text + text {
                    padding-left: 0.7em;
                }

                .round {
                    margin-top: 1em;
                    margin-left: 2em;
                    margin-right: 2em;
                    padding-bottom: 1.0em;
                    border-bottom: 2px ridge black;
                }

                .title-round {
                    margin-top: 1.2em;
                    display: grid;
                    justify-content: flex-start;
                    align-items: center;
                    grid-template-columns: 150px 200px auto auto;
                }

                .title-round > text + text {
                    padding-left: 0.7em;
                }

                .item {
                    margin-top: 1em;
                    margin-left: 2em;
                    margin-right: 2em;
                    padding-bottom: 1.0em;
                    border-bottom: 2px ridge black;
                }

                .title-item {
                    margin-top: 1.2em;
                    display: grid;
                    justify-content: flex-start;
                    align-items: center;
                    grid-template-columns: 100px 60px 100px 60px 200px auto auto;
                }

                .title-item > text + text {
                    padding-left: 0.7em;
                }

                .anchor {
                    margin-top: 1em;
                    margin-right: 0.3em;
                    padding: 0.2em;
                    cursor: pointer;
                }

                .edit-event-btn {
                    margin-top: 1em;
                    margin-left: 0.3em;
                    padding: 0.2em;
                    cursor: pointer;
                    font-weight: bold;
                    min-width: 50px;
                    text-align: center;
                }

                .remove-btn {
                    margin-top: 1em;
                    margin-left: 0.3em;
                    padding: 0.2em;
                    min-width: 70px;
                    cursor: pointer;
                }

                .g1 {
                    grid-area: seta;
                }
                .g2 {
                    grid-area: nome;
                }
                .g3 {
                    grid-area: content;
                }
                .g4 {
                    grid-area: edit-btn;
                }
                .g5 {
                    grid-area: edit;
                }
                .g6 {
                    grid-area: remove-btn;
                }

                .title-content {
                    color: cornflowerblue;
                    font-weight: bolder;
                    font-size: 1.5em;
                }

                .noselect {
                    -webkit-touch-callout: none; /* iOS Safari */
                    -webkit-user-select: none; /* Safari */
                    -khtml-user-select: none; /* Konqueror HTML */
                    -moz-user-select: none; /* Old versions of Firefox */
                    -ms-user-select: none; /* Internet Explorer/Edge */
                    user-select: none; /* Non-prefixed version, currently
                                        supported by Chrome, Edge, Opera and Firefox */
                }

                .btn_add {
                    display: flex;
                    justify-content: space-evenly;
                    margin-top: 1.0em;
                }

                .btn_add button {
                    border: 2px inset lightgray;
                    background-color: cornflowerblue;
                    padding: 0.3em 0.6em;
                    cursor: pointer;
                    color: white;
                    font-weight: bold;
                }

                .btn_add button:hover {
                    background-color: rgb(65, 122, 228);
                }

                .edit-event {
                    justify-content: center;
                    margin-top: 0.7em;
                    border-bottom: 2px ridge black;
                    padding-bottom: 1.2em;
                }

                .edit-event form {
                    display: grid;
                }

                .edit-event div {
                    justify-content: center;
                    display: flex;
                }

                .edit-event div > text, input, select {
                    flex: 1 0 100%;
                    margin: 0.5em;
                }

                .edit-event div > input, select {
                    padding-left: 0.2em;
                    border: 1px solid darkgray;
                }

                .edit-event div > input[type=submit] {
                    border: 2px inset lightgray;
                    background-color: cornflowerblue;
                    color: white;
                    font-weight: bold;
                    cursor: pointer;
                    padding: 0.1em 0;
                }

                .edit-event div > input[type=submit]:hover {
                    background-color: rgb(65, 122, 228);
                }

                .edit-item-btn {
                    cursor: pointer;
                    margin-left: 0.5em;
                    border-left: 1px solid black;
                }

                .remove-item-btn input {
                    margin: 0;
                    padding: 0;
                    background-color: transparent;
                    font-size: 0.9em;
                    border-left: 1px solid black;
                    padding-left: 0.5em;
                    cursor: pointer;
                }

                .remove-btn form {
                    display: flex;
                    justify-content: center;
                    cursor: pointer;
                }

                .remove-btn form > input {
                    margin: 0;
                    padding: 0;
                    background-color: transparent;
                    max-width: 70px;
                    font-weight: bold;
                    cursor: pointer;
                }

                .add-event {
                    display: grid;
                    justify-content: center;
                    margin-top: 1.5em;
                    text-align: center;
                }

                .title-add {
                    display: grid;
                    grid-auto-flow: column;
                    margin-top: 1.5em;
                }

                .add-event input, select {
                    border: 1px solid darkgray;
                    padding: 0.1em 0.2em;
                }

                .add-event input[type=submit] {
                    border: 2px inset lightgray;
                    color: white;
                    background-color: cornflowerblue;
                    cursor: pointer;
                    padding: 0.6em 3em;
                    margin-top: 1.5em;
                    font-weight: bold;
                }

                .add-event select {
                    padding: 0 0.2em;
                    min-width: 148px;
                }

                .add-event input[type=submit]:hover {
                    background-color: rgb(65, 122, 228);
                }

                .sub-title-item {
                    font-size: 1.7em;
                    font-weight: bold;
                    display: block;
                    color: cornflowerblue;
                    text-align: left;
                    margin-left: 0.3em;
                    margin-top: 0.5em;
                }

                .sub-title-item-plus {
                    font-size: 1.9em;
                    font-weight: 900;
                    color: cornflowerblue;
                    text-align: left;
                    margin-right: 0.3em;
                    margin-top: 0.5em;
                    cursor: pointer;
                    display: flex;
                    flex-direction: row;
                    justify-content: space-around;
                    flex-wrap: wrap;
                    min-width: 50px;
                }

                .sub-title-item-plus i:hover {
                    color: rgb(65, 122, 228);
                }

                .sub-title-item-plus :first-child {
                    position: relative;
                    top: -3px;
                }

                .sub-title {
                    display: flex;
                    justify-content: space-between;
                }

                .btn-add-event {
                    border: 2px inset lightgray;
                    background-color: cornflowerblue;
                    padding: 0.3em 0.6em;
                    cursor: pointer;
                    color: white;
                    font-weight: bold;
                }

                .btn-add-event:hover {
                    background-color: rgb(65, 122, 228);
                }

                .div-message {
                    display: flex;
                    justify-content: space-between;
                    background-color: white;
                    font-size: 1.5em;
                    font-weight: bold;
                }

                .div-message > text, i {
                    padding: 0.7em;
                }

                .message-label {
                    color: green;
                }

                .error-label {
                    color: red;
                }

                .message-cross {
                    cursor: pointer;
                    color: darkgrey;
                }

                .message-cross:hover {
                    color: grey;
                }

                .label-event {
                    display: flex;
                    justify-content: center;
                    margin-top: 0.7em;
                    border-bottom: 2px ridge black;
                    padding-bottom: 1.2em;
                }

                .label-event form {
                    display: grid;
                    width: 80%;
                }

                .label-event div {
                    justify-content: space-between;
                    display: grid;
                    grid-template-columns: auto auto;
                }

                .label-event div > text {
                    margin: 0.5em;
                }

                .sub-title-table {
                    display: flex;
                    justify-content: center;
                    font-size: 2em;
                    font-weight: bold;
                    color: darkgreen;
                }
            ");

            $head->addChild($style);

            $document->addChild($head);

            // Body
            $body = new DynamicTag('body');
            $body->style = "width: 100%; height: 100%; background-color: #777";

            $body->addChild('<div class="content">
            <div class="intro">
                <font size="20em">Block Word Register Tool</font>
            </div>
            <!-- WORD -->
            <div class="table">
            <!-- Message in first table -->
            <div class="div-message">'.$this->showError().'</div>
                <div class="sub-title-table">Word</div>
                <div class="title-table" style="margin-left: 2.2em; justify-content: flex-start;">
                    <text>ID</text>
                    <text>WORD</text>
                </div>'.(function(){

                    $ret = '';

                    foreach ($this->checker->getAllWord() as $index => $word) {

                        $ret .= '<div class="event">
                            <i class="anchor event-title g1 noselect" onClick="toggle(this)">▼</i>
                            <div class="title-table event-title g2">
                                <text>'.$index.'</text>
                                <text>'.$word.'</text>
                            </div>
                            <div class="g4 event-title edit-event-btn noselect" onClick="editToggle(this)">Edit</div>
                            <div class="g6 event-title remove-btn noselect">
                                <form onsubmit="return confirm(\'Do you want remove this word?\')" action="?" method="POST">
                                    <input type="hidden" name="remove-word" value="remove">
                                    <input type="hidden" name="id" value="'.$index.'">
                                    <input type="submit" value="Remove">
                                </form>
                            </div>
                            <div class="g5 edit-event" style="display: none">
                                <form action="?" method="POST">
                                    <div>
                                        <input type="hidden" name="edit-word" value="edit">
                                        <input type="hidden" name="id" value="'.$index.'">
                                    </div>
                                    <div>
                                        <text>WORD</text><input type="text" name="word" value="'.$word.'">
                                    </div>
                                    <div>
                                        <input type="submit" value="Editar">
                                    </div>
                                </form>
                            </div>
                            <div class="event-content g3" style="display: none">'.
                            (function() use($index, $word) {
                                
                                return '<div class="label-event">
                                    <form action="?" method="POST">
                                        <div>
                                            <text>ID</text><text>'.$index.'</text>
                                        </div>
                                        <div>
                                            <text>WORD</text><text>'.$word.'</text>
                                        </div>
                                    </form>
                                </div>';
                            }
                            )().'
                            </div>
                        </div>';
                    }

                    return $ret;
                })().'</div>
                    <div style="display: flex; justify-content: center; margin-top: 1.5em; margin-bottom: 1.5em;">
                        <button onClick="addEvent(this)" class="btn-add-event">Adicionar Word</button>
                    </div>
                    <div style="display: none; justify-content: center; margin-bottom: 1.5em;">
                        <div class="edit-event" style="border: none">
                            <div style="display: flex; justify-content: center; font-size: 1.5em; font-weight: bold; margin-bottom: 1em;">Adicionar Word</div>
                            <form action="?" method="POST">
                                <div>
                                    <input type="hidden" name="add-word" value="add">
                                </div>
                                <div>
                                    <text>WORD</text><input type="text" name="word" value="">
                                </div>
                                <div>
                                    <input type="submit" value="Adicionar">
                                </div>
                            </form>
                        </div>
                    </div>
                <!-- IP -->
                <div class="table">
                    <div class="sub-title-table">IP</div>
                    <div class="title-table" style="margin-left: 2.2em; justify-content: flex-start;">
                        <text>ID</text>
                        <text>IP</text>
                    </div>'.(function(){
    
                        $ret = '';
    
                        foreach ($this->checker->getAllIP() as $index => $ip) {
    
                            $ret .= '<div class="event">
                                <i class="anchor event-title g1 noselect" onClick="toggle(this)">▼</i>
                                <div class="title-table event-title g2">
                                    <text>'.$index.'</text>
                                    <text>'.$ip.'</text>
                                </div>
                                <div class="g4 event-title edit-event-btn noselect" onClick="editToggle(this)">Edit</div>
                                <div class="g6 event-title remove-btn noselect">
                                    <form onsubmit="return confirm(\'Do you want remove this ip?\')" action="?" method="POST">
                                        <input type="hidden" name="remove-ip" value="remove">
                                        <input type="hidden" name="id" value="'.$index.'">
                                        <input type="submit" value="Remove">
                                    </form>
                                </div>
                                <div class="g5 edit-event" style="display: none">
                                    <form action="?" method="POST">
                                        <div>
                                            <input type="hidden" name="edit-ip" value="edit">
                                            <input type="hidden" name="id" value="'.$index.'">
                                        </div>
                                        <div>
                                            <text>IP</text><input type="text" name="ip" value="'.$ip.'">
                                        </div>
                                        <div>
                                            <input type="submit" value="Editar">
                                        </div>
                                    </form>
                                </div>
                                <div class="event-content g3" style="display: none">'.
                                (function() use($index, $ip) {
                                    
                                    return '<div class="label-event">
                                        <form action="?" method="POST">
                                            <div>
                                                <text>ID</text><text>'.$index.'</text>
                                            </div>
                                            <div>
                                                <text>IP</text><text>'.$ip.'</text>
                                            </div>
                                        </form>
                                    </div>';
                                }
                                )().'
                                </div>
                            </div>';
                        }
    
                        return $ret;
                    })().'</div>
                    <div style="display: flex; justify-content: center; margin-top: 1.5em; margin-bottom: 1.5em;">
                        <button onClick="addEvent(this)" class="btn-add-event">Adicionar IP</button>
                    </div>
                    <div style="display: none; justify-content: center; margin-bottom: 1.5em;">
                        <div class="edit-event" style="border: none">
                            <div style="display: flex; justify-content: center; font-size: 1.5em; font-weight: bold; margin-bottom: 1em;">Adicionar IP</div>
                            <form action="?" method="POST">
                                <div>
                                    <input type="hidden" name="add-ip" value="add">
                                </div>
                                <div>
                                    <text>IP</text><input type="text" name="ip" value="">
                                </div>
                                <div>
                                    <input type="submit" value="Adicionar">
                                </div>
                            </form>
                        </div>
                    </div>
                </div>');

            $document->addChild($body);

            echo '<!DOCTYPE html>'.$document->toString();
        }
    }

    // Block Word Register
    $block = new BlockWordRegister();
    $block->show();
?>