#include "nk_field.h"

#include <xutility>
#include <algorithm>
#include <queue>
#include <iostream>
#include <iomanip>

#include "../common/graph_separation.h"

namespace penciloid
{
namespace nurikabe
{
Field::Field() : cells_(), decided_cells_(0), inconsistent_(false), fully_solved_(false)
{
}
Field::Field(const Problem &problem) : cells_(problem.height(), problem.width(), Cell(kNoClue, kCellUndecided)), decided_cells_(0), inconsistent_(false), fully_solved_(false)
{
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			cells_.at(CellPosition(y, x)).group_next_cell = cells_.GetIndex(CellPosition(y, x));

			Clue c = problem.GetClue(CellPosition(y, x));
			if (c != kNoClue) {
				cells_.at(CellPosition(y, x)).clue = c;
			}
		}
	}
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			Clue c = problem.GetClue(CellPosition(y, x));
			if (c != kNoClue) {
				DecideCell(CellPosition(y, x), kCellWhite);
			}
		}
	}
}
Field::Field(const Field &other) : cells_(other.cells_), inconsistent_(other.inconsistent_), decided_cells_(other.decided_cells_), fully_solved_(other.fully_solved_)
{
}
Field::Field(Field &&other) : cells_(std::move(other.cells_)), decided_cells_(other.decided_cells_), inconsistent_(other.inconsistent_), fully_solved_(other.fully_solved_)
{
}
Field::~Field()
{
}
Field &Field::operator=(const Field &other)
{
	cells_ = other.cells_;
	decided_cells_ = other.decided_cells_;
	inconsistent_ = other.inconsistent_;
	fully_solved_ = other.fully_solved_;
	return *this;
}
Field &Field::operator=(Field &&other)
{
	cells_ = std::move(other.cells_);
	decided_cells_ = other.decided_cells_;
	inconsistent_ = other.inconsistent_;
	fully_solved_ = other.fully_solved_;
	return *this;
}
void Field::DecideCell(CellPosition pos, CellState status)
{
	Cell &cell = cells_.at(pos);
	if (cell.status != kCellUndecided) {
		if (cell.status != status) {
			SetInconsistent();
		}
		return;
	}

	if (++decided_cells_ == static_cast<int>(height()) * static_cast<int>(width())) {
		fully_solved_ = true;
	}

	cell.status = status;
	for (Direction d : k4Neighborhood) {
		CellPosition pos2 = pos + d;
		if (cells_.IsPositionOnGrid(pos2) && cells_.at(pos2).status == status) {
			Join(cells_.GetIndex(pos), cells_.GetIndex(pos2));
		}
	}

	if (status == kCellBlack) {
		// avoid 2*2 cluster of black cells
		CheckCluster(pos - Direction(Y(0), X(0)));
		CheckCluster(pos - Direction(Y(0), X(1)));
		CheckCluster(pos - Direction(Y(1), X(0)));
		CheckCluster(pos - Direction(Y(1), X(1)));
	} else {
		int root_idx = GetRoot(GetIndex(pos));
		Cell &root_cell = cells_.at(root_idx);
		if (root_cell.clue != kNoClue) {
			AvoidGroupWithSeveralClue(pos);

			if (root_cell.clue.clue_high == GetGroupSize(root_idx)) {
				CloseGroup(root_idx);
			} else if (root_cell.clue.clue_high < GetGroupSize(root_idx)) {
				SetInconsistent();
				return;
			}
		}
	}
}
int Field::GetRoot(int cell_idx)
{
	if (cells_.at(cell_idx).group_parent_cell < 0) return cell_idx;
	return cells_.at(cell_idx).group_parent_cell = GetRoot(cells_.at(cell_idx).group_parent_cell);
}
void Field::Join(int cell_idx1, int cell_idx2)
{
	cell_idx1 = GetRoot(cell_idx1);
	cell_idx2 = GetRoot(cell_idx2);

	if (cell_idx1 == cell_idx2) return;

	auto &cell1 = cells_.at(cell_idx1);
	auto &cell2 = cells_.at(cell_idx2);

	if (cell1.clue != kNoClue && cell2.clue != kNoClue) {
		SetInconsistent();
		return;
	}

	std::vector<int> cells_to_be_checked;
	if (cell1.clue != kNoClue || cell2.clue != kNoClue) {
		int p_init = (cell1.clue != kNoClue ? cell_idx2 : cell_idx1);
		int p = p_init;
		do {
			cells_to_be_checked.push_back(p);
			p = cells_.at(p).group_next_cell;
		} while (p != p_init);
	}
	if (cell1.clue == kNoClue && (cell2.clue != kNoClue || cell1.group_parent_cell > cell2.group_parent_cell)) {
		cell2.group_parent_cell += cell1.group_parent_cell;
		cell1.group_parent_cell = cell_idx2;
	} else {
		cell1.group_parent_cell += cell2.group_parent_cell;
		cell2.group_parent_cell = cell_idx1;
	}

	std::swap(cell1.group_next_cell, cell2.group_next_cell);

	for (int p : cells_to_be_checked) {
		AvoidGroupWithSeveralClue(cells_.AsPosition(p));
	}
}
void Field::CheckCluster(CellPosition top)
{
	if (!(cells_.IsPositionOnGrid(top) && cells_.IsPositionOnGrid(top + Direction(Y(1), X(1))))) return;

	int cnt = 0;
	if (GetCell(top + Direction(Y(0), X(0))) == kCellBlack) ++cnt;
	if (GetCell(top + Direction(Y(0), X(1))) == kCellBlack) ++cnt;
	if (GetCell(top + Direction(Y(1), X(0))) == kCellBlack) ++cnt;
	if (GetCell(top + Direction(Y(1), X(1))) == kCellBlack) ++cnt;

	if (cnt == 4) {
		SetInconsistent();
		return;
	} else if (cnt == 3) {
		if (GetCell(top + Direction(Y(0), X(0))) == kCellUndecided) DecideCell(top + Direction(Y(0), X(0)), kCellWhite);
		if (GetCell(top + Direction(Y(0), X(1))) == kCellUndecided) DecideCell(top + Direction(Y(0), X(1)), kCellWhite);
		if (GetCell(top + Direction(Y(1), X(0))) == kCellUndecided) DecideCell(top + Direction(Y(1), X(0)), kCellWhite);
		if (GetCell(top + Direction(Y(1), X(1))) == kCellUndecided) DecideCell(top + Direction(Y(1), X(1)), kCellWhite);
	}
}
void Field::AvoidGroupWithSeveralClue(CellPosition pos)
{
	int root_pos = GetRoot(GetIndex(pos));
	if (!HasClueInGroup(root_pos)) return;

	for (int d = 0; d < 4; ++d) {
		Direction dir1 = k4Neighborhood[d], dir2 = k4Neighborhood[(d + 1) % 4];
		{
			CellPosition pos2 = pos + dir1 * 2;
			if (cells_.IsPositionOnGrid(pos2) && GetCell(pos2) == kCellWhite) {
				int root_pos2 = GetRoot(GetIndex(pos2));
				if (HasClueInGroup(root_pos2) && root_pos != root_pos2) {
					DecideCell(pos + dir1, kCellBlack);
				}
			}
		}
		{
			CellPosition pos2 = pos + dir1 + dir2;
			if (cells_.IsPositionOnGrid(pos2) && GetCell(pos2) == kCellWhite) {
				int root_pos2 = GetRoot(GetIndex(pos2));
				if (HasClueInGroup(root_pos2) && root_pos != root_pos2) {
					DecideCell(pos + dir1, kCellBlack);
					DecideCell(pos + dir2, kCellBlack);
				}
			}
		}
	}
}
void Field::CloseGroup(int cell_idx)
{
	int p = cell_idx;
	do {
		CellPosition pos = cells_.AsPosition(p);
		for (Direction d : k4Neighborhood) {
			CellPosition pos2 = pos + d;
			if (cells_.IsPositionOnGrid(pos2) && GetCell(pos2) == kCellUndecided) {
				DecideCell(pos2, kCellBlack);
			}
		}
		p = cells_.at(p).group_next_cell;
	} while (p != cell_idx);
}
void Field::ExpandBlack()
{
	struct Vector2
	{
		Vector2(int n_cell = 0, int n_black = 0) : n_cell(n_cell), n_black(n_black) {}

		inline Vector2 operator+(const Vector2 &other) { return Vector2(n_cell + other.n_cell, n_black + other.n_black); }
		inline Vector2 operator-() { return Vector2(-n_cell, -n_black); }

		int n_cell, n_black;
	};
	int n_cells = static_cast<int>(height()) * static_cast<int>(width());
	GraphSeparation<Vector2> graph(n_cells, n_cells * 2);

	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			CellState cell = GetCell(CellPosition(y, x));
			int idx = GetIndex(CellPosition(y, x));
			if (cell == kCellUndecided) graph.SetValue(idx, Vector2(1, 0));
			else if (cell == kCellBlack) graph.SetValue(idx, Vector2(1, 1));
			else continue;

			if (y != height() - 1 && GetCell(CellPosition(y + 1, x)) != kCellWhite) {
				graph.AddEdge(idx, GetIndex(CellPosition(y + 1, x)));
			}
			if (x != width() - 1 && GetCell(CellPosition(y, x + 1)) != kCellWhite) {
				graph.AddEdge(idx, GetIndex(CellPosition(y, x + 1)));
			}
		}
	}
	graph.Construct();

	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			if (GetCell(CellPosition(y, x)) != kCellUndecided) continue;
			std::vector<Vector2> separate_info = graph.Separate(GetIndex(CellPosition(y, x)));
			int n_black_unit = 0;
			for (Vector2 &vec : separate_info) {
				if (vec.n_black > 0) ++n_black_unit;
			}
			if (n_black_unit >= 2) {
				DecideCell(CellPosition(y, x), kCellBlack);
			}
		}
	}
}
void Field::ExpandWhite()
{
	/*
	necessary information for each cell:
	- the status of the cell (black / white local / white global)
	- master cell for local cells
	- identifier
	 */
	enum CellType
	{
		kBlack, kWhiteLocal, kWhiteGlobal
	};
	struct CellData
	{
		int master, id;
		CellType type;

		CellData() : master(-1), id(-1), type(kBlack) {}
	};
	Grid<CellData> data(height(), width());
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			if (GetCell(CellPosition(y, x)) == kCellBlack) continue;
			if (HasClueInGroup(GetIndex(CellPosition(y, x)))) {
				data.at(CellPosition(y, x)).type = kWhiteLocal;
				data.at(CellPosition(y, x)).master = GetRoot(GetIndex(CellPosition(y, x)));
				for (Direction d : k4Neighborhood) {
					CellPosition nb = CellPosition(y, x) + d;
					if (cells_.IsPositionOnGrid(nb) && GetCell(nb) != kCellBlack) {
						data.at(nb).type = kWhiteLocal;
						data.at(nb).master = GetRoot(GetIndex(CellPosition(y, x)));
					}
				}
			} else {
				if (data.at(CellPosition(y, x)).type == kBlack) {
					data.at(CellPosition(y, x)).type = kWhiteGlobal;
				}
			}
		}
	}
	int id_next = 0;
	std::queue<CellPosition> q;
	std::vector<CellPosition> group_undecided;
	std::vector<CellPosition> corner_black;

	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			CellType type = data.at(CellPosition(y, x)).type;
			if (type == kBlack || (type == kWhiteLocal && cells_.at(CellPosition(y, x)).clue == kNoClue)) continue;
			if (data.at(CellPosition(y, x)).id != -1) continue;

			int master_id = GetIndex(CellPosition(y, x));
			int expected_master = (type == kWhiteLocal ? data.at(CellPosition(y, x)).master : -1);
			q.push(CellPosition(y, x));
			data.at(CellPosition(y, x)).id = id_next;
			group_undecided.clear();
			while (!q.empty()) {
				CellPosition pos = q.front(); q.pop();
				data.at(pos).master = master_id;
				if (GetCell(pos) == kCellUndecided) group_undecided.push_back(pos);

				for (Direction d : k4Neighborhood) {
					CellPosition pos2 = pos + d;
					if (data.IsPositionOnGrid(pos2) && data.at(pos).type == data.at(pos2).type && data.at(pos2).id == -1 && (expected_master == -1 || data.at(pos2).master == expected_master)) {
						q.push(pos2);

						data.at(pos2).id = id_next;
					}
				}
			}

			Cell c = cells_.at(CellPosition(y, x));
			if (c.clue != kNoClue && -c.group_parent_cell + 1 == c.clue.clue_low && c.clue.clue_low == c.clue.clue_high && group_undecided.size() == 2) {
				for (Direction d : k4Neighborhood) {
					CellPosition cand = group_undecided[0] + d;
					if (abs(cand.x - group_undecided[1].x) + abs(cand.y - group_undecided[1].y) == 1 && GetCell(cand) == kCellUndecided) {
						corner_black.push_back(cand);
					}
				}
			}
			++id_next;
		}
	}

	struct Adjacency
	{
		int local_cell;
		int local_master, global_master;

		Adjacency(int local_cell, int local_master, int global_master) :
			local_cell(local_cell), local_master(local_master), global_master(global_master) {}
		inline bool operator<(const Adjacency &other)
		{
			return std::make_pair(local_master, global_master) < std::make_pair(other.local_master, other.global_master);
		}
		inline bool operator==(const Adjacency &other)
		{
			return local_master == other.local_master && global_master == other.global_master;
		}
	};
	std::vector<Adjacency> lg_adjacency;
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			CellPosition pos(y, x);
			if (data.at(pos).type != kWhiteLocal) continue;
			for (Direction d : k4Neighborhood) {
				CellPosition pos2 = pos + d;
				if (data.IsPositionOnGrid(pos2) && data.at(pos2).type != kBlack && data.at(pos2).id != data.at(pos).id) {
					lg_adjacency.push_back(Adjacency(GetIndex(pos), data.at(pos).master, data.at(pos2).master));
				}
			}
		}
	}
	std::sort(lg_adjacency.begin(), lg_adjacency.end());

	struct GroupData
	{
		int clue_min, clue_max;
		int n_cells, n_whites;
		int other_locals;

		GroupData(int clue_min = 0, int clue_max = 0, int n_cells = 0, int n_whites = 0, int other_locals = 0) :
			clue_min(clue_min), clue_max(clue_max), n_cells(n_cells), n_whites(n_whites), other_locals(other_locals) {}

		inline GroupData operator+(const GroupData &other) {
			return GroupData(clue_min + other.clue_min, clue_max + other.clue_max, n_cells + other.n_cells, n_whites + other.n_whites, other_locals + other.other_locals);
		}
		inline GroupData operator-() {
			return GroupData(-clue_min, -clue_max, -n_cells, -n_whites, -other_locals);
		}
	};
	std::vector<GroupData> group_summary(id_next, GroupData());
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			Cell cell = cells_.at(CellPosition(y, x));
			if (cell.status == kCellBlack) continue;

			int id = data.at(CellPosition(y, x)).id;
			group_summary[id].n_cells += 1;
			if (cell.status == kCellWhite) {
				group_summary[id].n_whites += 1;
				if (cell.clue != kNoClue) {
					group_summary[id].clue_min += cell.clue.clue_low;
					group_summary[id].clue_max += cell.clue.clue_high;
				}
			}
		}
	}

	int field_size = static_cast<int>(height()) * static_cast<int>(width());

	// Process local cells
	int n_global_units = 0;
	for (int i = 0; i < lg_adjacency.size(); ++i) {
		if (i == 0 || !(lg_adjacency[i] == lg_adjacency[i - 1])) {
			int global_id = data.at(lg_adjacency[i].global_master).id;
			int local_id = data.at(lg_adjacency[i].local_master).id;
			if (data.at(lg_adjacency[i].global_master).type == kWhiteGlobal) {
				group_summary[global_id] = group_summary[global_id] + GroupData(0, 0, 0, 0, 1);
			}
			++n_global_units;
		}
	}
	GraphSeparation<GroupData> graph_local(field_size + n_global_units, field_size * 2);
	// edges: local -- local
	const int kClueMaster = 1 << 28;
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			CellPosition pos(y, x);
			if (data.at(pos).type != kWhiteLocal) continue;
			int current_id = data.at(pos).id;

			if (y > 0 && data.at(pos - Direction(Y(1), X(0))).id == current_id) {
				graph_local.AddEdge(GetIndex(pos), GetIndex(pos - Direction(Y(1), X(0))));
			}
			if (x > 0 && data.at(pos - Direction(Y(0), X(1))).id == current_id) {
				graph_local.AddEdge(GetIndex(pos), GetIndex(pos - Direction(Y(0), X(1))));
			}

			if (cells_.at(pos).clue == kNoClue) {
				graph_local.SetValue(GetIndex(pos), GroupData(
					0, 0, 1, GetCell(pos) == kCellWhite ? 1 : 0
				));
			} else {
				graph_local.SetValue(GetIndex(pos), GroupData(
					kClueMaster, kClueMaster, 1, 1
				));
			}
		}
	}
	// edges: local -- global
	int vertex_id = field_size;
	for (int i = 0; i < lg_adjacency.size(); ++i) {
		graph_local.AddEdge(vertex_id, lg_adjacency[i].local_cell);
		if (i == lg_adjacency.size() - 1 || !(lg_adjacency[i] == lg_adjacency[i + 1])) {
			int global_id = data.at(lg_adjacency[i].global_master).id;
			int local_id = data.at(lg_adjacency[i].local_master).id;
			if (data.at(lg_adjacency[i].global_master).type == kWhiteGlobal) {
				graph_local.SetValue(vertex_id, group_summary[global_id] + GroupData(0, 0, 0, 0, -1));
			} else {
				graph_local.SetValue(vertex_id, GroupData(0, 0, 0, 0, 1));
			}
			++vertex_id;
		}
	}
	graph_local.Construct();
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			int idx = GetIndex(CellPosition(y, x));
			if (data.at(idx).type != kWhiteLocal) continue;

			std::vector<GroupData> sep = graph_local.Separate(idx);
			for (GroupData d : sep) {
				//bool only_master = (d.clue_min & ~kClueMaster) == 0;
				if (d.clue_min & kClueMaster) {
					d.clue_min = (d.clue_min ^ kClueMaster) + cells_.at(data.at(idx).master).clue.clue_low;
					d.clue_max = (d.clue_max ^ kClueMaster) + cells_.at(data.at(idx).master).clue.clue_high;
				}
				if ((d.other_locals == 0 && d.clue_max < d.n_whites) || (d.n_cells < d.clue_min) || (d.other_locals == 0 && d.clue_max == 0 && d.n_cells >= 2)) {
					DecideCell(CellPosition(y, x), kCellWhite);
				}
			}
		}
	}

	// Process global cells
	GraphSeparation<GroupData> graph_global(field_size, field_size * 2);
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			CellPosition pos(y, x);
			if (data.at(pos).type == kBlack) continue;
			bool is_local = (data.at(pos).type == kWhiteLocal);

			bool flg = (y == 1 && x == 2);
			if (y > 0) {
				CellPosition pos2 = pos + Direction(Y(-1), X(0));
				if (data.at(pos2).type != kBlack && !(is_local && data.at(pos).id == data.at(pos2).id)) {
					graph_global.AddEdge(
						is_local ? data.at(pos).master : GetIndex(pos),
						data.at(pos2).type == kWhiteLocal ? data.at(pos2).master : GetIndex(pos2)
					);
				}
			}
			if (x > 0) {
				CellPosition pos2 = pos + Direction(Y(0), X(-1));
				if (data.at(pos2).type != kBlack && !(is_local && data.at(pos).id == data.at(pos2).id)) {
					graph_global.AddEdge(
						is_local ? data.at(pos).master : GetIndex(pos),
						data.at(pos2).type == kWhiteLocal ? data.at(pos2).master : GetIndex(pos2)
					);
				}
			}

			if (is_local) {
				if (data.at(pos).master == GetIndex(pos)) {
					graph_global.SetValue(GetIndex(pos), group_summary[data.at(pos).id]);
				}
			} else {
				graph_global.SetValue(GetIndex(pos), GroupData(0, 0, 1, GetCell(pos) == kCellWhite ? 1 : 0));
			}
		}
	}
	graph_global.Construct();
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			if (data.at(CellPosition(y, x)).type != kWhiteGlobal) continue;
			std::vector<GroupData> sep = graph_global.Separate(GetIndex(CellPosition(y, x)));

			for (GroupData &d : sep) {
				if (d.clue_max < d.n_whites || d.n_cells < d.clue_min || (d.clue_max == 0 && d.n_cells >= 2)) {
					DecideCell(CellPosition(y, x), kCellWhite);
					break;
				}
			}
		}
	}

	for (CellPosition pos : corner_black) DecideCell(pos, kCellBlack);
}
std::ostream &operator<<(std::ostream &stream, const Field &field)
{
	for (Y y(0); y < field.height(); ++y) {
		for (X x(0); x < field.width(); ++x) {
			Field::CellState status = field.GetCell(CellPosition(y, x));
			if (status == Field::kCellBlack) stream << "##";
			else if (status == Field::kCellUndecided) stream << "..";
			else {
				Clue clue = field.GetClue(CellPosition(y, x));
				if (clue == kNoClue) stream << "  ";
				else stream << std::setw(2) << clue.clue_low;
			}
			stream << " ";
		}
		stream << "\n";
	}
	return stream;
}
}
}