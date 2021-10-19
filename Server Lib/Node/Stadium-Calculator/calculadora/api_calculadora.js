// Arquivo api_calculadora.js
// Criado em 10/07/2020 as 17:47 por Acrisio
// Definição e Implementação da Calculadora em JavaScript

const CalculadoraBase = require('../common/CalculadoraBaseInject.js');
const types = require('../type/type.js');

const DEBUG_FLAG = false;

// Calipers per percent
const CALIPER_PER_PERCENT = 3.6;

// Other Values per Scale
const VALUE_PER_SCALE = 10;

// Meters to Yards
const METER_TO_YARDS = 1.094

// HWI_NAME
const eHWI_NAME = [
    'percent',
    'caliper',
    'power',
    'hwi',
    'wind_front',
    'wind_back',
    'h_minus',
    'h_plus',
    'terreno',  // Suporte para versões antigas
    'slope'     // Suporte para versões antigas
];

// ALTURA_NAME
const eALTURA_NAME = [
    'alt',
    //'alt_plus',     // Suporte para versões antigas
    'value_minus',
    'value_plus'
];

// ALTURA_NAME_2
const eALTURA_NAME_2 = [
    'alt',
    'value_minus',
    'value_plus'
];

// TERRENO_NAME
const eTERRENO_NAME = [
    'percent',
    'value'
];

// Table HWI
class hwi_table {

    constructor(percent, caliper, pwr, hwi, wind_front, wind_back, h_minus, h_plus, /*Suporte*/ terreno = 0, slope = 0) {

        this.percent = percent;
        this.caliper = caliper;

        if (caliper > 0 && pwr > 0)
            this.diferença = caliper - pwr;
        else
            this.diferença = 0;

        this.power = pwr;
        this.hwi = hwi;
        this.wind_front = wind_front;
        this.wind_back = wind_back;
        this.h_minus = h_minus;
        this.h_plus = h_plus;

        // Suporte para versões antigas
        this.terreno = terreno;
        this.slope = slope;
    }
    
    // new to string
    toString() {
        return "Percent: " + this.percent.toFixed(2) + "\tCaliper: " + this.caliper.toFixed(1) 
                + "\tPower: " + this.power.toFixed(1) + "\tHWI: " + this.hwi.toFixed(3) + "\tWF: " 
                + this.wind_front.toFixed(3) + "\tWB: " + this.wind_back.toFixed(3) + "\tH-: " 
                + this.h_minus.toFixed(3) + "\tH+: " + this.h_plus.toFixed(3);
    };

    toStringWithDiff() {
        return this.toString() + "\Diferença: " + this.diferença.toFixed(3);
    }
};

// Table Altura
class altura_table {

    constructor(alt, value_minus, value_plus) {

        this.alt = alt;
        this.value_minus = value_minus;
        this.value_plus = value_plus;
    }

    toString() {
        return "Altura: " + this.alt_minus.toFixed(2)
                + "\tValor -: " + this.value_minus.toFixed(3) + "\tValor +: " + this.value_plus.toFixed(3);
    }
}

// Table Terreno
class terreno_table {

    constructor(percent, value) {

        this.percent = percent;
        this.value = value;
    }

    toString() {
        return "Percent: " + this.percent.toFixed(2) + "%\tValor: " + this.value.toFixed(3);
    }
}

// Shot Expanded
class ShotEx {

    constructor(shot, old) {

        this.hwi = [];
        this.altura = [];
        this.altura2 = []; // !@ esse aqui também tenho que gerar
        this.terreno = []; // !@ Esse aqui, tenho que tirar do player eu gerar os valores pela força da calculadora, com umas constantes

        // Inicializa
        this.init_HWI(shot.block_type.find((el) => {
            return el !== undefined && el !== null && el.hasOwnProperty('__type') && el.__type !== undefined && el.__type !== null && !isNaN(el.__type) && el.__type === types.eBLOCK_TYPE.TAB_HWI.VALUE; 
        }));

        this.init_altura(shot.block_type.find((el) => {
            return el !== undefined && el !== null && el.hasOwnProperty('__type') && el.__type !== undefined && el.__type !== null && !isNaN(el.__type) && el.__type === types.eBLOCK_TYPE.TAB_ALTURA.VALUE; 
        }));

        this.init_terreno(shot.block_type.find((el) => {
            return el !== undefined && el !== null && el.hasOwnProperty('__type') && el.__type !== undefined && el.__type !== null && !isNaN(el.__type) && el.__type === types.eBLOCK_TYPE.TAB_TERRENO.VALUE; 
        }));

        if (old)
            this.init_altura2Old(shot.block_type.find((el) => {
                return el !== undefined && el !== null && el.hasOwnProperty('__type') && el.__type !== undefined && el.__type !== null && !isNaN(el.__type) && el.__type === types.eBLOCK_TYPE.TAB_HEIGHT.VALUE; 
            }));
        else
            this.init_altura2(shot.block_type.find((el) => {
                return el !== undefined && el !== null && el.hasOwnProperty('__type') && el.__type !== undefined && el.__type !== null && !isNaN(el.__type) && el.__type === types.eBLOCK_TYPE.TAB_HEIGHT.VALUE; 
            }));
    }

