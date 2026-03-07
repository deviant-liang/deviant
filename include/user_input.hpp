#ifndef __USAGE__
#define __USAGE__

namespace deviant {

class UserInput {
public:
    // return true if the user entered a valid filename
    [[nodiscard]] bool handleUserInput(int argc, char* argv[]);

    const std::string& getFilename() {
        return filename_;
    }

private:
    std::string filename_;
};

}  // namespace deviant

#endif  // __USAGE__