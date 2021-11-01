const AuthKey = require('./auth')
const CMakePacket = require('./packet')
const CCalc = require('./smart_calculator')
const { getMacroName, getMacroParameters, postRequestCalculadoraStadium, logDate } = require('./util')
const { TOKEN } = require('./config');
const CIConLite = require('iconv-lite');
const BOT = require('./Discord/discord_bot');

function responseLogin(_player, _packet) {

    let size = _packet.indexOf(0, 0);
    const key = _packet.toString('utf-8', 0, size);

    if (key === AuthKey) {

        const uid = _packet.readUInt32LE(size + 1);

        console.log(`${logDate()} SERVER UID: ${uid}`);

        _player.uid = uid;

        _player.logged = true;

        // Reply
        _player.socket.write(CMakePacket(Buffer.concat([Buffer.from('OK'), Buffer.from('\0')]), this.reply_id));
    
    }else {

        // Derrubando
        console.log(`${logDate()} Failed Key Auth: ${key}`);

        _player.socket.end();
    }
}

function responseHandShake(_player, _packet) {

    let size = _packet.indexOf(0, 0);
    const message = _packet.toString('utf-8', 0, size);

    console.log(`${logDate()} Hand Shake: ${message}`);
}

function responseCommandServer(_player, _packet) {

    // Send Msg to Discod Bot
    let size = _packet.indexOf(0, 0);

    const command = CIConLite.decode(_packet.slice(0, size), 'Shift_JIS');

    // Send Message to discord
    if (BOT.state)
        BOT.sendMessageToChannelChatHistory(command);
    else {

        // Tenta conectar de 5 em 5 minutos para enviar a message
        if (BOT.start()) {

            const try_connect_bot_lambda = () => {

                if (BOT.state)
                    BOT.sendMessageToChannelChatHistory(command);
                else {

                    if (BOT.start())
                        setTimeout(try_connect_bot_lambda, 5 * 60000); // Tenta daqui a 5 minutos novamente
                }
            };
        
            setTimeout(try_connect_bot_lambda, 10 * 1000); // Espera 10 segundos para ver se conectou tudo ok
        }
    }
}

function responsePing(_player, _packet) {

    let index = 0;

    const id = _packet.readUInt32LE(index);
    index += 4;

    const createdTimestamp = _packet.readUInt32LE(index) * 1000; // milliseconds
    index += 4;

    const timeTaken = Date.now() - createdTimestamp;

    message = `Pong! This message had a latency of ${timeTaken}ms.`;
    
    let buff = Buffer.alloc(5);

    buff.writeUInt32LE(id, 0);
    buff.writeUInt8(this.id, 4);

    buff = Buffer.concat([buff, Buffer.from(message), Buffer.from('\0')]);

    _player.socket.write(CMakePacket(buff, this.reply_id));
}

function responseSmartCalc(_player, _packet) {

    let index = 0;
    const id = _packet.readUInt32LE(index);
    index += 4;

    let obj = {};
    obj.power = _packet.readUInt8(index);
    index++;
    obj.auxpart_pwr = _packet.readInt8(index);
    index++;
    obj.mascot_pwr = _packet.readInt8(index);
    index++;
    obj.card_pwr = _packet.readInt8(index);
    index++;
    obj.ps_card_pwr = _packet.readInt8(index);
    index++;
    obj.club_index = _packet.readUInt8(index);
    index++;
    obj.shot_index = _packet.readUInt8(index);
    index++;
    obj.power_shot_index = _packet.readUInt8(index);
    index++;
    obj.distance = _packet.readFloatLE(index);
    index += 4;
    obj.height = _packet.readFloatLE(index);
    index += 4;
    obj.wind = _packet.readFloatLE(index);
    index += 4;
    obj.degree = _packet.readFloatLE(index);
    index += 4;
    obj.ground = _packet.readFloatLE(index);
    index += 4;
    obj.spin = _packet.readFloatLE(index);
    index += 4;
    obj.curve = _packet.readFloatLE(index);
    index += 4;
    let opt_slope = _packet.readUInt8(index);
    index++;

    if (opt_slope) {

        // Special Slope
        let special_slope_break = [];

        for (let i = 0; i < 3; i++) {
            special_slope_break.push(_packet.readFloatLE(index));
            index += 4;
        }

        obj.slope_break = special_slope_break.join(', ');

    }else {

        // Normal Slope
        obj.slope_break = _packet.readFloatLE(index);
        index += 4;
    }

    // Calc
    const data = CCalc(obj);

    // Reply
    let buff = Buffer.alloc(6);

    buff.writeUInt32LE(id, 0);
    buff.writeUInt8(this.id, 4); // Packet ID

    index = 5;
    
    if (!(data instanceof Object)) {

        buff.writeUInt8(2, index); // String
        index++;
    
        buff = Buffer.concat([buff, Buffer.from(data), Buffer.from(['\0'])]);
    
    }else {

        if (!data.error) { // Sucesso

            buff.writeUInt8(0, index);
            index++;

            buff = Buffer.concat([buff, Buffer.alloc(20)]);

            buff.writeFloatLE(data.desvio, index);
            index += 4;
            buff.writeFloatLE(data.percent, index);
            index += 4;
            buff.writeFloatLE(data.caliper, index);
            index += 4;
            buff.writeFloatLE(data.hwi, index);
            index += 4;
            buff.writeFloatLE(data.spin, index);
            index += 4;

        }else { // Error

            buff.writeUInt8(1, index); // Error
            index++;

            buff = Buffer.concat([buff, Buffer.from(data.message), Buffer.from(['\0'])]);
        }
    }

    _player.socket.write(CMakePacket(buff, this.reply_id));
}

