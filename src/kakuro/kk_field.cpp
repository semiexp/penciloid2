#include "kk_field.h"
#include "../common/util.h"

namespace
{
#ifdef _MSC_VER
inline int popcount(int n) { return __popcnt(n); }
#else
inline int popcount(int n) { return __builtin_popcount(n); }
#endif
}

namespace penciloid
{
namespace kakuro
{
Field::Field() : cells_(), queue_(), groups_(nullptr), dictionary_(nullptr), n_groups_(0), inconsistent_(false), fully_solved_(false)
{
}
Field::Field(const Problem &problem, Dictionary *dictionary) :
	cells_(problem.height(), problem.width(), Cell()),
	queue_(),
	groups_(nullptr),
	dictionary_(dictionary),
	n_groups_(0),
	inconsistent_(false),
	fully_solved_(false)
{
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			Clue c = problem.GetClue(CellPosition(y, x));
			if (c != kEmptyCell) {
				if (c.horizontal != kNoClueValue) ++n_groups_;
				if (c.vertical != kNoClueValue) ++n_groups_;
			}
		}
	}
	groups_ = new CellGroup[n_groups_];
	queue_ = SearchQueue(n_groups_);

	int current_group_id = 0;
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			Clue c = problem.GetClue(CellPosition(y, x));
			if (c == kEmptyCell) continue;

