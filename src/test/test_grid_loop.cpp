#include "test_grid_loop.h"

#include <cassert>

#include "../common/grid_loop.h"

namespace penciloid
{
namespace test
{
void RunAllGridLoopTest()
{
	GridLoopBasicAccessors();
	GridLoopDecideEdgeBasic();
}
void GridLoopBasicAccessors()
{
	PlainGridLoop field(3, 3);

	assert(field.height() == 3);
	assert(field.width() == 3);
	assert(field.IsInconsistent() == false);
	assert(field.IsFullySolved() == false);
	assert(field.IsInAbnormalCondition() == false);

	field.SetInconsistent();
	assert(field.IsInconsistent() == true);
}
void GridLoopDecideEdgeBasic()
{
	PlainGridLoop field(3, 3);

	field.DecideEdge(Position(2, 1), PlainGridLoop::EDGE_LINE);
	field.DecideEdge(Position(4, 3), PlainGridLoop::EDGE_BLANK);

	assert(field.GetEdge(Position(2, 1)) == PlainGridLoop::EDGE_LINE);
	assert(field.GetEdge(Position(4, 3)) == PlainGridLoop::EDGE_BLANK);
}
}
}
