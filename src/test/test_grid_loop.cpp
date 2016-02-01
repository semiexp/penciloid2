#include "test_grid_loop.h"
#include "test.h"

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
	GridLoopClosedLoop();
}
void GridLoopBasicAccessors()
{
	PlainGridLoop field(Y(3), X(3));

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
	PlainGridLoop field(Y(3), X(3));

	field.DecideEdge(Position(Y(2), X(1)), PlainGridLoop::EDGE_LINE);
	field.DecideEdge(Position(Y(4), X(3)), PlainGridLoop::EDGE_BLANK);

	assert(field.GetEdge(Position(Y(2), X(1))) == PlainGridLoop::EDGE_LINE);
	assert(field.GetEdge(Position(Y(4), X(3))) == PlainGridLoop::EDGE_BLANK);
}
void GridLoopInspectVertex()
{
	{
		// two lines from a vertex: another edges will be blank
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(Position(Y(2), X(1)), PlainGridLoop::EDGE_LINE);
		field.DecideEdge(Position(Y(3), X(2)), PlainGridLoop::EDGE_LINE);

		assert(field.GetEdge(Position(Y(1), X(2))) == PlainGridLoop::EDGE_BLANK);
		assert(field.GetEdge(Position(Y(2), X(3))) == PlainGridLoop::EDGE_BLANK);
	}
	{
		// one line and two blanks from a vertex: the another edge will be a line
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(Position(Y(3), X(2)), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(Y(2), X(1)), PlainGridLoop::EDGE_LINE);
		field.DecideEdge(Position(Y(1), X(2)), PlainGridLoop::EDGE_BLANK);

		assert(field.GetEdge(Position(Y(2), X(3))) == PlainGridLoop::EDGE_LINE);
	}
	{
		// 3 blanks from a vertex: the another edge will also be blank
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(Position(Y(3), X(2)), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(Y(2), X(1)), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(Y(1), X(2)), PlainGridLoop::EDGE_BLANK);

		assert(field.GetEdge(Position(Y(2), X(3))) == PlainGridLoop::EDGE_BLANK);
	}
	{
		// around corner
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(Position(Y(0), X(1)), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(Y(6), X(1)), PlainGridLoop::EDGE_LINE);

		assert(field.GetEdge(Position(Y(1), X(0))) == PlainGridLoop::EDGE_BLANK);
		assert(field.GetEdge(Position(Y(5), X(0))) == PlainGridLoop::EDGE_LINE);
	}
	{
		// small loop avoidance
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(Position(Y(3), X(6)), PlainGridLoop::EDGE_LINE);
		field.DecideEdge(Position(Y(0), X(3)), PlainGridLoop::EDGE_BLANK);
		field.DecideEdge(Position(Y(1), X(0)), PlainGridLoop::EDGE_LINE);

		assert(field.GetEdge(Position(Y(2), X(1))) == PlainGridLoop::EDGE_BLANK);
	}
}
void GridLoopCopy()
{
	PlainGridLoop field(Y(3), X(3));

	field.DecideEdge(Position(Y(2), X(3)), PlainGridLoop::EDGE_LINE);

	PlainGridLoop field2(field);

	assert(field2.GetEdge(Position(Y(2), X(3))) == PlainGridLoop::EDGE_LINE);
}
void GridLoopClosedLoop()
{
	PlainGridLoop field(Y(3), X(3));

	field.DecideEdge(Position(Y(0), X(1)), PlainGridLoop::EDGE_LINE);
	field.DecideEdge(Position(Y(1), X(0)), PlainGridLoop::EDGE_LINE);
	field.DecideEdge(Position(Y(2), X(1)), PlainGridLoop::EDGE_LINE);
	field.DecideEdge(Position(Y(1), X(2)), PlainGridLoop::EDGE_LINE);

	assert(field.IsFullySolved() == true);
	assert(field.GetEdge(Position(Y(0), X(5))) == PlainGridLoop::EDGE_BLANK);
}
}
}
