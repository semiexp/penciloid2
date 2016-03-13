#pragma once

#include "../common/grid_loop_method.h"

namespace penciloid
{
namespace slitherlink
{
struct Method
{
	Method() :
		around_cell(true), adjacent_3(true), diagonal_3(true), diagonal_chain(true)
	{}

	GridLoopMethod grid_loop_method;
	bool around_cell, adjacent_3, diagonal_3, diagonal_chain;
};
}
}
