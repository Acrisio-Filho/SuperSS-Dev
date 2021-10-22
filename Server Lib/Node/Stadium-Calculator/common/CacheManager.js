// Arquivo CacheManager.js
// Criado em 27/07/2020 as 01:51 por Acrisio
// Definição da classe CacheManager

const MyError = require('../type/APIErrorException').APIErrorException;
const cbi = require('./CalculadoraBaseInject.js');
const MacroBase = require('./macro_base');
const api_calc = require('../calculadora/api_calculadora.js')
const api_mngr = require('../calculadora manager/api_calculadora_manager.js');
const api_macro = require('../MacroMathJS/api_macro');
const cah = require('async_hooks');
const { eTYPE_CACHE, CacheObject } = require('./CacheObject');
const CacheObjectBase = require('./CacheObject_base');
const { cipherUID } = require('../util/util');

const DEFAULT_LIMIT_ENTRY_CACHES = 100;         // 100 caches carregados
const TIME_EXPIRES_CACHE = (5 * 60 * 1000);     // 5 minutos
const INTERVAL_CLEAR_CACHE = (2 * 60 * 1000);   // 2 minutos

class CacheManager extends cah.AsyncLocalStorage {

    constructor(limite_cache = DEFAULT_LIMIT_ENTRY_CACHES) {

        // Constructor do AsyncLocalStorage
        super();

        this.limete_cache = limite_cache;
        
        // Initialize caches
        this.enterWith(/*Array cache*/[]);

        // Initialize Clear chaches
        this.interval_clear = setInterval(((thisArg) => {
            return () => {
                thisArg.onClear();
            };
        })(this), INTERVAL_CLEAR_CACHE);
    }

    get caches() {
        return this.getStore();
    }

    set caches(caches) {
        this.caches = caches;
    }

    // Public:
    listCalculadoras(uid, option, callback) {

        try {

            api_calc.Calculadora.listFiles(uid, option, callback);

        }catch (err) {

            console.log(`Error: ${err.message}`);

            throw new MyError(`Fail to list Calculators`);
        }
    }

    listMacros(uid, option, callback) {

        try {

            api_macro.MacroManager.listFiles(uid, option, callback);

        }catch (err) {

            console.log(`Error: ${err.message}`);

            throw new MyError(`Fail to list Macros`);
        }
    }

