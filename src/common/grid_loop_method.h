#pragma once

namespace penciloid
{
struct GridLoopMethod
{
	GridLoopMethod() : 
		avoid_three_lines(true), avoid_line_cycle(true), eliminate_closed_chain(true), hourglass_rule1(true)
	{}

	void DisableAll()
	{
		avoid_three_lines = false;
		avoid_line_cycle = false;
		eliminate_closed_chain = false;
		hourglass_rule1 = false;
	}

	bool avoid_three_lines;
	bool avoid_line_cycle, eliminate_closed_chain;
	bool hourglass_rule1;
};
}
