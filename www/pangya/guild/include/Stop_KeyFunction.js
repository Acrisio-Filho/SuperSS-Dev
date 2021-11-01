
function Stop_KeyFunction()
{
	var keyValue = event.keyCode;
	if ( keyValue==116 ) {
		//alert( 'キー操作は抑制されました' );
		event.keyCode = "";
		return false;
	}

	if((event.srcElement.tagName=="INPUT" || event.srcElement.tagName=="TEXTAREA") && (event.srcElement.type != "image")) {
		return true;
	} else {
		//alert( 'キー操作は抑制されました' );
		return false;
	}
}

document.onkeydown     = Stop_KeyFunction;
document.oncontextmenu = Stop_KeyFunction;
document.ondragstart   = Stop_KeyFunction;
document.onselectstart = Stop_KeyFunction;

//確認ダイアログ等を押した際にカーソルが隠れることへの対策
function CnfDialog(message) {
    if (confirm(message)) {
        return true;
    } else {
        window.focus();
        return false;
    }
}

function AlertDialog(message) {
    alert(message);
    window.focus();
    return false;
}
