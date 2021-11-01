// Arquivo api_macro.js
// Criado em 16/07/2020 as 12:03 por Acrisio
// Definição do Core do Macro Math JS e alguns exemplos

const math = require('./math.js');
const MacroBaseInject = require('../common/MacroBaseInject')

const GlobalMathParser = math.parser;
const GlobalMathImport = math.import;

// block function vunerables here
GlobalMathImport({
    'import':     function () { throw new Error('Function import is disabled') },
    'createUnit': function () { throw new Error('Function createUnit is disabled') },
    'evaluate':   function () { throw new Error('Function evaluate is disabled') },
    'parse':      function () { throw new Error('Function parse is disabled') },
    'simplify':   function () { throw new Error('Function simplify is disabled') },
    'derivative': function () { throw new Error('Function derivative is disabled') },
    'parser':     function()  { throw new Error('Function parser is disabled') },
    'compile':    function()  { throw new Error('Function compile is disabled') },
    'Fixed': function(any, precision) { return any.toFixed(precision); },
    'return': function(any) { return (any === undefined || any === null ? 'undefined' : any); }
}, {override: true});

// Api Core Macro Math JS
class CoreInputs {
    
    constructor(distancia, altura, vento, angulo, slope_bola, slope_green, terreno, power) {

        this.state = false;

        try {

            // Private:
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

            throw new Error("Error in constructor CoreInputs, " + e.message);
        }
    }

    // Public:
    sin() {

        if (this.state === false || !CoreInputs.isValid(this.angulo))
            throw new Error("instance of CoreInputs is invalid");

        return Math.sin(Math.PI / 180 * this.angulo);
    }

    cos() {

        if (this.state === false || !CoreInputs.isValid(this.angulo))
            throw new Error("instance of CoreInputs is invalid");

        return Math.cos(Math.PI / 180 * this.angulo) * -1;
    }

    distance() {

        if (this.state === false || !CoreInputs.isValid(this.distancia))
            throw new Error("instance of CoreInputs is invalid");

        return this.distancia;
    }

    height() {

        if (this.state === false || !CoreInputs.isValid(this.altura))
            throw new Error("instance of CoreInputs is invalid");

        return this.altura * 1.094 /*METER TO YARD*/;
    }

    wind() {

        if (this.state === false || !CoreInputs.isValid(this.vento))
            throw new Error("instance of CoreInputs is invalid");

        return this.vento;
    }

    degree() {

        if (this.state === false || !CoreInputs.isValid(this.angulo))
            throw new Error("instance of CoreInputs is invalid");

        return this.angulo;
    }

    slopeBall() {

        if (this.state === false || !CoreInputs.isValid(this.slope_bola))
            throw new Error("instance of CoreInputs is invalid");

        return this.slope_bola;
    }

    slopeGreen() {

        if (this.state === false || !CoreInputs.isValid(this.slope_green))
            throw new Error("instance of CoreInputs is invalid");

        return this.slope_green;
    }

    ground() {

        if (this.state === false || !CoreInputs.isValid(this.terreno))
            throw new Error("instance of CoreInputs is invalid");

        return this.terreno;
    }

    pwr() {

        if (this.state === false || !CoreInputs.isValid(this.power))
            throw new Error("instance of CoreInputs is invalid");

        return (this.power == 0) ? 1 : this.power; // Power usa para divisão então não pode ser 0
    }

    // Public Static:
    static isValid(value) {
        return (value === undefined || value === null || isNaN(value)) ? false : true;
    }

    static checkParameter(param) {

        if (param === undefined || param === null || param instanceof Object || param instanceof Array || param instanceof Function || isNaN(param))
            throw new Error("Invalid parameter");

        return param;
    }
}

class CoreResults {

    constructor(desvio, _percent, _caliper, _hwi, _spin) {

        this.state = false;

        try {

            // Private:
            this.desvio = CoreResults.checkParameter(desvio);
            this._percent = CoreResults.checkParameter(_percent);
            this._caliper = CoreResults.checkParameter(_caliper);
            this._hwi = CoreResults.checkParameter(_hwi);
            this._spin = CoreResults.checkParameter(_spin);

            // Public:
            this.state = true;

        }catch (e) {

            throw new Error("Error in Constructor CoreResults, " + e.message);
        }
    }

    // Public:
    deviation() {

        if (this.state === false || !CoreResults.isValid(this.desvio))
            throw new Error("instance of CoreResults is invalid");

        return this.desvio;
    }

    percent() {

        if (this.state === false || !CoreResults.isValid(this._percent))
            throw new Error("instance of CoreResults is invalid");

        return this._percent;
    }