    init_HWI(calc) {

        if (!(calc instanceof Object))
            return;

        if (calc.map.length < 8)
            return;

        let arr_new_1 = [];
        let last = -1;

        for (let i = 0; i < calc.map[0].length; i++) {

            if ((last = arr_new_1.last()) !== -1) {

                let ob = {};

                let diff = Math.abs(Math.abs(last.percent) - Math.abs(calc.map[0/*Percent*/][i]));

                let calipers = diff * CALIPER_PER_PERCENT;

                for (let j = 0; j < calc.map.length; j++) {
                    if (last.hasOwnProperty(eHWI_NAME[j]))
                        ob[eHWI_NAME[j]] = (last[eHWI_NAME[j]] - calc.map[j][i]) / calipers;
                }

                for (let j = 1; j <= calipers; j++) {

                    let new_ = new Object();
                    new_.toString = hwi_table.prototype.toString;

                    for (let prop in last) {

                        if (last.hasOwnProperty(prop) && ob.hasOwnProperty(prop))
                            new_[prop] = last[prop] - (ob[prop] * j);
                    }

                    arr_new_1.push(new_);
                }

            }else
                arr_new_1.push(new (Function.prototype.bind.apply(hwi_table, [null, ...calc.map.map((el) => {
                    return el[i];
                })])));
        }

        // Coloca os valores gerados no hwi table
        this.hwi.push.apply(this.hwi, arr_new_1);
    }

    init_altura(alt) {

        if (!(alt instanceof Object))
            return;

        if (alt.map.length < 3)
            return;

        let arr_new_1 = [];
        let last = -1;

        for (let i = 0; i < alt.map[0].length; i++) {

            if ((last = arr_new_1.last()) !== -1) {

                let ob = {};

                for (let j = 0; j < alt.map.length; j++) {
                    if (last.hasOwnProperty(eALTURA_NAME[j]))
                        ob[eALTURA_NAME[j]] = (last[eALTURA_NAME[j]] - (j == 0 ? Math.abs(alt.map[j][i]) : alt.map[j][i])) / VALUE_PER_SCALE;
                }

                for (let j = 1; j <= VALUE_PER_SCALE; j++) {

                    let new_ = new Object();
                    new_.toString = altura_table.prototype.toString;

                    for (let prop in last) {

                        if (last.hasOwnProperty(prop) && ob.hasOwnProperty(prop))
                            new_[prop] = (prop == eALTURA_NAME[0] ? Math.abs(last[prop] - (ob[prop] * j)) : last[prop] - (ob[prop] * j));
                    }

                    arr_new_1.push(new_);
                }

            }else
                arr_new_1.push(new (Function.prototype.bind.apply(altura_table, [null, ...alt.map.map((el) => {
                    return el[i];
                })])));
        }

        // Coloca os valores gerados no hwi table
        this.altura.push.apply(this.altura, arr_new_1);
    }

    init_altura2(alt2) {

        if (!(alt2 instanceof Object))
            return;

        if (alt2.map.length < 2)
            return;

        let arr_new_1 = [];
        let last = -1;

        for (let i = 0; i < alt2.map[0].length; i++) {

            if ((last = arr_new_1.last()) !== -1) {

                let ob = {};

                for (let j = 0; j < alt2.map.length; j++) {
                    if (last.hasOwnProperty(eALTURA_NAME_2[j]))
                        ob[eALTURA_NAME_2[j]] = (last[eALTURA_NAME_2[j]] -  alt2.map[j][i]) / VALUE_PER_SCALE;
                }

                for (let j = 1; j <= VALUE_PER_SCALE; j++) {

                    let new_ = new Object();
                    new_.toString = altura_table.prototype.toString;

                    for (let prop in last) {

                        if (last.hasOwnProperty(prop) && ob.hasOwnProperty(prop))
                            new_[prop] =  last[prop] - (ob[prop] * j);
                        else if (last.hasOwnProperty('alt'))
                            new_['alt'] = last['alt'] + ((5 / VALUE_PER_SCALE) * j);
                    }

                    arr_new_1.push(new_);
                }

            }else
                arr_new_1.push(new (Function.prototype.bind.apply(altura_table, [null, ...alt2.map.map((el) => {
                    return el[i];
                })])));
        }

        // Coloca os valores gerados no hwi table
        this.altura2.push.apply(this.altura2, arr_new_1);
    }

    init_altura2Old(alt2) {

        if (!(alt2 instanceof Object))
            return;

        if (alt2.map.length < 2)
            return;

        this.init_altura2(alt2);
    }

