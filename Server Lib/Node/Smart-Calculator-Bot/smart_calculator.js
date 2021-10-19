const DEBUG_FLAG = false;

const DESVIO_SCALE_PANGYA_TO_YARD = 0.3125 / 1.5;

class Vector3D {

    constructor(x, y, z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    normalize() {
        return this.divideScalar( this.length() );
    }

    multiplyScalar(value) {

        this.x *= value;
        this.y *= value;
        this.z *= value;

        return this;
    }

    add(vector3d) {

        this.x += vector3d.x;
        this.y += vector3d.y;
        this.z += vector3d.z;

        return this;
    }

    add3D(x, y, z) {

        this.x += x;
        this.y += y;
        this.z += z;

        return this;
    }

    sub(vector3d) {

        this.x -= vector3d.x;
        this.y -= vector3d.y;
        this.z -= vector3d.z;

        return this;
    }

    sub3D(x, y, z) {

        this.x -= x;
        this.y -= y;
        this.z -= z;

        return this;
    }

    divideScalar(value) {

        if (value != 0) {

            let scalar = 1 / value;

            this.x *= scalar;
            this.y *= scalar;
            this.z *= scalar;

        }else {

            this.x = 0.0;
            this.y = 0.0;
            this.z = 0.0;
        }

        return this;
    }

    cross(vector3d) {

        let x = this.x, y = this.y, z = this.z;
    
        this.x = y * vector3d.z - z * vector3d.y;
        this.y = z * vector3d.x - x * vector3d.z;
        this.z = x * vector3d.y - y * vector3d.x;
    
        return this;
    
      }

    length() {
        return Math.sqrt((this.x * this.x) + (this.y * this.y) + (this.z * this.z));
    }

    clone() {
        return new Vector3D(this.x, this.y, this.z);
    }
}

const TYPE_DISTANCE = {
    LESS_10: 0,
    LESS_15: 1,
    LESS_28: 2,
    LESS_58: 3,
    BIGGER_OR_EQUAL_58: 4,
}

function calculeTypeDistance(distance) {

    let type = TYPE_DISTANCE.BIGGER_OR_EQUAL_58;

    if (distance >= 58.0)
        return TYPE_DISTANCE.BIGGER_OR_EQUAL_58;
    else if (distance < 10.0)
        return TYPE_DISTANCE.LESS_10;
    else if (distance < 15.0)
        return TYPE_DISTANCE.LESS_15;
    else if (distance < 28.0)
        return TYPE_DISTANCE.LESS_28;
    else if (distance < 58.0)
        return TYPE_DISTANCE.LESS_58; // Esse não precisa já que ele passou do primeiro if, mas deixar assim

    return type;
}

function calculeTypeDistanceByPosition(position1, position2) {
    return calculeTypeDistance(Math.sqrt(Math.pow(position1.x - position2.x) + Math.pow(position1.z - position2.z)) * 0.3125);
}

// JP Base ball Static Object, 0x00E47E30
class Ball {

    position = new Vector3D(0.0, 0.0, 0.0);

    slope = new Vector3D(0.0, 1.0, 0.0);

    // Ball Flag state process
    state_process = 0;  // (0x00E47E30+0x04) 0x00E47E34

    max_height = 0.0;
    num_max_height = -1;

    count = 0;

    // Velocity
    velocity = new Vector3D(0.0, 0.0, 0.0);
    /*
    ball_0C = 0.0;  // (0x00E47E30+0x0C) 0x00E47E3C
    ball_10 = 0.0;  // (0x00E47E30+0x10) 0x00E47E40
    ball_14 = 0.0;  // (0x00E47E30+0x14) 0x00E47E44
    */

    ball_28 = 0.0;  // (0x00E47E30+0x28) 0x00E47E58
    ball_2C = 0.0;  // (0x00E47E30+0x2C) 0x00E47E5C
    ball_30 = 0.0;  // (0x00E47E30+0x30) 0x00E47E60

    curva = 0.0;    // (0x00E47E30+0x34) 0x00E47E64
    spin = 0.0;     // (0x00E47E30+0x38) 0x00E47E68

    rotation_curve = 0.0;  // (0x00E47E30+0x3C) 0x00E47E6C - rotation curva
    rotation_spin = 0.0;  // (0x00E47E30+0x40) 0x00E47E70 - rotation spin

    // Flags
    ball_44 = 0;    // (0x00E47E30+0x44) 0x00E47E74 - esse valor é setado em algum lugar
    ball_48 = 0;    // (0x00E47E30+0x48) 0x00E47E78 - esse valor na inicialização é o mesmo do ball_44
    ball_70 = -1;   // (0x00E47E30+0x70) 0x00E47EA0
    ball_90 = 0;    // (0x00E47E30+0x90) 0x00E47EC0

    ball_BC = 0;    // (0x00E47E30+0xBC) 0x00E47EEC

    mass = 0.045926999;  // 0x00E47E30+0xC4) 0x00E47EF4 // Mass
    diametro = 0.14698039;   // 0x00E47E30+0xC8) 0x00E47EF8 // diametro

