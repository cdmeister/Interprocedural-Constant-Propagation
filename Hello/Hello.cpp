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
#include <vector>
#include <map>


using namespace llvm;

#define DEBUG_TYPE "hello"

STATISTIC(HelloCounter, "Counts number of functions greeted");

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct Hello : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    Hello() : ModulePass(ID) {}
    
    private:
     std::map <llvm::Argument*,std::vector<llvm::Argument*>> supportSet;
    public:
    bool runOnModule(Module &M) override {
      ++HelloCounter;
      for(Module::iterator F=M.begin(), E=M.end(); F != E ; ++F){
        
        errs() << "Name of Function: " << F->getName() << '\n' << '\n';
        initSupportSets(&(*F)); 
        bool has_callInst = false;
        for (inst_iterator I = inst_begin(*F), E = inst_end(*F); I != E; ++I){ 
          if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {
            has_callInst =true;
            errs() << F->getName() << " has at least one call instruction" << '\n';
          }
        }
        if(has_callInst){ 
          Function::arg_iterator Foo_args_begin = F->arg_begin();
          Function::arg_iterator Foo_args_end = F->arg_end();
          for(; Foo_args_begin != Foo_args_end; ++Foo_args_begin){
            Value * v = dyn_cast<llvm::Value>(Foo_args_begin);
            errs() << "Arg: " << *v<< '\n';
            std::vector<llvm::Instruction*> seen_list;
            helper1(v,&(*F),Foo_args_begin,seen_list);   
            seen_list.clear();    
          }

          
          for (inst_iterator I = inst_begin(*F), E = inst_end(*F); I != E; ++I){
            /*errs() << "Inst: " << *I << '\n';
            for (Use &U : I->operands()) {
              Value *v = U.get();
              errs() << "Value: " << *v << '\n';
            }*/
            if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {
              //  We know know that this instruction is a callstie
              errs() <<"Callsite: "<< *callInst << " " << callInst->getNumArgOperands() << '\n';
                          CallSite CS(callInst);
              //  IMPORTANT!!!!!!!Get the definition of the called function in llvm::Function
              Function * test = CS.getCalledFunction();
              errs() <<"Get Called Function: "<< *test << '\n';
              //Loop through the arguments of the callsite parameters
              CallSite::arg_iterator AI = CS.arg_begin();
              CallSite::arg_iterator AE = CS.arg_end();

              Function::arg_iterator FI = F->arg_begin();
              Function::arg_iterator FE = F->arg_end();
              for(;AI != AE; ++AI){
                //errs() << *(callInst->getArgOperand(0)) << '\n';
                for(;FI != FE; ++FI){
                  Value * v_call = dyn_cast<llvm::Value>((AI));
                  Value * v_func = dyn_cast<llvm::Value>((FI));
                  //errs() << *FI << '\n';
                }
              }
            }
          }
        }
      }
      //Print Consumer Sets
     // for(auto &formalP : supportSet) {
     //   errs() << "The consumers of " << *(formalP.first) << "\n";
     //   for(auto &supp : formalP.second){
     //     errs() << *supp << '\n';
     //   }
     // }
      return false;
    }
    bool helper2(llvm::Instruction * inst, std::vector<llvm::Instruction *> seen_list){
      for(int i=0; i < seen_list.size(); ++i){
        if(inst->isIdenticalTo(seen_list[i])){
          return true;
        }  
      }
      return false;
    }


    void helper1(llvm::Value * v, Function * F, llvm::Argument * formal_param,
                  std::vector<llvm::Instruction*> seen_inst){
      if(Instruction * test = dyn_cast<Instruction>(v)){
        if(helper2(test,seen_inst)){
         return;  
        }
        else{
          seen_inst.push_back(test);  
        }
      }         
      for( User * U : v->users()){
        if (Instruction *Inst = dyn_cast<Instruction>(U)) {
            errs() <<"Lolz " << *Inst << "\n";


          // Base Case
          if (Inst->getOpcode() == Instruction::Call) {
            // Doing the check
            CallSite CS(Inst);
            errs() << "CallInst: " <<*Inst << "\n";
            Instruction * previous = dyn_cast<Instruction>(v);
            errs() <<"Previous: " << *previous << '\n';
                    
            CallSite::arg_iterator AI = CS.arg_begin();
            CallSite::arg_iterator AE = CS.arg_end();
            Function * defined_func = CS.getCalledFunction();
            Function::arg_iterator FI = defined_func->arg_begin();
            for(;AI != AE; ++AI, ++FI){
              if(Instruction * cs_arg = dyn_cast<Instruction>(*AI)){
                if(cs_arg->isIdenticalTo(previous)){
                  errs() <<"Doing the check for prev and callsite arg" << '\n';
                  supportSet[formal_param].push_back(FI); 
                }
              }
            }

  

            // Loop through the function formal parameters
            // This will be the key
      
            
            return;
          }
          else if(Inst->getOpcode() == Instruction::Ret){
             errs() << "Exiting\n";
            return;
          } 
          if(Inst->getOpcode() == Instruction::Store){
            Value * operand = dyn_cast<Value>(Inst->getOperand(1));
            //errs() <<"Store operand: "<< *operand << '\n';
            helper1(operand,F,formal_param,seen_inst);    
          }
          else if(Inst->getOpcode() == Instruction::Load){
             Value * load = dyn_cast<Value>(Inst);
            
            //errs() <<"Load operand: "<< *operand << '\n';
            helper1(load,F,formal_param,seen_inst);   
          }
          
          else{
            //Value * v1 = dyn_cast<Value>(U);
            Value * other = dyn_cast<Value>(Inst);
            helper1(other,F,formal_param,seen_inst);
            
          }
        }
        else{
          return;
          errs() << "Huh" << '\n';
        }
      }         
     }
      
    void initSupportSets(Function * F){
      Function::arg_iterator FI = F->arg_begin();
      Function::arg_iterator FE = F->arg_end();
      //  Loop through every formal parameter of the function and make an empty vector
      for(;FI != FE; ++FI){
        std::vector<llvm::Argument*> new_vector;
        supportSet[FI]=new_vector;
      }
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
      ++HelloCounter;
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
