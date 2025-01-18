// Arquivo lzpak.cpp
// Criado em 12/04/2024 as 06:38 por Acrisio
// Arquivo principal da ferramenta LZPak

// Compilação g++:
//	g++ lzpak.cpp -o lzpak.exe
// Compilação MSVC
//	cl /EHsc /std:c++17 lzpak.cpp /Fe:lzpak.exe

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

// C libs in C++
#include <cstdint>
#include <cstdio>
#include <cstring>

// C++
#include <fstream>
#include <memory>
#include <functional>
#include <filesystem>
#include <list>
#include <regex>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#pragma pack(push, 1)
struct sLZPakHeader {
	uint32_t	offset_file_entry;
	uint32_t	num_file_entry;
	uint8_t		version; 
};

enum eLZPakFileEntryVersion : uint8_t {
	LZPFEV_1,
	LZPFEV_2,
	LZPFEV_3,
	LZPFEV_RAW = 0xFu,
};

enum eLZPakFileEntryType : uint8_t {
	LZPFET_RAW,
	LZPFET_LZ77,
	LZPFET_DIRECTORY,
	LZPFET_LZ772,
};

struct sLZPakFileEntryHeader {
    sLZPakFileEntryHeader() { memset(this, 0, sizeof(sLZPakFileEntryHeader)); }
    sLZPakFileEntryHeader(uint8_t _name_length, uint8_t _type, uint8_t _version)
        : name_length(_name_length), type(_type), version(_version) {}
    sLZPakFileEntryHeader(sLZPakFileEntryHeader& _header) { *this = _header; }
	uint8_t	name_length;
	uint8_t type    : 4;
	uint8_t version : 4;
};

struct sLZPakFileEntry : public sLZPakFileEntryHeader {
	sLZPakFileEntry() : sLZPakFileEntryHeader() { memset(&this->offset, 0, sizeof(sLZPakFileEntry) - sizeof(sLZPakFileEntryHeader)); }
	sLZPakFileEntry(sLZPakFileEntryHeader& _header) : sLZPakFileEntryHeader(_header) {
	    memset(&this->offset, 0, sizeof(sLZPakFileEntry) - sizeof(sLZPakFileEntryHeader));
	}
	void* operator new(size_t _size_type, size_t _size) {
		
		void *ptr = ::operator new(_size_type + _size);

		return ptr;
	}
	void operator delete(void* _ptr) {
		::operator delete(_ptr);
	}
	static size_t calcule_size(sLZPakFileEntryHeader& _file_entry_header) {
		return sizeof(sLZPakFileEntry) + _file_entry_header.name_length + (size_t)((
			_file_entry_header.version < LZPFEV_3 || _file_entry_header.version == LZPFEV_RAW
		) & 1u);
	}
	const std::string getName() const {
		auto s = std::string(name, name + name_length);
		// replace to separator that work in Win/Unix
		std::replace(s.begin(), s.end(), '\\', '/');
		return s;
	}
	uint32_t	offset;
	uint32_t	compress_size;
	uint32_t	size;
	char		name[0];
};
#pragma pack(pop)

struct sLZPakHeaderEx : public sLZPakHeader {
    std::string author;
};

constexpr uint8_t	kLZPakVersion	= 0x12u;
constexpr uint32_t	kLZPakXorKey	= 0x71u;

constexpr uint32_t  kKeys_GB[4] =   { 0x03F607A9u, 0x036F5A3Eu, 0x011002B4u, 0x04AB00EAu };
constexpr uint32_t  kKeys_TH[4] =   { 0x050AD33Bu, 0x00BAFF09u, 0x0452FFDAu, 0x02CB4422u };
constexpr uint32_t  kKeys_JP[4] =   { 0x020A5FD4u, 0x01EEBDFFu, 0x02B3C6A0u, 0x04F6A3E1u };
constexpr uint32_t  kKeys_KR[4] =   { 0x0485B576u, 0x05148E02u, 0x05141D96u, 0x028FA9D6u };
constexpr uint32_t  kKeys_ID[4] =   { 0x01640DB7u, 0x01455A9Bu, 0x027F1AB7u, 0x05918B54u };
constexpr uint32_t  kKeys_EU[4] =   { 0x01E986D8u, 0x05818479u, 0x03D2B0BBu, 0x02C9B030u };

static const uint32_t*  gLocationKeys = nullptr;
static uint32_t         gLocationCustomKeys[4] = { 0u };

void DecryptXTEA(const uint32_t* _keys, uint64_t& _dest, const uint64_t _src) {

    uint32_t delta = 0xE3779B90u;
    uint32_t soma = 0x61C88647u;
    uint32_t *padrao = (uint32_t*)&_src;
    uint32_t *padrao2 = (uint32_t*)((uint8_t*)&_src + 4u);

    for (uint32_t i = 0u; i < 16u; i++) {

        *padrao2 -= (((*padrao << 4u) ^ (*padrao >> 5u)) + *padrao) ^ (delta + _keys[(delta >> 11u) & 3u]);
        delta += soma;
        *padrao -= (((*padrao2 << 4u) ^ (*padrao2 >> 5u)) + *padrao2) ^ (delta + _keys[delta & 3u]);
    }
    _dest = _src;
}

void EncryptXTEA(const uint32_t* _keys, uint64_t& _dest, const uint64_t _src) {

    uint32_t delta = 0u;
    uint32_t soma = 0x61C88647u;
    uint32_t *padrao = (uint32_t*)&_src;
    uint32_t *padrao2 = (uint32_t*)((uint8_t*)&_src + 4u);

    for (uint32_t i = 0u; i < 16u; i++) {

        *padrao += (((*padrao2 >> 5u) ^ (*padrao2 << 4u)) + *padrao2) ^ (delta  + _keys[delta & 3u]);
        delta -= soma;
        *padrao2 += (((*padrao >> 5u) ^ (*padrao << 4u)) + *padrao) ^ (delta + _keys[(delta >> 11u) & 3u]);
    }
    _dest = _src;
}

