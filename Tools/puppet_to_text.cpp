// Arquivo puppet_to_text.cpp
// Criado em 14/12/2022 as 14:04 por Acrisio
// Definição e Implementação do Tradutor dos arquivos .pet, .mpet, .bpet e .apet do PangYa do PC todas as versões

#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <ctime>

constexpr char default_file[] = "golf_ball_01.pet";
#define DISABLE_LOG false
#define PRINT_LOG_MATRIX_FUNCTIONS false

enum eFILE_TYPE : uint32_t {
	FT_TEXT = 1,
	FT_SMTL = 2,
	FT_BONE = 4,
	FT_ANIM = 8,
	FT_MESH = 16,
	FT_FANM = 32,
	FT_FRAM = 64,
	FT_MOTI = 128,
	FT_COLL = 256,
	FT_ALL  = 511,
	FT_SKINLIST = 65536,
	FT_EXTR = 131072,
};

enum eSPECULAR_MATERIAL_TYPE : uint32_t {
	SMT_BOOL,
	SMT_INT,
	SMT_FLOAT,
	SMT_VECTOR2,
	SMT_VECTOR3,
	SMT_VECTOR4,
	SMT_MATRIX,
	SMT_MATRIXARRAY,
	SMT_TEXTURE,
	SMT_NUM,
};

constexpr uint32_t FILE_PET  = eFILE_TYPE::FT_ALL;
constexpr uint32_t FILE_APET = eFILE_TYPE::FT_BONE | eFILE_TYPE::FT_ANIM | eFILE_TYPE::FT_FRAM | eFILE_TYPE::FT_MOTI;
constexpr uint32_t FILE_BPET = eFILE_TYPE::FT_COLL | eFILE_TYPE::FT_BONE | eFILE_TYPE::FT_EXTR;
constexpr uint32_t FILE_MPET = eFILE_TYPE::FT_TEXT | eFILE_TYPE::FT_BONE | eFILE_TYPE::FT_MESH | eFILE_TYPE::FT_FANM | eFILE_TYPE::FT_SKINLIST | eFILE_TYPE::FT_SMTL;
constexpr uint32_t FILE_ANIM_AND_BONE = eFILE_TYPE::FT_BONE | eFILE_TYPE::FT_ANIM;

struct version_s_0 {
	uint8_t minor;
	uint8_t major;
};

struct Version {
	Version() {}
	Version(uint32_t _u32) {
		_v.full_version = _u32;
	}
	union {
		version_s_0 _s_0;
		uint16_t version;
		uint32_t full_version;
	}_v;
	std::string toString() {
		return "Version " + std::to_string((uint16_t)_v._s_0.major) + "." + std::to_string((uint16_t)_v._s_0.minor);
	}
};

static Version VERSION_1_0{0xFFFE0100};
static Version VERSION_1_1{0xFFFE0101};
static Version VERSION_1_2{0xFFFE0102};
static Version VERSION_1_3{0xFFFE0103};

uint32_t gFileType = eFILE_TYPE::FT_ALL;
Version gVersion = VERSION_1_0;

void setFileType(std::string _file) {

	gFileType = eFILE_TYPE::FT_ALL;

	if (_file.empty())
		return;

	auto pos = _file.find_last_of(".");

	if (pos == _file.npos)
		return;

	const auto ext = _file.substr(pos);

	if (ext.empty())
		return;

	if (stricmp(ext.c_str(), ".pet") == 0)
		gFileType = FILE_PET;
	else if (stricmp(ext.c_str(), ".mpet") == 0)
		gFileType = FILE_MPET;
	else if (stricmp(ext.c_str(), ".apet") == 0)
		gFileType = FILE_APET;
	else if (stricmp(ext.c_str(), ".bpet") == 0)
		gFileType = FILE_BPET;
};

uint32_t getFileType() {
	return gFileType;
};

int32_t compareVersions(const Version& _v1, const Version& _v2) {

	if (_v1._v.full_version == _v2._v.full_version)
		return 0;
	
	if (_v1._v._s_0.major == _v2._v._s_0.major)
		return (_v1._v._s_0.minor < _v2._v._s_0.minor) ? -1 : 1;
	
	return (_v1._v._s_0.major < _v2._v._s_0.major) ? -1 : 1;
};

void setVersion(Version& _ver) {
	gVersion = _ver;
};

const Version& getVersion() {
	return gVersion;
}

auto toStringVectorOp = [](std::string _accumulate, auto& _value) -> std::string {
	return std::move(_accumulate) + "\n" + _value.toString();
};

auto toStringVectorStringOp = [](std::string _accumulate, auto& _value) -> std::string {
	return std::move(_accumulate) + "\n" + _value;
};

struct FileLog {
	FileLog(bool _enable = !DISABLE_LOG, const char* _name = nullptr) {

		if (_name == nullptr)
			name = makeFileName();
		else
			name = _name;
		
		if (_enable)
			out.open(name, std::ios_base::binary);

		if (!out.is_open()) {
			state = false;
			return;
		}

		state = true;
	}
	~FileLog() {
		if (out.is_open())
			out.close();
		state = false;
	}

	std::string makeFileName() {
		std::string str = "Log Puppet(";

		time_t now;
		std::time(&now);

		struct tm* timeinfo = std::localtime(&now);
		timeinfo->tm_mon++;
		timeinfo->tm_year += 1900;

		str += std::to_string(timeinfo->tm_mday) + "-" + std::to_string(timeinfo->tm_mon) 
			+ "-" + std::to_string(timeinfo->tm_year) + " " + std::to_string(timeinfo->tm_hour)
			+ "-" + std::to_string(timeinfo->tm_min) + "-" + std::to_string(timeinfo->tm_sec);
		
		str += ").log";

		return str;
	}

	template<typename T> FileLog& operator<<(const T& _value) {
		
		if (!state)
			return *this;
		
		out << _value;

		return *this;
	}

	FileLog& operator<<(std::ostream& (*f)(std::ostream&)) {

		if (!state)
			return *this;

		out << f;

		return *this;
	}

	std::string name;
	std::ofstream out;
	bool state;
};

FileLog sLog;

struct Vector2d {
	float x;
	float y;
	Vector2d& multiply(float _s) {
		x *= _s;
		y *= _s;

		return *this;
	}
	Vector2d& add(Vector2d _v) {
		x += _v.x;
		y += _v.y;

		return *this;
	}
	Vector2d& sub(Vector2d _v) {
		x -= _v.x;
		y -= _v.y;

		return *this;
	}
	float product(Vector2d _v) {
		return x * _v.x + y * _v.y;
	}
	std::string toString() {
		return "x: " + std::to_string(x) + ", y: " + std::to_string(y);
	}
};

struct Vector3d {
	float x;
	float y;
	float z;
	Vector3d& multiply(float _s) {
		x *= _s;
		y *= _s;
		z *= _s;

		return *this;
	}
	Vector3d& add(Vector3d _v) {
		x += _v.x;
		y += _v.y;
		z += _v.z;

		return *this;
	}
	Vector3d& sub(Vector3d _v) {
		x -= _v.x;
		y -= _v.y;
		z -= _v.z;

		return *this;
	}
	float product(Vector3d _v) {
		return x * _v.x + y * _v.y + z * _v.z;
	}
	std::string toString() {
		return "x: " + std::to_string(x) + ", y: " + std::to_string(y) + ", z: " + std::to_string(z);
	}
};

struct Vector4d {
	float x;
	float y;
	float z;
	float w;
	Vector4d& multiply(float _s) {
		x *= _s;
		y *= _s;
		z *= _s;
		w *= _s;

		return *this;
	}
	Vector4d& add(Vector4d _v) {
		x += _v.x;
		y += _v.y;
		z += _v.z;
		w += _v.w;
		
		return *this;
	}
	Vector4d& sub(Vector4d _v) {
		x -= _v.x;
		y -= _v.y;
		z -= _v.z;
		w -= _v.w;

		return *this;
	}
	float product(Vector4d _v) {
		return x * _v.x + y * _v.y + z * _v.z + w * _v.w;
	}
	std::string toString() {
		return "x: " + std::to_string(x) + ", y: " 
			+ std::to_string(y) + ", z: " + std::to_string(z) + ", w: " + std::to_string(w);
	}
};

