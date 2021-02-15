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

using namespace llvm;

namespace{
  struct Profiler : public ModulePass {
    static char ID;

    Function* hookLoad;
    Function* hookStore;

    Profiler() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) override {

      LLVMContext &C = M.getContext();
      Type* VoidTy = Type::getVoidTy(C);

      hookLoad = M.getFunction("_Z10instruLOADPvi");
      hookStore = M.getFunction("_Z11instruSTOREPvi");

      for(Module::iterator F = M.begin(), E = M.end(); F!= E; ++F)
      {
        // errs() << "module: " << F->getName().str() << "\n";
        for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
        {
          Profiler::runOnBasicBlock(BB);
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

      // First, check if the BB is from our instru. tools
      // Out instr. tools is compiled (to .bc) with hook_funcs_identify
      // for every BB in intru. tools, I planted a XOR 0x01234567, 0x89ABCDEF
      // to identify instru. blocks, avoiding to instrument our intru. tools.
      // I found this is XOR CONSTANT, CONSTANT is relatively safe
      // to use because usually these instructions will be folded.
      BasicBlock::iterator special_instr = BB->end();
      --special_instr;
      --special_instr;
      if (auto *op = dyn_cast<BinaryOperator>(&(*special_instr)))
      {
        if (op->getOpcode() == Instruction::Xor &&
            op->getNumOperands() >= 2)
        {
          Value* c_lhs = ConstantInt::get(Type::getInt32Ty(C), 0x01234567);
          Value* c_rhs = ConstantInt::get(Type::getInt32Ty(C), 0x89ABCDEF);

          Value* lhs = op->getOperand(0);
          Value* rhs = op->getOperand(1);

          if (c_lhs == lhs && c_rhs == rhs)
          {
            op->eraseFromParent();
            return true;
          }
        }
      }

      // Second, plant the hooks.
      errs() << "  Instru.: " << func_name << "\n" ;
      for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); 
        BI != BE; ++BI)
      {
        if (auto *op = dyn_cast<LoadInst>(&(*BI)))
        {
          // errs()  << "    L: "  << *BI <<  "\n";

          IRBuilder<> builder(op);
          builder.SetInsertPoint(&(*BB), ++builder.GetInsertPoint());
          LoadInst *CI = dyn_cast<LoadInst>(BI);
          Value* addr = op->getPointerOperand();
          Value *voidptr = builder.CreatePointerCast(addr, 
                                                     Type::getInt8PtrTy(C));
          Type* tp = CI->getType();
          int sizebits = (int) tp->getPrimitiveSizeInBits();
          CallInst *MI = builder.CreateCall(hookLoad, 
                                            {voidptr,
                                            builder.getInt32(sizebits)});
        }
        else if (auto *op = dyn_cast<StoreInst>(&(*BI)))
        {
          // errs()  << "    S: "  << *BI <<  "\n";

          IRBuilder<> builder(op);
          builder.SetInsertPoint(&(*BB), ++builder.GetInsertPoint());
          StoreInst *CI = dyn_cast<StoreInst>(BI);
          Value* addr = op->getPointerOperand();
          Value *voidptr = builder.CreatePointerCast(addr, 
                                                     Type::getInt8PtrTy(C));
          // Type* tp = CI->getType();
          Type* tp = CI->getValueOperand() -> getType();
          int sizebits = (int) tp->getPrimitiveSizeInBits();
          CallInst *MI = builder.CreateCall(hookStore, 
                                            {voidptr,
                                            builder.getInt32(sizebits)});

        }
      }

      return true;
    }
  };
}

char Profiler::ID = 0;
static RegisterPass<Profiler> X("profiler",
                                "Custom profiling tool",
                                false,
                                false);
