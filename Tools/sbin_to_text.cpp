// Arquivo sbin_to_text.cpp
// Criado em 01/07/2023 as 06:02 por Acrisio
// Definição e Implementação do Tradutor do arquivo .sbin do PangYa do PC todas as versões

#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <ctime>

#define DISABLE_LOG false

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

int32_t compareVersions(const Version& _v1, const Version& _v2) {

	if (_v1._v.full_version == _v2._v.full_version)
		return 0;
	
	if (_v1._v._s_0.major == _v2._v._s_0.major)
		return (_v1._v._s_0.minor < _v2._v._s_0.minor) ? -1 : 1;
	
	return (_v1._v._s_0.major < _v2._v._s_0.major) ? -1 : 1;
};

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
		std::string str = "Log Sbin(";

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

struct Vertex {
    uint32_t vtxNum;
    uint32_t vtxIndex;
    std::string toString() {
        return "vtxNum: " + std::to_string(vtxNum) + ", vtxIndex: " + std::to_string(vtxIndex);
    }
};

struct MapDDSValue {
    uint8_t values[16];
    std::string toString() {
        return "values:(" + std::to_string(sizeof(values)) 
            + ")\n" + BufferToHexString((unsigned char*)values, sizeof(values));
    }
};

struct TextureDDS {
    TextureDDS(uint32_t _ul = 0u) {
        (_ul);
        clear();
    };
    ~TextureDDS() {
        clear();
    };
    void clear() {
        width = 0u;
        height = 0u;
        numMapDDS = 0u;
        dataDDS.clear();
        mapDDS.clear();
    };
    std::string toString() {
        return "width: " + std::to_string(width)
            + ", height: " + std::to_string(height)
            + ", numMapDSS: " + std::to_string(numMapDDS)
            + ", dataDDS length: " + std::to_string(dataDDS.size())
            + ", mapDDS length: " + std::to_string(mapDDS.size())
            + "\ndataDDS: " + BufferToHexString((unsigned char*)dataDDS.data(), dataDDS.size())
            + "\nmapDDS: " + std::accumulate(mapDDS.begin(), mapDDS.end(), std::string(), toStringVectorOp);
    }
    uint32_t width;
    uint32_t height;
    uint32_t numMapDDS;
    std::vector<uint8_t> dataDDS;
    std::vector<MapDDSValue> mapDDS;
};

struct MeshShadowMap {
    uint32_t numMesh;
    uint32_t numMesh3d;
    uint32_t numMesh2d;
    uint32_t sizeTexture_global;
    uint32_t sizeTexture_type[2];
    uint32_t numTexture_global;
    uint32_t numTexture_type[2];
    std::vector<Vertex> vertex;
    std::vector<Vector3d> mesh3d;
    std::vector<Vector2d> mesh2d;
    std::vector<TextureDDS> texture_global;
    std::vector<TextureDDS> texture_type[2];
    std::string toString() {
        return "numMesh: " + std::to_string(numMesh)
            + ", numMesh3d: " + std::to_string(numMesh3d)
            + ", numMesh2d: " + std::to_string(numMesh2d)
            + ", sizeTexture_global: " + std::to_string(sizeTexture_global)
            + ", sizeTexture_type[0]: " + std::to_string(sizeTexture_type[0])
            + ", sizeTexture_type[1]: " + std::to_string(sizeTexture_type[1])
            + ", numTexture_global: " + std::to_string(numTexture_global)
            + ", numTexture_type[0]: " + std::to_string(numTexture_type[0])
            + ", numTexture_type[1]: " + std::to_string(numTexture_type[1])
            + "\nvertex length: " + std::to_string(vertex.size())
            + ", mesh3d length: " + std::to_string(mesh3d.size())
            + ", mesh2d length: " + std::to_string(mesh2d.size())
            + ", texture_global length: " + std::to_string(texture_global.size())
            + ", texture_type[0] length: " + std::to_string(texture_type[0].size())
            + ", texture_type[1] length: " + std::to_string(texture_type[1].size())
            + "\nvertex: " + std::accumulate(vertex.begin(), vertex.end(), std::string(), toStringVectorOp)
            + "\nmesh3d: " + std::accumulate(mesh3d.begin(), mesh3d.end(), std::string(), toStringVectorOp)
            + "\nmesh2d: " + std::accumulate(mesh2d.begin(), mesh2d.end(), std::string(), toStringVectorOp)
            + "\ntexture_global: " + std::accumulate(texture_global.begin(), texture_global.end(), std::string(), toStringVectorOp)
            + "\ntexture_type[0]: " + std::accumulate(texture_type[0].begin(), texture_type[0].end(), std::string(), toStringVectorOp)
            + "\ntexture_type[1]: " + std::accumulate(texture_type[1].begin(), texture_type[1].end(), std::string(), toStringVectorOp);
    }
};