    copy(other) {

        let cpy = this;

        cpy.position = other.position.clone();
        cpy.slope = other.slope.clone();
        cpy.velocity = other.velocity.clone();
        cpy.state_process = other.state_process;
        cpy.max_height = other.max_height;
        cpy.spin = other.spin;
        cpy.curva = other.curva;
        cpy.count = other.count;
        cpy.num_max_height = other.num_max_height;
        cpy.ball_28 = other.ball_28;
        cpy.ball_2C = other.ball_2C;
        cpy.ball_30 = other.ball_30;
        cpy.ball_3C = other.ball_3C;
        cpy.ball_40 = other.ball_40;
        cpy.ball_44 = other.ball_44;
        cpy.ball_48 = other.ball_48;
        cpy.ball_70 = other.ball_70;
        cpy.ball_90 = other.ball_90;
        cpy.ball_BC = other.ball_BC;
        cpy.ball_C4 = other.ball_C4;
        cpy.ball_C8 = other.ball_C8;
    }
}

const POWER_SHOT_FACTORY = {
    NO_POWER_SHOT: 0,
    ONE_POWER_SHOT: 1,
    TWO_POWER_SHOT: 2,
    ITEM_15_POWER_SHOT: 3
}

function getPowerShotFactory(ps) {

    let power_shot_factory = 0.0;

    switch (ps) {
        case POWER_SHOT_FACTORY.ONE_POWER_SHOT:
            power_shot_factory = 10.0;
            break;
        case POWER_SHOT_FACTORY.TWO_POWER_SHOT:
            power_shot_factory = 20.0;
            break;
        case POWER_SHOT_FACTORY.ITEM_15_POWER_SHOT:
            power_shot_factory = 15.0;
            break;
    }

    return power_shot_factory
}

const CLUB_TYPE = {
    WOOD: 0,
    IRON: 1,
    PW: 2, // SW e PW, não vou fazer esse que ele recalcula a força dele pela distância, TYPE[0= ( < 10y), 1= ( < 15y), 2= ( < 28y), 3= ( < 58y), 4= ( >= 58y)], POWER(SW, PW)[0= (30y, 30y), 1= (30y, 30y), 2= (30y, 30y), 3= (60y, 60y), 4= (80y, 100y)] + cards e etc.
    PT: 3
}

class ClubInfo {

    constructor(type, rotation_spin, rotation_curve, power_factor, degree, power_base) {

        this.type = type;
        this.rotation_spin = rotation_spin;
        this.rotation_curve = rotation_curve;
        this.power_factor = power_factor;
        this.degree = degree;
        this.power_base = power_base;
    }
}


const CLUB_INFO = {
    _1W: new ClubInfo(CLUB_TYPE.WOOD,  0.55, 1.61, 236.0, 10.0, 230.0),
    _2W: new ClubInfo(CLUB_TYPE.WOOD,  0.50, 1.41, 204.0, 13.0, 210.0),
    _3W: new ClubInfo(CLUB_TYPE.WOOD,  0.45, 1.26, 176.0, 16.0, 190.0),
    _2I: new ClubInfo(CLUB_TYPE.IRON,  0.45, 1.07, 161.0, 20.0, 180.0),
    _3I: new ClubInfo(CLUB_TYPE.IRON,  0.45, 0.95, 149.0, 24.0, 170.0),
    _4I: new ClubInfo(CLUB_TYPE.IRON,  0.45, 0.83, 139.0, 28.0, 160.0),
    _5I: new ClubInfo(CLUB_TYPE.IRON,  0.45, 0.73, 131.0, 32.0, 150.0),
    _6I: new ClubInfo(CLUB_TYPE.IRON,  0.41, 0.67, 124.0, 36.0, 140.0),
    _7I: new ClubInfo(CLUB_TYPE.IRON,  0.36, 0.61, 118.0, 40.0, 130.0),
    _8I: new ClubInfo(CLUB_TYPE.IRON,  0.30, 0.57, 114.0, 44.0, 120.0),
    _9I: new ClubInfo(CLUB_TYPE.IRON,  0.25, 0.53, 110.0, 48.0, 110.0),
    PW: new ClubInfo(CLUB_TYPE.PW,     0.18, 0.49, 107.0, 52.0, 100.0),
    SW: new ClubInfo(CLUB_TYPE.PW,     0.17, 0.42, 93.0, 56.0, 80.0),
    PT1: new ClubInfo(CLUB_TYPE.PT,    0.00, 0.00, 30.0, 0.00, 20.0),
    PT2: new ClubInfo(CLUB_TYPE.PT,    0.00, 0.00, 21.0, 0.00, 10.0)
}

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

class Club {

    type = CLUB_TYPE.WOOD; // 1W

    type_distance = TYPE_DISTANCE.BIGGER_OR_EQUAL_58;

    // 1W
    rotation_spin =     0.55;      // Rotação spin
    rotation_curve =    1.61;       // Rotação curva
    power_factor =      236;       // Power shot
    degree =            10;        // Angulo
    power_base =        230;       // Base power

    init(club_info) {

        this.type = club_info.type;

        this.rotation_spin  = club_info.rotation_spin;
        this.rotation_curve = club_info.rotation_curve;
        this.power_factor   = club_info.power_factor;
        this.degree         = club_info.degree;
        this.power_base     = club_info.power_base;
    }

    getDregRad() {
        return this.degree * Math.PI / 180;
    }

