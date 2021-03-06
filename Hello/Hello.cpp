//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Argument.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Analysis/TargetLibraryInfo.h"


#include <vector>
#include <map>
#include <queue>
#include <set>

using namespace llvm;

#define DEBUG_TYPE "hello"

STATISTIC(NumInstKilled, "Number of instructions killed");
STATISTIC(NumConstantsProp, "Number of constant propagated");
STATISTIC(NumOfArgsPop, "Number of arguments anaylzed");

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct Hello : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    Hello() : ModulePass(ID) {}
    
    private:
     std::map <llvm::Argument*,std::vector<llvm::Argument*>> consumerSet;
    public:
    

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesCFG();
      AU.addRequired<TargetLibraryInfoWrapperPass>();
    }


    bool runOnModule(Module &M) override {
      initConsumerSets(M); 
      // Print Consumer Sets
      for(auto &formalP : consumerSet) {
        //errs() << "The consumers of " << *(formalP.first) << "\n";
        for(auto &supp : formalP.second){
        //  errs() << *supp << '\n';
        }
      }
      ipConstantProp(M);


      return false;
    }

    void ipConstantProp(Module &M) {
      std::queue<llvm::Argument*> worklist;

      // Initialize worklist queue with every formal param of every function
      for(Module::iterator F=M.begin(), E=M.end(); F != E ; ++F){
        Function::arg_iterator formal_param = F->arg_begin();
        Function::arg_iterator FE = F->arg_end();

        for(;formal_param != FE; ++formal_param){
          worklist.push(formal_param);
        }
      }


      llvm::Argument *current_formal_param;
      bool isConstant = false;
      while(!worklist.empty()) {
        current_formal_param = worklist.front();
        worklist.pop();
        ++NumOfArgsPop;
        isConstant = isFormalParamConstant(current_formal_param);
        if (isConstant) {
          //errs() << "I am a constant formal param: " << *current_formal_param << '\n';
          ConstantPropagation(*(current_formal_param->getParent()));
          ++NumConstantsProp;
          for(auto &consumerParam : consumerSet[current_formal_param]) {
            worklist.push(consumerParam);
          }
        }
      }
    }
    
    /// PropagateConstantsIntoArguments - Look at all uses of the specified
    /// function.  If all uses are direct call sites, and all pass a particular
    /// constant in for an argument, propagate that constant in as the argument.
    ///
    /// Check to see if formal_param is constant at every callsite
    bool isFormalParamConstant(llvm::Argument* formal_param) {
      llvm::Function *F = formal_param->getParent();
      llvm::Instruction *formalParamInst;
      int position = formal_param->getArgNo();
      //errs() << *F << '\n';
      //errs() << position << '\n';

      std::pair<Constant*, bool> argConst;
      argConst.second = true;
      for (Use &U : F->uses()) {
        User *UR = U.getUser();
        // Ignore blockaddress uses.
        if (isa<BlockAddress>(UR)) continue;
        
        // Used by a non-instruction, or not the callee of a function, do not
        // transform.
        if (!isa<CallInst>(UR) && !isa<InvokeInst>(UR))
          return false;
        
        CallSite CS(cast<Instruction>(UR));
        if (!CS.isCallee(&U))
          return false;

        // Check out all of the potentially constant arguments.  Note that we don't
        // inspect varargs here.
        CallSite::arg_iterator AI = CS.arg_begin();
        CallSite::arg_iterator AE = CS.arg_end();
        Function::arg_iterator Arg = F->arg_begin();

        for (int i = 0; AI != AE; i++, ++AI, ++Arg) {
          if(i == position) {
             // If this argument is known non-constant, ignore it.
            if (!argConst.second) {
              continue;
            }
            
            Constant *C = dyn_cast<Constant>(*AI);
            if (C && argConst.first == nullptr) {
              argConst.first = C;   // First constant seen.
              argConst.second = true;
            } else if (C && argConst.first == C) {
              // Still the constant value we think it is.
            } else if (*AI == &*Arg) {
              // Ignore recursive calls passing argument down.
            } else {
              // Argument is not constant
              argConst.second = false;
            }
          }
        }
      }


      // If we got to this point, we might have a constant!
      // Do we have a constant argument?
      if (!argConst.second || formal_param->use_empty() ||
          formal_param->hasInAllocaAttr() || (formal_param->hasByValAttr() && !F->onlyReadsMemory())) {
        return false;
      }

      // Yay! it's constant!
      Value *V = argConst.first;
      if (!V) V = UndefValue::get(formal_param->getType());
      formal_param->replaceAllUsesWith(V);
      
      return true;
    }

    bool ifInstructionSeen(llvm::Instruction * inst, std::vector<llvm::Instruction *> seen_list){
      for(int i=0; i < seen_list.size(); ++i){
        if(inst->isIdenticalTo(seen_list[i])){
          return true;
        }  
      }
      return false;
    }


    void getConsumers(llvm::Value * v, Function * F, llvm::Argument * formal_param,
                  std::vector<llvm::Instruction*> seen_inst){
      
      if(Instruction * test = dyn_cast<Instruction>(v)){
        if(ifInstructionSeen(test, seen_inst)){
         return;  
        }
        else{
          seen_inst.push_back(test);  
        }
      }         

      for( User * U : v->users()){
        if (Instruction *Inst = dyn_cast<Instruction>(U)) {
            //errs() <<"Lolz " << *Inst << "\n";

          // The instruction is a call site, so look to find the actual param
          // that the relation exists on and add it to the formal params consumer set
          if (Inst->getOpcode() == Instruction::Call) {
            CallSite CS(Inst);
            //errs() << "CallInst: " <<*Inst << "\n";
            Instruction * previous = dyn_cast<Instruction>(v);
            if (previous) {

            }
            else {
              previous = Inst;
            }

            // if (Instruction * previous = dyn_cast<Instruction>(v)) {
              //errs() <<"Previous: " << *previous << '\n';
                      
              CallSite::arg_iterator AI = CS.arg_begin();
              CallSite::arg_iterator AE = CS.arg_end();
              Function * defined_func = CS.getCalledFunction();
              Function::arg_iterator FI = defined_func->arg_begin();

              for(;AI != AE; ++AI, ++FI){
                llvm::Value *arg_val = dyn_cast<llvm::Value>(AI);

                if(llvm::Instruction * cs_arg = dyn_cast<llvm::Instruction>(*AI)){

                  if(cs_arg->isIdenticalTo(previous)){
                    //errs() <<"Doing the check for prev and callsite arg" << '\n';
                    consumerSet[formal_param].push_back(FI); 
                  }
                }
                else {
                  consumerSet[formal_param].push_back(FI); 
                }
              }
            // }
            //test2.c
            //return;
          }

          else if(Inst->getOpcode() == Instruction::Ret){
             //errs() << "Exiting\n";
            return;
          } 

          if(Inst->getOpcode() == Instruction::Store){
            Value * operand = dyn_cast<Value>(Inst->getOperand(1));
            getConsumers(operand,F,formal_param,seen_inst);    
          }

          else if(Inst->getOpcode() == Instruction::Load){
            Value * load = dyn_cast<Value>(Inst);
            
            getConsumers(load,F,formal_param,seen_inst);   
          }
          
          else{
            //Value * v1 = dyn_cast<Value>(U);
            Value * other = dyn_cast<Value>(Inst);
            getConsumers(other,F,formal_param,seen_inst);
            
          }
        }
        else{
          return;
        }
      }         
     }
    
    // Initialize every formal parameter of every function with an empty vector of consumers  
    void initConsumerSets(Module &M){
      for(Module::iterator F=M.begin(), E=M.end(); F != E ; ++F){
        Function::arg_iterator FI = F->arg_begin();
        Function::arg_iterator FE = F->arg_end();

        for(;FI != FE; ++FI){
          std::vector<llvm::Argument*> new_vector;
          consumerSet[FI]=new_vector;
        }
      }
      
      for(Module::iterator F=M.begin(), E=M.end(); F != E ; ++F){
        
        //errs() << "Name of Function: " << F->getName() << '\n' << '\n';
        
        bool has_callInst = false;
        for (inst_iterator I = inst_begin(*F), E = inst_end(*F); I != E; ++I){ 
          if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {
            has_callInst =true;
            //errs() << F->getName() << " has at least one call instruction" << '\n';
          }
        }

        if(has_callInst){ 
          Function::arg_iterator Foo_args_begin = F->arg_begin();
          Function::arg_iterator Foo_args_end = F->arg_end();
          for(; Foo_args_begin != Foo_args_end; ++Foo_args_begin){
            Value * v = dyn_cast<llvm::Value>(Foo_args_begin);
            //errs() << "Arg: " << *v<< '\n';
            std::vector<llvm::Instruction*> seen_list;
            getConsumers(v,&(*F),Foo_args_begin,seen_list);   
            seen_list.clear();    
          }
          /*
          // Print out function body if it contains a callsite
          for (inst_iterator I = inst_begin(*F), E = inst_end(*F); I != E; ++I){
            if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {
              errs() <<"Callsite: "<< *callInst << " " << callInst->getNumArgOperands() << '\n';
                          CallSite CS(callInst);
              Function * test = CS.getCalledFunction();

              errs() <<"Get Called Function: "<< *test << '\n';
            }
          }*/
        }
      }
    }
    

  bool ConstantPropagation(Function &F) {
    // Initialize the worklist to all of the instructions ready to process...
    std::set<Instruction*> WorkList;
    for(inst_iterator i = inst_begin(F), e = inst_end(F); i != e; ++i) {
       WorkList.insert(&*i);
    }
    bool Changed = false;
    const DataLayout &DL = F.getParent()->getDataLayout();
    TargetLibraryInfo *TLI =
      &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();

    while (!WorkList.empty()) {
      Instruction *I = *WorkList.begin();
      WorkList.erase(WorkList.begin());    // Get an element from the worklist...

      if (!I->use_empty())                 // Don't muck with dead instructions...
        if (Constant *C = ConstantFoldInstruction(I, DL, TLI)) {
         // Add all of the users of this instruction to the worklist, they might
         // be constant propagatable now...
         for (User *U : I->users())
           WorkList.insert(cast<Instruction>(U));

          // Replace all of the uses of a variable with uses of the constant.
          I->replaceAllUsesWith(C);

          // Remove the dead instruction.
          WorkList.erase(I);
          I->eraseFromParent();

          // We made a change to the function...
          Changed = true;
          ++NumInstKilled;
        }
    }
    return Changed;
  }
   
  };
}

char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass");

namespace {
  // Hello2 - The second implementation with getAnalysisUsage implemented.
  struct Hello2 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Hello2() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      errs() << "Hello: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char Hello2::ID = 0;
static RegisterPass<Hello2>
Y("hello2", "Hello World Pass (with getAnalysisUsage implemented)");
