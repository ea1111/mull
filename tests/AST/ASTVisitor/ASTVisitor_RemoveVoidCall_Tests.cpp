#include "FixturePaths.h"

#include "mull/AST/ASTVisitor.h"
#include "mull/JunkDetection/CXX/ASTStorage.h"

#include <clang/Tooling/Tooling.h>

#include <gtest/gtest.h>

using namespace mull;
using namespace llvm;

static const char *const fakeSourceFilePath = "input.cc";
static FilePathFilter nullPathFilter;
static mull::TraverseMask
    traverseMask({mull::MutatorKind::RemoveVoidFunctionMutator});

TEST(ASTVisitorTest, voidCall) {
  const char *const binaryOperator = R"(///
void voidFunction() {}

void foo() {
  voidFunction();
};
)";

  ASTStorage storage("", "");

  std::unique_ptr<clang::ASTUnit> astUnit(
      clang::tooling::buildASTFromCode(binaryOperator, fakeSourceFilePath));

  ASTMutations astMutations;

  ThreadSafeASTUnit threadSafeAstUnit(std::move(astUnit));
  ASTVisitor astVisitor(threadSafeAstUnit, astMutations, nullPathFilter,
                        traverseMask);

  astVisitor.traverse();

  clang::SourceManager &sourceManager = threadSafeAstUnit.getSourceManager();

  const ASTMutation &mutation = astMutations.getMutation(
      fakeSourceFilePath, MutatorKind::RemoveVoidFunctionMutator, 5, 3);

  clang::SourceLocation begin = mutation.stmt->getSourceRange().getBegin();

  int clangLine = sourceManager.getExpansionLineNumber(begin);
  int clangColumn = sourceManager.getExpansionColumnNumber(begin);

  ASSERT_EQ(astMutations.count(), 1U);
  ASSERT_TRUE(astMutations.locationExists(
      fakeSourceFilePath, MutatorKind::RemoveVoidFunctionMutator, 5, 3));
  ASSERT_EQ(clangLine, 5U);
  ASSERT_EQ(clangColumn, 3U);
}
