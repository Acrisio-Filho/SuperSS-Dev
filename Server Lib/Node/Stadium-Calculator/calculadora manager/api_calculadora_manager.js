// Arquivo api_calculadora_manager.js
// Criado em 25/07/2020 as 05:07 por Acrisio
// Definição da class CalculadoraManager

const cb = require('../common/CalculadoraBaseInject.js');
const MyError = require('../type/APIErrorException').APIErrorException;
const types = require('../type/type');

const LIMIT_SHOT_PER_CALCULADORA = 50;

// Calculadora Manager
class CalculadoraManager extends cb.CalculadoraBaseInject {

    constructor(base) {

        // Calculadora Base Inject
        super(base);
    }

    makeShot(shot) {

        if (this.shot.length >= LIMIT_SHOT_PER_CALCULADORA)
            throw new MyError("Error não pode criar mais que " + LIMIT_SHOT_PER_CALCULADORA + " tacadas", 5);

        if (!(shot instanceof Object) || !shot.hasOwnProperty('shot') || !shot.hasOwnProperty('club') 
                || !shot.hasOwnProperty('special') || !shot.hasOwnProperty('spin') || !shot.hasOwnProperty('power') || isNaN(shot.shot)
                || isNaN(shot.club) || isNaN(shot.special) || isNaN(shot.spin) || isNaN(shot.power))
            throw new Error("Invalid Shot Object");

        let new_tab = new types.Tabela();

        new_tab.block_type.push(new types.Shot(shot.shot, shot.club, shot.special, shot.spin, shot.power));

        new_tab.block_type.push(new cb.GenericTableFloatArray(types.eBLOCK_TYPE.TAB_ALTURA.NAME, 3, 1));
        new_tab.block_type.push(new cb.GenericTableFloatArray(types.eBLOCK_TYPE.TAB_HWI.NAME, 8, 1));

        if (this.addTab(new_tab) !== 0)
            throw new Error("Fail to add new Shot");
    }

    editShot(index, shot) {

        if (index === undefined || index === null || isNaN(index) || index === -1)
            throw new Error("Invalid Index Shot");

        if (!(shot instanceof Object) || !shot.hasOwnProperty('shot') || !shot.hasOwnProperty('club') 
                || !shot.hasOwnProperty('special') || !shot.hasOwnProperty('spin') || !shot.hasOwnProperty('power') || isNaN(shot.shot)
                || isNaN(shot.club) || isNaN(shot.special) || isNaN(shot.spin) || isNaN(shot.power))
            throw new Error("Invalid Shot Object");

        let tab = this.getTab(index);

        if (tab === null)
            throw new Error("Fail to edit Shot, index(" + index + ") shot not found");

        let block_tab = tab.block_type.find((el) => {
            return el.__type === types.eBLOCK_TYPE.SHOT.VALUE;
        });

        if (block_tab === undefined)
            throw new MyError("Invalid Tab Shot(don't have shot tab in tacada)");

        block_tab.shot = shot.shot;
        block_tab.club = shot.club;
        block_tab.special = shot.special;
        block_tab.spin = shot.spin;
        block_tab.power = shot.power;
    }

    copyShot(index) {

        if (this.shot.length >= LIMIT_SHOT_PER_CALCULADORA)
            throw new MyError("Error não pode criar mais que " + LIMIT_SHOT_PER_CALCULADORA + " tacadas", 5);

        if (index === undefined || index === null || isNaN(index) || index === -1)
            throw new Error("Invalid Index Shot");

        let tab = this.getTab(index);

        if (tab === null)
            throw new Error("Fail to copy Shot, index(" + index + ") shot not found");

        let new_tab = new types.Tabela();

        // Clone
        tab.block_type.forEach((el) => {
            new_tab.block_type.push(el.clone());
        });

        this.addTab(new_tab);
    }

    editShotTableValues(shot, table, table_size, values) {

        if (shot >= this.shot.length)
            throw new MyError("Invalid Shot[" + shot + "]");

        let block_tab = this.shot[shot].block_type.find((el) => {
            return el.__type === table;
        });

        if (block_tab == undefined)
            throw new MyError(`Invalid Block Tab[${table}] Shot(${shot})`);

        if (values.length != (table_size.line * table_size.column))
            throw new MyError("Invalid values length[" + values.length + "] shot[" + shot + "] table[" + table + "]");

        if (block_tab.column !== table_size.column)
            throw new MyError(`Invalid column length(${table_size.line}, ${table_size.column})`);

        if (block_tab.line === table_size.line) {

            for (let i = 0; i < table_size.line; i++)
                for (let j = 0; j < table_size.column; j++)
                    block_tab.map[j][i] = values[((i * table_size.column) + j)];
        
        }else {

            let def = types.getBlockTypeDefByType(table);

            if (def === undefined)
                throw new MyError(`Invalid Table Type(${table}) Shot(${shot})`);
            
            if (table_size.line < def.min_line)
                throw new MyError(`Invalid line length(${table_size.line}) Table Type(${table}) Shot(${shot})`);

            let new_block = new types.GenericTableFloatArray(types.getBlockTypeNameByType(table), table_size.column, table_size.line);

            for (let i = 0; i < table_size.line; i++)
                for (let j = 0; j < table_size.column; j++)
                    new_block.map[j][i] = values[((i * table_size.column) + j)];

            block_tab.map = new_block.map;
            block_tab.length = new_block.length;
            block_tab.__type = new_block.__type;
            block_tab.line = new_block.line;
            block_tab.column = new_block.column;
        }
    }

    makeCalcualdora(name, id, version, autor, power, callback) {

        if (CalculadoraManager.checkParameterString(name) || CalculadoraManager.checkParameterNumber(id) 
                || CalculadoraManager.checkParameterNumber(version) || CalculadoraManager.checkParameterString(autor) 
                || CalculadoraManager.checkParameterString(power)) {
            
            callback({
                message: 'Invalid parameters',
                code: 30000
            }, undefined);

            return;
        }

        this.init_header_raw(id, version, autor, power);

        if (!this.state) {
            
            callback({
                message: 'Fail to init head',
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
                callback(null, id);
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

    static getShotName(shot) {
        return `${cb.Shot.getNameTypeByType(shot.shot, cb.SHOT_TYPE, false)} - ${cb.Shot.getNameTypeByType(shot.club, cb.CLUB_TYPE, false)} (${shot.power.toFixed(0)}y) - ${cb.Shot.getNameTypeByType(shot.special, cb.SPECIAL_SHOT_TYPE, false)}` + (shot.spin != 0 ? " - " + shot.spin + " Spin" : "")
    }

    // Sobrecarga
    onOpenedFile() {
        // Faz nada aqui
    }
}

module.exports = {
    CalculadoraManager: CalculadoraManager
}