    getPower(extraPower, pwrSlot, ps, spin) {

        // Get Auxpart
        // Get Card
        // Get Mascot
        let pwrjard = 0.0;

        switch(this.type) {
            case CLUB_TYPE.WOOD: {

                pwrjard = extraPower.total(ps) + getPowerShotFactory(ps) + ((pwrSlot - 15) * 2);

                pwrjard *= 1.5;
                pwrjard /= this.power_base;
                pwrjard += 1;
                pwrjard *= this.power_factor;

                break;
            }
            case CLUB_TYPE.IRON: {

                pwrjard = ((getPowerShotFactory(ps) / this.power_base + 1.0) * this.power_factor) + (extraPower.total(ps) * this.power_factor * 1.3) / this.power_base;

                break;
            }
            case CLUB_TYPE.PW: { // SW e PW

                const getPowerByDegree = (degree, spin) => {
                    return 0.5 + ((0.5 * (degree + (spin * _00D19B98))) / (56/*Ang*/ / 180 * Math.PI));
                }

                switch (this.type_distance) {
                    case TYPE_DISTANCE.LESS_10:
                    case TYPE_DISTANCE.LESS_15:
                    case TYPE_DISTANCE.LESS_28:
                        pwrjard = (getPowerByDegree(this.getDregRad(), spin) * (52.0 + (ps ? 28.0 : 0))) + (extraPower.total(ps) * this.power_factor) / this.power_base;
                        break;
                    case TYPE_DISTANCE.LESS_58:
                        pwrjard = (getPowerByDegree(this.getDregRad(), spin) * (80.0 + (ps ? 18.0 : 0))) + (extraPower.total(ps) * this.power_factor) / this.power_base;
                        break;
                    case TYPE_DISTANCE.BIGGER_OR_EQUAL_58:
                        pwrjard = ((getPowerShotFactory(ps) / this.power_base + 1.0) * this.power_factor) + (extraPower.total(ps) * this.power_factor) / this.power_base;
                        break;
                }

                break;
            }
            case CLUB_TYPE.PT: {
                pwrjard = this.power_factor;
                break;
            }
        }

        return pwrjard;
    }

    // Rotation
    getPower2(extraPower, pwrSlot, ps) {


        let pwrjard = (extraPower.auxpart + extraPower.mascot + extraPower.card) / 2 + (pwrSlot - 15);

        if (ps)
            pwrjard += (extraPower.ps_card / 2);

        pwrjard /= 170;

        return pwrjard + 1.5;
    }

    getRange(extraPower, pwrSlot, ps) {
        
        let pwr_range = this.power_base + extraPower.total(ps) + getPowerShotFactory(ps);

        if (this.type == CLUB_TYPE.WOOD)
            pwr_range += ((pwrSlot - 15) * 2)

        if (this.type == CLUB_TYPE.PW) {

            switch (this.type_distance) {
                case TYPE_DISTANCE.LESS_10:
                case TYPE_DISTANCE.LESS_15:
                case TYPE_DISTANCE.LESS_28:
                    pwr_range = 30.0 + (ps ? 30.0 : 0.0) + extraPower.total(ps)
                    break;
                case TYPE_DISTANCE.LESS_58:
                    pwr_range = 60.0 + (ps ? 20.0 : 0.0) + extraPower.total(ps)
                    break;
                case TYPE_DISTANCE.BIGGER_OR_EQUAL_58:
                    pwr_range = this.power_base + extraPower.total(ps) + getPowerShotFactory(ps);
                    break;
            }
        }

        return pwr_range;
    }
}

class Wind {

    wind = 0;
    degree = 0;

    getWind() {
        return new Vector3D(this.wind * Math.sin(this.degree * Math.PI / 180) * -1, 0, this.wind * Math.cos(this.degree * Math.PI / 180));
    }
}

// const Object ball
const ball = new Ball();

// const club
const club = new Club();

// const wind
const wind = new Wind();

// Const values
const _00D3D008 = 0.00001; // compare with 0 acho
const _00D046A8 = -1.0; // -1 value constant
const _00D00190 = 0.75; // const
const _00D083A0 = 0.02; // Step time, pangya 0.02
const _00D66CF8 = 3.0;  // Valor 3 que não sei bem o que é
const _00D3D028 = 0.00008; // Efeito Mangnus acho
const _00D1A888 = 12.566371;  // Não sei
const _00D3D210 = 25.132742; // Não sei
const _00CFF040 = 0.1;
const _00D66CA0 = 0.5;
const _00D16928 = 0.002;
const _00D17908 = 0.349065847694874;
const _00D19B98 = 0.0698131695389748;
const _00D16758 = 0.01

const slope_break_to_curve_slope = 0.00875;

const _00E42544_vect_slope = new Vector3D(0.0, 0.0, 1.0);

class QuadTree {

    constructor() {
        this.ball = new Ball();
        this.club = new Club();
        this.wind = new Wind();
    }

    gravityFactor = 1;
    gravity = 34.295295715332; //9.8 * 1.094 * 3.2/*/0.3125*/ */; /// 0.3125; // gravity in Yards(scale pangya)

    //_21D8 = 0.0; // Não sei bem o que é ainda
    _21D8_vect = new Vector3D(0.0, 0.0, 0.0);

    getGravity() {
        return this.gravity * this.gravityFactor;
    }

    ball = undefined;

    club = undefined;

    wind = undefined;

    // Usa no cobra
    ball_position_init = new Vector3D(0.0, 0.0, 0.0);
    power_range_shot = 0.0;

    shot = SHOT_TYPE.DUNK;
    power_factor_shot = 0.0;
    percentShot_sqrt = 0.0;
    spike_init = -1;
    spike_med = -1;
    power_factor = 0.0;
    cobra_init = -1;

