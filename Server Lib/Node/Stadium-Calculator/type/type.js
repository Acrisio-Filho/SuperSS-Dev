const utils = require('./utils');
const MyError = require('./APIErrorException').APIErrorException;

const eBLOCK_TYPE = {
    SHOT: {
        NAME: 'SHOT',
        VALUE: 0,
        DEF: {}
    },
    TAB_HWI: {
        NAME: 'TAB_HWI',
        VALUE: 1,
        DEF: {
            min_line: 1,
            min_column: 8,
            max_line: 30,
            max_column: 10
        }
    },
    TAB_ALTURA: {
        NAME: 'TAB_ALTURA',
        VALUE: 2,
        DEF: {
            min_line: 1,
            min_column: 3,
            max_line: 30,
            max_column: 4
        }
    },
    TAB_TERRENO: {
        NAME: 'TAB_TERRENO',
        VALUE: 3,
        DEF: {
            min_line: 1,
            min_column: 2,
            max_line: 30,
            max_column: 2
        }
    },
    TAB_HEIGHT: {
        NAME: 'TAB_HEIGHT',
        VALUE: 4,
        DEF: {
            min_line: 1,
            min_column: 2,
            max_line: 30,
            max_column: 4
        }
    }
}

exports.eBLOCK_TYPE = eBLOCK_TYPE;

function getBlockTypeNameByType(type) {

    let el = eBLOCK_TYPE.find((el) => {
        return el !== undefined && el !== null && el instanceof Object && el.hasOwnProperty('VALUE') && el.VALUE !== undefined && el.VALUE !== null && !isNaN(el.VALUE) && el.VALUE === type;
    });

    return el !== undefined && el.hasOwnProperty('NAME') && el.NAME !== undefined && el.NAME !== null && el.NAME !== '' ? el.NAME : undefined;
}

exports.getBlockTypeNameByType = getBlockTypeNameByType;

function getBlockTypeDefByType(type) {

    let el = eBLOCK_TYPE.find((el) => {
        return el !== undefined && el !== null && el instanceof Object && el.hasOwnProperty('VALUE') && el.VALUE !== undefined && el.VALUE !== null && !isNaN(el.VALUE) && el.VALUE === type;
    });

    return el !== undefined && el.hasOwnProperty('DEF') && el.DEF !== undefined && el.DEF !== null && el.DEF instanceof Object ? el.DEF : undefined;
}

exports.getBlockTypeDefByType = getBlockTypeDefByType;

function isBlockTypeTab(value) {

    return eBLOCK_TYPE.filter((el) => {
        return el !== undefined && el !== null && el instanceof Object && el.hasOwnProperty('NAME') && el.NAME !== undefined && el.NAME !== null && el.NAME !== '' && el.NAME.includes('TAB');
    }).findIndex((el) => {
        return el !== undefined && el !== null && el instanceof Object && el.hasOwnProperty('VALUE') && el.VALUE !== undefined && el.VALUE !== null && !isNaN(el.VALUE) && el.VALUE === value;
    }) !== -1;
}

// Block Type Header
class BlockTypeHeader {

    constructor(type, size) {

        if (!eBLOCK_TYPE.hasOwnProperty(type))
            throw new MyError(this.makeDebugLine("Invalid Block Type"));

        if (size > Number.MAX_UINT32_VALUE)
            throw new MyError(this.makeDebugLine("Invalid Size Block"));

        this.__type = eBLOCK_TYPE[type].VALUE;
        this.__size = size;

        this.state = true;
    }


    _serializeToBuffer() {

        let buff = Buffer.alloc(BlockTypeHeader.fixed_size());

        this._serialize(new DataView(buff.buffer, buff.byteOffset, buff.length));

        return buff;
    }

    _serialize(view) {

        if (!this.state)
            throw new MyError(this.makeDebugLine("Instance is invalid"));

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < BlockTypeHeader.fixed_size())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let size = 0;

        // Serialize
        view.setUint8(size, this.__type);             size += Uint8Array.BYTES_PER_ELEMENT;
        view.setUint32(size, this.__size, true);      size += Uint32Array.BYTES_PER_ELEMENT;