struct OutFrag {
    uint32_t totalGlobal;
    uint32_t totalType[2];
    std::vector<uint32_t> global_num_value;
    std::vector<uint32_t> type_num_value[2];
    std::vector<uint64_t> global_value;
    std::vector<uint64_t> type_value[2];
    std::string toString() {
        return "totalGlobal: " + std::to_string(totalGlobal)
            + ", totalType[0]: " + std::to_string(totalType[0])
            + ", totalType[1]: " + std::to_string(totalType[1])
            + ", global_num_value length: " + std::to_string(global_num_value.size())
            + ", type_num_value[0] length: " + std::to_string(type_num_value[0].size())
            + ", type_num_value[1] length: " + std::to_string(type_num_value[1].size())
            + ", global_value length: " + std::to_string(global_value.size())
            + ", type_value[0] length: " + std::to_string(type_value[0].size())
            + ", type_value[1] length: " + std::to_string(type_value[1].size())
            + "\nglobal_num_value: " + std::accumulate(global_num_value.begin(), global_num_value.end(), std::string(), toStringVectorRawOp)
            + "\ntype_num_value[0]: " + std::accumulate(type_num_value[0].begin(), type_num_value[0].end(), std::string(), toStringVectorRawOp)
            + "\ntype_num_value[1]: " + std::accumulate(type_num_value[1].begin(), type_num_value[1].end(), std::string(), toStringVectorRawOp)
            + "\nglobal_value: " + std::accumulate(global_value.begin(), global_value.end(), std::string(), toStringVectorRawOp)
            + "\ntype_value[0]: " + std::accumulate(type_value[0].begin(), type_value[0].end(), std::string(), toStringVectorRawOp)
            + "\ntype_value[1]: " + std::accumulate(type_value[1].begin(), type_value[1].end(), std::string(), toStringVectorRawOp);
    }
};

struct SBINCtx {
    std::string puppet; // root bone name
    MeshShadowMap msm;
    OutFrag of;
    std::string toString() {
        return "puppet: " + puppet
            + "\nMeshShadowMap: " + msm.toString()
            + "\nof: " + of.toString();
    }
};

