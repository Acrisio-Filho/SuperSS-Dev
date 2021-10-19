const CLUB_INFO_ENUM = [
    '_1W',
    '_2W',
    '_3W',
    '_2I',
    '_3I',
    '_4I',
    '_5I',
    '_6I',
    '_7I',
    '_8I',
    '_9I',
    'PW',
    'SW',
    'PT1',
    'PT2'
]

const POWER_SHOT_FACTORY_ENUM = [
    'NO_POWER_SHOT',
    'ONE_POWER_SHOT',
    'TWO_POWER_SHOT',
    'ITEM_15_POWER_SHOT'
]

const SHOT_TYPE_ENUM = [
    'DUNK',
    'TOMAHAWK',
    'SPIKE',
    'COBRA'
]

class User {

    power = 0;

    auxpart_pwr = 0;
    mascot_pwr = 0;
    card_pwr = 0;
    ps_card_pwr = 0;

    club_index = 0;
    shot_index = 0;
    power_shot_index = 0;

    distance = 0.0;
    height = 0.0;
    wind = 0.0;
    degree = 0.0;
    ground = 100.0;
    spin = 0.0;
    curve = 0.0;
    slope_break = 0.0;

    constructor() {

        // Faz nada por enquanto
    }

    toString() {

        return `
Power
(Ex: !power 15) - Power: ${this.power}

Power extras
(Ex: !ring 2)       - Ring(Power): ${this.auxpart_pwr}
(Ex: !mascot 2)     - Mascot(Power): ${this.mascot_pwr}
(Ex: !card 4)       - Card(Power): ${this.card_pwr}
(Ex: !card_ps 2)    - Card Power Shot(Power): ${this.ps_card_pwr}

Shot
(Ex: !club 1)   - Club[${this.club_index}]: ${CLUB_INFO_ENUM[this.club_index]}
(Ex: !shot 2)   - Shot[${this.shot_index}]: ${SHOT_TYPE_ENUM[this.shot_index]}
(Ex: !ps 2)     - Power Shot[${this.power_shot_index}]: ${POWER_SHOT_FACTORY_ENUM[this.power_shot_index]}

Inputs
(Ex: !d 224.4)  - Distance: ${this.distance}
(Ex: !h -3.41)  - Height: ${this.height}
(Ex: !w 5)      - Wind: ${this.wind}
(Ex: !a 360)    - Degree: ${this.degree}
(Ex: !g 100)    - Ground: ${this.ground}
(Ex: !s 9)      - Spin: ${this.spin}
(Ex: !c 30)     - Curve: ${this.curve}
(Ex: !b -2.1)   - Slope Break: ${this.slope_break}`;
    }

    static listAllClub() {
        return CLUB_INFO_ENUM.filter(el => !el.includes('PT')).map((el, index) => {
            return `[${index}]: ${el}`;
        }).join('\n');
    }

    static listAllShot() {
        return SHOT_TYPE_ENUM.map((el, index) => {
            return `[${index}]: ${el}`;
        }).join('\n');
    }

    static listAllPowerShot() {
        return POWER_SHOT_FACTORY_ENUM.map((el, index) => {
            return `[${index}]: ${el}`;
        }).join('\n');
    }

    static checkClubIndex(index) {
        return CLUB_INFO_ENUM.filter(el => !el.includes('PT')).length > index;
    }

    static checkShotIndex(index) {
        return SHOT_TYPE_ENUM.length > index;
    }

    static checkPowerShotIndex(index) {
        return POWER_SHOT_FACTORY_ENUM.length > index;
    }
}

module.exports = User;