function responseMacro(_player, _packet) {

    let index = 0;
    const id = _packet.readUInt32LE(index);
    index += 4;

    let obj = {};
    obj.distance = _packet.readFloatLE(index);
    index += 4;
    obj.height = _packet.readFloatLE(index);
    index += 4;
    obj.wind = _packet.readFloatLE(index);
    index += 4;
    obj.degree = _packet.readFloatLE(index);
    index += 4;
    obj.ground = _packet.readFloatLE(index);
    index += 4;
    obj.slope_break = _packet.readFloatLE(index);
    index += 4;
    obj.green_slope = _packet.readFloatLE(index);
    index += 4;
    obj.power = _packet.readFloatLE(index);
    index += 4;
    obj.desvio = _packet.readFloatLE(index);
    index += 4;
    obj.percent = _packet.readFloatLE(index);
    index += 4;
    obj.caliper = _packet.readFloatLE(index);
    index += 4;
    obj.hwi = _packet.readFloatLE(index);
    index += 4;
    obj.spin = _packet.readFloatLE(index);
    index += 4;
    let size = _packet.indexOf(0, index) - index;

    const full_macro = _packet.toString('ascii', index, size + index);

    // Function Reply
    const fReply = (data) => {

        let buff = Buffer.alloc(5);

        buff.writeUInt32LE(id, 0);
        buff.writeUInt8(this.id, 4); // Packet Id
        
        buff = Buffer.concat([buff, Buffer.from(data), Buffer.from(['\0'])]);

        _player.socket.write(CMakePacket(buff, this.reply_id));
    }

    let name = '';
    let parameters_in = [];

    try {

        name = getMacroName(full_macro);
        parameters_in = getMacroParameters(full_macro);

        postRequestCalculadoraStadium('/macro', {
            type: 'params',
            token: TOKEN,
            uid: id,
            name: name
        }, data => {

            let params = JSON.parse(data);

            if (params === undefined || params === null || !(params instanceof Object)) {
                fReply(`Failed to exec macro "${name}"`);

                return;
            }

            if (params.hasOwnProperty('error')) {
                
                console.log(logDate() + " " + params.error);
                
                fReply(`Failed to exec macro "${name}"`);
                
                return;
            }

            if (!params.hasOwnProperty('parameters') || !(params.parameters instanceof Array)) {
                fReply(`Failed to exec macro "${name}"`);

                return;
            }

            try {

                let parameters = params.parameters.filter((el) => {
                    return el != 'inputs' && el != 'results';
                });

                if (parameters_in.length != parameters.length)
                    throw new Error("invalids parameters");

                postRequestCalculadoraStadium('/macro', {
                    type: 'exec',
                    token: TOKEN,
                    uid: id,
                    name: name,
                    inputs: JSON.stringify({
                        distancia: obj.distance,
                        altura: obj.height,
                        vento: obj.wind,
                        angulo: obj.degree,
                        slope_bola: obj.slope_break,
                        slope_green: obj.green_slope,
                        terreno: obj.ground,
                        power: obj.power
                    }),
                    results : JSON.stringify({
                        desvio: obj.desvio,
                        percent: obj.percent,
                        caliper: obj.caliper,
                        hwi: obj.hwi,
                        spin: obj.spin
                    }),
                    parameters: JSON.stringify(parameters_in)
                }, data => {

                    let ret = JSON.parse(data);

                    if (ret === undefined || ret === null || !(ret instanceof Object)) {

                        fReply("System error");
                        
                        return;
                    }

                    if (ret.hasOwnProperty('error')) {

                        console.log(logDate() + " " + ret.error);

                        fReply(`Failed to exec macro "${name}"`);

                        return;
                    }

                    if (!ret.hasOwnProperty('message') || ret.message === '') {
                        fReply(`Failed to exec macro "${name}"`);
                        
                        return;
                    }

                    fReply(ret.message);
                });

            }catch (err) {

                fReply(err.toString());

                return;
            }
        });

    }catch (err) {

        fReply(err.toString());

        return;
    }
}