    init_terreno(terr) {

        if (!(terr instanceof Object))
            return;

        if (terr.map.length < 2)
            return;

        let arr_new_1 = [];
        let last = -1;

        for (let i = 0; i < terr.map[0].length; i++) {

            if ((last = arr_new_1.last()) !== -1) {

                let ob = {};

                for (let j = 0; j < terr.map.length; j++) {
                    if (last.hasOwnProperty(eTERRENO_NAME[j]))
                        ob[eTERRENO_NAME[j]] = (last[eTERRENO_NAME[j]] - terr.map[j][i]) / VALUE_PER_SCALE;
                }

                for (let j = 1; j <= VALUE_PER_SCALE; j++) {

                    let new_ = new Object();
                    new_.toString = terreno_table.prototype.toString;

                    for (let prop in last) {

                        if (last.hasOwnProperty(prop) && ob.hasOwnProperty(prop))
                            new_[prop] = last[prop] - (ob[prop] * j);
                    }

                    arr_new_1.push(new_);
                }

            }else
                arr_new_1.push(new (Function.prototype.bind.apply(terreno_table, [null, ...terr.map.map((el) => {
                    return el[i];
                })])));
        }

        // Coloca os valores gerados no hwi table
        this.terreno.push.apply(this.terreno, arr_new_1);
    }
}

// Calculadora
class Calculadora extends CalculadoraBase.CalculadoraBaseInject {

    constructor(base) {

        // Calculadora Base Inject
        super(base);

        this.combobox_shot = [];

        this.hwis = new Map();
    }

    initComboBoxValues() {

        this.combobox_shot = [];

        let shot_tmp = undefined;
        let block_shot = undefined;

        this.shot.forEach((el, index) => {

            block_shot = el.block_type.find((elb) => {
                return elb.__type === types.eBLOCK_TYPE.SHOT.VALUE;
            })

            if (block_shot === undefined)
                return;

            if ((shot_tmp = this.combobox_shot.find(shot => {
                return shot.id == block_shot.shot;
            })) !== undefined)
                shot_tmp.club.push({
                    index: index,
                    power: this.findPower(el),
                    name: `${CalculadoraBase.Shot.getNameTypeByType(block_shot.club, CalculadoraBase.CLUB_TYPE, false)} (${block_shot.power.toFixed(0)}y) - ${CalculadoraBase.Shot.getNameTypeByType(block_shot.special, CalculadoraBase.SPECIAL_SHOT_TYPE, false)}` + (block_shot.spin != 0 ? " - " + block_shot.spin + " Spin" : "")
                });
            else
                this.combobox_shot.push({
                    id: block_shot.shot,
                    name: CalculadoraBase.Shot.getNameTypeByType(block_shot.shot, CalculadoraBase.SHOT_TYPE, false),
                    club: [{
                        index: index,
                        power: this.findPower(el),
                        name: `${CalculadoraBase.Shot.getNameTypeByType(block_shot.club, CalculadoraBase.CLUB_TYPE, false)} (${block_shot.power.toFixed(0)}y) - ${CalculadoraBase.Shot.getNameTypeByType(block_shot.special, CalculadoraBase.SPECIAL_SHOT_TYPE, false)}` + (block_shot.spin != 0 ? " - " + block_shot.spin + " Spin" : "")
                    }]
                });
        });

        // sort
        this.combobox_shot.sort((a, b) => {
            return a.id - b.id;
        });
    }

    initHWIs() {

        this.hwis = new Map();

        this.shot.forEach((el) => {

            this.hwis.set(el, new ShotEx(el, this.isOld()));
        });
    }

    onOpenedFile() {

        this.initHWIs();

        this.initComboBoxValues();
    }

    findPower(tacada) {

        if (tacada === undefined || tacada === null || !(tacada instanceof Object))
            return 230; // Base 230y

        let block_shot = tacada.block_type.find((el) => {
            return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
        });

        if (block_shot === undefined)
            return 230; // Base 230y

        if (this.isOld()) {

            let tab = this.hwis.get(tacada);

            if (tab === undefined || tab === null || !(tab instanceof Object) || !tab.hasOwnProperty('hwi'))
                return 230; // Base 230y

            let pwr = this.find(tab.hwi, 'percent', 100)

            if (pwr === undefined || pwr === null || !(pwr instanceof Object) || !pwr.hasOwnProperty('menor') || !pwr.hasOwnProperty('maior')
                || !(pwr.menor instanceof Object) || !(pwr.maior instanceof Object) || !pwr.menor.hasOwnProperty('i') || !pwr.menor.hasOwnProperty('value')
                || !pwr.maior.hasOwnProperty('i') || !pwr.maior.hasOwnProperty('value') || isNaN(pwr.menor.i) || isNaN(pwr.menor.value) 
                || isNaN(pwr.maior.i) || isNaN(pwr.maior.value) || pwr.menor.i === -1 || pwr.maior.i === -1)
                    return 230; // Base 230y

            if (!tab.hwi[pwr.menor.i].hasOwnProperty('caliper'))
                return 230; // Base 230y

            if (pwr.menor.value == 100) {

                block_shot.power = tab.hwi[pwr.menor.i]['caliper'];

                return block_shot.power;
            }

            if (pwr.maior.value == 100) {
                
                block_shot.power = tab.hwi[pwr.maior.i]['caliper'];

                return block_shot.power;
            }
        
        }else
            return (block_shot.power == 0) ? 230/*230y Base*/ : block_shot.power;

        return 230; // 230y Base
    }

