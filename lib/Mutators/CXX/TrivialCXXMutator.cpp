#include "mull/Mutators/CXX/TrivialCXXMutator.h"

#include "mull/Debug/Debug.h"
#include "mull/MutationPoint.h"
#include "mull/ReachableFunction.h"

#include <irm/irm.h>

using namespace mull;
using namespace mull::cxx;

TrivialCXXMutator::TrivialCXXMutator(
    std::vector<std::unique_ptr<irm::IRMutation>> mutators, MutatorKind kind,
    std::string id, std::string description, std::string replacement,
    std::string diagnostics)
    : lowLevelMutators(std::move(mutators)), kind(kind), ID(std::move(id)),
      description(std::move(description)), replacement(std::move(replacement)),
      diagnostics(std::move(diagnostics)) {}

std::string TrivialCXXMutator::getUniqueIdentifier() { return ID; }

std::string TrivialCXXMutator::getUniqueIdentifier() const { return ID; }

std::string TrivialCXXMutator::getDescription() const { return description; }

MutatorKind TrivialCXXMutator::mutatorKind() { return kind; }

void TrivialCXXMutator::applyMutation(llvm::Function *function,
                                      const MutationPointAddress &address,
                                      irm::IRMutation *lowLevelMutation) {
  llvm::Instruction &instruction = address.findInstruction(function);
  lowLevelMutation->mutate(&instruction);
}

std::vector<MutationPoint *>
TrivialCXXMutator::getMutations(Bitcode *bitcode,
                                const FunctionUnderTest &function,
                                const ASTInformation &astInformation) {
  assert(bitcode);

  std::vector<MutationPoint *> mutations;

  MutatorKind thisMutatorKind = mutatorKind();

  for (llvm::Instruction *instruction : function.getSelectedInstructions()) {
    if (! astInformation.validMutation(*instruction, mutatorKind())) {
      continue;
    }

    for (auto &mutator : lowLevelMutators) {
      if (mutator->canMutate(instruction)) {
        auto point = new MutationPoint(this, mutator.get(), instruction,
                                       replacement, bitcode, diagnostics);
        mutations.push_back(point);
      }
    }
  }

  return mutations;
}
