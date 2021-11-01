// Arquivo translateCalculadora.js
// Criado em 24/07/20 as 01:06 por Acrisio
// Definição da classe que traduz os dados recebido do cliente, para o module Calculadora

const Calculadora = require('./api_calculadora.js');
const ccm = require('../common/CacheManager.js');

const APIError = require('../type/APIErrorException.js');

// My Error Exception
const MyError = APIError.APIErrorException;

// Core Input
class CoreInputs {
    
    constructor(tacada, distancia, altura, vento, angulo, slope_bola, slope_green, terreno, power) {

        this.state = false;

        try {

            // Private:
            this.tacada = CoreInputs.checkParameter(tacada);
            this.distancia = CoreInputs.checkParameter(distancia);
            this.altura = CoreInputs.checkParameter(altura);
            this.vento = CoreInputs.checkParameter(vento);
            this.angulo = CoreInputs.checkParameter(angulo);
            this.slope_bola = CoreInputs.checkParameter(slope_bola);
            this.slope_green = CoreInputs.checkParameter(slope_green);
            this.terreno = CoreInputs.checkParameter(terreno);
            this.power = CoreInputs.checkParameter(power);

            // Public:
            this.state = true;

        }catch (e) {

            throw new MyError("Error in constructor CoreInputs, " + e.message);
        }
    }

    // Public:
    shot() {

        if (this.state === false || !CoreInputs.isValid(this.tacada))
            throw new MyError("instance of CoreInputs is invalid");

        return this.tacada;
    }

    sin() {

        if (this.state === false || !CoreInputs.isValid(this.angulo))
            throw new MyError("instance of CoreInputs is invalid");

        return Math.sin(Math.PI / 180 * this.angulo);
    }

    cos() {

        if (this.state === false || !CoreInputs.isValid(this.angulo))
            throw new MyError("instance of CoreInputs is invalid");

        return Math.cos(Math.PI / 180 * this.angulo) * -1;
    }

    distance() {

        if (this.state === false || !CoreInputs.isValid(this.distancia))
            throw new MyError("instance of CoreInputs is invalid");

        return this.distancia;
    }

    height() {

        if (this.state === false || !CoreInputs.isValid(this.altura))
            throw new MyError("instance of CoreInputs is invalid");

        return this.altura * 1.094 /*METER TO YARD*/;
    }

    wind() {

        if (this.state === false || !CoreInputs.isValid(this.vento))
            throw new MyError("instance of CoreInputs is invalid");

        return this.vento;
    }

    degree() {

        if (this.state === false || !CoreInputs.isValid(this.angulo))
            throw new MyError("instance of CoreInputs is invalid");

        return this.angulo;
    }

    slopeBall() {

        if (this.state === false || !CoreInputs.isValid(this.slope_bola))
            throw new MyError("instance of CoreInputs is invalid");

        return this.slope_bola;
    }

    slopeGreen() {

        if (this.state === false || !CoreInputs.isValid(this.slope_green))
            throw new MyError("instance of CoreInputs is invalid");

        return this.slope_green;
    }

    ground() {

        if (this.state === false || !CoreInputs.isValid(this.terreno))
            throw new MyError("instance of CoreInputs is invalid");

        return this.terreno;
    }

    pwr() {

        if (this.state === false || !CoreInputs.isValid(this.power))
            throw new MyError("instance of CoreInputs is invalid");

        return (this.power == 0) ? 1 : this.power; // Power usa para divisão então não pode ser 0
    }

    // Public Static:
    static isValid(value) {
        return (value === undefined || value === null || isNaN(value)) ? false : true;
    }

    static checkParameter(param) {

        if (param === undefined || param === null || param instanceof Object || param instanceof Array || param instanceof Function || isNaN(param))
            throw new MyError("Invalid parameter");

        return param;
    }
}

// Input Object
class inputObject {

    constructor(type) {

        if (type === undefined || type === null || type instanceof Object || type instanceof Function || type instanceof ArrayBuffer || type === '')
            throw new MyError("Invalid Object type");

        this.type = type;

        this.inputs_property = [
            'tacada',
            'distancia',
            'altura',
            'vento',
            'angulo',
            'slope_bola',
            'slope_green',
            'terreno',
            'power'
        ];
    }

    makeInputsObject(inputs) {

        inputs = this.checkObjectInputs(inputs);

        let arr = [];

        this.inputs_property.forEach((prop) => {
            arr.push(inputs[prop]);
        });

        if (arr.length <= 0)
            throw new MyError("Unknown error in make inputs Object");

        // apply, first arg
        arr.unshift(null);

        return new (Function.prototype.bind.apply(CoreInputs, arr));
    }