    find(tab, prop, value) {

        let ret = { 
            'menor': { 'i': -1, 'value': -1 }, 
            'maior': { 'i': -1, 'value': -1 }
        };

        if (!(tab instanceof Array))
            return ret;

        tab.forEach((el, i) => {

            if (el.hasOwnProperty(prop)) {

                // Menor
                if (ret.menor.i == -1)
                    ret.menor = { 'i': i, 'value': el[prop] };
                else if (el[prop] <= value && (Math.abs((el[prop] / value) - 1) < Math.abs((ret.menor.value / value) - 1)))
                    ret.menor = { 'i': i, 'value': el[prop] };

                // Maior
                if (ret.maior.i == -1)
                    ret.maior = { 'i': i, 'value': el[prop] };
                else if (el[prop] >= value && (Math.abs((el[prop] / value) - 1) < Math.abs((ret.maior.value / value) - 1)))
                    ret.maior = { 'i': i, 'value': el[prop] };
            }
        })

        if (ret.menor.i != -1 && ret.maior.i == -1) {
            
            ret.maior.i = ret.menor.i;
            ret.maior.value = ret.menor.value;

        }else if (ret.maior.i != -1 && ret.menor.i == -1) {
            
            ret.menor.i = ret.maior.i;
            ret.menor.value = ret.maior.value;
        }

        return ret;
    }

    approach(value, menor_maior) {

        if (menor_maior === null || !(menor_maior instanceof Object)
                || !menor_maior.hasOwnProperty('menor') || !menor_maior.hasOwnProperty('maior'))
            return null;

        if (isNaN(value))
            return null;

        return (Math.abs((menor_maior.menor.value / value) - 1) <= Math.abs((menor_maior.maior.value / value) - 1) ? menor_maior.menor : menor_maior.maior);
    }

    getPropByApproach(tab, prop, value, approach) {

        if (!(tab instanceof Array) || tab.length <= 0 || prop === null || isNaN(value)
                || !(approach instanceof Object) || !approach.hasOwnProperty('menor') || !approach.hasOwnProperty('maior'))
            return 0;

        let app = this.approach(value, approach);

        if (app === null || !app.hasOwnProperty('i') || !app.hasOwnProperty('value'))
            return 0;

        if (!tab[app.i].hasOwnProperty(prop))
            return 0;

        return tab[app.i][prop];
    }

    getPropByApproach2(tab, prop, value, approach) {

        if (!(tab instanceof Array) || tab.length <= 0 || prop === null || isNaN(value)
                || !(approach instanceof Object))
            return 0;

        if (!approach.hasOwnProperty('spin') && (!approach.hasOwnProperty('menor') || !approach.hasOwnProperty('maior')))
            return 0;

        let app = null;

        if (!approach.hasOwnProperty('spin')) {

            app = this.approach(value, approach);

            if (app === null || !app.hasOwnProperty('i') || !app.hasOwnProperty('value'))
                return 0;

        }else
            app = approach;

        if (!tab[app.i].hasOwnProperty(prop))
            return 0;

        return tab[app.i][prop];
    }

    findAndGetProp(tab, prop_find, prop_ret, value) {

        let menor_maior = this.find(tab, prop_find, value);
        
        return {
            pwr: menor_maior, 
            value: this.getPropByApproach(tab, prop_ret, value, menor_maior)
        };
    }

    calculeSpin(percent, value, approach, spin = 1/*Aqui e o valor do Spin para a força da tacada*/) {

        if (isNaN(percent) || isNaN(value) || isNaN(spin) || !(approach instanceof Object) 
                || !approach.hasOwnProperty('menor') || !approach.hasOwnProperty('maior'))
            return null;

        let app = this.approach(value, approach);

        if (app === null || !app.hasOwnProperty('i') || !app.hasOwnProperty('value'))
            return null;

        let diff = Math.abs(app.value - value);

        // !@ Aqui tem que calcular o valor pela força com um valor constante
        let spin_variant = spin * percent;

        if (app.value > value) {

            app.spin = Math.round(diff / spin_variant) * -1;
        
        }else if (Math.round(diff / spin_variant) >= 1) {
            
            app = approach.maior;

            diff = Math.abs(app.value - value);

            app.spin = Math.round(diff / spin_variant) * -1;
                
        }else
            app.spin = 0;

        return app;
    }