function responseExpression(_player, _packet) {

    let index = 0;
    const id = _packet.readUInt32LE(index);
    index += 4;

    //console.log(`${logDate()} ${id}`);

    const size = _packet.indexOf(0, index) - index;
    const expr = _packet.toString('utf-8', index, size + index);

    // Function Reply
    const fReply = (data) => {

        let buff = Buffer.alloc(5);

        buff.writeUInt32LE(id, 0);
        buff.writeUInt8(this.id, 4); // Packet Id
        
        buff = Buffer.concat([buff, Buffer.from(data), Buffer.from(['\0'])]);

        _player.socket.write(CMakePacket(buff, this.reply_id));
    }

    postRequestCalculadoraStadium('/macro', {
        type: 'expr',
        token: TOKEN,
        uid: id,
        expression: expr
    },
    data => {
        
        let reply = JSON.parse(data);

        if (reply === undefined || reply === null || !(reply instanceof Object)) {
            
            console.log(logDate() + " Failed to execute expression");
            
            fReply("System error");

            return;
        }

        if (reply.hasOwnProperty('error')) {

            console.log(logDate() + " " + reply.error);

            fReply("Failed to execute expression");

            return;
        }

        if (!reply.hasOwnProperty('result')) {
            fReply("Failed to execute expression");

            return;
        }

        fReply(reply.result);
    });
}

function responseListMacro(_player, _packet) {

    let index = 0;
    const id = _packet.readUInt32LE(index);
    index += 4;

    // Function Reply
    const fReply = (data) => {

        let buff = Buffer.alloc(5);

        buff.writeUInt32LE(id, 0);
        buff.writeUInt8(this.id, 4); // Packet Id
        
        buff = Buffer.concat([buff, Buffer.from(data), Buffer.from(['\0'])]);

        _player.socket.write(CMakePacket(buff, this.reply_id));
    }

    postRequestCalculadoraStadium('/macro', {
        type: 'listex',
        token: TOKEN,
        uid: id
    },
    data => {

        let list = JSON.parse(data);

        if (list === undefined || list === null) {
            
            console.log(logDate() + " Failed to execute macro listex command");

            fReply("System error");
            
            return;
        }

        if (list instanceof Object && list.hasOwnProperty('error')) {

            console.log(logDate() + " " + list.error)

            fReply("System error");

            return;
        }

        if (!(list instanceof Array)) {

            console.log(logDate() + " Failed to execute macro listex command");

            fReply("System error");
            
            return;
        }

        fReply(list.filter(el => {

            if (el === undefined || el === null || !(el instanceof Object) || !el.hasOwnProperty('name') || el.name === ''
                    || !el.hasOwnProperty('parameters') || !(el.parameters instanceof Array))
                return false;

            return true;
        }).map((el) => {
            return `${el.name.replace(/.mcr/, '')}(${el.parameters.filter(el_p => {

                if (el_p === undefined || el_p === null || el_p === '' || el_p === 'inputs' || el_p === 'results')
                    return false;

                return true;
            }).join(', ')})`;
        }).join('\n'));
    });
}