        return size;
    }

    _deserialize(view) {

        if (!this.state)
            throw new MyError(this.makeDebugLine("Instance is invalid"));

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < BlockTypeHeader.fixed_size())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));
        
        let index = 0;

        this.__type = view.getUint8(index);           index += Uint8Array.BYTES_PER_ELEMENT;
        this.__size = view.getUint32(index, true);    index += Uint32Array.BYTES_PER_ELEMENT;
    }

    static fixed_size() {
        return Uint8Array.BYTES_PER_ELEMENT + Uint32Array.BYTES_PER_ELEMENT;
    }
}

exports.BlockTypeHeader = BlockTypeHeader;

// Head
class Head {

    constructor() {
        this.count_shot_entry = 0;
        this.version = 0;
        this.id = 0;
        this.nome_autor = "Acrisio SuperSS Dev";
        this.power = "270+6y";
    }

    serializeToBuffer() {

        let buff = Buffer.alloc(Head.deserialize_size());

        this.serialize(new DataView(buff.buffer, buff.byteOffset, buff.length));

        return buff;
    }

    serialize(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < Head.deserialize_size())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let size = 0;

        view.setUint32(size, this.count_shot_entry, true);  size += Uint32Array.BYTES_PER_ELEMENT;
        view.setUint16(size, this.version, true);           size += Uint16Array.BYTES_PER_ELEMENT;
        view.setUint16(size, this.id, true);                size += Uint16Array.BYTES_PER_ELEMENT;
        view.setString(size, this.nome_autor);              size += (Uint8Array.BYTES_PER_ELEMENT * 45);
        view.setString(size, this.power);                   size += (Uint8Array.BYTES_PER_ELEMENT * 45); 
        
        return size;
    }

    deserialize(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < Head.deserialize_size())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let index = 0;

        this.count_shot_entry = view.getUint32(index, true);    index += Uint32Array.BYTES_PER_ELEMENT;
        this.version = view.getUint16(index, true);             index += Uint16Array.BYTES_PER_ELEMENT;
        this.id = view.getUint16(index, true);                  index += Uint16Array.BYTES_PER_ELEMENT;
        this.nome_autor = view.getString(index, 45);            index += (Uint8Array.BYTES_PER_ELEMENT * 45);
        this.power = view.getString(index, 45);                 index += (Uint8Array.BYTES_PER_ELEMENT * 45);
    }

    static deserialize_size() {
        return (Uint32Array.BYTES_PER_ELEMENT/*Count*/ + (Uint16Array.BYTES_PER_ELEMENT/*Version, ID*/ * 2) + (Uint8Array.BYTES_PER_ELEMENT * 45/*Nome, Power*/ * 2));
    }
}

exports.Head = Head;

// Shot
class Shot extends BlockTypeHeader {

    constructor(shot = utils.SHOT_TYPE.COBRA.TYPE, club = utils.CLUB_TYPE._1W.TYPE, special = utils.SPECIAL_SHOT_TYPE._NO_POWER_SHOT.TYPE, spin = 0, power = 0) {

        super(eBLOCK_TYPE.SHOT.NAME, 0);

        this.shot = shot;
        this.club = club;
        this.special = special;
        this.spin = spin;
        this.power = power;
    }

    serializeToBuffer() {

        let buff = Buffer.alloc(Shot.deserialize_size());

        this.serialize(new DataView(buff.buffer, buff.byteOffset, buff.length));

        return buff;
    }

    clone() {

        let new_instance = new Shot(this.shot, this.club, this.special, this.spin, this.power);

        new_instance.__size = this.__size;

        return new_instance;
    }

    serialize(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < Shot.deserialize_size())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let size = 0;

        this.__size = Shot._deserialize_size();
        
        // BlockTypeHeader
        size += this._serialize(view);

        view.setUint8(size, this.shot);             size += Uint8Array.BYTES_PER_ELEMENT;
        view.setUint8(size, this.club);             size += Uint8Array.BYTES_PER_ELEMENT;
        view.setUint8(size, this.special);          size += Uint8Array.BYTES_PER_ELEMENT;
        view.setUint8(size, this.spin);             size += Uint8Array.BYTES_PER_ELEMENT;
        view.setUint16(size, this.power, true);     size += Uint16Array.BYTES_PER_ELEMENT;

