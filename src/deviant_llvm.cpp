#include "deviant_llvm.hpp"

namespace deviant {

DeviantLLVM::DeviantLLVM() {
    initModule();
    setupExternFunctions();
}

auto DeviantLLVM::initModule() -> void {
    context_ = std::make_unique<llvm::LLVMContext>();
    module_ = std::make_unique<llvm::Module>("deviant", *context_);

    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
}

auto DeviantLLVM::saveModuleToFile(const std::string& filename) -> void {
    std::error_code      err_code;
    llvm::raw_fd_ostream out(filename, err_code);
    module_->print(out, nullptr);
}

} // namespace deviant