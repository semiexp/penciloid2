#pragma once

namespace penciloid
{
struct GridLoopMethod
{
	GridLoopMethod() : 
		avoid_three_lines(true), avoid_cycle(true), hourglass_rule1(true)
	{}

	void DisableAll()
	{
		avoid_three_lines = false;
		avoid_cycle = false;
		hourglass_rule1 = false;
	}

	bool avoid_three_lines, avoid_cycle;
	bool hourglass_rule1;
};
}
