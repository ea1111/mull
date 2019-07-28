#pragma once

#include <string>
#include <vector>

namespace llvm {
class Function;
class Value;
class Module;
class Instruction;
} // namespace llvm

namespace mull {

class Context;
class Bitcode;
class MutationPoint;
class MutationPointAddress;
struct SourceLocation;

enum class MutatorKind {
  Unknown,
  ConditionalsBoundaryMutator,
  MathAddMutator,
  MathDivMutator,
  MathSubMutator,
  MathMulMutator,
  NegateMutator,
  RemoveVoidFunctionMutator,
  ReplaceCallMutator,
  AndOrReplacementMutator,
};

class Mutator {
public:
  virtual MutationPoint *getMutationPoint(Bitcode *bitcode,
                                          llvm::Function *function,
                                          llvm::Instruction *instruction,
                                          SourceLocation &sourceLocation,
                                          MutationPointAddress &address) = 0;

  virtual std::string getUniqueIdentifier() = 0;
  virtual std::string getUniqueIdentifier() const = 0;
  virtual MutatorKind mutatorKind() { return MutatorKind::Unknown; }

  virtual std::string getDescription() const = 0;

  virtual bool canBeApplied(llvm::Value &V) = 0;
  virtual llvm::Value *applyMutation(llvm::Function *function,
                                     MutationPointAddress &address) = 0;
  virtual ~Mutator() = default;
};

} // namespace mull
