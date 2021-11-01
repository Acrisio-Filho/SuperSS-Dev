<?php
    // Arquivo item_list.php
    // Criado em 03/08/2020 as 07:27 por Acrisio
    // Definição da página dinâmica de item list

    $items = json_decode(file_get_contents("config/list_item_itens.json"));

    if (!$items) {

        error_log('Error: load list_item_itens.json. ret: '.$items);

        Header("Location: \\");

        exit();
    }

    $arr_itens = [];

    foreach ($items as $el) {
        foreach($el as $k => $ell) {
            foreach($ell as $el2) {
                array_push($arr_itens, $el2);
            }
        }
    }

    function cmp_itens($a, $b) {
        return $a->id > $b->id;
    }

    usort($arr_itens, "cmp_itens");

    echo '<!DOCTYPE html>
    <html lang="pt-BR">
    <head>
        <meta charset="UTF-8">
        <link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
        <link rel="icon" href="/favicon.ico" type="image/x-icon">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Donate Item table</title>
        
        <style>
            table {
                width: 80%;
            }
            
            table, th, td 
            {
                border: 1px solid black;
                border-collapse: collapse;
            }
            
            th, td 
            { 
                padding: 5px;
                text-align: left;
            }
            
            th
            {
                background-color: black;
                color: white;
            }
            
            tr:nth-child(even) 
            {
                background-color: #eee;
            }
            
            tr:nth-child(odd) 
            {
                background-color: white;
            }
        </style>
    </head>
    <body>
    
    <h1>Itens Diversos</h1>';

    echo '<table><tr><th>ID</th><th style="display: none"></th><th>Nome</th><th>Image</th><th>Description</th></tr>';

    foreach ($arr_itens as $el2) {

        echo '<tr><td>'.$el2->id.'</td><td style="display: none">'.$el2->_typeid.'</td><td>'.$el2->name.'</td><td>';

        if (!$el2->is_setitem)
            echo '<img src="img/item/'.urlencode($el2->icon).'.png">';
        else {

            foreach ($el2->set_item as $set) {
                if ($set->icon != '')
                    echo '<img src="img/item/'.urlencode($set->icon).'.png">';
            }
        }
        
        echo '</td><td>'.$el2->description.'</td></tr>';
    }

    echo '</table>';

    echo '<h1>Card</h1><table><tr><th>ID</th><th style="display: none"></th><th>Nome</th><th>Volume</th><th>Image</th><th>Description</th></tr><tr><td>1</td><td  style="display: none">2080374784</td><td>ケン(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_char_00.png"></img></td><td>Spin Slot +1</td></tr><tr><td>2</td><td  style="display: none">2080374785</td><td>ケン(レア)</td><td>1</td><td><img src="img/item/card_icon_char_01.png"></img></td><td>Control Slot +1</td></tr><tr><td>3</td><td  style="display: none">2080374786</td><td>ケン(シークレット)</td><td>3</td><td><img src="img/item/card_icon_char_02.png"></img></td><td>Control slot +2
    Spin slot +1
    Distance -1y</td></tr><tr><td>4</td><td  style="display: none">2080374787</td><td>ケン(スーパーレア)</td><td>2</td><td><img src="img/item/card_icon_char_03.png"></img></td><td>Control slot +2, Spin slot +1, Distance -2y</td></tr><tr><td>5</td><td  style="display: none">2080374788</td><td>エリカ(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_char_04.png"></img></td><td>Curve Slot +1</td></tr><tr><td>6</td><td  style="display: none">2080374789</td><td>エリカ(レア)</td><td>1</td><td><img src="img/item/card_icon_char_05.png"></img></td><td>Curve Slot +2</td></tr><tr><td>7</td><td  style="display: none">2080374790</td><td>エリカ(スーパーレア)</td><td>1</td><td><img src="img/item/card_icon_char_06.png"></img></td><td>Curve Slot +3</td></tr><tr><td>8</td><td  style="display: none">2080374791</td><td>エリカ(シークレット)</td><td>2</td><td><img src="img/item/card_icon_char_07.png"></img></td><td>Curve slot +4, Control slot +1</td></tr><tr><td>9</td><td  style="display: none">2080374792</td><td>ダイスケ(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_char_08.png"></img></td><td>Spin Slot +1</td></tr><tr><td>10</td><td  style="display: none">2080374793</td><td>ダイスケ(レア)</td><td>2</td><td><img src="img/item/card_icon_char_09.png"></img></td><td>Spin slot+2</td></tr><tr><td>11</td><td  style="display: none">2080374794</td><td>ダイスケ(スーパーレア)</td><td>2</td><td><img src="img/item/card_icon_char_10.png"></img></td><td>Character Cards enhance your abilities. 1 Character Card can be used per outfit.</td></tr><tr><td>12</td><td  style="display: none">2080374795</td><td>ダイスケ(シークレット)</td><td>3</td><td><img src="img/item/card_icon_char_11.png"></img></td><td>Spin slot +3
    Control slot +1</td></tr><tr><td>13</td><td  style="display: none">2080374796</td><td>クー(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_char_12.png"></img></td><td>Power Slot +1</td></tr><tr><td>14</td><td  style="display: none">2080374797</td><td>クー(レア)</td><td>1</td><td><img src="img/item/card_icon_char_13.png"></img></td><td>Control Slot +1</td></tr><tr><td>15</td><td  style="display: none">2080374798</td><td>クー(スーパーレア)</td><td>3</td><td><img src="img/item/card_icon_char_14.png"></img></td><td>Control slot +2
    Spin slot +1
    Distance -2y</td></tr><tr><td>16</td><td  style="display: none">2080374799</td><td>クー(シークレット)</td><td>5</td><td><img src="img/item/card_icon_char_15.png"></img></td><td>コントロールスロット+3 パワースロット+1 飛距離-1ｙ</td></tr><tr><td>17</td><td  style="display: none">2080374800</td><td>セシリア(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_char_16.png"></img></td><td>Accuracy Slot +1</td></tr><tr><td>18</td><td  style="display: none">2080374801</td><td>セシリア(レア)</td><td>1</td><td><img src="img/item/card_icon_char_17.png"></img></td><td>Accuracy Slot +2</td></tr><tr><td>19</td><td  style="display: none">2080374802</td><td>セシリア(スーパーレア)</td><td>1</td><td><img src="img/item/card_icon_char_18.png"></img></td><td>Accuracy Slot +2, Curve Slot +1</td></tr><tr><td>20</td><td  style="display: none">2080374803</td><td>セシリア(シークレット)</td><td>4</td><td><img src="img/item/card_icon_char_19.png"></img></td><td>Control Slot + 1 / Accuracy Slot + 3
    (Card Pack Vol.4)</td></tr><tr><td>21</td><td  style="display: none">2080374804</td><td>マックス(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_char_20.png"></img></td><td>Power Slot +1</td></tr><tr><td>22</td><td  style="display: none">2080374805</td><td>マックス(レア)</td><td>1</td><td><img src="img/item/card_icon_char_21.png"></img></td><td>Power Slot +1, Accuracy Slot +1</td></tr><tr><td>23</td><td  style="display: none">2080374806</td><td>マックス(スーパーレア)</td><td>1</td><td><img src="img/item/card_icon_char_22.png"></img></td><td>Power Slot +2, Accuracy Slot +1</td></tr><tr><td>24</td><td  style="display: none">2080374807</td><td>マックス(シークレット)</td><td>4</td><td><img src="img/item/card_icon_char_23.png"></img></td><td>Power Slot + 3 / Control Slot + 1
    (Card Pack Vol.4)</td></tr><tr><td>25</td><td  style="display: none">2080374808</td><td>アリン(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_char_24.png"></img></td><td>Accuracy Slot +1</td></tr><tr><td>26</td><td  style="display: none">2080374809</td><td>アリン(レア)</td><td>1</td><td><img src="img/item/card_icon_char_25.png"></img></td><td>Accuracy Slot +1, Curve Slot +1</td></tr><tr><td>27</td><td  style="display: none">2080374810</td><td>アリン(スーパーレア)</td><td>1</td><td><img src="img/item/card_icon_char_26.png"></img></td><td>Accuracy Slot +1, Curve Slot +2</td></tr><tr><td>28</td><td  style="display: none">2080374811</td><td>アリン(シークレット)</td><td>5</td><td><img src="img/item/card_icon_char_27.png"></img></td><td>正確度スロット+2 カーブスロット+3</td></tr><tr><td>29</td><td  style="display: none">2080374812</td><td>カズ(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_char_28.png"></img></td><td>Spin Slot +1</td></tr><tr><td>30</td><td  style="display: none">2080374813</td><td>カズ(レア)</td><td>1</td><td><img src="img/item/card_icon_char_29.png"></img></td><td>Power Slot +1, Spin Slot +1</td></tr><tr><td>31</td><td  style="display: none">2080374814</td><td>カズ(スーパーレア)</td><td>2</td><td><img src="img/item/card_icon_char_30.png"></img></td><td>Power slot +1, Spin slot+2</td></tr><tr><td>32</td><td  style="display: none">2080374815</td><td>カズ(シークレット)</td><td>4</td><td><img src="img/item/card_icon_char_31.png"></img></td><td>Control Slot + 1 / Spin Slot + 3
    (Card Pack Vol.4)</td></tr><tr><td>33</td><td  style="display: none">2080374816</td><td>ルーシア(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_char_32.png"></img></td><td>Curve slot+1</td></tr><tr><td>34</td><td  style="display: none">2080374817</td><td>ルーシア(レア)</td><td>2</td><td><img src="img/item/card_icon_char_33.png"></img></td><td>Spin slot +1, Curve slot +1</td></tr><tr><td>35</td><td  style="display: none">2080374818</td><td>ケン(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_char_34.png"></img></td><td>Spin slot+1</td></tr><tr><td>36</td><td  style="display: none">2080374819</td><td>マックス(レア)</td><td>2</td><td><img src="img/item/card_icon_char_35.png"></img></td><td>Power slot +1, Accuracy slot +1</td></tr><tr><td>37</td><td  style="display: none">2080374820</td><td>セシリア(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_char_36.png"></img></td><td>Accuracy Slot +1</td></tr><tr><td>38</td><td  style="display: none">2080374821</td><td>クー(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_char_37.png"></img></td><td>Power slot +1</td></tr><tr><td>39</td><td  style="display: none">2080374822</td><td>アリン(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_char_38.png"></img></td><td>Accuracy slot +1</td></tr><tr><td>40</td><td  style="display: none">2080374823</td><td>カズ(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_char_39.png"></img></td><td>Spin slot +1</td></tr><tr><td>41</td><td  style="display: none">2080374824</td><td>エリカ(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_char_40.png"></img></td><td>Curve Slot +1</td></tr><tr><td>42</td><td  style="display: none">2080374825</td><td>ダイスケ(レア)</td><td>3</td><td><img src="img/item/card_icon_char_41.png"></img></td><td>Spin slot+2</td></tr><tr><td>43</td><td  style="display: none">2080374826</td><td>マックス(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_char_42.png"></img></td><td>Power slot +1</td></tr><tr><td>44</td><td  style="display: none">2080374827</td><td>クー(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_char_43.png"></img></td><td>Power slot +1</td></tr><tr><td>45</td><td  style="display: none">2080374828</td><td>カズ(レア)</td><td>3</td><td><img src="img/item/card_icon_char_44.png"></img></td><td>Power slot +1, Spin slot+1</td></tr><tr><td>46</td><td  style="display: none">2080374829</td><td>ルーシア(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_char_45.png"></img></td><td>Curve Slot +1</td></tr><tr><td>47</td><td  style="display: none">2080374830</td><td>ルーシア(スーパーレア)</td><td>3</td><td><img src="img/item/card_icon_char_46.png"></img></td><td>Spin slot +1, Curve slot +2</td></tr><tr><td>48</td><td  style="display: none">2080374831</td><td>ルーシア(シークレット)</td><td>3</td><td><img src="img/item/card_icon_char_47.png"></img></td><td>Spin slot +1, Curve slot +3</td></tr><tr><td>49</td><td  style="display: none">2080374832</td><td>ネル(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_char_48.png"></img></td><td>Accuracy Slot +1</td></tr><tr><td>50</td><td  style="display: none">2080374833</td><td>ネル(レア)</td><td>3</td><td><img src="img/item/card_icon_char_49.png"></img></td><td>Control Slot +1</td></tr><tr><td>51</td><td  style="display: none">2080374834</td><td>ネル(スーパーレア)</td><td>3</td><td><img src="img/item/card_icon_char_50.png"></img></td><td>Control slot +2
    Spin slot +1
    Distance -2y</td></tr><tr><td>52</td><td  style="display: none">2080374835</td><td>ネル(シークレット)</td><td>3</td><td><img src="img/item/card_icon_char_51.png"></img></td><td>Control slot +2
    Spin slot +1
    Distance -1y</td></tr><tr><td>53</td><td  style="display: none">2080374836</td><td>スピカ(ノーマル)</td><td>4</td><td><img src="img/item/card_icon_char_52.png"></img></td><td>Spin Slot + 1
    (Card Pack Vol.4)</td></tr><tr><td>54</td><td  style="display: none">2080374837</td><td>スピカ(レア)</td><td>3</td><td><img src="img/item/card_icon_char_53.png"></img></td><td>Spin Slot +2</td></tr><tr><td>55</td><td  style="display: none">2080374838</td><td>スピカ(スーパーレア)</td><td>4</td><td><img src="img/item/card_icon_char_54.png"></img></td><td>Control Slot + 1 / Spin Slot + 2
    (Card Pack Vol.4)</td></tr><tr><td>56</td><td  style="display: none">2080374839</td><td>スピカ(シークレット)</td><td>3</td><td><img src="img/item/card_icon_char_55.png"></img></td><td>Control Slot +2, Spin Slot +3, Distance -2y</td></tr><tr><td>57</td><td  style="display: none">2080374840</td><td>ダイスケ(ノーマル)</td><td>4</td><td><img src="img/item/card_icon_char_56.png"></img></td><td>Spin Slot + 1
    (Card Pack Vol.4)</td></tr><tr><td>58</td><td  style="display: none">2080374841</td><td>クー(レア)</td><td>4</td><td><img src="img/item/card_icon_char_57.png"></img></td><td>Control Slot + 1
    (Card Pack Vol.4)</td></tr><tr><td>59</td><td  style="display: none">2080374842</td><td>遠坂凛（スーパーレア）</td><td>100</td><td><img src="img/item/card_icon_char_58.png"></img></td><td>パワースロット+1 スピンスロット+2
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>60</td><td  style="display: none">2080374843</td><td>遠坂凛（シークレット）</td><td>100</td><td><img src="img/item/card_icon_char_59.png"></img></td><td>コントロールスロット+2 スピンスロット+1 飛距離-1y
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>61</td><td  style="display: none">2080374844</td><td>テリシア(レア)</td><td>100</td><td><img src="img/item/card_icon_char_60.png"></img></td><td>パワースロット+1 正確度スロット+1</td></tr><tr><td>62</td><td  style="display: none">2080374845</td><td>キョンシー(ノーマル)</td><td>100</td><td><img src="img/item/card_icon_char_61.png"></img></td><td>正確度スロット+1</td></tr><tr><td>63</td><td  style="display: none">2080374846</td><td>バフォメット(ノーマル)</td><td>100</td><td><img src="img/item/card_icon_char_62.png"></img></td><td>スピンスロット+1</td></tr><tr><td>64</td><td  style="display: none">2080374847</td><td>アルケー(ノーマル)</td><td>100</td><td><img src="img/item/card_icon_char_63.png"></img></td><td>パワースロット+1</td></tr><tr><td>65</td><td  style="display: none">2080374848</td><td>ドミニオン(ノーマル)</td><td>100</td><td><img src="img/item/card_icon_char_64.png"></img></td><td>カーブスロット+1</td></tr><tr><td>66</td><td  style="display: none">2080374849</td><td>リリム(ノーマル)</td><td>100</td><td><img src="img/item/card_icon_char_65.png"></img></td><td>カーブスロット+1</td></tr><tr><td>67</td><td  style="display: none">2080374850</td><td>ケット・シー(ノーマル)</td><td>100</td><td><img src="img/item/card_icon_char_66.png"></img></td><td>正確度スロット+1</td></tr><tr><td>68</td><td  style="display: none">2080374851</td><td>サキュバス(ノーマル)</td><td>100</td><td><img src="img/item/card_icon_char_67.png"></img></td><td>正確度スロット+1</td></tr><tr><td>69</td><td  style="display: none">2080374852</td><td>アルケー(シークレット)</td><td>100</td><td><img src="img/item/card_icon_char_68.png"></img></td><td>コントロールスロット+3 パワースロット+1</td></tr><tr><td>70</td><td  style="display: none">2080374853</td><td>ケット・シー(シークレット)</td><td>100</td><td><img src="img/item/card_icon_char_69.png"></img></td><td>スピンスロット+2 / カーブスロット+2</td></tr><tr><td>71</td><td  style="display: none">2080374854</td><td>リリム(シークレット)</td><td>100</td><td><img src="img/item/card_icon_char_70.png"></img></td><td>カーブスロット+4 / コントロールスロット+1</td></tr><tr><td>72</td><td  style="display: none">2080374855</td><td>サキュバス(シークレット)</td><td>100</td><td><img src="img/item/card_icon_char_71.png"></img></td><td>パワースロット+3 / コントロールスロット+1</td></tr><tr><td>73</td><td  style="display: none">2080374856</td><td>ドミニオン(シークレット)</td><td>100</td><td><img src="img/item/card_icon_char_72.png"></img></td><td>スピンスロット+1 / カーブスロット+3</td></tr><tr><td>74</td><td  style="display: none">2080374857</td><td>キョンシー(シークレット)</td><td>100</td><td><img src="img/item/card_icon_char_73.png"></img></td><td>コントロールスロット+2 / スピンスロット+2</td></tr><tr><td>75</td><td  style="display: none">2080374858</td><td>バフォメット(シークレット)</td><td>100</td><td><img src="img/item/card_icon_char_74.png"></img></td><td>コントロールスロット+1 / スピンスロット+3</td></tr><tr><td>76</td><td  style="display: none">2084569088</td><td>ポンタ(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_caddie_00.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>77</td><td  style="display: none">2084569089</td><td>ポンタ(レア)</td><td>1</td><td><img src="img/item/card_icon_caddie_01.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>78</td><td  style="display: none">2084569090</td><td>ポンタ(スーパーレア)</td><td>2</td><td><img src="img/item/card_icon_caddie_02.png"></img></td><td>Success rate increases (high)</td></tr><tr><td>79</td><td  style="display: none">2084569091</td><td>ポンタ(シークレット)</td><td>4</td><td><img src="img/item/card_icon_caddie_03.png"></img></td><td>Increase the chance of active items (SP)
    If you have multiple caddie cards equpped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>80</td><td  style="display: none">2084569092</td><td>ピピン(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_caddie_04.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>81</td><td  style="display: none">2084569093</td><td>ピピン(レア)</td><td>3</td><td><img src="img/item/card_icon_caddie_05.png"></img></td><td>Maximum distance increases (mid)</td></tr><tr><td>82</td><td  style="display: none">2084569094</td><td>ピピン(スーパーレア)</td><td>3</td><td><img src="img/item/card_icon_caddie_06.png"></img></td><td>Maximum distance increases (high)</td></tr><tr><td>83</td><td  style="display: none">2084569095</td><td>ピピン(シークレット)</td><td>3</td><td><img src="img/item/card_icon_caddie_07.png"></img></td><td>Maximum distance increases (super)</td></tr><tr><td>84</td><td  style="display: none">2084569096</td><td>タンプー（レア）</td><td>1</td><td><img src="img/item/card_icon_caddie_08.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>85</td><td  style="display: none">2084569097</td><td>タンプー(スーパーレア)</td><td>5</td><td><img src="img/item/card_icon_caddie_09.png"></img></td><td>風速が 6～9mの時、風速減少(大)
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>86</td><td  style="display: none">2084569098</td><td>タンプー(シークレット)</td><td>2</td><td><img src="img/item/card_icon_caddie_10.png"></img></td><td>Wind decreases when wind is 1~5m (low)
    Wind decreases when wind is 6~9m (high)</td></tr><tr><td>87</td><td  style="display: none">2084569099</td><td>タンプー(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_caddie_11.png"></img></td><td>Wind decreases when wind is 9m (low)</td></tr><tr><td>88</td><td  style="display: none">2084569100</td><td>ドルフ(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_caddie_12.png"></img></td><td>Bound Bonus (low)</td></tr><tr><td>89</td><td  style="display: none">2084569101</td><td>ドルフ(レア)</td><td>2</td><td><img src="img/item/card_icon_caddie_13.png"></img></td><td>Bound Bonus (mid)</td></tr><tr><td>90</td><td  style="display: none">2084569102</td><td>ドルフ(スーパーレア)</td><td>3</td><td><img src="img/item/card_icon_caddie_14.png"></img></td><td>Bound Bonus (high)</td></tr><tr><td>91</td><td  style="display: none">2084569103</td><td>ドルフ(シークレット)</td><td>5</td><td><img src="img/item/card_icon_caddie_15.png"></img></td><td>バウンドボーナス(特)
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>92</td><td  style="display: none">2084569104</td><td>ロロ(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_caddie_16.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>93</td><td  style="display: none">2084569105</td><td>ロロ(レア)</td><td>1</td><td><img src="img/item/card_icon_caddie_17.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>94</td><td  style="display: none">2084569106</td><td>ロロ(スーパーレア)</td><td>1</td><td><img src="img/item/card_icon_caddie_18.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>95</td><td  style="display: none">2084569107</td><td>ロロ(シークレット)</td><td>4</td><td><img src="img/item/card_icon_caddie_19.png"></img></td><td>Increases the Power Shot Distance(SP)
    If you have multiple caddie cards equipped,
    the ones with the same ability won\'t stack.
    (Card Pack Vol.4)</td></tr><tr><td>96</td><td  style="display: none">2084569108</td><td>キューマ(ノーマル)</td><td>1</td><td><img src="img/item/card_icon_caddie_20.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>97</td><td  style="display: none">2084569109</td><td>キューマ(レア)</td><td>1</td><td><img src="img/item/card_icon_caddie_21.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>98</td><td  style="display: none">2084569110</td><td>キューマ(スーパーレア)</td><td>2</td><td><img src="img/item/card_icon_caddie_22.png"></img></td><td>Gauge increased on pangya (high)</td></tr><tr><td>99</td><td  style="display: none">2084569111</td><td>キューマ(シークレット)</td><td>4</td><td><img src="img/item/card_icon_caddie_23.png"></img></td><td>Increase combo gauge with Pangya shots(SP)
    If you have multiple caddie cards equipped,
    the ones with the same ability won\'t stack.
    (Card Pack Vol.4)</td></tr><tr><td>100</td><td  style="display: none">2084569112</td><td>カディエ(スーパーレア)</td><td>1</td><td><img src="img/item/card_icon_caddie_24.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>101</td><td  style="display: none">2084569113</td><td>カディエ(シークレット)</td><td>1</td><td><img src="img/item/card_icon_caddie_25.png"></img></td><td>Similar Caddie Card effects won`t stack.</td></tr><tr><td>102</td><td  style="display: none">2084569115</td><td>ポンタ(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_caddie_27.png"></img></td><td>Success rate increases (low)
    Similar Caddie Card Effects do not stack.</td></tr><tr><td>103</td><td  style="display: none">2084569116</td><td>ピピン(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_caddie_28.png"></img></td><td>Maximum distance increases (low)
    Similar Caddie Card Effects do not stack.</td></tr><tr><td>104</td><td  style="display: none">2084569117</td><td>ティッキー(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_caddie_29.png"></img></td><td>Treasure Point increases (low)</td></tr><tr><td>105</td><td  style="display: none">2084569118</td><td>ティッキー(レア)</td><td>2</td><td><img src="img/item/card_icon_caddie_30.png"></img></td><td>Treasure Point increases (mid)</td></tr><tr><td>106</td><td  style="display: none">2084569119</td><td>ウィンクルピピン(ノーマル)</td><td>2</td><td><img src="img/item/card_icon_caddie_31.png"></img></td><td>Treasure Point increased at Par</td></tr><tr><td>107</td><td  style="display: none">2084569120</td><td>ウィンクルピピン(レア)</td><td>2</td><td><img src="img/item/card_icon_caddie_32.png"></img></td><td>Treasure Point increased at Birdie</td></tr><tr><td>108</td><td  style="display: none">2084569121</td><td>カディエ(レア)</td><td>2</td><td><img src="img/item/card_icon_caddie_33.png"></img></td><td>Impact zone increased at 1 Wind (low)</td></tr><tr><td>109</td><td  style="display: none">2084569122</td><td>ティッキー(スーパーレア)</td><td>3</td><td><img src="img/item/card_icon_caddie_34.png"></img></td><td>Treasure Point Increased (high)</td></tr><tr><td>110</td><td  style="display: none">2084569123</td><td>ウィンクルピピン(スーパーレア)</td><td>3</td><td><img src="img/item/card_icon_caddie_35.png"></img></td><td>Treasure Point increased at Eagle</td></tr><tr><td>111</td><td  style="display: none">2084569124</td><td>カディエ(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_caddie_36.png"></img></td><td>Impact zone increased at 7~9 wind (high)</td></tr><tr><td>113</td><td  style="display: none">2084569129</td><td>ミンティ(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_caddie_39.png"></img></td><td>Starting Gauge is increased (low)</td></tr><tr><td>114</td><td  style="display: none">2084569130</td><td>ミンティ(レア)</td><td>3</td><td><img src="img/item/card_icon_caddie_40.png"></img></td><td>Starting Gauge is increased (mid)</td></tr><tr><td>115</td><td  style="display: none">2084569131</td><td>ミンティ(スーパーレア)</td><td>3</td><td><img src="img/item/card_icon_caddie_41.png"></img></td><td>Starting Gauge is increased (high)</td></tr><tr><td>116</td><td  style="display: none">2084569132</td><td>ミンティ(シークレット)</td><td>3</td><td><img src="img/item/card_icon_caddie_42.png"></img></td><td>Starting Gauge is increased (super)</td></tr><tr><td>117</td><td  style="display: none">2084569133</td><td>ロロ(ノーマル)</td><td>4</td><td><img src="img/item/card_icon_caddie_43.png"></img></td><td>Increases the Power Shot Distance(S)
    If you have multiple caddie cards equpped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>118</td><td  style="display: none">2084569134</td><td>セイバー（スーパーレア）</td><td>100</td><td><img src="img/item/card_icon_caddie_46.png"></img></td><td>インパクトゾーンが増加する（効果小）
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>119</td><td  style="display: none">2084569135</td><td>セイバー（シークレット）</td><td>100</td><td><img src="img/item/card_icon_caddie_47.png"></img></td><td>飛距離増加(特)
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>120</td><td  style="display: none">2084569136</td><td>カレン(スーパーレア)</td><td>100</td><td><img src="img/item/card_icon_caddie_48.png"></img></td><td>インパクトゾーンが増加する（効果小）
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>121</td><td  style="display: none">2084569137</td><td>ロロ(レア)</td><td>100</td><td><img src="img/item/card_icon_caddie_49.png"></img></td><td>パワーショット時の飛距離が増加する（効果中）
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>128</td><td  style="display: none">2084569144</td><td>ピピン(スーパーレア)特典</td><td>100</td><td><img src="img/item/card_icon_caddie_06.png"></img></td><td>飛距離増加(大)
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>129</td><td  style="display: none">2084569145</td><td>タンプー(スーパーレア)特典</td><td>100</td><td><img src="img/item/card_icon_caddie_09.png"></img></td><td>風速が 6～9mの時、風速減少(大)
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>130</td><td  style="display: none">2084569146</td><td>ロロ(スーパーレア)特典</td><td>100</td><td><img src="img/item/card_icon_caddie_18.png"></img></td><td>パワーショット時の飛距離が増加する（効果大）
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>131</td><td  style="display: none">2084569147</td><td>キューマ(スーパーレア)特典</td><td>100</td><td><img src="img/item/card_icon_caddie_22.png"></img></td><td>コンボゲージ増加(大)
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>132</td><td  style="display: none">2084569148</td><td>カディエ(スーパーレア)特典</td><td>100</td><td><img src="img/item/card_icon_caddie_24.png"></img></td><td>インパクトゾーンが増加する（効果小）
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>133</td><td  style="display: none">2084569149</td><td>ミンティ(スーパーレア)特典</td><td>100</td><td><img src="img/item/card_icon_caddie_41.png"></img></td><td>パンヤコンボゲージ初期値増加(大)
    ※キャディカードを2枚以上使用する場合、同じ
    能力値を持っているカードは重複適用されない。</td></tr><tr><td>134</td><td  style="display: none">2088763392</td><td>基礎訓練</td><td>1</td><td><img src="img/item/card_icon_special_00.png"></img></td><td>Exp +10
    (Single use)</td></tr><tr><td>135</td><td  style="display: none">2088763393</td><td>双子の訓練</td><td>5</td><td><img src="img/item/card_icon_special_01.png"></img></td><td>経験値が20増加します。
    ※使用後、削除されます。</td></tr><tr><td>136</td><td  style="display: none">2088763394</td><td>訓練合宿</td><td>5</td><td><img src="img/item/card_icon_special_02.png"></img></td><td>経験値が50増加します。
    ※使用後、削除されます。</td></tr><tr><td>137</td><td  style="display: none">2088763395</td><td>マカロンの心</td><td>1</td><td><img src="img/item/card_icon_special_03.png"></img></td><td>Gain +10% more Pang for 2 hrs
    (Single use)</td></tr><tr><td>138</td><td  style="display: none">2088763396</td><td>マカロンの真心</td><td>3</td><td><img src="img/item/card_icon_special_04.png"></img></td><td>Gain +20% more Pang for 1 hr</td></tr><tr><td>139</td><td  style="display: none">2088763397</td><td>マカロンの魂</td><td>5</td><td><img src="img/item/card_icon_special_05.png"></img></td><td>1時間 獲得PP+50%
    ※使用後、削除されます。
    ※同じ能力値を持っているスペシャルカードと、効果は重複しません。</td></tr><tr><td>140</td><td  style="display: none">2088763398</td><td>シフォンの心</td><td>1</td><td><img src="img/item/card_icon_special_06.png"></img></td><td>Gain +10% more Exp for 2 hrs
    (Single use)</td></tr><tr><td>141</td><td  style="display: none">2088763399</td><td>シフォンの真心</td><td>3</td><td><img src="img/item/card_icon_special_07.png"></img></td><td>Gain +20% more Exp for 1 hr</td></tr><tr><td>142</td><td  style="display: none">2088763400</td><td>シフォンの魂</td><td>5</td><td><img src="img/item/card_icon_special_08.png"></img></td><td>1時間 獲得経験値+40%
    ※使用後、削除されます。
    ※同じ能力値を持っているスペシャルカードと、効果は重複しません。</td></tr><tr><td>143</td><td  style="display: none">2088763401</td><td>PP袋(小)</td><td>5</td><td><img src="img/item/card_icon_special_09.png"></img></td><td>Gives 2000 Pang. Special Cards are for one time use. </td></tr><tr><td>144</td><td  style="display: none">2088763402</td><td>PP袋(中)</td><td>5</td><td><img src="img/item/card_icon_special_10.png"></img></td><td>10000PPが入った袋
    ※使用後、削除されます。</td></tr><tr><td>145</td><td  style="display: none">2088763403</td><td>PP袋(大)</td><td>5</td><td><img src="img/item/card_icon_special_11.png"></img></td><td>50000PPが入った袋
    ※使用後、削除されます。</td></tr><tr><td>146</td><td  style="display: none">2088763404</td><td>ピピンの応援</td><td>1</td><td><img src="img/item/card_icon_special_12.png"></img></td><td>Power is increased by +1 for 2 hrs
    (Single use)</td></tr><tr><td>147</td><td  style="display: none">2088763405</td><td>ピピンの後押し</td><td>5</td><td><img src="img/item/card_icon_special_13.png"></img></td><td>1時間の間、パワーが+2されます。
    ※使用後、削除されます。</td></tr><tr><td>148</td><td  style="display: none">2088763406</td><td>カディエの魔法</td><td>1</td><td><img src="img/item/card_icon_special_14.png"></img></td><td>Control is increased by +1 for 2 hrs
    (Single use)</td></tr><tr><td>149</td><td  style="display: none">2088763407</td><td>カディエの魔術</td><td>5</td><td><img src="img/item/card_icon_special_15.png"></img></td><td>2時間の間、コントロールが+2されます。
    ※使用後、削除されます。</td></tr><tr><td>150</td><td  style="display: none">2088763408</td><td>ティッキーの祝福</td><td>1</td><td><img src="img/item/card_icon_special_16.png"></img></td><td>Accuracy is increased by +1 for 2 hrs
    (Single use)</td></tr><tr><td>151</td><td  style="display: none">2088763409</td><td>ティッキーの祈り</td><td>5</td><td><img src="img/item/card_icon_special_17.png"></img></td><td>1時間の間、正確度が+2されます。
    ※使用後、削除されます。</td></tr><tr><td>152</td><td  style="display: none">2088763410</td><td>ドルフの応援</td><td>1</td><td><img src="img/item/card_icon_special_18.png"></img></td><td>Spin is increased by +1 for 2 hrs
    (Single use)</td></tr><tr><td>153</td><td  style="display: none">2088763411</td><td>ロイの行進</td><td>5</td><td><img src="img/item/card_icon_special_19.png"></img></td><td>2時間の間、スピンが+2されます。
    ※使用後、削除されます。</td></tr><tr><td>154</td><td  style="display: none">2088763412</td><td>キューマの励まし</td><td>1</td><td><img src="img/item/card_icon_special_20.png"></img></td><td>Curve is increased by +1 for 2 hrs
    (Single use)</td></tr><tr><td>155</td><td  style="display: none">2088763413</td><td>キューマの激励</td><td>5</td><td><img src="img/item/card_icon_special_21.png"></img></td><td>1時間の間、カーブが+2されます。
    ※使用後、削除されます。</td></tr><tr><td>156</td><td  style="display: none">2088763414</td><td>キャディ達の祝福</td><td>1</td><td><img src="img/item/card_icon_special_22.png"></img></td><td>Starting Gauge is increased for 2 hrs
    (Single use)</td></tr><tr><td>157</td><td  style="display: none">2088763415</td><td>体力補充パック</td><td>5</td><td><img src="img/item/card_icon_special_23.png"></img></td><td>2時間の間、パンヤコンボゲージの初期値が33増加します。
    ※使用後、削除されます。</td></tr><tr><td>158</td><td  style="display: none">2088763416</td><td>ビリーのかばん</td><td>1</td><td><img src="img/item/card_icon_special_24.png"></img></td><td>Item slots increased by +1 for 2 hrs
    (Single use)</td></tr><tr><td>159</td><td  style="display: none">2088763417</td><td>ビリーの旅行かばん</td><td>5</td><td><img src="img/item/card_icon_special_25.png"></img></td><td>3時間の間、装備アイテムの最大スロット数が+1する。
    ※使用後、削除されます。</td></tr><tr><td>160</td><td  style="display: none">2088763418</td><td>レインボーフェザー</td><td>2</td><td><img src="img/item/card_icon_special_26.png"></img></td><td>Pangya impact zone increased for 1 hour (low)</td></tr><tr><td>161</td><td  style="display: none">2088763419</td><td>セピアウィンド</td><td>2</td><td><img src="img/item/card_icon_special_27.png"></img></td><td>Clear Bonus increases by 10%
    at Sepia Wind for 2 hrs</td></tr><tr><td>162</td><td  style="display: none">2088763420</td><td>ウィンドヒル</td><td>2</td><td><img src="img/item/card_icon_special_28.png"></img></td><td>Clear Bonus increases by 10%
    at Wind Hill for 2 hrs</td></tr><tr><td>163</td><td  style="display: none">2088763421</td><td>ピンクウィンド</td><td>2</td><td><img src="img/item/card_icon_special_29.png"></img></td><td>Clear Bonus increases by 10%
    at Pink Wind for 2 hrs</td></tr><tr><td>164</td><td  style="display: none">2088763422</td><td>ブールムーン</td><td>2</td><td><img src="img/item/card_icon_special_30.png"></img></td><td>Clear Bonus increases by 10%
    at Blue Moon for 2 hrs</td></tr><tr><td>165</td><td  style="display: none">2088763423</td><td>ランダムPP袋(小)</td><td>2</td><td><img src="img/item/card_icon_special_31.png"></img></td><td>Small pouch that contains Pang</td></tr><tr><td>166</td><td  style="display: none">2088763424</td><td>妖精の耳</td><td>2</td><td><img src="img/item/card_icon_special_32.png"></img></td><td>Pangya impact zone increased for 30 mins (mid)</td></tr><tr><td>167</td><td  style="display: none">2088763425</td><td>トレジャーハンター</td><td>2</td><td><img src="img/item/card_icon_special_33.png"></img></td><td>Treasure Point increase for 2 hrs</td></tr><tr><td>168</td><td  style="display: none">2088763426</td><td>中級訓練</td><td>2</td><td><img src="img/item/card_icon_special_34.png"></img></td><td>Exp +25</td></tr><tr><td>169</td><td  style="display: none">2088763427</td><td>マカロンの想い</td><td>2</td><td><img src="img/item/card_icon_special_35.png"></img></td><td>Gain +10% more Pang for 2 hrs</td></tr><tr><td>170</td><td  style="display: none">2088763428</td><td>シフォンの想い</td><td>2</td><td><img src="img/item/card_icon_special_36.png"></img></td><td>Gain +10% more Exp for 2 hrs</td></tr><tr><td>171</td><td  style="display: none">2088763429</td><td>ドルフの傘</td><td>2</td><td><img src="img/item/card_icon_special_37.png"></img></td><td>Chance of rain is increased for 1 hour</td></tr><tr><td>172</td><td  style="display: none">2088763430</td><td>壊れたエボートの欠片</td><td>2</td><td><img src="img/item/card_icon_special_38.png"></img></td><td>Abbot Elemental Shard</td></tr><tr><td>173</td><td  style="display: none">2088763431</td><td>ブルーラグーン(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_special_39.png"></img></td><td>Clear Bonus increased by 10%
    at Blue Lagoon for 2 hrs</td></tr><tr><td>174</td><td  style="display: none">2088763432</td><td>ブルーウォーター(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_special_40.png"></img></td><td>Clear Bonus increased by 10%
    at Blue Water for 2 hrs</td></tr><tr><td>175</td><td  style="display: none">2088763433</td><td>シャイニングサンド(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_special_41.png"></img></td><td>Clear Bonus increased by 10%
    at Shining Sand for 2 hrs</td></tr><tr><td>176</td><td  style="display: none">2088763434</td><td>ディープインフェルノ(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_special_42.png"></img></td><td>Clear Bonus increased by 10%
    at Deep Inferno for 2 hrs</td></tr><tr><td>177</td><td  style="display: none">2088763435</td><td>シルビアキャノン(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_special_43.png"></img></td><td>Clear Bonus increased by 10%
    at Silvia Cannon for 2 hrs</td></tr><tr><td>178</td><td  style="display: none">2088763436</td><td>イースタンバレー(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_special_44.png"></img></td><td>Clear Bonus increased by 10%
    at Eastern Valley for 2 hrs</td></tr><tr><td>179</td><td  style="display: none">2088763437</td><td>ロストシーウェイ(ノーマル)</td><td>3</td><td><img src="img/item/card_icon_special_45.png"></img></td><td>Clear Bonus increased by 10%
    at Lost Seaway for 2 hrs
    </td></tr><tr><td>180</td><td  style="display: none">2088763438</td><td>ミンティの愛</td><td>3</td><td><img src="img/item/card_icon_special_46.png"></img></td><td>Distance +2y for 2 hrs</td></tr><tr><td>181</td><td  style="display: none">2088763439</td><td>ハロウの怒り</td><td>3</td><td><img src="img/item/card_icon_special_47.png"></img></td><td>Starting Gauge is increased for 1 hr (low)</td></tr><tr><td>182</td><td  style="display: none">2088763440</td><td>スノウのかばん</td><td>3</td><td><img src="img/item/card_icon_special_48.png"></img></td><td>Item slot is increased by +1 for 1 hrs</td></tr><tr><td>183</td><td  style="display: none">2088763441</td><td>ドロシーの魔法</td><td>3</td><td><img src="img/item/card_icon_special_49.png"></img></td><td>Gauge is increased at Pangya for 2 hrs (low)</td></tr><tr><td>184</td><td  style="display: none">2088763442</td><td>アメリの助言</td><td>3</td><td><img src="img/item/card_icon_special_50.png"></img></td><td>Gauge is increased at Pangya for 1 hr (mid)</td></tr><tr><td>185</td><td  style="display: none">2088763443</td><td>妖精の耳</td><td>3</td><td><img src="img/item/card_icon_special_51.png"></img></td><td>Pangya impact zone increased for 30 mins (mid)</td></tr><tr><td>186</td><td  style="display: none">2088763444</td><td>中級訓練</td><td>3</td><td><img src="img/item/card_icon_special_52.png"></img></td><td>EXP +25</td></tr><tr><td>187</td><td  style="display: none">2088763445</td><td>アイスインフェルノ（ノーマル）</td><td>4</td><td><img src="img/item/card_icon_special_53.png"></img></td><td>For 2 hours you gain 10% clear bonus
    for the map Ice Inferno.
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>188</td><td  style="display: none">2088763446</td><td>ウィズシティ（ノーマル）</td><td>4</td><td><img src="img/item/card_icon_special_54.png"></img></td><td>For 2 hours you gain 10% clear bonus
    for the map Wiz City.
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>189</td><td  style="display: none">2088763447</td><td>レッドパラソル</td><td>4</td><td><img src="img/item/card_icon_special_55.png"></img></td><td>For an hour if it rains during the game,
    it will rain for 2 more holes.
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>190</td><td  style="display: none">2088763448</td><td>忘却の花</td><td>4</td><td><img src="img/item/card_icon_special_57.png"></img></td><td>For 30 minutes you gain the Mulligan Rose effect.
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>191</td><td  style="display: none">2088763449</td><td>ピピンの応援</td><td>4</td><td><img src="img/item/card_icon_special_60.png"></img></td><td>For 2 hours you gain Power + 1
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>192</td><td  style="display: none">2088763450</td><td>カディエの魔法</td><td>4</td><td><img src="img/item/card_icon_special_61.png"></img></td><td>For 2 hours you gain Control + 1
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>193</td><td  style="display: none">2088763451</td><td>ティッキーの祝福</td><td>4</td><td><img src="img/item/card_icon_special_62.png"></img></td><td>For 2 hours you gain Accuracy + 1
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>194</td><td  style="display: none">2088763452</td><td>ドルフの芸</td><td>4</td><td><img src="img/item/card_icon_special_63.png"></img></td><td>For 2 hours you gain Spin + 1
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>195</td><td  style="display: none">2088763453</td><td>キューマ（？）の励まし</td><td>4</td><td><img src="img/item/card_icon_special_64.png"></img></td><td>For 2 hours you gain Curve + 1
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>196</td><td  style="display: none">2088763454</td><td>キャディ達の祝福</td><td>4</td><td><img src="img/item/card_icon_special_59.png"></img></td><td>For 2 hours your initial pangya combo gauge will increase.
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>197</td><td  style="display: none">2088763455</td><td>スノウのかばん</td><td>4</td><td><img src="img/item/card_icon_special_56.png"></img></td><td>For an hour you gain Item Slot + 1
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>198</td><td  style="display: none">2088763456</td><td>レインボーフェザー</td><td>4</td><td><img src="img/item/card_icon_special_58.png"></img></td><td>For an hour your pangya impact zone will increase(S)
    (Disappears after use)
    (Card Pack Vol.4)</td></tr><tr><td>199</td><td  style="display: none">2088763457</td><td>ソレン(レア)</td><td>4</td><td><img src="img/item/card_icon_special_65.png"></img></td><td>Duostar Artisan
    Used for Ranking up Clubs in the Club Workshop.
    (Card Pack Vol.4)</td></tr><tr><td>200</td><td  style="display: none">2088763458</td><td>Fate stay/night（ノーマル）</td><td>100</td><td><img src="img/item/card_icon_special_66.png"></img></td><td>2時間の間、獲得経験値が10％増加します。
    ※使用後、削除されます。
    ※Vol.2のシフォンと効果は重複しません。
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>201</td><td  style="display: none">2088763459</td><td>セイバー（ノーマル）</td><td>100</td><td><img src="img/item/card_icon_special_67.png"></img></td><td>（30分）忘却化機能
    ※使用後、削除されます。
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>202</td><td  style="display: none">2088763460</td><td>セイバー（レア）</td><td>100</td><td><img src="img/item/card_icon_special_68.png"></img></td><td>30分間パンヤゾーン増加(中)
    ※使用後、削除されます。
    ※同じ能力値を持っているスペシャルカードと、効果は重複しません。
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>203</td><td  style="display: none">2088763461</td><td>遠坂凛（ノーマル）</td><td>100</td><td><img src="img/item/card_icon_special_69.png"></img></td><td>2時間 飛距離増加(中)
    ※使用後、削除されます。
    ※同じ能力値を持っているスペシャルカードと、効果は重複しません。
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>204</td><td  style="display: none">2088763462</td><td>遠坂凛（レア）</td><td>100</td><td><img src="img/item/card_icon_special_70.png"></img></td><td>1時間パンヤコンボゲージ増加(中)
    ※使用後、削除されます。
    ※同じ能力値を持っているスペシャルカードと、効果は重複しません。
    (C)TYPE-MOON・ufotable・FSNPC</td></tr><tr><td>205</td><td  style="display: none">2088763463</td><td>ウィングトロス社の研究</td><td>5</td><td><img src="img/item/card_icon_special_71.png"></img></td><td>1時間 クラブ熟練度+20%
    ※使用後、削除されます。
    ※同じ能力値を持っているスペシャルカードと、効果は重複しません。</td></tr><tr><td>206</td><td  style="display: none">2101346305</td><td>ルカ(ノーマル)</td><td>4</td><td><img src="img/item/card_icon_npc_00.png"></img></td><td>Increase Pang gain (S)
    Pang gain increased by + 5%
    If you have multiple NPC cards equipped,
    the ones with the same ability won\'t stack.
    (Card Pack Vol.4)</td></tr><tr><td>207</td><td  style="display: none">2101346306</td><td>ルカ(レア)</td><td>4</td><td><img src="img/item/card_icon_npc_01.png"></img></td><td>Increase Pang gain(M)
    Pang gain increased by + 10%
    If you have multiple NPC cards equipped,
    the ones with the same ability won\'t stack.
    (Card Pack Vol.4)</td></tr><tr><td>208</td><td  style="display: none">2101346307</td><td>ルカ(スーパーレア)</td><td>4</td><td><img src="img/item/card_icon_npc_02.png"></img></td><td>Increase Pang gain(L)
    Pang gain increased by + 15%
    If you have multiple NPC cards equipped,
    the ones with the same ability won\'t stack.
    (Card Pack Vol.4)</td></tr><tr><td>209</td><td  style="display: none">2101346308</td><td>ルカ(シークレット)</td><td>4</td><td><img src="img/item/card_icon_npc_03.png"></img></td><td>Increase Pang gain(SP)
    Pang gain increased by + 20%
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>210</td><td  style="display: none">2101346309</td><td>ケイマン(ノーマル)</td><td>4</td><td><img src="img/item/card_icon_npc_04.png"></img></td><td>Increase experience gain(S)
    Experience gain increased by + 5%
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>211</td><td  style="display: none">2101346310</td><td>ケイマン(レア)</td><td>4</td><td><img src="img/item/card_icon_npc_05.png"></img></td><td>Increase experience gain(M)
    Experience gain increased by + 10%
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>212</td><td  style="display: none">2101346311</td><td>ケイマン(スーパーレア)</td><td>4</td><td><img src="img/item/card_icon_npc_06.png"></img></td><td>Increase experience gain(L)
    Experience gain increased by + 15%
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>213</td><td  style="display: none">2101346312</td><td>ケイマン(シークレット)</td><td>4</td><td><img src="img/item/card_icon_npc_07.png"></img></td><td>Increase experience gain(SP)
    Experience gain increased by + 20%
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>214</td><td  style="display: none">2101346313</td><td>ミューレン(ノーマル)</td><td>4</td><td><img src="img/item/card_icon_npc_08.png"></img></td><td>Duostar Artisan`s Ability(S)
    Control + 1 if your distance is 240y or higher.
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>215</td><td  style="display: none">2101346314</td><td>ミューレン(レア)</td><td>4</td><td><img src="img/item/card_icon_npc_09.png"></img></td><td>Duostar Artisan`s Ability(M)
    Control + 2 if your distance is 260y or higher.
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>216</td><td  style="display: none">2101346315</td><td>ミューレン(スーパーレア)</td><td>4</td><td><img src="img/item/card_icon_npc_10.png"></img></td><td>Duostar Artisan`s Ability(L)
    Control + 3 if your distance is 280y or higher.
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>217</td><td  style="display: none">2101346316</td><td>ミューレン(シークレット)</td><td>4</td><td><img src="img/item/card_icon_npc_11.png"></img></td><td>Duostar Artisan`s Ability(SP)
    Control + 4 if your distance is 300y or higher.
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>218</td><td  style="display: none">2101346317</td><td>ティタンチャム(ノーマル)</td><td>4</td><td><img src="img/item/card_icon_npc_12.png"></img></td><td>Giga Yard Artisan`s Ability(S)
    Impact zone is increased if your distance is 260y or higher.(S)
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>219</td><td  style="display: none">2101346318</td><td>ティタンチャム(レア)</td><td>4</td><td><img src="img/item/card_icon_npc_13.png"></img></td><td>Giga Yard Artisan`s Ability(M)
    Impact zone is increased if your distance is 280y or higher.(M)
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>220</td><td  style="display: none">2101346319</td><td>ティタンチャム(スーパーレア)</td><td>4</td><td><img src="img/item/card_icon_npc_14.png"></img></td><td>Giga Yard Artisan`s Ability(L)
    Impact zone is increased if your distance is 300y or higher.(L)
    If you have multiple NPC cards equipped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>221</td><td  style="display: none">2101346320</td><td>ティタンチャム(シークレット)</td><td>5</td><td><img src="img/item/card_icon_npc_15.png"></img></td><td>ギガヤーズ社職人の能力(特)
    1Wの飛距離が320y以上の時、 インパクトゾーン増加(特)
    ※パワーショット時の飛距離増加は対象外。
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr><tr><td>222</td><td  style="display: none">2101346321</td><td>ルナーテューム海賊船員(ノーマル)</td><td>4</td><td><img src="img/item/card_icon_npc_16.png"></img></td><td>Long putting bonus(S)
    Successful long putting gives + 30% more bonus Pang
    If you have multiple caddie cards equpped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>223</td><td  style="display: none">2101346322</td><td>ルナーテューム海賊船員(レア)</td><td>5</td><td><img src="img/item/card_icon_npc_17.png"></img></td><td>ロングパットボーナス(中)
    ロングパット成功ボーナスPP+50%
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr><tr><td>224</td><td  style="display: none">2101346323</td><td>ルナーテューム海賊団(スーパーレア)</td><td>5</td><td><img src="img/item/card_icon_npc_18.png"></img></td><td>ロングパットボーナス(大)
    ロングパット成功ボーナスPP+70%
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr><tr><td>225</td><td  style="display: none">2101346324</td><td>ルナーテュームの海賊船(シークレット)</td><td>5</td><td><img src="img/item/card_icon_npc_19.png"></img></td><td>ロングパットボーナス(特)
    ロングパット成功ボーナスPP+100%
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr><tr><td>226</td><td  style="display: none">2101346325</td><td>クリストファーJ(ノーマル)</td><td>5</td><td><img src="img/item/card_icon_npc_20.png"></img></td><td>記録ボーナス(小)
    パー, バーディー , イーグル, アルバトロス,
    ホールインワンボーナスPP+30%
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr><tr><td>227</td><td  style="display: none">2101346326</td><td>クリストファーJ(レア)</td><td>4</td><td><img src="img/item/card_icon_npc_21.png"></img></td><td>Record Bonus(M)
    + 50% Bonus Pang for Par, Birdie, Eagle, Albatross
    and Hole-in-One.
    If you have multiple caddie cards equpped,
    the ones with the same ability won`t stack.
    (Card Pack Vol.4)</td></tr><tr><td>228</td><td  style="display: none">2101346328</td><td>クリストファーJ(シークレット)</td><td>5</td><td><img src="img/item/card_icon_npc_23.png"></img></td><td>記録ボーナス(特)
    パー, バーディー , イーグル, アルバトロス,
    ホールインワンボーナスPP+100%
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr><tr><td>233</td><td  style="display: none">2101346333</td><td>ミューレン(スーパーレア)特典</td><td>100</td><td><img src="img/item/card_icon_npc_10.png"></img></td><td>デュアル社職人の能力(大)
    1Wの飛距離が280y以上の時、 コントロール+3
    ※パワーショット時の飛距離増加は対象外。
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr><tr><td>234</td><td  style="display: none">2101346334</td><td>ティタンチャム(レア)特典</td><td>100</td><td><img src="img/item/card_icon_npc_13.png"></img></td><td>ギガヤーズ社職人の能力(中)
    1Wの飛距離が280y以上の時、 インパクトゾーン増加(中)
    ※パワーショット時の飛距離増加は対象外。
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr><tr><td>235</td><td  style="display: none">2101346335</td><td>クリストファーJ(レア)特典</td><td>100</td><td><img src="img/item/card_icon_npc_21.png"></img></td><td>記録ボーナス(中)
    パー, バーディー , イーグル, アルバトロス,
    ホールインワンボーナスPP+50%
    ※NPCカードを2枚以上使用する場合、
    同じ能力値を持っているカードは重複適用されない。</td></tr></table>';
?>