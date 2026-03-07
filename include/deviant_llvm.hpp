#ifndef __DEVIANT_LLVM__
#define __DEVIANT_LLVM__

#include <map>
#include <string>

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include "parser.hpp"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "ast.hpp"

namespace deviant {

class CodeGenBlock {
public:
    CodeGenBlock(llvm::BasicBlock* bb) {
        bblock_ = bb;
    }
    ~CodeGenBlock() {
    }
    void setCodeBlock(llvm::BasicBlock* bb) {
        bblock_ = bb;
    }
    llvm::BasicBlock* currentBlock() {
        return bblock_;
    }
    std::map<std::string, llvm::AllocaInst*>& getValueNames() {
        return locals_;
    }
    std::map<std::string, std::string>& getTypeMap() {
        return types_;
    }

private:
    llvm::BasicBlock*                        bblock_{nullptr};
    std::map<std::string, llvm::AllocaInst*> locals_;
    std::map<std::string, std::string>       types_;
};

class DeviantLLVM {
public:
    DeviantLLVM();

    void execute(const std::string& program) {
        // parse the program
        parser_ = std::make_unique<Parser>(program);
        auto ast = parser_->parse();

        // compile to LLVM IR
        compile(*ast);

#ifdef _DEBUG // print generated codex
        module_->print(llvm::outs(), nullptr);
#endif

        // save module IR to file
        saveModuleToFile("./out.ll");
    }

    llvm::LLVMContext& getGlobalContext() {
        return *context_.get();
    }

    llvm::Type* getGenericIntegerType() {
        return llvm::Type::getInt32Ty(getGlobalContext());
    }

    llvm::Module* getModule() {
        return module_.get();
    }

    llvm::IRBuilder<>* getBuilder() {
        return builder_.get();
    }

    void newScope(llvm::BasicBlock* bb) {
        if (!bb) {
            bb = llvm::BasicBlock::Create(getGlobalContext(), "scope");
        }
        code_blocks_.push_front(new CodeGenBlock(bb));
    }

    void endScope() {
        CodeGenBlock* top = code_blocks_.front();
        code_blocks_.pop_front();
        delete top;
    }

    // set the LLVM block where to put the next instructions
    void setInsertPoint(llvm::BasicBlock* bblock) {
        setCurrentBlock(bblock);
    }

    llvm::AllocaInst* findVariable(const std::string& var_name) {
        // Only look in current scope, since outer scope isn't valid while in
        // function declaration.
        auto& names = locals();
        if (names.find(var_name) != names.end()) {
            return names[var_name];
        }
        // return nullptr;

        // Travers from inner to outer scope (block) to find the variable.
        for (auto& cb : code_blocks_) {
            auto& names_s = cb->getValueNames();
            if (names_s.find(var_name) != names_s.end()) {
                return names_s[var_name];
            }
        }

        return nullptr;
    }

    void conductVar(std::string var_name, llvm::AllocaInst* alloca) {
        code_blocks_.front()->getValueNames()[var_name] = alloca;
    }

    llvm::BasicBlock* currentBlock() {
        return code_blocks_.front()->currentBlock();
    }

    std::map<std::string, llvm::AllocaInst*>& locals() {
        return code_blocks_.front()->getValueNames();
    }

private:
    void setCurrentBlock(llvm::BasicBlock* block) {
        code_blocks_.front()->setCodeBlock(block);
    }

    void initModule();

    void saveModuleToFile(const std::string& filename);

    void compile(Program& ast) {
        // compile main body
        ast.generateCode(*this);
    }

    void setupExternFunctions() {
        // i8* to substitute for char*, void*, etc
        [[maybe_unused]] auto byte_ptr_Ty =
            builder_->getInt8Ty()->getPointerTo();

        // int print(const char* format, ...)
        module_->getOrInsertFunction(
            "printf",
            llvm::FunctionType::get(
                llvm::IntegerType::getInt32Ty(*context_),
                llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
                true /* this is var arg func type*/));
    }

    llvm::Function* createFunction(const std::string&  fn_name,
                                   llvm::FunctionType* fn_type) {
        // function prototype might already be defined
        auto fn = module_->getFunction(fn_name);

        // if not, allocate the function
        if (!fn) {
            fn = createFunctionPrototype(fn_name, fn_type);
        }

        createFunctionBlock(fn);
        return fn;
    }

    llvm::Function* createFunctionPrototype(const std::string&  fn_name,
                                            llvm::FunctionType* fn_type) {
        auto fn = llvm::Function::Create(
            fn_type, llvm::Function::ExternalLinkage, fn_name, *module_);

        verifyFunction(*fn);
        return fn;
    }

    // create function block
    void createFunctionBlock(llvm::Function* fn) {
        auto entry = createBB("entry", fn);
        builder_->SetInsertPoint(entry);
    }

    llvm::BasicBlock* createBB(const std::string& name,
                               llvm::Function*    fn = nullptr) {
        return llvm::BasicBlock::Create(*context_, name, fn);
    }

    std::unique_ptr<Parser> parser_;

    // currently complier function
    llvm::Function*                    fn_;
    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::Module>      module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    std::list<CodeGenBlock*>           code_blocks_;
};

} // namespace deviant

#endif // __DEVIANT_LLVM__