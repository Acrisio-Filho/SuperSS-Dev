const http = require('http');
const querys = require('querystring');
const { Resolver } = require('dns');

const HOSTNAME = 'Seu HOSTNAME|IP';
const PORT = 80;
const PATH = '/stadium';

function postRequestCalculadoraStadium(path, data, callback) {

    const d = querys.stringify(data);

	const options = {
		hostname: HOSTNAME,
		port: PORT,
		path: PATH + path,
		method: 'POST',
		headers: {
			'Content-Type': 'application/x-www-form-urlencoded',
			'Content-Length': d.length
		}
	}

	const req = http.request(options, res => {

		console.log(`StatusCode: ${res.statusCode}`);

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
		console.error(error);
	});

	req.write(d);
	req.end();
}

const TABLE_CIPHER = "FODASEMINU";
const TABLE_DECHIPHER = {
    'F': '0',
    'O': '1',
    'D': '2',
    'A': '3',
    'S': '4',
    'E': '5',
    'M': '6',
    'I': '7',
    'N': '8',
    'U': '9'
}

function cipherUID(value) {

	if (isNaN(value))
		return null;

	let chiph = '';

	const value_str = value.toString();

	for (let i = 0; i < value_str.length; i++) {
		chiph += TABLE_CIPHER[value_str[i].valueOf()];
	}

	return chiph;
}

function decipherUID(ciph) {

    if (ciph == '')
        return NaN;

    let deciph = '';

    for (let i = 0; i < ciph.length; i++) {
        deciph += TABLE_DECHIPHER[ciph[i]];
    }

    return new Number(deciph);
}

function resolverHostname(hostname, callback) {

	const resolver = new Resolver();
	resolver.setServers(['8.8.8.8', '8.8.4.4']); // Dns Google

	resolver.resolve4(hostname, (err, address) => {

		if (err) {

			callback(err, undefined);

			return;
		}

		// Reply
		callback(null, address);
	});
}

module.exports = {
	postRequestCalculadoraStadium: postRequestCalculadoraStadium,
	cipherUID: cipherUID,
	decipherUID: decipherUID,
	resolverHostname: resolverHostname
};