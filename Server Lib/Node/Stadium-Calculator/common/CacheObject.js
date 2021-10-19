// Arquivo CacheObject.js
// Criado em 27/07/2020 as 00:20 por Acrisio
// Definição da classe CacheObject

const { TOKEN } = require('./token')
const crypto = require('crypto');
const { postRequestCalculadoraStadium, decipherUID } = require('../util/util');

const eTYPE_CACHE = {
    CALCULADORA: 1,
    MACRO: 2
}

const eTYPE_PERMISSION = {
    PUBLIC_VIEW_AND_COPY: 0,
    PUBLIC_VIEW: 1,
    PRIVATE: 2,
}

const pass_cache = 'KF7dshudihasiudahsiudhsaiudhsiuadhasiuhdiuaTF7';

class CacheObject {

    cache = '';
    uid = -1;
    id = -1;
    permission = 0;
    owner_uid = -1;

    constructor() {
        
        this.cache = '';
        this.uid = -1;
        this.id = -1;
        this.permission = 0;
    }

    static copyFile(file_name, copy_name, uid, type, callback) {

        let permission = eTYPE_PERMISSION.PUBLIC_VIEW_AND_COPY;

        const owner_uid = decipherUID(file_name.substr(0, file_name.indexOf('.')));

        if (!isNaN(owner_uid) && owner_uid == uid)
            permission = eTYPE_PERMISSION.PRIVATE; // É do mesmo player então cópia ela como privado

        postRequestCalculadoraStadium(type == eTYPE_CACHE.CALCULADORA 
            ? '/calculadora.php' 
            : '/macro.php',
            {
                token: TOKEN,
                type: 'copy',
                uid: uid,
                permission: permission,
                name: file_name,
                copy: copy_name
            }, data => {

                try {

                    const reply = JSON.parse(data);

                    if (reply.hasOwnProperty('error') && reply.error != null) {

                        callback(reply.error, undefined);

                        return;
                    }

                    if (reply.id == -1) {

                        callback({
                            message: 'System error',
                            code: 22000
                        }, undefined);

                        return;
                    }

                    // Reply
                    callback(null, reply.id);

                }catch (err) {

                    console.log(`Error: ${err}`);

                    callback(err, undefined);
                }
            });
    }

    static deleteFile(file_name, id, type, callback) {

        postRequestCalculadoraStadium(type == eTYPE_CACHE.CALCULADORA
            ? '/calculadora.php'
            : '/macro.php',
            {
                token: TOKEN,
                type: 'delete',
                name: file_name,
                id: id
            }, data => {

                try {

                    const reply = JSON.parse(data);

                    if (reply.hasOwnProperty('error') && reply.error != null) {

                        callback(reply.error);

                        return;
                    }

                    // Reply
                    callback(null);

                }catch (err) {

                    console.log(`Fail to delete File[${file_name}]`);

                    callback(err);
                }
            });
    }

    writeFileBuffer(file_name, buffer, type, callback) {

        // Make Hash
        let hash = crypto.createHmac('sha256', pass_cache);
    
        hash.update(buffer);

        this.cache = hash.digest('hex');

        let MagicType = Buffer.alloc(5 + 2 + this.cache.length);

        MagicType.write("CACHE", 0, 'utf8');
        MagicType.writeInt16LE(this.cache.length, 5);
        MagicType.write(this.cache, 7, 'utf8');

        MagicType = Buffer.concat([MagicType, Buffer.from(buffer)]);

        const options = {};

        options.token = TOKEN;
        options.raw = MagicType.toString('base64');
        options.permission = this.permission;

        if (this.id == -1) {

            options.type = 'create';
            options.uid = this.uid;

            options.name = file_name;
        }else {

            options.type = 'update';
            options.id = this.id;
        }

        postRequestCalculadoraStadium(type == eTYPE_CACHE.CALCULADORA 
        ?   '/calculadora.php'
        :   '/macro.php',
        options,
        data => {

            try {

                const reply = JSON.parse(data);

                if (reply.hasOwnProperty('error') && reply.error != null) {

                    callback(reply.error, undefined);

                    return;
                }

                if (reply.id == -1) {
                 
                    callback({
                        message: 'System error', 
                        code: 50000
                    }, undefined);

                    return;
                }

                // Set or Update Id
                this.id = reply.id;

                callback(null, reply.id);

            }catch (err) {
                
                callback({
                    message: err,
                    code: 20000
                }, undefined);
            }
        });
    }