    getH(hwis, input) {

        if (!(hwis instanceof Object) || !(input instanceof Object))
            return 0;

        const HP = 0.825 + Math.abs(input.height() * 0.01);

        // Terreno
        let app_terr = this.find(hwis.terreno, 'percent', input.terreno);
        let terreno = this.getPropByApproach(hwis.terreno, 'value', input.terreno, app_terr);

        // Altura 1
        let app_alt1 = this.find(hwis.altura, 'alt', Math.abs(input.altura));
        let calt = this.getPropByApproach(hwis.altura, (input.altura < 0 ? 'value_minus' : 'value_plus'), Math.abs(input.altura), app_alt1);

        // Calcula valor da altura
        let dist = input.distancia + (input.cos() * input.wind()) + terreno;

        let pwr = this.find(hwis.hwi, 'power', dist);
        let percent = this.getPropByApproach(hwis.hwi, 'percent', dist, pwr) / 100;
        let h = this.getPropByApproach(hwis.hwi, (input.altura < 0 ? 'h_minus' : 'h_plus'), dist, pwr);

        dist += (input.height() * calt);

        pwr = this.find(hwis.hwi, 'power', dist);
        let percent2 = this.getPropByApproach(hwis.hwi, 'percent', dist, pwr) / 100;
        let h2 = this.getPropByApproach(hwis.hwi, (input.altura < 0 ? 'h_minus' : 'h_plus'), dist, pwr);

        let alt = (input.height() * calt * (Math.sqrt(h2+h)/2) * (Math.sqrt(percent+percent2)/HP));

        dist += alt;

        if (DEBUG_FLAG)
            console.log("Alt: " + alt.toFixed(2) + "\tH1: " + h.toFixed(3) + "\tH2: " + h2.toFixed(3));

        pwr = this.find(hwis.hwi, 'power', dist);
        percent2 = this.getPropByApproach(hwis.hwi, 'percent', dist, pwr) / 100;
        h2 = this.getPropByApproach(hwis.hwi, (input.altura < 0 ? 'h_minus' : 'h_plus'), dist, pwr);

        let h3 = (Math.sqrt(h2+h)/2) * (Math.sqrt(percent+percent2)/HP);

        alt = h3 * input.height() * calt;

        if (DEBUG_FLAG)
            console.log("Alt: " + alt.toFixed(2) + "\tH2: " + h2.toFixed(3) + "\tH3: " + h3.toFixed(3) + "\tPercent: " + percent2.toFixed(2));

        return h3;
    }

    getSlopeVariant(tacada) {

        let slope_variant = 4;

        if (!(tacada instanceof Object))
            return slope_variant;

        let block_shot = tacada.block_type.find((el) => {
            return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
        });

        if (block_shot === undefined)
            return slope_variant;

        switch (block_shot.shot) {
            case CalculadoraBase.SHOT_TYPE.TOMAHAWK.TYPE:
            {
                slope_variant = 3.7;

                break;
            }
            case CalculadoraBase.SHOT_TYPE.DUNK.TYPE:
            {
                if (block_shot.spin == 30)
                    slope_variant = 3.92;

                break;
            }
            case CalculadoraBase.SHOT_TYPE.COBRA.TYPE:
            case CalculadoraBase.SHOT_TYPE.SPIKE.TYPE:
            {
                slope_variant = 3.4;

                break;
            }
        }

        return slope_variant;
    }

    getSpinVariant(tacada) {

        let spin_variant = 0;

        if (!(tacada instanceof Object))
            return spin_variant;

        let block_shot = tacada.block_type.find((el) => {
            return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
        });

        if (block_shot === undefined)
            return spin_variant;

        // !@ Esses aqui tem que calcular pela força da tacada
        switch (block_shot.shot) {
            case CalculadoraBase.SHOT_TYPE.COBRA.TYPE:
            {
                spin_variant = 0.9;

                break;
            }
            case CalculadoraBase.SHOT_TYPE.SO_PANGYA.TYPE:
            {
                spin_variant = 0.6;

                break;
            }
            case CalculadoraBase.SHOT_TYPE.SPIKE.TYPE:
            {
                if (block_shot.spin == 30)
                    spin_variant = 1.07;

                break;
            }
            case CalculadoraBase.SHOT_TYPE.TOMAHAWK.TYPE:
            {       
                if (block_shot.spin == 30)
                    spin_variant = 0.55;

                break;
            }
            case CalculadoraBase.SHOT_TYPE.DUNK.TYPE:
            {
                spin_variant = 0.5;

                break;
            }
        }

        return spin_variant;
    }

    getTerrenoVariant(tacada, terreno) {

        if (terreno === undefined || terreno === null || isNaN(terreno) || terreno === 0)
            return 0;

        if (!(tacada instanceof types.Tabela))
            return 0;

        let block_shot = tacada.block_type.find((el) => {
            return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
        });

        if (block_shot === undefined)
            return 0;

        let percent = (100 - terreno);
        let calc_power = parseInt(this.head.power);

        let pwr = ((calc_power - 200) / 2);

        // Aqui tem que verifica, 1w, 2w, 2i, 3i, PW e SW, cada um tem seu valor, 1w é 0.57(+/-)
        const club_base = 0.57 * (block_shot.power / (calc_power == 0 ? 1 : calc_power));

        pwr = club_base + (pwr * 0.0014) + (pwr * 0.0005);
        pwr = pwr + (percent * 0.006);
        pwr = pwr * percent;

        if (DEBUG_FLAG)
            console.log(pwr);

        return pwr;
    }

