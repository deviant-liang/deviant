#include "user_input.hpp"

#include <iostream>

namespace deviant {
namespace {
enum class Option { HELP, VERSION, INCORRECT };

void printErrorMesesage(const std::string err) {
    std::cout << "Deviant Error: " << err << "\n";
}

void printMessage(Option option) {
    switch (option) {
        case Option::HELP:
            printf("Usage:\n");
            printf("deviant filename -h -v -q path\n");
            printf("\t-h this help text.\n");
            printf("\t-v be more verbose.\n");
            printf("\t-q be quiet.\n");
            break;
        case Option::VERSION:
            printf("deviant version 1.0.0\n");
            break;
        case Option::INCORRECT:
            printErrorMesesage("Input parameter is not correct");
            printErrorMesesage(
                "Run 'deviant --help' for all supported options.");
            break;
        default:
            break;
    }
}

bool isValidDvtFile(const std::string& filename) {
    size_t      location = filename.find_first_of('.');
    std::string extension = filename.substr(location + 1, filename.size());
    return (extension == "dvt" || extension == "dv") ? true : false;
}

} // namespace

bool UserInput::handleUserInput(int argc, char* argv[]) {
    if (argc <= 1) {
        printMessage(Option::HELP);
        return false;
    }
    std::string arg(argv[1]);
    if (arg.size() > 1 && arg[0] == '-') {
        const std::string& opt = (arg[1] == '-') ? (arg.substr(2, arg.size()))
                                                 : (arg.substr(1, arg.size()));

        if (opt == "version") {
            printMessage(Option::VERSION);
        } else if (opt == "h" || opt == "help") {
            printMessage(Option::HELP);
        } else {
            printMessage(Option::INCORRECT);
        }
    } else if (isValidDvtFile(arg)) { // filename
        filename_ = arg;
        return true;
    } else {
        printMessage(Option::INCORRECT);
    }
    return false;
}
} // namespace deviant