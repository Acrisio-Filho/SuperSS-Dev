// Arquivo MacroBaseInject.js
// Criado em 29/11/2020 as 11:40 por Acrisio
// Definição e Implementação da classe MacroBaseInject

const CacheObjectBaseInject = require('./CacheObjectBaseInject')
const MacroBase = require('./macro_base');

class MacroBaseInject extends CacheObjectBaseInject {

    constructor(base = undefined) {

        if (base)
            super(base);
        else
            super(new MacroBase());

        // Override
        this.base.onOpenedFile = ((new_this) => {
            return function() {
                return new_this.onOpenedFile();
            };
        })(this);
    }

    // Gettters and Setters
    get name() {
        return this.base.name;
    }

    set name(name) {
        this.base.name = name;
    }

    get body() {
        return this.base.body;
    }

    set body(body) {
        this.base.body = body;
    }

    get parameters() {
        return this.base.parameters;
    }

    set parameters(parameters) {
        this.base.parameters = parameters;
    }

    // Methods

    static listFiles(uid, option, callback) {
        MacroBase.listFiles(uid, option, callback);
    }

    // Abstract method - New Override
    onOpenedFile() {
        throw "[MacroBaseInject::onOpnenedFile] Function Abstract, extended class have implement this method";
    }
}

module.exports = MacroBaseInject;