void LZ77_Decompress(std::vector< uint8_t >& _destination, uint8_t* _source, uint32_t _size, uint32_t _compress_size, std::function<void(size_t,size_t)> _func_log = nullptr) {
	
	_destination.clear();

	if (_source == nullptr || _size == 0u || _compress_size == 0u)
		return;

	_destination.resize(_size);

	uint8_t mask8bit = 0u;
	uint32_t s_index = 0u, d_index = 0u;
	uint16_t compress_head = 0u, offset_copy = 0u, size_copy = 0u;

	while (s_index < _compress_size && d_index < _size) {
		
		mask8bit = _source[s_index++];

		for (uint32_t bits = 0u; bits < 8u && d_index < _size && s_index < _compress_size; bits++) {
			
			if (mask8bit & 1u) {

				if ((s_index + 2u) > _compress_size) {

				    _destination.clear();

				    return;
				}

				compress_head = *(uint16_t*)&_source[s_index];	s_index += sizeof(uint16_t);
				
				offset_copy = compress_head & 0xFFFu;
				size_copy = 2u + (compress_head >> 0xCu);

				if (offset_copy > d_index || (d_index + size_copy) > _size) {

					_destination.clear();

					return;
				}
				
				memcpy(&_destination[d_index], &_destination[d_index - offset_copy], size_copy);	d_index += size_copy;

			}else
				_destination[d_index++] = _source[s_index++];
			
			mask8bit >>= 1u;

			// Log
			if (_func_log != nullptr)
			    _func_log(s_index, _compress_size);
		}
	}
}

void LZ77_Compress(std::vector< uint8_t >& _destination, uint8_t* _source, uint32_t _size, uint8_t _level = 5u, std::function<void(size_t,size_t)> _func_log = nullptr) {

    _destination.clear();

    if (_source == nullptr || _size == 0u)
        return;

    uint16_t max_dic_window = 0u;
    uint16_t max_match = 0xFu + 2u;

    switch (_level) {
        case 0:
            max_dic_window = 0x5u;
            break;
        case 1:
            max_dic_window = 0xFu;
            break;
        case 2:
            max_dic_window = 0x5Fu;
            break;
        case 3:
            max_dic_window = 0xFFu;
        case 4:
            max_dic_window = 0x5FFu;
            break;
        case 5:
        default:
            max_dic_window = 0xFFFu;
    }

    _destination.resize(_size + (_size / 8u) + 1u);

    uint32_t d_index = 0u, s_index = 0u;
    uint8_t* pMask8bit = nullptr;
    std::pair< uint16_t, uint8_t* > best_match;

    std::function<std::pair< uint16_t, uint8_t* >()> findBestMatch = [&]() -> std::pair< uint16_t, uint8_t* > {

        std::pair< uint16_t, uint8_t* > match((uint16_t)0u, nullptr);

        if (s_index <= 2u || (s_index + 3u) > _size)
            return match;

        uint32_t dic_window = s_index - (s_index > max_dic_window ? max_dic_window : s_index);
        uint32_t ts_index;
        uint16_t match_len;

        while (dic_window < (s_index - 3u)) {

            ts_index = s_index;

            while (dic_window < s_index && ts_index < _size && (ts_index - s_index) < max_match && _source[dic_window] == _source[ts_index]) {
                ++dic_window;
                ++ts_index;
            }

            match_len = (uint16_t)(ts_index - s_index);

            if (match_len > 2u && match.first <= match_len) {

                match.first = match_len;
                match.second = &_source[dic_window - match_len];

                // best match
                if (match.first == max_match || ts_index == _size || dic_window == s_index)
                    break;
            }

            // next byte
            dic_window = (dic_window - match_len) + 1u;
        }

        return match;
    };

    while (s_index < _size) {

        if (d_index >= _destination.size()) {

            _destination.clear();

            return;
        }

        pMask8bit = &_destination[d_index++];
        *pMask8bit = 0u;

        for (uint32_t bits = 0u; bits < 8u && s_index < _size; bits++) {

            if (d_index >= _destination.size()) {

                _destination.clear();

                return;
            }

            best_match = findBestMatch();

            if (best_match.second == nullptr)
                _destination[d_index++] = _source[s_index++];
            else {

                if ((d_index + 2u) > _destination.size() || (s_index + best_match.first) > _size) {

                    _destination.clear();

                    return;
                }

                *(uint16_t*)&_destination[d_index] = (uint16_t)(((best_match.first - 2u) << 0xCu) | (&_source[s_index] - best_match.second));

                d_index += 2u;
                s_index += best_match.first;

                *pMask8bit |= (1u << bits) & 0xFFu;
            }

			// Log
			if (_func_log != nullptr)
			    _func_log(s_index, _size);
        }
    }

    _destination.resize(d_index);
}

void LZ772_Decompress(std::vector< uint8_t >& _destination, uint8_t* _source, uint32_t _size, uint32_t _compress_size, std::function<void(size_t,size_t)> _func_log = nullptr) {
	
	constexpr uint16_t kObfuscationKeys[] { 0xFF21u, 0x834Fu, 0x675Fu, 0x34u, 0xF237u, 0x815Fu, 0x4765u, 0x233u }; 

	_destination.clear();

	if (_source == nullptr || _size == 0u || _compress_size == 0u)
		return;

	_destination.resize(_size);

	uint8_t mask8bit = 0u, tmpMask8bit = 0u;
	uint32_t s_index = 0u, d_index = 0u;
	uint16_t compress_head = 0u, offset_copy = 0u, size_copy = 0u;

	while (s_index < _compress_size && d_index < _size) {
		
		tmpMask8bit = mask8bit = _source[s_index++];

		tmpMask8bit ^= 0xC8u;

		for (uint32_t bits = 0u; bits < 8u && d_index < _size && s_index < _compress_size; bits++) {
			
			if (tmpMask8bit & 1u) {

				if ((s_index + 2u) > _compress_size) {

				    _destination.clear();

				    return;
				}

				compress_head = *(uint16_t*)&_source[s_index];	s_index += sizeof(uint16_t);
				
				compress_head ^= kObfuscationKeys[(mask8bit >> 3u) & 7u];

				offset_copy = compress_head & 0xFFFu;
				size_copy = 2u + (compress_head >> 0xCu);

				if (offset_copy > d_index || (d_index + size_copy) > _size) {

					_destination.clear();

					return;
				}
				
				memcpy(&_destination[d_index], &_destination[d_index - offset_copy], size_copy);	d_index += size_copy;

			}else
				_destination[d_index++] = _source[s_index++];
			
			tmpMask8bit >>= 1u;
			
			// Log
			if (_func_log != nullptr)
			    _func_log(s_index, _compress_size);
		}
	}
}

