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
	GridLoopInspectVertex();
	GridLoopCopy();
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
void GridLoopInspectVertex()
{
	{
		// two lines from a vertex: another edges will be blank
		PlainGridLoop field(3, 3);

		field.DecideEdge(Position(2, 1), PlainGridLoop::EDGE_LINE);
		field.DecideEdge(Position(3, 2), PlainGridLoop::EDGE_LINE);

		assert(field.GetEdge(Position(1, 2)) == PlainGridLoop::EDGE_BLANK);
		assert(field.GetEdge(Position(2, 3)) == PlainGridLoop::EDGE_BLANK);
	}
	{
		// one line and two blanks from a vertex: the another edge will be a line
		PlainGridLoop field(3, 3);

		field.DecideEdge(Position(3, 2), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(2, 1), PlainGridLoop::EDGE_LINE);
		field.DecideEdge(Position(1, 2), PlainGridLoop::EDGE_BLANK);

		assert(field.GetEdge(Position(2, 3)) == PlainGridLoop::EDGE_LINE);
	}
	{
		// 3 blanks from a vertex: the another edge will also be blank
		PlainGridLoop field(3, 3);

		field.DecideEdge(Position(3, 2), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(2, 1), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(1, 2), PlainGridLoop::EDGE_BLANK);

		assert(field.GetEdge(Position(2, 3)) == PlainGridLoop::EDGE_BLANK);
	}
	{
		// around corner
		PlainGridLoop field(3, 3);

		field.DecideEdge(Position(0, 1), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(6, 1), PlainGridLoop::EDGE_LINE);

		assert(field.GetEdge(Position(1, 0)) == PlainGridLoop::EDGE_BLANK);
		assert(field.GetEdge(Position(5, 0)) == PlainGridLoop::EDGE_LINE);
	}
	{
		// small loop avoidance
		PlainGridLoop field(3, 3);

		field.DecideEdge(Position(3, 6), PlainGridLoop::EDGE_LINE);
		field.DecideEdge(Position(0, 3), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(1, 0), PlainGridLoop::EDGE_LINE);

		assert(field.GetEdge(Position(2, 1)) == PlainGridLoop::EDGE_BLANK);
	}
}
void GridLoopCopy()
{
	PlainGridLoop field(3, 3);

	field.DecideEdge(Position(2, 3), PlainGridLoop::EDGE_LINE);

	PlainGridLoop field2(field);

	assert(field2.GetEdge(Position(2, 3)) == PlainGridLoop::EDGE_LINE);
}
}
}
