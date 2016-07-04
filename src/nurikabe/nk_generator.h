#pragma once

#include <random>

#include "../common/type.h"
#include "nk_problem.h"

namespace penciloid
{
namespace nurikabe
{
bool GenerateByLocalSearch(Y height, X width, std::mt19937 *rnd, Problem *ret);
}
}