#include <iostream>
#include "../Projeto IOCP/UTIL/md5.hpp"

using namespace stdA;

int main() {

    std::string in;

    std::cout << "type raw string: ";
    std::cin >> in;

    md5::processData((unsigned char*)in.data(), in.length());

    std::cout << md5::getHash() << std::endl;

    return 0;
}