void LZ772_Compress(std::vector< uint8_t >& _destination, uint8_t* _source, uint32_t _size, uint8_t _level = 5u, std::function<void(size_t,size_t)> _func_log = nullptr) {

	constexpr uint16_t kObfuscationKeys[] { 0xFF21u, 0x834Fu, 0x675Fu, 0x34u, 0xF237u, 0x815Fu, 0x4765u, 0x233u }; 

    _destination.clear();

    if (_source == nullptr || _size == 0u)
        return;

    uint16_t max_dic_window = 0u;
    uint16_t max_match = 0xFu + 2u;

    switch (_level) {
        case 0:
            max_dic_window = 0x5u;
            break;
        case 1:
            max_dic_window = 0xFu;
            break;
        case 2:
            max_dic_window = 0x5Fu;
            break;
        case 3:
            max_dic_window = 0xFFu;
        case 4:
            max_dic_window = 0x5FFu;
            break;
        case 5:
        default:
            max_dic_window = 0xFFFu;
    }

    _destination.resize(_size + (_size / 8u) + 1u);

    uint32_t d_index = 0u, s_index = 0u;
    uint8_t* pMask8bit = nullptr;
    uint8_t* pMask8bitPtr[8] { nullptr };
    std::pair< uint16_t, uint8_t* > best_match;

    std::function<std::pair< uint16_t, uint8_t* >()> findBestMatch = [&]() -> std::pair< uint16_t, uint8_t* > {

        std::pair< uint16_t, uint8_t* > match((uint16_t)0u, nullptr);

        if (s_index <= 2u || (s_index + 3u) > _size)
            return match;

        uint32_t dic_window = s_index - (s_index > max_dic_window ? max_dic_window : s_index);
        uint32_t ts_index;
        uint16_t match_len;

        while (dic_window < (s_index - 3u)) {

            ts_index = s_index;

            while (dic_window < s_index && ts_index < _size && (ts_index - s_index) < max_match && _source[dic_window] == _source[ts_index]) {
                ++dic_window;
                ++ts_index;
            }

            match_len = (uint16_t)(ts_index - s_index);

            if (match_len > 2u && match.first <= match_len) {

                match.first = match_len;
                match.second = &_source[dic_window - match_len];

                // best match
                if (match.first == max_match || ts_index == _size || dic_window == s_index)
                    break;
            }

            // next byte
            dic_window = (dic_window - match_len) + 1u;
        }

        return match;
    };

    while (s_index < _size) {

        if (d_index >= _destination.size()) {

            _destination.clear();

            return;
        }

        pMask8bit = &_destination[d_index++];
        *pMask8bit = 0u;

        for (uint32_t bits = 0u; bits < 8u && s_index < _size; bits++) {

            if (d_index >= _destination.size()) {

                _destination.clear();

                return;
            }

            best_match = findBestMatch();

            if (best_match.second == nullptr)
                _destination[d_index++] = _source[s_index++];
            else {

                if ((d_index + 2u) > _destination.size() || (s_index + best_match.first) > _size) {

                    _destination.clear();

                    return;
                }

                *(uint16_t*)&_destination[d_index] = (uint16_t)(((best_match.first - 2u) << 0xCu) | (&_source[s_index] - best_match.second));

                pMask8bitPtr[bits] = &_destination[d_index];

                d_index += 2u;
                s_index += best_match.first;

                *pMask8bit |= (1u << bits) & 0xFFu;
            }
        }

        *pMask8bit ^= 0xC8u;

        for (uint32_t i = 0u; i < 8u; i++) {

            if (pMask8bitPtr[i] != nullptr)
                *(uint16_t*)pMask8bitPtr[i] ^= kObfuscationKeys[(*pMask8bit >> 3u) & 7u];

            pMask8bitPtr[i] = nullptr;
        }

		// Log
		if (_func_log != nullptr)
			_func_log(s_index, _size);

    }

    _destination.resize(d_index);
}

const uint32_t* findKeyLocation(sLZPakFileEntry& _header) {

    std::list< const uint32_t* > l_location_keys = { kKeys_GB, kKeys_TH, kKeys_JP, kKeys_KR, kKeys_ID, kKeys_EU };

    uint64_t src;

    for (auto& location_keys : l_location_keys) {
        
        src = ((uint64_t)_header.size << 32u) | _header.offset;

        DecryptXTEA(location_keys, src, src);

        if ((src & 0xFFFFFFFFu) == 0u)
            return location_keys;
    }

    // Not found
    return nullptr;
}

