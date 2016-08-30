#pragma once

#include "kk_problem.h"
#include "kk_dictionary.h"

namespace penciloid
{
namespace kakuro
{
class Field
{
public:
	static const int kMaxCellValue = 9;
	static const int kCellUndecided = -1;
	static const int kCellClue = -2;

	Field();
	Field(const Problem &problem, Dictionary *dictionary = nullptr);
	Field(const Field &other);
	Field(Field &&other);
	~Field();

	Field &operator=(const Field &other);
	Field &operator=(Field &&other);

	Y height() const { return cells_.height(); }
	X width() const { return cells_.width(); }

	bool IsInconsistent() const { return inconsistent_; }
	bool IsFullySolved() const { return fully_solved_; }
	void SetInconsistent() { inconsistent_ = true; }

	int GetCell(CellPosition pos) const { return cells_.at(pos).value; }

	void DecideCell(CellPosition cell, int value) { DecideCell(cells_.GetIndex(cell), value); }
	void EliminateCandidate(CellPosition cell, int value) { EliminateCandidate(cells_.GetIndex(cell), value); }

	void CheckGroupAll();

private:
	struct Cell
	{
		// <value> is 1-origin, but bit indices of <candidates> is 0-origin.
		int value;
		unsigned int candidates;
		int group_id[2], next_cell[2];

		Cell() : value(kCellUndecided), candidates(kFullyUndecidedCandidates), group_id(), next_cell() {}
		Cell(int value) : value(value), candidates(0), group_id(), next_cell() {}
	};
	struct CellGroup
	{
		int representative;
		int n_cells, expected_sum;
		int n_decided, current_sum;
		int group_candidate;

		CellGroup() : representative(-1), n_cells(0), expected_sum(0), n_decided(0), current_sum(0), group_candidate((1 << kMaxCellValue) - 1) {}
	};

	static const unsigned int kFullyUndecidedCandidates = (1 << kMaxCellValue) - 1;

	void DecideCell(int cell_id, int value);
	void EliminateCandidate(int cell_id, int cand_value);
	void RestrictCandidate(int cell_id, unsigned int restriction);
	void EliminateCandidateFromOtherCellsInGroup(int cell_id, int cand_value);

	void CheckGroup(int group_id);

	Grid<Cell> cells_;
	CellGroup *groups_;
	Dictionary *dictionary_;
	int n_groups_;
	bool inconsistent_, fully_solved_;
};
}
}