			cells_.at(CellPosition(y, x)) = Cell(kCellClue);
			if (c.horizontal != kNoClueValue) {
				int previous_cell_id = -1;
				int n_cells = 0;
				for (X x2(x + 1); x2 < width() && problem.GetClue(CellPosition(y, x2)) == kEmptyCell; ++x2) {
					int cell_id = cells_.GetIndex(CellPosition(y, x2));
					cells_.at(cell_id).group_id[0] = current_group_id;
					cells_.at(cell_id).next_cell[0] = previous_cell_id;
					previous_cell_id = cell_id;
					++n_cells;
				}
				cells_.at(CellPosition(y, x + 1)).next_cell[0] = previous_cell_id;
				groups_[current_group_id].representative = previous_cell_id;
				groups_[current_group_id].n_cells = n_cells;
				groups_[current_group_id].expected_sum = c.horizontal;
				++current_group_id;
			}
			if (c.vertical != kNoClueValue) {
				int previous_cell_id = -1;
				int n_cells = 0;
				for (Y y2(y + 1); y2 < height() && problem.GetClue(CellPosition(y2, x)) == kEmptyCell; ++y2) {
					int cell_id = cells_.GetIndex(CellPosition(y2, x));
					cells_.at(cell_id).group_id[1] = current_group_id;
					cells_.at(cell_id).next_cell[1] = previous_cell_id;
					previous_cell_id = cell_id;
					++n_cells;
				}
				cells_.at(CellPosition(y + 1, x)).next_cell[1] = previous_cell_id;
				groups_[current_group_id].representative = previous_cell_id;
				groups_[current_group_id].n_cells = n_cells;
				groups_[current_group_id].expected_sum = c.vertical;
				++current_group_id;
			}
		}
	}
}
Field::Field(const Field &other) :
	cells_(other.cells_),
	queue_(other.queue_),
	groups_(new CellGroup[other.n_groups_]),
	n_groups_(other.n_groups_),
	inconsistent_(other.inconsistent_),
	fully_solved_(other.fully_solved_)
{
	for (int i = 0; i < n_groups_; ++i) groups_[i] = other.groups_[i];
}
Field::Field(Field &&other) :
	cells_(std::move(other.cells_)),
	queue_(std::move(other.queue_)),
	groups_(other.groups_),
	n_groups_(other.n_groups_),
	inconsistent_(other.inconsistent_),
	fully_solved_(other.fully_solved_)
{
	other.groups_ = nullptr;
}
Field::~Field()
{
	if (groups_) delete[] groups_;
}
Field &Field::operator=(const Field &other)
{
	cells_ = other.cells_;
	queue_ = other.queue_;
	n_groups_ = other.n_groups_;
	inconsistent_ = other.inconsistent_;
	fully_solved_ = other.fully_solved_;

	if (groups_ != nullptr) delete[] groups_;
	groups_ = new CellGroup[n_groups_];
	for (int i = 0; i < n_groups_; ++i) groups_[i] = other.groups_[i];

	return *this;
}
Field &Field::operator=(Field &&other)
{
	cells_ = std::move(other.cells_);
	queue_ = std::move(other.queue_);
	if (groups_ != nullptr) delete[] groups_;
	groups_ = other.groups_;
	n_groups_ = other.n_groups_;
	inconsistent_ = other.inconsistent_;
	fully_solved_ = other.fully_solved_;

	return *this;
}
void Field::DecideCell(int cell_id, int value)
{
	Cell &cell = cells_.at(cell_id);
	if (cell.value != kCellUndecided) {
		if (cell.value != value) SetInconsistent();
		return;
	}
	cell.value = value;

	groups_[cell.group_id[0]].current_sum += value;
	groups_[cell.group_id[0]].n_decided += 1;
	groups_[cell.group_id[0]].group_candidate &= ~(1 << (value - 1));

	groups_[cell.group_id[1]].current_sum += value;
	groups_[cell.group_id[1]].n_decided += 1;
	groups_[cell.group_id[1]].group_candidate &= ~(1 << (value - 1));

	queue_.Push(cell.group_id[0]);
	queue_.Push(cell.group_id[1]);
	EliminateCandidateFromOtherCellsInGroup(cell_id, value);
}
void Field::EliminateCandidate(int cell_id, int cand_value)
{
	RestrictCandidate(cell_id, ~(1 << (cand_value - 1)));
}
void Field::RestrictCandidate(int cell_id, unsigned int restriction)
{
	Cell &cell = cells_.at(cell_id);
	cell.candidates &= restriction;
	if (cell.candidates == 0) {
		SetInconsistent();
		return;
	}
	if (cell.candidates == (cell.candidates & -static_cast<int>(cell.candidates))) {
		DecideCell(cell_id, 1 + PopCount(cell.candidates - 1));
	}
}
void Field::EliminateCandidateFromOtherCellsInGroup(int cell_id, int cand_value)
{
	for (int t = 0; t < 2; ++t) {
		for (int i = cells_.at(cell_id).next_cell[t]; i != cell_id; i = cells_.at(i).next_cell[t]) {
			EliminateCandidate(i, cand_value);
		}
	}
}
void Field::CheckGroup(int group_id)
{
	if (dictionary_ != nullptr) {
		CellGroup &grp = groups_[group_id];
		int next_candidate = dictionary_->GetPossibleCandidates(grp.n_cells - grp.n_decided, grp.expected_sum - grp.current_sum, grp.group_candidate);
		if (grp.group_candidate != next_candidate) {
			int cell = groups_[group_id].representative;
			int t = (cells_.at(cell).group_id[0] == group_id ? 0 : 1);
			do {
				if (cells_.at(cell).value == kCellUndecided) {
					RestrictCandidate(cell, next_candidate);
				}
				cell = cells_.at(cell).next_cell[t];
			} while (cell != groups_[group_id].representative);
		}
	}
}
void Field::DecideCell(CellPosition pos, int value)
{
	if (queue_.IsActive()) {
		DecideCell(cells_.GetIndex(pos), value);
	} else {
		queue_.Activate();
		DecideCell(cells_.GetIndex(pos), value);
		QueueProcessAll();
		queue_.Deactivate();
	}
}
void Field::EliminateCandidate(CellPosition pos, int value)
{
	if (queue_.IsActive()) {
		EliminateCandidate(cells_.GetIndex(pos), value);
	} else {
		queue_.Activate();
		EliminateCandidate(cells_.GetIndex(pos), value);
		QueueProcessAll();
		queue_.Deactivate();
	}
}
void Field::CheckGroupAll()
{
	if (queue_.IsActive()) {
		for (int i = 0; i < n_groups_; ++i) queue_.Push(i);
	} else {
		queue_.Activate();
		for (int i = 0; i < n_groups_; ++i) queue_.Push(i);
		QueueProcessAll();
		queue_.Deactivate();
	}
}
void Field::QueueProcessAll()
{
	while (!queue_.IsEmpty()) {
		int id = queue_.Pop();
		if (IsInconsistent()) return;
		CheckGroup(id);
	}
}
}
}