void make_file(char* _name, eLZPakFileEntryVersion _version, eLZPakFileEntryType _type) {

    sLZPakHeaderEx header;

    header.version = kLZPakVersion;
    header.author = "SuperSS";

    std::list< std::pair< bool, std::string> > l_directory;
    std::string base_dir;

    std::error_code ec;
    std::filesystem::file_status st = std::filesystem::status(_name, ec);

    if (ec.value() != 0) {

        printf("Falha ao verificar o status do caminho: \"%s\", error_value: %d, %s.\n", _name, ec.value(), ec.message().c_str());

        return;

    }else if (!std::filesystem::exists(st)) {

        printf("Esse caminho não existe: \"%s\"\n", _name);

        return;

    }else if (std::filesystem::is_directory(st)) {

        std::string directory_name = _name;

        auto dpos = directory_name.find_last_of("/\\");

        if (dpos != std::string::npos && (dpos + 1u) == directory_name.length())
            directory_name.pop_back();

        dpos = directory_name.find_last_of("/\\");

        if (dpos != std::string::npos && dpos != 0u)
            base_dir = directory_name.substr(0u, dpos + 1u);
        else
            base_dir = directory_name;

	if (std::filesystem::path(directory_name).parent_path().empty())
		base_dir = "";

        // Primeiro
        l_directory.push_back(std::make_pair(true, directory_name));

        try {

            for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(directory_name))
                l_directory.push_back(std::make_pair(std::filesystem::is_directory(dir_entry.status()), dir_entry.path().string().c_str()));

        }catch (std::filesystem::filesystem_error const& fs_error) {

            printf("Falha ao ler o diretório: \"%s\", error_value: %d, %s\n", directory_name.c_str(), fs_error.code().value(), fs_error.code().message().c_str());

            return;
        }

        if (l_directory.empty()) {

            printf("Diretório vazio: \"%s\"\n", directory_name.c_str());

            return;
        }

    }else {

        l_directory.push_back(std::make_pair(false, _name));

        base_dir = _name;

        const auto dpos = base_dir.find_last_of("/\\");

        if (dpos != std::string::npos && dpos != 0u)
            base_dir = base_dir.substr(0u, dpos + 1u);
    }

    std::ifstream in;
    std::ofstream out;
    uint64_t length;

    const char* name = nullptr;
    size_t name_len = 0u;
    size_t name_len_by_version = 0u;
    std::vector< uint8_t > source, destination;
    std::list< std::unique_ptr< sLZPakFileEntry > > l_file_entry;

    // Número de arquivos e diretórios
    header.num_file_entry = (uint32_t)l_directory.size();

    std::string out_file_name = std::filesystem::current_path().string().c_str();

    const auto tpos = out_file_name.find_last_of("/\\");

    if (tpos != std::string::npos && (tpos + 1u) != out_file_name.length())
        out_file_name += "/";

    out_file_name += "newpak.pak";

    if (header.num_file_entry == 1u)
        printf("Criando LZPak: %s do arquivo: \"%s\"\n", out_file_name.c_str(), _name);
    else
        printf("Criando LZPak: %s do diretório: \"%s\", número de arquivos e diretórios: 0x%08X.\n",
                out_file_name.c_str(), _name, header.num_file_entry);

    out.open(out_file_name, std::ios_base::binary);

    if (!out.good() || !out.is_open()) {

        printf("Falha em abrir para escrever no arquivo: \"%s\"\n", out_file_name.c_str());

        return;
    }

    printf("Escolha a versão:\n"
            "1) Versão 1;\n"
            "2) Versão 2;\n"
            "3) Versão 3;\n"
            "4) Versão Raw.\n"
            "A versão padrão é a 3: ");

    uint32_t input_version = _version;

    scanf("%d", &input_version);

    switch (input_version) {
        case 1:
            _version = LZPFEV_1;
            break;
        case 2:
            _version = LZPFEV_2;
            break;
        case 3:
        default:
            _version = LZPFEV_3;
            break;
        case 4:
            _version = LZPFEV_RAW;
            break;
    }

    printf("Escolha o tipo de compressão:\n"
            "1) Raw;\n"
            "2) LZ77;\n"
            "3) LZ772.\n"
            "O tipo de compressão padrão é o 3: ");

    uint32_t input_type = _type;

    scanf("%d", &input_type);

    switch (input_type) {
        case 1:
            _type = LZPFET_RAW;
            break;
        case 2:
            _type = LZPFET_LZ77;
            break;
        case 3:
        default:
            _type = LZPFET_LZ772;
    }

    uint32_t input_compress_level = 5u;

    if (_type == LZPFET_LZ77 || _type == LZPFET_LZ772) {

        printf("Escolha o nível de compressão, valor de 0 a 5: ");

        scanf("%d", &input_compress_level);

        if (input_compress_level > 5u)
            input_compress_level = 5u;
    }

    // Padrão Japonês
    gLocationKeys = kKeys_JP;

    if (_version == LZPFEV_3) {

        int32_t location_file = -1;

        do {

            printf("Especifique a localidade do arquivo.\n"
                    "0) Global;\n"
                    "1) Tailandês;\n"
                    "2) Japonês;\n"
                    "3) Coreano;\n"
                    "4) Indonesiano;\n"
                    "5) Europeu;\n"
                    "6) Digitar uma chave customizada.\n");

        } while (scanf("%d", &location_file) != 1 || location_file < 0 || location_file > 6);

        switch (location_file) {
            case 0:
                gLocationKeys = kKeys_GB;
                break;
            case 1:
                gLocationKeys = kKeys_TH;
                break;
            case 2:
                gLocationKeys = kKeys_JP;
                break;
            case 3:
                gLocationKeys = kKeys_KR;
                break;
            case 4:
                gLocationKeys = kKeys_ID;
                break;
            case 5:
                gLocationKeys = kKeys_EU;
                break;
            case 6: {

				std::string line;

                do {

                    printf("Exemplo de chave customizada: 4ffff,3ddd,4444,2222.\n");

                    do {

						std::getline(std::cin, line);

                        if (std::cin.fail()) {

                            printf("Falha ao ler uma linha do buffer de entrada do console.\n");

                            return;
                        }

                    } while ((line.length() < 9 && line[0] == '\n'));

                } while (sscanf(line.c_str(), "%08x,%08x,%08x,%08x", &gLocationCustomKeys[0], &gLocationCustomKeys[1], &gLocationCustomKeys[2], &gLocationCustomKeys[3]) != 4);

                gLocationKeys = gLocationCustomKeys;
                break;
            }
        }
    }

    for (auto& directory : l_directory) {

        if (directory.first) {

            name = (directory.second.length() == base_dir.length() ? directory.second.c_str() : &directory.second[base_dir.length()]);
            name_len = strlen(name);
            name_len_by_version = name_len;

            if (_version == LZPFEV_3)
                name_len_by_version = ((name_len / 8u) + ((name_len % 8u != 0u) & 1u)) * 8u;
            else if (_version < LZPFEV_3 || _version == LZPFEV_RAW)
                name_len_by_version = name_len + 1u;

            std::unique_ptr< sLZPakFileEntry > pFileEntry(
                    new(name_len_by_version) sLZPakFileEntry
                    );

            pFileEntry->name_length = (uint8_t)(_version == LZPFEV_3 ? name_len_by_version : name_len);
            pFileEntry->version = _version;
            pFileEntry->type = LZPFET_DIRECTORY;
            pFileEntry->offset = 0u;
            pFileEntry->size = 0u;
            pFileEntry->compress_size = 0u;

            memset(&pFileEntry->name, 0, name_len_by_version);

            if (_version < LZPFEV_3 || _version == LZPFEV_RAW) {

                pFileEntry->size ^= kLZPakXorKey;

                for (uint32_t i = 0u; i < pFileEntry->name_length; i++)
                    pFileEntry->name[i] = name[i] ^ kLZPakXorKey;

            }else if (_version == LZPFEV_3) {

                uint64_t src = ((uint64_t)pFileEntry->size << 32u) | pFileEntry->offset;

                EncryptXTEA(gLocationKeys, src, src);

                pFileEntry->size = (src >> 32u) & 0xFFFFFFFFu;
                pFileEntry->offset = src & 0xFFFFFFFFu;

                uint64_t *src2 = nullptr;

                memcpy(pFileEntry->name, name, name_len);

                for (uint32_t i = 0u; i < pFileEntry->name_length; i += 8u) {

                    src2 = (uint64_t*)&pFileEntry->name[i];
                    EncryptXTEA(gLocationKeys, *src2, *src2);
                }
            }

            l_file_entry.push_back(std::move(pFileEntry));

            continue;

        }

        // Arquivo
        in.open(directory.second.c_str(), std::ios_base::binary);

        if (!in.good() || !in.is_open()) {

            printf("Falha em abrir o arquivo para ler: \"%s\"\n", directory.second.c_str());

            return;
        }

        in.seekg(0u, std::ios_base::end);

        length = in.tellg();

        in.seekg(0u, std::ios_base::beg);

        name = (directory.second.length() == base_dir.length() ? directory.second.c_str() : &directory.second[base_dir.length()]);
        name_len = strlen(name);
        name_len_by_version = name_len;

        if (_version == LZPFEV_3)
            name_len_by_version = ((name_len / 8u) + ((name_len % 8u != 0u) & 1u)) * 8u;
        else if (_version < LZPFEV_3 || _version == LZPFEV_RAW)
            name_len_by_version = name_len + 1u;

        std::unique_ptr< sLZPakFileEntry > pFileEntry(
                new(name_len_by_version) sLZPakFileEntry
                );

        pFileEntry->name_length = (uint8_t)(_version == LZPFEV_3 ? name_len_by_version : name_len);
        pFileEntry->version = _version;
        pFileEntry->type = _type;
        pFileEntry->offset = (uint32_t)out.tellp();
        pFileEntry->size = (uint32_t)length;
        pFileEntry->compress_size = 0u;

        memset(&pFileEntry->name, 0, name_len_by_version);

        const auto epos = directory.second.find_last_of('.');

        // .wav, .mp3 é Raw
        if (epos != std::string::npos && (strcmp(&directory.second[epos], ".wav") == 0
                    || strcmp(&directory.second[epos], ".mp3") == 0))
            pFileEntry->type = LZPFET_RAW;

        source.resize((uint32_t)length);

        in.read((char*)source.data(), source.size());

        if (!in.good()) {

            printf("Falha ao ler o arquivo: \"%s\", size: 0x%08X.\n", directory.second.c_str(), (uint32_t)source.size());

            return;
        }

        // fecha
        in.close();

        if (_type == LZPFET_RAW)
            destination = std::move(source);
        else if (_type == LZPFET_LZ77 || _type == LZPFET_LZ772) {

            std::function<void(size_t, size_t)> log_compress = [&directory](size_t _processed, size_t _total) -> void {

                static time_t last_time = 0u;

                time_t current_time = time(nullptr);

                if (last_time == 0u || last_time != current_time) {

                    last_time = current_time;

                    printf("Progresso da compressão do arquivo \"%s\": %d%%.\n", directory.second.c_str(), (int32_t)((_processed * 100u) / _total));
                }
            };

            if (_type == LZPFET_LZ77)
                LZ77_Compress(destination, source.data(), (uint32_t)source.size(), input_compress_level, log_compress);
            else if (_type == LZPFET_LZ772)
                LZ772_Compress(destination, source.data(), (uint32_t)source.size(), input_compress_level, log_compress);

            if (destination.empty()) {

                printf("Falha ao comprimir o arquivo: \"%s\"\n", directory.second.c_str());

                return;
            }

            printf("Comprimiu o arquivo: \"%s\", size: 0x%08X, compress_size: 0x%08X, Taxa de compressão: %d%%.\n",
                    directory.second.c_str(), (uint32_t)source.size(), (uint32_t)destination.size(), (int32_t)(100u - (destination.size() * 100u) / source.size()));
        }

        pFileEntry->compress_size = (uint32_t)destination.size();

        if (_version < LZPFEV_3 || _version == LZPFEV_RAW) {

            pFileEntry->size ^= kLZPakXorKey;

            for (uint32_t i = 0u; i < pFileEntry->name_length; i++)
                pFileEntry->name[i] = name[i] ^ kLZPakXorKey;

        }else if (_version == LZPFEV_3) {

            uint64_t src = ((uint64_t)pFileEntry->size << 32u) | pFileEntry->offset;

            EncryptXTEA(gLocationKeys, src, src);

            pFileEntry->size = (src >> 32u) & 0xFFFFFFFFu;
            pFileEntry->offset = src & 0xFFFFFFFFu;

            uint64_t *src2 = nullptr;

            memcpy(pFileEntry->name, name, name_len);

            for (uint32_t i = 0u; i < pFileEntry->name_length; i += 8u) {

                src2 = (uint64_t*)&pFileEntry->name[i];
                EncryptXTEA(gLocationKeys, *src2, *src2);
            }
        }

        out.write((char*)destination.data(), destination.size());

        if (!out.good()) {

            printf("Falha ao escrever o arquivo no LZPak: \"%s\" > \"%s\", size: 0x%08X.\n",
                    directory.second.c_str(), out_file_name.c_str(), (uint32_t)destination.size());

            return;
        }

        l_file_entry.push_back(std::move(pFileEntry));
    }

    uint16_t author_len_be = (uint16_t)header.author.length();

    // big endian
    author_len_be = (author_len_be << 8u) | (author_len_be >> 8u);

    if (!header.author.empty()) {

        out.write(header.author.data(), header.author.length());

        if (!out.good()) {

            printf("Falha ao escrever o autor no LZPak: \"%s\" > \"%s\", length: 0x%08X,\n", header.author.c_str(), out_file_name.c_str(), (uint32_t)header.author.length());

            return;
        }
    }

    out.write((char*)&author_len_be, sizeof(uint16_t));

    if (!out.good()) {

        printf("Falha ao escrever o comprimento do autor em big endian no LZPak: 0x%08X > \"%s\"\n", author_len_be, out_file_name.c_str());

        return;
    }

    header.offset_file_entry = (uint32_t)out.tellp();

    for (auto& file_entry : l_file_entry) {

        out.write((char*)file_entry.get(), sLZPakFileEntry::calcule_size(*file_entry));

        if (!out.good()) {

            printf("Falha ao escrever o File Entry no LZPak: 0x%016llX > \"%s\", size: 0x%08X.\n",
                    (uint64_t)file_entry.get(), out_file_name.c_str(), (uint32_t)sLZPakFileEntry::calcule_size(*file_entry));

            return;
        }
    }

    out.write((char*)&header, sizeof(sLZPakHeader));

    if (!out.good()) {

        printf("Falha ao escrever o LZPak Header no LZPak: 0x%016llX > \"%s\", size: 0x%08X.\n",
                (uint64_t)&header, out_file_name.c_str(), (uint32_t)sizeof(sLZPakHeader));

        return;
    }

    printf("Criou arquivo LZPak: \"%s\", size: 0x%08X com sucesso!\n", out_file_name.c_str(), (uint32_t)out.tellp());

    out.close();
}

