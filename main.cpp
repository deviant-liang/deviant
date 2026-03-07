#include <fstream>
#include <string>
#include <vector>

#include "deviant_llvm.hpp"
#include "user_input.hpp"

std::string readFile(const std::string& filename) {
    std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::ate);

    std::ifstream::pos_type filesize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(filesize);
    ifs.read(bytes.data(), filesize);

    return std::string(bytes.data(), filesize);
}

int main(int argc, char* argv[]) {
    deviant::UserInput user_input;
    bool handle_file = user_input.handleUserInput(argc, argv);

    if (!handle_file)
        return 1;

    std::string program(readFile(user_input.getFilename()));
    deviant::DeviantLLVM vm;
    vm.execute(program);

    return EXIT_SUCCESS;
}