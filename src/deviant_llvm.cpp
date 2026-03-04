#include "deviant_llvm.hpp"

namespace deviant {
DeviantLLVM::DeviantLLVM() {
  initModule();
  setupExternFunctions();
}

void DeviantLLVM::initModule() {
  context_ = std::make_unique<llvm::LLVMContext>();
  module_ = std::make_unique<llvm::Module>("deviant", *context_);

  builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
}

void DeviantLLVM::saveModuleToFile(const std::string& filename) {
  std::error_code err_code;
  llvm::raw_fd_ostream out(filename, err_code);
  module_->print(out, nullptr);
}
}  // namespace deviant