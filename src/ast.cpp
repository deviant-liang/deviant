#include "ast.hpp"

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "deviant_llvm.hpp"

namespace deviant {
llvm::Value* Program::generateCode(DeviantLLVM& context) {
    llvm::Value* last = nullptr;
    for (size_t i = 0; i < statements_.size(); ++i) {
        auto stmt = statements_[i].get();
        last = stmt->generateCode(context);
    }
    return last;
}

llvm::Value* AstNode::generateCode(DeviantLLVM& context) {
    return llvm::ConstantInt::get(context.getGenericIntegerType(), 0, true);
}

llvm::Value* Integer::generateCode(DeviantLLVM& context) {
    return llvm::ConstantInt::get(context.getGenericIntegerType(), value_,
                                  true);
}

llvm::Value* Identifier::generateCode(DeviantLLVM& context) {
    // a usual stack variable
    llvm::AllocaInst* alloc = context.findVariable(name_);
    if (alloc != nullptr) {
        return new llvm::LoadInst(alloc->getAllocatedType(), alloc, name_,
                                  false, context.currentBlock());
    }
    return nullptr;
}

llvm::Value* VariableDeclaration::generateCode(DeviantLLVM& context) {
    llvm::Value* val = nullptr;

    const std::string& var_name(identifier_->getName());

    if (context.findVariable(var_name)) { // already declard!
        return nullptr;
    }

    // TODO: understand
    // context.locals()[identifier_->getName()] = nullptr;
    val = new llvm::AllocaInst(context.getGenericIntegerType(), 0,
                               identifier_->getName().c_str(),
                               context.currentBlock());

    // TODO: remove hardcode
    auto alloca = static_cast<llvm::AllocaInst*>(val);
    context.conductVar(var_name, alloca);

    return val;
}

llvm::Value* Assignment::generateCode(DeviantLLVM& context) {
    // TODO:
    llvm::AllocaInst* alloc = context.findVariable(var_name_);
    if (alloc) {
        llvm::Value* val = expr_->generateCode(context);
        return new llvm::StoreInst(val, alloc, false, context.currentBlock());
    } else { // not declare yet
        return nullptr;
    }
}

llvm::Value* Block::generateCode(DeviantLLVM& context) {
    llvm::Value* last = nullptr;
    for (size_t i = 0; i < statements_.size(); ++i) {
        auto stmt = statements_[i].get();
        last = stmt->generateCode(context);
    }
    return last;
}

llvm::Value* ReturnStatement::generateCode(DeviantLLVM& context) {
    if (ret_expr_) {
        llvm::Value* ret = ret_expr_->generateCode(context);
        if (ret == nullptr)
            return nullptr;
        return context.getBuilder()->CreateRet(ret);
    } else {
        return nullptr;
    }
}

llvm::Value* FunctionStatement::generateCode(DeviantLLVM& context) {
    auto fn = context.getModule()->getFunction(fn_name_);

    // TODO: remove hardcode
    auto fn_type = llvm::FunctionType::get(/*return type*/
                                           context.getBuilder()->getInt32Ty(),
                                           /*vararg*/ false);

    // if not, allocate the function
    if (!fn) {
        fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage,
                                    fn_name_, *context.getModule());
        verifyFunction(*fn);
    }

    // createFunctionBlock(fn);
    auto entry =
        llvm::BasicBlock::Create(context.getGlobalContext(), "entry", fn);
    context.getBuilder()->SetInsertPoint(entry);

    context.newScope(entry);

    body_->generateCode(context);

    context.endScope();

    return fn;
}

llvm::Value* FunctionCall::generateCode(DeviantLLVM& context) {
    // args
    std::vector<llvm::Value*> args;
    args.reserve(args_.size());
    for (size_t i = 0; i < args_.size(); ++i) {
        args.push_back(args_[i]->generateCode(context));
    }

    // printf
    if (fn_name_ == "print") {
        args.push_back(args[0]);
        llvm::Value* str = context.getBuilder()->CreateGlobalStringPtr("%d");
        args[0] = str;
        return context.getBuilder()->CreateCall(
            context.getModule()->getFunction("printf"), args, "printfCall");
    }

    return context.getBuilder()->CreateCall(
        context.getModule()->getFunction(fn_name_), args);
}

llvm::Value* IfStatement::generateCode(DeviantLLVM& context) {
#if 0
  llvm::Value* cmp_result = condition_->generateCode(context);
#endif
    // hardcode
    llvm::Value* cmp_result = llvm::ConstantInt::get(
        llvm::Type::getInt1Ty(context.getGlobalContext()), true);

    if (!cmp_result)
        return nullptr;

    llvm::Function*   fn = context.currentBlock()->getParent();
    llvm::BasicBlock* then_block =
        llvm::BasicBlock::Create(context.getGlobalContext(), "then", fn);
    llvm::BasicBlock* else_block =
        llvm::BasicBlock::Create(context.getGlobalContext(), "else");
    llvm::BasicBlock* merge_block =
        llvm::BasicBlock::Create(context.getGlobalContext(), "merge");
    llvm::BranchInst::Create(then_block, else_block, cmp_result,
                             context.currentBlock());

    bool need_merge_block = false;

    context.newScope(then_block);
    llvm::Value* then_val = then_->generateCode(context);
    if (then_val == nullptr)
        return nullptr;

    if (!context.currentBlock()->getTerminator()) {
        llvm::BranchInst::Create(merge_block, context.currentBlock());
        need_merge_block = true;
    }

    fn->insert(fn->end(), else_block);
    context.endScope();

    context.newScope(else_block);
    [[maybe_unused]] llvm::Value* else_val = nullptr;
    if (else_) {
        else_->generateCode(context);
    }

    if (!context.currentBlock()->getTerminator()) {
        llvm::BranchInst::Create(merge_block, context.currentBlock());
        need_merge_block = true;
    }
    context.endScope();
    if (need_merge_block) {
        context.newScope(merge_block);
        fn->insert(fn->end(), merge_block);
        context.getBuilder()->SetInsertPoint(merge_block);
    }

    return merge_block;
}

} // namespace deviant