struct Matrix {
	Vector3d v1;
	Vector3d v2;
	Vector3d v3;
	Vector3d v4;
	std::string toString() {
		return "v1: " + v1.toString() + "\nv2: " + v2.toString() + "\nv3: " + v3.toString() + "\nv4: " + v4.toString();
	}
};

struct Matrix4 {
	Vector4d v1;
	Vector4d v2;
	Vector4d v3;
	Vector4d v4;
	Matrix4() {
		v1 = {1, 0, 0, 0};
		v2 = {0, 1, 0, 0};
		v3 = {0, 0, 1, 0};
		v4 = {0, 0, 0, 1};
	}
	Matrix4(Matrix _m3) {
		setBasics(_m3);
	}
	Matrix4(Vector4d _v1, Vector4d _v2, Vector4d _v3, Vector4d _v4) {
		v1 = _v1;
		v2 = _v2;
		v3 = _v3;
		v4 = _v4;
	}
	void setBasics(Matrix _m3) {
		v1.x = _m3.v1.x;
		v1.y = _m3.v2.x;
		v1.z = _m3.v3.x;
		v1.w = _m3.v4.x;
		
		v2.x = _m3.v1.y;
		v2.y = _m3.v2.y;
		v2.z = _m3.v3.y;
		v2.w = _m3.v4.y;

		v3.x = _m3.v1.z;
		v3.y = _m3.v2.z;
		v3.z = _m3.v3.z;
		v3.w = _m3.v4.z;

		v4.x = 0;
		v4.y = 0;
		v4.z = 0;
		v4.w = 1;
	}
	Vector4d multiply(Vector4d _v4) {
		Vector4d out = {0};

		out.x = _v4.x * v1.x + _v4.y * v1.y + _v4.z * v1.z + _v4.w * v1.w;
		out.y = _v4.x * v2.x + _v4.y * v2.y + _v4.z * v2.z + _v4.w * v2.w;
		out.z = _v4.x * v3.x + _v4.y * v3.y + _v4.z * v3.z + _v4.w * v3.w;
		out.w = _v4.x * v4.x + _v4.y * v4.y + _v4.z * v4.z + _v4.w * v4.w;

		return out;
	}
	Matrix4 multiply(Matrix4 _a, Matrix4 _b) {
		Matrix4 out = {};

		out.v1.x = _a.v1.x * _b.v1.x + _a.v1.y * _b.v2.x + _a.v1.z * _b.v3.x + _a.v1.w * _b.v4.x;
		out.v2.x = _a.v2.x * _b.v1.x + _a.v2.y * _b.v2.x + _a.v2.z * _b.v3.x + _a.v2.w * _b.v4.x;
		out.v3.x = _a.v3.x * _b.v1.x + _a.v3.y * _b.v2.x + _a.v3.z * _b.v3.x + _a.v3.w * _b.v4.x;
		out.v4.x = _a.v4.x * _b.v1.x + _a.v4.y * _b.v2.x + _a.v4.z * _b.v3.x + _a.v4.w * _b.v4.x;

		out.v1.y = _a.v1.x * _b.v1.y + _a.v1.y * _b.v2.y + _a.v1.z * _b.v3.y + _a.v1.w * _b.v4.y;
		out.v2.y = _a.v2.x * _b.v1.y + _a.v2.y * _b.v2.y + _a.v2.z * _b.v3.y + _a.v2.w * _b.v4.y;
		out.v3.y = _a.v3.x * _b.v1.y + _a.v3.y * _b.v2.y + _a.v3.z * _b.v3.y + _a.v3.w * _b.v4.y;
		out.v4.y = _a.v4.x * _b.v1.y + _a.v4.y * _b.v2.y + _a.v4.z * _b.v3.y + _a.v4.w * _b.v4.y;

		out.v1.z = _a.v1.x * _b.v1.z + _a.v1.y * _b.v2.z + _a.v1.z * _b.v3.z + _a.v1.w * _b.v4.z;
		out.v2.z = _a.v2.x * _b.v1.z + _a.v2.y * _b.v2.z + _a.v2.z * _b.v3.z + _a.v2.w * _b.v4.z;
		out.v3.z = _a.v3.x * _b.v1.z + _a.v3.y * _b.v2.z + _a.v3.z * _b.v3.z + _a.v3.w * _b.v4.z;
		out.v4.z = _a.v4.x * _b.v1.z + _a.v4.y * _b.v2.z + _a.v4.z * _b.v3.z + _a.v4.w * _b.v4.z;

		out.v1.w = _a.v1.x * _b.v1.w + _a.v1.y * _b.v2.w + _a.v1.z * _b.v3.w + _a.v1.w * _b.v4.w;
		out.v2.w = _a.v2.x * _b.v1.w + _a.v2.y * _b.v2.w + _a.v2.z * _b.v3.w + _a.v2.w * _b.v4.w;
		out.v3.w = _a.v3.x * _b.v1.w + _a.v3.y * _b.v2.w + _a.v3.z * _b.v3.w + _a.v3.w * _b.v4.w;
		out.v4.w = _a.v4.x * _b.v1.w + _a.v4.y * _b.v2.w + _a.v4.z * _b.v3.w + _a.v4.w * _b.v4.w;

		return out;
	}
	Matrix4& multiply(Matrix4 _m4) {
		*this = multiply(*this, _m4);
		return *this;
	}
	float determinant() const {
		return v1.x * v2.y * v3.z * v4.w - v1.x * v2.y * v3.w * v4.z + v1.x * v2.z * v3.w * v4.y - v1.x * v2.z * v3.y * v4.w
			+ v1.x * v2.w * v3.y * v4.z - v1.x * v2.w * v3.z * v4.y - v1.y * v2.z * v3.w * v4.x + v1.y * v2.z * v3.x * v4.w
			- v1.y * v2.w * v3.x * v4.z + v1.y * v2.w * v3.z * v4.x - v1.y * v2.x * v3.z * v4.w + v1.y * v2.x * v3.w * v4.z
			+ v1.z * v2.w * v3.x * v4.y - v1.z * v2.w * v3.y * v4.x + v1.z * v2.x * v3.y * v4.w - v1.z * v2.x * v3.w * v4.y
			+ v1.z * v2.y * v3.w * v4.x - v1.z * v2.y * v3.x * v4.w - v1.w * v2.x * v3.y * v4.z + v1.w * v2.x * v3.z * v4.y
			- v1.w * v2.y * v3.z * v4.x + v1.w * v2.y * v3.x * v4.z - v1.w * v2.z * v3.x * v4.y + v1.w * v2.z * v3.y * v4.x;
	}
	Matrix4& inverse() {
		const float det = determinant();
		if (det == 0.f) {
			v1 = {0};
			v2 = {0};
			v3 = {0};
			v4 = {0};
			return *this;
		}

		const float invdet = 1.f / det;

		Matrix4 res;
		res.v1.x = invdet * (v2.y * (v3.z * v4.w - v3.w * v4.z) + v2.z * (v3.w * v4.y - v3.y * v4.w) + v2.w * (v3.y * v4.z - v3.z * v4.y));
		res.v1.y = -invdet * (v1.y * (v3.z * v4.w - v3.w * v4.z) + v1.z * (v3.w * v4.y - v3.y * v4.w) + v1.w * (v3.y * v4.z - v3.z * v4.y));
		res.v1.z = invdet * (v1.y * (v2.z * v4.w - v2.w * v4.z) + v1.z * (v2.w * v4.y - v2.y * v4.w) + v1.w * (v2.y * v4.z - v2.z * v4.y));
		res.v1.w = -invdet * (v1.y * (v2.z * v3.w - v2.w * v3.z) + v1.z * (v2.w * v3.y - v2.y * v3.w) + v1.w * (v2.y * v3.z - v2.z * v3.y));
		res.v2.x = -invdet * (v2.x * (v3.z * v4.w - v3.w * v4.z) + v2.z * (v3.w * v4.x - v3.x * v4.w) + v2.w * (v3.x * v4.z - v3.z * v4.x));
		res.v2.y = invdet * (v1.x * (v3.z * v4.w - v3.w * v4.z) + v1.z * (v3.w * v4.x - v3.x * v4.w) + v1.w * (v3.x * v4.z - v3.z * v4.x));
		res.v2.z = -invdet * (v1.x * (v2.z * v4.w - v2.w * v4.z) + v1.z * (v2.w * v4.x - v2.x * v4.w) + v1.w * (v2.x * v4.z - v2.z * v4.x));
		res.v2.w = invdet * (v1.x * (v2.z * v3.w - v2.w * v3.z) + v1.z * (v2.w * v3.x - v2.x * v3.w) + v1.w * (v2.x * v3.z - v2.z * v3.x));
		res.v3.x = invdet * (v2.x * (v3.y * v4.w - v3.w * v4.y) + v2.y * (v3.w * v4.x - v3.x * v4.w) + v2.w * (v3.x * v4.y - v3.y * v4.x));
		res.v3.y = -invdet * (v1.x * (v3.y * v4.w - v3.w * v4.y) + v1.y * (v3.w * v4.x - v3.x * v4.w) + v1.w * (v3.x * v4.y - v3.y * v4.x));
		res.v3.z = invdet * (v1.x * (v2.y * v4.w - v2.w * v4.y) + v1.y * (v2.w * v4.x - v2.x * v4.w) + v1.w * (v2.x * v4.y - v2.y * v4.x));
		res.v3.w = -invdet * (v1.x * (v2.y * v3.w - v2.w * v3.y) + v1.y * (v2.w * v3.x - v2.x * v3.w) + v1.w * (v2.x * v3.y - v2.y * v3.x));
		res.v4.x = -invdet * (v2.x * (v3.y * v4.z - v3.z * v4.y) + v2.y * (v3.z * v4.x - v3.x * v4.z) + v2.z * (v3.x * v4.y - v3.y * v4.x));
		res.v4.y = invdet * (v1.x * (v3.y * v4.z - v3.z * v4.y) + v1.y * (v3.z * v4.x - v3.x * v4.z) + v1.z * (v3.x * v4.y - v3.y * v4.x));
		res.v4.z = -invdet * (v1.x * (v2.y * v4.z - v2.z * v4.y) + v1.y * (v2.z * v4.x - v2.x * v4.z) + v1.z * (v2.x * v4.y - v2.y * v4.x));
		res.v4.w = invdet * (v1.x * (v2.y * v3.z - v2.z * v3.y) + v1.y * (v2.z * v3.x - v2.x * v3.z) + v1.z * (v2.x * v3.y - v2.y * v3.x));
		*this = res;

		return *this;
	}
	Matrix4& transpose() {
		std::swap(v2.x, v1.y);
		std::swap(v3.x, v1.z);
		std::swap(v3.y, v2.z);
		std::swap(v4.x, v1.w);
		std::swap(v4.y, v2.w);
		std::swap(v4.z, v3.w);

		return *this;
	}
	Vector4d to_translation() {
		return Vector4d{v1.w, v2.w, v3.w, 1.f};
	}
	std::string toString() {
		return "v1: " + v1.toString() + "\nv2: " + v2.toString() + "\nv3: " + v3.toString() + "\nv4: " + v4.toString();
	}
};

