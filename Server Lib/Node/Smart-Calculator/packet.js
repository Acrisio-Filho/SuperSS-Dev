//const { CMyHexDump, logDate } = require('./util')

function makePacket(_data, _type) {

	const length = _data.length;

	let pckt = Buffer.alloc(2/*Length*/ + 1/*Type*/);

	pckt.writeUInt16LE(length, 0);
	pckt.writeUInt8(_type, 2);

    let ret = Buffer.concat([pckt, Buffer.from(_data)]);
    
    // !@ Teste
    //console.log(`${logDate()} ${CMyHexDump(ret, 16)}`);

    return ret;
}

module.exports = makePacket;