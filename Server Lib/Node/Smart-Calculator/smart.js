const net = require('net');
const { CMyHexDump, logDate } = require('./util');
const CTranslatePacket = require('./dispatch')
const SessionManager = require('./session_manager')
const CIConLite = require('iconv-lite');

// Usa no BOT Discord
const CMakePacket = require('./packet')
const { ePACKET } = require('./types')
const BOT = require('./Discord/discord_bot');

const DEBUG_FLAG = false;

// Command notice
const SEPARETOR_CMD = ',';

function bytesToHex(bytes) {
	return Array.from(
		bytes,
		byte => byte.toString(16).padStart(2, "0")
	).join("");
}

BOT.message_callback = (command, args) => {

	if (command.length === 0)
		return;

	if (command === 'chat_discord') {

		if (args.length == 0)
			return;

		// Send All servers connected
		if (args.length === 1) {

			SessionManager.sessions.forEach(server => {

				let pckt = Buffer.alloc(4);

				pckt.writeUInt32LE(server.uid);

				// send
				server.socket.write(CMakePacket(Buffer.concat([pckt, Buffer.from(command + ' ' + args[0]), Buffer.from('\0')]), ePACKET.REPLY_SERVER));
			});

		}else if (args.length === 2) {

			if (isNaN(args[0]))
				return;

			const server = SessionManager.sessions.find(el => {
				return el.uid == args[0];
			});

			if (server === undefined)
				return;

			let pckt = Buffer.alloc(4);

			pckt.writeUInt32LE(server.uid);

			// send
			server.socket.write(CMakePacket(Buffer.concat([pckt, Buffer.from(command + ' ' + args[1]), Buffer.from('\0')]), ePACKET.REPLY_SERVER));
		}

	}
	
	else if (command === 'notice') {

		if (args.length == 0)
			return;

		// Send All servers connected
		if (args.length === 1) {

			SessionManager.sessions.forEach(server => {

				let pckt = Buffer.alloc(4);

				pckt.writeUInt32LE(server.uid);

				// send
				server.socket.write(CMakePacket(Buffer.concat([pckt, Buffer.from(command + ' '), Buffer.from(bytesToHex(CIConLite.encode(args[0], 'Shift_JIS')), 'hex'), Buffer.from('\0')]), ePACKET.REPLY_SERVER));
			});

		}else if (args.length >= 2) {

			const complex = args.join(' ');

			if (!complex)
				return;

			let commands = complex.split(SEPARETOR_CMD);

			if (commands.length == 0)
				return;

			if (commands.length === 1) {

				SessionManager.sessions.forEach(server => {

					let pckt = Buffer.alloc(4);
	
					pckt.writeUInt32LE(server.uid);
	
					// send
					server.socket.write(CMakePacket(Buffer.concat([pckt,  Buffer.from(command + ' '), Buffer.from(bytesToHex(CIConLite.encode(commands[0].trimStart(), 'Shift_JIS')), 'hex'), Buffer.from('\0')]), ePACKET.REPLY_SERVER));
				});
			
			}else if (commands.length >= 2) {

				if (isNaN(commands[0])) {

					SessionManager.sessions.forEach(server => {

						let pckt = Buffer.alloc(4);
		
						pckt.writeUInt32LE(server.uid);
		
						// send
						server.socket.write(CMakePacket(Buffer.concat([pckt,  Buffer.from(command + ' '), Buffer.from(bytesToHex(CIConLite.encode(commands.join(SEPARETOR_CMD).trimStart(), 'Shift_JIS')), 'hex'), Buffer.from('\0')]), ePACKET.REPLY_SERVER));
					});
		
					return;
				}

				// Find server and send message
				const server_uid = Number(commands.shift());

				const server = SessionManager.sessions.find(el => {
					return el.uid == server_uid;
				});

				if (server === undefined)
					return;

				let pckt = Buffer.alloc(4);

				pckt.writeUInt32LE(server.uid);

				// send
				server.socket.write(CMakePacket(Buffer.concat([pckt,  Buffer.from(command + ' '), Buffer.from(bytesToHex(CIConLite.encode(commands.join(SEPARETOR_CMD).trimStart(), 'Shift_JIS')), 'hex'), Buffer.from('\0')]), ePACKET.REPLY_SERVER));
			}
		}
	}
}

// Start BOT Discord
// !@ desabiliata, por que já tem outro testando
BOT.actived = false;

const ret_start_bot = BOT.start();

if (!ret_start_bot)
	console.log(`${logDate()} Fail start bot.`);

const server = net.createServer(function(socket) {
	
	console.log(logDate() + " Cliente connected: " + (socket.remoteAddress + ":" + socket.remotePort));

	// Cria socket session
	SessionManager.createSession(socket);
	
	socket.on('data', function(data) {

		// Log
		console.log(logDate() + ' received data from socket: ' + (socket.remoteAddress + " " + socket.remotePort)); 
		
		if (DEBUG_FLAG)
			console.log(' cliente Received: \n' + CMyHexDump(data, 16));
		
		const session = SessionManager.findSession(socket);

		if (session === undefined) {

			console.log(`${logDate()} [onData] Not found session of Socket: ${socket.remoteAddress}:${socket.remotePort}`);

			socket.end();

			return;
		}

		session.data = Buffer.concat([session.data, data]);

		// Translate
		CTranslatePacket(session);
	});

	// 3 segundos inativo fecha o socket
	socket.setTimeout(3000, () => {

		const session = SessionManager.findSession(socket);

		if (session === undefined) {

			console.log(`${logDate()} [onTimeout] Not found session of Socket: ${socket.remoteAddress}:${socket.remotePort}`);

			socket.end();

			return;
		}

		// Fecha ele não fez login
		if (!session.logged)
			socket.end();
	});

	socket.on('error', (err) => {
		console.log(logDate() + " " + err);
	});

	socket.on('close', (err) => {

		if (err)
			console.log(logDate() + " " + err);

		SessionManager.closeSession(socket);
	})
});

server.on('connection', (socket) => {

	// Só aceita conexão local
	if (socket.remoteAddress != '127.0.0.1')
		socket.end();
});

server.on("error", (err) => {
	console.log(logDate() + " " + err);
})

server.on('listening', () => {

	console.log(`${logDate()} Server listing in port: ${server.address().port}`)
})

server.listen(12345, '127.0.0.1');

// handle uncaught exceptions so the application cannot crash
process.on('uncaughtException', function (err) {
	console.log(logDate() + ' Caught exception: ' + err)
	console.trace()
})