struct Area {
	Vector3d min;
	Vector3d max;
	std::string toString() {
		return "min: " + min.toString() + "\nmax: " + max.toString();
	}
};

struct PETModelHeader {

	std::string getId() {
		return std::string(id, id + 4);
	}
	std::string toString() {
		return "id: " + getId() + ", length: " + std::to_string(length);
	}

	char id[4];
	uint32_t length;
};

struct Texture {
	char name[32];
	char flag; // mapType
	uint8_t group;
	uint16_t __bytealign; // byte align to pack(4)
	uint32_t diffuse;
	uint32_t handle;
	std::string toString() {
		return "name: " + std::string(name) + "\nflag: " + std::to_string((int16_t)flag) + "\tgroup: " 
			+ std::to_string((uint16_t)group)
			+ "\ndiffuse: " + std::to_string(diffuse) 
			+ "\thandle: " + std::to_string(handle);
	}
};

struct Bone {
	std::string name;
	uint16_t parent;
	Matrix matrix;
	float unknown_v1_3; // valor novo da versão 1.3
	std::string toString() {
		return "name: " + name + ", parent: " + std::to_string(parent) + ", unknown_v1_3: " 
			+ std::to_string(unknown_v1_3)
			+ "\nmatrix:\n" + matrix.toString();
	}
};

struct PositionData {
	float time;
	Vector3d v;
	std::string toString() {
		return "time: " + std::to_string(time) + ", vector3d: " + v.toString();
	}
};

struct RotationData {
	float time;
	Vector4d v;
	std::string toString() {
		return "time: " + std::to_string(time) + ", vector4d: " + v.toString();
	}
};

struct ScalingData {
	float time;
	Vector3d v;
	std::string toString() {
		return "time: " + std::to_string(time) + ", vector3d: " + v.toString();
	}
};

struct AnimationFlag {
	float time;
	float scale;
	std::string toString() {
		return "time: " + std::to_string(time) + ", scale: " + std::to_string(scale);
	}
};

struct Animation {
	uint16_t bone_idx;
	uint32_t position_length;
	uint32_t rotation_length;
	uint32_t scaling_length;
	uint32_t flags_length; // acho que esse é o scale
	std::vector<PositionData> positions;
	std::vector<RotationData> rotations;
	std::vector<ScalingData> scalings;
	std::vector<AnimationFlag> flags;
	std::string toString() {
		return "bone_idx: " + std::to_string(bone_idx)
			+ "\nPositions:" + std::accumulate(positions.begin(), positions.end(), std::string(), toStringVectorOp)
			+ "\nRotations:" + std::accumulate(rotations.begin(), rotations.end(), std::string(), toStringVectorOp)
			+ "\nScalings:" + std::accumulate(scalings.begin(), scalings.end(), std::string(), toStringVectorOp)
			+ "\nFlags:" + std::accumulate(flags.begin(), flags.end(), std::string(), toStringVectorOp);
	}
};

struct Frame {
	uint32_t index;
	std::vector< std::string > messages;
	int32_t check;
	std::string toString() {
		return "index: " + std::to_string(index) 
			+ "\tcheck: " + std::to_string(check)
			+ "\nmessages: " + std::accumulate(messages.begin(), messages.end(), std::string(), toStringVectorStringOp);
	}
};

struct Motion {
	std::string name;
	uint32_t frame_start;
	uint32_t frame_end;
	std::string nextmove;
	std::string connection_method;
	float connection_time;
	std::string top_version;
	std::string toString() {
		return "name: " + name + "\nframe_start: " 
			+ std::to_string(frame_start) + ", frame_end: " + std::to_string(frame_end)
			+ "\nnextmove: " + nextmove + "\nconnection_method: " + connection_method
			+ "\ntop_version: " + top_version + "\nconnection_time: " + std::to_string(connection_time);
	}
};

struct CollisionBox {
	uint32_t shape;
	uint32_t show;
	std::vector<std::string> scripts; // 4 = {0=> Box name, 1=> Bone name, 2,3=> Options(bound_box) %s %s}
	Area area;
	std::string toString() {
		return "shape: " + std::to_string(shape) + ", show: " + std::to_string(show)
		 	+ "\nscripts:\n" + std::accumulate(scripts.begin(), scripts.end(), std::string(), toStringVectorStringOp)
			+ "\narea:\n" + area.toString();
	}
};

struct BoneInformation {
	uint16_t weight;
	uint16_t id;
	std::string toString() {
		return "weight: " + std::to_string(weight) + ", id: " + std::to_string(id);
	}
};

