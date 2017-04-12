#pragma once

#include "kk_type.h"
#include "kk_problem.h"
#include "../common/grid.h"

namespace penciloid
{
namespace kakuro
{
// A class for Kakuro answer.
class Answer
{
public:
	Answer();

	// Initialize a problem with the empty board (each cell is initialized with kNoClueValue)
	Answer(Y height, X width);
	Answer(const Answer &other);
	Answer(Answer &&other);
	~Answer() {}

	Answer &operator=(const Answer &other);
	Answer &operator=(Answer &&other);

	inline Y height() const { return grid_.height(); }
	inline X width() const { return grid_.width(); }
	inline void SetValue(CellPosition cell, int clue) { grid_.at(cell) = clue; }
	inline int GetValue(CellPosition cell) const { return grid_.at(cell); }

	// Returns the problem one of whose answer is this <Answer>.
	Problem ExtractProblem() const;

private:
	Grid<int> grid_;
};
}
}
