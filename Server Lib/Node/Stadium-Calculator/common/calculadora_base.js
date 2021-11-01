const co = require('./CacheObject.js');
const CacheObjectBase = require('./CacheObject_base');
const types = require('../type/type');
const MyError = require('../type/APIErrorException').APIErrorException;
const { postRequestCalculadoraStadium } = require('../util/util');
const { TOKEN } = require('./token.js');

// CalculadoraBase
class CalculadoraBase extends CacheObjectBase {

    constructor() {

        // Cache Object Base
        super();

        this.head = new types.Head();
        this.shot = [];             // Tabela das Tacadas

        this.current_file_name = null;

        this.state = false;
    }

    clear_head() {
        this.head = new types.Head();
    }

    clear_tabs() {

        if (this.shot.length > 0)
            this.shot = [];
    }

    isOld() {
        return this.head.id == 2 && this.head.version == 105;
    }

    // Salva as modificações e e fecha
    close(callback) {

        if (this.state) {

            this.writeFile((err, id) => {

                if (err) {

                    callback(err);

                    return;
                }

                this.clear_head();
                this.clear_tabs();

                this.current_file_name = null;

                this.state = false;

                // Reply
                callback(null);
            });

        }else
            callback(null); // Close silencioso
    }

    init_header_raw(id, version, nome_autor, power) {

        this.clear_head();

        this.head.id = id;
        this.head.version = version;
        this.head.nome_autor = nome_autor;
        this.head.power = power;

        this.state = true;
    }

    init_header(head) {

        this.clear_head();

        this.head = head;

        this.state = false;
    }

    openFile(callback = undefined, arg = undefined) {

        if (this.current_file_name === null) {
            
            callback({
                message: 'File name invalid',
                code: 11
            }, undefined);

            return;
        }

        this.readFileBuffer(this.current_file_name, co.eTYPE_CACHE.CALCULADORA, (err, data) => {
            this.readFile(err, data, callback, arg);
        });
    }

    writeFile(callback) {

        let buffer = null;

        if (this.state) {
            
            if (this.head.id == 2 && this.head.version == 105) { // Old

                if (this.shot.length == 0) {

                    // Apenas o head
                    buffer = new ArrayBuffer(types.Head.deserialize_size());

                    let view = new DataView(buffer);

                    // Update Elements number
                    this.head.count_shot_entry = this.shot.length;

                    this.head.serialize(view);
                    
                }else {

                    buffer = new ArrayBuffer(types.Head.deserialize_size() + (this.shot.length * this.shot[0].deserialize_size_old()));

                    let view = new DataView(buffer);

                    // Update Elements number
                    this.head.count_shot_entry = this.shot.length;

                    this.head.serialize(view);

                    let size = types.Head.deserialize_size();
                    
                    this.shot.forEach((el) => {
                        size += el.serializeOld(new DataView(view.buffer, size + view.byteOffset, el.deserialize_size_old()));
                    });
                }
            
            }else if (this.head.id == 3 && this.head.version == 201) { // New

                if (this.shot.length == 0) {

                    // Apenas o head
                    buffer = new ArrayBuffer(types.Head.deserialize_size());

                    let view = new DataView(buffer);

                    // Update Elements number
                    this.head.count_shot_entry = this.shot.length;

                    this.head.serialize(view);

                }else {

                    buffer = new Buffer.alloc(types.Head.deserialize_size());

                    let view = new DataView(buffer.buffer, buffer.byteOffset, buffer.length);

                    // Update Elements number
                    this.head.count_shot_entry = this.shot.length;

                    this.head.serialize(view);

                    //let size = types.Head.deserialize_size();

                    this.shot.forEach((el) => {
                        buffer = Buffer.concat([buffer, el.serializeToBuffer()]);
                    });
                }
            }
        }

        try {

            // Save
            if (buffer !== undefined && buffer !== null && (buffer instanceof ArrayBuffer || buffer instanceof Buffer) && buffer.byteLength > 0)
                this.writeFileBuffer(this.current_file_name, Buffer.from(buffer), co.eTYPE_CACHE.CALCULADORA, (err, id) => {

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

    writeFileNew(callback) {

        if (this.state /*&&  this.shot.length > 0 */) {

            // Converte para a nova versão
            if (this.head.id == 2 && this.head.version == 105) {

                this.head.id = 3;
                this.head.version = 201;
            }

            // Save
            this.writeFile(callback);
        }else
            callback({
                message: 'Invalid state calculator',
                code: 201
            }, undefined);
    }

    readFile(err, data, callback = undefined, arg = undefined) {

        try {

            // Error
            if (err) throw err;

            if (data === undefined || data === null || !Buffer.isBuffer(data) || !(data.buffer instanceof ArrayBuffer))
                throw new Error("Invalid data returned from File System, readFile");

            let view = new DataView(data.buffer, data.byteOffset, data.byteLength);

            this.head.deserialize(view);

            let index = types.Head.deserialize_size();
            let tmp = undefined;

            if (this.head.id == 2 && this.head.version == 105)  {   // Old

                this.shot = [];

                for (let i = 0; i < this.head.count_shot_entry; i++) {

                    // Initialize new table
                    tmp = new types.Tabela();
                    tmp.deserializeOld(new DataView(view.buffer, index + view.byteOffset, tmp.deserialize_size_old()));       index += tmp.deserialize_size_old();

                    this.shot.push(tmp);
                }

                this.state = true;
            
            }else if (this.head.id == 3 && this.head.version == 201) { // New

                this.shot = [];

                for (let i = 0; i < this.head.count_shot_entry; i++) {

                    // Initialize new table
                    tmp = new types.Tabela();
                    tmp.deserialize(new DataView(view.buffer, index + view.byteOffset, tmp.deserialize_size()));       index += tmp.deserialize_size();

                    this.shot.push(tmp);
                }

                this.state = true;

            }else {

                // Log
                console.log("Error: Version(" + this.head.version + ") and id(" + this.head.id + ") is invalid.");

                this.close((err) => {

                    if (err)
                        console.log(`Fail to close Calculator`);

                    callback({
                        message: 'Calculator version invalid',
                        code: 350
                    }, undefined);
                });

                return;
            }

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

    addTab(tab) {
        
        if (!(tab instanceof types.Tabela))
            return -3;

        this.shot.push(tab);

        return 0;
    }

    deleteTab(index) {

        if (isNaN(index))
            return -2;

        if (index < 0 || index > this.shot.length)
            return -1;

        this.shot.splice(index, 1);

        return 0;
    }

    getTab(index) {

        if (isNaN(index))
            return null;

        if (index < 0 || index > this.shot.length)
            return null;

        return this.shot[index];
    }

    setTab(tab, index) {

        if (!(tab instanceof types.Tabela))
            return -3;

        if (isNaN(index))
            return -2;

        if (index < 0 || index > this.shot.length)
            return -1;

        this.shot[index] = tab;

        return 0;
    }

    static listFiles(uid, option, callback) {

        try {

            postRequestCalculadoraStadium('/calculadora.php', {
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

            throw new MyError(`Fail to list calculators`);
        }
    }

    // Abstract method
    onOpenedFile() {
        throw "[CalculadoraBase::onOpnenedFile] Function Abstract, extended class have implement this method";
    }
}

exports.CalculadoraBase = CalculadoraBase;