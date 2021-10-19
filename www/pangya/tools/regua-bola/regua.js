// Arquivo regua.js
// Criado em 29/05/2020 as 03:56 por Acrisio
// Definição e Implementação da Régua da bola em canvas

// debug
var debug = false;

// DOM Elements
var checkAutoFit = document.getElementById("CheckAutoFit");
var checkCenterHitBar = document.getElementById("CheckCenterHitBar");
var spanSpinReal = document.getElementById("SpanSpinReal");
var spanCurveReal = document.getElementById("SpanCurveReal");
var autoFitWarning = document.getElementById("AutoFitWarning");
var spanAutoFitOnOff = document.getElementById("SpanAutoFitOnOff");
var spanCenterHitBar = document.getElementById("SpanCenterHitBar");
var selectConfig = document.getElementById("SelectConfig");
var checkEditPositionWindow = document.getElementById("CheckEditPositionWindow");

// Storege Configuração
const KeyName = "Regua-Bola-Acrisio-SuperSS-Dev-Config";

// Warning Auto Fit Resolution
const WARNING_AUTO_FIT_RESOLUTION = "Auto Fit nessa resolução o spin e a curva não fica perfeitos no zero";
const EMPTY_CONFIG_SAVED_MSG = "Nenhuma configuração salva";

// Valores constantes
const BALL_GUI_RESOLUTION = 480;

// Auto Fit Spin e Curve pixel perfect
const AUTO_FIT_PERFECT_PIXEL = 240;

// Raio da bola padrão sem o auto fit
const STANDARD_BALL_RAIO = 30;

// Padding Canvas
const CANVAS_PADDING = 20;

// Ponto que o a pessoa quer
var pointer = {
    x: 0,
    y: 0
};

// Ponto que a pessoa está
var clientPoint = {
    x: 0,
    y: 0
}

var resolution = null;
var houver = false;

// Verifica se tem no storage a chave
var config = JSON.parse(localStorage.getItem(KeyName));

if (debug)
    console.log("Local Storage\"" + KeyName + "\": " + config);

if (config == null)
    config = [];

// Init
window.addEventListener("load", init);

function init() {
    
    var url = new URL(document.location.href);

    resolution = {
        width: parseInt(url.searchParams.get("w")),
        height: parseInt(url.searchParams.get("h")),
        auto_fit_aviso: false,
        cHeight: 0,
        cWidth: 0,
        cPadding: 0,
        pointer_raio: 0,
        proportion: 0,
        center: {
            x: 0,
            y: 0
        },
        init() {

            // Proporção
            this.proportion = (this.height / BALL_GUI_RESOLUTION);

            // Aviso de que o auto fit nessa resolução o spin e a curva nunca vai da o valor 0 certos
            if (this.height % AUTO_FIT_PERFECT_PIXEL != 0)
                this.auto_fit_aviso = true;

            // Canvas padding
            this.cPadding = Math.round(CANVAS_PADDING * this.proportion);

            // Canvas size
            this.cWidth = (Math.round(this.proportion * STANDARD_BALL_RAIO * 2) + this.cPadding * 2);
            this.cHeight = (Math.round(this.proportion * STANDARD_BALL_RAIO * 2) + this.cPadding * 2);

            // Calcule center of canvas
            this.center.x = Math.floor(this.cWidth / 2);
            this.center.y = Math.floor(this.cHeight / 2);

            // Aviso auto fit
            if (this.auto_fit_aviso)
                autoFitWarning.innerHTML = WARNING_AUTO_FIT_RESOLUTION;

            // Ball Center
            this.pointer_raio = Math.round(this.cPadding / 4) * this.getProportion();
        },
        getRaioByProportion() {
            return this.getProportion() * STANDARD_BALL_RAIO;
        },
        getProportion() {

            if (!checkAutoFit.checked)
                return Math.floor(this.proportion);
            return this.proportion;
        },
        getCenterByProportion() {

            var center = {
                x: this.center.x + 82,
                y: this.center.y + 58
            }

            var proportion = this.getProportion();

            var width_center = 140 * proportion;

            if (checkCenterHitBar.checked)
                width_center = ((this.width * 0.5) - (180 * proportion));

            center.x = (width_center + 82) - (82 * proportion);
            center.y = center.y - (58 * proportion);

            // Correção do centro visual
            if (center.x != this.center.x)
                center.x += Math.floor(this.center.x - center.x);

            // Correção do centro visual
            if (center.y != this.center.y)
                center.y += Math.floor(this.center.y - center.y);

            return center;
        }
    };

    // Error
    if (resolution == null || resolution.width == null || resolution.height == null)
        return;
    
    // Inicializa os valores da resolução
    resolution.init();
    
    myCanvas = document.getElementById("myCanvas");
    ctx = myCanvas.getContext("2d");

    // Init canvas
    myCanvas.width = resolution.cWidth;
    myCanvas.height = resolution.cHeight;

    // Listeners
    initListeners();

    // init config saved
    initConfigSaved();

    // Pinta o primerio
    draw();
}

