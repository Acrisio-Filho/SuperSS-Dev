$(document).ready(function ($) {
    $('#dialogWindow').dialog({
        autoOpen: false,
        closeOnEscape: false,
        modal: true,
        width: 400,
        height: 300,
        resizable: false,
        show: "",
        hide: "",
        open: function (event, ui) { $(".ui-dialog-titlebar-close").hide(); },
        close: function () { }
    });
});

var gachaCount = 0;
function dialogOpen( count ) {
    $('#dialogWindow').removeAttr("display");
    $('#dialogWindow').dialog('open');
    gachaCount = count;

    switch (count) {
        case 1:
            $('#Msg_gachaPlay').html("The remaining number of plays will be consumed \"one time\" and will be \"played once\". <br>Are you sure?");
            break;
        case 10:
            $('#Msg_gachaPlay').html("The remaining number of plays will be consumed \"9 times\" and will be \"played 10 times\". <br>Are you sure?");
            break;
        default:
            $('#dialogWindow').dialog('close');
            break;
    }
}

function goGacha() {
    location.href = "./gacha_lottery.php?count=" + gachaCount;
    gachaCount = 0;
}

function dialogClose() {
    $('#dialogWindow').dialog('close');
}