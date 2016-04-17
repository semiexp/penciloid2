#pragma once

#include <iostream>
#include <string>

#include "sl_problem.h"

namespace penciloid
{
namespace slitherlink
{
bool InputProblem(std::istream &str, Problem &problem);
std::string StringOfProblem(Problem &problem);
Problem ProblemOfString(std::string &str);
}
}
