<?php
    // Arquivo stream_monitor.php
    // Criado em 25/04/2021 as 15:50 por Acrisio
    // Ferramenta que mostra o Monitor de Stream dos players

    include_once("source/gm_tools_base.inc");

    class StreamMonitor extends GMTools {

        protected function getName() {
            return "Stream Monitor";
        }

        public function __construct() {

            $this->checkLoginAndCapacity();
        }

        public function show() {

            echo '<!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <title>Monitor Pangya Live Stream</title>
            
                <style>
                    .image-label {
                        text-align: center;
                        color: red;
                        font-size: 20px;
                        font-weight: bold;
                    }
            
                    .image-view {
                        margin: 5px;
                        width: 200px;
                    }
            
                    .division {
                        display: flex;
                        border-bottom: 2px dashed black;
                    }
            
                    .division > div {
                        margin-left: 10px;
                        font-size: 25px;
                        font-weight: bold;
                    }
                </style>
            </head>
            <body>
                <!-- Image container template -->
                <div id="image-template" style="display: none;">
                    <div class="image-label">#</div>
                    <img class="image-view" src="">
                </div>
                <!-- Focus div-->
                <div id="div-focus" style="display: flex; justify-content: center; width: 100%;">
                    <div>
                        <div class="image-label" id="focus-label" style="margin-bottom: 5px;">#</div>
                        <img src="" value="0" style="width: 640px">
                    </div>
                </div>
                <div class="division">
                    <div># Ao vivo</div>
                </div>
                <div id="div-mozaic-live" style="display: flex; margin: 5px; flex-wrap: wrap;">
                    <!-- all images -->
                </div>
                <div class="division">
                    <div># Parado</div>
                </div>
                <div id="div-mozaic-stop" style="display: flex; margin: 5px; flex-wrap: wrap;">
                    <!-- all images -->
                </div>
            <script>
            
                const HOST_WEBSHOT = \'superss.ga\';
                const PORT_WEBSHOT = 4573;
                const min_width = 200;
                const focus_width = 640;
                const IMAGE_FOCUS = document.querySelector(\'#div-focus img\');
                const MOZAIC_LIVE = document.getElementById(\'div-mozaic-live\');
                const MOZAIC_STOP = document.getElementById(\'div-mozaic-stop\');
                const IMAGE_TEMPLATE = document.getElementById(\'image-template\');
            
                const HTML_SPECIAL_CHARS_MAP = {
                    \'&\': \'&amp;\',
                    \'<\': \'&lt;\',
                    \'>\': \'&gt;\',
                    \'"\': \'&quot;\',
                    "\'": \'&#039;\'
                };
            
                function escapeHtml(text) {
                    return text.replace(/[&<>"\']/g, function(m) { return HTML_SPECIAL_CHARS_MAP[m]; });
                }
            
                function setFocus(element, type = 4) {
            
                    const state = `FOCUS-${type}`;
            
                    if (!element || element.getAttribute(\'state\') == state)
                        return;
            
                    element.style.border = "3px solid red";
                    element.setAttribute(\'state\', state);
            
                    // Focus
                    socket.send(JSON.stringify({
                        type: state,
                        id: element.id.replace(\'image-\', \'\')
                    }));
                }
            
                function setMiniFrame(element) {
            
                    if (!element || element.getAttribute(\'state\') == \'MINI\')
                        return;
            
                    element.style.border = \'none\';
                    element.setAttribute(\'state\', \'MINI\');
            
                    // MINI-FRAME
                    socket.send(JSON.stringify({
                        type: \'MINI-FRAME\',
                        id: element.id.replace(\'image-\', \'\')
                    }));
                }
            
                const socket = new WebSocket(`ws://${HOST_WEBSHOT}:${PORT_WEBSHOT}`);
                socket.onopen = (evt) => {
                    console.log(`Connected: ${evt}`);
                };
                socket.onmessage = (image) => {
            
                    const evt = JSON.parse(image.data);
            
                    const id = `image-${evt.id}`;
                    
                    if (IMAGE_FOCUS.getAttribute(\'value\') == id) {
            
                        if (evt.frame)
                            IMAGE_FOCUS.src = `data:image/jpeg;base64,${evt.frame}`;
            
                        if (evt.nickname)
                            IMAGE_FOCUS.parentElement.children[0].innerHTML = evt.nickname == \'\' ? \'#\' : escapeHtml(evt.nickname);
                    }
            
                    let imageElement = document.getElementById(id);
            
                    if (!imageElement) {
                        
                        // Faz nada é fechamento
                        if (evt.type == 3 && evt.frame == null && evt.close)
                            return;
            
                        let img_container = IMAGE_TEMPLATE.cloneNode(true);
            
                        img_container.id = undefined;
                        img_container.style.display = "block";
            
                        if (evt.nickname)
                            img_container.children[0].innerHTML = evt.nickname == \'\' ? \'#\' : escapeHtml(evt.nickname);
                        
                        imageElement = img_container.children[1];
            
                        imageElement.id = id;
                        imageElement.setAttribute(\'state\', \'MINI\');
            
                        imageElement.addEventListener(\'click\', function(mouse_evt) {
            
                            // Close
                            if (mouse_evt.ctrlKey && mouse_evt.shiftKey) {
            
                                // Close
                                socket.send(JSON.stringify({
                                    type: \'CLOSE\',
                                    id: this.id.replace(\'image-\', \'\')
                                }));
            
                                // Replace first child from mozaic
                                if (IMAGE_FOCUS.getAttribute(\'value\') == this.id) {
            
                                    const childs = MOZAIC_LIVE.querySelectorAll(\'img\');
            
                                    if (childs.length >= 1) {
            
                                        IMAGE_FOCUS.setAttribute(\'value\', childs[0].id);
                                        IMAGE_FOCUS.src = childs[0].src;
                                        IMAGE_FOCUS.parentElement.children[0].innerHTML = childs[0].parentElement.children[0].innerHTML;
            
                                        // !@ não seta focus no server(4x por segundo, deixa 1 por segundo)
                                        //setFocus(childs[0]);
            
                                    }else {
            
                                        // Clear
                                        IMAGE_FOCUS.setAttribute(\'value\', 0);
                                        IMAGE_FOCUS.src = "";
                                        IMAGE_FOCUS.parentElement.children[0].innerHTML = \'#\';
                                    }
                                }
            
                            }else if (mouse_evt.shiftKey) { // Toggle Stop/Play Send Frames
            
                                // Toggle
                                if (MOZAIC_LIVE.contains(this.parentElement))
                                    MOZAIC_STOP.appendChild(this.parentElement);
                                else
                                    MOZAIC_LIVE.appendChild(this.parentElement);
            
                                // Stop/Play
                                socket.send(JSON.stringify({
                                    type: \'TOGGLE-SEND-FRAMES\',
                                    id: this.id.replace(\'image-\', \'\')
                                }));
            
                            }else if (mouse_evt.altKey) { // Toggle DirectX/GDI+ Frames capture
            
                                // Toggle DirectX/GDI+ Frames capture
                                socket.send(JSON.stringify({
                                    type: \'TOGGLE-CAPTURE\',
                                    id: this.id.replace(\'image-\', \'\')
                                }));
            
                            }else {
            
                                IMAGE_FOCUS.setAttribute(\'value\', this.id);
                                IMAGE_FOCUS.src = this.src;
                                IMAGE_FOCUS.parentElement.children[0].innerHTML = this.parentElement.children[0].innerHTML;
            
                                if (mouse_evt.detail > 1) // Mais de uma click rápido tira o foco
                                    setMiniFrame(this);
                                else
                                    setFocus(this, (mouse_evt.ctrlKey) ? 10 : 4);
            
                                document.querySelectorAll(\'img\').forEach((el) => {
            
                                    // Clear
                                    if (el != this)
                                        setMiniFrame(el);
            
                                });
                            }
            
                        }.bind(imageElement));
                        
                        MOZAIC_LIVE.appendChild(imageElement.parentElement);
                        //MOZAIC_LIVE.appendChild(img_container);
                    
                    }else if (evt.type == 3 && evt.frame == null && evt.close) {
            
                        MOZAIC_LIVE.removeChild(imageElement.parentElement);
            
                        // Replace first child from mozaic
                        if (IMAGE_FOCUS.getAttribute(\'value\') == imageElement.id) {
            
                            const childs = MOZAIC_LIVE.querySelectorAll(\'img\');
            
                            if (childs.length >= 1) {
            
                                IMAGE_FOCUS.setAttribute(\'value\', childs[0].id);
                                IMAGE_FOCUS.src = childs[0].src;
                                IMAGE_FOCUS.parentElement.children[0].innerHTML = childs[0].parentElement.children[0].innerHTML;
            
                                // !@ não seta focus no server(4x por segundo, deixa 1 por segundo)
                                //setFocus(childs[0]);
                                
                            }else {
            
                                // Clear
                                IMAGE_FOCUS.setAttribute(\'value\', 0);
                                IMAGE_FOCUS.src = "";
                                IMAGE_FOCUS.parentElement.children[0].innerHTML = \'#\';
                            }
                        }
            
                        // removed child
                        return;
                    }
            
                    if (evt.frame)
                        imageElement.src = `data:image/jpeg;base64,${evt.frame}`;
            
                    if (evt.nickname)
                        imageElement.parentElement.children[0].innerHTML = evt.nickname == \'\' ? \'#\' : escapeHtml(evt.nickname);
            
                    // First Player
                    if (IMAGE_FOCUS.getAttribute(\'value\') == 0) {
            
                        IMAGE_FOCUS.setAttribute(\'value\', imageElement.id);
                        IMAGE_FOCUS.src = imageElement.src;
                        IMAGE_FOCUS.parentElement.children[0].innerHTML = imageElement.parentElement.children[0].innerHTML;
            
                        // !@ não seta focus no server(4x por segundo, deixa 1 por segundo)
                        //setFocus(imageElement);
                    }
            
                };
            
            
            </script>
            </body>
            </html>';
        }
    }

    // Stream Monitor
    $monitor = new StreamMonitor();
    $monitor->show();
?>