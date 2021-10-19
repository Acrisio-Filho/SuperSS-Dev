// Seta handler window.onload
window.onload = myOnload;

// Interval
var interval_value = 1000/24; // 24x por segundo

var arr = [
    '.',
    '. .',
    '. . .',
    '. . . .',
    '. . . . .'
];

var arr_len = arr.length;

var msg = "Now Loading ";
var index = 0;

var my_div_anim = null;
var my_img_loading = null;
var my_img_set = null;

// Seta Timer para mostrar o loading
var my_interval = setInterval(handlerCheckLoading, interval_value);

function handlerCheckLoading() {

    // Tira o fundo
    if (my_img_set == null && (my_img_loading != null || (my_img_loading = document.getElementById("myImg")) != null)) {
        my_img_set = true;
        my_img_loading.onload = bgImg;
    }

    // Animation
    if (my_div_anim != null || (my_div_anim = document.getElementById("LoadingAnim")) != null) {
        document.getElementById("LStrong").style.display = "inline";
        my_div_anim.innerHTML = arr[parseInt(index++ / 5) % arr_len];
    }
}

function bgImg() {

    my_img_loading.style = "position:absolute; z-index:1; width:800px; height:553px;";
}

function myOnload() {
    clearInterval(my_interval);
    document.getElementById("BlockLoading").style.display = "none";
    document.getElementById("myImg").style.display = "none";
}

