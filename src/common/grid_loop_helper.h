#pragma once

#include "grid_loop.h"
#include "union_find.h"

namespace penciloid
{
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
	
	auto cell_id = [height, width, field_outside](Position pos) -> int {
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
			Position cell_position(2 * y + 1, 2 * x + 1);
			for (int d = 0; d < 4; ++d) {
				if (grid->GetEdge(cell_position + dirs[d]) == GridLoop<T>::EDGE_LINE) {
					uf.Join(cell_id(Position(y, x)), cell_id(Position(y, x) + dirs[d]) + 1);
					uf.Join(cell_id(Position(y, x)) + 1, cell_id(Position(y, x) + dirs[d]));
				} else if (grid->GetEdge(cell_position + dirs[d]) == GridLoop<T>::EDGE_BLANK) {
					uf.Join(cell_id(Position(y, x)), cell_id(Position(y, x) + dirs[d]));
					uf.Join(cell_id(Position(y, x)) + 1, cell_id(Position(y, x) + dirs[d]) + 1);
				}
			}
		}
	}

	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			Position cell_position(2 * y + 1, 2 * x + 1);
			for (int d = 0; d < 4; ++d) {
				if (uf.Root(cell_id(Position(y, x))) == uf.Root(cell_id(Position(y, x) + dirs[d]))) {
					grid->DecideEdge(cell_position + dirs[d], GridLoop<T>::EDGE_BLANK);
				}
				if (uf.Root(cell_id(Position(y, x))) == uf.Root(cell_id(Position(y, x) + dirs[d]) + 1)) {
					grid->DecideEdge(cell_position + dirs[d], GridLoop<T>::EDGE_LINE);
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

	auto pos_id = [height, width](Position pos) -> int {
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
				if (grid->GetEdgeSafe(Position(y, x) + dirs[d]) != GridLoop<T>::EDGE_BLANK) {
					// We can assume that Position(y, x) + dirs[d] is a valid position on the grid
					uf.Join(pos_id(Position(y, x)), pos_id(Position(y, x) + dirs[d]));
				}
			}
		}
	}

	int line_root = -1;
	for (Y y(0); y <= 2 * height; ++y) {
		for (X x(0); x <= 2 * width; ++x) {
			if (static_cast<int>(y) % 2 != static_cast<int>(x) % 2) {
				if (grid->GetEdge(Position(y, x)) == GridLoop<T>::EDGE_LINE) {
					int root = uf.Root(pos_id(Position(y, x)));
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
}