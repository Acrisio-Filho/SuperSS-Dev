// Arquivo APIErrorException.js
// Criado em 25/07/2020 as 09:30 por Acrisio
// Definição da classe Exception da API

class APIErrorException extends Error {

    constructor(message, code) {

        super(message);

        this.code = (code === undefined || code === null || isNaN(code)) ? 0 : code;
    }
}

exports.APIErrorException = APIErrorException;