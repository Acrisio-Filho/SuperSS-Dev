// Arquivo CacheObject_base.js
// Criado em 29/11/2020 as 11:15 por Acrisio
// Definição e Implementação da classe CacheObjectBase

const { CacheObject } = require('./CacheObject');
const MyError = require('../type/APIErrorException').APIErrorException;

class CacheObjectBase extends CacheObject {

    state = false;
    current_file_name = null;

    constructor() {

        // CacheObject
        super();
    }

    onOpenedFile() {
        throw MyError("method onOpenedFile not implemented");
    }

    openFile(callback = undefined, arg = undefined) {
        throw MyError("method openFile not implemented");
    }

    close(callback) {
        throw MyError("method close not implemented");
    }

    writeFile(callback) {
        throw MyError("method writeFile not implemented");
    }

    readFile(err, data, callback = undefined, arg = undefined) {
        throw MyError("method readFile not implemented");
    }

    getFileName() {
        throw MyError("method getFileName not implemented");
    }

    setFileName(name) {
        throw MyError("method setFileName not implemented");
    }

    static listFiles(uid, option, callback) {
        throw MyError("method listFiles not implemented");
    }

}

module.exports = CacheObjectBase;