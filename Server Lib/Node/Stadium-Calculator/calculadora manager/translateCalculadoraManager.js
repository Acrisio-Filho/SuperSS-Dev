// Arquivo translateCalculadoraManager.js
// Criado em 25/07/2020 as 05:13 por Acrisio
// Definição da classe TranslateAPICalculadoraManager

const calc_manager = require('./api_calculadora_manager.js');
const ccm = require('../common/CacheManager.js')

const APIError = require('../type/APIErrorException.js');
const types = require('../type/type');

const { cipherUID } = require('../util/util')

// My Error Exception
const MyError = APIError.APIErrorException;

// Input Object
class inputObject {

    constructor(type) {

        if (type === undefined || type === null || type instanceof Object || type instanceof Function || type instanceof ArrayBuffer || type === '')
            throw new MyError("Invalid Object type. constructor");

        this.type = type;
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

            cache_mngr.listCalculadoras(this.uid, 'copy', (err, list) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                // Reply
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

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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
        
                // nome das tacadas
                let names = calc.shot.map((el, index) => {
                    return {
                        id: index,
                        name: calc_manager.CalculadoraManager.getShotName(el.block_type.find((el) => {
                            return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
                        }))
                    };
                });
        
                // Reply
                callback(null, JSON.stringify(names));
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    create(cache_mngr, callback) {
        
        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[create]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[create]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[create]");

        if (!this.hasOwnProperty('power') || this.power instanceof Object || this.power instanceof Function || this.power instanceof ArrayBuffer || this.power === '')
            throw new MyError("Invalid Object Type[create]");

        if (!this.hasOwnProperty('permission') || this.permission instanceof Object || this.permission instanceof Function || this.permission instanceof ArrayBuffer || isNaN(this.permission))
            throw new MyError('Invalid Object Type[create]');

        const uid_cipher = cipherUID(this.uid);

        if (uid_cipher == null)
            throw new MyError('Invalid uid Object Type[create]');

        const name_file = `${uid_cipher}.${this.name}-${this.power}`;

        try {

            cache_mngr.makeCalculadora(name_file, this.uid, this.permission, this.name, this.power, (err, id) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                // Reply
                calc_manager.CalculadoraManager.listFiles(this.uid, 'copy', (err, list) => {

                    if (err) {

                        callback(err, undefined);

                        return;
                    }

                    // passa a lista de calculadoras
                    callback(null, JSON.stringify({
                        name: name_file,
                        list: list
                    }));
                });
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error fail to create Calculadora[" + name_file + "]", err.code);
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

            cache_mngr.copyCalculadora(this.name, this.uid, (err, copy_name) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (copy_name === undefined) {
                    
                    callback({
                        message: "Fail to copy Calculadora[" + this.name + "]",
                        code: 20001
                    }, undefined);

                    return;
                }

                calc_manager.CalculadoraManager.listFiles(this.uid, 'copy', (err, list) => {

                    if (err) {

                        callback(err, undefined);

                        return;
                    }

                    // Reply
                    callback(null, JSON.stringify({
                        name: copy_name,
                        list: list
                    }));
                });
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error: fail to copy Calculadora[" + this.name + "]", err.code);
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

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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

                if (calc.uid != this.uid) {

                    callback({
                        message: `You don't owner th calculator[${this.name}]`,
                        code: 30
                    }, undefined);

                    return;
                }

                try {
            
                    cache_mngr.deleteCalculadora(this.name, calc.id, (err) => {

                        if (err) {

                            callback(err, undefined);

                            return;
                        }

                        // Sucessful send calculadora lista
                        calc_manager.CalculadoraManager.listFiles(this.uid, 'copy', (err, list) => {

                            if (err) {

                                callback(err, undefined);

                                return;
                            }

                            callback(null, JSON.stringify(list));
                        });
                    });
        
                }catch(err) {
        
                    console.log("Error: " + err.message);
        
                    callback({
                        message: "Error fail to delete Calculadora[" + this.name + "]",
                        code: 8200
                    }, undefined);
                }
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    select(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[selecet]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[select]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[select]");

        if (!this.hasOwnProperty('shot') || this.shot instanceof Object || this.shot instanceof Function || this.shot instanceof ArrayBuffer || this.shot === undefined || isNaN(this.shot))
            throw new MyError("Invalid Object Type[select]");

        if (!this.hasOwnProperty('table') || this.table instanceof Object || this.table instanceof Function || this.table instanceof ArrayBuffer || this.table === undefined || isNaN(this.table))
            throw new MyError("Invalid Object Type[select]");

        try {

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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

                if (this.shot >= calc.shot.length) {
                    
                    callback({
                        message: "Invalid Shot[" + this.shot + "]",
                        code: 16002
                    }, undefined);

                    return;
                }

                let block_tab = calc.shot[this.shot].block_type.find((el) => {
                    return el.__type === this.table;
                });

                if (block_tab === undefined) {
                    
                    callback({
                        message: "Invalid Table Value[" + this.table + "]",
                        code: 16003
                    }, undefined);

                    return;
                }

                // Reply
                callback(null, JSON.stringify({
                    all_tab: calc.shot[this.shot].getAllTabType(types.eBLOCK_TYPE.SHOT.VALUE),
                    table: this.table,
                    line: block_tab.line,
                    column: block_tab.column,
                    map: block_tab.map
                }));
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    edit(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[edit]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[edit]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[edit]");

        if (!this.hasOwnProperty('shot') || this.shot instanceof Object || this.shot instanceof Function || this.shot instanceof ArrayBuffer || this.shot === undefined || isNaN(this.shot))
            throw new MyError("Invalid Object Type[edit]");

        if (!this.hasOwnProperty('table') || this.table instanceof Object || this.table instanceof Function || this.table instanceof ArrayBuffer || this.table === undefined || isNaN(this.table))
            throw new MyError("Invalid Object Type[edit]");

        if (!this.hasOwnProperty('table_size') || !(this.table_size instanceof Object) || !this.table_size.hasOwnProperty('line') || !this.table_size.hasOwnProperty('column')
                || this.table_size.line === undefined || this.table_size.line == null || this.table_size.column === undefined || this.table_size.column === null || isNaN(this.table_size.line) || isNaN(this.table_size.column))
            throw new MyError("Invalid Object Type[edit");

        if (!this.hasOwnProperty('values') || this.values instanceof Function || !(this.values instanceof Array) || this.values.length <= 0)
            throw new MyError("Invalid Object Type[edit]");
        
        this.values.forEach((el) => {
            
            if (el === undefined || el === null || isNaN(el))
                throw new MyError("Invalid Object Type[edit]");
        });

        try {

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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

                try {

                    // Edit shot[Table] values
                    calc.editShotTableValues(this.shot, this.table, this.table_size, this.values);

                    // Reply
                    callback(null, JSON.stringify({
                        message: "Shot saved with successful"
                    }));
        
                }catch (err) {
        
                    console.log("Error: " + err.message);
        
                    callback({
                        message: `Error to edit shot(${this.shot}) table(${this.table}) values in calculadora[${this.name}].`,
                        code: 400
                    }, undefined);
                }
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    save(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[save]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[save]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[save]");

        try {

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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

                try {

                    cache_mngr.writeCacheInFile(this.name, (err, id) => {

                        if (err) {

                            callback(err, undefined);

                            return;
                        }
            
                        // Reply
                        callback(null, JSON.stringify({
                            message: "Calculadora salvada com sucesso"
                        }));
                    });
        
                }catch (err) {
        
                    console.log("Error: " + err.message);
        
                    callback({
                        message: "Fail to save Calculadora[" + this.name + "]",
                        code: 310
                    }, undefined);
                }
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    remove(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[remove]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[remove]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[remove]");

        if (!this.hasOwnProperty('shot') || this.shot instanceof Object || this.shot instanceof Function || this.shot instanceof ArrayBuffer || this.shot === undefined || isNaN(this.shot))
            throw new MyError("Invalid Object Type[remove]");

        try {

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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

                try {

                    if (calc.deleteTab(this.shot) !== 0)
                        throw new MyError("Fail to delete Shot[" + this.shot + "]");

                    // nome das tacadas
                    let names = calc.shot.map((el, index) => {
                        return {
                            id: index,
                            name: calc_manager.CalculadoraManager.getShotName(el.block_type.find((el) => {
                                return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
                            }))
                        };
                    });
            
                    // Reply
                    callback(null, JSON.stringify(names));
        
                }catch (err) {
        
                    console.log("Error: " + err.message);
        
                    callback({
                        message: "Error to delete Shot[" + this.shot + "] from Calculadora[" + this.name + "]",
                        code: 401
                    }, undefined);
                }
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    make(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[make]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[make]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[make]");

        if (!this.hasOwnProperty('shot') || !(this.shot instanceof Object))
            throw new MyError("Invalid Object Type[make]");

        try {

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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

                try {

                    calc.makeShot(this.shot);

                    // nome das tacadas
                    let names = calc.shot.map((el, index) => {
                        return {
                            id: index,
                            name: calc_manager.CalculadoraManager.getShotName(el.block_type.find((el) => {
                                return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
                            }))
                        };
                    });

                    // Reply
                    callback(null, JSON.stringify(names));

                }catch (err) {

                    console.log("Error: " + err.message);

                    callback({
                        message: "Error to make Shot in Calculadora[" + this.name + "]",
                        code: err.code
                    }, undefined);
                }                
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    edit_shot(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[edit_shot]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[edit_shot]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[edit_shot]");

        if (!this.hasOwnProperty('index_shot') || this.index_shot instanceof Object || this.index_shot instanceof Function || this.index_shot instanceof ArrayBuffer || isNaN(this.index_shot) || this.index_shot === -1)
            throw new MyError("Invalid Object Type[edit_shot]");

        if (!this.hasOwnProperty('shot') || !(this.shot instanceof Object))
            throw new MyError("Invalid Object Type[edit_shot]");

        try {

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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

                try {

                    calc.editShot(this.index_shot, this.shot);

                    // nome das tacadas
                    let names = calc.shot.map((el, index) => {
                        return {
                            id: index,
                            name: calc_manager.CalculadoraManager.getShotName(el.block_type.find((el) => {
                                return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
                            }))
                        };
                    });
            
                    // Reply
                    callback(null, JSON.stringify(names));
        
                }catch (err) {
        
                    console.log("Error: " + err.message);
        
                    callback({
                        message: "Error to edit Shot(" + this.index_shot + ") in Calculadora[" + this.name + "]",
                        code: err.code
                    }, undefined);
                }
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    copy_shot(cache_mngr, callback) {

        if (this.checkManagerParameter(cache_mngr))
            throw new MyError("Invalid parameter cache_mngr Type[copy_shot]");

        if (this.checkUIDParameter())
            throw new MyError("Invalid Object Type[copy_shot]");

        if (!this.hasOwnProperty('name') || this.name instanceof Object || this.name instanceof Function || this.name instanceof ArrayBuffer || this.name === '')
            throw new MyError("Invalid Object Type[copy_shot]");

        if (!this.hasOwnProperty('index_shot') || this.index_shot instanceof Object || this.index_shot instanceof Function || this.index_shot instanceof ArrayBuffer || isNaN(this.index_shot) || this.index_shot === -1)
            throw new MyError("Invalid Object Type[copy_shot]");

        try {

            cache_mngr.getCalculadoraManager(this.name, (err, calc) => {

                if (err) {

                    callback(err, undefined);

                    return;
                }

                if (calc === undefined || calc === null || !(calc instanceof calc_manager.CalculadoraManager)) {
                    
                    callback({
                        message: "Failt to open Calculadora[" + this.name + "]",
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

                try {

                    calc.copyShot(this.index_shot);

                    // nome das tacadas
                    let names = calc.shot.map((el, index) => {
                        return {
                            id: index,
                            name: calc_manager.CalculadoraManager.getShotName(el.block_type.find((el) => {
                                return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
                            }))
                        };
                    });
            
                    // Reply
                    callback(null, JSON.stringify(names));
        
                }catch (err) {
        
                    console.log("Error: " + err.message);
        
                    callback({
                        message: "Error to copy Shot(" + this.index_shot + ") in Calculadora[" + this.name + "]",
                        code: err.code
                    }, undefined);
                }
            });

        }catch (err) {

            console.log("Error: " + err.message);

            throw new MyError("Error to open calculadora[" + this.name + "]", (err.hasOwnProperty('code') && err.code.toUpperCase() === 'ENOENT' ? 1 : 0));
        }
    }

    notify(e) {

        // Carregou o arquivo com sucesso
    }
}

// Translate API Calculadora Manager
class TranslateAPICalculadoraManager {

    constructor(cache_mngr) {

        this.cache_mngr = cache_mngr;

        this.type = [
            'list',         // Lista as calculadoras
            'open',         // Abre uma calculadora
            'save',         // Salva a calculadora
            'create',       // Cria uma calculadora
            'delete',       // Deleta a calculadora
            'select',       // Seleciona uma tacada
            'edit',         // Edita a tacada
            'edit_shot',    // Edita os info da tacada
            'make',         // Constroi uma tacada
            'remove',       // Deleta uma tacada
            'copy',         // Cópia uma calculadora
            'copy_shot'     // Cópia uma tacada
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
            console.log(`[TranslateAPICalculadoraManager::requestTranslate][Error] ${err.message}`);

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

        // Reply Error
        callback({
            message: `Unknown type[${obj.type}]`,
            code: 15000
        }, undefined);
    }
}

module.exports = {
    TranslateAPICalculadoraManager: TranslateAPICalculadoraManager
}