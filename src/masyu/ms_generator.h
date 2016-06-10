#pragma once

#include <random>

#include "../common/type.h"
#include "ms_problem.h"

namespace penciloid
{
namespace masyu
{
bool GenerateByLocalSearch(Y height, X width, std::mt19937 *rnd, Problem *ret);
}
}