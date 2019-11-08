#include "mull/MutationsFinder.h"

#include "mull/Config/Configuration.h"
#include "mull/Parallelization/Parallelization.h"
#include "mull/Program/Program.h"
#include "mull/ReachableFunction.h"

using namespace mull;
using namespace llvm;

MutationsFinder::MutationsFinder(std::vector<std::unique_ptr<Mutator>> mutators,
                                 const Configuration &config,
                                 const ASTInformation &astInformation)
    : mutators(std::move(mutators)), config(config),
      astInformation(astInformation) {}

std::vector<MutationPoint *>
MutationsFinder::getMutationPoints(const Program &program,
                                   std::vector<FunctionUnderTest> &functions) {
  std::vector<SearchMutationPointsTask> tasks;
  tasks.reserve(config.parallelization.workers);
  for (int i = 0; i < config.parallelization.workers; i++) {
    tasks.emplace_back(program, mutators, astInformation);
  }

  TaskExecutor<SearchMutationPointsTask> finder(
      "Searching mutants across functions", functions, ownedPoints, tasks);
  finder.execute();

  std::vector<MutationPoint *> mutationPoints;
  for (auto &point : ownedPoints) {
    mutationPoints.push_back(point.get());
  }

  llvm::outs() << "MutationsFinder: found " << mutationPoints.size() << " mutations.\n";

  return mutationPoints;
}