        return size;
    }

    deserialize(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < Shot.deserialize_size())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let index = 0;

        this._deserialize(view);                index += BlockTypeHeader.fixed_size();

        if (this.__type !== eBLOCK_TYPE.SHOT.VALUE)
            throw new MyError(this.makeDebugLine(`Invalid Block Type(${this.__type})`));

        this.shot = view.getUint8(index);           index += Uint8Array.BYTES_PER_ELEMENT;
        this.club = view.getUint8(index);           index += Uint8Array.BYTES_PER_ELEMENT;
        this.special = view.getUint8(index);        index += Uint8Array.BYTES_PER_ELEMENT;
        this.spin = view.getUint8(index);           index += Uint8Array.BYTES_PER_ELEMENT;
        this.power = view.getUint16(index, true);   index += Uint16Array.BYTES_PER_ELEMENT;
    }

    // Suport para versões antigas
    serializeOld(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < Shot.deserialize_size_old())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let size = 0;

        let name = "";
        let tmp = null;

        // Shot
        if ((tmp = Shot.getNameTypeByType(this.shot, utils.SHOT_TYPE)) !== null)
            name += tmp;

        // Club
        if ((tmp = Shot.getNameTypeByType(this.club, utils.CLUB_TYPE)) !== null)
            name += tmp;

        // Special
        if ((tmp = Shot.getNameTypeByType(this.special, utils.SPECIAL_SHOT_TYPE)) !== null)
            name += tmp;

        // Spin
        if (this.spin == 30)
            name += "30s";

        view.setString(size, name);     size += Shot.deserialize_size_old();

        return size;
    }

    // Suporte para versões antigas
    deserializeOld(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < Shot.deserialize_size_old())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let index = 0;

        let shot_tmp = view.getString(index, (Uint8Array.BYTES_PER_ELEMENT * 45));    index += (Uint8Array.BYTES_PER_ELEMENT * 45);

        if (shot_tmp == null)
            return;

        // aqui desmembra e compara
        let matchs = shot_tmp.match(/([^\d]+)(\d\w)(.*)(\d{2}[\w ]*)|([^\d]+)(\d\w)(.*)/i);
        
        if (!(matchs instanceof Array) || matchs.length <= 1)
            return;

        // Remova first element is global match
        matchs.shift();

        let tmp = undefined;

        matchs.forEach((el) => {

            if (el !== null && el !== undefined) {
                
                // Shot
                if ((tmp = Shot.getTypeFromStringName(el, utils.SHOT_TYPE)) !== -1)
                    this.shot = tmp;

                // Club
                if ((tmp = Shot.getTypeFromStringName(el, utils.CLUB_TYPE)) !== -1)
                    this.club = tmp

                // Special
                if ((tmp = Shot.getTypeFromStringName(el, utils.SPECIAL_SHOT_TYPE)) !== -1)
                    this.special = tmp;

                // Spin
                tmp = el.match(/(\d+)(s)/i);

                if (tmp instanceof Array && tmp.length == 3 && !isNaN(tmp[1]))
                    this.spin = parseInt(tmp[1]);
            }
        });
    }

    // Support Util getType From String Name
    static getTypeFromStringName(stack, type_class) {

        if (!(type_class instanceof Object) || stack === '')
            return -1;

        let obj = null;
        
        if ((obj = utils.find_if(type_class, (el) => {
            return stack.includes(el.NAME) || (el.hasOwnProperty("NAME2") && stack.includes(el.NAME2));
        })) !== null)
            return obj.TYPE;

        return -1;
    }

    // Support Util get Name Type By Type
    static getNameTypeByType(type, type_class, old = true) {

        if (!(type_class instanceof Object) || isNaN(type))
            return null;

        let obj = null;

        if ((obj = utils.find_if(type_class, (el) => {
            return el.TYPE == type;
        })) !== null) {

            if (old && obj.hasOwnProperty("NAME2"))
                return obj.NAME2;

            return obj.NAME;
        }

        return null;
    }

    static _deserialize_size() {
        return ((Uint8Array.BYTES_PER_ELEMENT * 4) + Uint16Array.BYTES_PER_ELEMENT);
    }

    static deserialize_size() {
        return Shot._deserialize_size() + BlockTypeHeader.fixed_size();
    }

    static deserialize_size_old() {
        return (Uint8Array.BYTES_PER_ELEMENT * 45);
    }
}

