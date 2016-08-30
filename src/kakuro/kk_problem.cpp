#include "kk_problem.h"

#include <utility>

namespace penciloid
{
namespace kakuro
{
Problem::Problem() : grid_()
{
}
Problem::Problem(Y height, X width) : grid_(height, width, kEmptyCell)
{
}
Problem::Problem(const Problem &other) : grid_(other.grid_)
{
}
Problem::Problem(Problem &&other) : grid_(std::move(other.grid_))
{
}
Problem &Problem::operator=(const Problem &other)
{
	grid_ = other.grid_;
	return *this;
}
Problem &Problem::operator=(Problem &&other)
{
	grid_ = std::move(other.grid_);
	return *this;
}
}
}