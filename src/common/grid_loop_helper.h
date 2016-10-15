#pragma once

#include "grid_loop.h"
#include "union_find.h"

namespace penciloid
{
// Using GridLoop<T> is allowed as long as the destructor isn't called

template <class T>
void ApplyInOutRule(GridLoop<T> *grid)
{
	Y height = grid->height();
	X width = grid->width();
	int number_of_cells = static_cast<int>(height) * static_cast<int>(width);
	int field_outside = number_of_cells;

	// uf[2 * i] : i-th cell
	// uf[2 * i + 1] : virtual cell which is opposite to i-th cell
	UnionFind uf(2 * (number_of_cells + 1));
	
	auto cell_id = [height, width, field_outside](CellPosition pos) -> int {
		if (0 <= pos.y && pos.y < height && 0 <= pos.x && pos.x < width) {
			return 2 * (static_cast<int>(pos.y) * static_cast<int>(width) + static_cast<int>(pos.x));
		}
		return field_outside * 2;
	};

	static const Direction dirs[] = {
		Direction(Y(1), X(0)),
		Direction(Y(0), X(1)),
		Direction(Y(-1), X(0)),
		Direction(Y(0), X(-1))
	};

	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			LoopPosition cell_position(2 * y + 1, 2 * x + 1);
			for (int d = 0; d < 4; ++d) {
				if (grid->GetEdge(cell_position + dirs[d]) == GridLoop<T>::kEdgeLine) {
					uf.Join(cell_id(CellPosition(y, x)), cell_id(CellPosition(y, x) + dirs[d]) + 1);
					uf.Join(cell_id(CellPosition(y, x)) + 1, cell_id(CellPosition(y, x) + dirs[d]));
				} else if (grid->GetEdge(cell_position + dirs[d]) == GridLoop<T>::kEdgeBlank) {
					uf.Join(cell_id(CellPosition(y, x)), cell_id(CellPosition(y, x) + dirs[d]));
					uf.Join(cell_id(CellPosition(y, x)) + 1, cell_id(CellPosition(y, x) + dirs[d]) + 1);
				}
			}
		}
	}

	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			LoopPosition cell_position(2 * y + 1, 2 * x + 1);
			for (int d = 0; d < 4; ++d) {
				if (uf.Root(cell_id(CellPosition(y, x))) == uf.Root(cell_id(CellPosition(y, x) + dirs[d]))) {
					grid->DecideEdge(cell_position + dirs[d], GridLoop<T>::kEdgeBlank);
				}
				if (uf.Root(cell_id(CellPosition(y, x))) == uf.Root(cell_id(CellPosition(y, x) + dirs[d]) + 1)) {
					grid->DecideEdge(cell_position + dirs[d], GridLoop<T>::kEdgeLine);
				}
			}
		}
	}
}
template <class T>
void CheckConnectability(GridLoop<T> *grid)
{
	Y height = grid->height();
	X width = grid->width();
	int segment_count = static_cast<int>(2 * height + 1) * static_cast<int>(2 * width + 1);
	UnionFind uf(segment_count);

	auto pos_id = [height, width](LoopPosition pos) -> int {
		return static_cast<int>(pos.y) * static_cast<int>(2 * width + 1) + static_cast<int>(pos.x);
	};

	static const Direction dirs[] = {
		Direction(Y(1), X(0)),
		Direction(Y(0), X(1)),
		Direction(Y(-1), X(0)),
		Direction(Y(0), X(-1))
	};

	for (Y y(0); y <= 2 * height; y += 2) {
		for (X x(0); x <= 2 * width; x += 2) {
			for (int d = 0; d < 4; ++d) {
				if (grid->GetEdgeSafe(LoopPosition(y, x) + dirs[d]) != GridLoop<T>::kEdgeBlank) {
					// We can assume that Position(y, x) + dirs[d] is a valid position on the grid
					uf.Join(pos_id(LoopPosition(y, x)), pos_id(LoopPosition(y, x) + dirs[d]));
				}
			}
		}
	}

	int line_root = -1;
	for (Y y(0); y <= 2 * height; ++y) {
		for (X x(0); x <= 2 * width; ++x) {
			if (static_cast<int>(y) % 2 != static_cast<int>(x) % 2) {
				if (grid->GetEdge(LoopPosition(y, x)) == GridLoop<T>::kEdgeLine) {
					int root = uf.Root(pos_id(LoopPosition(y, x)));
					if (line_root == -1) line_root = root;
					else if (line_root != root) {
						grid->SetInconsistent();
						return;
					}
				}
			}
		}
	}
}
template <class T>
void Assume(T *grid)
{
	Y height = grid->GridLoop<T>::height();
	X width = grid->GridLoop<T>::width();
	while (true) {
		bool updated = false;
		for (Y y(0); y <= height * 2; ++y) {
			for (X x(0); x <= width * 2; ++x) {
				if (static_cast<int>(y % 2) == static_cast<int>(x % 2)) continue;
				if (grid->GetEdge(LoopPosition(y, x)) != T::kEdgeUndecided) continue;

				T field_line = *grid, field_blank = *grid;
				field_line.DecideEdge(LoopPosition(y, x), T::kEdgeLine);
				field_blank.DecideEdge(LoopPosition(y, x), T::kEdgeBlank);

				if (field_line.IsInconsistent() && field_blank.IsInconsistent()) {
					grid->SetInconsistent();
					return;
				}
				if (field_line.IsInconsistent()) {
					*grid = field_blank;
					updated = true;
				} else if (field_blank.IsInconsistent()) {
					*grid = field_line;
					updated = true;
				}
			}
		}
		if (!updated) break;
	}
}
}
