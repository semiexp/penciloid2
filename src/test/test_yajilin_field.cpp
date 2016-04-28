#include "test_yajilin_field.h"
#include "test.h"

#include <cassert>

#include "../yajilin/yj_problem.h"
#include "../yajilin/yj_field.h"

namespace penciloid
{
namespace test
{
void RunAllYajilinFieldTest()
{
	YajilinFieldClueTest();
}
void YajilinFieldClueTest()
{
	using namespace yajilin;
	Problem problem(Y(6), X(6));
	problem.SetClue(CellPosition(Y(3), X(2)), Problem::kClueNorth, 2);
	problem.SetClue(CellPosition(Y(3), X(4)), Problem::kClueNorth, 1);
	problem.SetClue(CellPosition(Y(4), X(1)), Problem::kClueEast, 1);
	Field field(problem);
	assert(field.IsFullySolved() == true);
	assert(field.IsInconsistent() == false);
}
}
}