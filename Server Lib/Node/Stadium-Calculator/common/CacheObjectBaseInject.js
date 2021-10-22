// Arquivo CacheObjectBaseInject.js
// Criado em 29/11/2020 as 11:42 por Acrisio
// Definição e Implementação da classe CacheObjectBaseInject

const CacheObjectBase = require('./CacheObject_base');

class CacheObjectBaseInject {

    base = undefined;

    constructor(base = undefined) {

        if (base)
            this.base = base;
        else
            this.base = new CacheObjectBase;

        // Override
        this.base.onOpenedFile = ((new_this) => {
            return function() {
                return new_this.onOpenedFile();
            };
        })(this);
    }

    // Cache Object - Getters and Setters
    get cache() {
        return this.base.cache;
    }

    set cache(cache) {
        return this.base.cache = cache;
    }

    // Cache Object - Methods
    readCacheFile(file_name) {
        return this.base.readCacheFile(file_name);
    }

    // Gettters and Setters
    get current_file_name() {
        return this.base.current_file_name;
    }

    set current_file_name(current_file_name) {
        this.base.current_file_name = current_file_name;
    }

    get state() {
        return this.base.state;
    }

    set state(state) {
        this.base.state = state;
    }

    get id() {
        return this.base.id;
    }

    set id(id) {
        this.base.id = id;
    }

    get uid() {
        return this.base.uid;
    }

    set uid(uid) {
        this.base.uid = uid;
    }

    get owner_uid() {
        return this.base.owner_uid;
    }

    set owner_uid(owner_uid) {
        this.base.owner_uid = owner_uid;
    }

    get permission() {
        return this.base.permission;
    }

    set permission(permission) {
        this.base.permission = permission;
    }

    // Methods

    // Salva as modificações e e fecha
    close(callback) {
        return this.base.close(callback);
    }

    openFile(callback = undefined, arg = undefined) {
        return this.base.openFile(callback, arg);
    }

    writeFile(callback) {
        return this.base.writeFile(callback);
    }

    readFile(err, data, callback = undefined, arg = undefined) {
        return this.base.readFile(err, data, callback, arg);
    }

    setFileName(file) {
        return this.base.setFileName(file);
    }

    getFileName() {
        return this.base.getFileName();
    }

    static listFiles(uid, option, callback) {
        this.base.prototype.listFiles(uid, option, callback);
    }

    // Abstract method - New Override
    onOpenedFile() {
        throw "[CacheObjectBaseInject::onOpnenedFile] Function Abstract, extended class have implement this method";
    }
}

module.exports = CacheObjectBaseInject;