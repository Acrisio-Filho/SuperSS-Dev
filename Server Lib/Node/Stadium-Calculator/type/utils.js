// New DataView support
if (!DataView.prototype.getUint8Buffer) {

    DataView.prototype.getUint8Buffer = function(ByteOffset, Size) {

        if (isNaN(Size) || isNaN(ByteOffset))
            return null;

        if (this.buffer.byteLength < (this.byteOffset + ByteOffset + Size))
            return null;

        return new Uint8Array(this.buffer, ByteOffset + this.byteOffset, Size);
    }
}

if (!DataView.prototype.setUint8Buffer) {

    DataView.prototype.setUint8Buffer = function(ByteOffset, Buffer) {

        if (isNaN(ByteOffset))
            return;

        if (!(Buffer instanceof Uint8Array))
            return;

        if (this.buffer.byteLength < (this.byteOffset + ByteOffset + Buffer.length))
            return;
        
        let view = new Uint8Array(this.buffer, ByteOffset + this.byteOffset, Buffer.length);

        view.set(Buffer);
    }
}

if (!DataView.prototype.getString) {

    DataView.prototype.getString = function(ByteOffset, Size) {

        if (isNaN(Size) || isNaN(ByteOffset))
            return null;

        if (this.buffer.byteLength < (this.byteOffset + ByteOffset + Size))
            return null;

        let view = this.getUint8Buffer(ByteOffset, Size);

        if (view == null)
            return null;

        let dec = new TextDecoder("utf-8");

        return dec.decode(view);
    }
}

if (!DataView.prototype.setString) {

    DataView.prototype.setString = function(ByteOffset, string) {

        if (isNaN(ByteOffset))
            return;

        if (string === '')
            return;

        if (this.buffer.byteLength < (this.byteOffset + ByteOffset + string.length))
            return;

        let enc = new TextEncoder();

        this.setUint8Buffer(ByteOffset, enc.encode(string));
    }
}

// Last iterator array
if (!Array.prototype.last) {
    Array.prototype.last = function() {
        return (this.length == 0 ? -1 : this[this.length - 1]);
    }
}

// First
if (!Array.prototype.first) {
    Array.prototype.first = function() {
        return (this.length == 0 ? -1 : this[0]);
    }
}

if (!Number.prototype.MAX_UINT8_VALUE)
    Number.prototype.MAX_UINT8_VALUE = 0xFF;

if (!Number.prototype.MAX_UINT16_VALUE)
    Number.prototype.MAX_UINT16_VALUE = 0xFFFF;

if (!Number.prototype.MAX_UINT32_VALUE)
    Number.prototype.MAX_UINT32_VALUE = 0xFFFFFFFF

if (!Number.prototype.MAX_UINT64_VALUE)
    Number.prototype.MAX_UINT64_VALUE = 0xFFFFFFFFFFFFFFFF

// Find if
function find_if(arr, func_cmpare) {

    if (!(arr instanceof Object) || !(func_cmpare instanceof Function))
        return null;

    for (const el in arr) {
        
        if (arr.hasOwnProperty(el) && func_cmpare(arr[el]))
            return arr[el];
    }

    return null;
}

exports.find_if = find_if;

function showDebugLine(message) {
    console.log(`[Line ${(new Error()).stack.split('\n')[3].trim()}]${message}`);
}

exports.showDebugLine = showDebugLine;

if (!Object.prototype.makeDebugLine) {
    Object.prototype.makeDebugLine = function(message) {
        return showDebugLine(`[${(this.constructor.name ? this.constructor.name : Anonimous)}] ${message}`);
    }
}

if (!Object.prototype.filter) {
    Object.prototype.filter = function(func) {

        let ret_obj = {};

        for (let prop in this) {
            if (this.hasOwnProperty(prop) && func(this[prop], prop, this))
                ret_obj[prop] = this[prop];
        }

        return ret_obj;
    };
}

if (!Object.prototype.map) {
    Object.prototype.map = function(func) {

        let ret_obj = {};

        for (let prop in this) {
            if (this.hasOwnProperty(prop))
                ret_obj[prop] = func(this[prop], prop, this);
        }

        return ret_obj;
    }
}

if (!Object.prototype.find) {
    Object.prototype.find = function(func) {

        for (let prop in this)
            if (this.hasOwnProperty(prop) && func(this[prop], prop, this))
                return this[prop];

        return undefined;
    }
}

if (!Object.prototype.findIndex) {
    Object.prototype.findIndex = function(func) {

        for (let prop in this)
            if (this.hasOwnProperty(prop) && func(this[prop], prop, this))
                return prop;

        return -1;
    }
}

const SHOT_TYPE = {
    COBRA: {
        TYPE: 0,
        NAME: "Cobra"
    },
    BACK_SPIN: {
        TYPE: 1,
        NAME: "Back Spin",
        NAME2: "Bs" // Support
    },
    DUNK: {
        TYPE: 2,
        NAME: "Dunk"
    },
    SO_PANGYA: {
        TYPE: 3,
        NAME: "Só PangYa"
    },
    SPIKE: {
        TYPE: 4,
        NAME: "Spike"
    },
    TOMAHAWK: {
        TYPE: 5,
        NAME: "Tomahawk",
        NAME2: "Toma" // Support
    }
}

exports.SHOT_TYPE = SHOT_TYPE;

const CLUB_TYPE = {
    _1W: {
        TYPE: 0,
        NAME: "1W"
    },
    _2W: {
        TYPE: 1,
        NAME: "2W"
    },
    _3W: {
        TYPE: 2,
        NAME: "3W"
    },
    _2I: {
        TYPE: 3,
        NAME: "2I"
    },
    _3I: {
        TYPE: 4,
        NAME: "3I"
    },
    _4I: {
        TYPE: 5,
        NAME: "4I"
    },
    _5I: {
        TYPE: 6,
        NAME: "5I"
    },
    _6I: {
        TYPE: 7,
        NAME: "6I"
    },
    _7I: {
        TYPE: 8,
        NAME: "7I"
    },
    _8I: {
        TYPE: 9,
        NAME: "8I"
    },
    _9I: {
        TYPE: 10,
        NAME: "9I"
    },
    _PW: {
        TYPE: 11,
        NAME: "PW"
    },
    _SW: {
        TYPE: 12,
        NAME: "SW"
    }
}

exports.CLUB_TYPE = CLUB_TYPE;

const SPECIAL_SHOT_TYPE = {
    _NO_POWER_SHOT: {
        TYPE: 0,
        NAME: "No Power Shot",
        NAME2: "Sem PS" // Support
    },
    _1_POWER_SHOT:  {
        TYPE: 1,
        NAME: "Power Shot",
        NAME2: "1PS" // Support
    },
    _2_POWER_SHOT: {
        TYPE: 2,
        NAME: "Double Power Shot",
        NAME2: "2PS" // Support
    }
}

exports.SPECIAL_SHOT_TYPE = SPECIAL_SHOT_TYPE;