exports.Shot = Shot;

// Generic Table Float Array
class GenericTableFloatArray extends BlockTypeHeader {

    constructor(type, column, line) {

        super(type, 0);

        if (isNaN(column) || isNaN(line))
            throw new MyError(this.makeDebugLine("Invalid column or line parameter"));

        if (column <= 0 || line <= 0)
            throw new MyError(this.makeDebugLine("Invalid column or line parameter"));

        let def = getBlockTypeDefByType(this.__type);

        if (def === undefined)
            throw new MyError(this.makeDebugLine(`Invalid type(${this.__type})`));

        if (line < def.min_line || line > def.max_line)
            throw new MyError(this.makeDebugLine(`Invalid Line length(${line}) to Type(${this.__type})`));

        if (column < def.min_column || column > def.max_column)
            throw new MyError(this.makeDebugLine(`Invalid Column length(${column}) to Type(${this.__type})`));

        this.column = column;
        this.line = line;

        // Set Length
        this.length = column * (Float32Array.BYTES_PER_ELEMENT * line);

        this.map = [];

        // alloc memory
        for (let i = 0; i < column; i++)
            this.map.push(new Float32Array(new ArrayBuffer(Float32Array.BYTES_PER_ELEMENT * line)));
    }

    clone() {

        let new_instance = new GenericTableFloatArray(getBlockTypeNameByType(this.__type), this.column, this.line);

        new_instance.__size = this.__size;

        for (let j = 0; j < this.column; j++)
            for (let i = 0; i < this.line; i++)
                new_instance.map[j][i] = this.map[j][i];

        return new_instance;
    }

    serializeToBuffer() {

        let buff = Buffer.alloc(this.deserialize_size());

        this.serialize(new DataView(buff.buffer, buff.byteOffset, buff.length));

        return buff;
    }

    serialize(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < this.length)
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let size = 0;

        this.__size = this._deserialize_size();
        
        size += this._serialize(view);

        view.setUint32(size, this.line, true);        size += Uint32Array.BYTES_PER_ELEMENT;
        view.setUint32(size, this.column, true);      size += Uint32Array.BYTES_PER_ELEMENT;

        this.map.forEach((el) => {

            // Set
            (new Uint8Array(view.buffer, size + view.byteOffset, el.length * el.BYTES_PER_ELEMENT)).set(new Uint8Array(el.buffer));

            // Add Size
            size += el.length * el.BYTES_PER_ELEMENT;
        });

