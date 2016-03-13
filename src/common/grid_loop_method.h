#pragma once

namespace penciloid
{
struct GridLoopMethod
{
	GridLoopMethod() : 
		avoid_three_lines(true), avoid_cycle(true)
	{}

	bool avoid_three_lines, avoid_cycle;
};
}