    checkObjectInputs(inputs) {

        if (!(inputs instanceof Object))
            throw new MyError("Invalid Inputs Object");

        this.inputs_property.forEach((prop) => {
            if (!inputs.hasOwnProperty(prop))
                throw new MyError("Invalid Inputs Object not have property{" + prop + "}");
        });     

        return inputs;
    }

    checkUIDParameter() {
        return (!this.hasOwnProperty('uid') || this.uid instanceof Object || this.uid instanceof Function || this.uid instanceof ArrayBuffer || isNaN(this.uid));
    }

    checkManagerParameter(cache_mngr) {
        return (cache_mngr === undefined || cache_mngr === null || !(cache_mngr instanceof ccm.CacheManager));
    }

    list(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[list]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[list]");

        try {

            cache_mngr.listCalculadoras(this.uid, 'view', (err, list) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                callback(null, JSON.stringify(list));
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to list calculadoras.");
        }
    }

    open(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[open]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[open]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[open]");

        try {
            
            cache_mngr.getCalculadora(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof Calculadora.Calculadora)) {

                    callback({
                        message: "Fail to open Calculadora[" + this.name + "]",
                        code: 16000
                    }, undefined);
                    
                    return;
                }

                if (!calc.state) {

                    callback({
                        message: "Error to open calculadora[" + this.name + "]",
                        code: 16001
                    }, undefined);

                    return;
                }

                // Update Combobox_shot and HWIS
                calc.onOpenedFile();
        
                callback(null, JSON.stringify(calc.combobox_shot));
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() == 'ENOENT' ? 1 : 0));
        }
    }

    calcule(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[calcule]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[calcule]");

        if (!this.hasOwnProperty('input') || !(this.input instanceof Object))
            throw new MyError("Invalid parameter input");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid parameter name");

        try {
            
            cache_mngr.getCalculadora(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof Calculadora.Calculadora)) {
                    
                    callback({
                        message: "Fail to get Calculadora[" + this.name + "]",
                        code: 16000
                    }, undefined);

                    return;
                }

                // Opcional
                let old_flag = (this.hasOwnProperty('old_flag') && typeof this.old_flag === 'boolean' ? this.old_flag : false);

                let input = this.makeInputsObject(this.input);

                try {

                    callback(null, JSON.stringify(calc.calcula(input, old_flag)));

                }catch (err) {

                    console.log("Error: " + err.message);

                    callback({
                        message: "Error to calcule",
                        code: 17000
                    }, undefined);
                }

            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to get calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() == 'ENOENT' ? 1 : 0));
        }
    }

    notify(e) {

        // Carregou o arquivo com sucesso
    }
}

// TranslateAPICalculadora
class TranslateAPICalculadora {

    // Public:
    constructor(cache_mngr) {

        this.cache_mngr = cache_mngr;

        this.type = [
            'list',
            'open',
            'calcule'
        ];
    }

    /**
     * 
     * @param data {JSON.strinfy({type: open[, values]})} 
     * 
     * @returns Object
     */
    requestTranslate(data, callback) {

        try {

            if (data === undefined || data === null || data instanceof Object || data instanceof Function || data instanceof ArrayBuffer || data === '')
                throw new MyError("Invalid data");

            let obj = JSON.parse(data);

            this.translate(obj, callback);
        
        }catch (err) {

            // Log
            console.log(`[TranslateAPICalculadora::requestTranslate][Error] ${err.message}`);

            callback({
                message: err.message, 
                code: err.code
            }, undefined);
        }
    }

    // Private:
    translate(obj, callback) {

        if (obj === undefined || obj === null || !(obj instanceof Object) || !obj.hasOwnProperty('type') || obj.type === '')
            throw new MyError("Invalid json data");

        for (let i = 0; i < this.type.length; i ++) {
            
            if (this.type[i].toLowerCase() == obj.type) {

                let call = new inputObject(obj.type);

                if (!(obj.type in call))
                    throw new MyError("Error type[" + obj.type + "] unknown");

                for (let _prop in obj)
                    if (!call.hasOwnProperty(_prop))
                        call[_prop] = obj[_prop];

                // Call especifico translate
                call[this.type[i]](this.cache_mngr, callback);

                // Exit Function
                return;
            }
        }

        // Reply
        callback({
            message: `Unknown type[${obj.type}]`,
            code: 15000
        }, undefined);
    }
}

module.exports = {
    TranslateAPICalculadora: TranslateAPICalculadora
}