function initConfigSaved() {

    if (config != null && config.length > 0) {

        var first_el = false;
        var index = 0;

        config.forEach(el => {

            // Só mostra as posições salvas da mesma resolução
            if ((el.resolution.width * el.resolution.height) == (resolution.width * resolution.height)) {

                // Clear
                if (!first_el) {
                    
                    selectConfig.innerHTML = "";

                    first_el = true;
                }

                var option = document.createElement("option");

                option.id = index;

                option.innerHTML = el.resolution.width + "x" + el.resolution.height + (el.auto_fit ? " - Auto Fit" : "") + (el.center_hit_bar ? " - Center Hit Bar" : "");

                selectConfig.appendChild(option);
            }

            index++;
        });

        // Clear
        if (!first_el) {
                    
            selectConfig.innerHTML = "";

            first_el = true;
        }

        if (selectConfig.options.length == 0) {

            var option = document.createElement("option");

            option.id = "Empty";

            option.innerHTML = EMPTY_CONFIG_SAVED_MSG;

            selectConfig.appendChild(option);
        }

        // Primeiro elemento
        selectConfig.selectedIndex = 0;

        // Make Evento change para mudar a posição para o primeiro elemento
        selectConfig.dispatchEvent(new Event("change"));
    
    }else { // Empty

        // Clear
        if (!first_el) {
                    
            selectConfig.innerHTML = "";

            first_el = true;
        }

        if (selectConfig.options.length == 0) {

            var option = document.createElement("option");

            option.id = "Empty";

            option.innerHTML = EMPTY_CONFIG_SAVED_MSG;

            selectConfig.appendChild(option);
        }
    }

}

function savePosition() {

    var new_config = {
        resolution: {
            width: resolution.width,
            height: resolution.height
        },
        position: {
            left: window.screenX,
            top: window.screenY
        },
        auto_fit: checkAutoFit.checked,
        center_hit_bar: checkCenterHitBar.checked
    };

    config.unshift(new_config);

    // update config
    updateConfig();

    // Update visual
    initConfigSaved();
}

function deletePosition() {

    if (selectConfig.options.length > 0 && selectConfig.selectedIndex != -1) {

        var index = parseInt(selectConfig.options[selectConfig.selectedIndex].id);

        if (index != "Empty" && index < config.length) {

            config.splice(index, 1);

            // Atualiza config
            updateConfig();

            // Update visual
            initConfigSaved();
        }
    }
}

// Atualiza config no localStorage
function updateConfig() {

    if (config != null && config.length >= 0) {

        if (config.length == 0)
            localStorage.removeItem(KeyName);
        else
            localStorage.setItem(KeyName, JSON.stringify(config));
    }
}

function initListeners() {

    myCanvas.addEventListener("mousemove", function(e) {

        var pointer_pos = getPointerPosition();

        if (debug)
            console.log("Client X: " + e.clientX + " Y: " + e.clientY + " - Offset X: " + e.target.offsetLeft + " Y: " + e.target.offsetTop + " - Pointer X: " + pointer_pos.x + " Y: " + pointer_pos.y);

        // Update Client point
        clientPoint.x = (e.clientX - e.target.offsetLeft);
        clientPoint.y = (e.clientY - e.target.offsetTop);

        if (clientPoint.x == pointer_pos.x && clientPoint.y == pointer_pos.y)
            houver = true;
        else
            houver = false;

        // Update
        update();

        // Desenha
        draw();
    });

    // Check Auto Fit
    checkAutoFit.addEventListener("change", function(el) {

        update();

        draw();
    });

    // Check Center Hit Bar
    checkCenterHitBar.addEventListener("change", function(e) {

        update();

        draw();
    });

    // Change Config Saved
    selectConfig.addEventListener("change", function(el) {

        if (config.length <= 0)
            return;

        var config_filter = config.filter((cfg) => {
            return (cfg.resolution.width * cfg.resolution.height) == (resolution.width * resolution.height);
        });

        if (el.target.selectedIndex != -1 && el.target.selectedIndex < config_filter.length) {
            
            var index = el.target.selectedIndex;
            
            window.screenX = config_filter[index].position.left;
            window.screenY = config_filter[index].position.top;

            if (config_filter[index].auto_fit)
                checkAutoFit.checked = true;
            else
                checkAutoFit.checked = false;

            if (config_filter[index].center_hit_bar)
                checkCenterHitBar.checked = true;
            else
                checkCenterHitBar.checked = false;

            // Atualiza
            update();

            // Desenha
            draw();
        }

    });

    // Edit position window
    document.addEventListener("keydown", function(e) {

        if (checkEditPositionWindow.checked) {
            switch (e.keyCode) {
                case KeyboardEvent.DOM_VK_UP:
                    window.screenY -= 1;
                    break;
                case KeyboardEvent.DOM_VK_DOWN:
                    window.screenY += 1;
                    break;
                case KeyboardEvent.DOM_VK_LEFT:
                    window.screenX -= 1;
                    break;
                case KeyboardEvent.DOM_VK_RIGHT:
                    window.screenX += 1;
                    break;
            }
        }
    });
}

