#pragma once

#include <random>
#include "sl_clue_placement.h"
#include "sl_generator_option.h"
#include "sl_problem.h"

namespace penciloid
{
namespace slitherlink
{
bool GenerateByLocalSearch(const CluePlacement &placement, const GeneratorOption &constraint, std::mt19937 *rnd, Problem *ret);
}
}