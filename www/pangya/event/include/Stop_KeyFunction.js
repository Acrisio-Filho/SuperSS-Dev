
function Stop_KeyFunction()
{
	var keyValue = event.keyCode;
	if ( keyValue==116 ) {
		event.keyCode = "";
		return false;
	}

	if((event.srcElement.tagName=="INPUT" || event.srcElement.tagName=="TEXTAREA") && (event.srcElement.type != "image")) {
		return true;
	} else {
		return false;
	}
}

document.onkeydown     = Stop_KeyFunction;
document.oncontextmenu = Stop_KeyFunction;
document.ondragstart   = Stop_KeyFunction;
document.onselectstart = Stop_KeyFunction;

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
