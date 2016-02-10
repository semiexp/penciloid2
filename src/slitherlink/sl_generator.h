#pragma once

#include <random>
#include "sl_clue_placement.h"
#include "sl_generator_option.h"
#include "sl_problem.h"

namespace penciloid
{
namespace slitherlink
{
typedef int Symmetry;

bool GenerateByLocalSearch(const CluePlacement &placement, const GeneratorOption &constraint, std::mt19937 *rnd, Problem *ret);

// Generates a clue placement. (only the positions of clues are specified; actual values of them aren't)
// <symmetry> should be the bitwise or of desired <kSymmetry...>'s.
// The constraint of <number_of_clues> doesn't necessarily be met, in the following cases:
// 1. The value is too large.
// 2. The constraint can't be met because of the symmetry condition.
CluePlacement GenerateCluePlacement(Y height, X width, int number_clues, Symmetry symmetry, std::mt19937 *rnd);

// The field remains same after rotation by 180 degrees.
const Symmetry kSymmetryDyad = 1;

// The field remains same after rotating by 90 degrees.
// This symmetry is ignored when the field is not a square.
const Symmetry kSymmetryTetrad = 2 | kSymmetryDyad;

// The field remains same after reflecting over the horizontal line.
const Symmetry kSymmetryHorizontalLine = 4;

// The field remains same after reflecting over the vertical line.
const Symmetry kSymmetryVerticalLine = 8;

}
}