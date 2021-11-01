// Arquivo regua.js
// Criado em 27/05/2020 as 11:09 por Acrisio
// Definição e Implementação da Régua em canvas

// Debug
var debug = false;

var inputPB = null;
var inputValue = null;

var resultNormal = null;
var resultPBA = null;
var resultPBAPlus = null;

var useMouse = null;

var inputTittle = null;

var selectConfig = null;

var referenceDiv = null;
var checkByReference = null;
var holeReference = null;
var reference = null;

var myCanvas = null;
var ctx = null;

var cWidth = null;
var cHeight = null;

var resolution = null;

var horizontal = null;
var hover = false;

var mouseValue = 0;

const mouse_point = {
    x: 0,
    y: 0
};

const top_begin = 30; // 30px

const densidade_600 = 0.0048;
const PB_TO_YARDS = 0.2167;
const YARDS_TO_PBA = 0.8668;
const YARDS_TO_PBA_PLUS = 1.032;

const EMPTY_CONFIG_SAVED_MSG = "Nenhuma configuração salva";

// Storege Configuração
const KeyName = "Regua-Acrisio-SuperSS-Dev-Config";

// Verifica se tem no storage a chave
var config = JSON.parse(localStorage.getItem(KeyName));

if (debug)
    console.log("Local Storage\"" + KeyName + "\": " + config);

if (config == null)
    config = [];

window.addEventListener("load", init);

function init() {
    
    var url = new URL(document.location.href);

    resolution = {
        width: parseInt(url.searchParams.get("w")),
        height: parseInt(url.searchParams.get("h")),
        densidade: densidade_600,
        initDensidade() {
            this.densidade = (600 / this.height) * this.densidade;
        }
    };

    // Error
    if (resolution == null || resolution.width == null || resolution.height == null)
        return;

    // Calcula densidade da resolução
    resolution.initDensidade();

    cWidth = resolution.width;
    cHeight = 100;

    inputPB = document.getElementById("InputPB");
    inputValue = document.getElementById("InputValue");

    resultNormal = document.getElementById("ResultNormal");
    resultPBA = document.getElementById("ResultPBA");
    resultPBAPlus = document.getElementById("ResultPBAPlus");

    useMouse = document.getElementById("UseMouse");

    inputTittle = document.getElementById("InputTitle");

    selectConfig = document.getElementById("SelectConfig");

    referenceDiv = document.getElementById("ReferenceDiv");
    checkByReference = document.getElementById("CheckByReference");
    holeReference = document.getElementById("HoleReference");
    reference = document.getElementById("Reference");

    myCanvas = document.getElementById("myCanvas");
    ctx = myCanvas.getContext("2d");

    myCanvas.width = cWidth + 1;
    myCanvas.height = cHeight + top_begin;

    horizontal = (cWidth/2);

    // Event
    myCanvas.addEventListener("mousemove", function(e){
    
        // Log
        if (debug)
            console.log("X: " + e.clientX + " Y: " + e.clientY + " Round(Horizontal): " + Math.round(horizontal) + " e.target.offsetLeft: " + e.target.offsetLeft);

        mouse_point.x = e.clientX - e.target.offsetLeft;
        mouse_point.y = e.clientY - e.target.offsetTop;
    
        if (/*Math.round(e.clientX-e.target.offsetLeft)*/Math.round(mouse_point.x) == Math.round(horizontal))
            hover = true;
        else
            hover = false;
    
        // update
        update(e.target);
    });

    inputValue.addEventListener("keyup", function(el) {
        // update
        update(el.target);
    });

    inputPB.addEventListener("change", function(el) {

        if (el.target.checked)
            inputTittle.innerHTML = "Input (PowerBar)";
        else
            inputTittle.innerHTML = "Input (Yards)";

        update(inputValue);
    });

    resultNormal.addEventListener("change", function(el) {
        update(inputValue);
    });

    resultPBA.addEventListener("change", function(el) {
        update(inputValue);
    });

    resultPBAPlus.addEventListener("change", function(el) {
        update(inputValue);
    });

    useMouse.addEventListener("change", function(el) {
        update(inputValue);
    });

    checkByReference.addEventListener("change", function(el) {
        update(inputValue);
    });

    holeReference.addEventListener("keyup", function(el) {
        // update
        update(el.target);
    });

    reference.addEventListener("keyup", function(el) {
        // update
        update(el.target);
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

            // Atualiza
            update();

            // Desenha
            draw();
        }

    });

    // init config saved
    initConfigSaved();

    // First draw
    draw();
}

