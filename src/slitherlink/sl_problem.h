#pragma once

#include "../common/type.h"

namespace penciloid
{
namespace slitherlink
{
class Problem
{
public:
	typedef int Clue;

	static const Clue kNoClue = -1;

	Problem();

	// Initialize a problem with the empty board
	Problem(Y height, X width);

	// Place clues according to <clues>. Only '0', '1', '2' and '3' are considered as clues; other characters stand for empty cells.
	Problem(Y height, X width, const char* clues[]);

	Problem(const Problem &other);
	Problem(Problem &&other);
	Problem &operator=(const Problem &other);
	Problem &operator=(const Problem &&other);

	~Problem();

	inline Y height() const { return height_; }
	inline X width() const { return width_; }

	inline void SetClue(Position cell, Clue clue) { field_clue_[CellId(cell)] = clue; }
	inline Clue GetClue(Position cell) const { return field_clue_[CellId(cell)]; }

private:
	unsigned int CellId(Position pos) const { return int(pos.y) * int(width()) + int(pos.x); }

	Clue *field_clue_;
	Y height_;
	X width_;
};
}
}
