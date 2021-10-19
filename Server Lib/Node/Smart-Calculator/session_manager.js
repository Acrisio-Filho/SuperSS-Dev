
class SessionManager {

    constructor() {
        // Faz nada
    }

    static sessions = [];

    static findSession(socket) {

        let it = SessionManager.sessions.find((el) => {
            return el.id == (`${socket.remoteAddress}:${socket.remotePort}`);
        });

        return it;
    }

    static createSession(socket) {

        let session = SessionManager.findSession(socket);

        if (session !== undefined) {
            session.socket = socket;
            session.logged = false;
        }else {

            session = {
                id: `${socket.remoteAddress}:${socket.remotePort}`,
                socket: socket,
                logged: false,
                uid: 0,                 // UID do server
                data: Buffer.from('')
            };

            SessionManager.sessions.push(session);
        }
    }

    static closeSession(socket) {

        const session = SessionManager.findSession(socket);

        if (session !== undefined) {

            SessionManager.sessions.splice(
                SessionManager.sessions.findIndex((el) => {
                    return el.id == session.id
                }),
                1
            )
        }
    }
}

module.exports = SessionManager;