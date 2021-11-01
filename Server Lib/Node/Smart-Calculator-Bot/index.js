// Arquivo index.js
// Criado em 27/09/2020 as 19:52 por Acrisio
// App do Smart Calculator com Bot do Discord

const Discord = require("discord.js");
const config = require("./config.json");

const prefix = "!";

const CManagerUser = require('./manager_user');
const CUser = require('./user');
const CCalc = require('./smart_calculator');

function findOrCreateUser(id) {

	let user = mngrUser.findUser(id);

	if (user === undefined)
		user = mngrUser.addUser(id, new CUser());

	return user;
}

const mngrUser = new CManagerUser(900000);

const client = new Discord.Client();

client.on("message", function (message) {
	if (message.author.bot) return;
	if (!message.content.startsWith(prefix)) return;

	const commandBody = message.content.slice(prefix.length);
	const args = commandBody.split(' ');
	const command = args.shift().toLowerCase();

	if (command === "ping") {
		const timeTaken = Date.now() - message.createdTimestamp;
		message.reply(`Pong! This message had a latency of ${timeTaken}ms.`);
	}

	else if (command === "destroy") {

		if (message.member.hasPermission('ADMINISTRATOR'))
			setImmediate(() => {
				client.destroy();
			});
	}

	else if (command === "info") {

		message.channel.send(`Smart Calculator Bot - Version 1.0.0 Alpha - Developer: Acrisio SuperSS Dev
Comandos:
* !ping - Mostra a latência do Bot.
* !myinfo - Mostra seus dados de configuração e calculos que foram inseridos.
* !close - Fecha a sessão e excluí seus dados.
* !list - (Ex: !list club) - (type) {
	club: Mostra os clubes disponíveis e o seu índice para usar com o comando "!club";
	shot: Mostra as tacas disponíveis e o seu índice para usar com o comando "!shot";
	ps: Mostra os power shot disponíveis e o seu índice para usar com o comando "!ps".
}.
* !calc - Faz os calculo com os dados inseridos.

Obs: Para ver os comandos de inserção de dados use o comando !myinfo.

Obs: Depois de 5 minutos de inatividade seus dados serão excluídos.
`);
	}

	else if (command === "myinfo") {

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session`);

			return;
		}

		message.reply(`info:${user.getUser().toString()}`);
	}

	else if (command === "close") {
		mngrUser.deleteUser(message.author.id);
		message.reply(`Closed session`);
	}

	else if (command === 'list') {

		if (args.length <= 0)
			return;

		switch (args[0].toLowerCase()) {
			case 'club':
				message.reply(CUser.listAllClub());
				break;
			case 'shot':
				message.reply(CUser.listAllShot());
				break;
			case 'ps':
				message.reply(CUser.listAllPowerShot());
				break;
		}
	}

	else if (command === 'club') {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let index = Math.floor(parseFloat(args[0]));

		if (!CUser.checkClubIndex(index)) {

			message.reply('Invalid Club index');

			return;
		}

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.club_index = index;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command ===  "shot") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let index = Math.floor(parseFloat(args[0]));

		if (!CUser.checkShotIndex(index)) {

			message.reply('Invalid Shot index');

			return;
		}

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.shot_index = index;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "ps") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let index = Math.floor(parseFloat(args[0]));

		if (!CUser.checkPowerShotIndex(index)) {

			message.reply('Invalid Power Shot index');

			return;
		}

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.power_shot_index = index;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "power") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let power = Math.floor(parseFloat(args[0]));

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.power = power;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "ring") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let ring_pwr = Math.floor(parseFloat(args[0]));

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.auxpart_pwr = ring_pwr;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "mascot") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let mascot_pwr = Math.floor(parseFloat(args[0]));

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.mascot_pwr = mascot_pwr;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "card") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let card_pwr = Math.floor(parseFloat(args[0]));

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.card_pwr = card_pwr;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "card_ps") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let ps_card_pwr = Math.floor(parseFloat(args[0]));

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.ps_card_pwr = ps_card_pwr;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "d") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.distance = parseFloat(args[0]);

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "h") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.height = parseFloat(args[0]);

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "w") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.wind = parseFloat(args[0]);

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "a") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.degree = parseFloat(args[0]);

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "g") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let ground = parseFloat(args[0]);

		if (ground < 50.0 || ground > 100.0)
			ground = 100.0;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.ground = ground;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "s") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.spin = parseFloat(args[0]);

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "c") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.curve = parseFloat(args[0]);

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "b") {

		if (args.length <= 0)
			return;

		if (isNaN(args[0]))
			return;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.slope_break = parseFloat(args[0]);

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "make_slope") {

		if (args.length < 3)
			return;

		if (isNaN(args[0]) || isNaN(args[1]) || isNaN(args[2]))
			return;

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		user.slope_break = `${parseFloat(args[0])}, ${parseFloat(args[1])}, ${parseFloat(args[2])}`;

		// Print Info
		//message.reply(`info:${user.toString()}`);
	}

	else if (command === "calc") {

		let user = findOrCreateUser(message.author.id);

		if (user === undefined) {
			
			message.reply(`Fail to create session.`);

			return;
		}

		user = user.getUser();

		message.reply(CCalc(user));
	}
});

client.on('ready', () => {
	// Log
	console.log("Login bot OK!");
});

// Login with discord
client.login(config.BOT_TOKEN);
