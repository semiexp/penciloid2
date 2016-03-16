#pragma once

namespace penciloid
{
struct GridLoopMethod
{
	GridLoopMethod() : 
		avoid_three_lines(true), avoid_cycle(true)
	{}

	void DisableAll()
	{
		avoid_three_lines = false;
		avoid_cycle = false;
	}

	bool avoid_three_lines, avoid_cycle;
};
}
