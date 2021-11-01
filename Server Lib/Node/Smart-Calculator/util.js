const http = require('https');
const { HOSTNAME, PORT } = require('./config');

function isprint(char) {
    return !( /[\x00-\x08\x0E-\x1F\x80-\xFF]/.test(char));
}

function myhexdump(data, block_size) {
	var rest = block_size - (data.length % block_size), i = 0;
	var addr = 0;
	var str = '';
	
	for (addr = 0; addr < data.length; addr += block_size) {
		str += ('0000' + addr.toString(16)).slice(-4) + '\t';
		for (i = addr; i < ((addr + block_size) < data.length ?  addr + block_size : data.length); ++i) {
			str += ('00' + data[i].toString(16)).slice(-2) + ' ';
		}
		
		if (i % block_size) {
			for (i = 0; i < rest; ++i)
				str += '--' + ' ';
		}
		
		for (i = addr; i < ((addr + block_size) < data.length ? addr + block_size : data.length); ++i) {
			str += (isprint(String.fromCharCode(data[i])) ? String.fromCharCode(data[i]) + '' : '.');
		}
		str += '\r\n';
	}
	
	return str;
}

function getMacroName(full_macro) {

    let name = full_macro;
    let ret = null;

    if (name instanceof Object || name instanceof Function || name === undefined || name === null || name === '')
        throw new Error("Invalid name");

    ret = name.match(/^([A-z]+\.?[A-z _0-9]+)\(.*\)$/);

    if (ret === undefined || ret === null || !(ret instanceof Array) || ret.length <= 1 || ret[1] === undefined || ret[1] === null || ret[1] === '')
        throw new Error("Invalid name");

    return ret[1];
}

function getMacroParameters(full_macro) {

    let param = full_macro;
    let ret = [];

    if (param instanceof Object || param instanceof Function || param === undefined || param === null)
        throw new Error("Invalid parameters");

    // Macro sem parâmetros é permitido
    if (param === '')
        return ret;

    param = param.match(new RegExp(/^[A-z]+\.?[A-z _0-9]+\(([A-z _0-9,.\-]*)\)$/));

    if (param === undefined || param === null || !(param instanceof Array) || param.length <= 1 || param[1] === undefined || param[1] === null)
        throw new Error("Invalid parameters");

    // Macro sem parâmetros é permitido
    if (param[1] === '')
        return ret;

    param = param[1].split(',').filter((el) => {
        return el !== undefined && el !== null && el !== '' && el !== "\n";
    });

    if (param === undefined || param === null || !(param instanceof Array) || param.length <= 0)
        throw new Error("Invalid parameters");

    // Tira os espaços em brancos
    param.forEach((el, index, arr) => {
        arr[index] = el.trim();
    })

    for (let i = 0; i < param.length; i++)
        if (param[i] === undefined || param[i] === null || param[i] === '' || param[i].indexOf(' ') !== -1)
            throw new Error("Invalid parameters");

    return param;
}

function postRequestCalculadoraStadium(path, data, callback) {

	const d = JSON.stringify(data);

	const options = {
		hostname: HOSTNAME,
		port: PORT,
		path: path,
		method: 'POST',
		headers: {
			'Content-Type': 'application/json',
			'Content-Length': d.length
		}
	}

	const req = http.request(options, res => {

		console.log(`${logDate()} StatusCode: ${res.statusCode}`);

		var buff = "";

		res.on('data', d => {
			buff += d;
        });
        
        res.on('end', () => {

            if (res.statusCode == 200)
				callback(buff);
        });
	});

	req.on('error', error => {
		console.error(logDate() + " " + error);
	});

	req.write(d);
	req.end();
}

function logDate() {

    const now = new Date();

    return `${now.toLocaleString()}.${String(now.getMilliseconds()).padStart(3, '0')}`;
}

module.exports = {
    isprint: isprint,
    logDate: logDate,
    CMyHexDump: myhexdump,
    getMacroName: getMacroName,
    getMacroParameters: getMacroParameters,
    postRequestCalculadoraStadium: postRequestCalculadoraStadium
}