bool loadSBIN(FileObject& _fo, SBINCtx& _sbin) {

    if (_fo.index >= _fo.size)
        return false;
    
    if (!_fo.read((char*)&_sbin.msm.numMesh, sizeof(uint32_t)))
        return false;
    
    sLog << "Loading Mesh Vertex(" << _sbin.msm.numMesh << ")";
    
    if ((_fo.index + _sbin.msm.numMesh * sizeof(Vertex)) > _fo.size)
        return false;
    
    uint32_t i;

    // Mesh
    Vertex vtx;
    for (i = 0u; i < _sbin.msm.numMesh; i++) {
        if (!_fo.read((char*)&vtx, sizeof(Vertex)))
            return false;
        _sbin.msm.vertex.push_back(vtx);
    }

    sLog << " Loadded(" << _sbin.msm.vertex.size() << ")" << std::endl;

    if (!_fo.read((char*)&_sbin.msm.numMesh3d, sizeof(uint32_t)))
        return false;
    if (!_fo.read((char*)&_sbin.msm.numMesh2d, sizeof(uint32_t)))
        return false;
    
    sLog << "Loading Mesh3d(" << _sbin.msm.numMesh3d << ")";

    Vector3d v3;
    for (i = 0u; i < _sbin.msm.numMesh3d; i++) {
        if (!_fo.read((char*)&v3, sizeof(Vector3d)))
            return false;
        _sbin.msm.mesh3d.push_back(v3);
    }

    sLog << " Loadded(" << _sbin.msm.mesh3d.size() << ")" << std::endl;

    sLog << "Loading Mesh2d(" << _sbin.msm.numMesh2d << ")";

    Vector2d v2;
    for (i = 0u; i < _sbin.msm.numMesh2d; i++) {
        if (!_fo.read((char*)&v2, sizeof(Vector2d)))
            return false;
        _sbin.msm.mesh2d.push_back(v2);
    }

    sLog << " Loadded(" << _sbin.msm.mesh2d.size() << ")" << std::endl;

    TextureDDS texture;

    if (!_fo.read((char*)&_sbin.msm.sizeTexture_global, sizeof(uint32_t)))
        return false;
    
    sLog << "Loading Texture Global(" << _sbin.msm.sizeTexture_global << ")";
    
    if (_sbin.msm.sizeTexture_global > 0u) {
        if (!_fo.read((char*)&_sbin.msm.numTexture_global, sizeof(uint32_t)))
            return false;

        sLog << " (" << _sbin.msm.numTexture_global << ")";

        for (i = 0u; i < _sbin.msm.numTexture_global; i++) {

            texture.clear();

            if (!_fo.read((char *)&texture.width, sizeof(uint32_t)))
                return false;
            if (!_fo.read((char *)&texture.height, sizeof(uint32_t)))
                return false;
            texture.dataDDS.resize((texture.width * texture.height) / 2);
            if (!_fo.read((char *)texture.dataDDS.data(), texture.dataDDS.size()))
                return false;
            if (!_fo.read((char *)&texture.numMapDDS, sizeof(uint32_t)))
                return false;
            if (texture.numMapDDS > 0u) {
                texture.mapDDS.resize(texture.numMapDDS);
                if (!_fo.read((char *)texture.mapDDS.data(), texture.numMapDDS * sizeof(MapDDSValue)))
                    return false;
            }
            _sbin.msm.texture_global.push_back(texture);
        }

        sLog << " Loadded(" << _sbin.msm.texture_global.size() << ")" << std::endl;
    }

    for (i = 0; i < 2u; i++) {

        if (!_fo.read((char *)&_sbin.msm.sizeTexture_type[i], sizeof(uint32_t)))
            return false;
        
        sLog << "Loading Texture Type[" << i << "](" << _sbin.msm.sizeTexture_type[i] << ")";

        if (_sbin.msm.sizeTexture_type[i] > 0u) {
            if (!_fo.read((char *)&_sbin.msm.numTexture_type[i], sizeof(uint32_t)))
                return false;
            
            sLog << " (" << _sbin.msm.numTexture_type[i] << ")";
            
            for (uint32_t j = 0u; j < _sbin.msm.numTexture_type[i]; j++) {

                texture.clear();

                if (!_fo.read((char *)&texture.width, sizeof(uint32_t)))
                    return false;
                if (!_fo.read((char *)&texture.height, sizeof(uint32_t)))
                    return false;
                texture.dataDDS.resize((texture.width * texture.height) / 2);
                if (!_fo.read((char *)texture.dataDDS.data(), texture.dataDDS.size()))
                    return false;
                if (!_fo.read((char *)&texture.numMapDDS, sizeof(uint32_t)))
                    return false;
                if (texture.numMapDDS > 0u) {
                    texture.mapDDS.resize(texture.numMapDDS);
                    if (!_fo.read((char *)texture.mapDDS.data(), texture.numMapDDS * sizeof(MapDDSValue)))
                        return false;
                }
                _sbin.msm.texture_type[i].push_back(texture);
            }

            sLog << " Loadded(" << _sbin.msm.texture_type[i].size() << ")" << std::endl;
        }
    }

    // Out Frag
    if (!_fo.read((char*)&_sbin.of.totalGlobal, sizeof(uint32_t)))
        return false;
    for (i = 0u; i < 2u; i++)
        if (!_fo.read((char*)&_sbin.of.totalType[i], sizeof(uint32_t)))
            return false;
    
    sLog << "Loading Out Frag, Global(" << _sbin.of.totalGlobal 
        << "), Type[0](" << _sbin.of.totalType[0] << "), Type[1](" << _sbin.of.totalType[1] << ")";

    if (_sbin.of.totalGlobal > 0u) {
        if ((_fo.index + _sbin.msm.numMesh * sizeof(uint32_t)) > _fo.size)
            return false;
        uint32_t numOFGlobal;
        for (i = 0u; i < _sbin.msm.numMesh; i++) {
            if (!_fo.read((char*)&numOFGlobal, sizeof(uint32_t)))
                return false;
            _sbin.of.global_num_value.push_back(numOFGlobal);
        }
    }

    sLog << " Loadded Global Num Values(" << _sbin.of.global_num_value.size() << ")";

    for (i = 0u; i < 2u; i++) {
        if (_sbin.of.totalType[i] > 0u) {
            if ((_fo.index + _sbin.msm.numMesh * sizeof(uint32_t)) > _fo.size)
                return false;
            uint32_t numOFType;
            for (uint32_t j = 0u; j < _sbin.msm.numMesh; j++) {
                if (!_fo.read((char*)&numOFType, sizeof(uint32_t)))
                    return false;
                _sbin.of.type_num_value[i].push_back(numOFType);
            }
        }

        sLog << " Loadded Type[" << i << "] Num Values(" << _sbin.of.type_num_value[i].size() << ")";
    }

    if (_sbin.of.totalGlobal > 0u) {
        if ((_fo.index + _sbin.of.totalGlobal * sizeof(uint64_t)) > _fo.size)
            return false;
        uint64_t valueOFGlobal;
        for (i = 0u; i < _sbin.of.totalGlobal; i++) {
            if (!_fo.read((char*)&valueOFGlobal, sizeof(uint64_t)))
                return false;
            _sbin.of.global_value.push_back(valueOFGlobal);
        }
    }

    sLog << " Loadded Global Values(" << _sbin.of.global_value.size() << ")";
    
    for (i = 0u; i < 2u; i++) {
        if (_sbin.of.totalType[i] > 0u) {
            if ((_fo.index + _sbin.of.totalType[i] * sizeof(uint64_t)) > _fo.size)
                return false;
            uint64_t valueOFType;
            for (uint32_t j = 0u; j < _sbin.of.totalType[i]; j++) {
                if (!_fo.read((char*)&valueOFType, sizeof(uint64_t)))
                    return false;
                _sbin.of.type_value[i].push_back(valueOFType);
            }
        }

        sLog << " Loadded Type[" << i << "] Values(" << _sbin.of.type_value[i].size() << ")";
    }

    sLog << "\nFinal Index: " << _fo.index << std::endl;

    return true;
};

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

    Version ver{VERSION_1_0._v.full_version};

    if (fo.index >= fo.size)
        return -3;
    
    if (!fo.read((char*)&ver, sizeof(Version)))
        return -4;
    
    std::vector<SBINCtx> sbins;
    SBINCtx sbin;
    
    if ((ver._v.full_version & 0xFFFE0000) == 0xFFFE0000 && compareVersions(ver, VERSION_1_0) >= 0) {
        uint32_t numPuppets;
        if (!fo.read((char*)&numPuppets, sizeof(uint32_t)))
            return -5;
        if (numPuppets <= 0)
            return -6;

        for (uint32_t i = 0u; i < numPuppets; i++) {
            if (!fo.readFixedString(sbin.puppet))
                return -7;
            loadSBIN(fo, sbin);
            sbins.push_back(sbin);
        }
    }else {
        fo.resetIndex();
        sbin.puppet = "Beta version";
        loadSBIN(fo, sbin);
        sbins.push_back(sbin);
    }

    for (auto& el : sbins)
        sLog << "sbin:\n" << el.toString() << std::endl;
	
    return 0;
}