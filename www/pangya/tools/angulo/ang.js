// Arquivo ang.js
// Criado em 09/05/2020 as 22:00 por Acrisio
// Implementação do visualizador de angulo

var debug = false;

var ctx = document.getElementById("myCanvas").getContext("2d");
var span_ang360 = document.getElementById("span-ang360");
var span_ang90 = document.getElementById("span-ang90");
var span_sin = document.getElementById("span-sin");
var span_cos = document.getElementById("span-cos");
var scale = document.getElementById("scale").value;
var antialiasing = document.getElementById("checkAntialiasing");

antialiasing.addEventListener("change", function() {
    drawWind();
});

// Verifica se tem no storage a chave
var config = JSON.parse(localStorage.getItem(Configuration.KeyName));

if (debug)
    console.log("Local Storage\"" + Configuration.KeyName + "\": " + config);

if (config == null)
    config = new Configuration();

var wind = new Image();
wind.onload = loadImage;

var GAng = 0;
var ratio_spect = 0.75; // 0.75 4:3, 0.6 Wide
var ratio = 1.0;

var seta = new Image()
seta.src = "seta.png";

function updateScale() {
    scale = parseFloat(document.getElementById("scale").value);

    // Log
    if (debug)
        console.log(scale);
}

function updateRatioSpect() {
    if (ratio_spect >= 0.75)
        ratio = 1.0;
    else
        ratio = 1.01;
};

const SETA_CENTER_X = (500*0.24)/2;
const SETA_CENTER_Y = (500*0.24)/2-8;
const SETA_ZOOM_X = 500*0.76;
const SETA_SOOM_Y = 500*0.76;

function drawWind() {

    // Update ratio spect
    updateRatioSpect();

    ctx.imageSmoothingEnabled = antialiasing.checked;

    // clear
    ctx.fillStyle = 'white';
    ctx.fillRect(0, 0, 500, 500);
    
    ctx.drawImage(wind, config.ix, config.iy, 500, 500, 0, 0, 500*ratio*config.zoom, 500*ratio*config.zoom);
    ctx.save();

    ctx.translate(250, 250);
    ctx.rotate((Math.PI/180)*GAng*-1);
    ctx.translate(-250, -250);

    ctx.drawImage(seta, 0, 0, 500, 500, SETA_CENTER_X, SETA_CENTER_Y, SETA_ZOOM_X, SETA_SOOM_Y);

    ctx.restore();

    // Style - cor line
    ctx.fillStyle = "red";
    
    // Line Left Horizontal
    ctx.fillRect(0, 500/2, 100, 1);

    // Line Right Horizontal
    ctx.fillRect(500-100, 500/2, 100, 1);

    // Line Top Vertical
    ctx.fillRect(500/2, 0, 1, 100);

    // Line Down(Bottom) Veritcal
    ctx.fillRect(500/2, 500-100, 1, 100);
}

function loadImage(e) {
    
    oldW = config.realW;
    oldH = config.realH;

    config.realW = e.target.naturalWidth;
    config.realH = e.target.naturalHeight;
    
    if (config.ix == 0 || config.realW != oldW)
        config.ix = config.realW - 135;

    if (config.iy == 0 || config.realH != oldH)
        config.iy = config.realH - 145;

    ratio_spect = config.realH / config.realW;

    drawWind();

    // Log
    if (debug)
        console.log(config.realW + " x " + config.realH);
}

function updateAng(e) {

    // Log
    if (debug) {
        console.log(e);
        console.log("X:" + e.clientX);
        console.log("Y:" + e.clientY);
    }

    ang = Math.atan2((500/2+e.target.offsetLeft)-e.clientX, (500/2+e.target.offsetTop)-e.clientY)*180/Math.PI;

    if (ang < 0)
        ang = ang + 360;

    GAng = ang;

    span_ang360.innerHTML = ang.toFixed(2);

    ang90 = Math.abs(Math.asin(Math.sin((Math.PI/180)*ang))*180/Math.PI);

    span_ang90.innerHTML = ang90.toFixed(2);

    span_sin.innerHTML = Math.sin((Math.PI/180)*ang).toFixed(4);
    span_cos.innerHTML = Math.cos((Math.PI/180)*ang).toFixed(4);

    // Log
    if (debug)
        console.log("Ang: " + ang);
}

document.getElementById("myCanvas").addEventListener("mousemove", function(e) {
    updateAng(e);
    drawWind();
});

// Copia ângulo clicando com o mouse
document.getElementById("myCanvas").addEventListener("click", copyToClipboard);

document.getElementById("myFile").addEventListener("change", function(e) {
    
    // Log
    if (debug) {
        console.log(e);
        console.log(e.target.files[e.target.files.length-1]);
    }

    const reader = new FileReader();

    reader.onload = function(evt) {
        wind.src = 'data:image/jpg;base64,'+ btoa(evt.target.result);
    }

    reader.readAsBinaryString(e.target.files[e.target.files.length-1]);
});

function buttonAction(ba) {

    updateScale();

    switch (ba) {
        case 'up':
            config.iy+=scale;
            break;
        case 'down':
            config.iy-=scale;
            break;
        case 'left':
            config.ix-=scale;
            break;
        case 'right':
            config.ix+=scale;
            break;
        case '+':
            config.zoom+=0.2;
            break;
        case '-':
            config.zoom-=0.2;
            break;
        case 'save':
            localStorage.setItem(Configuration.KeyName, JSON.stringify(config));
            break;
    }
    drawWind();
}

function copyToClipboard() {

    var target_id = "__hiddenCopy__";

    var target = document.getElementById(target_id);

    if (!target) {

        // Cria um
        target = document.createElement("textarea");
        target.style.position = "absolute";
        target.style.left = "-999px";
        target.style.top = 0;
        target.id = target_id;

        document.body.appendChild(target);
    }

    // 360 check
    var check360cpy = document.getElementById("check360cpy");

    target.textContent = (check360cpy && check360cpy.checked ? span_ang360.innerHTML : span_ang90.innerHTML); // Copy Content

    target.focus();
    target.setSelectionRange(0, target.value.length);

    var success;
    try {
        document.execCommand("copy");
    }catch (e) {
        success = false;
    }

    // limpa o target
    target.textContent = "";

    return success;
}