struct Vertex {
	Vector3d v;
	float weight;
	std::vector<BoneInformation> bone_infos;
	std::string toString() {
		return "vector3d: " + v.toString() + "\nweight: " + std::to_string(weight) + "\nbone_infos: " 
			+ std::accumulate(bone_infos.begin(), bone_infos.end(), std::string(), toStringVectorOp);
	}
};

struct UVMapping {
	Vector2d v;
	std::string toString() {
		return "vector2d: " + v.toString();
	}
};

struct PetTriPoint {
	uint32_t pos;
	Vector3d v;
	std::vector<UVMapping> uv_mappings;
	std::string toString() {
		return "pos: " + std::to_string(pos) + ", vector3d: " + v.toString() 
			+ "\nuv_mapping: " + std::accumulate(uv_mappings.begin(), uv_mappings.end(), std::string(), toStringVectorOp);
	}
};

struct Polygon {
	PetTriPoint pet_tri_points[3];
	std::string toString() {
		return "polygon[0]: " + pet_tri_points[0].toString() + "\npolygon[1]: " + pet_tri_points[1].toString()
			+ "\npolygon[2]: " + pet_tri_points[2].toString();
	}
};

struct TextureIndex {
	uint8_t index;
	std::string toString() {
		return "index: " + std::to_string((uint16_t)index);
	}
};

struct mesh_new_value_v1_2 {
	int8_t value;
	std::string toString() {
		return "value: " + std::to_string((int16_t)value);
	}
};

struct extra_value_index_length {
	uint32_t index;
	uint32_t length;
	std::string toString() {
		return "index: " + std::to_string(index) + ", length: " + std::to_string(length);
	}
};

struct mpet_extra_value {
	uint32_t unknown;
	extra_value_index_length value_verticies;
	extra_value_index_length value_polygons;
	std::string toString() {
		return "unknown: " + std::to_string(unknown) + "\nverticies: " + value_verticies.toString()
			+ "\npolygons: " + value_polygons.toString();
	}
};

struct Extra {
	int16_t bone_id;
	std::string toString() {
		return "bone_id: " + std::to_string(bone_id);
	}
};

struct Mesh {
	uint32_t vertice_length;
	uint32_t polygon_length;
	uint8_t mpet_extra_value_length;
	std::vector<Vertex> vertices;
	std::vector<Polygon> polygons;
	std::vector<TextureIndex> texture_indexs;
	std::vector<mpet_extra_value> mpet_extra_values;
	std::vector<mesh_new_value_v1_2> mesh_new_values_v1_2;
	std::string toString() {
		return "Vertice length: " + std::to_string(vertices.size()) 
			+ ", Polygon length: " + std::to_string(polygons.size())
			+ ", Texture Index length: " + std::to_string(texture_indexs.size())
			+ "\nmpet_extra_value_length: " + std::to_string((uint16_t)mpet_extra_value_length)
			+ ", mesh_new_value_v1_2_length: " + std::to_string(mesh_new_values_v1_2.size())
			+ "\nmpet_extra_Values:" + std::accumulate(mpet_extra_values.begin(), mpet_extra_values.end(), std::string(), toStringVectorOp)
			+ "\nVertices:" + std::accumulate(vertices.begin(), vertices.end(), std::string(), toStringVectorOp)
			+ "\nPolygons:" + std::accumulate(polygons.begin(), polygons.end(), std::string(), toStringVectorOp)
			+ "\nTexture Indexs:" + std::accumulate(texture_indexs.begin(), texture_indexs.end(), std::string(), toStringVectorOp)
			+ "\nMesh New Values Ver 1.2:" + std::accumulate(mesh_new_values_v1_2.begin(), mesh_new_values_v1_2.end(), std::string(), toStringVectorOp);
	}
};

struct specular_value {
	std::string name;
	uint32_t type;
	// values
	uint32_t bBOOL;
	int32_t iINT;
	float fFLOAT;
	Vector2d vVECTOR2;
	Vector3d vVECTOR3;
	Vector4d vVECTOR4;
	Matrix4 mMATRIX;
	std::string sTEXTURE;
	std::string getValueToString() {
		switch (type) {
			case SMT_BOOL:
				return std::to_string(bBOOL);
			case SMT_INT:
				return std::to_string(iINT);
			case SMT_FLOAT:
				return std::to_string(fFLOAT);
			case SMT_VECTOR2:
				return vVECTOR2.toString();
			case SMT_VECTOR3:
				return vVECTOR3.toString();
			case SMT_VECTOR4:
				return vVECTOR4.toString();
			case SMT_MATRIX:
				return mMATRIX.toString();
			case SMT_TEXTURE:
				return sTEXTURE;
		}

		return "@";
	}
	std::string toString() {
		return "type: " + std::to_string(type) + "\nname: " + name
			+ "\nvalue: " + getValueToString();
	}
};

struct SpecularMaterial {
	std::string name;
	std::vector<specular_value> specular_values;
	std::string toString() {
		return "name: " + name
			+ "\nspecular_values:" + std::accumulate(specular_values.begin(), specular_values.end(), std::string(), toStringVectorOp);
	}
};

struct FaceAnimation {
	uint8_t group;
	char name[32];
	char material_name[32];
	std::string toString() {
		return "group: " + std::to_string((uint16_t)group) + "\nname: " + name
			+ "\nmaterial_name: " + material_name;
	}
};

struct FileObject {
	FileObject(const char* _file) {
		
		name = (char*)_file;
		
		in.open(name, std::ios_base::binary);

		state = in.is_open();

		in.seekg(0, std::ios_base::end);
		size = in.tellg();
		in.seekg(0, std::ios_base::beg);

		index = 0;
	}

	bool read(char* _buf, size_t _size) {

		in.read(_buf, _size);

		index += in.gcount();

		if (!in || in.gcount() != _size)
			return false;	

		return true;
	}

	bool peekBlock(char* _buf, size_t _size, size_t _offset) {
		
		if (_offset >= size)
			return false;
		if ((_offset + _size) > size)
			return false;
	
		size_t old_index = index;
		
		in.seekg(_offset, std::ios_base::beg);

		in.read(_buf, _size);

		in.seekg(old_index, std::ios_base::beg);

		if (!in || in.gcount() != _size)
			return false;

		return true;
	};

	bool jumpBlock(size_t _size) {
		if (index >= size)
			return false;
		if ((index + _size) > size)
			return false;
		
		in.seekg(index + _size, std::ios_base::beg);

		index += _size;

		return true;
	}

	std::ifstream in;
	char* name;
	size_t size;
	size_t index;
	bool state;
};

struct Block {
	Block() {
		buf = nullptr;
		size = 0;
		index = 0;
	}
	Block(char* _buf, size_t _size) {
		Block();
		init(_buf, _size);
	}
	Block(size_t _size) {
		Block();
		init(_size);
	}
	~Block() {
		free();
	}

	void init(char* _buf, size_t _size) {
		free();
		buf = new char[_size];
		memcpy(buf, _buf, _size);
		size = _size;
		index = 0;
	}
	void init(size_t _size) {
		free();
		buf = new char[_size];
		size = _size;
		index = 0;
	}
	void free() {
		if (buf != nullptr)
			delete[] buf;
		buf = nullptr;
		size = 0;
		index = 0;
	}

	uint8_t readByte() {

		if (buf == nullptr)
			return 0xFF;
		if (index >= size)
			return 0xFF;
		if ((index + 1) > size)
			return 0xFF;
		
		index++;

		return (uint8_t)buf[index - 1];
	}

	bool readBuff(void* _buf, size_t _size) {
		
		if (_buf == nullptr || buf == nullptr)
			return false;
		if (index >= size)
			return false;
		if ((index + _size) > size)
			return false;

		memcpy(_buf, buf + index, _size);
		index += _size;

		return true;
	}

	bool readString(std::string& _str) {

		if (buf == nullptr)
			return false;
		if (index >= size)
			return false;
		
		_str.clear();

		size_t length = strlen(buf + index);

		if ((index + length + 1) > size)
			return false;
		
		_str.assign(buf + index, buf + index + length);

		index += length + 1;

		return true;
	}