    // Init Shot
    initShot(ball, club, wind, options) {

        this.ball = ball;
        this.club = club;
        this.wind = wind;

        this.shot = options.shot;
        this.spike_init = -1;
        this.spike_med = -1;
        this.cobra_init = -1;

        this.ball.position = options.position.clone();

        // Usa no Cobra
        this.ball_position_init = options.position.clone();

        // Type distance
        this.club.type_distance = calculeTypeDistance(options.distance);

        // init max_height
        this.ball.max_height = this.ball.position.y;

        this.ball.count = 0;
        this.ball.num_max_height = -1;

        let pwr = this.club.getPower(options.power.options, options.power.pwr, options.ps, options.spin);

        // Guarda para usa no cobra
        this.power_range_shot = this.club.getRange(options.power.options, options.power.pwr, options.ps);

        // Guarda para usar no spike
        this.power_factor = pwr;

        pwr *= Math.sqrt(options.percentShot);

        // Multiplica por 1.0 ou 1.3
        if (options.shot == SHOT_TYPE.TOMAHAWK || options.shot == SHOT_TYPE.SPIKE/*Toma e spike */)
            pwr *= 1.3;
        else
            pwr *= 1.0;

        // Percent Erro de pangya e ground
        pwr *= Math.sqrt(options.ground * 0.01);

        // Guarda para usar no spike
        this.power_factor_shot = pwr;
        this.percentShot_sqrt = Math.sqrt(options.percentShot);

        this.ball.curva = options.curva;
        this.ball.spin = options.spin;

        let value1 = this.getValuesDegree(options.mira_rad + (0 - (this.ball.curva * _00D17908)), 1);
        let value2 = this.getValuesDegree((this.club.type_distance == TYPE_DISTANCE.BIGGER_OR_EQUAL_58 ? this.club.getDregRad() : this.club.getDregRad() + (this.ball.spin * _00D19B98)), 0);

        this.ball.curva -= this.getSlope(options.mira_rad - options.slope_mira_rad, Math.random()/* Rotation Ball Line */);

        pwr *= ((Math.abs(this.ball.curva) * 0.1) + 1);

        // tava cos2, neg_rad, neg_sin
        let vectA = new Vector3D(value2.neg_sin, value2.neg_rad, value2.cos2);

        // !@ Teste
        if (DEBUG_FLAG)
            console.log(`Power final: ${pwr}`);

        vectA.multiplyScalar(pwr);

        // !@ debug
        if (DEBUG_FLAG)
            console.log(vectA);

        let v1 = new Vector3D(value1.cos, value1.rad, value1.sin);
        let v2 = new Vector3D(value1._C, value1._10, value1._14);
        let v3 = new Vector3D(value1.neg_sin, value1.neg_rad, value1.cos2);
        let v4 = new Vector3D(value1._24, value1._28, value1._2C);

        this.ball.velocity.x = v2.x * vectA.y + vectA.x * v1.x + v3.x * vectA.z + v4.x;
        this.ball.velocity.y = v1.y * vectA.x + v2.y * vectA.y + v3.y * vectA.z + v4.y;
        this.ball.velocity.z = v1.z * vectA.x + v2.z * vectA.y + v3.z * vectA.z + v4.z;

        // Rotação eixo X, Z
        this.ball.rotation_curve = this.ball.curva * options.percentShot;
        this.ball.rotation_spin = this.club.type_distance == TYPE_DISTANCE.BIGGER_OR_EQUAL_58 
            ? (this.club.getPower2(options.power.options, options.power.pwr, options.ps) * options.percentShot) * options.percentShot
            : 0.0;

        // !@ Log
        if (DEBUG_FLAG)
            console.log(`Spin Rotation: ${this.ball.rotation_spin}`);

        this.ball.ball_48 = this.ball.ball_44; // Flag Power Shot

        // Log
        if (DEBUG_FLAG)
            console.log(`Vel.X: ${this.ball.velocity.x}, Vel.Y: ${this.ball.velocity.y}, Vel.Z: ${this.ball.velocity.z}`);
    }

    getSlope(mira, line_ball) {

        // values Degree To Matrix
        const valuesDegreeToMatrix = (value) => {

            return {
                v1: new Vector3D(value.cos, value.rad, value.sin),
                v2: new Vector3D(value._C, value._10, value._14),
                v3: new Vector3D(value.neg_sin, value.neg_rad, value.cos2),
                v4: new Vector3D(value._24, value._28, value._2C)
            }
        }

        // Matrix cross Matrix
        const applyMatrix = (m1, m2) => {

            return {
                v1: new Vector3D(
                    m1.v1.x * m2.v1.x + m1.v1.y * m2.v2.x + m1.v1.z * m2.v3.x,
                    m1.v1.x * m2.v1.y + m1.v1.y * m2.v2.y + m1.v1.z * m2.v3.y,
                    m1.v1.x * m2.v1.z + m1.v1.y * m2.v2.z + m1.v1.z * m2.v3.z
                ),
                v2: new Vector3D(
                    m1.v2.x * m2.v1.x + m1.v2.y * m2.v2.x + m1.v2.z * m2.v3.x,
                    m1.v2.x * m2.v1.y + m1.v2.y * m2.v2.y + m1.v2.z * m2.v3.y,
                    m1.v2.x * m2.v1.z + m1.v2.y * m2.v2.z + m1.v2.z * m2.v3.z
                ),
                v3: new Vector3D(
                    m1.v3.x * m2.v1.x + m1.v3.y * m2.v2.x + m1.v3.z * m2.v3.x,
                    m1.v3.x * m2.v1.y + m1.v3.y * m2.v2.y + m1.v3.z * m2.v3.y,
                    m1.v3.x * m2.v1.z + m1.v3.y * m2.v2.z + m1.v3.z * m2.v3.z
                ),
                v4: new Vector3D(
                    m1.v4.x * m2.v1.x + m1.v4.y * m2.v2.x + m1.v4.z * m2.v3.x + m2.v4.x,
                    m1.v4.x * m2.v1.y + m1.v4.y * m2.v2.y + m1.v4.z * m2.v3.y + m2.v4.y,
                    m1.v4.x * m2.v1.z + m1.v4.y * m2.v2.z + m1.v4.z * m2.v3.z + m2.v4.z
                )
            };
        };

        // Calc slope
        let ball_slope_cross_const_vect = this.ball.slope.clone().cross(_00E42544_vect_slope);

        let slope_matrix = {
            v1: ball_slope_cross_const_vect.clone().normalize(),
            v2: this.ball.slope.clone(),
            v3: ball_slope_cross_const_vect.clone().cross(this.ball.slope).normalize(),
            v4: new Vector3D(0.0, 0.0, 0.0)
        };

        let value1 = this.getValuesDegree(mira * -1, 1);
        let value2 = this.getValuesDegree(line_ball * -2.0, 1);

        let m1 = applyMatrix(valuesDegreeToMatrix(value2), slope_matrix);

        let m2 = applyMatrix(m1, valuesDegreeToMatrix(value1));

        // Log
        if (DEBUG_FLAG) {

            console.log('slope_matrix: ', slope_matrix);
            console.log('value1: ', value1);
            console.log('value2: ', value2);
            console.log('value1 Matrix: ', valuesDegreeToMatrix(value1));
            console.log('value2 Matrix: ', valuesDegreeToMatrix(value2));
            console.log('m1: ', m1);
            console.log('m2: ', m2);
        }

        return m2.v2.x * _00D66CA0;
    }

