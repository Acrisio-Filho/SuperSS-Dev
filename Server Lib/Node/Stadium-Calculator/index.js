const http = require('http');
const ctc = require('./calculadora/translateCalculadora.js');
const ctcm = require('./calculadora manager/translateCalculadoraManager.js');
const ctm = require('./MacroMathJS/translateMacro');
const ccm = require('./common/CacheManager.js');
const { resolverHostname } = require('./util/util');
const MyError = require('./type/APIErrorException').APIErrorException;
const { TOKEN } = require('./common/config');

// Const Cache Manager Object
const cCacheManagerCalculator = new ccm.CacheManager();
const cCacheManagerMacro = new ccm.CacheManager();

const coreTranslateCalculadora = new ctc.TranslateAPICalculadora(cCacheManagerCalculator);
const coreTranslateCalculadoraManager = new ctcm.TranslateAPICalculadoraManager(cCacheManagerCalculator);
const coreTranslateMacro = new ctm.TranslateAPIMacroMathJS(cCacheManagerMacro);

//const hostname = "127.0.0.1";
const PORT = process.env.PORT || 8000;

const HOSTNAME_ALLOW = [];
const HOSTNAME_ALLOW_RESOLVED = [];

const callback_resolve_allow = () => {

	// Clear HOSTNAME_ALLOW_RESOLVED
	HOSTNAME_ALLOW_RESOLVED.splice(0, HOSTNAME_ALLOW_RESOLVED.length);

	var count = 0;

	const callback_resolve = (err, address) => {

		if (err)
			console.log(`Error to resolve hostname[${HOSTNAME_ALLOW[count]}]. Message: ${err.message}`);
		else {

			// Add hostname resolved
			HOSTNAME_ALLOW_RESOLVED.push(address);

		}

		if (++count < HOSTNAME_ALLOW.length)
			resolverHostname(HOSTNAME_ALLOW[count], callback_resolve);
	};

	// Trigger
	resolverHostname(HOSTNAME_ALLOW[count], callback_resolve);

	// Next Resolver
	setTimeout(callback_resolve_allow, 60 * 1000 * 5); // 5 min
}

// Resolve Hostnames
//setImmediate(callback_resolve_allow);

//const LOCAL_ENABLE = false;

class Peer {

	url = '';
	method = '';
	rawData = undefined;
	_res = undefined;

	constructor(url, method, rawData, res) {

		this.url = url;
		this.method = method;
		this.rawData = rawData;
		this.res = res;
	}

	get res() {

		if (this._res.destroyed || this._res.writableEnded)
			throw new MyError(`Response is closed`, 7050);

		return this._res;
	}

	set res(res) {
		this._res = res;
	}
}

// Create HTTP server 
const server = http.createServer((req, res) => {

	if (req.method == "OPTIONS") {

		// Set the response HTTP header with HTTP status and Content type
		try {

			res.writeHead(200, {
				'Access-Control-Allow-Origin': '*',
				'Content-Type': 'application/json',
				'Access-Control-Allow-Methods': 'POST, GET, OPTIONS',
				'Access-Control-Allow-Headers': 'Content-Type',
				'Vary': 'Accept-Encoding, Origin'
			});

			res.end();

		}catch (err) {

			console.log(err);
		}

	} else {

		let peer = new Peer(req.url, req.method, '', res);

		let errorHandler = ((peer) => {
			return (e) => {

				try {

					// Log
					console.log(e);

					peer.res.writeHead(200, {
						'Content-Type': 'application/json',
						'Access-Control-Allow-Origin': '*'
					})

					peer.res.write(JSON.stringify({ error: (e === undefined || e === null) ? "Unknown Error" : "Error System" }));

					peer.res.end();

				} catch (err) {

					console.log(err);
				}
			};
		})(peer);

		req.on('aborted', errorHandler);
		req.on('error', errorHandler);

		req.on('data', ((peer) => {
			return (chunk) => {
				peer.rawData += chunk;
			};
		})(peer));

		req.on('end', ((peer) => {
			return () => {

				try {

					// Log
					console.log(`finish Request[URL: ${peer.url}, METHOD: ${peer.method}] Data: ${peer.rawData}`);

					peer.res.writeHead(200, {
						'Content-Type': 'application/json',
						'Access-Control-Allow-Origin': '*'
					})

					if (peer.method == "POST") {

						// Check Token
						try {
							
							const token = JSON.parse(peer.rawData).token;

							if (token != TOKEN)
								throw new MyError("Invalid Token");

						}catch (err) {

							console.log(`Fail to parse token. Message: ${err.message}`);

							peer.res.writeHead(404).end('Not found');

							return;
						}

						if (peer.url == "/macro") {
							coreTranslateMacro.requestTranslate(peer.rawData, (err, data) => {

								if (err) {
									peer.res.writeHead(404).end('Not found');
								}else {
									peer.res.write(data);
									peer.res.end();
								}
							})
							return;
						} else if (peer.url == "/calculadora") {
							coreTranslateCalculadora.requestTranslate(peer.rawData, (err, data) => {

								if (err) {
									peer.res.writeHead(404).end('Not found');
								}else {
									peer.res.write(data);
									peer.res.end();
								}
							});
							return;
						} else if (peer.url == "/calculadora_manager") {
							coreTranslateCalculadoraManager.requestTranslate(peer.rawData, (err, data) => {

								if (err) {
									peer.res.writeHead(404).end('Not found');
								}else {
									peer.res.write(data)
									peer.res.end();
								}
							});
							return;
						} else
							peer.res.write(JSON.stringify({ error: "Invalid request" }));

					} else
						peer.res.write(JSON.stringify({ error: "Invalid request" }));

					peer.res.end();
				
				}catch (err) {

					console.log(err);
				}
			};
		})(peer));
	}
});

server.on('connection', (socket) => {

	var IP = socket.remoteAddress;

	if (IP.substr(0, 7) == "::ffff:")
		IP = IP.substr(7);

	// Log
	console.log(`Conexão IP: ${IP}:${socket.remotePort} aceita.`);

	/*if (LOCAL_ENABLE && IP == '127.0.0.1') {

		// Log
		console.log(`Conexão IP: ${IP}:${socket.remotePort} aceita.`);

		return;
	}

	// Só aceita conexão do IP ss3.sytes.net e localhost
	if (HOSTNAME_ALLOW_RESOLVED.length == 0 && IP == '127.0.0.1') {

		// Log
		console.log(`Conexão IP: ${IP}:${socket.remotePort} aceita.`);

		return;
	}

	for (let i = 0; i < HOSTNAME_ALLOW_RESOLVED.length; i++) {

		if (HOSTNAME_ALLOW_RESOLVED[i] == IP) {

			// Log
			console.log(`Conexão IP: ${IP}:${socket.remotePort} aceita.`);

			return;
		}
	}

	// Fecha a conexão não passou em nenhum IPs aceitos
	console.log(`Recusando a conexão com ip: ${IP}:${socket.remotePort}.`);

	socket.destroy();*/

	return;
});

// handle uncaught exceptions so the application cannot crash
process.on('uncaughtException', function (err) {
	console.log('Caught exception: ' + err)
	console.trace()
})

// Prints a log once the server starts listening
server.listen(PORT, () => {
	console.log(`Server running at http://${server.address().address}:${PORT}/`);
})