<?php
    // Arquivo check_nickname.php
    // Criado em 18/08/2019 as 01:28 por Acrisio
    // Verifica se o comprimento do nickname é válido

    $nickname = '';
    $msg = null;

    if (isset($_POST['nickname']) && $_POST['nickname'] != '') {
        
        $nickname = $_POST['nickname'];

        $len = strlen(mb_convert_encoding($nickname, "SJIS", "UTF-8"));

        $msg = ['message' => 'Seu nickname "'.htmlspecialchars($nickname).'" tem o comprimento de '.$len, 'state' => ($len <=16 ? "OK" : "TOO LONG") ];
    }

    echo '<!DOCTYPE html>
        <html lang="pt-br">
        <head>
            <meta charset="UTF-8" />
            <link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
            <link rel="icon" href="/favicon.ico" type="image/x-icon">
            <title>Check nickname</title>
        <style>
            html, body {
                padding: 0;
                margin: 0;
                border: 0;
            }

            .tooltip {
                position: relative;
                display: inline-block;
                /* border-bottom: 1px dotted black; */
            }

            .tooltip .tooltiptext {
                visibility: hidden;
                width: 200px;
                background-color: #555;
                color: #fff;
                text-align: center;
                border-radius: 6px;
                padding: 5px 0;
                position: absolute;
                z-index: 1;
                /*top: 125%;*/
                /*left: 50%;*/
                margin-left: 15px;
                opacity: 0;
                transition: opacity 0.3s;
            }

            .tooltip .tooltiptext::after {
                content: "";
                position: absolute;
                top: 10px;
                left: -5px;
                margin-left: -5px;
                border-width: 5px;
                border-style: solid;
                border-color: transparent #555 transparent transparent;
            }

            .tooltip:hover .tooltiptext {
                visibility: visible;
                opacity: 1;
            }
            
            .tooltiplink {
                cursor: pointer;
            }
        </style>
        <script>
            function putInText(emoticon, name) {
                
                var el = document.getElementById(\'nickname\');
                var invert = document.getElementById(\'invert\');

                name = (emoticon != null) ? emoticon.innerHTML : name;

                if (invert && invert.checked)
                    //name = \'（＠\' + name.substr(1);
                    name = \'(-\' + name.substr(1);

                if (el)
                    el.value += name;

                return false;
            }
        </script>
        </head>
        <body>';

    // Table central
    echo '<table align="center" valign="center" cellpadding="0" cellspacing="0" border="0">
        <tr>
            <td align="center" valign="center">';

    echo '<table cellspacing="0" cellpadding="0" border="0" width="400">
            <tr>
                <td align="center">';

    echo '<table cellspacing="0" cellpadding="0" border="0" width="256" height="200" background="img/emoticon.png">';

    $emoticon_jp = [
        ['index' => 1, 'kr_names' => [
        '(긴장)'
        ], 'jpn_names' => [
        '（まじ）'
        ]
        ]
        , ['index' => 2, 'kr_names' => [
        '(썰렁)'
        ], 'jpn_names' => [
        '（あせ）'
        ]
        ]
        , ['index' => 3, 'kr_names' => [
        '(하하)'
        , '(웃음)'
        ], 'jpn_names' => [
        '（わら）'
        , '（笑う）'
        ]
        ]
        , ['index' => 4, 'kr_names' => [
        '(미안)'
        ], 'jpn_names' => [
        '（しょぼ）'
        ]
        ]
        , ['index' => 5, 'kr_names' => [
        '(미소)'
        ], 'jpn_names' => [
        '（にや）'
        ]
        ]
        , ['index' => 6, 'kr_names' => [
        '(화남)'
        ], 'jpn_names' => [
        '（いかり）'
        ]
        ]
        , ['index' => 7, 'kr_names' => [
        '(놀람)'
        , '(헉)'
        ], 'jpn_names' => [
        '（びっくり）'
        , '（へ）'
        ]
        ]
        , ['index' => 8, 'kr_names' => [
        '(새침)'
        ], 'jpn_names' => [
        '（ん）'
        ]
        ]
        , ['index' => 9, 'kr_names' => [
        '(윙크)'
        ], 'jpn_names' => [
        '（ういんく）'
        ]
        ]
        , ['index' => 10, 'kr_names' => [
        '(요원)'
        , '(감시)'
        ], 'jpn_names' => [
        '（ぐらさん）'
        , '（感じ）'
        ]
        ]
        , ['index' => 11, 'kr_names' => [
        '(두근)'
        ], 'jpn_names' => [
        '（すき）'
        ]
        ]
        , ['index' => 12, 'kr_names' => [
        '(흑흑)'
        , '(울음)'
        , '(눈물)'
        ], 'jpn_names' => [
        '（かなし）'
        , '（泣く）'
        , '（涙）'
        ]
        ]
        , ['index' => 13, 'kr_names' => [
        '(엉엉)'
        , '(통곡)'
        ], 'jpn_names' => [
        '（わーん）'
        , '（泣き喚き）'
        ]
        ]
        , ['index' => 14, 'kr_names' => [
        '(감동)'
        ], 'jpn_names' => [
        '（うる）'
        ]
        ]
        , ['index' => 15, 'kr_names' => [
        '(민망)'
        ], 'jpn_names' => [
        '（はず）'
        ]
        ]
        , ['index' => 16, 'kr_names' => [
        '(절망)'
        , '(좌절)'
        ], 'jpn_names' => [
        '（ぎょ）'
        , '（挫折）'
        ]
        ]
        , ['index' => 17, 'kr_names' => [
        '(구토)'
        ], 'jpn_names' => [
        '（げー）'
        ]
        ]
        , ['index' => 18, 'kr_names' => [
        '(현기증)'
        ], 'jpn_names' => [
        '（あれ）'
        ]
        ]
        , ['index' => 19, 'kr_names' => [
        '(인사)'
        , '(안녕)'
        ], 'jpn_names' => [
        '（ぺこ）'
        , '（こんにちは）'
        ]
        ]
        , ['index' => 20, 'kr_names' => [
        '(메롱)'
        ], 'jpn_names' => [
        '（にこ）'
        ]
        ]
        , ['index' => 21, 'kr_names' => [
        '(거만)'
        ], 'jpn_names' => [
        '（むふ）'
        ]
        ]
        , ['index' => 22, 'kr_names' => [
        '(헤헤)'
        , '(캬캬)'
        ], 'jpn_names' => [
        '（ひょえ）'
        , '（はは）'
        ]
        ]
        , ['index' => 23, 'kr_names' => [
        '(무안)'
        ], 'jpn_names' => [
        '（気まずい）'
        ]
        ]
        , ['index' => 24, 'kr_names' => [
        '(박수)'
        ], 'jpn_names' => [
        '（拍手）'
        ]
        ]
        , ['index' => 25, 'kr_names' => [
        '(아니)'
        ], 'jpn_names' => [
        '（ううん）'
        ]
        ]
        , ['index' => 26, 'kr_names' => [
        '(꼬깔)'
        ], 'jpn_names' => [
        '（三角帽子）'
        ]
        ]
        , ['index' => 27, 'kr_names' => [
        '(사랑)'
        ], 'jpn_names' => [
        '（はーと）'
        ]
        ]
        , ['index' => 28, 'kr_names' => [
        '(이별)'
        ], 'jpn_names' => [
        '（だめだ）'
        ]
        ]
        , ['index' => 29, 'kr_names' => [
        '(케이크)'
        , '(케익)'
        , '(케잌)'
        ], 'jpn_names' => [
        '（けーき）'
        , '（ケーキ）'
        , '（ケーキ）'
        ]
        ]
        , ['index' => 30, 'kr_names' => [
        '(선물)'
        ], 'jpn_names' => [
        '（おくりもの）'
        ]
        ]
        , ['index' => 31, 'kr_names' => [
        '(편지)'
        ], 'jpn_names' => [
        '（めーる）'
        ]
        ]
        , ['index' => 32, 'kr_names' => [
        '(입술)'
        ], 'jpn_names' => [
        '（くち）'
        ]
        ]
        , ['index' => 33, 'kr_names' => [
        '(장미)'
        ], 'jpn_names' => [
        '（ばら）'
        ]
        ]
        , ['index' => 34, 'kr_names' => [
        '(당근)'
        ], 'jpn_names' => [
        '（にんじん）'
        ]
        ]
        , ['index' => 35, 'kr_names' => [
        '(맥주)'
        ], 'jpn_names' => [
        '（びーる）'
        ]
        ]
        , ['index' => 36, 'kr_names' => [
        '(커피)'
        ], 'jpn_names' => [
        '（こーひー）'
        ]
        ]
        , ['index' => 37, 'kr_names' => [
        '(와인)'
        ], 'jpn_names' => [
        '（わいん）'
        ]
        ]
        , ['index' => 38, 'kr_names' => [
        '(펭귄)'
        ], 'jpn_names' => [
        '（ぺんぎん）'
        ]
        ]
        , ['index' => 39, 'kr_names' => [
        '(뼈다귀)'
        ], 'jpn_names' => [
        '（ほね）'
        ]
        ]
        , ['index' => 40, 'kr_names' => [
        '(전구)'
        ], 'jpn_names' => [
        '（でんき）'
        ]
        ]
        , ['index' => 41, 'kr_names' => [
        '(전화)'
        ], 'jpn_names' => [
        '（でんわ）'
        ]
        ]
        , ['index' => 42, 'kr_names' => [
        '(음표)'
        , '(음악)'
        ], 'jpn_names' => [
        '（おんぷ）'
        , '（音楽）'
        ]
        ]
        , ['index' => 43, 'kr_names' => [
        '(악마)'
        ], 'jpn_names' => [
        '（あくま）'
        ]
        ]
        , ['index' => 44, 'kr_names' => [
        '(천사)'
        ], 'jpn_names' => [
        '（てんし）'
        ]
        ]
        , ['index' => 45, 'kr_names' => [
        '(해)'
        , '(태양)'
        ], 'jpn_names' => [
        '（たいよう）'
        , '（対応）'
        ]
        ]
        , ['index' => 46, 'kr_names' => [
        '(불)'
        ], 'jpn_names' => [
        '（ほのお）'
        ]
        ]
        , ['index' => 47, 'kr_names' => [
        '(눈사람)'
        , '(눈)'
        ], 'jpn_names' => [
        '（ゆき）'
        ]
        ]
        , ['index' => 48, 'kr_names' => [
        '(땀)'
        ], 'jpn_names' => [
        '（なみだ）'
        ]
        ]
        , ['index' => 49, 'kr_names' => [
        '(비)'
        ], 'jpn_names' => [
        '（あめ）'
        ]
        ]
        , ['index' => 50, 'kr_names' => [
        '(번개)'
        , '(벼락)'
        ], 'jpn_names' => [
        '（かみなり）'
        , '（雷）'
        ]
        ]
        , ['index' => 51, 'kr_names' => [
        '(별)'
        ], 'jpn_names' => [
        '（ほし）'
        ]
        ]
        , ['index' => 52, 'kr_names' => [
        '(우산)'
        ], 'jpn_names' => [
        '（かさ）'
        ]
        ]
        , ['index' => 53, 'kr_names' => [
        '(시간)'
        ], 'jpn_names' => [
        '（時間）'
        ]
        ]
        , ['index' => 54, 'kr_names' => [
        '(팡)'
        ], 'jpn_names' => [
        '（PP）'
        ]
        ]
        , ['index' => 55, 'kr_names' => [
        '(폭죽)'
        ], 'jpn_names' => [
        '（爆竹）'
        ]
        ]
        , ['index' => 56, 'kr_names' => [
        '(핸드폰)'
        ], 'jpn_names' => [
        '（ケータイ）'
        ]
        ]
        , ['index' => 57, 'kr_names' => [
        '(가위)'
        ], 'jpn_names' => [
        '（ちょき）'
        ]
        ]
        , ['index' => 58, 'kr_names' => [
        '(바위)'
        ], 'jpn_names' => [
        '（ぐー）'
        ]
        ]
        , ['index' => 59, 'kr_names' => [
        '(보)'
        ], 'jpn_names' => [
        '（ぱー）'
        ]
        ]
        , ['index' => 60, 'kr_names' => [
        '(승리)'
        ], 'jpn_names' => [
        '（ぴーす）'
        ]
        ]
        , ['index' => 61, 'kr_names' => [
        '(최고)'
        , '(일등)'
        ], 'jpn_names' => [
        '（ぐっど）'
        , '（一番）'
        ]
        ]
        , ['index' => 62, 'kr_names' => [
        '(꼴찌)'
        , '(꼴등)'
        ], 'jpn_names' => [
        '（ぶー）'
        , '（ビリ）'
        ]
        ]
        , ['index' => 63, 'kr_names' => [
        '(좋아)'
        , '(오케)'
        ], 'jpn_names' => [
        '（おーけー）'
        , '（OK）'
        ]
        ]
        , ['index' => 64, 'kr_names' => [
        '(정지)'
        ], 'jpn_names' => [
        '（まて）'
        ]
        ]
        , ['index' => 65, 'kr_names' => [
        '(검지)'
        , '(손가락)'
        ], 'jpn_names' => [
        '（それ）'
        , '（指）'
        ]
        ]
    ];

    $emoticon = [
        ['index' => 1, 'kr_names' => [
        '(긴장)'
        ], 'jpn_names' => [
        '(tense)'
        ]
        ]
        , ['index' => 2, 'kr_names' => [
        '(썰렁)'
        ], 'jpn_names' => [
        '(oo;)'
        ]
        ]
        , ['index' => 3, 'kr_names' => [
        '(하하)'
        , '(웃음)'
        ], 'jpn_names' => [
        '(:D)'
        , '(:D)'
        ]
        ]
        , ['index' => 4, 'kr_names' => [
        '(미안)'
        ], 'jpn_names' => [
        '(sorry)'
        ]
        ]
        , ['index' => 5, 'kr_names' => [
        '(미소)'
        ], 'jpn_names' => [
        '(:])'
        ]
        ]
        , ['index' => 6, 'kr_names' => [
        '(화남)'
        ], 'jpn_names' => [
        '(mad)'
        ]
        ]
        , ['index' => 7, 'kr_names' => [
        '(놀람)'
        , '(헉)'
        ], 'jpn_names' => [
        '(OO)'
        , '(OO)'
        ]
        ]
        , ['index' => 8, 'kr_names' => [
        '(새침)'
        ], 'jpn_names' => [
        '(:>)'
        ]
        ]
        , ['index' => 9, 'kr_names' => [
        '(윙크)'
        ], 'jpn_names' => [
        '(wink)'
        ]
        ]
        , ['index' => 10, 'kr_names' => [
        '(요원)'
        , '(감시)'
        ], 'jpn_names' => [
        '(cool)'
        , '(cool)'
        ]
        ]
        , ['index' => 11, 'kr_names' => [
        '(두근)'
        ], 'jpn_names' => [
        '(love)'
        ]
        ]
        , ['index' => 12, 'kr_names' => [
        '(흑흑)'
        , '(울음)'
        , '(눈물)'
        ], 'jpn_names' => [
        '(T.T)'
        , '(T.T)'
        , '(涙)'
        ]
        ]
        , ['index' => 13, 'kr_names' => [
        '(엉엉)'
        , '(통곡)'
        ], 'jpn_names' => [
        '(sob)'
        , '(sob)'
        ]
        ]
        , ['index' => 14, 'kr_names' => [
        '(감동)'
        ], 'jpn_names' => [
        '(:[)'
        ]
        ]
        , ['index' => 15, 'kr_names' => [
        '(민망)'
        ], 'jpn_names' => [
        '(:/)'
        ]
        ]
        , ['index' => 16, 'kr_names' => [
        '(절망)'
        , '(좌절)'
        ], 'jpn_names' => [
        '(D:)'
        , '(D:)'
        ]
        ]
        , ['index' => 17, 'kr_names' => [
        '(구토)'
        ], 'jpn_names' => [
        '(puke)'
        ]
        ]
        , ['index' => 18, 'kr_names' => [
        '(현기증)'
        ], 'jpn_names' => [
        '(:S)'
        ]
        ]
        , ['index' => 19, 'kr_names' => [
        '(인사)'
        , '(안녕)'
        ], 'jpn_names' => [
        '(bow)'
        , '(Hellow)'
        ]
        ]
        , ['index' => 20, 'kr_names' => [
        '(메롱)'
        ], 'jpn_names' => [
        '(:P)'
        ]
        ]
        , ['index' => 21, 'kr_names' => [
        '(거만)'
        ], 'jpn_names' => [
        '(:})'
        ]
        ]
        , ['index' => 22, 'kr_names' => [
        '(헤헤)'
        , '(캬캬)'
        ], 'jpn_names' => [
        '(lol)'
        , '(lol)'
        ]
        ]
        , ['index' => 23, 'kr_names' => [
        '(무안)'
        ], 'jpn_names' => [
        '(shame)'
        ]
        ]
        , ['index' => 24, 'kr_names' => [
        '(박수)'
        ], 'jpn_names' => [
        '(clap)'
        ]
        ]
        , ['index' => 25, 'kr_names' => [
        '(아니)'
        ], 'jpn_names' => [
        '(no)'
        ]
        ]
        , ['index' => 26, 'kr_names' => [
        '(꼬깔)'
        ], 'jpn_names' => [
        '(hat)'
        ]
        ]
        , ['index' => 27, 'kr_names' => [
        '(사랑)'
        ], 'jpn_names' => [
        '(<3)'
        ]
        ]
        , ['index' => 28, 'kr_names' => [
        '(이별)'
        ], 'jpn_names' => [
        htmlspecialchars('(</3)')
        ]
        ]
        , ['index' => 29, 'kr_names' => [
        '(케이크)'
        , '(케익)'
        , '(케잌)'
        ], 'jpn_names' => [
        '(cake)'
        , '(cake)'
        , '(ケーキ)'
        ]
        ]
        , ['index' => 30, 'kr_names' => [
        '(선물)'
        ], 'jpn_names' => [
        '(gift)'
        ]
        ]
        , ['index' => 31, 'kr_names' => [
        '(편지)'
        ], 'jpn_names' => [
        '(mail)'
        ]
        ]
        , ['index' => 32, 'kr_names' => [
        '(입술)'
        ], 'jpn_names' => [
        '(kiss)'
        ]
        ]
        , ['index' => 33, 'kr_names' => [
        '(장미)'
        ], 'jpn_names' => [
        '(rose)'
        ]
        ]
        , ['index' => 34, 'kr_names' => [
        '(당근)'
        ], 'jpn_names' => [
        '(<)'
        ]
        ]
        , ['index' => 35, 'kr_names' => [
        '(맥주)'
        ], 'jpn_names' => [
        '(beer)'
        ]
        ]
        , ['index' => 36, 'kr_names' => [
        '(커피)'
        ], 'jpn_names' => [
        '(joe)'
        ]
        ]
        , ['index' => 37, 'kr_names' => [
        '(와인)'
        ], 'jpn_names' => [
        '(v)'
        ]
        ]
        , ['index' => 38, 'kr_names' => [
        '(펭귄)'
        ], 'jpn_names' => [
        '(pen)'
        ]
        ]
        , ['index' => 39, 'kr_names' => [
        '(뼈다귀)'
        ], 'jpn_names' => [
        '(bone)'
        ]
        ]
        , ['index' => 40, 'kr_names' => [
        '(전구)'
        ], 'jpn_names' => [
        '(idea)'
        ]
        ]
        , ['index' => 41, 'kr_names' => [
        '(전화)'
        ], 'jpn_names' => [
        '(tel)'
        ]
        ]
        , ['index' => 42, 'kr_names' => [
        '(음표)'
        , '(음악)'
        ], 'jpn_names' => [
        '(note)'
        , '(note)'
        ]
        ]
        , ['index' => 43, 'kr_names' => [
        '(악마)'
        ], 'jpn_names' => [
        '(horn)'
        ]
        ]
        , ['index' => 44, 'kr_names' => [
        '(천사)'
        ], 'jpn_names' => [
        '(halo)'
        ]
        ]
        , ['index' => 45, 'kr_names' => [
        '(해)'
        , '(태양)'
        ], 'jpn_names' => [
        '(sun)'
        , '(sun)'
        ]
        ]
        , ['index' => 46, 'kr_names' => [
        '(불)'
        ], 'jpn_names' => [
        '(fire)'
        ]
        ]
        , ['index' => 47, 'kr_names' => [
        '(눈사람)'
        , '(눈)'
        ], 'jpn_names' => [
        '(snow)'
        , '(Snowy)'
        ]
        ]
        , ['index' => 48, 'kr_names' => [
        '(땀)'
        ], 'jpn_names' => [
        '(tear)'
        ]
        ]
        , ['index' => 49, 'kr_names' => [
        '(비)'
        ], 'jpn_names' => [
        '(Rainy)'
        ]
        ]
        , ['index' => 50, 'kr_names' => [
        '(번개)'
        , '(벼락)'
        ], 'jpn_names' => [
        '(boom)'
        , '(boom)'
        ]
        ]
        , ['index' => 51, 'kr_names' => [
        '(별)'
        ], 'jpn_names' => [
        '(star)'
        ]
        ]
        , ['index' => 52, 'kr_names' => [
        '(우산)'
        ], 'jpn_names' => [
        '(umb)'
        ]
        ]
        , ['index' => 53, 'kr_names' => [
        '(시간)'
        ], 'jpn_names' => [
        '(time)'
        ]
        ]
        , ['index' => 54, 'kr_names' => [
        '(팡)'
        ], 'jpn_names' => [
        '(PP)'
        ]
        ]
        , ['index' => 55, 'kr_names' => [
        '(폭죽)'
        ], 'jpn_names' => [
        '(pop)'
        ]
        ]
        , ['index' => 56, 'kr_names' => [
        '(핸드폰)'
        ], 'jpn_names' => [
        '(cell)'
        ]
        ]
        , ['index' => 57, 'kr_names' => [
        '(가위)'
        ], 'jpn_names' => [
        '(>)'
        ]
        ]
        , ['index' => 58, 'kr_names' => [
        '(바위)'
        ], 'jpn_names' => [
        '(rock)'
        ]
        ]
        , ['index' => 59, 'kr_names' => [
        '(보)'
        ], 'jpn_names' => [
        '(five)'
        ]
        ]
        , ['index' => 60, 'kr_names' => [
        '(승리)'
        ], 'jpn_names' => [
        '(peace)'
        ]
        ]
        , ['index' => 61, 'kr_names' => [
        '(최고)'
        , '(일등)'
        ], 'jpn_names' => [
        '(up)'
        , '(up)'
        ]
        ]
        , ['index' => 62, 'kr_names' => [
        '(꼴찌)'
        , '(꼴등)'
        ], 'jpn_names' => [
        '(down)'
        , '(down)'
        ]
        ]
        , ['index' => 63, 'kr_names' => [
        '(좋아)'
        , '(오케)'
        ], 'jpn_names' => [
        '(ok)'
        , '(ok)'
        ]
        ]
        , ['index' => 64, 'kr_names' => [
        '(정지)'
        ], 'jpn_names' => [
        '(stop)'
        ]
        ]
        , ['index' => 65, 'kr_names' => [
        '(검지)'
        , '(손가락)'
        ], 'jpn_names' => [
        '(one)'
        , '(one)'
        ]
        ]
    ];

    for ($i = 0; $i < count($emoticon); $i++) {

        if ($i == 0)
            echo '<tr height="24">';
        else if ($i % 9 == 0)
            echo '<td></td></tr><tr height="24">';
        
        echo '<td width="26" height="24" align="center" valign="center" class="tooltip" onclick="putInText(null, \''.$emoticon[$i]['jpn_names'][0].'\')"><span class="tooltiptext">&nbsp;&nbsp;';
        
        for ($j = 0; $j < count($emoticon[$i]['jpn_names']); $j++) {
            
            echo ($j == 0 ? "" : ", ").'<span class="tooltiplink" onclick="event.stopPropagation(); putInText(this)">'.$emoticon[$i]['jpn_names'][$j].'</span>';
        }
        
        echo '&nbsp;&nbsp;</span></td>';
    }

    echo '<tr height="100%"></tr>';

    echo '</table>';

    echo '		</td>';
    echo '	</tr>
            <tr>
                <td height="50" align="left" valign="top">
                    <div style="padding-left: 65px">
                        <input id="invert" type="checkbox">
                        Inverter
                    </div>
                </td>
            </tr>
            <tr>
                <td align="center" valign="center" height="50" style="font-size: 15px">
                    Seu nickname tem que ter o comprimento de 16
                </td>
            </tr>
            <tr height="35">
                <td align="center">';

                if ($msg != '') {
                    echo '<span style="font-size: 14px; color: '.($msg['state'] == 'OK' ? 'green' : 'red').'">'.$msg['message'].'</span>';
                }

    echo		'</td>
            </tr>
            <tr>
                <td align="center">';

    echo '<form method="POST" style="width: 256px">';

    echo '<span style="display: block; text-align: left;">Nickname:<br></span>
        <input id="nickname" name="nickname" type="text" value="'.($nickname != '' ? $nickname : '').'" style="width: 100%; height: 20px; margin: 0; padding: 0; border: 1px solid black">';
    echo '<input type="submit" value="Check nickname" style="margin-top: 20px;  width: 100%; height: 24px;">';

    echo '</form>';

    // Table interna
    echo '		</td>
            </tr>
        </table>';

    // Table principal
    echo '		</td>
            </tr>
        </table>';

    echo '</body>
        </html>'
?>