    getValuesDegree(degree, option = 0) {

        // !@ Debug
        if (DEBUG_FLAG)
            console.log(`degree rad: ${degree}`);

        let obj = new Object();

        if (option == 0) {

            obj.cos = 1.0;
            obj.rad = 0.0; // degree
            obj.sin = 0.0;
            obj._C = 0.0;
            obj._10 = Math.cos(degree);
            obj._14 = Math.sin(degree) * -1;
            obj.neg_sin = 0.0;
            obj.neg_rad = Math.sin(degree);
            obj.cos2 = obj._10;
            obj._24 = 0.0;
            obj._28 = 0.0;
            obj._2C = 0.0;

        }else if (option == 1) {
        
            obj.cos = Math.cos(degree);
            obj.rad = 0.0; // degree
            obj.sin = Math.sin(degree);
            obj._C = 0.0;
            obj._10 = 1.0;
            obj._14 = 0.0;
            obj.neg_sin = obj.sin * -1;
            obj.neg_rad = 0.0;
            obj.cos2 = obj.cos;
            obj._24 = 0.0;
            obj._28 = 0.0;
            obj._2C = 0.0;
        }

        // !@ debug
        if (DEBUG_FLAG)
            console.log(obj);

        return obj;
    }

    ballProcess(steptime, final = undefined) {

        this.bounceProcess(steptime, final);

        // Cobra
        if (this.shot == SHOT_TYPE.COBRA && this.cobra_init < 0) {

            if (this.percentShot_sqrt < Math.sqrt(0.8))
                this.percentShot_sqrt = Math.sqrt(0.8); // 80%

            if (this.ball.count == 0) {

                this.ball.velocity.y = 0.0;

                this.ball.velocity.normalize().multiplyScalar(this.power_factor_shot);
            }

            let diff = this.ball.position.clone().sub(this.ball_position_init).length();
            let cobra_init_up = ((this.power_range_shot * this.percentShot_sqrt) - 100.0) * 3.2;

            if (diff >= cobra_init_up) {

                // 1W, 2W e 3W
                let power_multiply = 0.0;
                
                if (this.club.type == CLUB_TYPE.WOOD) {

                    switch (this.club.power_base) {
                        case 230.0:
                            power_multiply = 74.0;
                            break;
                        case 210.0:
                            power_multiply = 76.0;
                            break;
                        case 190.0:
                            power_multiply = 80.0;
                            break;
                    }
                }

                this.cobra_init = this.ball.count;

                this.ball.velocity.normalize().multiplyScalar(power_multiply).multiplyScalar(this.percentShot_sqrt);

                this.ball.rotation_spin = 2.5;
            }

        }else {

            if (this.spike_init < 0 && this.cobra_init < 0 && this.club.type_distance == TYPE_DISTANCE.BIGGER_OR_EQUAL_58) {

                this.ball.rotation_spin -= ((_00D66CA0 - (this.ball.spin * _00CFF040)) * _00D083A0);
                //else quando a bola kika
                //    ball.ball_40 -= _00D16928;

            }else if ((this.shot == SHOT_TYPE.SPIKE && this.spike_init >= 0) || (this.shot == SHOT_TYPE.COBRA && this.cobra_init >= 0))
                this.ball.rotation_spin -= _00D083A0;

            if (this.shot == SHOT_TYPE.SPIKE && this.ball.count == 0) {

                this.ball.velocity.y = 0.0;
                this.ball.velocity.normalize().multiplyScalar(this.power_factor_shot);

                // check se a bola andou já
                this.ball.velocity.normalize().multiplyScalar(72.5).multiplyScalar(this.percentShot_sqrt * 2);

                this.ball.rotation_spin = 3.1;

                this.spike_init = this.ball.count;

            }

            if (this.shot == SHOT_TYPE.SPIKE && this.ball.num_max_height >= 0 && (this.ball.num_max_height + 0x3C) < this.ball.count && this.spike_med < 0) {
                
                this.spike_med = this.ball.count;

                if (this.club.type == CLUB_TYPE.WOOD) {

                    let new_power = 0.0;

                    switch (this.club.power_base) {
                        case 230.0:

                            new_power = 344.0;

                            if ((this.power_factor * this.percentShot_sqrt) < 344.0)
                                new_power -= (this.power_factor * this.percentShot_sqrt);
                            else
                                new_power = 0.0;

                            new_power = new_power / 112.0 * 21.5;

                            new_power = -8 - new_power;

                            this.ball.velocity.y = new_power;

                            break;
                        case 210.0:

                            new_power = 306.0;

                            if ((this.power_factor * this.percentShot_sqrt) < 306.0)
                                new_power -= (this.power_factor * this.percentShot_sqrt);
                            else
                                new_power = 0.0;

                            new_power = new_power / 105.0 * 20.5;

                            new_power = -10.3 - new_power;

                            this.ball.velocity.y = new_power;

                            break;
                        case 190.0:

                            new_power = 273.0;

                            if ((this.power_factor * this.percentShot_sqrt) < 273.0)
                                new_power -= (this.power_factor * this.percentShot_sqrt);
                            else
                                new_power = 0.0;

                            new_power = new_power / 100 * 20.2;

                            new_power = -10.8 - new_power;

                            this.ball.velocity.y = new_power;

                            break;
                    }
                }

                this.ball.velocity.multiplyScalar(this.percentShot_sqrt * 7);

                this.ball.rotation_spin = this.ball.spin;
            }
        }

        // if (this.ball.position.y > this.ball.max_height)
        //     this.ball.max_height = this.ball.position.y;
        // else if (this.ball.num_max_height == -1)
        //     this.ball.num_max_height = this.ball.count;

        if (this.ball.velocity.y < 0 && this.ball.num_max_height < 0) {

            this.ball.max_altura = this.ball.position.y;
            this.ball.num_max_height = this.ball.count;
        }
        
        this.ball.count++;
    }
    