    readFileBuffer(file_name, type, callback) {

        // Clear Cache
        this.cache = '';

        postRequestCalculadoraStadium(type == eTYPE_CACHE.CALCULADORA
            ?   '/calculadora.php'
            :   '/macro.php',
            {
                token: TOKEN,
                type: 'load',
                name: file_name
            }, data => {

                try {

                    const reply = JSON.parse(data);

                    if (reply.hasOwnProperty('error') && reply.error != null) {

                        callback(reply.error, undefined);

                        return;
                    }

                    if (!reply.hasOwnProperty('id')
                        || !reply.hasOwnProperty('uid')
                        || !reply.hasOwnProperty('permission')
                        || !reply.hasOwnProperty('name')
                        || !reply.hasOwnProperty('raw')
                        || !reply.hasOwnProperty('owner_uid')) {

                        callback({
                            message: 'System error',
                            code: 40000
                        }, undefined);
                        
                        return;
                    }

                    this.id = reply.id;
                    this.uid = reply.uid;
                    this.permission = reply.permission;
                    this.owner_uid = reply.owner_uid;

                    // Load Raw
                    const raw_buff = Buffer.from(reply.raw, 'base64');

                    let ret = undefined;
                    let position = 0;

                    let MagicType = Buffer.alloc(5);

                    if (raw_buff.length < MagicType.length)
                        throw new Error("[CacheObject::readFileBuffer] Error file[" + file_name + "] invalid size[" + raw_buff.length + "]");

                    let sd = raw_buff.copy(MagicType, 0, position, 5 + position);

                    if (sd !== MagicType.length)
                        throw new Error("[CacheObject::readFileBuffer] Fail to read Cache Magic Type");

                    position += sd;

                    // Verifica se tem o Cache
                    if (MagicType.includes('CACHE', 0, 'utf8')) {

                        // Read
                        MagicType = Buffer.concat([MagicType, Buffer.alloc(2)]);

                        sd = raw_buff.copy(MagicType, 5, position, 2 + position);

                        if (sd !== 2)
                            throw new Error("[CacheObject::readFileBuffer] Fail to read size cache string");

                        position += sd;

                        let length = MagicType.readUInt16LE(5);

                        if (length >= raw_buff.length || length >= 1024)
                            throw new Error("[CacheObject::readFileBuffer] Error file[" + file_name + "] size[" + length + "] cache string is invalid");

                        MagicType = Buffer.concat([MagicType, Buffer.alloc(length)]);

                        sd = raw_buff.copy(MagicType, 7, position, length + position);

                        if (sd !== length)
                            throw new Error("[CacheObject::readFileBuffer] Fail to read Cache string");

                        position += sd;

                        this.cache = MagicType.toString('utf8', 7, length);
                    
                    }else   // Not have Cache Magic Type
                        position = 0;

                    ret = Buffer.alloc(raw_buff.length - position);

                    sd = raw_buff.copy(ret, 0, position, ret.length + position);

                    if (sd !== ret.length)
                        throw new Error("[CacheObject::readFileBuffer] Fail to read Buffer");

                    // Make cache if not have cache initialized
                    if (this.cache === undefined || this.cache === null || this.cache === '') {

                        let hash = crypto.createHmac('sha256', pass_cache);

                        hash.update(ret);

                        this.cache = hash.digest('hex');
                    }

                    // Reply
                    callback(null, ret);

                }catch (err) {

                    callback({
                        message: err,
                        code: 20000
                    }, undefined);
                }
            });
    }

    readCacheFile(file_name) {

        postRequestCalculadoraStadium(type == eTYPE_CACHE.CALCULADORA
            ?   '/calculadora.php'
            :   '/macro.php',
            {
                token: TOKEN,
                type: 'load',
                name: file_name
            }, data => {

                try {

                    const reply = JSON.parse(data);

                    if (reply.hasOwnProperty('error') && reply.error != null) {

                        callback(reply.error, undefined);

                        return;
                    }

                    if (!reply.hasOwnProperty('id')
                        || !reply.hasOwnProperty('uid')
                        || !reply.hasOwnProperty('permission')
                        || !reply.hasOwnProperty('name')
                        || !reply.hasOwnProperty('raw')
                        || !reply.hasOwnProperty('owner_uid')) {

                        callback({
                            message: 'System error',
                            code: 40000
                        }, undefined);
                        
                        return;
                    }

                    this.id = reply.id;
                    this.uid = reply.uid;
                    this.permission = reply.permission;
                    this.owner_uid = reply.owner_uid;

                    // Load Raw
                    const raw_buff = Buffer.from(reply.raw, 'base64');

                    let ret = undefined;
                    let position = 0;

                    let MagicType = Buffer.alloc(5);

                    if (raw_buff.length < MagicType.length)
                        throw new Error("[CacheObject::readCacheFile] Error file[" + file_name + "] invalid size[" + raw_buff.length + "]");

                    let sd = raw_buff.copy(MagicType, 0, position, 5 + position);

                    if (sd !== MagicType.length)
                        throw new Error("[CacheObject::readCacheFile] Fail to read Cache Magic Type");

                    position += sd;

                    // Verifica se tem o Cache
                    if (MagicType.includes('CACHE', 0, 'utf8')) {

                        // Read
                        MagicType = Buffer.concat([MagicType, Buffer.alloc(2)]);

                        sd = raw_buff.copy(MagicType, 5, position, 2 + position);

                        if (sd !== 2)
                            throw new Error("[CacheObject::readCacheFile] Fail to read size cache string");

                        position += sd;

                        let length = MagicType.readUInt16LE(5);

                        if (length >= raw_buff.length || length >= 1024)
                            throw new Error("[CacheObject::readCacheFile] Error file[" + file_name + "] size[" + length + "] cache string is invalid");

                        MagicType = Buffer.concat([MagicType, Buffer.alloc(length)]);

                        sd = raw_buff.copy(MagicType, 7, position, length + position);

                        if (sd !== length)
                            throw new Error("[CacheObject::readCacheFile] Fail to read Cache string");

                        position += sd;

                        this.cache = MagicType.toString('utf8', 7, length);
                    
                    }else   // Not have Cache Magic Type
                        position = 0;

                    let buff = Buffer.alloc(raw_buff.length - position);

                    sd = raw_buff.copy(buff, 0, position, buff.length + position);

                    if (sd !== buff.length)
                        throw new Error("[CacheObject::readCacheFile] Fail to read Buffer");

                    // Make Hash
                    let hash = crypto.createHmac('sha256', pass_cache);

                    hash.update(buff);

                    ret = hash.digest('hex');

                    // Reply
                    callback(null, ret);

                }catch (err) {

                    callback({
                        message: err,
                        code: 20000
                    }, undefined);
                }
            });
    }
}

module.exports = {
    CacheObject: CacheObject,
    eTYPE_CACHE: eTYPE_CACHE,
    eTYPE_PERMISSION: eTYPE_PERMISSION
}