function responseStadiumCalc(_player, _packet) {

    let index = 0;

    const id = _packet.readUInt32LE(index);
    index += 4;

    let size = _packet.indexOf(0, index) - index;

    const name = _packet.toString('ascii', index, size + index);
    index += size + 1; // Null String

    let obj = {};

    obj.tacada = _packet.readUInt8(index);
    index++;
    obj.distancia = _packet.readFloatLE(index);
    index += 4;
    obj.altura = _packet.readFloatLE(index);
    index += 4;
    obj.vento = _packet.readFloatLE(index);
    index += 4;
    obj.angulo = _packet.readFloatLE(index);
    index += 4;
    obj.terreno = _packet.readFloatLE(index);
    index += 4;

    // Normal Slope
    obj.slope_bola = _packet.readFloatLE(index);
    index += 4;
    obj.slope_green = _packet.readFloatLE(index);
    index += 4;
    obj.power = _packet.readFloatLE(index);
    index += 4;

    // Function Reply
    const fReply = (data) => {

        let buff = Buffer.alloc(6);

        buff.writeUInt32LE(id, 0);
        buff.writeUInt8(this.id, 4); // Packet ID

        index = 5;
        
        if (!(data instanceof Object)) {

            buff.writeUInt8(2, index); // String
            index++;
        
            buff = Buffer.concat([buff, Buffer.from(data), Buffer.from(['\0'])]);
        
        }else {

            if (!data.error) { // Sucesso

                buff.writeUInt8(0, index);
                index++;

                buff = Buffer.concat([buff, Buffer.alloc(20)]);

                buff.writeFloatLE(data.desvio, index);
                index += 4;
                buff.writeFloatLE(data.percent, index);
                index += 4;
                buff.writeFloatLE(data.caliper, index);
                index += 4;
                buff.writeFloatLE(data.hwi, index);
                index += 4;
                buff.writeFloatLE(data.spin, index);
                index += 4;

            }else { // Error

                buff.writeUInt8(1, index); // Error
                index++;

                buff = Buffer.concat([buff, Buffer.from(data.message), Buffer.from(['\0'])]);
            }
        }

        _player.socket.write(CMakePacket(buff, this.reply_id));
    }

    // ResultsSender, retorno
    class ResultsSender {

        constructor(desvio, percent, caliper, hwi, spin) {
    
            this.desvio = ResultsSender.isValidParameter(desvio);
            this.percent = ResultsSender.isValidParameter(percent);
            this.caliper = ResultsSender.isValidParameter(caliper);
            this.hwi = ResultsSender.isValidParameter(hwi);
            this.spin = ResultsSender.isValidParameter(spin);

            // support reply function
            this.error = false;
        }
    
        static isValidParameter(parameter) {
    
            if (parameter === undefined || isNaN(parameter))
                return 0;
            
            return parameter;
        }
    }

    // Calc com http server
    postRequestCalculadoraStadium('/calculadora', {
        type: 'calcule',
        token: TOKEN,
        name: name,
        input: obj,
        uid: id,
        old_flag: false
    }, data => {

        let result = JSON.parse(data);

        if (result === undefined || result === null || !(result instanceof Object)) {
            
            fReply("System error");
            
            return;
        }

        if (result.hasOwnProperty("error")) {

            console.log(logDate() + " Error: " + result.error);

            if (result.hasOwnProperty('code') && result.code === 1)
                fReply("Open calculator again, shot has changed.");
            else
                fReply("Error to calcule.");

            return;
        }

        // Formata o resultado
        result_ret = new ResultsSender();

        for (let prop in result)
            if (result.hasOwnProperty(prop) && result_ret.hasOwnProperty(prop))
                result_ret[prop] = result[prop];

        // Percent * 100, to value real
        result_ret.percent *= 100;

        // Send Reply
        fReply(result_ret);
    });
}

