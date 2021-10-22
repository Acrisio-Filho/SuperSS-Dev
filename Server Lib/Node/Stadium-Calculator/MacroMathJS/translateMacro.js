// Arquivo translateMacro.js
// Criado em 16/07/2020 as 12:03 por Acrisio
// Definição do Core do Macro Math JS e alguns exemplos

const ccg = require('./api_macro.js');
const ccm = require('../common/CacheManager');
const APIError = require('../type/APIErrorException.js');
const { cipherUID } = require('../util/util');

// My Error Exception
const MyError = APIError.APIErrorException;

const coreGlobal = new ccg.CoreMacroMathJS(ccg.GlobalMathParser);

// Input Object
class inputObject {

    constructor(type) {

        if (type === undefined || type === null || type instanceof Object || type instanceof Function || type instanceof ArrayBuffer || type === '')
            throw new MyError("Invalid Object type");

        this.type = type;

        this.input_property = {
            'inputs': {
                NAME: 'inputs',
                TYPE: Object
            },
            'results': {
                NAME: 'results',
                TYPE: Object
            },
            'parameters': {
                NAME: 'parameters',
                TYPE: Array
            },
            'macro': {
                NAME: 'macro',
                TYPE: Object
            }
        };

        this.input_property_save = {
            'macro': {
                NAME: 'macro',
                TYPE: Object
            }
        };
    }

    checkUIDParameter() {
        return (!this.hasOwnProperty('uid') || this.uid instanceof Object || this.uid instanceof Function || this.uid instanceof ArrayBuffer || isNaN(this.uid));
    }

    checkManagerParameter(cache_mngr) {
        return (cache_mngr === undefined || cache_mngr === null || !(cache_mngr instanceof ccm.CacheManager));
    }

