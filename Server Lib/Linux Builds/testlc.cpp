#include <iostream>
#include <locale>
#include <codecvt>
#include <string>
#include <cstdint>

#include "../Projeto IOCP/UTIL/hex_util.h"

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

int main() {

	wchar_t ha[] = L"Ola";

	std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> conv16;

	std::u16string str16 = conv16.from_bytes(reinterpret_cast<char*>(ha));

	stdA::hex_util::ShowHex((unsigned char*)ha, sizeof(ha));
	stdA::hex_util::ShowHex((unsigned char*)str16.data(), str16.size() * 2);

	return 0;
}
