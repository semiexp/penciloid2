#include "test_akari_problem.h"
#include "test.h"

#include <cassert>
#include <cstdio>

#include "../akari/ak_problem.h"

namespace penciloid
{
namespace test
{
void RunAllAkariProblemTest()
{
	AkariProblemBasicAccessors();
	AkariProblemStringInput();
}
void AkariProblemBasicAccessors()
{
	using namespace akari;

	Problem prob(Y(4), X(3));

	assert(prob.height() == Y(4));
	assert(prob.width() == X(3));

	assert(prob.GetClue(CellPosition(Y(0), X(0))) == kEmpty);
	prob.SetClue(CellPosition(Y(0), X(0)), Clue(2));
	assert(prob.GetClue(CellPosition(Y(0), X(0))) == Clue(2));
	assert(prob.GetClue(CellPosition(Y(1), X(0))) == kEmpty);
	assert(prob.GetClue(CellPosition(Y(0), X(1))) == kEmpty);
}
void AkariProblemStringInput()
{
	using namespace akari;

	const char* prob_base[] = {
		"0...",
		"#.2.",
		"...."
	};
	Problem prob(Y(3), X(4), prob_base);

	assert(prob.height() == Y(3));
	assert(prob.width() == X(4));

	assert(prob.GetClue(CellPosition(Y(0), X(0))) == Clue(0));
	assert(prob.GetClue(CellPosition(Y(0), X(1))) == kEmpty);
	assert(prob.GetClue(CellPosition(Y(1), X(0))) == kBlock);
	assert(prob.GetClue(CellPosition(Y(1), X(2))) == Clue(2));
}
}
}