    formHWICalc(input_calc) {

        if (!(input_calc instanceof Object))
            return null;

        // !@ Fazendo aqui para não ter que trocar lá em baixo, é só um teste
        input_calc.input.height = function() {
            return this.altura * METER_TO_YARDS;
        }

        const alt_prop = (input_calc.input.altura < 0 ? 'value_minus' : 'value_plus');
        const h_prop = (input_calc.input.altura < 0 ? 'h_minus' : 'h_plus');
        const wind_prop = (input_calc.input.cos() < 0 ? 'wind_front' : 'wind_back');

        // Slope variant
        let slope_variant = this.getSlopeVariant(input_calc.tacada);

        // Terreno
        let terreno = this.getTerrenoVariant(input_calc.tacada, input_calc.input.terreno);

        // Percent
        let percent = this.findAndGetProp(input_calc.tab.hwi, 'power', 'percent', input_calc.input.distancia).value / 100;

        // 0.003 const, 0.000012 relação altura, (!@ talvez tenha que colocar o abs e '-'(menos))
        let const_alt2 = (0.0037 + (input_calc.input.height() * 0.000012));

        // Altura 1
        let alt = this.findAndGetProp(input_calc.tab.altura, 'alt', alt_prop, Math.abs(input_calc.input.altura)).value;

        // Power Slope
        let pwrSlope = Math.abs(input_calc.input.slope_bola) * 0.0075;
        
        // Calcula tacada New
        let avg_func = (tab, input, prop, func) => {

            let dist = input.distancia;

            let first = this.findAndGetProp(tab, 'power', prop, dist).value;

            dist += func(first);

            let last = this.findAndGetProp(tab, 'power', prop, dist).value;

            return ((first + last) / 2);

        };

        // Influência do vento
        let iv = avg_func(input_calc.tab.hwi, input_calc.input, wind_prop, (first) => {
            return (input_calc.input.wind() * input_calc.input.cos() * first);
        });

        // H-/+
        let h = avg_func(input_calc.tab.hwi, input_calc.input, h_prop, (first) => {
            return 0;///*(*/input_calc.input.height()/* * value * alt)*/;
        });

        let h_variant = this.findAndGetProp(this.tmp_tab, 'percent', 'value', percent * 100).value;

        let alt_variant = (Math.abs(input_calc.input.altura) - 5);

        alt_variant = (alt_variant <= 0) ? 1 : (1 - (alt_variant / 55));

        if (DEBUG_FLAG)
            console.log(`H: ${h}, H_Variant: ${h_variant}, Alt_Variant: ${alt_variant}`);

        h += (h_variant * alt_variant);

        // Wind Influ alt
        let ivh = (input_calc.input.wind() * input_calc.input.cos() * iv);

        // !@ Teste
        h = h - (ivh * (input_calc.input.cos() < 0 ? 0.011 : 0.013));

        if (DEBUG_FLAG)
            console.log(`ALTURA: ${input_calc.input.height()} (${input_calc.input.altura})    H: ${h}  ALT: ${alt}    IVH: ${ivh}`);

        // !@ Teste h- generate
        if (DEBUG_FLAG && input_calc.input.hasOwnProperty('h_minus_teste')) {

            console.log(`H-{ ${input_calc.input.h_minus_teste} } generate.`);

            h = input_calc.input.h_minus_teste;
            alt = 1;
        }

        let influs = (input_calc.input.height() * h * alt) + (input_calc.input.wind() * input_calc.input.cos() * iv) + (terreno * percent) + pwrSlope;

        let hwi = avg_func(input_calc.tab.hwi, input_calc.input, 'hwi', () => {
            return influs/* * 0.25*/;
        });

        hwi += input_calc.input.height() * -(const_alt2);

        let dist = input_calc.input.distancia + influs;

        percent = this.findAndGetProp(input_calc.tab.hwi, 'power', 'percent', dist).value / 100;

        let slope = (hwi / slope_variant) * percent * input_calc.input.slope_bola;
        let desvio = hwi * input_calc.input.sin() * input_calc.input.wind() + slope;

        let hdv = Math.abs(input_calc.input.sin() * desvio * 0.1);

        hwi += (Math.abs(input_calc.input.sin() * desvio * 0.11) * 0.00321);

        // Recalcula desvio com o hdv
        desvio = hwi * input_calc.input.sin() * input_calc.input.wind() + slope;

        // h = avg_func(input_calc.tab.hwi, input_calc.input, 'percent', (first) => {
        //     return (input_calc.input.height() * percent * (h / first) * alt);
        // }) / 100 * h;

        //console.log(h);

        influs = (input_calc.input.height() * h * alt) + (input_calc.input.wind() * input_calc.input.cos() * iv) + (terreno * percent) + pwrSlope;

        dist = input_calc.input.distancia + influs + hdv;

        let { pwr, value } = this.findAndGetProp(input_calc.tab.hwi, 'power', 'percent', dist);

        // return values
        return {
            dist: dist,
            desvio: desvio,
            percent: value / 100,
            pwr: pwr,
            hwi: hwi,
            slope: slope
        }
    }

