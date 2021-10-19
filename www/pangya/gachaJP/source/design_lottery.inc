<?php
    // Arquivo design_lottery.inc
    // Criado em 24/05/2020 as 02:22 por Acrisio
    // Definição e Implementação da classe statica Design

    include_once('player_singleton.inc');
    include_once('config.inc');
    include_once('debug_log.inc');

    // tem que ser sem o https, só a pagina gacha_whats.php, por que se colocar o http o link todo ele reseta a session
    const LINKS = [ 'LOGIN_ERROR' => BASE_GACHA_URL.'unknown_error.html',
                    'CURRENT_INDEX' => BASE_GACHA_URL.'current/gacha_item.php',
                    'LIST_ITEM' => /*BASE_GACHA_URL.*/'list_item.php',
                    'GACHA_WHATS' => /*BASE_GACHA_URL.*/'gacha_whats.php',
                    'GACHA_LOTTERY' => /*BASE_GACHA_URL.*/'gacha_lottery.php',
                    'UNKNOWN_ERROR' => BASE_GACHA_URL.'unknown_error.html'];

    class DesignLottery {

        static public function checkIE() {
            
            if (stristr($_SERVER['HTTP_USER_AGENT'], 'MSIE 7.0;') == null 
                || stristr($_SERVER['HTTP_USER_AGENT'], 'compatible;') == null
                || !preg_match("/Trident\/[345678]\.[01];/", $_SERVER['HTTP_USER_AGENT'])) {

                DebugLog::Log("Bad navigator request. (".$_SERVER['HTTP_USER_AGENT'].")");

                // Redireciona para a página de erro
                header("Location: ".$_SERVER['REQUEST_SCHEME'].'://'.NAME_SITE.':'.$_SERVER['SERVER_PORT'].'/pangya.php');

                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        static public function checkLogin() {

            // Verifica se o player esta conectado
            if (PlayerSingleton::getInstance() == null || !PlayerSingleton::getInstance()['logged']) {
                
                DebugLog::Log("Not have session, request page without login.");

                // Redireciona para a página de erro
                header("Location: ".LINKS['LOGIN_ERROR']);

                // sai do script para o navegador redirecionar para a página
                exit();
            }
        }

        static public function beginHTML() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();
            
            echo '<!DOCTYPE html>';
            echo '<html lang="pt-br">';
        }

        static public function endHTML() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</html>';
        }

        static public function beginHead() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '<head>';

            echo '<meta content="IE=11.0000" http-equiv="X-UA-Compatible">';

            echo '<meta charset="UTF-8">';

            echo '<script type="text/javascript" src="./include/Stop_KeyFunction.js"></script>';

            echo '<meta content="no-cache" http-equiv="pragma">';
            echo '<meta content="no-store" http-equiv="cache-control">';
            echo '<meta content="no-cache" http-equiv="cache-control">';
            echo '<meta content="no" http-equiv="Imagetoolbar">';

            // !@ -- Gacha JS 2021, flash foi desativado pela adobe
            echo '<script src="./include/createjs.1.0.0.min.js"></script>';
            echo '<script src="./include/pako.min.js"></script>';

            echo '<script type="text/javascript">
                var canvas, stage, exportRoot, anim_container, dom_overlay_container, fnStartAnimation;
                var frames_resources_count = 0, loaderPercent = 0, end_frame = 3643;
                window.onerror = function(message, file, line, col, error) {
                    if (error.number == -2147024882)
                        document.location.href = "./gacha_result.php";
                    return true;
                };
                function init() {
                    var rq = new XMLHttpRequest();
                    rq.addEventListener("error", function(evt){document.location.href = "./gacha_result.php"});
                    rq.addEventListener("abort", function(evt){document.location.href = "./gacha_result.php"});
                    rq.addEventListener("load", function(evt){
                
                        if (evt === undefined) {
                
                            document.location.href = "./gacha_result.php"
                
                            return;
                        }
                
                        if (evt.target.readyState != XMLHttpRequest.DONE || evt.target.status != 200) {
                
                                document.location.href = "./gacha_result.php"
                
                            return;
                        }
                
                        if (evt.target.response === undefined || evt.target.response === null) {
                
                            document.location.href = "./gacha_result.php"
                
                            return;
                        }
                
                        var response = pako.inflate(evt.target.response, {to: \'string\'});
                
                        var script = document.createElement("script");
                        script.innerHTML = response;
                
                        document.body.appendChild(script);
                
                        // Init
                        init2();
                    });
                
                    rq.open("GET", "./include/animation.js");
                
                    rq.responseType = "arraybuffer";
                    rq.send(null);
                }
                function init2() {
                    canvas = document.getElementById("canvas");
                    anim_container = document.getElementById("animation_container");
                    dom_overlay_container = document.getElementById("dom_overlay_container");
                    var comp=AdobeAn.getComposition("C8415CD5DC890249BB4A3EB7872843A5");
                    var lib=comp.getLibrary();
                    var loader = new createjs.LoadQueue(false);
                    loader.installPlugin(createjs.Sound);
                    loader.addEventListener("fileload", function(evt){handleFileLoad(evt,comp)});
                    loader.addEventListener("complete", function(evt){handleComplete(evt,comp)});
                    var lib=comp.getLibrary();
                
                    setLoading("Resources", loaderPercent / lib.properties.manifest.length)
                
                    loader.loadManifest(lib.properties.manifest);
                }
                function handleFileLoad(evt, comp) {
                    loaderPercent++;
                
                    var images=comp.getImages();	
                    if (evt && (evt.item.type == "image")) { images[evt.item.id] = evt.result; }
                    
                    setLoading("Resources", loaderPercent / AdobeAn.getComposition("C8415CD5DC890249BB4A3EB7872843A5").getLibrary().properties.manifest.length);
                }
                function handleComplete(evt,comp) {
                
                    //This function is always called, irrespective of the content. You can use the variable "stage" after it is created in token create_stage.
                    var lib=comp.getLibrary();
                    var ss=comp.getSpriteSheet();
                    var queue = evt.target;
                    var ssMetadata = lib.ssMetadata;
                    for(i=0; i<ssMetadata.length; i++) {
                        ss[ssMetadata[i].name] = new createjs.SpriteSheet( {"images": [queue.getResult(ssMetadata[i].name)], "frames": ssMetadata[i].frames} )
                    }
                
                    setTimeout(function() {
                        exportRoot = new lib.gacha_p2();
                        stage = new lib.Stage(canvas);
                        stage.enableMouseOver();	
                        //Registers the "tick" event listener.
                        fnStartAnimation = function() {
                            stage.addChild(exportRoot);
                            createjs.Ticker.framerate = lib.properties.fps;
                            createjs.Ticker.addEventListener("tick", stage);
                        }	    
                        //Code to support hidpi screens and responsive scaling.
                        AdobeAn.makeResponsive(false,\'both\',false,1,[canvas,anim_container,dom_overlay_container]);	
                        AdobeAn.compositionLoaded(lib.properties.id);
                        fnStartAnimation();
                    }, 100);
                }
                function playSound(id, loop, offset) {
                    return createjs.Sound.play(id, {\'interrupt\':createjs.Sound.INTERRUPT_EARLY, \'loop\': loop, \'offset\': offset});
                }
                function setLoading(title, percent) {
                
                    var percent_str = title + " " + (percent * 100).toFixed(0) + "%";
                
                    var canvas2 = document.getElementById("canvas2");
                    var cc = canvas2.getContext("2d");
                    var gd = cc.createLinearGradient(0, 0, 0, 500);
                    gd.addColorStop(0, \'cyan\');
                    gd.addColorStop(1, \'green\');
                
                    var wid = 800 * 0.8;
                    var hght = 50;
                    var obj = {
                        x: (800-wid)/2,
                        y: (500-hght)/2,
                        width: wid,
                        height: hght,
                        border: 5,
                        sizeText: 35
                    }
                
                    cc.fillStyle = gd;
                    cc.fillRect(0, 0, 800, 500);
                
                    cc.strokeStyle = \'black\';
                    cc.lineWidth = obj.border;
                    cc.lineJoin = "round";
                    cc.strokeRect(obj.x-obj.border/2, obj.y-obj.border/2, obj.width, obj.height);
                
                    gd = cc.createLinearGradient(obj.x, 0, obj.x + obj.width, 0);
                    gd.addColorStop(0, \'darkblue\');
                    gd.addColorStop(1, \'blue\');
                
                    cc.fillStyle = gd;
                
                    cc.fillRect(obj.x, obj.y, (obj.width-obj.border) * percent, obj.height-obj.border);
                
                    cc.font = obj.sizeText + "px serif";
                    cc.textBaseline = "top";
                
                    var length = cc.measureText(percent_str).width
                
                    cc.fillStyle = "white";
                    cc.fillText(percent_str, obj.x + (obj.width/2-length/2), obj.y + (obj.height/2-(obj.sizeText+obj.border)/2));
                }
            </script>';

            echo '<script type="text/javascript">
                function writeGacha() {
                    document.write(\'<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" codebase="//fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,0,0" width="800" height="500" id="gacha_ver5" align="middle"><param name="allowScriptAccess" value="sameDomain" /><param name="movie" value="./gacha_p2.swf" /><param name="menu" value="false" /><param name="quality" value="high" /><param name="bgcolor" value="#ffffff" /><param name="wmode" value="window" /><embed src="./gacha_p2.swf" menu="false" quality="high" wmode="window" bgcolor="#ffffff" width="800" height="500" name="gacha_ver" align="middle" allowScriptAccess="sameDomain" type="application/x-shockwave-flash" pluginspage="//www.macromedia.com/go/getflashplayer" /></object>\');
                }
            </script>';
            echo '<link rel="stylesheet" type="text/css" href="./include/base.css">';
        }

        static public function endHead() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</head>';
        }

        static public function beginBody() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '<body>';
        }

        static public function endBody() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();

            echo '</body>';
        }

        static public function beginContent() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();
        }

        static public function endContent() {

            // Verifica se está com navegador correto(acessando pelo ProjectG)
            self::checkIE();

            // Verifica se o player esta conectado
            self::checkLogin();
        }
    }
?>