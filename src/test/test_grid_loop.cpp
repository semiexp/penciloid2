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
	GridLoopHourglassRule();
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

	field.DecideEdge(LoopPosition(Y(2), X(1)), PlainGridLoop::kEdgeLine);
	field.DecideEdge(LoopPosition(Y(4), X(3)), PlainGridLoop::kEdgeBlank);

	assert(field.GetEdge(LoopPosition(Y(2), X(1))) == PlainGridLoop::kEdgeLine);
	assert(field.GetEdge(LoopPosition(Y(4), X(3))) == PlainGridLoop::kEdgeBlank);
}
void GridLoopInspectVertex()
{
	{
		// two lines from a vertex: another edges will be blank
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(LoopPosition(Y(2), X(1)), PlainGridLoop::kEdgeLine);
		field.DecideEdge(LoopPosition(Y(3), X(2)), PlainGridLoop::kEdgeLine);

		assert(field.GetEdge(LoopPosition(Y(1), X(2))) == PlainGridLoop::kEdgeBlank);
		assert(field.GetEdge(LoopPosition(Y(2), X(3))) == PlainGridLoop::kEdgeBlank);
	}
	{
		// one line and two blanks from a vertex: the another edge will be a line
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(LoopPosition(Y(3), X(2)), PlainGridLoop::kEdgeBlank);
		field.DecideEdge(LoopPosition(Y(2), X(1)), PlainGridLoop::kEdgeLine);
		field.DecideEdge(LoopPosition(Y(1), X(2)), PlainGridLoop::kEdgeBlank);

		assert(field.GetEdge(LoopPosition(Y(2), X(3))) == PlainGridLoop::kEdgeLine);
	}
	{
		// 3 blanks from a vertex: the another edge will also be blank
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(LoopPosition(Y(3), X(2)), PlainGridLoop::kEdgeBlank);
		field.DecideEdge(LoopPosition(Y(2), X(1)), PlainGridLoop::kEdgeBlank);
		field.DecideEdge(LoopPosition(Y(1), X(2)), PlainGridLoop::kEdgeBlank);

		assert(field.GetEdge(LoopPosition(Y(2), X(3))) == PlainGridLoop::kEdgeBlank);
	}
	{
		// around corner
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(LoopPosition(Y(0), X(1)), PlainGridLoop::kEdgeBlank);
		field.DecideEdge(LoopPosition(Y(6), X(1)), PlainGridLoop::kEdgeLine);

		assert(field.GetEdge(LoopPosition(Y(1), X(0))) == PlainGridLoop::kEdgeBlank);
		assert(field.GetEdge(LoopPosition(Y(5), X(0))) == PlainGridLoop::kEdgeLine);
	}
	{
		// small loop avoidance
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(LoopPosition(Y(3), X(6)), PlainGridLoop::kEdgeLine);
		field.DecideEdge(LoopPosition(Y(0), X(3)), PlainGridLoop::kEdgeBlank);
		field.DecideEdge(LoopPosition(Y(1), X(0)), PlainGridLoop::kEdgeLine);

		assert(field.GetEdge(LoopPosition(Y(2), X(1))) == PlainGridLoop::kEdgeBlank);
	}
}
void GridLoopCopy()
{
	PlainGridLoop field(Y(3), X(3));

	field.DecideEdge(LoopPosition(Y(2), X(3)), PlainGridLoop::kEdgeLine);

	PlainGridLoop field2(field);

	assert(field2.GetEdge(LoopPosition(Y(2), X(3))) == PlainGridLoop::kEdgeLine);
}
void GridLoopClosedLoop()
{
	PlainGridLoop field(Y(3), X(3));

	field.DecideEdge(LoopPosition(Y(0), X(1)), PlainGridLoop::kEdgeLine);
	field.DecideEdge(LoopPosition(Y(1), X(0)), PlainGridLoop::kEdgeLine);
	field.DecideEdge(LoopPosition(Y(2), X(1)), PlainGridLoop::kEdgeLine);
	field.DecideEdge(LoopPosition(Y(1), X(2)), PlainGridLoop::kEdgeLine);

	assert(field.IsFullySolved() == true);
	assert(field.GetEdge(LoopPosition(Y(0), X(5))) == PlainGridLoop::kEdgeBlank);
}
void GridLoopHourglassRule()
{
	{
		PlainGridLoop field(Y(3), X(3));

		field.DecideEdge(LoopPosition(Y(0), X(1)), PlainGridLoop::kEdgeLine);
		field.DecideEdge(LoopPosition(Y(0), X(5)), PlainGridLoop::kEdgeBlank);
		field.DecideEdge(LoopPosition(Y(2), X(3)), PlainGridLoop::kEdgeLine);
		field.DecideEdge(LoopPosition(Y(3), X(2)), PlainGridLoop::kEdgeBlank);

		assert(field.GetEdge(LoopPosition(Y(0), X(3))) == PlainGridLoop::kEdgeUndecided);
	}

	for (bool use_hourglass_rule : {false, true}) {
		GridLoopMethod method;
		method.hourglass_rule1 = use_hourglass_rule;
		PlainGridLoop field(Y(3), X(3));
		field.SetMethod(method);

		field.DecideEdge(LoopPosition(Y(0), X(1)), PlainGridLoop::kEdgeLine);
		field.DecideEdge(LoopPosition(Y(0), X(5)), PlainGridLoop::kEdgeBlank);
		field.DecideEdge(LoopPosition(Y(6), X(5)), PlainGridLoop::kEdgeLine);
		field.DecideEdge(LoopPosition(Y(2), X(3)), PlainGridLoop::kEdgeLine);
		field.DecideEdge(LoopPosition(Y(3), X(2)), PlainGridLoop::kEdgeBlank);

		assert(field.IsFullySolved() == false);
		auto expected = use_hourglass_rule ? PlainGridLoop::kEdgeBlank : PlainGridLoop::kEdgeUndecided;
		assert(field.GetEdge(LoopPosition(Y(0), X(3))) == expected);
	}
}

}
}
