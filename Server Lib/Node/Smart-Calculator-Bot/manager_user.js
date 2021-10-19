
const eUSER_ENTRY_STATE = {
    
    UNDEFINED: -1,

    ACTIVE: 0,

    DELETING: 1
}

class UserEntry {

    // Marke state
    state = eUSER_ENTRY_STATE.UNDEFINED;

    id = undefined;
    lastUpdate = undefined;
    userObj = undefined;

    constructor(id, userObj) {

        this.id = id;
        this.userObj = userObj;

        this.lastUpdate = Date.now();

        this.state = eUSER_ENTRY_STATE.ACTIVE;

        // Log
        console.log(`[UserEntry::constructor] Usuário[ID=${this.id}] criado com sucesso.`);
    }

    getUser() {

        this.lastUpdate = Date.now();

        return this.userObj;
    }
}

class ManagerUser {

    users = [];
    timeout = undefined;
    max_time_clear = 300000/*5 minutos*/;

    // Time in milliseconds
    constructor(max_time_clear = 300000/*5 minutos*/) {

        this.max_time_clear = max_time_clear;

        this.timeout = setInterval(((manager) => {
            
            return () => {
                manager.clear();
            }
        })(this), this.max_time_clear);
    }

    destroy() {

        if (this.timeout !== undefined)
            clearInterval(this.timeout);
    }

    clear() {

        // Log
        console.log(`[ManagerUser::clear] Iniciando limpeza de usuários inátivos.`);

        if (this.users.length < 0)
            return;

        this.users.forEach((el) => {

            if ((el.lastUpdate + this.max_time_clear) < Date.now()) {

                el.state = eUSER_ENTRY_STATE.DELETING;

                // Log
                console.log(`[ManagerUser::clear] Usuário[ID=${el.id}] foi colocado na fila para ser deletado.`);
            }
        });

        this.users = this.users.filter((el) => {
            return el.state === eUSER_ENTRY_STATE.ACTIVE;
        });
    }

    // Add usuário
    addUser(id, userObj) {

        let userEntry = undefined;

        let userEntryIndex = this.users.findIndex((el) => {
            return el.id === id;
        });

        if (userEntryIndex !== -1) {

            if (this.users[userEntryIndex].state !== eUSER_ENTRY_STATE.ACTIVE)
                this.users[userEntryIndex] = new UserEntry(id, userObj);

            userEntry = this.users[userEntryIndex];

        } else {
            
            userEntry = new UserEntry(id, userObj);

            this.users.push(userEntry);
        }

        return userEntry;
    }

    // Delete usuário
    deleteUser(id) {

        let userIndex = this.users.findIndex((el) => {
            return el.id === id;
        });

        if (userIndex !== -1) {

            // Log
            console.log(`[ManagerUser::deleteUser] Usuário[ID=${this.users[userIndex].id}] deletado.`);

            this.users.splice(userIndex, 1);
        }
    }

    // Procura usuário
    findUser(id) {
        return this.users.find((el) => {
            return el.id === id && el.state === eUSER_ENTRY_STATE.ACTIVE;
        });
    }
}

module.exports = ManagerUser;