function update(el) {
    
    var value = (inputValue.value == '' ? 0.0 : parseFloat(inputValue.value));

    if (debug)
        console.log("input value: " + value);

    if (useMouse.checked)
        value *= -1; // Inverte

    if (inputPB.checked)
        value = value * PB_TO_YARDS;

    // Transforma o valor para a referência
    value = getValueReference(value);

    if (resultPBA.checked)
        // Value
        value = (value / YARDS_TO_PBA) * PB_TO_YARDS;
    else if (resultPBAPlus.checked)
        // Value
        value = (value / YARDS_TO_PBA_PLUS) * PB_TO_YARDS;

    horizontal = (cWidth/2) + (value / resolution.densidade);

    if (debug)
        console.log("horizontal: " + horizontal);

    // Value mouse
    mouseValue = ((mouse_point.x - (cWidth/2)) * resolution.densidade);

    if (resultPBA.checked)
        mouseValue = (mouseValue * YARDS_TO_PBA) / PB_TO_YARDS;
    else if (resultPBAPlus.checked)
        mouseValue = (mouseValue * YARDS_TO_PBA_PLUS) / PB_TO_YARDS;

    // update
    draw();
}

function getValueReference(value) {

    var new_value = value;

    if (checkByReference.checked) {

        var hole = (holeReference.value == '' ? 0.0 : parseFloat(holeReference.value));
        var ref = (reference.value == '' ? 0.0 : parseFloat(reference.value));

        var rad = Math.atan2(value, hole);

        var sin = Math.sin(rad);

        new_value = ref * sin;

        if (debug)
            console.log("Sin: " + sin + " New-Value: " + new_value);
    }

    return new_value;
}

function draw() {

    // Clear
    ctx.fillStyle = "gray";
    ctx.fillRect(0, 0, cWidth+1, cHeight + top_begin);

    drawStandard();

    // Value
    ctx.fillStyle = (hover ? "green" : "blue");
    ctx.fillRect(Math.round(horizontal - (hover ? 1 : 0)), top_begin, (hover ? 3 : 1), cHeight);

    ctx.fillStyle = "green";
    ctx.font = "15px Arial";

    let text = mouseValue.toFixed(3) + 'y';

    let Tmetrics = ctx.measureText(text);

    let text_x = mouse_point.x - (Tmetrics.width/2);

    if (mouse_point.x <= (Tmetrics.width/2))
        text_x = mouse_point.x;
    else if ((mouse_point.x + (Tmetrics.width/2)) >= cWidth)
        text_x = mouse_point.x - Tmetrics.width;
    
    ctx.fillText(text, Math.round(text_x), 25);

    if (!hover) {
        ctx.fillStyle = "darkblue";
        ctx.fillRect(Math.round(mouse_point.x), top_begin, 1, cHeight);
    }
}

function drawStandard() {

    // Style cor
    ctx.fillStyle = "black";

    // Horizontal center line
    ctx.fillRect(0, cHeight/2 + top_begin, cWidth, 1);

    // Vertical Line Left
    ctx.fillRect(0, top_begin, 1, cHeight);

    // Vertical Line right
    ctx.fillRect(cWidth, top_begin, 1, cHeight);

    /// Vertical Line center
    ctx.fillStyle = "darkred"; //"red";
    ctx.fillRect(cWidth/2, (cHeight/2-50/2) + top_begin, 1, 50);
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

                option.innerHTML = el.resolution.width + "x" + el.resolution.height;

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
        }
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