// Arquivo gbin_to_text.cpp
// Criado em 03/07/2023 as 18:25 por Acrisio
// Definição e Implementação do Tradutor dos arquivos .gbin e .aibin do PangYa do PC todas as versões

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <numeric>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <ctime>

#define DISABLE_LOG false

auto toStringVectorOp = [](std::string _accumulate, auto& _value) -> std::string {
	return std::move(_accumulate) + "\n" + _value.toString();
};

auto toStringVectorRawOp = [](std::string _accumulate, auto& _value) -> std::string {
	return std::move(_accumulate) + (_accumulate.empty() ? "" : ", ") + std::to_string(_value);
};

std::string BufferToHexString(unsigned char* buffer, size_t size) {
    std::stringstream ss;

     size_t offset = 0, i = 0, j = 0;

	 ss << std::endl;
	
	//ss << "ADDR 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F \tASCII            \n";
	for(i = 0; i < size; i++) {
		// Offset
		if((i % 16) == 0) {
			ss << std::uppercase << std::hex << std::setw(4) << std::setfill('0');
			ss << offset;
			ss << " ";

			offset += 16;
		}

		// Body
		ss << std::uppercase << std::hex << std::setw(2) << std::setfill('0');
		ss << (int)buffer[i];
		ss << " ";

		if(((i + 1) % 16) == 0) {
			ss << std::nouppercase << std::dec << "\t";
			for(j = (i - 15); j < (i + 1); j++) {
				if((buffer[j] >= 32) && (buffer[j] <= 126))
					ss << buffer[j];
				else
					ss << ".";
			}
			ss << std::endl;
		}
	}

	// Resto
	if((size % 16) != 0) {
		for(i = 0; i < (16 - (size % 16)); i++) {
			ss << std::uppercase << std::hex << std::setw(2) << std::setfill('-');
			ss << "-";
			ss << " ";
		}

		ss << std::nouppercase << std::dec << "\t";
		for(i = (size - (size % 16)); i < size; i++) {
			if((buffer[i] >= 32) && (buffer[i] <= 126))
				ss << buffer[i];
			else
				ss << ".";
		}

		for(i = 0; i < (16 - (size % 16)); i++)
			ss << ".";
		ss << std::endl;
	}

	ss.seekg(0, std::ios_base::end);
	
	size_t size_str = (size_t)ss.tellg();
	
	ss.seekg(0, std::ios_base::beg);

	char *buffer_str = new char[size_str];

	ss.read(buffer_str, size_str);

	std::string str(buffer_str, size_str);

	delete[] buffer_str;

	return str;
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
		std::string str = "Log Gbin(";

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

    std::string getExtsion() {

        std::string ret = "";

        char *ptr = strrchr(name, '.');

        if (ptr != nullptr && ptr >= name)
            ret.assign(ptr, ptr + strlen(ptr));

        return ret;
    }

	bool read(char* _buf, size_t _size) {

		in.read(_buf, _size);

		index += in.gcount();

		if (!in || in.gcount() != _size)
			return false;	

		return true;
	}

    bool readFixedString(std::string& _str) {

		if (index >= size)
			return false;
		
		uint32_t length;
		if (!read((char*)&length, sizeof(uint32_t)))
			return false;
		
		if (length < 0)
			return false;
		
		if ((index + length) > size)
			return false;
		
		_str.clear();
		_str.resize(length);

		if (length == 0)
			return true;

		if (!read((char*)_str.data(), length))
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

    bool resetIndex(size_t _index = 0u) {

        if (_index > size)
            return false;
        
        in.seekg(_index, std::ios_base::beg);

        index = _index;

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

struct BlockCtx {
    size_t index;
    size_t length;
};

// Packed
#pragma pack(1)

struct HeaderV71 {
    char magic[4];
    uint32_t version;
    uint32_t num_element_global;
    uint32_t num_element_type[2];
    uint32_t num_camera;
    uint32_t num_light;
    uint32_t num_sound_box; // sound, npc, wind, extras
    uint32_t num_texture;
    uint32_t num_node;
    void clear() {
        memset(this, 0, sizeof(HeaderV71));
    };
    std::string toString() {
        return "magic: " + std::string(magic, magic + sizeof(magic))
            + "\nversion: " + std::to_string(version)
            + "\nnmum_element_global: " + std::to_string(num_element_global)
            + "\nnum_element_type[0]: " + std::to_string(num_element_type[0])
            + "\nnum_element_type[1]: " + std::to_string(num_element_type[1])
            + "\nnum_camera: " + std::to_string(num_camera)
            + "\nnum_light: " + std::to_string(num_light)
            + "\nnum_sound_box: " + std::to_string(num_sound_box)
            + "\nnum_texture: " + std::to_string(num_texture)
            + "\nnum_node: " + std::to_string(num_node);
    }
};

struct HeaderV72 : public HeaderV71 {
    uint32_t num_new_element;
    void clear() {
        memset(this, 0, sizeof(HeaderV72));
    };
    std::string toString() {
        return HeaderV71::toString()
            + "\nnum_new_element: " + std::to_string(num_new_element);
    }
};

struct ExtraValueV72 {
    int32_t guid;
    int32_t unknown2;
    void clear() {
        memset(this, 0, sizeof(ExtraValueV72));
    };
    std::string toString() {
        return "guid: " + std::to_string(guid)
            + "\nunknown2: " + std::to_string(unknown2);
    }
};

struct CameraV71 {
    char name[32];
    Vector3d pos;
    Vector3d dest;
    float fov;
    float bank;
    void clear() {
        memset(this, 0, sizeof(CameraV71));
    };
    std::string toString() {
        return "name: " + std::string(name)
            + "\npos: " + pos.toString()
            + "\ndest: " + dest.toString()
            + "\nfov: " + std::to_string(fov)
            + "\nbank: " + std::to_string(bank);
    }
};

struct CameraV72 : public CameraV71 {
    Vector3d pos2;
    Vector3d dest2;
    ExtraValueV72 extra_value;
    void clear() {
        memset(this, 0, sizeof(CameraV72));
    };
    std::string toString() {
        return CameraV71::toString()
            + "\npos2: " + pos2.toString()
            + "\ndest2: " + dest2.toString()
            + "\nextra_value: " + extra_value.toString();
    }
};

struct LightV71 {
    uint8_t type;
    char name[32];
    Vector3d pos;
    char data[64];
    void clear() {
        memset(this, 0, sizeof(LightV71));
    };
    std::string toString() {
        return "type: " + std::to_string((uint16_t)type)
            + "\nname: " + std::string(name)
            + "\npos: " + pos.toString()
            + "\ndata: " + std::string(data);
    }
};

struct LightV72 : public LightV71 {
    Vector3d pos2;
    ExtraValueV72 extra_value;
    void clear() {
        memset(this, 0, sizeof(LightV72));
    };
    std::string toString() {
        return LightV71::toString()
            + "\npos2: " + pos2.toString()
            + "\nextra_value: " + extra_value.toString();
    }
};

struct SoundBoxV71 {
    uint8_t type;
    char name[64];
    Area min_max;
    void clear() {
        memset(this, 0, sizeof(SoundBoxV71));
    };
    std::string toString() {
        return "type: " + std::to_string((uint16_t)type)
            + "\nname: " + std::string(name)
            + "\nmin_max: " + min_max.toString();
    }
};

struct SoundBoxV72 : public SoundBoxV71 {
    Area min_max2;
    ExtraValueV72 extra_value;
    void clear() {
        memset(this, 0, sizeof(SoundBoxV72));
    };
    std::string toString() {
        return SoundBoxV71::toString()
            + "\nmin_max2: " + min_max2.toString()
            + "\nextra_value: " + extra_value.toString();
    }
};

struct TextureV71 {
    char name[32];
    void clear() {
        memset(this, 0, sizeof(TextureV71));
    };
    std::string toString() {
        return "name: " + std::string(name);
    }
};

struct NodeBase {
    virtual ~NodeBase() {};
    virtual void clear() = 0;
    virtual size_t getSize() = 0;
    virtual char* getName() = 0;
    virtual uint32_t getNumVect() = 0;
    virtual uint32_t getType() = 0;
    virtual std::vector< Vector3d >& getVects() = 0;
    virtual void pushVect(Vector3d& _vect) = 0;
};

struct NodeV71 : public NodeBase {
    char name[16];
    uint32_t num_vect;
    uint32_t type;
    std::vector< Vector3d > vects;
    NodeV71(uint32_t _ul = 0u) {
        (_ul);
        clear();
    };
    virtual ~NodeV71() {
        clear();
    };
    virtual void clear() override {
        memset(name, 0, sizeof(name));
        num_vect = 0u;
        type = 0u;
        vects.clear();
    };
    virtual size_t getSize() override {
        return sizeof(name) + sizeof(num_vect) + sizeof(type);
    };
    virtual char* getName() override {
        return name;
    };
    virtual uint32_t getNumVect() override {
        return num_vect;
    };
    virtual uint32_t getType() override {
        return type;
    };
    virtual std::vector< Vector3d >& getVects() override {
        return vects;
    };
    virtual void pushVect(Vector3d& _vect) override {
        vects.push_back(_vect);
    };
    std::string toString() {
        return "name: " + std::string(name)
            + "\nnum_vect: " + std::to_string(num_vect)
            + "\ntype: " + std::to_string(type)
            + "\nvects: " + std::accumulate(vects.begin(), vects.end(), std::string(), toStringVectorOp);
    }
};

struct NodeV72 : public NodeBase {
    char name[32];
    uint32_t num_vect;
    uint32_t type;
    std::vector< Vector3d > vects;
    NodeV72(uint32_t _ul = 0u) {
        (_ul);
        clear();
    };
    NodeV72(char *_name, uint32_t _num_vect, uint32_t _type, std::vector< Vector3d >& _vects) {
        strncpy(name, _name, sizeof(name) - 1);
        num_vect = _num_vect;
        type = _type;
        vects = _vects;
    };
    virtual ~NodeV72() {
        clear();
    };
    virtual void clear() override {
        memset(name, 0, sizeof(name));
        num_vect = 0u;
        type = 0u;
        vects.clear();
    };
    virtual size_t getSize() override {
        return sizeof(name) + sizeof(num_vect) + sizeof(type);
    };
    virtual char* getName() override {
        return name;
    };
    virtual uint32_t getNumVect() override {
        return num_vect;
    };
    virtual uint32_t getType() override {
        return type;
    };
    virtual std::vector< Vector3d >& getVects() override {
        return vects;
    };
    virtual void pushVect(Vector3d& _vect) override {
        vects.push_back(_vect);
    };
    std::string toString() {
        return "name: " + std::string(name)
            + "\nnum_vect: " + std::to_string(num_vect)
            + "\ntype: " + std::to_string(type)
            + "\nvects: " + std::accumulate(vects.begin(), vects.end(), std::string(), toStringVectorOp);
    }
};

struct ElementV71 {
    union {
        uint32_t ul_option;
        struct {
            // ele não está distribuidos assim pelo que vi no s4 qa
            uint32_t unknown1 : 8;
            uint32_t unknown2 : 8;
            uint32_t unknown3 : 8;
            uint32_t unknown4 : 8;
        }bit_fileds;
    }u_option;
    uint32_t vtxNum;
    char name[32];
    Matrix matrix1;
    Matrix matrix2;
    uint32_t course_type; // type[0], type[1] (1 - Blue Lagoon, 2 - Blue Water)
    char class_name[32];
    void clear() {
        memset(this, 0, sizeof(ElementV71));
    };
    std::string toString() {
        return "u_option: " + std::to_string(u_option.ul_option)
            + " ("
                + std::to_string(u_option.bit_fileds.unknown1)
                + ", " + std::to_string(u_option.bit_fileds.unknown2)
                + ", " + std::to_string(u_option.bit_fileds.unknown3)
                + ", " + std::to_string(u_option.bit_fileds.unknown4)
            + ")"
            + "\nvtxNum: " + std::to_string(vtxNum)
            + "\nname: " + std::string(name)
            + "\nmatrix1: " + matrix1.toString()
            + "\nmatrix2: " + matrix2.toString()
            + "\ncourse_type: " + std::to_string(course_type)
            + "\nclass_name: " + std::string(class_name);
    }
};

struct ElementV72 : public ElementV71 {
    Matrix matrix2_v72;
    ExtraValueV72 extra_value;
    void clear() {
        memset(this, 0, sizeof(ElementV72));
    };
    std::string toString() {
        return ElementV71::toString()
            + "\nmatrix2_v72: " + matrix2_v72.toString()
            + "\nextra_value: " + extra_value.toString();
    }
};

struct NewElementV72 {
    ExtraValueV72 extra_value;
    char name[64];
    Matrix matrix1;
    Matrix matrix2;
    uint32_t type;
    void clear() {
        memset(this, 0, sizeof(NewElementV72));
    };
    std::string toString() {
        return "extra_value: " + extra_value.toString()
            + "\nname: " + std::string(name)
            + "\nmatrix1: " + matrix1.toString()
            + "\nmatrix2: " + matrix2.toString()
            + "\ntype: " + std::to_string(type);
    }
};

struct PointXZ {
    float x;
    float z;
    void clear() {
        memset(this, 0, sizeof(PointXZ));
    };
    std::string toString() {
        return "x: " + std::to_string(x)
            + ", z: " + std::to_string(z);
    };
};

// fast par hole, tee and pin check in gbin
struct MapCheckV71 {
    uint8_t par_hole;
    PointXZ tee_type[2];
    PointXZ pin_type[2][3];
    void clear() {
        memset(this, 0, sizeof(MapCheckV71));
    };
    std::string toString(){
        return "par_hole: " + std::to_string((uint16_t)par_hole)
            + "\ntee_type[0]: " + tee_type[0].toString()
            + "\ntee_type[1]: " + tee_type[1].toString()
            + "\npin_type[0][0~2]: {\npin[0]: " + pin_type[0][0].toString()
            + ",\npin[1]: " + pin_type[0][1].toString()
            + ",\npin[2]: " + pin_type[0][2].toString()
            + "\n}\npin_type[1][0~2]: {\npin[0]: " + pin_type[1][0].toString()
            + ",\npin[1]: " + pin_type[1][1].toString()
            + ",\npin[2]: " + pin_type[1][2].toString()
            + "\n}";
    };
};

// fast par_hole check in gbin
struct MapCheckV72 {
    uint8_t par_hole;
    void clear() {
        memset(this, 0, sizeof(MapCheckV72));
    };
    std::string toString() {
        return "par_hole: " + std::to_string((uint16_t)par_hole);
    };
};

struct MapColorVtx {
    uint32_t map_color_vtx[3];
    void clear() {
        memset(this, 0, sizeof(MapColorVtx));
    };
    std::string toString() {
        return "map_color_vtx[0~2]: {" + std::to_string(map_color_vtx[0])
            + ", " + std::to_string(map_color_vtx[1])
            + ", " + std::to_string(map_color_vtx[2])
            + "}";
    };
};

struct ElementBaseFaceMapColorVtxV71 {
    std::vector< MapColorVtx > map;
    void clear() {
        map.clear();
    };
    std::string toString() {
        return "map(" + std::to_string(map.size()) + "): " + std::accumulate(map.begin(), map.end(), std::string(), toStringVectorOp);
    };
};

struct ElementBaseFaceMapColorVtxV72 {
    std::map< std::string, std::vector< uint32_t > > map;
    void clear() {
        map.clear();
    };
    std::string toString() {

        std::string ret = "map(" + std::to_string(map.size()) + "):\n{\n";

        for (auto& el : map) {
            ret += "\nname: " + el.first;
            ret += "\nvalues(" + std::to_string(el.second.size()) + "):\n{\n";
            ret += std::accumulate(el.second.begin(), el.second.end(), std::string(), toStringVectorRawOp);
            ret += "\n}";
        }

        ret += "\n}";

        return ret;
    };
};

// unpacked
#pragma pack()

struct GbinCtx {
    HeaderV72 header;
    std::vector< CameraV72 > cameras;
    std::vector< LightV72 > lights;
    std::vector< SoundBoxV72 > sound_boxs;
    std::vector< TextureV71 > textures;
    std::vector< NodeV72 > nodes;
    std::vector< ElementV72 > elements;
    std::vector< NewElementV72 > new_elements;
    ElementBaseFaceMapColorVtxV71 element_base_face_map_color_vtx_v71;
    ElementBaseFaceMapColorVtxV72 element_base_face_map_color_vtx_v72;
    ElementV72 element_base;
    MapCheckV71 map_check_v71;
    MapCheckV72 map_check_v72;
    void clear() {
        header.clear();
        cameras.clear();
        lights.clear();
        sound_boxs.clear();
        textures.clear();
        nodes.clear();
        elements.clear();
        new_elements.clear();
        element_base_face_map_color_vtx_v71.clear();
        element_base_face_map_color_vtx_v72.clear();
        element_base.clear();
        map_check_v71.clear();
        map_check_v72.clear();
    };
    std::string toString() {
        return "header: " + header.toString()
            + "\ncameras("
                + std::to_string(cameras.size())
            + "): " + std::accumulate(cameras.begin(), cameras.end(), std::string(), toStringVectorOp)
            + "\nlights("
                + std::to_string(lights.size())
            + "): " + std::accumulate(lights.begin(), lights.end(), std::string(), toStringVectorOp)
            + "\nsound_boxs("
                + std::to_string(sound_boxs.size())
            + "): " + std::accumulate(sound_boxs.begin(), sound_boxs.end(), std::string(), toStringVectorOp)
            + "\ntextures("
                + std::to_string(textures.size())
            + "): " + std::accumulate(textures.begin(), textures.end(), std::string(), toStringVectorOp)
            + "\nnodes("
                + std::to_string(nodes.size())
            + "): " + std::accumulate(nodes.begin(), nodes.end(), std::string(), toStringVectorOp)
            + "\nelement_base: " + element_base.toString()
            + "\nelement_base_face_map_color_vtx:" + (
                header.version < 0x72 
                    ? element_base_face_map_color_vtx_v71.toString()
                    : element_base_face_map_color_vtx_v72.toString()
            )
            + "\nelements("
                + std::to_string(elements.size())
            + "): " + std::accumulate(elements.begin(), elements.end(), std::string(), toStringVectorOp)
            + "\nnew_elements("
                + std::to_string(new_elements.size())
            + "): " + std::accumulate(new_elements.begin(), new_elements.end(), std::string(), toStringVectorOp)
            + "\nmap_check: " + (
                header.version < 0x72
                    ? map_check_v71.toString()
                    : map_check_v72.toString()
            );
    }
};

bool loadGBin(FileObject& _fo, GbinCtx& _gbin) {

    _gbin.clear();

    if (_fo.index >= _fo.size)
        return false;
    
    if ((_fo.index + sizeof(HeaderV71)) > _fo.size)
        return false;
    
    if (!_fo.read((char*)&_gbin.header, sizeof(HeaderV71)))
        return false;

    if (strncmp(_gbin.header.magic, "WEPX", 4) != 0)
        return false;
    
    sLog << "Version: " << _gbin.header.version;
    
    if (_gbin.header.version > 0x71
            && !_fo.read((char*)&_gbin.header.num_new_element, sizeof(uint32_t)))
        return false;
    
    sLog << " Index: " << _fo.index << std::endl;
    
    uint32_t i;

    sLog << "Loading Camera(" << _gbin.header.num_camera << ")";

    CameraV72 cam;

    for (i = 0u; i < _gbin.header.num_camera; i++) {
        cam.clear();

        if (!_fo.read((char*)&cam, sizeof(CameraV71)))
            return false;

        if (_gbin.header.version > 0x71
                && !_fo.read((char*)(&cam) + sizeof(CameraV71), sizeof(CameraV72) - sizeof(CameraV71)))
            return false;

        _gbin.cameras.push_back(cam);
    }

    sLog << " Loadded(" << _gbin.cameras.size() << ")" << " Index: " << _fo.index << std::endl;

    sLog << "Loading Light(" << _gbin.header.num_light << ")";

    LightV72 light;

    for (i = 0u; i < _gbin.header.num_light; i++) {
        light.clear();

        if (!_fo.read((char*)&light, sizeof(LightV71) - sizeof(light.data)))
            return false;
        
        if (light.type != 0u
                && !_fo.read((char*)light.data, sizeof(light.data)))
            return false;
        
        if (_gbin.header.version > 0x71
                && !_fo.read((char*)(&light) + sizeof(LightV71), sizeof(LightV72) - sizeof(LightV71)))
            return false;
        
        _gbin.lights.push_back(light);
    }

    sLog << " Loadded(" << _gbin.lights.size() << ")" << " Index: " << _fo.index << std::endl;

    sLog << "Loading SoundBox(" << _gbin.header.num_sound_box << ")";

    SoundBoxV72 sound_box;

    for (i = 0u; i < _gbin.header.num_sound_box; i++) {
        sound_box.clear();

        if (!_fo.read((char*)&sound_box, sizeof(SoundBoxV71)))
            return false;
        
        if (_gbin.header.version > 0x71
                && !_fo.read((char*)(&sound_box) + sizeof(SoundBoxV71), sizeof(SoundBoxV72) - sizeof(SoundBoxV71)))
            return false;
        
        _gbin.sound_boxs.push_back(sound_box);
    }

    sLog << " Loadded(" << _gbin.sound_boxs.size() << ")" << " Index: " << _fo.index  << std::endl;

    sLog << "Loading Texture(" << _gbin.header.num_texture << ")";

    TextureV71 texture;

    for (i = 0u; i < _gbin.header.num_texture; i++) {
        texture.clear();

        if (!_fo.read((char*)&texture, sizeof(TextureV71)))
            return false;

        _gbin.textures.push_back(texture);
    }

    sLog << " Loadded(" << _gbin.textures.size() << ")" << " Index: " << _fo.index  << std::endl;

    sLog << "Loading Node(" << _gbin.header.num_node << ")";

    std::string file_ext = _fo.getExtsion();

    NodeBase *pNode = nullptr;
    Vector3d vec3;

    // Extra node point (tee) to short game, *extra (approach), *extra_shortgame(shortgame)
    // .aibin file, shortgame
    if (!file_ext.empty() && file_ext.compare(".aibin") == 0)
        pNode = new NodeV72();
    else
        pNode = new NodeV71();

    for (i = 0u; i < _gbin.header.num_node; i++) {
        pNode->clear();

        if (!_fo.read(pNode->getName(), pNode->getSize()))
            return false;

        if (pNode->getNumVect() > 0u) {

            for (uint32_t j = 0u; j < pNode->getNumVect(); j++) {
                if (!_fo.read((char*)&vec3, sizeof(Vector3d)))
                    return false;
                pNode->pushVect(vec3);
            }
        }

        _gbin.nodes.push_back(NodeV72(pNode->getName(), pNode->getNumVect(), pNode->getType(), pNode->getVects()));
    }

    if (pNode != nullptr)
        delete pNode;

    pNode = nullptr;

    sLog << " Loadded(" << _gbin.nodes.size() << ")" << " Index: " << _fo.index << std::endl;

    if ((_gbin.header.num_element_global + _gbin.header.num_element_type[0] + _gbin.header.num_element_type[1]) > 0u) {

        sLog << "Loading Base Element";

        if (!_fo.read((char *)&_gbin.element_base, sizeof(ElementV71)))
            return false;

        if (_gbin.header.version > 0x71
                && !_fo.read((char *)(&_gbin.element_base) + sizeof(ElementV71), sizeof(ElementV72) - sizeof(ElementV71)))
            return false;

        sLog << " Loadded Index: " << _fo.index << std::endl;

        // Load Element base map color vtx
        if (_gbin.header.version < 0x72) {
            sLog << "Loading Element base map color vtx v71(" << _gbin.element_base.vtxNum << ")";

            MapColorVtx map_color_vtx;

            for (i = 0u; i < _gbin.element_base.vtxNum; i++) {
                map_color_vtx.clear();

                if (!_fo.read((char *)&map_color_vtx, sizeof(MapColorVtx)))
                    return false;

                _gbin.element_base_face_map_color_vtx_v71.map.push_back(map_color_vtx);
            }

            sLog << " Loadded(" << _gbin.element_base_face_map_color_vtx_v71.map.size() << ") Index: " << _fo.index << std::endl;

        }else{

            sLog << "Loading Element base map color vtx v72(";

            uint32_t j, k, l, h;

            if (!_fo.read((char *)&j, sizeof(uint32_t)))
                return false;

            sLog << j << ")";

            std::vector<uint32_t> map_color_vtx_single_coord;

            if (j > 0u) {
                std::string name_map_puppet;

                for (i = 0u; i < j; i++) {

                    name_map_puppet.clear();
                    map_color_vtx_single_coord.clear();

                    if (!_fo.readFixedString(name_map_puppet))
                        return false;

                    if (!_fo.read((char *)&k, sizeof(uint32_t)))
                        return false;

                    if (k > 0u) {

                        for (uint32_t a = 0u; a < k; a++) {
                            if (!_fo.read((char *)&l, sizeof(uint32_t)))
                                return false;

                            for (uint32_t b = 0u; b < l; b++) {
                                if (!_fo.read((char *)&h, sizeof(uint32_t)))
                                    return false;

                                map_color_vtx_single_coord.push_back(h);
                            }
                        }
                    }

                    _gbin.element_base_face_map_color_vtx_v72.map.insert(std::make_pair(name_map_puppet, map_color_vtx_single_coord));
                }
            }

            sLog << " Loadded(" << _gbin.element_base_face_map_color_vtx_v72.map.size() << ") Index: " << _fo.index << std::endl;
        }
    }

    sLog << "Loadding Element("
        << (_gbin.header.num_element_global + _gbin.header.num_element_type[0] + _gbin.header.num_element_type[1])
        << ")";

    ElementV72 element;

    for (i = 0u; i < (_gbin.header.num_element_global + _gbin.header.num_element_type[0] + _gbin.header.num_element_type[1]); i++) {
        element.clear();

        if (!_fo.read((char*)&element, sizeof(ElementV71)))
            return false;

        if (_gbin.header.version > 0x71
                && !_fo.read((char*)(&element) + sizeof(ElementV71), sizeof(ElementV72) - sizeof(ElementV71)))
            return false;
        
        _gbin.elements.push_back(element);
    }

    sLog << " Loadded(" << _gbin.elements.size() << ")" << " Index: " << _fo.index  << std::endl;

    if (_gbin.header.version > 0x71) {
        sLog << "Loading New Element(" << _gbin.header.num_new_element << ")";

        NewElementV72 new_element;

        for (i = 0u; i < _gbin.header.num_new_element; i++) {
            new_element.clear();

            if (!_fo.read((char*)&new_element, sizeof(NewElementV72)))
                return false;

            _gbin.new_elements.push_back(new_element);
        }

        sLog << " Loadded(" << _gbin.new_elements.size() << ")" << " Index: " << _fo.index  << std::endl;
    }

    // Map check section
    if ((_gbin.header.num_element_global + _gbin.header.num_element_type[0] + _gbin.header.num_element_type[1]) > 0u) {

        sLog << "Loading Map Check";

        if (_gbin.header.version < 0x72) {

            if (!_fo.read((char *)&_gbin.map_check_v71, sizeof(MapCheckV71)))
                return false;

        }else{

            if (!_fo.read((char *)&_gbin.map_check_v72, sizeof(MapCheckV72)))
                return false;

        }

        sLog << " Loadded Index: " << _fo.index << std::endl;
    }

    // Final index, to check right read file
    sLog << "Final Index: " << _fo.index << std::endl;

    return true;
}

int main(int _argc, char* _argv[]) {

    if (_argc < 2) {
        sLog << "Invalid parameters(" << _argc << "), " << std::endl;
        return -2;
    }

	char *file = _argv[1];

	FileObject fo(file);

	if (!fo.state) {
		sLog << "Fail to open file: " << file << std::endl;
		return -1;
	}

	sLog << "File: " << fo.name << ", length: " << fo.size << std::endl;

    if (fo.index >= fo.size)
        return -3;

    GbinCtx gbin;

    if (!loadGBin(fo, gbin))
        return -4;
    
    sLog << "gbin:\n" << gbin.toString() << std::endl;

    return 0;
}