function draw() {

    var ball_position = getBallPosition();

    // Clear Canvas
    ctx.rect(0, 0, resolution.cWidth, resolution.cHeight);
    ctx.fillStyle = "gray";
    ctx.fill();

    if (debug) {
        ctx.rect(0, 0, resolution.cWidth, resolution.cHeight);
        ctx.strokeStyle = "black";
        ctx.stroke();
    }

    ctx.closePath();

    // Ball
    ctx.beginPath();

    ctx.arc(ball_position.x, ball_position.y, ball_position.raio, 0, 2*Math.PI);

    ctx.strokeStyle = "black";
    ctx.stroke();

    ctx.closePath();

    // Pointer ball
    if (!houver) {
        ctx.beginPath();

        ctx.arc(ball_position.x + pointer.x, ball_position.y + pointer.y, resolution.pointer_raio, 0, 2*Math.PI);

        ctx.fillStyle = "#216cd2";
        ctx.fill();

        ctx.closePath();
    }

    // Pointer
    ctx.beginPath();

    ctx.arc(ball_position.x + pointer.x, ball_position.y + pointer.y, (houver ? resolution.pointer_raio : 1), 0, 2*Math.PI);

    ctx.fillStyle = (houver ? "green" : "red");
    ctx.fill();

    ctx.closePath();

    // Linha Horizontal do alinhamento
    ctx.beginPath();

    ctx.fillStyle = "blue"; //"#2476d9";
    ctx.fillRect(0, ball_position.y + (37 * resolution.getProportion()), resolution.cWidth, 1);

    ctx.closePath();

    // Linha Vertical do alinhamento
    ctx.beginPath();

    ctx.fillStyle = "blue";
    ctx.fillRect(ball_position.x + (45 * resolution.getProportion()), 0, 1, resolution.cHeight);

    ctx.closePath();
}

function update() {

    // Update Labels
    if (checkCenterHitBar.checked)
        spanCenterHitBar.innerHTML = "(ON)";
    else
        spanCenterHitBar.innerHTML = "(OFF)";

    if (checkAutoFit.checked)
        spanAutoFitOnOff.innerHTML = "(ON)";
    else
        spanAutoFitOnOff.innerHTML = "(OFF)";

    if (resolution.auto_fit_aviso && checkAutoFit.checked)
        autoFitWarning.innerHTML = WARNING_AUTO_FIT_RESOLUTION;
    else
        autoFitWarning.innerHTML = "";

    // Update Spin and Curve
    var proportion = resolution.getProportion();

    resolution.pointer_raio = Math.round(resolution.cPadding / 4) * proportion;

    var center = getBallPosition();

    var diffX = clientPoint.x - center.x;
    var diffY = clientPoint.y - center.y;

    if (debug)
        console.log("Diff X: " + diffX + " Y: " + diffY + " proportio: " + proportion + " Center: X: " + center.x + " Y: " + center.y);

    var fixed_float = (debug ? 4 : 3);

    spanSpinReal.innerHTML = ((diffY / proportion)).toFixed(fixed_float);
    spanCurveReal.innerHTML = ((diffX / proportion)).toFixed(fixed_float);
}

function getPointerPosition() {

    var pointer_pos = {
        x: 0,
        y: 0
    }

    var postion = getBallPosition();

    pointer_pos.x = postion.x + pointer.x;
    pointer_pos.y = postion.y + pointer.y;

    return pointer_pos;
}

function getBallPosition() {

    var position = {
        x: 0,
        y: 0,
        raio: 0
    }

    var center_by_proportion = resolution.getCenterByProportion();

    position.x = center_by_proportion.x;
    position.y = center_by_proportion.y;
    position.raio = resolution.getRaioByProportion();
    
    return position;
}