    // Public:
    getCalculadora(name, callback) {

        let cache = this.findCache(name);

        try {

            if (cache !== undefined) {

                if (cache.hasOwnProperty('calculadora') && cache.calculadora instanceof api_calc.Calculadora) {

                    // Access Date
                    cache.access_date = Date.now();

                    callback(null, cache.calculadora);

                    return;
                }

                cache.calculadora = new api_calc.Calculadora(cache.calc_base);

                cache.calculadora.onOpenedFile();

                // Access Date
                cache.access_date = Date.now();

                callback(null, cache.calculadora);

                return;
            }

            // Cria o cache
            this.makeCacheCalculadora(name, (err, cache) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (cache === undefined || cache === null || !(cache instanceof Object)) {
                   
                    callback({
                        message: "Fail to make Cahce of File[" + name + "]",
                        code: 8000
                    }, undefined);

                    return;
                }

                cache.calculadora = new api_calc.Calculadora(cache.calc_base);

                cache.calculadora.onOpenedFile();

                // Push new Cache
                this.caches.push(cache);

                // Reply
                callback(null, cache.calculadora);
            });

        }catch (err) {

            console.log("Error: ", err.message);

            throw new MyError("Fail to get Calculadora[" + name + "]");
        }
    }

    getCalculadoraManager(name, callback) {

        let cache = this.findCache(name);

        try {

            if (cache !== undefined) {

                if (cache.hasOwnProperty('manager') && cache.manager instanceof api_mngr.CalculadoraManager) {

                    // Access Date
                    cache.access_date = Date.now();

                    callback(null, cache.manager);

                    return;
                }

                cache.manager = new api_mngr.CalculadoraManager(cache.calc_base);

                cache.manager.onOpenedFile();

                // Access Date
                cache.access_date = Date.now();

                callback(null, cache.manager);

                return;
            }

            // Cria o cache
            this.makeCacheCalculadora(name, (err, cache) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (cache === undefined || cache === null || !(cache instanceof Object)) {
                    
                    callback({
                        message: "Fail to make Cahce of File[" + name + "]",
                        code: 8000
                    }, undefined);

                    return;
                }

                cache.manager = new api_mngr.CalculadoraManager(cache.calc_base);

                cache.manager.onOpenedFile();

                // Push new Cache
                this.caches.push(cache);

                callback(null, cache.manager);
            });

        }catch (err) {

            console.log("Error: ", err.message)

            throw new MyError("Fail to get Manager of Calculadora[" + name + "]");
        }
    }

    getMacro(name, callback) {

        let cache = this.findCache(name);

        try {

            if (cache !== undefined) {

                if (cache.hasOwnProperty('macro') && cache.macro instanceof api_macro.MacroManager) {

                    // Access Date
                    cache.access_date = Date.now();

                    callback(null, cache.macro);

                    return;
                }

                cache.macro = new api_macro.MacroManager(cache.calc_base);

                cache.macro.onOpenedFile();

                // Access Date
                cache.access_date = Date.now();

                callback(null, cache.macro);

                return;
            }

            // Cria o cache
            this.makeCacheMacro(name, (err, cache) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (cache === undefined || cache === null || !(cache instanceof Object)) {
                    
                    callback({
                        message: "Fail to make Cahce of File[" + name + "]",
                        code: 8000
                    }, undefined);

                    return;
                }

                cache.macro = new api_macro.MacroManager(cache.calc_base);

                cache.macro.onOpenedFile();

                // Push new Cache
                this.caches.push(cache);

                callback(null, cache.macro);
            });

        }catch (err) {

            console.log("Error: ", err.message)

            throw new MyError("Fail to get Manager of Macro[" + name + "]");
        }
    }

    makeCalculadora(name, uid, permission, autor, power, callback) {

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '')
            throw new MyError("Invalid parameters name");

        let cache = this.findCache(name);

        if (cache !== undefined && cache != null && cache instanceof Object)
            throw new MyError("Já existe essa calculadora[" + name + "]");

        api_mngr.CalculadoraManager.listFiles(uid, 'copy', (err, list) => {

            if (err) {

                callback(err, undefined);

                return;
            }

            if (list.find(el => {
                return el.name == name;
            }) != undefined) {
                
                callback({
                    message: `Calculator[${name}] already exists`,
                    code: 20000
                }, undefined);

                return;
            }

            const calc = new api_mngr.CalculadoraManager();

            calc.uid = uid;
            calc.permission = permission;

            calc.makeCalcualdora(name, 3, 201, autor, power, (err, id) => {

                if (err) {

                    callback({
                        message: "Error ao criar a Calculadora[" + name + "]",
                        code: 4
                    }, undefined);

                    return;
                }

                // Log
                console.log(`Calculator[${name}] created(id: ${id})`);
        
                // Push new Calculadora Cache
                this.caches.push({
                    calc_base: calc.base,
                    manager: calc
                });

                // Reply
                callback(null, id);
            });
        })
    }

    makeMacro(name, uid, permission, autor, body, parameters, callback) {

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '')
            throw new MyError("Invalid parameters name");

        let cache = this.findCache(name);

        if (cache !== undefined && cache != null && cache instanceof Object)
            throw new MyError("Já existe esse macro[" + name + "]");

        api_macro.MacroManager.listFiles(uid, 'copy', (err, list) => {

            if (err) {

                callback(err, undefined);

                return;
            }

            if (list.find(el => {
                return el.name == name;
            }) != undefined) {
                
                callback({
                    message: `Macro[${name}] already exists`,
                    code: 20000
                }, undefined);

                return;
            }

            const macro = new api_macro.MacroManager();

            macro.uid = uid;
            macro.permission = permission;

            macro.makeMacro(name, autor, body, parameters, (err, macro) => {

                if (err) {

                    callback({
                        message: "Error ao criar a Macro[" + name + "]",
                        code: 4
                    }, undefined);

                    return;
                }

                // Log
                console.log(`Macro[${name}] created(id: ${macro.id})`);
        
                // Push new Macro Cache
                this.caches.push({
                    calc_base: macro.base,
                    macro: macro
                });

                // Reply
                callback(null, macro);
            });
        })
    }

    deleteCalculadora(name, id, callback) {

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '')
            throw new MyError("Invalid parameters name");

        let cache = this.findCache(name);

        // Remove do cache se tiver
        if (cache !== undefined && cache !== null && cache instanceof Object)
            this.removeCache(name, (err) => {

                if (err) {

                    // Log
                    console.log("Fail to delete Cache File[" + name  + "]");

                    callback(err);

                    return;
                }

                // Call Again                
                this.deleteCalculadora(name, id, callback);
            });
        else
           CacheObject.deleteFile(name, id, eTYPE_CACHE.CALCULADORA, callback);
    }

    deleteMacro(name, id, callback) {

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '')
            throw new MyError("Invalid parameters name");

        let cache = this.findCache(name);

        // Remove do cache se tiver
        if (cache !== undefined && cache !== null && cache instanceof Object)
            this.removeCache(name, (err) => {

                if (err) {

                    // Log
                    console.log("Fail to delete Cache File[" + name  + "]");

                    callback(err);

                    return;
                }

                // Call Again                
                this.deleteMacro(name, id, callback);
            });
        else
           CacheObject.deleteFile(name, id, eTYPE_CACHE.MACRO, callback);
    }

    copyCalculadora(name, uid, callback) {

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '')
            throw new MyError("Invalid parameters name");

        try {

            api_mngr.CalculadoraManager.listFiles(uid, 'copy', (err, list) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (list.find(el => {
                    return el.name == name;
                }) == undefined) {

                    callback({
                        message: `fail to copy calculadora[${name}]`,
                        code: 20000
                    }, undefined);

                    return;
                }

                // Transfere assínatura de cópia
                let name_cpy = name;

                const sign = cipherUID(uid);

                if (sign != null)
                    name_cpy = `${sign}.${name.substr(name.indexOf('.') + 1)}`;

                const MAX_COPY = 10;

                let num_cpy = 0;
                let copy_name = `${name_cpy}[${++num_cpy}]`;

                let good_copy_name = false;

                while (num_cpy <= MAX_COPY && (good_copy_name = (list.find(el => {
                    return el.name == copy_name;
                }) != undefined)))
                    copy_name = `${name_cpy}[${++num_cpy}]`;

                if (good_copy_name) {
                    
                    callback({
                        message: `Calculadora[${name_cpy}] already has gone of limit of copy's`,
                        code: 20001
                    }, undefined);

                    return;
                }

                CacheObject.copyFile(name, copy_name, uid, eTYPE_CACHE.CALCULADORA, (err, id) => {

                    if (err) {

                        callback(err, undefined);

                        return;
                    }

                    // Log
                    console.log(`Calculadora[${name}] copiada para[${copy_name}], ID: ${id}`);

                    // Reply
                    callback(null, copy_name);
                });

            });

        }catch (err) {

            console.log(`Error: ${err.message}`);

            throw new MyError(`Failt to copy calculadora[${name}]`);
        }
    }

    copyMacro(name, uid, callback) {

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '')
            throw new MyError("Invalid parameters name");

        try {

            api_macro.MacroManager.listFiles(uid, 'copy', (err, list) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (list.find(el => {
                    return el.name == name;
                }) == undefined) {

                    callback({
                        message: `fail to copy macro[${name}]`,
                        code: 20000
                    }, undefined);

                    return;
                }

                // Transfere assínatura de cópia
                let name_cpy = name;

                const sign = cipherUID(uid);

                if (sign != null)
                    name_cpy = `${sign}.${name.substr(name.indexOf('.') + 1)}`;

                const MAX_COPY = 10;

                let num_cpy = 0;
                let copy_name = `${name_cpy}[${++num_cpy}]`;

                let good_copy_name = false;

                while (num_cpy <= MAX_COPY && (good_copy_name = (list.find(el => {
                    return el.name == copy_name;
                }) != undefined)))
                    copy_name = `${name_cpy}[${++num_cpy}]`;

                if (good_copy_name) {
                    
                    callback({
                        message: `Macro[${name_cpy}] already has gone of limit of copy's`,
                        code: 20001
                    }, undefined);

                    return;
                }

                CacheObject.copyFile(name, copy_name, uid, eTYPE_CACHE.MACRO, (err, id) => {

                    if (err) {

                        callback(err, undefined);

                        return;
                    }

                    // Log
                    console.log(`Macro[${name}] copiado para[${copy_name}], ID: ${id}`);

                    // Reply
                    callback(null, copy_name);
                });

            });

        }catch (err) {

            console.log(`Error: ${err.message}`);

            throw new MyError(`Failt to copy macro[${name}]`);
        }
    }

    writeCacheInFile(name, callback) {

        let cache = this.findCache(name);

        try {

            if (cache === undefined || cache === null || !(cache instanceof Object) || !cache.hasOwnProperty('calc_base') || !(cache.calc_base instanceof CacheObjectBase))
                throw new MyError("Cache[" + name + "] not exists");

            cache.calc_base.writeFile((err, id) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                // Log
                console.log(`Update Cache File[${name}] id: ${id}`);

                // Reply
                callback(null, id);
            });

        }catch (err) {

            console.log("Error to write cache to file[" + name + "]. Message: " + err.message);

            callback({
                message: "Error to write cache to file[" + name + "]",
                code: 8100
            }, undefined);
        }
    }

    // Private:
    makeCacheCalculadora(name, callback) {

        let cache = undefined;

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '') {

            callback({
                message: 'Invalid name',
                code: 8001
            }, undefined);

            return;
        }

        try {

            if (this.caches.length >= this.limete_cache) {

                // Passaou do limite do cache, procura o cache que não foi mexido a mais tempo e remove ele
                let old = this.findOldestCache();

                if (old === undefined || old === null || !(old instanceof Object)) {

                    callback({
                        message: 'Fail to find oldest cache',
                        code: 8002
                    }, undefined);

                    return; // undefined
                }

                this.removeCache(old.calc_base.getFileName(), (err) => {

                    if (err) {

                        callback({
                            message: 'Fail to remove cache',
                            code: 8003
                        }, undefined);
    
                        return;   // undefined
                    }

                    // Espaço liberado pode criar um novo cache
                    this.makeCacheCalculadora(name, callback);
                })

                return;
            }

            cache = new cbi.CalculadoraBase();

            cache.onOpenedFile = function() {}; // not throw exception

            cache.setFileName(name);

            cache.openFile((err, arg) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (!cache.state) {

                    callback({
                        message: "Fail to open Calculadora[" + name + "]",
                        code: 8004
                    }, undefined);

                    return;
                }

                // Object Cache
                cache = {
                    calc_base: cache,
                    create_date: Date.now(),
                    access_date: Date.now()
                };

                // Log
                console.log("Make Cache[" + cache.calc_base.cache + "] to File[" + name + "]");

                // Reply
                callback(null, cache);
            });

        }catch (err) {

            cache = undefined;

            console.log("Error to make Cache file[" + name + "]. Message: " + err.message);

            callback(err, undefined);
        }

    }

    makeCacheMacro(name, callback) {

        let cache = undefined;

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '') {

            callback({
                message: 'Invalid name',
                code: 8001
            }, undefined);

            return;
        }

        try {

            if (this.caches.length >= this.limete_cache) {

                // Passaou do limite do cache, procura o cache que não foi mexido a mais tempo e remove ele
                let old = this.findOldestCache();

                if (old === undefined || old === null || !(old instanceof Object)) {

                    callback({
                        message: 'Fail to find oldest cache',
                        code: 8002
                    }, undefined);

                    return; // undefined
                }

                this.removeCache(old.calc_base.getFileName(), (err) => {

                    if (err) {

                        callback({
                            message: 'Fail to remove cache',
                            code: 8003
                        }, undefined);
    
                        return;   // undefined
                    }

                    // Espaço liberado pode criar um novo cache
                    this.makeCacheMacro(name, callback);
                })

                return;
            }

            cache = new MacroBase();

            cache.onOpenedFile = function() {}; // not throw exception

            cache.setFileName(name);

            cache.openFile((err, arg) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (!cache.state) {

                    callback({
                        message: "Fail to open Macro[" + name + "]",
                        code: 8004
                    }, undefined);

                    return;
                }

                // Object Cache
                cache = {
                    calc_base: cache,
                    create_date: Date.now(),
                    access_date: Date.now()
                };

                // Log
                console.log("Make Cache[" + cache.calc_base.cache + "] to File[" + name + "]");

                // Reply
                callback(null, cache);
            });

        }catch (err) {

            cache = undefined;

            console.log("Error to make Cache file[" + name + "]. Message: " + err.message);

            callback(err, undefined);
        }

    }

    removeCache(name, callback) {

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '') {

            callback({
                message: 'Invalid parameter',
                code: 5000
            });

            return;
        }

        try {

            let cache_index = this.caches.findIndex((el) => {
                return el.calc_base.getFileName() === name;
            });

            if (cache_index !== -1) {

                const cache = this.caches[cache_index];

                if (cache.calc_base)
                    cache.calc_base.close((err) => {

                        if (err) {

                            callback(err);

                            return;
                        }

                        // Log
                        console.log("Removendo Cache[" + cache.calc_base.cache + "] of File[" + name + "]");
                        
                        // Find Index Again
                        cache_index = this.caches.findIndex((el) => {
                            return el.calc_base.cache == cache.calc_base.cache;
                        });

                        if (cache_index !== -1)
                            this.caches.splice(cache_index, 1);

                        // Reply
                        callback(null);
                    });
                else {

                    // Log
                    console.log("Removendo Cache[" + cache.calc_base.cache + "] of File[" + name + "]");
                    
                    // Find Index Again
                    cache_index = this.caches.findIndex((el) => {
                        return el.calc_base.cache == cache.calc_base.cache;
                    });

                    if (cache_index !== -1)
                        this.caches.splice(cache_index, 1);

                    // Reply
                    callback(null);
                }
            }

        }catch (err) {

            console.log("Error to delete Cache file[" + name + "]. Message: " + err.message);

            callback({
                message: `Fail to delete cache file[${name}`,
                code: 150
            });
        }
    }

    findCache(name) {

        let cache = undefined;

        if (name === undefined || name === null || name instanceof Object || name instanceof Function || name === '')
            return cache;

        try {

            cache = this.caches.find((el) => {
                return el.calc_base.getFileName() === name;
            });

        }catch (err) {

            cache = undefined;

            console.log("Error to find Cache file[" + name + "]. Message: " + err.message);
        }

        return cache;
    }

    findOldestCache() {

        let cache = undefined;

        try {

            let copy = this.caches.slice(0, this.caches.length);

            copy.sort((a, b) => {
                
                if (a.access_date == b.access_date)
                    return a.create_date - b.create_date;

                return a.access_date - b.access_date;
            });

            if (copy.length > 0)
                return copy[0];

        }catch (err) {

            cache = undefined;

            console.log("Error to find oldest Cache. Message: " + err.message);
        }

        return cache;
    }

    onClear() {

        try {

            // Log
            console.log("Limpando caches antigos...");

            let removes = this.caches.map((el) => {
                return (CacheManager.isValidCache(el) && (Date.now() - el.access_date) >= TIME_EXPIRES_CACHE) ? el.calc_base.getFileName() : null;
            }).filter((el) => {
                return el !== null;
            });

            removes.forEach((el) => {
                
                if (el !== undefined && el !== null && el !== '')
                    this.removeCache(el, (err) => {

                        if (err)
                            console.log(err);
                    });
            });

        }catch (err) {

            console.log("Error to clear old caches. Message: " + err.message);
        }
    }

    static isValidCache(cache) {
        return (cache !== undefined && cache !== null && cache instanceof Object && cache.hasOwnProperty('access_date') && cache.hasOwnProperty('create_date')
            && cache.hasOwnProperty('calc_base') && !isNaN(cache.access_date) && !isNaN(cache.create_date) && cache.calc_base instanceof cbi.CalculadoraBase);
    };
}

module.exports = {
    CacheManager: CacheManager
}