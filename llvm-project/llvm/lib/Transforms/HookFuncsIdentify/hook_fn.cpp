// TODO, organize the include libraries
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/NoFolder.h"

using namespace llvm;

namespace{
  struct HookFuncsIdentify : public ModulePass {
    static char ID;

    HookFuncsIdentify() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) override {

      LLVMContext &C = M.getContext();
      Type* VoidTy = Type::getVoidTy(C);

      for(Module::iterator F = M.begin(), E = M.end(); F!= E; ++F)
      {
        for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
        {
          HookFuncsIdentify::runOnBasicBlock(BB);
        }
      }
      return false;
    }

    virtual bool runOnBasicBlock(Function::iterator &BB)
    {
      LLVMContext &C = BB->getContext();

      Function* bb_func = BB->getParent();
      StringRef f_name = bb_func->getName();
      std::string func_name = f_name.str();
      std::string bb_name = BB->getName().str();

      // If empty block, return back
      if (BB->begin() == BB->end()) { return true; }

      // We plant a xor instruction in the second last of every block
      BasicBlock::iterator BI = BB->end();
      --BI;

      auto *op = dyn_cast<Instruction>(&(*BI));
      Value* lhs = ConstantInt::get(Type::getInt32Ty(C), 0x01234567);
      Value* rhs = ConstantInt::get(Type::getInt32Ty(C), 0x89ABCDEF);
          
      IRBuilder<NoFolder> builder(op);
      builder.SetInsertPoint(&(*BI));
      auto *new_ins = builder.CreateXor(lhs, rhs);

      return true;
    }
  };
}

char HookFuncsIdentify::ID = 0;
static RegisterPass<HookFuncsIdentify> X("hook_funcs_identify",
                                "Identify hook functions",
                                false,
                                false);
