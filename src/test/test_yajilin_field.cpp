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
	YajilinCornerCellTest();
	YajilinFieldClueSameDirectionTest();
}
void YajilinFieldClueTest()
{
	using namespace yajilin;
	Problem problem(Y(6), X(6));
	problem.SetClue(CellPosition(Y(3), X(2)), kClueNorth, 2);
	problem.SetClue(CellPosition(Y(3), X(4)), kClueNorth, 1);
	problem.SetClue(CellPosition(Y(4), X(1)), kClueEast, 1);
	Field field(problem);
	assert(field.IsFullySolved() == true);
	assert(field.IsInconsistent() == false);
}
void YajilinCornerCellTest()
{
	using namespace yajilin;
	Problem problem(Y(10), X(10));
	problem.SetClue(CellPosition(Y(0), X(5)), kClueNorth, 0);
	Field field(problem);
	assert(field.GetCellState(CellPosition(Y(1), X(0))) == Field::kCellLine);
	assert(field.GetCellState(CellPosition(Y(0), X(1))) == Field::kCellLine);
	assert(field.GetCellState(CellPosition(Y(0), X(3))) == Field::kCellLine);
	assert(field.GetCellState(CellPosition(Y(1), X(4))) == Field::kCellLine);
	assert(field.GetCellState(CellPosition(Y(1), X(6))) == Field::kCellLine);
	assert(field.GetCellState(CellPosition(Y(0), X(7))) == Field::kCellLine);
}
void YajilinFieldClueSameDirectionTest()
{
	using namespace yajilin;
	Problem problem(Y(6), X(6));
	problem.SetClue(CellPosition(Y(0), X(2)), kClueSouth, 2);
	problem.SetClue(CellPosition(Y(2), X(2)), kClueSouth, 1);
	Field field(problem);
	assert(field.GetCellState(CellPosition(Y(1), X(2))) == Field::kCellBlock);
}

}
}