	bool readFixedString(std::string& _str) {

		if (buf == nullptr)
			return false;
		if (index >= size)
			return false;
		
		uint32_t length;
		if (!readBuff(&length, sizeof(uint32_t)))
			return false;
		
		if (length < 0)
			return false;
		
		if ((index + length) > size)
			return false;
		
		_str.clear();
		_str.resize(length);

		if (length == 0)
			return true;

		if (!readBuff((void*)_str.data(), length))
			return false;

		return true;
	}

	bool ignore(size_t _size) {

		if (buf == nullptr)
			return false;
		if (index >= size)
			return false;
		if ((index + _size) > size)
			return false;
	
		index += _size;

		return true;
	}

	char* buf;
	size_t size;
	size_t index;
};

struct BlockModelCtx {
	PETModelHeader model;
	size_t index;
};

bool readPETModelBlock(FileObject& _fo, BlockModelCtx* _bmc) {

	if (_bmc == nullptr)
		return false;
	if (_fo.index >= _fo.size)
		return false;
	if ((_fo.index + sizeof(PETModelHeader)) > _fo.size)
		return false;

	PETModelHeader pmh{0};

	if (!_fo.read((char*)&pmh, sizeof(PETModelHeader)))
		return false;

	if ((_fo.index + pmh.length) > _fo.size)
		return false;

	_bmc->model = pmh;
	_bmc->index = _fo.index;

	// pula bloco
	if (!_fo.jumpBlock(pmh.length))
		return false;

	return true;
};

Version* loadVersion(Block* _block, size_t _size) {

	if (_block == nullptr)
		return nullptr;

	Version *ver = new Version;

	ver->_v.full_version = VERSION_1_0._v.full_version;

	if (!_block->readBuff(ver, sizeof(Version))) {
		delete ver;
		return nullptr;
	}

	return ver;
};

Mesh* loadMesh(Block* _block, size_t _size) {

	if (_block == nullptr)
		return nullptr;

	Mesh *mesh = new Mesh;
	uint32_t i, j;

	mesh->vertice_length = 0;
	mesh->polygon_length = 0;
	mesh->mpet_extra_value_length = 0;

	auto fileType = getFileType();

	if (fileType == FILE_MPET) {
		if (!_block->readBuff(&mesh->mpet_extra_value_length, sizeof(uint8_t))) {
			delete mesh;
			return nullptr;
		}
		sLog << "Mpet Extra Values(" << (uint16_t)mesh->mpet_extra_value_length << ")";

		mpet_extra_value mpet_extra;

		for (i = 0; i < mesh->mpet_extra_value_length; i++) {
			if (!_block->readBuff(&mpet_extra, sizeof(mpet_extra_value))) {
				delete mesh;
				return nullptr;
			}
			mesh->mpet_extra_values.push_back(mpet_extra);
		}
		sLog << "\tLoadded: " << mesh->mpet_extra_values.size() << std::endl;
	}

	if (!_block->readBuff(&mesh->vertice_length, sizeof(uint32_t))) {
		delete mesh;
		return nullptr;
	}
	sLog << "Vertex(" << mesh->vertice_length << ")";

	Vertex vtx;
	BoneInformation bi;
	uint16_t sumWeight;

	for (i = 0; i < mesh->vertice_length; i++) {
		vtx.bone_infos.clear();
		if (!_block->readBuff(&vtx.v, sizeof(Vector3d))) {
			delete mesh;
			return nullptr;
		}

		if (fileType == FILE_MPET) {
			if (!_block->readBuff(&vtx.weight, sizeof(float))) {
				delete mesh;
				return nullptr;
			}
		}else
			vtx.weight = 1.f;

		sumWeight = 0;
		do {
			bi.weight = _block->readByte();
			bi.id = _block->readByte();
			if (bi.id == 0xFF)
				bi.id = -1;
			else if (bi.id == 0xFE) {
				if (!_block->readBuff(&bi.id, sizeof(uint16_t))) {
					delete mesh;
					return nullptr;
				}
			}
			vtx.bone_infos.push_back(bi);
			sumWeight += bi.weight;
		} while (sumWeight < 0xFF);
		
		if (vtx.bone_infos.size() < 2) {
			if (!_block->ignore(2)) {
				delete mesh;
				return nullptr;
			}
		}

		mesh->vertices.push_back(vtx);
	}
	sLog << "\tLoadded: " << mesh->vertices.size() << "\tIndex: " << _block->index << std::endl;

	if (!_block->readBuff(&mesh->polygon_length, sizeof(uint32_t))) {
		delete mesh;
		return nullptr;
	}
	sLog << "Polygon(" << mesh->polygon_length << ")";

	Polygon polygon;
	UVMapping uv_mappin;
	uint8_t count_uv_mappins;

	for (i = 0; i < mesh->polygon_length; i++) {
		for (j = 0; j < 3; j++) {
			polygon.pet_tri_points[j].uv_mappings.clear();
			if (!_block->readBuff(&polygon.pet_tri_points[j].pos, sizeof(uint32_t))) {
				delete mesh;
				return nullptr;
			}
			if (!_block->readBuff(&polygon.pet_tri_points[j].v, sizeof(Vector3d))) {
				delete mesh;
				return nullptr;
			}
			if (compareVersions(getVersion(), VERSION_1_2) >= 0) {
				if (!_block->readBuff(&count_uv_mappins, sizeof(uint8_t))) {
					delete mesh;
					return nullptr;
				}
			}else
				count_uv_mappins = 1;
			while (count_uv_mappins-- > 0) {
				if (!_block->readBuff(&uv_mappin, sizeof(UVMapping))) {
					delete mesh;
					return nullptr;
				}
				polygon.pet_tri_points[j].uv_mappings.push_back(uv_mappin);
			}
		}
		mesh->polygons.push_back(polygon);
	}
	sLog << "\tLoadded: " << mesh->polygons.size() << "\tIndex: " << _block->index << std::endl;

	TextureIndex texture_index;

	for(i = 0; i < mesh->polygon_length; i++) {
		if (!_block->readBuff(&texture_index, sizeof(TextureIndex))) {
			delete mesh;
			return nullptr;
		}
		mesh->texture_indexs.push_back(texture_index);
	}
	sLog << "Texture Index(" << mesh->polygon_length << ")\tLoadded: " 
		<< mesh->texture_indexs.size() << "\tIndex: " << _block->index << std::endl;
	
	if (compareVersions(getVersion(), VERSION_1_2) >= 0) {
		mesh_new_value_v1_2 mesh_new_value;

		for (i = 0; i < mesh->polygon_length; i++) {
			if (!_block->readBuff(&mesh_new_value, sizeof(mesh_new_value_v1_2))) {
				delete mesh;
				return nullptr;
			}
			mesh->mesh_new_values_v1_2.push_back(mesh_new_value);
		}
		sLog << "Mesh New Value Ver 1.2(" << mesh->polygon_length << ")\tLoadded: "
			<< mesh->mesh_new_values_v1_2.size() << "\tIndex: " << _block->index << std::endl;
	}

	return mesh;
};

std::vector<Frame> loadFrame(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<Frame>();

	std::vector<Frame> vf;

	uint32_t length, i, j;
	if (!_block->readBuff(&length, sizeof(uint32_t))) {
		vf.clear();
		return vf;
	}
	sLog << "Frame(" << length << ")";

	Frame frame;
	std::string msg;

	for (i = 0; i < length; i++) {
		frame.index = 0;
		frame.check = 0;
		frame.messages.clear();
		if (!_block->readBuff(&frame.index, sizeof(uint32_t))) {
			vf.clear();
			return vf;
		}
		for (j = 0; j < 3; j++) {
			msg.clear();
			if (!_block->readFixedString(msg)) {
				vf.clear();
				return vf;
			}
			frame.messages.push_back(msg);
		}

		if (length < 2 && frame.messages[0].c_str()[0] == '\0')
			break;
		
		if (!_block->readBuff(&frame.check, sizeof(uint32_t))) {
			vf.clear();
			return vf;
		}

		vf.push_back(frame);
	}
	sLog << "\tLoadded: " << vf.size() << std::endl;

	return vf;
};

