const { ePACKET } = require('./types')
const {
    responseLogin,
    responseHandShake,
    responsePing,
    responseListMacro,
    responseMacro,
    responseExpression,
    responseSmartCalc,
    responseStadiumCalc,
    responseStadiumListCalc,
    responseStadiumOpenCalc,
    responseCommandServer
} = require('./translates')
const { logDate } = require('./util');

const map_response = [];

function execResponse(_player, _type, _packet) {

    const it = map_response.find((el) => {
        return el.id == _type;
    });

    if (it !== undefined)
        it.exec(_player, _packet);
    else
        console.log(`${logDate()} [execResponse][Error] Socket[IP=${_player.socket.remoteAddress}, PORT=${_player.socket.remotePort}] Packet Id: ${_type} invalid.`)
}

// Login
map_response.push({
    id: ePACKET.LOGIN,
    reply_id: ePACKET.HAND_SHAKE,
    exec: responseLogin
})

// HAND SHAKE
map_response.push({
    id: ePACKET.HAND_SHAKE,
    reply_id: ePACKET.HAND_SHAKE,
    exec: responseHandShake
})

// COMMAND SERVER
map_response.push({
    id: ePACKET.COMMAND_SERVER,
    reply_id: ePACKET.REPLY_SERVER,
    exec: responseCommandServer
})

// SMART PING
map_response.push({
    id: ePACKET.COMMAND_SMART_PING,
    reply_id: ePACKET.REPLY_SMART,
    exec: responsePing
})

// SMART LIST MACRO
map_response.push({
    id: ePACKET.COMMAND_SMART_LIST_MACRO,
    reply_id: ePACKET.REPLY_SMART,
    exec: responseListMacro
})

// SMART CALC
map_response.push({
    id: ePACKET.COMMAND_SMART_CALC,
    reply_id: ePACKET.REPLY_SMART,
    exec: responseSmartCalc
})

// SMART MACRO
map_response.push({
    id: ePACKET.COMMAND_SMART_MACRO,
    reply_id: ePACKET.REPLY_SMART,
    exec: responseMacro
})

// SMART EXPRESSION
map_response.push({
    id: ePACKET.COMMAND_SMART_EXPRESSION,
    reply_id: ePACKET.REPLY_SMART,
    exec: responseExpression
})

// STADIUM PING
map_response.push({
    id: ePACKET.COMMAND_STADIUM_PING,
    reply_id: ePACKET.REPLY_STADIUM,
    exec: responsePing
})

// STADIUM LIST MACRO
map_response.push({
    id: ePACKET.COMMAND_STADIUM_LIST_MACRO,
    reply_id: ePACKET.REPLY_STADIUM,
    exec: responseListMacro
})

// STADIUM LIST CALC
map_response.push({
    id: ePACKET.COMMAND_STADIUM_LIST_CALC,
    reply_id: ePACKET.REPLY_STADIUM,
    exec: responseStadiumListCalc
})

// STADIUM OPEN CALC
map_response.push({
    id: ePACKET.COMMAND_STADIUM_OPEN_CALC,
    reply_id: ePACKET.REPLY_STADIUM,
    exec: responseStadiumOpenCalc
})

// STADIUM CALC
map_response.push({
    id: ePACKET.COMMAND_STADIUM_CALC,
    reply_id: ePACKET.REPLY_STADIUM,
    exec: responseStadiumCalc
})

// STADIUM MACRO
map_response.push({
    id: ePACKET.COMMAND_STADIUM_MACRO,
    reply_id: ePACKET.REPLY_STADIUM,
    exec: responseMacro
})

// STADIUM EXPRESSION
map_response.push({
    id: ePACKET.COMMAND_STADIUM_EXPRESSION,
    reply_id: ePACKET.REPLY_STADIUM,
    exec: responseExpression
})

function translatePacket(player) {

	let len = 0;

	while (player.data.length >= 2 && (len = player.data.readUInt16LE(0)) < player.data.length) {

        let type = player.data[2];
        
        execResponse(player, type, player.data.slice(3, len + 3));

        player.data = player.data.slice(len + 3, player.data.length);
	}
}

module.exports = translatePacket;