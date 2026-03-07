#ifndef __USAGE__
#define __USAGE__

#include <string>

namespace deviant {

class UserInput {
public:
    // return true if the user entered a valid filename
    [[nodiscard]] auto handleUserInput(int argc, char* argv[]) -> bool;

    auto getFilename() const -> const std::string& {
        return filename_;
    }

private:
    std::string filename_;
};

} // namespace deviant

#endif // __USAGE__