    caliper() {

        if (this.state === false || !CoreResults.isValid(this._caliper))
            throw new Error("instance of CoreResults is invalid");

        return this._caliper;
    }

    hwi() {

        if (this.state === false || !CoreResults.isValid(this._hwi))
            throw new Error("instance of CoreResults is invalid");

        return this._hwi;
    }

    spin() {

        if (this.state === false || !CoreResults.isValid(this._spin))
            throw new Error("instance of CoreResults is invalid");

        return this._spin;
    }

    // Public Static:
    static isValid(value) {
        return (value === undefined || value === null || isNaN(value)) ? false : true;
    }

    static checkParameter(param) {

        if (param === undefined || param === null || param instanceof Object || param instanceof Array || param instanceof Function || isNaN(param))
            throw new Error("Invalid parameter");

        return param;
    }
}

class CoreMacroMathJS {

    constructor(GlobalParser) {

        if (GlobalParser === undefined || GlobalParser === null || !(GlobalParser instanceof Function))
            throw new Error("Global Parser is invalid");

        this.parser = GlobalParser;

        this.inputs_property = [
            'distancia',
            'altura',
            'vento',
            'angulo',
            'slope_bola',
            'slope_green',
            'terreno',
            'power'
        ];

        this.results_property = [
            'desvio',
            'percent',
            'caliper',
            'hwi',
            'spin'
        ];

        this.macro_property = [
            'name',
            'parameters',
            'body'
        ];
    }

    // Public:
    execMacro(input) {

        try {

            if (!(input instanceof Object) || !input.hasOwnProperty('inputs') || !input.hasOwnProperty('results') || !input.hasOwnProperty('parameters') || !input.hasOwnProperty('macro')
                    || !(input.inputs instanceof Object) || !(input.results instanceof Object) || !(input.parameters instanceof Array) || !(input.macro instanceof Object))
                throw new Error("Invalid input");

            let inputs = this.makeInputsObject(input.inputs);
            let results = this.makeResultsObject(input.results);
            let macro = this.checkObjectMacro(input.macro);
            let parameters = this.checkParameters(input.parameters);

            // Parameters default
            parameters.unshift(inputs, results);

            if (parameters.length < 2)
                throw new Error("Unknown error, number{" + parameters.length + "} of parameters is invalid");

            if (parameters.length > 2 && macro.parameters.length != parameters.length)
                throw new Error(`call macro with number of parameters diferent from requested to the macro. Macro request ${macro.parameters.length} argument(s) and was passed ${parameters.length} argument(s).`);

            // Make parser
            let parser = this.parser();

            for (let i = 0; i < macro.parameters.length; i++)
                parser.set(macro.parameters[i], parameters[i]);

            // Check Code
            let code = CoreMacroMathJS.checkCode(macro.body);

            let lines = code.split('\n');

            let result = undefined;

            for (let i = 0; i < lines.length; i++) {

                try {

                    if (lines[i] !== '')
                        result = parser.evaluate(lines[i]);

                }catch (e) {

                    throw new Error("Error na linha: " + (i+1) + ", Message: " + e.message);
                }
            }

            // Clear parser
            parser.clear();

            return this.resultToString(result);

        }catch (e) {

            throw new Error("Error ao executar o macro, " + e.message);
        }
    }

    execExpression(expression) {

        try {
            
            if (expression instanceof Object || expression instanceof Function || expression instanceof Array || expression === undefined || expression === null || expression === '')
                throw new Error("Invalid expression");

            if (expression.length >= 200)
                throw new Error("Invalid expression, muito grande");

            let parser = this.parser();

            let code = CoreMacroMathJS.checkCode(expression);

            let ret = parser.evaluate(code);

            if (ret === undefined || ret === null || ret === '' || (ret instanceof Array && ret.length <= 0))
                throw new Error("Couldn't execute expression");

            return this.resultToString(ret);
        
        }catch (e) {

            throw new Error("Error in execExpression, " + e.message);
        }
    }

    openMacro(input) {

        try {

            if (!(input instanceof Object) || !input.hasOwnProperty('macro') || !(input.macro instanceof Object))
                throw new Error("Invalid input");

            let macro = this.checkObjectMacro(input.macro);

            if (macro === undefined || macro === null || !(macro instanceof Object))
                throw new Error("Invalid Macro");

            return macro;

        }catch (e) {

            throw new Error("Error ao abrir macro, " + e.message);
        }
    }

    // Private:
    resultToString(result) {

        if (result === undefined || result === null)
            return undefined;
        
        if (result instanceof Function)
            return "[Function]";

        if (result instanceof Object)
            return "[Object]";

        return result.toString();
    }