std::vector<Texture> loadTexture(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<Texture>();

	std::vector<Texture> vt;

	uint32_t length, i;
	if (!_block->readBuff(&length, sizeof(uint32_t))) {
		vt.clear();
		return vt;
	}
	sLog << "Texture(" << length << ")";

	Texture texture;

	for (i = 0; i < length; i++) {
		if (!_block->readBuff(&texture, sizeof(Texture))) {
			vt.clear();
			return vt;
		}
		texture.group &= 0xFF;
		vt.push_back(texture);
	}
	sLog << "\tLoadded: " << vt.size() << std::endl;

	return vt;
};

std::vector<Bone> loadBone(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<Bone>();

	std::vector<Bone> vb;

	uint16_t length, i;
	if (!_block->readBuff(&length, sizeof(uint8_t))) {
		vb.clear();
		return vb;
	}
	length &= 0xFF;
	if (length == 0) {
		if (!_block->readBuff(&length, sizeof(uint16_t))) {
			vb.clear();
			return vb;
		}
	}
	sLog << "Bone(" << (uint16_t)length << ")";

	Bone bone;
	auto fileType = getFileType();

	for (i = 0; i < length; i++) {
		bone.parent = 0;
		bone.unknown_v1_3 = 0.f;
		bone.name.clear();
		memset(&bone.matrix, 0, sizeof(Matrix));

		if (!_block->readString(bone.name)) {
			vb.clear();
			return vb;
		}
		if (!_block->readBuff(&bone.parent, sizeof(uint8_t))) {
			vb.clear();
			return vb;
		}
		bone.parent &= 0xFF;
		if (bone.parent == 0xFF)
			bone.parent = -1;
		else if (bone.parent == 0xFE) {
			if (!_block->readBuff(&bone.parent, sizeof(uint16_t))) {
				vb.clear();
				return vb;
			}
		}
		if (fileType != FILE_APET && (fileType == FILE_PET || fileType == FILE_BPET || fileType == FILE_MPET)) {
			if (!_block->readBuff(&bone.matrix, sizeof(Matrix))) {
				vb.clear();
				return vb;
			}
			if (compareVersions(getVersion(), VERSION_1_3) >= 0) {
				if (!_block->readBuff(&bone.unknown_v1_3, sizeof(float))) {
					vb.clear();
					return vb;
				}
			}
		}
		vb.push_back(bone);
	}
	sLog << "\tLoadded: " << vb.size() << std::endl;

	return vb;
};

std::vector<Animation> loadAnimation(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<Animation>();

	std::vector<Animation> va;

	Animation anim;
	PositionData pd;
	RotationData rd;
	ScalingData sd;
	AnimationFlag af;
	uint32_t i;

	while(true) {
		if (!_block->readBuff(&anim.bone_idx, sizeof(uint8_t))) {
			va.clear();
			return va;
		}
		anim.bone_idx &= 0xFF;
		if (anim.bone_idx == 0xFF)
			break;
		if (anim.bone_idx == 0xFE) {
			if (!_block->readBuff(&anim.bone_idx, sizeof(uint16_t))) {
				va.clear();
				return va;
			}
		}
		if ((int8_t)anim.bone_idx < 0 || (int16_t)anim.bone_idx < 0)
			break;
		
		anim.positions.clear();
		anim.rotations.clear();
		anim.scalings.clear();
		anim.flags.clear();
		
		if (!_block->readBuff(&anim.position_length, sizeof(uint32_t))) {
			va.clear();
			return va;
		}
		sLog << "Animation->Position(" << anim.position_length << ")";

		for (i = 0; i < anim.position_length; i++) {
			if (!_block->readBuff(&pd, sizeof(PositionData))) {
				va.clear();
				return va;
			}
			anim.positions.push_back(pd);
		}
		sLog << "\tLoadded: " << anim.positions.size() << "\tIndex: " << _block->index << std::endl;

		if (!_block->readBuff(&anim.rotation_length, sizeof(uint32_t))) {
			va.clear();
			return va;
		}
		sLog << "Animation->Rotation(" << anim.rotation_length << ")";

		for (i = 0; i < anim.rotation_length; i++) {
			if (!_block->readBuff(&rd, sizeof(RotationData))) {
				va.clear();
				return va;
			}
			anim.rotations.push_back(rd);
		}
		sLog << "\tLoadded: " << anim.rotations.size() << "\tIndex: " << _block->index << std::endl;

		if (!_block->readBuff(&anim.scaling_length, sizeof(uint32_t))) {
			va.clear();
			return va;
		}
		sLog << "Animation->Scaling(" << anim.scaling_length << ")";

		for (i = 0; i < anim.scaling_length; i++) {
			if (!_block->readBuff(&sd, sizeof(ScalingData))) {
				va.clear();
				return va;
			}
			anim.scalings.push_back(sd);
		}
		sLog << "\tLoadded: " << anim.scalings.size() << "\tIndex: " << _block->index << std::endl;

		if (compareVersions(getVersion(), VERSION_1_3) >= 0) {
			if (!_block->readBuff(&anim.flags_length, sizeof(uint32_t))) {
				va.clear();
				return va;
			}
			sLog << "Animation->Flag(" << anim.flags_length << ")";

			for (i = 0; i < anim.flags_length; i++) {
				if (!_block->readBuff(&af, sizeof(AnimationFlag))) {
					va.clear();
					return va;
				}
				anim.flags.push_back(af);
			}
			sLog << "\tLoadded: " << anim.flags.size() << "\tIndex: " << _block->index << std::endl;
		}

		va.push_back(anim);
	};

	sLog << "Animation(" << va.size() << ")" << std::endl;

	return va;
};

std::vector<Motion> loadMotion(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<Motion>();
	
	std::vector<Motion> vm;

	uint32_t length, i;
	if (!_block->readBuff(&length, sizeof(uint32_t))) {
		vm.clear();
		return vm;
	}
	sLog << "Motion(" << length << ")";

	Motion motion;

	for (i = 0; i < length; i++) {
		motion.frame_start = 0;
		motion.frame_end = 0;
		motion.connection_time = 0;
		motion.name.clear();
		motion.top_version.clear();
		motion.connection_method.clear();
		motion.nextmove.clear();

		if (!_block->readFixedString(motion.name)) {
			vm.clear();
			return vm;
		}
		if (!_block->readBuff(&motion.frame_start, sizeof(uint32_t))) {
			vm.clear();
			return vm;
		}
		if (!_block->readBuff(&motion.frame_end, sizeof(uint32_t))) {
			vm.clear();
			return vm;
		}
		if (!_block->readFixedString(motion.nextmove)) {
			vm.clear();
			return vm;
		}
		if (!_block->readFixedString(motion.connection_method)) {
			vm.clear();
			return vm;
		}
		if (!_block->readBuff(&motion.connection_time, sizeof(float))) {
			vm.clear();
			return vm;
		}
		if (!_block->readFixedString(motion.top_version)) {
			vm.clear();
			return vm;
		}
		vm.push_back(motion);
	}
	sLog << "\tLoadded: " << vm.size() << std::endl;

	return vm;
};

