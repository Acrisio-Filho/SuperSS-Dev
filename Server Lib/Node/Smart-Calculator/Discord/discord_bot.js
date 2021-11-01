const Discord = require('discord.js');
const config = require('./config.json');
const { logDate } = require('../util')

// Perfix command
const prefix = '!';

// Channel name
const CHANNEL_CHAT_HISTORY_ID = 'Seu canal id';

// Class
class Bot {

    client = undefined;
    message_callback = null;
    state = false;
    actived = false;

    constructor(actived = true) {

        this.state = false;
        this.actived = actived;
    }

    start() {

        if (!this.actived) {

            console.log(`${logDate()} Bot is desactived, can't start it.`);

            return false;
        }

        if (this.message_callback == null || !(this.message_callback instanceof Function)) {

            console.log(`${logDate()} Fail start bot discord, message callback is invalid.`);

            return false;
        }

        // Desconecta se já estiver connectado
        if (this.client !== undefined && this.client.ws.status != 0)
            this.client.destroy();

        // Make Object Discord client
        this.client = new Discord.Client();

        // on Message
        this.client.on('message', ((_main) => {
            
            return function (message) {
            
                // Verifica primeiro depois manda para o callback de tratamento de comandos definido externamente
                if (message.author.bot) return;
                if (!message.content.startsWith(prefix)) return;
                if (!message.member.hasPermission('ADMINISTRATOR')) return;

                const commandBody = message.content.slice(prefix.length);
                const args = commandBody.split(' ');
                const command = args.shift().toLowerCase();

                // Chama o callback
                _main.message_callback(command, args);
            }

        })(this));

        // on connected
        this.client.on('ready', () => {

            // state
            this.state = true;

            // Log
            console.log(logDate() + " Login bot OK!");
        });

        // Login with discord
        this.client.login(config.BOT_TOKEN).then( () => {
            console.log(`${logDate()} Login OK`);
        }).catch(err => {
            console.log(`${logDate()} ${err}`);
        });

        return true;
    }

    sendMessageToChannelChatHistory(message) {

        this.client.channels.fetch(CHANNEL_CHAT_HISTORY_ID)
        .then(channel => {

            channel.send(message);

        })
        .catch(err => {

            console.log(`${logDate()} ${err}`);
        });
    }

    close() {

        if (this.client !== undefined)
            this.client.destroy();

        this.state = false;
    }
}

// Singleton return Object of class Bot
module.exports = new Bot();