        return size;
    }

    deserialize(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < this.length)
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let index = 0;

        this._deserialize(view);        index += BlockTypeHeader.fixed_size();

        if (!isBlockTypeTab(this.__type))
            throw new MyError(this.makeDebugLine(`Invalid Block Type(${this.__type})`));

        let def = getBlockTypeDefByType(this.__type);

        if (def === undefined)
            throw new MyError(this.makeDebugLine(`Invalid Block Type(${this.__type})`));

        this.line = view.getUint32(index, true);        index += Uint32Array.BYTES_PER_ELEMENT;
        this.column = view.getUint32(index, true);      index += Uint32Array.BYTES_PER_ELEMENT;

        if (this.line < def.min_line || this.line > def.max_line)
            throw new MyError(this.makeDebugLine(`Invalid Line length(${this.line}) Block Type(${this.__type})`));

        if (this.column < def.min_column || this.column > def.max_column)
            throw new MyError(this.makeDebugLine(`Invalid Column length(${this.column}) Block Type(${this.__type})`));

        // Set Length
        this.length = this.column * (Float32Array.BYTES_PER_ELEMENT * this.line);

        this.map = [];

        // alloc memory
        for (let i = 0; i < this.column; i++)
            this.map.push(new Float32Array(new ArrayBuffer(Float32Array.BYTES_PER_ELEMENT * this.line)));

        this.map.forEach((el) => {

            // Set
            (new Uint8Array(el.buffer)).set(new Uint8Array(view.buffer, index + view.byteOffset, el.length * el.BYTES_PER_ELEMENT));

            // Add index
            index += el.length * el.BYTES_PER_ELEMENT;
        });
    }

    serializeOld(column, line, view) {

        if (isNaN(column) || isNaN(line))
            throw new MyError(this.makeDebugLine("Invalid column or line parameter"));

        if (this.length < (column * (Float32Array.BYTES_PER_ELEMENT * line)))
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.length})`));

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < (column * (Float32Array.BYTES_PER_ELEMENT * line)))
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.length})`));

        let size = 0;

        for (let i = 0; i < column && i < this.map.length; i++) {

            // Set
            (new Uint8Array(view.buffer, size + view.byteOffset, line * Float32Array.BYTES_PER_ELEMENT)).set(new Uint8Array(this.map[i].buffer));

            // Add Size
            size += line * Float32Array.BYTES_PER_ELEMENT;
        }

        return size;
    }

    deserializeOld(column, line, view) {

        if (isNaN(column) || isNaN(line))
            throw new MyError(this.makeDebugLine("Invalid column or line parameter"));

        if (this.length < (column * (Float32Array.BYTES_PER_ELEMENT * line)))
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.length})`));

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < (column * (Float32Array.BYTES_PER_ELEMENT * line)))
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.length})`));

        let index = 0;

        for (let i = 0; i < column && i < this.map.length; i++) {

            // Set
            (new Uint8Array(this.map[i].buffer)).set(new Uint8Array(view.buffer, index + view.byteOffset, line * Float32Array.BYTES_PER_ELEMENT));

            // Add index
            index += line * Float32Array.BYTES_PER_ELEMENT;
        }
    }

    _deserialize_size() {
        return this.length;
    }

    deserialize_size() {
        return this._deserialize_size() + BlockTypeHeader.fixed_size() + (2 * Uint32Array.BYTES_PER_ELEMENT);
    }

    deserialize_size_old(column, line) {

        if (isNaN(column) || isNaN(line))
            throw new MyError(this.makeDebugLine("Invalid column or line parameter"));

        return (column * (Float32Array.BYTES_PER_ELEMENT * line));
    }
}

exports.GenericTableFloatArray = GenericTableFloatArray;

// Tabela
class Tabela {
    
    constructor() {
        this.block_type = [];
    }

    getAllTabType(exclude = undefined) {

        return this.block_type.map((el) => {
            return el.__type;
        }).filter((el) => {
            return exclude === undefined || el !== exclude;
        });
    }

    serializeToBuffer() {

        let buff = Buffer.alloc(this.deserialize_size());

        this.serialize(new DataView(buff.buffer, buff.byteOffset, buff.length));

        return buff;
    }

    serialize(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < this.deserialize_size())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let size = 0;

        view.setUint32(size, this.block_type.length, true);     size += Uint32Array.BYTES_PER_ELEMENT;

        this.block_type.forEach((el) => {
            size += el.serialize(new DataView(view.buffer, size + view.byteOffset, (el.deserialize_size) ? el.deserialize_size() : el.constructor.deserialize_size()));
        });

