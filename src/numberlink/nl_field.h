#pragma once

#include "../common/type.h"
#include "../common/grid.h"
#include "nl_type.h"
#include "nl_problem.h"

#include <vector>
#include <algorithm>

template <typename T>
class vector2
{
public:
	vector2() : index(0) {}

	bool empty() const { return index == 0; }
	void push_back(const T &value) { data[index++] = value; }
	void pop_back() { --index; }
	T back() const { return data[index - 1]; }

private:
	T data[10000];
	int index;
};

namespace penciloid
{
namespace numberlink
{
class Field
{
public:
	enum EdgeState {
		kEdgeUndecided,
		kEdgeLine,
		kEdgeBlank
	};

	Field();

	// Initialize a field from a problem
	Field(const Problem &problem);

	~Field() {}

	inline Y height() const { return mate_.height(); }
	inline X width() const { return mate_.width(); }
	bool IsInconsistent() const { return inconsistent_; }

	// Check whether <cell> has no incident line
	bool IsIsolatedCell(CellPosition cell) const {
		int idx = GetIndex(cell);
		return mate_.at(idx) == idx;
	}

	// Get the status of the edge between <cell1> and (<cell1> + Direction(Y(0), X(1)) )
	inline EdgeState GetHorizontalLine(CellPosition cell) const { return line_horizontal_(cell); }

	// Get the status of the edge between <cell1> and (<cell1> + Direction(Y(1), X()) )
	inline EdgeState GetVerticalLine(CellPosition cell) const { return line_vertical_(cell); }

	// Set the status of the edge between <cell1> and (<cell1> + Direction(Y(0), X(1)) ) kEdgeLine / kEdgeBlack
	void SetHorizontalLine(CellPosition cell);
	void SetHorizontalBlank(CellPosition cell);

	// Set the status of the edge between <cell1> and (<cell1> + Direction(Y(1), X(0)) ) kEdgeLine / kEdgeBlack
	void SetVerticalLine(CellPosition cell);
	void SetVerticalBlank(CellPosition cell);

	void AddRestorePoint() { history_.push_back({ 4, 0 }); }
	void Restore();

private:
	void SetInconsistent() {
		history_.push_back({ 3, (int)inconsistent_ });
		inconsistent_ = true;
	}
	int GetIndex(CellPosition cell) const { return mate_.GetIndex(cell); }
	void UpdateMate(int idx, int val) {
		history_.push_back({ (idx << 3) | 0, mate_.at(idx) });
		mate_.at(idx) = val;
	}
	void UpdateLineHorizontal(int idx, EdgeState val) {
		history_.push_back({ (idx << 3) | 1, line_horizontal_.at(idx) });
		line_horizontal_.at(idx) = val;
	}
	void UpdateLineVertical(int idx, EdgeState val) {
		history_.push_back({ (idx << 3) | 2, line_vertical_.at(idx) });
		line_vertical_.at(idx) = val;
	}

	void Inspect(CellPosition cell);
	void Join(CellPosition cell1, CellPosition cell2);

	const int kFullyConnectedCell = 0x7fffffff;

	vector2<std::pair<int, int> > history_;
	Grid<int> mate_;
	Grid<EdgeState> line_horizontal_, line_vertical_;
	Grid<bool> endpoint_;
	bool inconsistent_;
};
}
}
