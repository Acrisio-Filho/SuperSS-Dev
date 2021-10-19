<?php
	// Arquivo notice.php
	// Criado em 25/01/2018 as 14:55 por Acrisio
	// Definição e Implementação da página que mostra os updates e noticias

	// Inicializa os valores
	include_once("config/notice_singleton.inc");

?>

<!DOCTYPE html>
<html lang="pt-BR">
<head>
	<meta charset="UTF-8">
	<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
	<link rel="icon" href="/favicon.ico" type="image/x-icon">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Notice</title>
	<script>
		var getElementsByClassName;
		if (!document.getElementsClassName) {
			getElementsByClassName = function (className, tag, elm){
				if (document.getElementsByClassName) {
					getElementsByClassName = function (className, tag, elm) {
						elm = elm || document;
						var elements = elm.getElementsByClassName(className),
							nodeName = (tag)? new RegExp("\\b" + tag + "\\b", "i") : null,
							returnElements = [],
							current;
						for(var i=0, il=elements.length; i<il; i+=1){
							current = elements[i];
							if(!nodeName || nodeName.test(current.nodeName)) {
								returnElements.push(current);
							}
						}
						return returnElements;
					};
				}
				else if (document.evaluate) {
					getElementsByClassName = function (className, tag, elm) {
						tag = tag || "*";
						elm = elm || document;
						var classes = className.split(" "),
							classesToCheck = "",
							xhtmlNamespace = "http://www.w3.org/1999/xhtml",
							namespaceResolver = (document.documentElement.namespaceURI === xhtmlNamespace)? xhtmlNamespace : null,
							returnElements = [],
							elements,
							node;
						for(var j=0, jl=classes.length; j<jl; j+=1){
							classesToCheck += "[contains(concat(' ', @class, ' '), ' " + classes[j] + " ')]";
						}
						try	{
							elements = document.evaluate(".//" + tag + classesToCheck, elm, namespaceResolver, 0, null);
						}
						catch (e) {
							elements = document.evaluate(".//" + tag + classesToCheck, elm, null, 0, null);
						}
						while ((node = elements.iterateNext())) {
							returnElements.push(node);
						}
						return returnElements;
					};
				}
				else {
					getElementsByClassName = function (className, tag, elm) {
						tag = tag || "*";
						elm = elm || document;
						var classes = className.split(" "),
							classesToCheck = [],
							elements = (tag === "*" && elm.all)? elm.all : elm.getElementsByTagName(tag),
							current,
							returnElements = [],
							match;
						for(var k=0, kl=classes.length; k<kl; k+=1){
							classesToCheck.push(new RegExp("(^|\\s)" + classes[k] + "(\\s|$)"));
						}
						for(var l=0, ll=elements.length; l<ll; l+=1){
							current = elements[l];
							match = false;
							for(var m=0, ml=classesToCheck.length; m<ml; m+=1){
								match = classesToCheck[m].test(current.className);
								if (!match) {
									break;
								}
							}
							if (match) {
								returnElements.push(current);
							}
						}
						return returnElements;
					};
				}
				return getElementsByClassName(className, tag, elm);
			};
		}else
			getElementsByClassName = document.getElementsByClassName;

		function tab(nome, conteudo) {
			this.nome = nome;
			this.conteudo = conteudo;
		}

		var arr = [];
		arr.push(new tab('all', 'all'));
		arr.push(new tab('notice', 'notice-c'));
		arr.push(new tab('event', 'event-c'));
		arr.push(new tab('update', 'update-c'));

		function altertab(nome, conteudo) {
			var c = undefined, m = undefined, i = 0;

			for (i = 0; i < arr.length; i++) {
				m = document.getElementById(arr[i].nome);
				m.className = 'menu';
				
				if (arr[i].conteudo != 'all') {
					c = getElementsByClassName(arr[i].conteudo);

					for (var j = 0; j < c.length; ++j) {
						c[j].style.display = 'none';
					}
				}
			}

			m = document.getElementById(nome);
			m.className = 'menu-sel';

			if (conteudo == 'all') {
				for (i = 0; i < arr.length; i++) {
					if (arr[i].conteudo != 'all') {
						c = getElementsByClassName(arr[i].conteudo);

						for (var j = 0; j < c.length; ++j) {
							c[j].style.display = '';
						}
					}
				}
			}else {
				c = getElementsByClassName(conteudo);

				for (var j = 0; j < c.length; ++j) {
					c[j].style.display = '';
				}
			}
		}
	</script>

	<style type="text/css">
		* {
			margin: 0;
			padding: 0;
		}
		html {
			width: 100%;
		}
		body {
			width: 100%;
		}
		h1 {
			color: black;
			text-align: center;
			display: block;
		}
		#div_principal {
			display: block;
			width: 100%;
		}
		#div_title {
			width: 100%;
			display: block;
			margin: 5% auto;
		}
		td {
			border: 1px solid black;
		}
		.menu {
			background-color: #85d6f9;
			text-align: center;
			cursor: pointer;
		}
		.menu:hover {
			background-color: #def5ff;
		}
		.menu-sel {
			background-color: #41a0c9;
			border: 1px solid black;
			border-top: 2px solid black;
			border-bottom: none;
			text-align: center;
		}
		.d1 td,tr {
			border: none;
		}
		.d2 td,tr {
			border: none;
		}
		.separetor {
			background-color: #4180a9;
		}
	</style>