        return size;
    }

    deserialize(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < this.deserialize_size())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let index = 0;

        let length = view.getUint32(index, true);       index += Uint32Array.BYTES_PER_ELEMENT;

        let tmp_block_type_head = new BlockTypeHeader(eBLOCK_TYPE.SHOT.NAME, 0);

        for (let i = 0; i < length; i++) {

            tmp_block_type_head._deserialize(new DataView(view.buffer, index + view.byteOffset, BlockTypeHeader.fixed_size()));

            if (tmp_block_type_head.__type === eBLOCK_TYPE.SHOT.VALUE) {

                let block = new Shot();

                block.deserialize(new DataView(view.buffer, index + view.byteOffset, Shot.deserialize_size()));     index += Shot.deserialize_size();

                this.block_type.push(block);

            }else if (isBlockTypeTab(tmp_block_type_head.__type)) {

                let line = (new DataView(view.buffer, index + view.byteOffset + BlockTypeHeader.fixed_size(), 4)).getUint32(0, true);
                let column = (new DataView(view.buffer, index + view.byteOffset + BlockTypeHeader.fixed_size() + 4, 4)).getUint32(0, true);

                let block = new GenericTableFloatArray(getBlockTypeNameByType(tmp_block_type_head.__type), column, line);

                block.deserialize(new DataView(view.buffer, index + view.byteOffset, block.deserialize_size()));    index += block.deserialize_size();

                this.block_type.push(block);

            }else {

                // Log
                console.log(`Invalid Block Type(${tmp_block_type_head.__type}) pass ${tmp_block_type_head.__size} of bytes`);

                index += tmp_block_type_head.__size;
            }
        }
    }

    serializeOld(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < this.deserialize_size_old())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let size = 0;

        size += this.block_type[0].serializeOld(new DataView(view.buffer, size + view.byteOffset, Shot.deserialize_size_old()));
        size += this.block_type[1].serializeOld(2, 13, new DataView(view.buffer, size + view.byteOffset, this.block_type[1].deserialize_size_old(2, 13)));
        size += this.block_type[2].serializeOld(4, 19, new DataView(view.buffer, size + view.byteOffset, this.block_type[2].deserialize_size_old(4, 19)));
        size += this.block_type[3].serializeOld(10, 22, new DataView(view.buffer, size + view.byteOffset, this.block_type[3].deserialize_size_old(10, 22)));
        size += this.block_type[4].serializeOld(2, 19, new DataView(view.buffer, size + view.byteOffset, this.block_type[4].deserialize_size_old(2, 19)));

        return size;
    }

    deserializeOld(view) {

        if (!(view instanceof DataView))
            throw new MyError(this.makeDebugLine("Invalid Dataview parameter"));

        if (view.byteLength < this.deserialize_size_old())
            throw new MyError(this.makeDebugLine(`Invalid Dataview length(${view.byteLength})`));

        let index = 0;

        let shot = new Shot();

        shot.deserializeOld(new DataView(view.buffer, index + view.byteOffset, Shot.deserialize_size_old()));                              index += Shot.deserialize_size_old();

        this.block_type.push(shot);

        let block = new GenericTableFloatArray(eBLOCK_TYPE.TAB_TERRENO.NAME, 2, 13);

        block.deserializeOld(2, 13, new DataView(view.buffer, index + view.byteOffset, block.deserialize_size_old(2, 13)));                index += block.deserialize_size_old(2, 13);

        this.block_type.push(block);

        block = new GenericTableFloatArray(eBLOCK_TYPE.TAB_ALTURA.NAME, 4, 19);

        block.deserializeOld(4, 19, new DataView(view.buffer, index + view.byteOffset, block.deserialize_size_old(4, 19)));                index += block.deserialize_size_old(4, 19);

        this.block_type.push(block);

        block = new GenericTableFloatArray(eBLOCK_TYPE.TAB_HWI.NAME, 10, 22);

        block.deserializeOld(10, 22, new DataView(view.buffer, index + view.byteOffset, block.deserialize_size_old(10, 22)));              index += block.deserialize_size_old(10, 22);

        this.block_type.push(block);

        block = new GenericTableFloatArray(eBLOCK_TYPE.TAB_HEIGHT.NAME, 2, 19);

        block.deserializeOld(2, 19, new DataView(view.buffer, index + view.byteOffset, block.deserialize_size_old(2, 19)));                 index += block.deserialize_size_old(2, 19);

        this.block_type.push(block);
    }

    deserialize_size() {
        return this.block_type.map((el) => {
            return (el.deserialize_size) ? el.deserialize_size() : el.constructor.deserialize_size();
        }).reduce((accumulator, currValue) => {
            return accumulator + currValue;
        }, 4);
    }

    deserialize_size_old() {
        return (Shot.deserialize_size_old() + (2*13*Float32Array.BYTES_PER_ELEMENT) + (4*19*Float32Array.BYTES_PER_ELEMENT) + (10*22*Float32Array.BYTES_PER_ELEMENT) + (2*19*Float32Array.BYTES_PER_ELEMENT));
    }
}

exports.Tabela = Tabela;