    newCalc(input_calc) {

        if (!(input_calc instanceof Object))
            return null;

        // Slope variant
        let slope_variant = this.getSlopeVariant(input_calc.tacada);

        // Terreno
        let app_terr = this.find(input_calc.tab.terreno, 'percent', input_calc.input.terreno);
        let terreno = this.getPropByApproach(input_calc.tab.terreno, 'value', input_calc.input.terreno, app_terr);

        // Altura 2
        let app_alt2 = this.find(input_calc.tab.altura2, 'alt', Math.abs(input_calc.input.altura));
        let alt2 = this.getPropByApproach(input_calc.tab.altura2, (input_calc.input.altura < 0 ? 'value_minus' : 'value_plus'), Math.abs(input_calc.input.altura), app_alt2);

        // Power Slope
        let pwrSlope = Math.abs(input_calc.input.slope_bola) * 0.0075;
        
        // Calcula tacada New
        let dist = input_calc.input.distancia + input_calc.input.height() + (input_calc.input.cos() * input_calc.input.wind()) + terreno + pwrSlope;

        let pwr = this.find(input_calc.tab.hwi, 'power', dist);
        let percent = this.getPropByApproach(input_calc.tab.hwi, 'percent', dist, pwr) / 100;
        let hwi = this.getPropByApproach(input_calc.tab.hwi, 'hwi', dist, pwr) + (input_calc.input.height() * -alt2);
        let vento_influ = this.getPropByApproach(input_calc.tab.hwi, (input_calc.input.cos() < 0 ? 'wind_front' : 'wind_back'), dist, pwr);

        let slope = (hwi / slope_variant) * percent * input_calc.input.slope_bola;
        let desvio = hwi * input_calc.input.sin() * input_calc.input.wind() + slope;
        let hdv = Math.abs(input_calc.input.sin() * desvio * 0.11);

        hwi += (hdv * 0.00321);

        // Recalcula desvio com o hdv
        desvio = hwi * input_calc.input.sin() * input_calc.input.wind() + slope;

        let h = this.getH(input_calc.tab, input_calc.input);

        for(let i = 0; i < 3; i++) {

            dist = input_calc.input.distancia + (input_calc.input.height() * h) + (input_calc.input.cos() * input_calc.input.wind() * vento_influ) + (terreno * percent) + hdv + pwrSlope;

            pwr = this.find(input_calc.tab.hwi, 'power', dist);
            percent = this.getPropByApproach(input_calc.tab.hwi, 'percent', dist, pwr) / 100;
            hwi = this.getPropByApproach(input_calc.tab.hwi, 'hwi', dist, pwr) + (input_calc.input.height() * -alt2);
            vento_influ = this.getPropByApproach(input_calc.tab.hwi, (input_calc.input.cos() < 0 ? 'wind_front' : 'wind_back'), dist, pwr);
            
            slope = (hwi / slope_variant) * percent * input_calc.input.slope_bola;
            desvio = hwi * input_calc.input.sin() * input_calc.input.wind() + slope;
            hdv = Math.abs(input_calc.input.sin() * desvio * 0.11);

            hwi += (hdv * 0.00321);

            // Recalcula desvio com o hdv
            desvio = hwi * input_calc.input.sin() * input_calc.input.wind() + slope;
        }

        // return values
        return {
            dist: dist,
            desvio: desvio,
            percent: percent,
            pwr: pwr,
            hwi: hwi,
            slope: slope
        }
    }