int32_t main(int32_t _argc, char* _argv[]) {
	
	if (_argc < 2)
		return 1;

    const auto lpos = std::string(_argv[1]).find_last_of('.');

    if (lpos == std::string::npos || strcmp(&_argv[1][lpos], ".pak") != 0
            || (_argc > 2 && strcmp(_argv[2], "-c") == 0)) {

        make_file(_argv[1], LZPFEV_3, LZPFET_LZ772);

        return 0;
    }

	std::ifstream in(_argv[1], std::ios_base::binary);

	if (!in.good() || !in.is_open()) {

		printf("Falha em abrir o arquivo: \"%s\".\n", _argv[1]);

		return 2;
	}

	in.seekg(0, std::ios_base::end);

	uint64_t len = in.tellg();

	if (len < sizeof(sLZPakHeader)) {
		
		printf("LZPak inválido, o arquivo tem o tamanho de 0x%016llX, mas o header do LZPak é de 0x%016llX.\n", len, (uint64_t)sizeof(sLZPakHeader));

		return 3;
	}

	in.seekg(-(int64_t)sizeof(sLZPakHeader), std::ios_base::end);

	printf("Abriu o LZPak:\n\tTamanho: 0x%016llX\n\tNome: %s\n", len, std::filesystem::path(_argv[1]).filename().string().c_str());

	sLZPakHeaderEx header;

	in.read((char*)&header, sizeof(sLZPakHeader));

	if (!in.good()) {
		
		printf("Falha ao ler o arquivo, não conseguiu ler o cabeçalho do LZPak.\n");

		return 4;
	}

	printf("LZPakHeader:\n\tVersion: 0x%02X\n\tOffset File Entry: 0x%08X\n\tNumber of File Entry: 0x%08X\n",
		header.version,
		header.offset_file_entry,
		header.num_file_entry);

	if (header.version != kLZPakVersion)
		printf("Aviso: a versão do LZPak é difrente de 0x%02X != 0x%02X\n",
			kLZPakVersion,
			header.version);

	std::list< std::unique_ptr< sLZPakFileEntry > > l_file_entry;
	sLZPakFileEntryHeader file_entry_header;
    std::unique_ptr< sLZPakFileEntry > pFileEntry(nullptr);

	in.seekg(header.offset_file_entry, std::ios_base::beg);

	if (!in.good()) {
		
		printf("Falha ao andar no arquivo para o offset file entry: 0x%08X.\n", header.offset_file_entry);

		return 5;
	}

	for (uint32_t i = 0u; i < header.num_file_entry; i++) {
		
		in.read((char*)&file_entry_header, sizeof(sLZPakFileEntryHeader));
	
		if (!in.good()) {
			
			printf("Falha ao ler o arquivo, não conseguiu ler o cabeçalho do LZPakFileEntry.\n");
	
			return 6;
		}

		pFileEntry.reset(
		    new(file_entry_header.name_length + (size_t)(
		        (file_entry_header.version < LZPFEV_3 || file_entry_header.version == LZPFEV_RAW) & 1u
		    )) sLZPakFileEntry(file_entry_header)
		);

		in.read((char*)&pFileEntry->offset, 
			sLZPakFileEntry::calcule_size(file_entry_header) - sizeof(sLZPakFileEntryHeader)
		);

		if (!in.good()) {
			
			printf("Falha ao ler o arquivo, não conseguiu ler o File Entry no offset: 0x%08X\n", (uint32_t)in.tellg());

			return 7;
		}

		if (pFileEntry->version != LZPFEV_RAW && pFileEntry->version < LZPFEV_3) {

			pFileEntry->size ^= kLZPakXorKey;
			
			for (uint32_t i = 0u; i < pFileEntry->name_length; i++)
				pFileEntry->name[i] ^= (uint8_t)kLZPakXorKey;

		}else if (pFileEntry->version == LZPFEV_3) {
			
		    if (gLocationKeys == nullptr) {

		        printf("Procuando chave XTEA da localidade especifica.\n");

		        const uint32_t* keys = findKeyLocation(*pFileEntry);

		        if (keys == nullptr) {

		            printf("Não encontrou a chave XTEA da localidade especifica.\n");

                    int32_t location_file = -1;

                    do {

                        printf("Especifique a localidade do arquivo.\n"
                                "0) Global;\n"
                                "1) Tailandês;\n"
                                "2) Japonês;\n"
                                "3) Coreano;\n"
                                "4) Indonesiano;\n"
                                "5) Europeu;\n"
                                "6) Digitar uma chave customizada.\n");

                    } while (scanf("%d", &location_file) != 1 || location_file < 0 || location_file > 6);

                    switch (location_file) {
                        case 0:
                            keys = kKeys_GB;
                            break;
                        case 1:
                            keys = kKeys_TH;
                            break;
                        case 2:
                            keys = kKeys_JP;
                            break;
                        case 3:
                            keys = kKeys_KR;
                            break;
                        case 4:
                            keys = kKeys_ID;
                            break;
                        case 5:
                            keys = kKeys_EU;
                            break;
                        case 6: {

							std::string line;

                            do {

                                printf("Exemplo de chave customizada: 4ffff,3ddd,4444,2222.\n");

                                do {

									std::getline(std::cin, line);

                                    if (std::cin.fail()) {

                                        printf("Falha ao ler uma linha do buffer de entrada do console.\n");

                                        return 8;
                                    }

                                } while ((line.length() < 9 && line[0] == '\n'));

                            } while (sscanf(line.c_str(), "%08x,%08x,%08x,%08x", &gLocationCustomKeys[0], &gLocationCustomKeys[1], &gLocationCustomKeys[2], &gLocationCustomKeys[3]) != 4);

                            keys = gLocationCustomKeys;
                            break;
                        }
                    }
		        }

		        // verifica a chave
		        uint64_t src_tst = ((uint64_t)pFileEntry->size << 32u) | pFileEntry->offset;

		        DecryptXTEA(keys, src_tst, src_tst);

		        if ((src_tst & 0xFFFFFFFFu) != 0u) {

		            printf("Falha na chave XTEA, localidade inválida. Chave: { 0x%08X, 0x%08X, 0x%08X, 0x%08X }\n",
		                    keys[0], keys[1], keys[2], keys[3]);

		            return 9;
		        }

		        gLocationKeys = keys;

                printf("Usando chave XTEA: { 0x%08X, 0x%08X, 0x%08X, 0x%08X }.\n",
                        gLocationKeys[0], gLocationKeys[1], gLocationKeys[2], gLocationKeys[3]);
		    }

		    uint64_t src = ((uint64_t)pFileEntry->size << 32u) | pFileEntry->offset;

		    DecryptXTEA(gLocationKeys, src, src);

		    pFileEntry->size = (src >> 32u) & 0xFFFFFFFFu;
		    pFileEntry->offset = src & 0xFFFFFFFFu;

		    uint64_t *src2 = nullptr;

		    for (uint32_t i = 0u; i < pFileEntry->name_length; i += 8u) {

		        src2 = (uint64_t*)&pFileEntry->name[i];
		        DecryptXTEA(gLocationKeys, *src2, *src2);
		    }
		}

		printf("LZPakFileEntry:\n\tNome: %s\n\tOffset: 0x%08X\n\tVersão: 0x%X\n\tTipo: 0x%X\n\tTamanho: 0x%08X\n\tTamanho comprimido: 0x%08X\n",
			pFileEntry->getName().c_str(),
			pFileEntry->offset,
			pFileEntry->version,
			pFileEntry->type,
			pFileEntry->size,
			pFileEntry->compress_size);

		l_file_entry.push_back(std::move(pFileEntry));
	}

	if (l_file_entry.empty()) {

	    printf("Não conseguiu ler nenhum File Entry.\n");

	    return 10;
	}

    auto& last_file_entry = l_file_entry.back();

    int32_t rest_author = (int32_t)(header.offset_file_entry - (last_file_entry->offset + last_file_entry->compress_size));

    if (rest_author > 2) {

        in.seekg(header.offset_file_entry - 2u, std::ios_base::beg);

        if (!in.good()) {

            printf("Falha ao andar no arquivo para o offset autor comprimento nome: 0x%08X.\n", header.offset_file_entry - 2u);

            return 11;
        }

        int16_t author_len;

        in.read((char*)&author_len, sizeof(int16_t));

        if (!in.good()) {

            printf("Falha ao ler o arquivo no offset autor comprimento nome: 0x%08X.\n", header.offset_file_entry - 2u);

            return 12;
        }

        // big endian
        author_len = (author_len >> 8u) & 0xFFu | (author_len << 8u) & 0xFF00u;

        if (author_len == 0)
            header.author = "Desconhecido";
        else {

            if (author_len < 0 || author_len > (rest_author - 2)) {

                printf("Falha no comprimento do nome do autor, maior que os dados disponíveis, 0x%08X > 0x%08X.\n", author_len, rest_author - 2);

                return 13;
            }

            header.author.resize((size_t)author_len);

            in.seekg(header.offset_file_entry - 2u - author_len);

            if (!in.good()) {

                printf("Falha ao andar no arquivo para o offset autor nome: 0x%08X.\n", header.offset_file_entry - 2u - author_len);

                return 14;
            }

            in.read(header.author.data(), header.author.length());

            if (!in.good()) {

                printf("Falha ao ler o arquivo no offset authpr nome: 0x%08X.\n", header.offset_file_entry - 2u - author_len);

                return 15;
            }
        }

    }else
        header.author = "Desconhecido";

    printf("Autor do LZPak é: %s.\n", header.author.c_str());

	if (_argc > 2) {
		
        std::function<bool(std::string)> make_dir_if_not_exists = [&](std::string _dir) -> bool {

            if (_dir.empty())
                return false;

            std::error_code ec;
            std::filesystem::file_status st = std::filesystem::status(_dir, ec);

            if (ec.value() != 0 && st.type() != std::filesystem::file_type::not_found) {

                printf("Falha ao verifica pegar a informação do diretório: \"%s\", error_value: %d, %s.\n", _dir.c_str(), ec.value(), ec.message().c_str());

                return false;
            }

            if (st.type() != std::filesystem::file_type::not_found && st.type() != std::filesystem::file_type::directory) {

                printf("Falha ao criar o diretório: \"%s\", um componente do caminho não é um diretório.\n", _dir.c_str());

                return false;
            }

            auto path = std::filesystem::path(_dir).parent_path();

            if (!path.empty() && path != path.root_path() && !make_dir_if_not_exists(path.string().c_str()))
                return false;

            ec.clear();

            if (std::filesystem::create_directory(_dir, ec)) {

                printf("Criou o diretório: \"%s\".\n", _dir.c_str());
                
                return true;
            }

            if (ec.value() != 0) {

                printf("Falha ao criar o diretório: \"%s\", error_value: %d, %s.\n", _dir.c_str(), ec.value(), ec.message().c_str());

                return false;
            }

            // Diretório já existe
            return true;
		};

		std::ofstream out;
		std::vector< char > buf;
        std::vector< uint8_t > decompress;
        std::string out_path_name = "./";
        std::string out_file_name = "";

		if (_argc > 3) {

		    out_path_name = _argv[3];

            if (out_path_name.empty())
                out_path_name = "./";
            else if (out_path_name.back() != '/' && out_path_name.back() != '\\')
                out_path_name += "/";
		}

		printf("Procurando por arquivo(s): \"%s\"\n", _argv[2]);

        std::regex regex_replace_all_asterisk("\\*");
        std::regex regex_find(std::regex_replace(_argv[2], regex_replace_all_asterisk, ".*"));

		for (auto& file_entry : l_file_entry) {
			
		    // All
		    if (std::regex_search(file_entry->getName(), regex_find)) {

				printf("Arquivo encontrado: %s\n", file_entry->getName().c_str());
				
				if (file_entry->type != LZPFET_DIRECTORY) {

                    std::string dirname_out_file = file_entry->getName();

                    const auto pos = dirname_out_file.find_last_of("/\\");

					if (pos != std::string::npos && !dirname_out_file.substr(0u, pos).empty())
					    dirname_out_file = out_path_name + dirname_out_file.substr(0u, pos) + "/";
					else
					    dirname_out_file = out_path_name;

					if (!make_dir_if_not_exists(dirname_out_file)) {

					    printf("Não conseguiu criar o diretório para salvar o arquivo.\n\tDirectory: \"%s\"\n\tFile: \"%s\"\n",
					            dirname_out_file.c_str(),
					            std::filesystem::path(file_entry->getName()).filename().string().c_str());

					    return 16;
					}

					out_file_name = dirname_out_file + std::filesystem::path(file_entry->getName()).filename().string().c_str();

					out.open(out_file_name.c_str(), std::ios_base::binary);

					if (!out.good() || !out.is_open()) {
						
						printf("Não conseguiu abrir/criar para escrever o arquivo: \"%s\".\n", out_file_name.c_str());

						continue;
					}

					printf("Extraindo no offset: 0x%08X, o arquivo: %s\n", file_entry->offset, file_entry->getName().c_str());

					in.seekg(file_entry->offset, std::ios_base::beg);

					if (!in.good()) {
						
						printf("Não conseguiu andar para o offset: 0x%08X, para ler no arquivo.\n", file_entry->offset);

						return 17;
					}

					if (file_entry->type == LZPFET_RAW) {
						
						buf.resize(file_entry->size);
						in.read(buf.data(), buf.size());
						
						if (!in.good()) {
							
							printf("Não conseguiu ler o arquivo, no offset: 0x%08X, para extrair o arquivo: %s\n",
								file_entry->offset,
								file_entry->getName().c_str());
					
							return 18;
						}

						out.write(buf.data(), buf.size());
						
						if (!out.good()) {

							printf("Não conseguiu escrever no arquivo: \"%s\"\n", out_file_name.c_str());

							out.close();
							out.clear();
							
							continue;
						}

					} else if (file_entry->type == LZPFET_LZ77) {

						buf.resize(file_entry->compress_size);
						in.read(buf.data(), buf.size());

						if (!in.good()) {
							
							printf("Não conseguiu ler o arquivo, no offset: 0x%08X, para extrair o arquivo: %s\n",
								file_entry->offset,
								file_entry->getName().c_str());
					
							return 18;
						}

					    LZ77_Decompress(decompress, (uint8_t*)buf.data(), file_entry->size, (uint32_t)buf.size());
						
						if (decompress.empty()) {
						
							printf("Não conseguiu descomprimir o arquivo:\n\tNome: %s\n\tOffset: 0x%08X\n\tTamanho: 0x%08X\n\tTamanho comprimido: 0x%08X\n",
									file_entry->getName().c_str(),
									file_entry->offset,
									file_entry->size,
									file_entry->compress_size);

							return 19;
						}

						out.write((char*)decompress.data(), decompress.size());

						if (!out.good()) {

							printf("Não conseguiu escrever no arquivo: \"%s\"\n", out_file_name.c_str());

							out.close();
							out.clear();
							
							continue;
						}

					} else if (file_entry->type == LZPFET_LZ772) {

						buf.resize(file_entry->compress_size);
						in.read(buf.data(), buf.size());

						if (!in.good()) {
							
							printf("Não conseguiu ler o arquivo, no offset: 0x%08X, para extrair o arquivo: %s\n",
								file_entry->offset,
								file_entry->getName().c_str());
					
							return 18;
						}

						LZ772_Decompress(decompress, (uint8_t*)buf.data(), file_entry->size, (uint32_t)buf.size());

						if (decompress.empty()) {
						
							printf("Não conseguiu descomprimir o arquivo:\n\tNome: %s\n\tOffset: 0x%08X\n\tTamanho: 0x%08X\n\tTamanho comprimido: 0x%08X\n",
									file_entry->getName().c_str(),
									file_entry->offset,
									file_entry->size,
									file_entry->compress_size);

							return 19;
						}

						out.write((char*)decompress.data(), decompress.size());

						if (!out.good()) {

							printf("Não conseguiu escrever no arquivo: \"%s\"\n", out_file_name.c_str());

							out.close();
							out.clear();
							
							continue;
						}
					}
					
					if (out.is_open())
						out.close();

					printf("Extraiu o arquivo: %s para \"%s\" com sucesso!\n", file_entry->getName().c_str(), out_file_name.c_str());
				}
			}
		}
	}

	l_file_entry.clear();

	return 0;
}
