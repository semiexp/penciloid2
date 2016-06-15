#include "nk_field.h"

#include <xutility>

namespace penciloid
{
namespace nurikabe
{
Field::Field() : cells_(), inconsistent_(false), fully_solved_(false)
{
}
Field::Field(Y height, X width) : cells_(height, width, Cell()), inconsistent_(false), fully_solved_(false)
{
}
Field::Field(const Field &other) : cells_(other.cells_), inconsistent_(other.inconsistent_), fully_solved_(other.fully_solved_)
{
}
Field::Field(Field &&other) : cells_(std::move(other.cells_)), inconsistent_(other.inconsistent_), fully_solved_(other.fully_solved_)
{
}
Field::~Field()
{
}
Field &Field::operator=(const Field &other)
{
	cells_ = other.cells_;
	inconsistent_ = other.inconsistent_;
	fully_solved_ = other.fully_solved_;
}
int Field::GetRoot(int cell_idx)
{
	if (cells_.at(cell_idx).group_parent_cell < 0) return cell_idx;
	return cells_.at(cell_idx).group_parent_cell = GetRoot(cells_.at(cell_idx).group_parent_cell);
}
}
}