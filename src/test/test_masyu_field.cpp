#include "test_masyu_field.h"
#include "test.h"

#include <cassert>

#include "../masyu/ms_field.h"

namespace penciloid
{
namespace test
{
void RunAllMasyuFieldTest()
{
	MasyuFieldAddClueTest();
}
void MasyuFieldAddClueTest()
{
	using namespace masyu;

	Field field(Y(5), X(5));

	assert(field.GetEdge(LoopPosition(Y(2), X(3))) == Field::kEdgeUndecided);
	assert(field.GetEdge(LoopPosition(Y(2), X(5))) == Field::kEdgeUndecided);
	assert(field.GetEdge(LoopPosition(Y(3), X(2))) == Field::kEdgeUndecided);
	assert(field.GetEdge(LoopPosition(Y(5), X(2))) == Field::kEdgeUndecided);
	field.AddClue(CellPosition(Y(1), X(1)), kClueBlack);
	assert(field.GetEdge(LoopPosition(Y(2), X(3))) == Field::kEdgeLine);
	assert(field.GetEdge(LoopPosition(Y(2), X(5))) == Field::kEdgeLine);
	assert(field.GetEdge(LoopPosition(Y(3), X(2))) == Field::kEdgeLine);
	assert(field.GetEdge(LoopPosition(Y(5), X(2))) == Field::kEdgeLine);

	assert(field.GetEdge(LoopPosition(Y(8), X(3))) == Field::kEdgeUndecided);
	assert(field.GetEdge(LoopPosition(Y(8), X(5))) == Field::kEdgeUndecided);
	field.AddClue(CellPosition(Y(4), X(2)), kClueWhite);
	assert(field.GetEdge(LoopPosition(Y(8), X(3))) == Field::kEdgeLine);
	assert(field.GetEdge(LoopPosition(Y(8), X(5))) == Field::kEdgeLine);
}
}
}