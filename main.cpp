#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#include "deviant_llvm.hpp"
#include "user_input.hpp"

auto read_file(const std::string& filename) -> std::string {
    std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::ate);

    std::ifstream::pos_type filesize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(filesize);
    ifs.read(bytes.data(), filesize);

    return std::string(bytes.data(), filesize);
}

auto main(int argc, char* argv[]) -> int {
    deviant::UserInput user_input;
    const bool         handle_file = user_input.handle_user_input(argc, argv);

    if (!handle_file) { return EXIT_FAILURE; }

    std::string          program(read_file(user_input.get_filename()));
    deviant::DeviantLLVM vm;
    vm.execute(program);

    return EXIT_SUCCESS;
}