    makeInputsObject(inputs) {

        inputs = this.checkObjectInputs(inputs);

        let arr = [];

        this.inputs_property.forEach((prop) => {
            arr.push(inputs[prop]);
        });

        if (arr.length <= 0)
            throw new Error("Unknown error in make inputs Object");

        // apply, first arg
        arr.unshift(null);

        return new (Function.prototype.bind.apply(CoreInputs, arr));
    }

    makeResultsObject(results) {

        results = this.checkObjectResults(results);

        let arr = [];

        this.results_property.forEach((prop) => {
            arr.push(results[prop]);
        });

        if (arr.length <= 0)
            throw new Error("Unknown error in make results Object");

        // apply, first arg
        arr.unshift(null);

        return new (Function.prototype.bind.apply(CoreResults, arr));
    }

    checkObjectInputs(inputs) {

        if (!(inputs instanceof Object))
            throw new Error("Invalid Inputs Object");

        this.inputs_property.forEach((prop) => {
            if (!inputs.hasOwnProperty(prop))
                throw new Error("Invalid Inputs Object not have property{" + prop + "}");
        });     

        return inputs;
    }

    checkObjectResults(results) {

        if (!(results instanceof Object))
            throw new Error("Invalid Results Object");

        this.results_property.forEach((prop) => {
            if (!results.hasOwnProperty(prop))
                throw new Error("Invalid Results Object not have property{" + prop + "}");
        });

        return results;
    }

    checkObjectMacro(macro) {

        if (!(macro instanceof Object))
            throw new Error("Invalid Macro Object");

        this.macro_property.forEach((prop) => {
            if (!macro.hasOwnProperty(prop))
                throw new Error("Invalid Macro Object not have property{" + prop + "}");
        });

        for (let i = 0; i < macro.parameters.length; i++)
            if (macro.parameters[i] === undefined || macro.parameters[i] === null || macro.parameters[i] === '' 
                    || macro.parameters[i] instanceof Object || macro.parameters[i] instanceof Function)
                throw new Error("Parameter {" + macro.parameters[i] + "} of macro is invalid");

        // Check size body
        if (macro.body.length >= 5000)
            throw new Error("Macro is invalid, muito grande");

        return macro;
    }

    checkParameters(parameters) {

        for (let i = 0; i < parameters.length; i++)
            if (parameters[i] === undefined || parameters[i] === null || parameters[i] === '' 
                    || parameters[i] instanceof Object || parameters[i] instanceof Function)
                throw new Error("Parameter {" + parameters[i].toString() + "} sended is invalid");

        return parameters;
    }

    // Private Static:
    static checkCode(body) {

        if (body === null || body === undefined || body === '')
            return body;

        let re1 = new RegExp(/\/\*(\*(?!\/)|[^*])*\*\//ig);
        let re2 = new RegExp(/\/\/.*(\n|\r|\n\r|\r\n)*/ig);
        let re3 = new RegExp(/\\\t|\t/ig);

        body = body.replace(re1, '');
        body = body.replace(re2, '');
        body = body.replace(re3, '\\t');
        body = body.replace(/;/ig, ' ');
        
        return body;
    }
}

class MacroManager extends MacroBaseInject {

    constructor(base) {

        // Macro Base Inject
        super(base);
    }

    makeMacro(name, autor, body, parameters, callback) {

        if (MacroManager.checkParameterString(name) || MacroManager.checkParameterString(autor)
            || body == '' || !(parameters instanceof Array)) {
            
            callback({
                message: 'Invalid parameters',
                code: 30000
            }, undefined);

            return;
        }

        this.name = autor;
        this.body = body;
        this.parameters = parameters;

        this.state = true;

        if (!this.state) {
            
            callback({
                message: 'Fail to init macro',
                code: 30001
            }, undefined);

            return;
        }

        try {

            this.setFileName(name);

            this.writeFile((err, id) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                // Reply
                callback(null, this);
            });

        }catch (err) {

            console.log("Error: " + err.message);

            callback({
                message: 'Failt to make calculator',
                code: 205
            }, undefined);
        }
    }

    static checkParameterString(parameter) {
        return parameter === undefined || parameter === null || parameter instanceof Object || parameter instanceof Function || parameter instanceof ArrayBuffer || parameter === '';
    }

    static checkParameterNumber(parameter) {
        return parameter === undefined || parameter === null || parameter instanceof Object || parameter instanceof Function || parameter instanceof ArrayBuffer || isNaN(parameter);
    }

    // Sobrecarga
    onOpenedFile() {
        // Faz nada aqui
    }
}

module.exports = {
    MacroManager: MacroManager,
    CoreMacroMathJS: CoreMacroMathJS,
    GlobalMathParser: GlobalMathParser
}