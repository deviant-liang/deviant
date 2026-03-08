#ifndef __USAGE__
#define __USAGE__

#include <string>

namespace deviant {

class UserInput {
public:
    // return true if the user entered a valid filename
    [[nodiscard]] auto handle_user_input(int argc, char* argv[]) -> bool;

    auto get_filename() const -> const std::string& { return filename_; }

private:
    std::string filename_;
};

} // namespace deviant

#endif // __USAGE__