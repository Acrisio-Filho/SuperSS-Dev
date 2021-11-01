// Arquivo macro_base.js
// Criado em 29/11/2020 as 11:57 por Acrisio
// Definição e Implementação da classe MacroBase

const CacheObjectBase = require('./CacheObject_base');
const { postRequestCalculadoraStadium } = require('../util/util')
const { TOKEN } = require('./token');
const { eTYPE_CACHE } = require('./CacheObject');

class MacroBase extends CacheObjectBase {

    name = null;
    body = null;
    parameters = null;

    constructor() {

        // Cache Object Base
        super();

        this.current_file_name = null;

        this.name = null;
        this.body = null;
        this.parameters = null;

        this.state = false;
    }

    // Salva as modificações e e fecha
    close(callback) {

        if (this.state) {

            this.writeFile((err, id) => {

                if (err) {

                    callback(err);

                    return;
                }

                this.current_file_name = null;
                this.name = null;
                this.body = null;
                this.parameters = null;

                this.state = false;

                // Reply
                callback(null);
            });

        }else
            callback(null); // Close silencioso
    }

    openFile(callback = undefined, arg = undefined) {

        if (this.current_file_name === null) {
            
            callback({
                message: 'File name invalid',
                code: 11
            }, undefined);

            return;
        }

        this.readFileBuffer(this.current_file_name, eTYPE_CACHE.MACRO, (err, data) => {
            this.readFile(err, data, callback, arg);
        });
    }

    writeFile(callback) {

        let buffer = null;

        if (this.state) {

            const macro = {
                name: this.name,
                body: this.body,
                parameters: this.parameters
            }

            buffer = Buffer.from(JSON.stringify(macro));
        }

        try {

            // Save
            if (buffer !== undefined && buffer !== null && (buffer instanceof ArrayBuffer || buffer instanceof Buffer) && buffer.byteLength > 0)
                this.writeFileBuffer(this.current_file_name, Buffer.from(buffer), eTYPE_CACHE.MACRO, (err, id) => {

                    if (err) {
                        
                        callback(err, undefined);

                        return;
                    }

                    callback(null, id);
                });
            else
                callback({
                    message: 'Invalid buffer',
                    cose: 200
                }, undefined);

        }catch (err) {

            console.log("error save file[" + this.current_file_name + "]. Message: " + err.message);

            callback(err, undefined);
        }
    }

    readFile(err, data, callback = undefined, arg = undefined) {

        try {

            // Error
            if (err) throw err;

            if (data === undefined || data === null || !Buffer.isBuffer(data) || !(data.buffer instanceof ArrayBuffer))
                throw new Error("Invalid data returned from File System, readFile");

            const macro = JSON.parse(data.toString('utf-8'));

            if (!macro.hasOwnProperty('body') || !macro.hasOwnProperty('name') || !macro.hasOwnProperty('parameters'))
                throw new Error("Invalid macro Object");
            
            this.name = macro.name;
            this.body = macro.body;
            this.parameters = macro.parameters;

            this.state = true;

            // Call onOpenedFile
            this.onOpenedFile();

            // Call Back
            if (callback !== undefined && callback instanceof Function)
                callback(null, arg);
        
        }catch (error) {

            console.log("Error to open file. Message: " + error.message);

            callback(error, undefined);
        }
    }

    setFileName(file) {
        this.current_file_name = file;
    }

    getFileName() {
        return this.current_file_name;
    }

    static listFiles(uid, option, callback) {

        try {

            postRequestCalculadoraStadium('/macro.php', {
                token: TOKEN,
                type: 'list',
                uid: uid,
                option: option
            }, (data) => {

                try {

                    const reply = JSON.parse(data);

                    if (reply.hasOwnProperty('error') && reply.error != null) {

                        callback(reply.error, undefined);

                        return;
                    }

                    if (!reply.hasOwnProperty('list') || !(reply.list instanceof Object)) {

                        callback({
                            message: 'System error',
                            code: 30001
                        }, undefined);

                        return;
                    }

                    // Reply
                    callback(null, reply.list);

                }catch (err) {
                    
                    console.log(`Error: ${err}`);

                    callback({
                        message: '',
                        code: 30000
                    }, undefined);
                }
            });

        }catch (err) {

            console.log(`Error: ${err.message}`);

            throw new MyError(`Fail to list macros`);
        }
    }

    // Abstract method
    onOpenedFile() {
        throw "[MacroBase::onOpnenedFile] Function Abstract, extended class have implement this method";
    }
}

module.exports = MacroBase;