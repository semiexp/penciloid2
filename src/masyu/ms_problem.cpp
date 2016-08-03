#include "ms_problem.h"

#include <utility>

namespace penciloid
{
namespace masyu
{
Problem::Problem() : cells_()
{
}
Problem::Problem(Y height, X width) : cells_(height, width, kNoClue)
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
Problem &Problem::operator=(const Problem &other) {
	cells_ = other.cells_;
	return *this;
}
Problem &Problem::operator=(Problem &&other) {
	cells_ = std::move(other.cells_);
	return *this;
}
}
}
