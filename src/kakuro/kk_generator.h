#pragma once

#include <random>

#include "../common/type.h"
#include "kk_problem.h"
#include "kk_dictionary.h"

namespace penciloid
{
namespace kakuro
{
bool GenerateByLocalSearch(Problem &frame, Dictionary *dic, std::mt19937 *rnd, Problem *ret);
}
}