std::vector<CollisionBox> loadCollisionBox(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<CollisionBox>();

	std::vector<CollisionBox> vcb;

	uint32_t length, i, j;
	if (!_block->readBuff(&length, sizeof(uint32_t))) {
		vcb.clear();
		return vcb;
	}
	sLog << "CollisionBox(" << length << ")";

	CollisionBox coll;
	std::string script;

	for (i = 0; i < length; i++) {
		coll.shape = 0;
		coll.show = 0;
		coll.scripts.clear();
		memset(&coll.area, 0, sizeof(Area));
		if (!_block->readBuff(&coll.shape, sizeof(uint32_t))) {
			vcb.clear();
			return vcb;
		}
		if (!_block->readBuff(&coll.show, sizeof(uint32_t))) {
			vcb.clear();
			return vcb;
		}
		for (j = 0; j < 4; j++) {
			script.clear();
			if (!_block->readFixedString(script)) {
				vcb.clear();
				return vcb;
			}
			coll.scripts.push_back(script);
		}
		if (!_block->readBuff(&coll.area, sizeof(Area))) {
			vcb.clear();
			return vcb;
		}
		vcb.push_back(coll);
	}
	sLog << "\tLoadded: " << vcb.size() << std::endl;

	return vcb;
};

std::vector<FaceAnimation> loadFaceAnimation(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<FaceAnimation>();

	std::vector<FaceAnimation> vfa;

	uint32_t length, i;
	if (!_block->readBuff(&length, sizeof(uint32_t))) {
		vfa.clear();
		return vfa;
	}
	sLog << "FaceAnimetion(" << length << ")";

	FaceAnimation face_animation;

	for (i = 0; i < length; i++) {
		if (!_block->readBuff(&face_animation, sizeof(FaceAnimation))) {
			vfa.clear();
			return vfa;
		}
		vfa.push_back(face_animation);
	}
	sLog << "\tLoadded: " << vfa.size() << std::endl;

	return vfa;
};

std::vector<SpecularMaterial> loadSpecularMaterial(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<SpecularMaterial>();

	std::vector<SpecularMaterial> vsm;

	uint32_t  length, i, j;
	if (!_block->readBuff(&length, sizeof(uint32_t))) {
		vsm.clear();
		return vsm;
	}
	sLog << "SpecularMaterial(" << length << ")";

	SpecularMaterial smtl;
	specular_value value;
	uint32_t value_length, value_type;

	for (i = 0; i < length; i++) {
		smtl.name.clear();
		smtl.specular_values.clear();

		if (!_block->readFixedString(smtl.name)) {
			vsm.clear();
			return vsm;
		}

		if (!_block->readBuff(&value_length, sizeof(uint32_t))) {
			vsm.clear();
			return vsm;
		}

		for (j = 0; j < value_length; j++) {
			value.name.clear();
			value.sTEXTURE.clear();
			value.type = (uint32_t)eSPECULAR_MATERIAL_TYPE::SMT_NUM;

			if (!_block->readFixedString(value.name)) {
				vsm.clear();
				return vsm;
			}
			if (!_block->readBuff(&value.type, sizeof(uint32_t))) {
				vsm.clear();
				return vsm;
			}

			switch (value.type) {
				case eSPECULAR_MATERIAL_TYPE::SMT_BOOL:
					if (!_block->readBuff(&value.bBOOL, sizeof(uint32_t))) {
						vsm.clear();
						return vsm;
					}
					break;
				case eSPECULAR_MATERIAL_TYPE::SMT_INT:
					if (!_block->readBuff(&value.iINT, sizeof(int32_t))) {
						vsm.clear();
						return vsm;
					}
					break;
				case eSPECULAR_MATERIAL_TYPE::SMT_FLOAT:
					if (!_block->readBuff(&value.fFLOAT, sizeof(float))) {
						vsm.clear();
						return vsm;
					}
					break;
				case eSPECULAR_MATERIAL_TYPE::SMT_VECTOR2:
					if (!_block->readBuff(&value.vVECTOR2, sizeof(Vector2d))) {
						vsm.clear();
						return vsm;
					}
					break;
				case eSPECULAR_MATERIAL_TYPE::SMT_VECTOR3:
					if (!_block->readBuff(&value.vVECTOR3, sizeof(Vector3d))) {
						vsm.clear();
						return vsm;
					}
					break;
				case eSPECULAR_MATERIAL_TYPE::SMT_VECTOR4:
					if (!_block->readBuff(&value.vVECTOR4, sizeof(Vector4d))) {
						vsm.clear();
						return vsm;
					}
					break;
				case eSPECULAR_MATERIAL_TYPE::SMT_MATRIX:
					if (!_block->readBuff(&value.mMATRIX, sizeof(Matrix4))) {
						vsm.clear();
						return vsm;
					}
					break;
				case eSPECULAR_MATERIAL_TYPE::SMT_TEXTURE:
					if (!_block->readFixedString(value.sTEXTURE)) {
						vsm.clear();
						return vsm;
					}
					break;
			}
			smtl.specular_values.push_back(value);
		}
		vsm.push_back(smtl);
	}
	sLog << "\tLoadded: " << vsm.size() << std::endl;

	return vsm;
};

std::vector<Extra> loadExtra(Block* _block, size_t _size) {

	if (_block == nullptr)
		return std::vector<Extra>();

	std::vector<Extra> extras;

	uint32_t length, i;
	if (!_block->readBuff(&length, sizeof(uint32_t))) {
		extras.clear();
		return extras;
	}
	sLog << "Extra(" << length << ")";

	Extra extra;

	for (i = 0; i < length; i++) {
		if (!_block->readBuff(&extra.bone_id, sizeof(uint8_t))) {
			extras.clear();
			return extras;
		}
		extra.bone_id &= 0xFF;
		if (extra.bone_id == 0xFF)
			extra.bone_id = -1;
		else if (extra.bone_id == 0xFE) {
			if (!_block->readBuff(&extra.bone_id, sizeof(int16_t))) {
				extras.clear();
				return extras;
			}
		}
		extras.push_back(extra);
	}
	sLog << "\tLoadded: " << extras.size() << std::endl;

	return extras;
};

Matrix4 calc_bonemat(std::vector<Bone>& _vb, uint16_t _bone_id
#if PRINT_LOG_MATRIX_FUNCTIONS == true
, FileLog& _log
#endif
) {
	Matrix4 m4 = {};

#if PRINT_LOG_MATRIX_FUNCTIONS == true
	_log << "bone_id: " << _bone_id << std::endl;
	_log << m4.toString() << std::endl;
#endif

	while (_bone_id != (uint16_t)-1) {
		m4 = m4.multiply(Matrix4(_vb[_bone_id].matrix), m4);
#if PRINT_LOG_MATRIX_FUNCTIONS == true
		_log << m4.toString() << std::endl;
#endif
		_bone_id = _vb[_bone_id].parent;
	}

	return m4;
};