    // View and Copy
    list(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[list]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[list]");

        try {

            cache_mngr.listMacros(this.uid, 'copy', (err, list) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                // Reply
                callback(null, JSON.stringify(list));
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to list Macros.");
        }
    }

    // View
    listex(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[listex]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[listex]");

        var num_macros = 0;
        var error_once = false;
        const list_macro = [];

        const pushAndSendMacroList = (macro) => {

            if (!error_once) {

                list_macro.push({
                    name: macro.name,
                    parameters: macro.parameters
                });

                // Reply
                if (list_macro.length == num_macros)
                    callback(null, JSON.stringify(list_macro));
            }
        };

        try {

            cache_mngr.listMacros(this.uid, 'view', (err, list) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                num_macros = list.length;

                list.forEach(el => {

                    cache_mngr.getMacro(el.name, (err, macro) => {

                        if (err) {

                            callback({
                                message: `Fail to listex macro`,
                                code: 12
                            }, undefined);

                            error_once = true;

                            return;
                        }

                        pushAndSendMacroList(macro);
                    });
                });
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to listEx Macros.");
        }
    }

    params(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[params]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[params]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[params]");

        try {

            cache_mngr.getMacro(this.name, (err, macro) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                // Reply
                callback(null, JSON.stringify({
                    parameters: macro.parameters
                }));
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to get Params Macro[" + this.name + "]", err.code);
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

            cache_mngr.getMacro(this.name, (err, macro) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                // Reply
                callback(null, JSON.stringify({
                    name: macro.name,
                    body: macro.body,
                    parameters: macro.parameters
                }));
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open Macro[" + this.name + "]", err.code);
        }
    }

    exec(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[exec]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[exec]");

        if (!this.hasOwnProperty('name') || this.name === undefined || this.name === null || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[exec]");

        if (!this.hasOwnProperty('inputs') || this.inputs instanceof Object || this.inputs instanceof Function || this.inputs instanceof ArrayBuffer || this.inputs === '')
            throw new MyError("Invalid Object Type[exec]");

        if (!this.hasOwnProperty('results') || this.results instanceof Object || this.results instanceof Function || this.results instanceof ArrayBuffer || this.results === '')
            throw new MyError("Invalid Object Type[exec]");

        if (!this.hasOwnProperty('parameters') || this.parameters instanceof Object || this.parameters instanceof Function || this.parameters instanceof ArrayBuffer || this.parameters === '')
            throw new MyError("Invalid Object Type[exec]");

        try {

            cache_mngr.getMacro(this.name, (err, macro) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                const obj = this.makeObjectInputMacro({
                    inputs: this.inputs,
                    results: this.results,
                    parameters: this.parameters,
                    macro: JSON.stringify({
                        name: macro.name,
                        body: macro.body,
                        parameters: macro.parameters
                    })
                });
    
                if (obj === undefined || obj === null || !(obj instanceof Object)) {
                    
                    callback({
                        message: "Invalid Object Input Macro", 
                        code: 6000
                    }, undefined);

                    return;
                }
    
                let secureReturn = coreGlobal.execMacro(obj);
    
                // Log
                console.log(`Result: ${secureReturn}`);
    
                // Reply
                callback(null, JSON.stringify({
                    message: secureReturn
                }));

            });

        }catch (err) {

            console.log(err.message);

            callback({
                message: err.message,
                code: err.code
            }, undefined);
        }
    }
    
    save(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[save]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[save]");

        if (!this.hasOwnProperty('input') || this.input === undefined || this.input === null || this.input instanceof Object || this.input instanceof Function || this.input instanceof ArrayBuffer || this.input === '')
            throw new MyError("Invalid Object Type[save]");

        if (!this.hasOwnProperty('permission') || this.permission === undefined || this.permission === null 
                || this.permission instanceof Object || this.permission instanceof Function || this.permission instanceof ArrayBuffer || isNaN(this.permission))
            throw new MyError('Invalid Object Type[save]');

        try {

            let obj = JSON.parse(this.input);

            if (obj === undefined || obj === null || !(obj instanceof Object))
                throw new MyError("Error to parser JSON data");

            obj = this.makeObjectInputMacroSave(obj);

            if (obj === undefined || obj === null || !(obj instanceof Object))
                throw new MyError("Invalid Object Input Macro");

            if (!obj.hasOwnProperty('macro') || !(obj.macro instanceof Object))
                throw new MyError("Invalid Object Input Macro");

            // Open and check object Macro
            const inputMacro = coreGlobal.openMacro(obj);

            // Check Code
            inputMacro.body = ccg.CoreMacroMathJS.checkCode(inputMacro.body);

            const uid_cipher = cipherUID(this.uid);

            if (uid_cipher == null)
                throw new MyError('Invalid uid Object Type[save]');
    
            const name_file = `${uid_cipher}.${inputMacro.name}`;

            cache_mngr.getMacro(name_file, (err, macro) => {

                if (err) {

                    if (err.code == 9 || err.code == 8004/*Macro not exists*/) {

                        // Make macro
                        cache_mngr.makeMacro(name_file, this.uid, this.permission, name_file, inputMacro.body, inputMacro.parameters, (err, macro) => {

                            if (err) {

                                callback(err, undefined);

                                return;
                            }

                            // Reply
                            callback(null, JSON.stringify({
                                message: 'Macro has saved with success',
                                macro: {
                                    name: name_file,
                                    body: 'none',
                                    parameters: ['inputs', 'results']
                                }
                            }));
                        })

                        return;
                    }

                    callback(err, undefined);

                    return;
                }

                // Update
                macro.name = name_file;
                macro.body = inputMacro.body;
                macro.parameters = inputMacro.parameters;

                try {

                    cache_mngr.writeCacheInFile(name_file, (err, id) => {

                        if (err) {

                            callback(err, undefined);

                            return;
                        }
                        
                        // Reply
                        callback(null, JSON.stringify({
                            message: `Macro has saved with success`,
                            macro: {
                                name: name_file,
                                body: 'none',
                                parameters: ['inputs', 'results']
                            }
                        }));
                    });

                }catch (err) {

                    console.log(`Error to save macro[${name_file}]. Message: ${err.message}`);

                    callback({
                        message: `Fail to save macro[${name_file}]`,
                        code: 310
                    }, undefined);
                }

            });

        }catch (err) {

            // Log
            console.log(err.message);

            callback({
                mesage: err.message,
                code: err.code
            }. undefined);
        }
    }

    delete(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[delete]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[delete]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[delete]");

        try {

            cache_mngr.getMacro(this.name, (err, macro) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                try {

                    cache_mngr.deleteMacro(this.name, macro.id, (err) => {

                        if (err) {

                            callback(err, undefined);

                            return;
                        }

                        // Reply
                        callback(null, JSON.stringify({
                            name: this.name
                        }));
                    });
                
                }catch (err) {

                    console.log(`Error to delete macro[${this.name}]. Message: ${err.message}`);

                    callback({
                        message: `Fail to delete Macro[${this.name}]`,
                        code: 8200
                    }, undefined);
                }
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to delete Macro[" + this.name + "]");
        }
    }

    copy(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[copy]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[copy]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[copy]");

        try {

            cache_mngr.copyMacro(this.name, this.uid, (err, copy_name) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (copy_name === undefined) {
                    
                    callback({
                        message: "Fail to copy Macro[" + this.name + "]",
                        code: 20001
                    }, undefined);

                    return;
                }

                cache_mngr.getMacro(copy_name, (err, macro) => {

                    if (err) {

                        callback(err, undefined);

                        return;
                    }

                    macro.name = copy_name;

                    try {

                        cache_mngr.writeCacheInFile(copy_name, (err, id) => {

                            if (err) {

                                callback(err, undefined);

                                return;
                            }

                            // Reply
                            callback(null, JSON.stringify({
                                message: `Macro[${this.name}] cópiado para Macro[${copy_name}] com sucesso`,
                                macro: {
                                    name: copy_name,
                                    body: 'none',
                                    parameters: ['inputs', 'results']
                                }
                            }));
                        })

                    }catch (err) {

                        console.log(`Fail to writeCacheInFile[${copy_name}]. Message: ${err.message}`);

                        callback({
                            message: `Fail to copy Macro[${copy_name}]`,
                            code: 10001,
                        }, undefined);
                    }
                });
            })

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to copy Macro[" + this.name + "]");
        }
    }

    expr(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[expr]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[expr]");

        if (!this.hasOwnProperty('expression') || this.expression instanceof Object || this.expression instanceof Function || this.expression instanceof ArrayBuffer || this.expression === '')
            throw new MyError("Invalid Object Type[expr]");

        try {

            let secureReturn = coreGlobal.execExpression(this.expression);

            // Log
            console.log(`Result: ${secureReturn}`);

            // Reply
            callback(null, JSON.stringify({
                result: secureReturn
            }));

        }catch (e) {

            // Log
            console.log("Error to execute expression. Message: " + e.message);

            throw new MyError("Error to execute expression. Message: " + e.message, e.code);
        }
    }

    notify(e) {

        // Carregou o arquivo com sucesso
    }

    // Private:
    makeObjectInputMacro(obj) {

        if (!(obj instanceof Object))
            throw new MyError("Invalid Object Input Macro");

        let name = undefined, type = undefined;

        for (let prop in this.input_property) {
            
            if (this.input_property.hasOwnProperty(prop)) {

                name = this.input_property[prop].NAME;
                type = this.input_property[prop].TYPE;

                if (!obj.hasOwnProperty(name) || obj[name] === undefined || obj[name] === null || obj[name] === '')
                    throw new MyError("Error Object Input Macro not have {" + prop + "}");

                // Parse JSON Object Prop
                obj[name] = JSON.parse(obj[name]);

                if (obj[name] === undefined || obj[name] === null || !(obj[name] instanceof type))
                    throw new MyError("Error to parser JSON data from " + name);
            }
        }

        return obj;
    }

    makeObjectInputMacroSave(obj) {

        if (!(obj instanceof Object))
            throw new Error("Invalid Object Input Macro");

        let name = undefined, type = undefined;

        for (let prop in this.input_property_save) {
            
            if (this.input_property_save.hasOwnProperty(prop)) {

                name = this.input_property_save[prop].NAME;
                type = this.input_property_save[prop].TYPE;

                if (!obj.hasOwnProperty(name) || obj[name] === undefined || obj[name] === null || obj[name] === '')
                    throw new MyError("Error Object Input Macro not have {" + prop + "}");

                // Parse JSON Object Prop
                obj[name] = JSON.parse(obj[name]);

                if (obj[name] === undefined || obj[name] === null || !(obj[name] instanceof type))
                    throw new MyError("Error to parser JSON data from " + name);
            }
        }

        return obj;
    }
}

// Class API Macro Math JS
class TranslateAPIMacroMathJS {

    constructor(cache_mngr) {

        this.cache_mngr = cache_mngr;

        this.input_property = {
            'inputs': {
                NAME: 'inputs',
                TYPE: Object
            },
            'results': {
                NAME: 'results',
                TYPE: Object
            },
            'parameters': {
                NAME: 'parameters',
                TYPE: Array
            },
            'macro': {
                NAME: 'macro',
                TYPE: Object
            }
        };

        this.type = [
            'list',
            'listex',
            'open',
            'params',
            'exec',
            'save',
            'delete',
            'copy',
            'expr'
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
            console.log(`[TranslateAPIMacroMathJS::requestTranslate][Error] ${err.message}`);

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

                return; // Finish
            }
        }

        callback({
            message: `Unknown type[${obj.type}]`,
            code: 8000
        }, undefined);
    }
}

module.exports = {
    TranslateAPIMacroMathJS: TranslateAPIMacroMathJS
};