    oldCalc(input_calc) {

        if (!(input_calc instanceof Object))
            return null;

        // Terreno
        let app_terr = this.find(input_calc.tab.terreno, 'percent', input_calc.input.terreno);
        let terreno = this.getPropByApproach(input_calc.tab.terreno, 'value', input_calc.input.terreno, app_terr);

        // Altura 2
        let app_alt2 = this.find(input_calc.tab.altura2, 'alt', Math.abs(input_calc.input.altura));
        let alt2 = this.getPropByApproach(input_calc.tab.altura2, (input_calc.input.altura < 0 ? 'value_minus' : 'value_plus'), Math.abs(input_calc.input.altura), app_alt2);

        // Old
        let dist = input_calc.input.distancia;

        let pwr = this.find(input_calc.tab.hwi, 'power', dist);
        let te = this.getPropByApproach(input_calc.tab.hwi, 'terreno', dist, pwr);
        let percent = this.getPropByApproach(input_calc.tab.hwi, 'percent', dist, pwr) / 100;
        let vento_influ = this.getPropByApproach(input_calc.tab.hwi, (input_calc.input.cos() < 0 ? 'wind_front' : 'wind_back'), dist, pwr);
        let h = this.getPropByApproach(input_calc.tab.hwi, (input_calc.input.altura < 0 ? 'h_minus' : 'h_plus'), dist, pwr);
        let slope_influ = this.getPropByApproach(input_calc.tab.hwi, 'slope', dist, pwr);

        let terrAlt = (input_calc.input.altura * -0.005) + 1;

        terreno *= te * terrAlt;

        let app_alt = this.find(input_calc.tab.altura, 'alt', Math.abs(input_calc.input.altura));
        let alt = this.getPropByApproach(input_calc.tab.altura, (input_calc.input.altura < 0 ? "value_minus" : "value_plus"), Math.abs(input_calc.input.altura), app_alt);

        alt = (alt * h) * input_calc.input.altura;

        alt2 = alt2 / (Math.pow(percent, 1.2));

        let relacaoAlt = input_calc.input.altura * -alt2;
        
        dist = input_calc.input.distancia + (input_calc.input.wind() * vento_influ * input_calc.input.cos()) + terreno

        pwr = this.find(input_calc.tab.hwi, 'power', dist);
        let hwi = this.getPropByApproach(input_calc.tab.hwi, 'hwi', dist, pwr);
        vento_influ = this.getPropByApproach(input_calc.tab.hwi, (input_calc.input.cos() < 0 ? 'wind_front' : 'wind_back'), dist, pwr);

        vento_influ = vento_influ * Math.pow(0.99, input_calc.input.altura);

        hwi += relacaoAlt;

        let hdv = Math.abs(input_calc.input.sin() * input_calc.input.wind() * 0.055);

        dist = input_calc.input.distancia + (input_calc.input.wind() * vento_influ * input_calc.input.cos()) + terreno + alt + hdv;

        pwr = this.find(input_calc.tab.hwi, 'power', dist);
        percent = this.getPropByApproach(input_calc.tab.hwi, 'percent', dist, pwr) / 100;

        // Aqui é a nova influ slope, por que a slope tem que ser pela porcentagem do final da tacada, não o pin inicial
        // !@ Só tem esse modificação, se tirar ele fica igual a antiga formula
        slope_influ = this.getPropByApproach(input_calc.tab.hwi, 'slope', dist, pwr);

        let slope = (hwi / 4) * input_calc.input.slope_bola * slope_influ;

        let desvio = (hwi * input_calc.input.wind() * input_calc.input.sin()) + slope;

        // return values
        return {
            dist: dist,
            desvio: desvio,
            percent: percent,
            pwr: pwr,
            hwi: hwi,
            slope: slope
        }
    }

    calcula(input, old_calc = false) {

        // Início checkers
        if (!(input instanceof Object))
            return null;

        if (input.tacada < 0 || input.tacada > this.shot.length)
            return null;

        let tacada = this.hwis.get(this.shot[input.tacada]);

        if (tacada === undefined)
            return null;

        // Fim checkers

        // !@ Teste H- generate
        if (DEBUG_FLAG) {
            
            if (input.slope_green != 1000)
                input.h_minus_teste = input.slope_green;

            input.slope_green = 0;
        }

        // Calc
        let result = null;
        
        if (old_calc) {

            // Old
            result = this.oldCalc({
                input: input,
                tab: tacada,
                tacada: this.shot[input.tacada]
            });
        
        }else {

            // New
            result = this.formHWICalc({//this.newCalc({
                input: input,
                tab: tacada,
                tacada: this.shot[input.tacada]
            });
        }

        if (result === null)
            return null;

        let block_shot = this.shot[input.tacada].block_type.find((el) => {
            return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
        });

        if (block_shot === undefined)
            return null;

        // Add o valor de slope do green só para tacadas Back Spin
        if (block_shot.shot == CalculadoraBase.SHOT_TYPE.BACK_SPIN.TYPE && input.slope_green != 0)
            result.desvio += input.slope_green;

        // Calcula spin e caliper, se a tacada tiver
        let spin_variant = this.getSpinVariant(this.shot[input.tacada]);

        if (spin_variant != 0)
            result.pwr = this.calculeSpin(result.percent, result.dist, result.pwr, spin_variant);

        // Caliper
        let caliper = this.getPropByApproach2(tacada.hwi, 'caliper', result.dist, result.pwr);

        let spin = (result.pwr.hasOwnProperty('spin') ? result.pwr.spin : 0);

        // Suporte para versões antigas
        if (block_shot.shot == CalculadoraBase.SHOT_TYPE.DUNK.TYPE && block_shot.spin == 0)
            spin += 9;
        else
            spin += block_shot.spin;

        // Teste
        if (DEBUG_FLAG) {

            console.log(result.dist.toFixed(3));
            console.log((result.percent * 100).toFixed(1));
            console.log(caliper.toFixed(1));
            console.log(spin);
            console.log(result.hwi.toFixed(3));
            console.log(result.desvio.toFixed(3));
            console.log(result.slope.toFixed(3));
            console.log(result.pwr);
        }

        // Result
        return {
            desvio: (isNaN(result.desvio) ? 0 : result.desvio),
            percent: (isNaN(result.percent) ? 0 : result.percent),
            hwi: (isNaN(result.hwi) ? 0 : result.hwi),
            caliper: (isNaN(caliper) ? 0 : caliper),
            spin: (isNaN(spin) ? 0 : spin)
        };
    }
}

module.exports = {
    Calculadora: Calculadora
}