int main(int _argc, char* _argv[]) {
	char* file = (char*)default_file;

	if (_argc > 1)
		file = _argv[1];

	FileObject fo(file);

	if (!fo.state) {
		sLog << "Fail to open file: " << file << std::endl;
		return -1;
	}

	sLog << "File: " << fo.name << ", length: " << fo.size << std::endl;

	setFileType(fo.name);

	sLog << "File Type: " << getFileType() << std::endl;

	BlockModelCtx bmc{0};
	std::vector<BlockModelCtx> bmcs;

	while (readPETModelBlock(fo, &bmc)) {
		bmcs.push_back(bmc);
		sLog << "Model:\n\tName: " << bmc.model.getId() << "\tLength: " << bmc.model.length << "\tIndex: " << bmc.index << std::endl;
	}

	Block block;
	std::vector<Bone> bones;
	std::vector<Texture> textures;

	for (auto& bmcr : bmcs) {
		sLog << "Loading " << bmcr.model.getId() << std::endl;
		block.init(bmcr.model.length);
		if (!fo.peekBlock(block.buf, bmcr.model.length, bmcr.index))
			continue;
		if (bmcr.model.getId().compare("VERS") == 0) {
			auto version = loadVersion(&block, bmcr.model.length);
			if (version != nullptr) {
				sLog << "Version: " << version->toString() << std::endl;
				setVersion(*version);
				delete version;
			}
		}else if (bmcr.model.getId().compare("MESH") == 0) {
			auto mesh = loadMesh(&block, bmcr.model.length);
			if (mesh != nullptr) {
				sLog << "Mesh: " << mesh->toString() << std::endl;
				FileLog mtlLog(true, "pippin2.mtl");
				float r,g,b;
				for (auto& mtl : textures) {
					r = ((mtl.diffuse & 0xFF00 >> 8) / 255.f);
					g = ((mtl.diffuse & 0xFF0000 >> 16) / 255.f);
					b = ((mtl.diffuse & 0xFF000000 >> 24) / 255.f);
					mtlLog << "newmtl " << mtl.name << std::endl;
					mtlLog << "Kd " << r << " " << g << " " << b << std::endl;
					mtlLog << "Ka " << r << " " << g << " " << b << std::endl;
					mtlLog << "Ks 0.0 0.0 0.0" << std::endl;
					mtlLog << "Ns 5.0" << std::endl;
					mtlLog << "Ke 0.0 0.0 0.0" << std::endl;
					mtlLog << "Ni 1.0" << std::endl;
					mtlLog << "illum 2" << std::endl;
					mtlLog << "map_Kd " << mtl.name << std::endl;
					if (mtl.name[0] == '[')
						mtlLog << "map_d " << ([](const char* _n) -> std::string {
							std::string tmp = _n;

							auto pos = tmp.find_last_of(".");

							if (pos == tmp.npos)
								return (tmp + "_mask");
							
							return (tmp.substr(0, pos) + "_mask" + tmp.substr(pos));
						})(mtl.name) << std::endl;
					mtlLog << std::endl;
				}
				FileLog tmpLog(true, "pippin2.obj");
				tmpLog << "mtllib " << "pippin2.mtl" << std::endl;
				tmpLog << "o " << fo.name << std::endl;
				Matrix4 m4t = {
					Vector4d{1, 0, 0, 0},
					Vector4d{0, 1, 0, 0},
					Vector4d{0, 0, -1, 0},
					Vector4d{0, 0, 0, 1}
				};
				Vector4d vZero = {0.f, 0.f, 0.f, 1.f};
				for (auto& vertx : mesh->vertices) {
					Vector4d v4 = {0};
					Vector4d vpos = {vertx.v.x, vertx.v.y, vertx.v.z, 1};
#if PRINT_LOG_MATRIX_FUNCTIONS == true
					tmpLog << v4.toString() << std::endl;
					auto matb = calc_bonemat(bones, vertx.bone_infos[0].id, tmpLog);
					tmpLog << matb.toString() << std::endl;
					tmpLog << vpos.toString() << std::endl;
#else
					auto matb = calc_bonemat(bones, vertx.bone_infos[0].id);
#endif
					v4.add(m4t.multiply(matb.multiply(vpos).multiply(1.f / 255 * vertx.bone_infos[0].weight)));
#if PRINT_LOG_MATRIX_FUNCTIONS == true
					tmpLog << v4.toString() << std::endl;
#endif
					tmpLog << "v " << (v4.x / v4.w) << " " << (v4.y / v4.w) << " " << (v4.z / v4.w) << std::endl;
#if PRINT_LOG_MATRIX_FUNCTIONS == true
					tmpLog << matb.inverse().multiply(m4t.multiply(v4)).toString() << std::endl;
					break;
#endif
				}
				for (auto& faces : mesh->polygons) {
					for (uint32_t ii = 0; ii < 3; ii++) {
						tmpLog << "vn " << faces.pet_tri_points[ii].v.x << " " << faces.pet_tri_points[ii].v.z
							<< " " << faces.pet_tri_points[ii].v.y << std::endl;
					}
				}
				for (auto& vts : mesh->polygons) {
					for (uint32_t ii = 0; ii < 3; ii++) {
						tmpLog << "vt " << vts.pet_tri_points[ii].uv_mappings[0].v.x << 
							" " << (1.f - vts.pet_tri_points[ii].uv_mappings[0].v.y) << std::endl;
					}
				}
				int ii = 1;
				tmpLog << "s 1" << std::endl;
				uint8_t curr_txt_idx = 255;
				for (uint32_t k = 0; k < mesh->polygon_length; k++) {
					auto& faces = mesh->polygons[k];
					if (curr_txt_idx == 255 || curr_txt_idx != mesh->texture_indexs[k].index) {
						curr_txt_idx = mesh->texture_indexs[k].index;
						tmpLog << "usemtl " << textures[curr_txt_idx].name << std::endl;
					}
					tmpLog << "f " << faces.pet_tri_points[0].pos + 1 << "/" << ii << "/" << ii
						<< " " << faces.pet_tri_points[1].pos + 1 << "/" << ii + 1 << "/" << ii + 1
						<< " " << faces.pet_tri_points[2].pos + 1 << "/" << ii + 2 << "/" << ii + 2 << std::endl;
					ii += 3;
				}
				delete mesh;
			}
		}else if (bmcr.model.getId().compare("SMTL") == 0) {
			auto smtls = loadSpecularMaterial(&block, bmcr.model.length);
			if (smtls.size() > 0) {
				sLog << "Specular Materials:" << std::accumulate(smtls.begin(), smtls.end(), std::string(), toStringVectorOp) << std::endl;
				smtls.clear();
				smtls.shrink_to_fit();
			}
		}else if (bmcr.model.getId().compare("FRAM") == 0) {
			auto frames = loadFrame(&block, bmcr.model.length);
			if (frames.size() > 0) {
				sLog << "Frames:" << std::accumulate(frames.begin(), frames.end(), std::string(), toStringVectorOp) << std::endl;
				frames.clear();
				frames.shrink_to_fit();
			}
		}else if (bmcr.model.getId().compare("TEXT") == 0) {
			textures = loadTexture(&block, bmcr.model.length);
			if (textures.size() > 0) {
				sLog << "Textures:" << std::accumulate(textures.begin(), textures.end(), std::string(), toStringVectorOp) << std::endl;
				//textures.clear();
				//textures.shrink_to_fit();
			}
		}else if (bmcr.model.getId().compare("BONE") == 0) {
			bones = loadBone(&block, bmcr.model.length);
			if (bones.size() > 0) {
				sLog << "Bones:" << std::accumulate(bones.begin(), bones.end(), std::string(), toStringVectorOp) << std::endl;
				//bones.clear();
				//bones.shrink_to_fit();
			}
		}else if (bmcr.model.getId().compare("ANIM") == 0) {
			auto animations = loadAnimation(&block, bmcr.model.length);
			if (animations.size() > 0) {
				sLog << "Animations:" << std::accumulate(animations.begin(), animations.end(), std::string(), toStringVectorOp) << std::endl;
				animations.clear();
				animations.shrink_to_fit();
			}
		}else if (bmcr.model.getId().compare("MOTI") == 0) {
			auto motions = loadMotion(&block, bmcr.model.length);
			if (motions.size() > 0) {
				sLog << "Motions:" << std::accumulate(motions.begin(), motions.end(), std::string(), toStringVectorOp) << std::endl;
				motions.clear();
				motions.shrink_to_fit();
			}
		}else if (bmcr.model.getId().compare("COLL") == 0) {
			auto collisions = loadCollisionBox(&block, bmcr.model.length);
			if (collisions.size() > 0) {
				sLog << "CollisionBoxs:" << std::accumulate(collisions.begin(), collisions.end(), std::string(), toStringVectorOp) << std::endl;
				collisions.clear();
				collisions.shrink_to_fit();
			}
		}else if (bmcr.model.getId().compare("FANM") == 0) {
			auto face_animations = loadFaceAnimation(&block, bmcr.model.length);
			if (face_animations.size() > 0) {
				sLog << "FaceAnimations:" << std::accumulate(face_animations.begin(), face_animations.end(), std::string(), toStringVectorOp) << std::endl;
				face_animations.clear();
				face_animations.shrink_to_fit();
			}
		}else if (bmcr.model.getId().compare("EXTR") == 0) {
			auto extras = loadExtra(&block, bmcr.model.length);
			if (extras.size() > 0) {
				sLog << "Extras:" << std::accumulate(extras.begin(), extras.end(), std::string(), toStringVectorOp) << std::endl;
				extras.clear();
				extras.shrink_to_fit();
			}
		}
	}

	return 0;
}
