// Arquivo CalculadoraBaseInject.js
// Criado em 26/07/2020 as 05:53 por Acrisio
// Definição da classe CachaObject

const { CalculadoraBase } = require('./calculadora_base.js');
const CacheObjectBaseInject = require('./CacheObjectBaseInject')

class CalculadoraBaseInject extends CacheObjectBaseInject {

    constructor(base = undefined) {

        if (base)
            super(base);
        else
            super(new CalculadoraBase());

        // Override
        this.base.onOpenedFile = ((new_this) => {
            return function() {
                return new_this.onOpenedFile();
            };
        })(this);
    }

    // Gettters and Setters
    get head() {
        return this.base.head;
    }

    set head(head) {
        this.base.head = head;
    }

    get shot() {
        return this.base.shot;
    }

    set shot(shot) {
        this.base.shot = shot;
    }

    // Methods
    clear_head() {
        return this.base.clear_head();
    }

    clear_tabs() {
        return this.base.clear_tabs();
    }

    isOld() {
        return this.base.isOld();
    }

    init_header_raw(id, version, nome_autor, power) {
        return this.base.init_header_raw(id, version, nome_autor, power);
    }

    init_header(head) {
        return this.base.init_header(head);
    }

    writeFileNew(callback) {
        return this.base.writeFileNew(callback);
    }

    addTab(tab) {
        return this.base.addTab(tab);
    }

    deleteTab(index) {
        return this.base.deleteTab(index);
    }

    getTab(index) {
        return this.base.getTab(index);
    }

    setTab(tab, index) {
        return this.base.setTab(tab, index);
    }

    static listFiles(uid, option, callback) {
        CalculadoraBase.listFiles(uid, option, callback);
    }

    // Abstract method - New Override
    onOpenedFile() {
        throw "[CalculadoraBaseInject::onOpnenedFile] Function Abstract, extended class have implement this method";
    }
}

const types = require('../type/type');
const utils = require('../type/utils')

module.exports = {
    Head: types.Head,
    CalculadoraBase: CalculadoraBase,
    Tabela: types.Tabela,
    Shot: types.Shot,
    GenericTableFloatArray: types.GenericTableFloatArray,
    SHOT_TYPE: utils.SHOT_TYPE,
    CLUB_TYPE: utils.CLUB_TYPE,
    SPECIAL_SHOT_TYPE: utils.SPECIAL_SHOT_TYPE,
    CalculadoraBaseInject: CalculadoraBaseInject
}