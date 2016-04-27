#include "yj_problem.h"

#include <xutility>

namespace penciloid
{
namespace yajilin
{
Problem::Problem() : cells_()
{
}
Problem::Problem(Y height, X width) : cells_(height, width, Cell(kNoClue, 0))
{
}
Problem::Problem(const Problem &other) : cells_(other.cells_)
{
}
Problem::Problem(Problem &&other) : cells_(std::move(other.cells_))
{
}
Problem::~Problem()
{
}
}
}