    bounceProcess(steptime, final = undefined) {

        if (this.shot == SHOT_TYPE.SPIKE && this.ball.num_max_height >= 0 && (this.ball.num_max_height + 0x3C) > this.ball.count)
            return;

        let accellVect = this.applyForce();

        let otherVect = accellVect.clone();

        otherVect.divideScalar(this.ball.mass).multiplyScalar(steptime);

        this.ball.velocity.add(otherVect);

        // check
        if (this.ball.ball_70 == -1)
            if (DEBUG_FLAG)
                console.log("ball.ball_70 == -1");

        if (this.ball.num_max_height == -1) {

            let tmpVect = this._21D8_vect.clone().divideScalar(this.ball.mass).multiplyScalar(steptime);

            this.ball.velocity.add(tmpVect);
        }

        this.ball.ball_2C += (this.ball.rotation_curve * _00D1A888 * steptime);

        this.ball.ball_30 += (this.ball.rotation_spin * _00D3D210 * steptime);

        this.ball.position.add(this.ball.velocity.clone().multiplyScalar((final !== undefined ? final : steptime)));

    }

    applyForce() {

        let retVect = new Vector3D(0.0, 0.0, 0.0);

        if (this.ball.rotation_curve >= _00D3D008)
            if (DEBUG_FLAG)
                console.log("[00D3D008] < ball_3C");

        if (this.ball.ball_90 == 0)
            if (DEBUG_FLAG)
                console.log("ball_90 == 0");

        if (this.ball.rotation_curve != 0) {

            let vectorb = new Vector3D(this.ball.velocity.z * _00D046A8, 0, this.ball.velocity.x);

            vectorb.normalize();

            if (this.cobra_init < 0 || this.spike_init < 0)
                vectorb.multiplyScalar(_00D00190 * this.ball.rotation_curve * this.club.rotation_curve);

            retVect.add(vectorb);
        }

        // !@ Teste primeira vez
        if (this.shot == SHOT_TYPE.SPIKE && this.spike_init < 0)
            return new Vector3D(0.0, 0.0, 0.0);
        else if (this.shot == SHOT_TYPE.COBRA && this.cobra_init < 0)
            return retVect;

        let windVect = this.wind.getWind();

        windVect.multiplyScalar((this.shot == SHOT_TYPE.SPIKE ? _00D16758 : _00D083A0));

        retVect.add(windVect);

        retVect.y = retVect.y - (this.getGravity() * this.ball.mass);

        if (DEBUG_FLAG)
            console.log(retVect.y);

        if (this.ball.rotation_spin != 0)
            retVect.y = retVect.y + (this.club.rotation_spin * _00D66CF8 * this.ball.rotation_spin);

        if (DEBUG_FLAG)
            console.log(retVect.y);

        let velVect = this.ball.velocity.clone();
        
        velVect.multiplyScalar(velVect.length() * _00D3D028);

        retVect.sub(velVect);

        // return
        return retVect;
    }
}

const diffYZ = function(vect1, vect2) {
    return Math.sqrt(Math.pow(vect1.x - vect2.x, 2) + Math.pow(vect1.z - vect2.z, 2));
};

const find_power = (power_player, club_info, shot, power_shot, distancia, altura, vento, angulo, terreno, spin, curva, slope, mira = undefined, percent = undefined) => {

    const altura_colision = altura * 1.094 * 3.2/*0.3125*/ /*- 0.14/*Cup*/;
    const distanciaScale = distancia * 3.2/*0.3125*/
    var vball = new Ball();
    const vclub = club;

    // init Club Info
    if (club != undefined)
        vclub.init(club_info);

    // Calcule type distance
    vclub.type_distance = calculeTypeDistance(distancia);

    let slope_mira_rad = 0.0;

    if (slope instanceof Vector3D) {

        slope_mira_rad = slope.y;
        vball.slope = slope.clone();

        vball.slope.y = 1.0;

    }else if (!isNaN(slope))
        vball.slope = new Vector3D(slope * slope_break_to_curve_slope * -1, 1.0, 0.0);

    // !@ Log
    if (DEBUG_FLAG)
        console.log(`Find: Altura: ${altura_colision}, Distância: ${distanciaScale}`);

    // !@ Log
    if (DEBUG_FLAG)
        console.log(`Mira Degree Rad: ${mira || 0}`);

    const margin = 0.05;
    const limit_checking = 1000;
    let count = 0;
    let isFind = false;
    let found = {
        power: -1,
        desvio: 0.0
    }

    wind.wind = vento;
    wind.degree = angulo;

    let wind_vect = wind.getWind();

    if (DEBUG_FLAG)
        console.log(`Influência do vento: X: ${wind_vect.x}, Y: ${wind_vect.y}, Z: ${wind_vect.z}`);

    const options = {
        distance: distancia,
        percentShot: percent || 1.0,
        ground: terreno,
        mira_rad: mira || 0.0,
        slope_mira_rad: slope_mira_rad,
        spin: spin/30,
        curva: curva/30,
        position: new Vector3D(0.0, 0.0, 0.0),
        shot: shot,
        ps: power_shot,
        power: power_player || {
            pwr: 31,
            options: {
                auxpart: 0,
                mascot: 4,
                card: 4,
                ps_auxpart: 0,
                ps_mascot: 0,
                ps_card: 8,
                total: function(option) {
                    
                    let pwr = this.auxpart + this.mascot + this.card;

                    if (option == 1 || option == 2 || options == 3)
                        pwr += this.ps_auxpart + this.ps_mascot + this.ps_card;

                    return pwr;
                }
            }
        }
    }

    const powerRange = vclub.getRange(options.power.options, options.power.pwr, options.ps);

    // !@ Teste
    if (DEBUG_FLAG)
        console.log(`PowerRange: ${powerRange}`);

    // !@ teste
    if (DEBUG_FLAG)
        console.log('Options:', options);

    const findAlturaColision = (qt, altura_colision) => {

        let count = 0;

        let copy_ball = new Ball();

        do {

            copy_ball.copy(vball);

            qt.ballProcess(_00D083A0);
    
        }while((vball.position.y > altura_colision || vball.num_max_height == -1) && (count++) < 3000)

        let last_step = Math.abs((altura_colision - copy_ball.position.y) / (vball.position.y - copy_ball.position.y));

        vball.copy(copy_ball);

        qt.ballProcess(_00D083A0, _00D083A0 * last_step);

        // !@ Log
        if (DEBUG_FLAG)
            console.log(`(C)End: X: ${copy_ball.position.x}, Y: ${copy_ball.position.y}, Z: ${copy_ball.position.z}`);
        
        // !@ Log
        if (DEBUG_FLAG)
            console.log(`End: X: ${vball.position.x}, Y: ${vball.position.y}, Z: ${vball.position.z}, Dst: ${distanciaScale}`);
        
        if (Math.abs(distanciaScale - vball.position.z) <= margin)
            return 0;
        
        return distanciaScale - vball.position.z;
    }

    const qt = new QuadTree();

    let ret = 0;

    let lado = 0;
    let feed = 0.00006;

    do {

        if (options.percentShot > 1.0)
            options.percentShot = 1.0;
        else if (options.percentShot < 0.0)
            options.percentShot = 0.1;

        // !@ Teste
        if (DEBUG_FLAG)
            console.log(`Percent Shot: ${(options.percentShot*100).toFixed(4)} %`);

        qt.initShot(vball, vclub, wind, options);

        ret = findAlturaColision(qt, altura_colision);

        // !@ Teste
        if (DEBUG_FLAG)
            console.log(`Ret: ${ret}`);

        if (ret == 0)
            isFind = true;
        else {

            // Não tem como achar a força por que nem mandando 100% chega
            if (options.percentShot == 1.0 && ret > 0)
                break;

            // Não tem como achar a força por que não pode mandar 0.0 de porcentagem
            if (options.percentShot == 0.1 && ret < 0)
                break;

            if (lado == 0)
                lado = (ret < 0 ? -1 : 1);
            else if ((ret < 0 && lado == 1) || (ret > 0 && lado == -1))
                feed *= 0.5;

            options.percentShot += ret * feed;
        }

    } while (!isFind && (count++) < limit_checking);

    if (isFind) {

        // !@ log
        if (DEBUG_FLAG)
            console.log('Curva: ', vball.curva);

        found.power = options.percentShot;
        found.desvio = (vball.position.x + (options.position.x + (Math.tan(options.mira_rad) * distanciaScale))) * DESVIO_SCALE_PANGYA_TO_YARD
        found.power_range = powerRange;

        if (DEBUG_FLAG)
            console.log(`Encontrou a força. ${(found.power * 100).toFixed(2)}% - ${(powerRange * found.power).toFixed(1)} - ${(found.desvio / 0.2167 * -1/* Invertido no visual */).toFixed(2)}`);

    }else {
        
        if (DEBUG_FLAG) {

            console.log(`Não encontrou a força. chegou no limite de ${limit_checking} tentativas`);

            console.log(`Ret: ${ret}`);
        }
    }

    return found;
}

const SHOT_TYPE = {
    DUNK: 0,
    TOMAHAWK: 1,
    SPIKE: 2,
    COBRA: 3,
}

function checkValidInput(value) {
        
    if (value == '' || isNaN(value))
        return 0.0;

    return Number(value);
}

function checkValidInputSlope(value) {

    if (value == '')
        return 0.0;

    if (isNaN(value)) {

        let split = value.split(',');

        if (split.length !== 3 || isNaN(split[0]) || isNaN(split[1]) || isNaN(split[2]))
            return 0.0;

        return new Vector3D(Number(split[0]) * slope_break_to_curve_slope, Number(split[1]) * Math.PI / 180, Number(split[2]) * slope_break_to_curve_slope);
    }

    return Number(value);
}

// Calcula
function calc(user_input) {

    let power = checkValidInput(user_input.power);
    let auxpart_pwr = checkValidInput(user_input.auxpart_pwr);
    let card_pwr = checkValidInput(user_input.card_pwr);
    let mascot_pwr = checkValidInput(user_input.mascot_pwr);
    let card_ps_pwr = checkValidInput(user_input.ps_card_pwr);

    let club = CLUB_INFO[CLUB_INFO_ENUM[user_input.club_index]];

    let shot = SHOT_TYPE[SHOT_TYPE_ENUM[user_input.shot_index]];

    let power_shot = POWER_SHOT_FACTORY[POWER_SHOT_FACTORY_ENUM[user_input.power_shot_index]];

    let distance = checkValidInput(user_input.distance);
    let height = checkValidInput(user_input.height);
    let wind = checkValidInput(user_input.wind);
    let degree = checkValidInput(user_input.degree);
    let ground = checkValidInput(user_input.ground)
    let spin = checkValidInput(user_input.spin);
    let curve = checkValidInput(user_input.curve);
    let slope_break = checkValidInputSlope(user_input.slope_break);

    // 100 % ground
    if (ground == 0.0)
        ground = 100.0;

    let result = '';

    // Make options
    const input_values = {
        power_player: {
            pwr: power,
            options: {
                auxpart: auxpart_pwr,
                mascot: mascot_pwr,
                card: card_pwr,
                ps_auxpart: 0,
                ps_mascot: 0,
                ps_card: card_ps_pwr,
                total: function(option) {
                    
                    let pwr = this.auxpart + this.mascot + this.card;

                    if (option == 1 || option == 2 || option == 3)
                        pwr += this.ps_auxpart + this.ps_mascot + this.ps_card;

                    return pwr;
                }
            }
        },
        club_info: club,
        shot: shot,
        power_shot: power_shot,
        distance: distance,
        height: height,
        wind: wind,
        degree: degree,
        ground: ground,
        spin: spin,
        curva: curve,
        slope: slope_break
    };

    // Calc
    const found = find_power(   input_values.power_player,
                                input_values.club_info,
                                input_values.shot,
                                input_values.power_shot,
                                input_values.distance, 
                                input_values.height, 
                                input_values.wind, 
                                input_values.degree, 
                                input_values.ground, 
                                input_values.spin, 
                                input_values.curva,
                                input_values.slope);

    let f = [found];
    let index_f = 0;

    if (found.power != -1) {

        do {

            index_f++;

            f.push
            (
                find_power
                (   
                    input_values.power_player,
                    input_values.club_info,
                    input_values.shot,
                    input_values.power_shot,
                    input_values.distance, 
                    input_values.height, 
                    input_values.wind, 
                    input_values.degree, 
                    input_values.ground, 
                    input_values.spin, 
                    input_values.curva, 
                    input_values.slope,
                    Math.atan2(f[index_f - 1].desvio * 1.5, input_values.distance), 
                    f[index_f - 1].power
                )
            );

        } while (f[index_f].power != -1 && f[index_f -1].power != -1 && Math.abs(f[index_f - 1].desvio - f[index_f].desvio) >= 0.05);
    }

    if (f[index_f].power != -1) {

        result = `${(f[index_f].power * 100).toFixed(1)}%, ${(f[index_f].power_range * f[index_f].power).toFixed(1)}y, ${((desvioByDegree(f[index_f].desvio, distance) / 0.2167)).toFixed(2)}pb`;

    }else {

        result = 'Não conseguiu calcular a tacada, não chega ou é muito fraco.'
    }

    return result;
}

const YARDS_TO_PB = 0.2167;
const YARDS_TO_PBA = 0.8668;
const YARDS_TO_PBA_PLUS = 1.032;
const YARDS_TO_PBA_40 = 2.58;
const YARDS_TO_PBA_100 = 6.2;

function desvioByDegree(yards, distance) {
    return Math.sin(Math.atan2(yards * -1.5, distance)) * distance / 1.5;
}

function smartDesvio(yards, distance) {

    yards = desvioByDegree(yards, distance);

    let pb_sample = yards / YARDS_TO_PB;

    if (Math.abs(pb_sample) <= 5.0)
        return `${pb_sample.toFixed(2)}pb`;

    pb_sample = yards / YARDS_TO_PBA;

    if (Math.abs(pb_sample) <= 5.0)
        return `${pb_sample.toFixed(2)}pba`;

    pb_sample = yards / YARDS_TO_PBA_PLUS;

    if (Math.abs(pb_sample) <= 5.0)
        return `${pb_sample.toFixed(2)}pba+`;

    pb_sample = yards / YARDS_TO_PBA_40;

    if (Math.abs(pb_sample) <= 5.0)
        return `${pb_sample.toFixed(2)}pba40`;

    return `${(yards / YARDS_TO_PBA_100).toFixed(2)}pba100`;
}


module.exports = calc;