function responseStadiumListCalc(_player, _packet) {

    let index = 0;

    const id = _packet.readUInt32LE(index);
    index += 4;

    // Function Reply
    const fReply = (data) => {

        let buff = Buffer.alloc(6);

        buff.writeUInt32LE(id, 0);
        buff.writeUInt8(this.id, 4); // Packet ID

        index = 5;
        
        if (!(data instanceof Array)) {

            buff.writeUInt8(2, index); // String
            index++;
        
            buff = Buffer.concat([buff, Buffer.from(data), Buffer.from(['\0'])]);
        
        }else {

            buff.writeUInt8(0, index);
            index++;

            // Realloc o buffer
            buff = Buffer.concat([buff, Buffer.alloc(2)]);

            buff.writeUInt16LE(data.length, index);
            index += 2;

            data.forEach(el => {
                buff = Buffer.concat([buff, Buffer.from(el.name), Buffer.from('\0')]);
            });
        }

        _player.socket.write(CMakePacket(buff, this.reply_id));
    }

    postRequestCalculadoraStadium('/calculadora', {
        type: 'list',
        token: TOKEN,
        uid: id
    }, data => {

        let list = JSON.parse(data);

        if (list === undefined || list === null) {
            
            fReply("System error");
            
            return;
        }

        if (list instanceof Object && list.hasOwnProperty('error')) {

            console.log(logDate() + " Error: " + list.error);

            fReply("Error to list calculators.");

            return;
        }

        if (!(list instanceof Array)) {
            
            fReply("System error");
            
            return;
        }

        fReply(list);
    });
}

function responseStadiumOpenCalc(_player, _packet) {

    let index = 0;

    const id = _packet.readUInt32LE(index);
    index += 4;

    let size = _packet.indexOf(0, index) - index;

    const name = _packet.toString('ascii', index, size + index);
    index += size;

    // Function Reply
    const fReply = (data) => {

        let buff = Buffer.alloc(6);

        buff.writeUInt32LE(id, 0);
        buff.writeUInt8(this.id, 4); // Packet ID

        index = 5;
        
        if (!(data instanceof Array)) {

            buff.writeUInt8(2, index); // String
            index++;
        
            buff = Buffer.concat([buff, Buffer.from(data), Buffer.from(['\0'])]);
        
        }else {

            buff.writeUInt8(0, index);
            index++;

            buff = Buffer.concat([buff, Buffer.alloc(4)]);

            buff.writeUInt32LE(1/*Calulator Opened ID*/, index);
            index += 4;

            buff = Buffer.concat([buff, Buffer.from(name), Buffer.from('\0')]);
            index += name.length + 1;

            // Realloc o buffer
            buff = Buffer.concat([buff, Buffer.alloc(2)]);

            buff.writeUInt16LE(data.length, index);
            index += 2;

            data.forEach(shot => {

                buff = Buffer.concat([buff, Buffer.alloc(4)]);

                buff.writeUInt32LE(shot.id, index);
                index += 4;

                buff = Buffer.concat([buff, Buffer.from(shot.name), Buffer.from('\0')]);
                index += shot.name.length + 1;

                buff = Buffer.concat([buff, Buffer.alloc(2)]);

                buff.writeUInt16LE(shot.club.length, index);
                index += 2;

                shot.club.forEach(club => {

                    buff = Buffer.concat([buff, Buffer.alloc(8)]);

                    buff.writeUInt32LE(club.index, index);
                    index += 4;

                    buff.writeFloatLE(club.power, index);
                    index += 4;

                    buff = Buffer.concat([buff, Buffer.from(club.name), Buffer.from('\0')]);
                    index += club.name.length + 1;
                });
            });
        }

        _player.socket.write(CMakePacket(buff, this.reply_id));
    }

    postRequestCalculadoraStadium('/calculadora', {
        type: 'open',
        token: TOKEN,
        name: name,
        uid: id
    }, data => {

        let combo_shot = JSON.parse(data);

        if (combo_shot === undefined || combo_shot === null) {

            fReply("System error");

            return;
        }

        if (combo_shot instanceof Object && combo_shot.hasOwnProperty('error')) {

            console.log(logDate() + " Error: " + combo_shot.error);

            if (combo_shot.hasOwnProperty('code') && combo_shot.code === 1)
                fReply('Error list of calculators is changed. execute command "list calc" to get new index.');
            else
                fReply('Error to open calculator');

            return;
        }

        if (!(combo_shot instanceof Array)) {
            
            fReply("System error");
            
            return;
        }

        // reply
        fReply(combo_shot);
    });
}

module.exports = {
    responseLogin: responseLogin,
    responseHandShake: responseHandShake,
    responsePing: responsePing,
    responseListMacro: responseListMacro,
    responseMacro: responseMacro,
    responseExpression: responseExpression,
    responseSmartCalc: responseSmartCalc,
    responseStadiumCalc: responseStadiumCalc,
    responseStadiumListCalc: responseStadiumListCalc,
    responseStadiumOpenCalc: responseStadiumOpenCalc,
    responseCommandServer: responseCommandServer
}