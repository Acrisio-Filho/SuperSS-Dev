// Vars constants
var arr_opts = [];
var atte = ["name", "category"]
            
// Options Select
function myFunction(id) {

    regx = new RegExp(".*-" + id.split("-")[1] + "$");

    md = document.getElementById("myDropdown-" + id);

    if (md.className.indexOf("show") >= 0) {

        // Update
        md.className = md.className.replace("show", "");

        // Restore
        for (il in arr_opts) {

            if (arr_opts[il] != id && regx.test(arr_opts[il])) {

                md2 = document.getElementById("drop-" + arr_opts[il]);

                // Restore
                if (md2 && md2.className.indexOf("show") === -1)
                    md2.className += " show";
            }
        }
        
    }else {

        // Clear
        for (il in arr_opts) {

            if (arr_opts[il] != id && regx.test(arr_opts[il])) {

                md2 = document.getElementById("drop-" + arr_opts[il]);
            
                if (md2 && md2.className.indexOf("show") >= 0)
                    md2.className = md2.className.replace("show", "");
            }
        }

        // Update
        md.className += " show";
    }
}

function filterFunction(el, id) {
    
    var input, filter, a, i;
    
    input = document.getElementById("myInput-" + id);
    
    filter = input.value.toUpperCase();
    
    div = document.getElementById("myDropdown-" + id);
    
    a = div.getElementsByTagName("span");

    for (i = 0; i < a.length; i++) {
        
        if (a[i].id != (el.id + "-SPAN")) {
            txtValue = a[i].textContent || a[i].innerText;
    
            for (j in atte) {
                txtValue = txtValue + a[i].getAttribute(atte[j]);
            }

            if (txtValue.toUpperCase().indexOf(filter) > -1) {
                a[i].style.display = "";
            } else {
                a[i].style.display = "none";
            }
        }
    }
}

function myClick(event, id) {

    if (event.target)
        er = event.target;
    else if (event.srcElement)
        er = event.srcElement
    else
        er = event;

    if (er.className.indexOf("dropbtn") === -1 && er.id != ("myInput-" + id) && er.id != "itemSelected" 
        && er.className.indexOf("category") === -1 && er.className.indexOf("rarity") === -1 && er.className.indexOf("have") === -1) {
        
        ed = null;

        do {
            
            if (er.nodeType != 9 && er.className.indexOf("lid") >= 0 && er.nodeName == "SPAN" 
                && (ld = er.attributes.getNamedItem("name")) != null && er.className.indexOf("have") === -1) {
                
                d = document.getElementById("sel-" + id);
                
                if (er.className.indexOf("selectopt") === -1 && er.className.indexOf("have") === -1)
                    er.className += " selectopt";

                clone = er.childNodes[0].cloneNode(true);

                clone.id = "itemSelected";

                if (d) {
                    d.replaceChild(clone, d.firstChild);
                }
                ed = er;

                inpt = document.getElementById("item-" + id);

                if (inpt) {
                    inpt.value = "{\"typeid\":" + er.id + ", \"category\":\"" + er.getAttribute("category") + "\"}";
                }
                
                break;
            }else if ((er.id && er.id == "itemSelected") || (er.className && er.className.indexOf("have") >= 0))
                return;

        }while (ed == null && (er = er.parentNode) != null);

        var openDropdown = document.getElementById("myDropdown-" + id);
        
        if (openDropdown.className.indexOf("show") >= 0) {

            openDropdown.className = openDropdown.className.replace("show", "");

            regx = new RegExp(".*-" + id.split("-")[1] + "$");

            for (il in arr_opts) {

                if (arr_opts[il] != id && regx.test(arr_opts[il])) {
    
                    md2 = document.getElementById("drop-" + arr_opts[il]);

                    if (md2 && md2.className.indexOf("show") === -1)
                        md2.className += " show";
                }
            }
        }

        // Selecteds opts
        var fs = document.getElementById("fieldSetlist-" + id);

        for (k in fs.childNodes) {

            if (fs.childNodes[k].id != ed.id) {

                if (fs.childNodes[k].className && fs.childNodes[k].className.indexOf("selectopt") >= 0)
                    fs.childNodes[k].className = fs.childNodes[k].className.replace("selectopt", "");
            }
        }
	}
}

// Aba
function btnReward(el, id) {

    regx = new RegExp(".*-" + id + "$");

    for (il in arr_opts) {

        if (regx.test(arr_opts[il])) {

            inpt = document.getElementById("item-" + arr_opts[il]);

            if (inpt && inpt.value == "undefined") {

                alert("Por favor escolha seus itens ainda tem campos para selecionar.");

                return false;
            }
        }
    }

    return true;
}

function btnHelp(el) {

    if (el) {

        el.style.display = "none";

        msg_err = document.getElementById("Error-Msg");

        if (msg_err)
            msg_err.style.display = "none";

        toggleHelp(document.getElementById("aba-help"));
    }
}

function changeAba(el, id) {

    help = document.getElementById("aba-help");
    
    if (el && el.className && el.className.indexOf("ab-sel") === -1 && help && help.className && help.className.indexOf("aba-help-sel") === -1) {

        rev = 1;

        if ((id - 1) == 0) {
            rev = 2;
        }

        ctt = document.getElementById("aba-content-" + id);
        revr = document.getElementById("aba-content-" + rev);
        ab_rev = document.getElementById("aba-" + rev);

        if (ab_rev && ab_rev.className.indexOf("ab-sel")) {
            ab_rev.className = ab_rev.className.replace("ab-sel", "");
        }

        el.className += " ab-sel";

        if (revr) {
            revr.style.display = "none";
        }

        if (ctt) {
            ctt.style.display = "table-cell";
        }
    }
}

function toggleHelp(el) {

    if (el && el.className) {
        
        if (el.className.indexOf("aba-help-sel") === -1) {

            ctt = document.getElementById("aba-help-content");
            a1 = document.getElementById("aba-content-1");
            a2 = document.getElementById("aba-content-2");
            
            el.className += " aba-help-sel";

            if (a1)
                a1.style.display = "none";

            if (a2)
                a2.style.display = "none";

            if (ctt)
                ctt.style.display = "table-cell";

        }else {

            ctt = document.getElementById("aba-help-content");
            
            el.className = el.className.replace("aba-help-sel", "");

            if (ctt)
                ctt.style.display = "none";

            a1 = document.getElementById("aba-content-1");

            if (a1) {

                aa1 = document.getElementById("aba-1");

                if (aa1 && aa1.className && aa1.className.indexOf("ab-sel") >= 0) {
                    
                    a1.style.display = "table-cell";

                    changeAba(aa1, 1);

                    return;
                }
            }

            a2 = document.getElementById("aba-content-2");

            if (a2) {

                aa2 = document.getElementById("aba-2");

                if (aa2 && aa2.className && aa2.className.indexOf("ab-sel") >= 0) {
                    
                    a2.style.display = "table-cell";

                    changeAba(aa2, 2);

                    return;
                }
            }

            changeAba(document.getElementById("aba-1", 1));
        }
    }
}

function init() {
    changeAba(document.getElementById("aba-1"), 1);
    toggleHelp(document.getElementById("aba-help"));
}