</head>
<body onload="altertab('all', 'all')" bgColor="#ffffff">
	<div id="div_principal">
		<div id="div_title">
			<h1>Pangya SuperSS</h1>
		</div>
	</div>

	<table width="100%" cellspacing="0" cellpadding="0" border="0" bgColor="#def5ff">
		<tr>
			<td width="2" style="border: none"></td>
			<td class="menu-sel" id="all" onclick="altertab('all', 'all')">
				All
			</td>
			<td class="menu" id="notice" onclick="altertab('notice', 'notice-c')">
				Notice
			</td>
			<td class="menu" id="event" onclick="altertab('event', 'event-c')">
				Event
			</td>
			<td class="menu" id="update" onclick="altertab('update', 'update-c')">
				Update
			</td>
			<td width="4" style="border: none"></td>
		</tr>
	</table>

	<table class="d1" width="100%" cellspacing="0" cellpadding="0" border="0" bgColor="#def5ff">
		<tr>
			<td height="3"></td>
		</tr>
		<tr>
			<td>
				<div style="display: block; width: 100%; height: 155px; overflow-y: auto; overflow-x: hidden">
				<?php

					if (count(NoticeSingleton::getInstance()->getAllNotices())) {

						$GLOBALS['NOTICE_TYPE_LABEL_1'] = [
							'all',
							'notice',
							'event',
							'update'
						];

						$GLOBALS['NOTICE_TYPE_LABEL_2'] = [
							'[A]',
							'[N]',
							'[E]',
							'[U]'
						];

						echo '<table class="d2" width="100%" cellspacing="5" cellpadding="0" border="0">';

						foreach(NoticeSingleton::getInstance()->getAllNoticesSortByDate() as $n) {
							
							echo '<tr class="'.$GLOBALS['NOTICE_TYPE_LABEL_1'][$n->type].'-c">
									<td width="1"></td>
									<td width="70%" align="left">';

									if ($n->body != '')
										echo '<a href="#" onclick="javascript:window.open(\'notice_view.php?id='.$n->id.'\', \'Notice\', \'width=600,height=400,scrollbars=yes\')">';

									// Título
									echo $GLOBALS['NOTICE_TYPE_LABEL_2'][$n->type];
									echo ' '.$n->title;

									if ($n->body != '')
										echo '</a>';

							echo '	</td>
									<td align="right">
										'.date("d/m/y", $n->date).'
									</td>
									<td width="20"></td>
								  </tr>
								  <tr class="'.$GLOBALS['NOTICE_TYPE_LABEL_1'][$n->type].'-c">
								  	<td height="1" class="separetor" colspan="4"></td>
								  </tr>';
						}

						echo '</table>';
					}
				?>
				</div>
			</td>
		</tr>
	</table>

</body>
</html>