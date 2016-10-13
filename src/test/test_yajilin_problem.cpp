#include "test_yajilin_problem.h"
#include "test.h"

#include <cassert>
#include <cstdio>

#include "../yajilin/yj_problem.h"

namespace penciloid
{
namespace test
{
void RunAllYajilinProblemTest()
{
	YajilinProblemStringInitializer();
}
void YajilinProblemStringInitializer()
{
	using namespace yajilin;

	const char* prob_base[] = {
		"..........",
		"..^0>1....",
		"..v0......",
		"..........",
		"........<2",
		".........."
	};
	Problem prob(Y(6), X(5), prob_base);

	assert(prob.height() == Y(6));
	assert(prob.width() == X(5));
	assert(prob.GetClue(CellPosition(Y(1), X(1))).direction == kClueNorth);
	assert(prob.GetClue(CellPosition(Y(1), X(1))).clue_value == 0);
	assert(prob.GetClue(CellPosition(Y(1), X(2))).direction == kClueEast);
	assert(prob.GetClue(CellPosition(Y(1), X(2))).clue_value == 1);
	assert(prob.GetClue(CellPosition(Y(2), X(1))).direction == kClueSouth);
	assert(prob.GetClue(CellPosition(Y(2), X(1))).clue_value == 0);
	assert(prob.GetClue(CellPosition(Y(4), X(4))).direction == kClueWest);
	assert(prob.GetClue(CellPosition(Y(4), X(4))).clue_value == 2);
	assert(prob.GetClue(CellPosition(Y(4), X(3))).direction == kNoClue);
}
}
}
