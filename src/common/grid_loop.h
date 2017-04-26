#pragma once

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cstring>

#include "type.h"
#include "mini_vector.h"
#include "grid_loop_method.h"
#include "auto_array.h"
#include "search_queue.h"

#include<cassert>

namespace penciloid
{
// Manages a loop on 2D-grid.
// This class uses Curiously Recurring Template Pattern (CRTP).
// This should be inherited as follows:
//   class PlainGridLoop : public GridLoop <PlainGridLoop>
template <class T>
class GridLoop
{
public:
	// Type for "number of edges".
	typedef unsigned int EdgeCount;

	enum EdgeState {
		kEdgeUndecided,
		kEdgeLine,
		kEdgeBlank
	};

	GridLoop();
	GridLoop(Y height, X width);

	GridLoop(const GridLoop &other);
	GridLoop(GridLoop &&other);
	GridLoop &operator=(const GridLoop &other);
	GridLoop &operator=(GridLoop &&other);

	~GridLoop();

	Y height() const { return height_; }
	X width() const { return width_; }

	// Returns whether <pos> is within the range of this field.
	bool IsPositionOnField(LoopPosition pos) const { return 0 <= pos.y && pos.y <= 2 * height_ && 0 <= pos.x && pos.x <= 2 * width_; }

	// Returns the number of edges whose status is kEdgeLine or kEdgeBlank.
	EdgeCount GetNumberOfDecidedEdges() const { return decided_edges_; }

	// Returns the number of edges whose status is kEdgeLine.
	EdgeCount GetNumberOfDecidedLines() const { return decided_lines_; }

	// Note that both of IsInconsistent() and IsFullySolved() may hold.
	bool IsInconsistent() const { return inconsistent_; }
	bool IsFullySolved() const { return fully_solved_; }
	bool IsInAbnormalCondition() const { return abnormal_; }
	void SetInconsistent() { 
		if (!history_.empty() && !inconsistent_) {
			history_.push_back({ kHistorySetInconsistent, FieldComponent() });
		}
		inconsistent_ = true;
	}

	GridLoopMethod GetMethod() const { return method_; }
	void SetMethod(const GridLoopMethod &m) { method_ = m; }

	// Returns the status of edge <edge>.
	// <edge> should be a legitimate position.
	EdgeState GetEdge(LoopPosition edge) const;

	// Returns the status of edge <edge>.
	// If <edge> is out of range, kEdgeBlank will be returned.
	EdgeState GetEdgeSafe(LoopPosition edge) const;

	// Makes the status of edge <edge> <state>.
	// Status of other decidable edges will be also changed.
	// edge can be out of field range.
	void DecideEdge(LoopPosition edge, EdgeState status);

	// Returns the another end of the chain, which starts from <point> to the direction <dir>.
	// <point> should be one of the ends of the chain.
	LoopPosition GetAnotherEnd(LoopPosition point, Direction dir) const;

	// Returns the length of the chain, which starts from <point> to the direction <dir>.
	// <point> should be one of the ends of the chain.
	EdgeCount GetChainLength(LoopPosition point, Direction dir) const;

	// Returns two end vertices of the chain which edge <edge> belongs to.
	// Non-const version changes internal status to perform "path compression".
	std::pair<LoopPosition, LoopPosition> GetEndsOfChain(LoopPosition edge) const;

	// Returns the identifier of the chain which edge <edge> belongs to.
	// All edges of a chain have the same identifier.
	// It is guaranteed that the identifier of a chain is changed only if the chain is connected to another one.
	unsigned int GetChainIdentifier(LoopPosition edge) const;

	// Returs whether <edge> is the representative of the chain it belongs to.
	// It is guaranteed that there is exactly one representative edge in a chain.
	bool IsRepresentativeOfChain(LoopPosition edge) const;

	// Check around position <pos>.
	// This method internally invokes Inspect(pos).
	void Check(LoopPosition pos);

	// Perform Check(pos) for all possible (vertex / cell / edge).
	void CheckAllVertex();
	void CheckAllCell();
	void CheckAllEdge();

	// Add a restore point
	void AddRestorePoint();

	// Roll back this field to the last restore point
	void Rollback();

	//
	// Public methods below are intended to be "overridden" by the subclass.
	//

	// This method (of the subclass) will be called when the status of edge <edge> was decided.
	void HasDecided(LoopPosition edge) {}

	// This method (of the subclass) should call Check(Position) for constraint propagation.
	// This will be called "something has happened" to edge <edge>, e.g. the status of <edge> was decided.
	void CheckNeighborhood(LoopPosition edge);

	// This method (of the subclass) should inspect around position <pos>.
	// <pos> does not necessarily represent the position of an edge.
	// Don't call this method directly (instead, use Check(pos) ).
	void Inspect(LoopPosition pos) {}

	// Invoke func() with the internal queue enabled.
	template <class F>
	void QueuedRun(F func) {
		if (queue_.IsActive()) func();
		else {
			queue_.Activate();
			func();
			QueueProcessAll();
			queue_.Deactivate();
		}
	}

private:
	struct FieldComponent
	{
		union {
			// Edges on an end of a chain should have correct end_vertices, another_end_edge and chain_size.
			// For other edges, following another_end_edge should lead to an end of the chain.
			// Additionally, all edge of the same chain of an edge should be able to be computed by following list_next_edge.
			// Each edge should have correct edge_status.
			struct { // as an edge
				EdgeState edge_status;
				unsigned int end_vertices[2];
				unsigned int another_end_edge;
				unsigned int list_next_edge;
				EdgeCount chain_size;
			};
			struct { // as a vertex

			};
		};
	};

	const int kHistoryRestorePoint = -1;
	const int kHistorySetInconsistent = -2;
	const int kHistorySetSolved = -3;

	bool IsVertex(LoopPosition pos) const { return pos.y % 2 == 0 && pos.x % 2 == 0; }
	bool IsEdge(LoopPosition pos) const { return static_cast<int>(pos.y % 2) != static_cast<int>(pos.x % 2); }

	unsigned int Id(Y y, X x) const { return int(y) * (2 * int(width_) + 1) + int(x); }
	unsigned int Id(LoopPosition pos) const { return int(pos.y) * (2 * int(width_) + 1) + int(pos.x); }
	LoopPosition AsPosition(unsigned int id) const { return LoopPosition(Y(id / (2 * int(width_) + 1)), X(id % (2 * int(width_) + 1))); }

	bool IsEndOfAChain(LoopPosition edge) const { return IsEndOfAChain(Id(edge)); }
	bool IsEndOfAChain(unsigned int edge_id) const { return field_[field_[edge_id].another_end_edge].another_end_edge == edge_id; }
	bool IsEndOfAChainVertex(unsigned int edge_id, unsigned int vertex_id) const;
	unsigned int GetAnotherEndAsId(LoopPosition point, Direction dir) const;

	void Check(unsigned int id) { Check(AsPosition(id)); }
	void DecideChain(unsigned int id, EdgeState status);
	void CheckNeighborhoodOfChain(unsigned int id);
	void HasFullySolved();
	// Merge the edge at (vertex + dir1) and at (vertex + dir2)
	void Join(LoopPosition vertex, Direction dir1, Direction dir2);
	void InspectVertex(LoopPosition vertex);

	void QueueProcessAll();

	AutoArray<FieldComponent> field_;
	SearchQueue queue_;
	std::vector<std::pair<int, FieldComponent> > history_;

	Y height_;
	X width_;
	EdgeCount decided_edges_, decided_lines_;
	bool inconsistent_, fully_solved_, abnormal_;

	GridLoopMethod method_;
};
template<class T>
GridLoop<T>::GridLoop()
	: field_(),
	  queue_(),
	  history_(),
	  height_(0),
	  width_(0),
	  decided_edges_(0),
	  decided_lines_(0),
	  inconsistent_(false),
	  fully_solved_(false),
	  abnormal_(false),
	  method_()
{
}
template<class T>
GridLoop<T>::GridLoop(Y height, X width)
	: field_((static_cast<int>(height) * 2 + 1) * (static_cast<int>(width) * 2 + 1)),
	  queue_((static_cast<int>(height) * 2 + 1) * (static_cast<int>(width) * 2 + 1)),
	  history_(),
	  height_(height),
	  width_(width),
	  decided_edges_(0),
	  decided_lines_(0),
	  inconsistent_(false),
	  fully_solved_(false),
	  abnormal_(false),
	  method_()
{
	for (Y y(0); y <= 2 * height_; ++y) {
		for (X x(0); x <= 2 * width_; ++x) {
			unsigned int id = Id(y, x);
			if (int(y % 2) != int(x % 2)) { // Edge
				field_[id].edge_status = kEdgeUndecided;
				if (y % 2 == 0) {
					field_[id].end_vertices[0] = Id(y, x - 1);
					field_[id].end_vertices[1] = Id(y, x + 1);
				} else {
					field_[id].end_vertices[0] = Id(y - 1, x);
					field_[id].end_vertices[1] = Id(y + 1, x);
				}
				field_[id].another_end_edge = id;
				field_[id].list_next_edge = id;
				field_[id].chain_size = 1;
			}
		}
	}

	queue_.Activate();
	Join(LoopPosition(Y(0), X(0)), Direction(Y(1), X(0)), Direction(Y(0), X(1)));
	Join(LoopPosition(2 * height, X(0)), Direction(Y(-1), X(0)), Direction(Y(0), X(1)));
	Join(LoopPosition(Y(0), 2 * width), Direction(Y(1), X(0)), Direction(Y(0), X(-1)));
	Join(LoopPosition(2 * height, 2 * width), Direction(Y(-1), X(0)), Direction(Y(0), X(-1)));
	while (!queue_.IsEmpty()) queue_.Pop();
	queue_.Deactivate();
}
template<class T>
GridLoop<T>::GridLoop(const GridLoop<T> &other)
	: field_(other.field_),
	  queue_((static_cast<int>(other.height()) * 2 + 1) * (static_cast<int>(other.width()) * 2 + 1)),
	  history_(other.history_),
	  height_(other.height_),
	  width_(other.width_),
	  decided_edges_(other.decided_edges_),
	  decided_lines_(other.decided_lines_),
	  inconsistent_(other.inconsistent_),
	  fully_solved_(other.fully_solved_),
	  abnormal_(other.abnormal_),
	  method_(other.method_)
{
}
template<class T>
GridLoop<T>::GridLoop(GridLoop<T> &&other)
	: field_(std::move(other.field_)),
	  queue_(std::move(other.queue_)),
	  history_(std::move(other.history_)),
	  height_(other.height_),
	  width_(other.width_),
	  decided_edges_(other.decided_edges_),
	  decided_lines_(other.decided_lines_),
	  inconsistent_(other.inconsistent_),
	  fully_solved_(other.fully_solved_),
	  abnormal_(other.abnormal_),
	  method_(other.method_)
{
}
template<class T>
GridLoop<T> &GridLoop<T>::operator=(const GridLoop<T> &other)
{
	height_ = other.height_;
	width_ = other.width_;
	decided_edges_ = other.decided_edges_;
	decided_lines_ = other.decided_lines_;
	inconsistent_ = other.inconsistent_;
	fully_solved_ = other.fully_solved_;
	abnormal_ = other.abnormal_;
	method_ = other.method_;

	field_ = other.field_;
	queue_ = other.queue_;
	history_ = other.history_;

	return *this;
}
template<class T>
GridLoop<T> &GridLoop<T>::operator=(GridLoop<T> &&other)
{
	height_ = other.height_;
	width_ = other.width_;
	decided_edges_ = other.decided_edges_;
	decided_lines_ = other.decided_lines_;
	inconsistent_ = other.inconsitent_;
	fully_solved_ = other.fully_solved_;
	abnormal_ = other.abnormal_;
	method_ = other.method_;

	field_ = std::move(other.field_);
	queue_ = std::move(other.queue_);
	history_ = std::move(other.history_);

	return *this;
}
template<class T>
GridLoop<T>::~GridLoop()
{
}
template<class T>
typename GridLoop<T>::EdgeState GridLoop<T>::GetEdge(LoopPosition edge) const
{
	return field_[Id(edge)].edge_status;
}
template<class T>
typename GridLoop<T>::EdgeState GridLoop<T>::GetEdgeSafe(LoopPosition edge) const
{
	if (IsPositionOnField(edge)) return GetEdge(edge);
	return kEdgeBlank;
}
template<class T>
void GridLoop<T>::DecideEdge(LoopPosition edge, EdgeState status)
{
	if (!IsPositionOnField(edge)) {
		if (status != kEdgeBlank) {
			SetInconsistent();
		}
		return;
	}

	unsigned int id = Id(edge);
	if (field_[id].edge_status == status) return;
	if (field_[id].edge_status != kEdgeUndecided) {
		SetInconsistent();
		return;
	}

	if (queue_.IsActive()) {
		DecideChain(id, status);
		CheckNeighborhoodOfChain(id);
	} else {
		queue_.Activate();

		DecideChain(id, status);
		CheckNeighborhoodOfChain(id);
		QueueProcessAll();

		queue_.Deactivate();
	}
}
template<class T>
LoopPosition GridLoop<T>::GetAnotherEnd(LoopPosition point, Direction dir) const
{
	unsigned int edge_id = Id(point + dir);
	return AsPosition(field_[edge_id].end_vertices[0] + field_[edge_id].end_vertices[1] - Id(point));
}
template<class T>
typename GridLoop<T>::EdgeCount GridLoop<T>::GetChainLength(LoopPosition point, Direction dir) const
{
	return field_[Id(point + dir)].chain_size;
}
template<class T>
std::pair<LoopPosition, LoopPosition> GridLoop<T>::GetEndsOfChain(LoopPosition edge) const
{
	unsigned int edge_id = Id(edge);
	while (!IsEndOfAChain(edge_id)) {
		edge_id = field_[edge_id].another_end_edge;
	}
	return{ AsPosition(field_[edge_id].end_vertices[0]), AsPosition(field_[edge_id].end_vertices[1]) };
}
template <class T>
unsigned int GridLoop<T>::GetChainIdentifier(LoopPosition edge) const
{
	unsigned int edge_id = Id(edge);
	while (!IsEndOfAChain(edge_id)) {
		edge_id = field_[edge_id].another_end_edge;
	}
	return std::min(edge_id, field_[edge_id].another_end_edge);
}
template <class T>
bool GridLoop<T>::IsRepresentativeOfChain(LoopPosition edge) const
{
	unsigned int edge_id = Id(edge);
	return IsEndOfAChain(edge_id) && edge_id <= field_[edge_id].another_end_edge;
}
template<class T>
bool GridLoop<T>::IsEndOfAChainVertex(unsigned int edge_id, unsigned int vertex_id) const {
	return field_[edge_id].end_vertices[0] == vertex_id || field_[edge_id].end_vertices[1] == vertex_id;
}
template<class T>
unsigned int GridLoop<T>::GetAnotherEndAsId(LoopPosition point, Direction dir) const {
	unsigned int edge_id = Id(point + dir);
	return field_[edge_id].end_vertices[0] + field_[edge_id].end_vertices[1] - Id(point);
}
template <class T>
void GridLoop<T>::Check(LoopPosition pos)
{
	if (!IsPositionOnField(pos)) return;

	if (queue_.IsActive()) {
		queue_.Push(Id(pos));
	} else {
		queue_.Activate();

		queue_.Push(Id(pos));
		QueueProcessAll();

		queue_.Deactivate();
	}
}
template <class T>
void GridLoop<T>::CheckAllVertex()
{
	QueuedRun([&]() {
		for (Y y(0); y <= 2 * height_; ++y) {
			for (X x(0); x <= 2 * width_; ++x) {
				Check(LoopPosition(y, x));
			}
		}
	});
}

template <class T>
void GridLoop<T>::CheckAllCell()
{
	QueuedRun([&]() {
		for (Y y(1); y < 2 * height_; ++y) {
			for (X x(1); x < 2 * width_; ++x) {
				Check(LoopPosition(y, x));
			}
		}
	});
}
template <class T>
void GridLoop<T>::CheckAllEdge()
{
	QueuedRun([&]() {
		for (Y y(0); y <= 2 * height_; ++y) {
			for (X x(0); x <= 2 * width_; ++x) {
				if (static_cast<int>(y % 2) != static_cast<int>(x % 2)) Check(LoopPosition(y, x));
			}
		}
	});
}
template <class T>
void GridLoop<T>::AddRestorePoint()
{
	history_.push_back({ -1, FieldComponent() });
}
template <class T>
void GridLoop<T>::Rollback()
{
	while (!history_.empty()) {
		auto last = history_.back();
		history_.pop_back();

		if (last.first == kHistoryRestorePoint) break;
		if (last.first == kHistorySetInconsistent) {
			inconsistent_ = false;
		} else if (last.first == kHistorySetSolved) {
			fully_solved_ = false;
		} else {
			if (field_[last.first].edge_status != kEdgeUndecided && last.second.edge_status == kEdgeUndecided) {
				--decided_edges_;
				if (field_[last.first].edge_status == kEdgeLine) --decided_lines_;
			}
			field_[last.first] = last.second;
		}
	}
}
template <class T>
void GridLoop<T>::DecideChain(unsigned int id, EdgeState status)
{
	unsigned int id_start = id;
	do {
		if (!history_.empty()) {
			history_.push_back({ id, field_[id] });
		}
		field_[id].edge_status = status;
		++decided_edges_;
		if (status == kEdgeLine) ++decided_lines_;
		id = field_[id].list_next_edge;
	} while (id != id_start);
}
template <class T>
void GridLoop<T>::CheckNeighborhoodOfChain(unsigned int id)
{
	unsigned int id_start = id;
	do {
		static_cast<T*>(this)->CheckNeighborhood(AsPosition(id));
		id = field_[id].list_next_edge;
	} while (id != id_start);
}
template <class T>
void GridLoop<T>::HasFullySolved()
{
	for (Y y(0); y <= 2 * height_; ++y) {
		for (X x(0); x <= 2 * width_; ++x) {
			if (int(y % 2) != int(x % 2) && GetEdge(LoopPosition(y, x)) == kEdgeUndecided) {
				DecideEdge(LoopPosition(y, x), kEdgeBlank);
			}
		}
	}
}
template <class T>
void GridLoop<T>::Join(LoopPosition vertex, Direction dir1, Direction dir2)
{
	unsigned int edge1_id = Id(vertex + dir1);
	unsigned int edge2_id = Id(vertex + dir2);

	if (field_[edge1_id].end_vertices[0] != Id(vertex) && field_[edge1_id].end_vertices[1] != Id(vertex)) return;
	if (field_[edge2_id].end_vertices[0] != Id(vertex) && field_[edge2_id].end_vertices[1] != Id(vertex)) return;
	if (!IsEndOfAChain(edge1_id) || !IsEndOfAChain(edge2_id)) return;
	if (field_[edge1_id].another_end_edge == edge2_id) return; // avoid joining the same chain again

	unsigned int end1_vertex = GetAnotherEndAsId(vertex, dir1);
	unsigned int end2_vertex = GetAnotherEndAsId(vertex, dir2);
	unsigned int end1_edge = field_[edge1_id].another_end_edge;
	unsigned int end2_edge = field_[edge2_id].another_end_edge;

	// change the status of edges if necessary
	if (field_[edge1_id].edge_status == kEdgeUndecided && field_[edge2_id].edge_status != kEdgeUndecided) {
		DecideChain(edge1_id, field_[edge2_id].edge_status);
		CheckNeighborhoodOfChain(edge1_id);
		Join(vertex, dir1, dir2); // assure that two edges are still disjoint (or end this function call)
		return;
	}
	if (field_[edge2_id].edge_status == kEdgeUndecided && field_[edge1_id].edge_status != kEdgeUndecided) {
		DecideChain(edge2_id, field_[edge1_id].edge_status);
		CheckNeighborhoodOfChain(edge2_id);
		Join(vertex, dir1, dir2);
		return;
	}

	if (end1_vertex == end2_vertex) {
		if (field_[edge1_id].edge_status == kEdgeUndecided) {
			if (decided_lines_ != 0 && method_.eliminate_closed_chain) {
				DecideChain(edge1_id, kEdgeBlank);
				DecideChain(edge2_id, kEdgeBlank);
				CheckNeighborhoodOfChain(edge1_id);
				CheckNeighborhoodOfChain(edge2_id);
				return;
			}
		} else if (field_[edge1_id].edge_status == kEdgeLine) {
			if (decided_lines_ != field_[edge1_id].chain_size + field_[edge2_id].chain_size) {
				SetInconsistent();
			} else {
				fully_solved_ = true;
				if (!history_.empty()) {
					history_.push_back({ kHistorySetSolved, FieldComponent() });
				}
				HasFullySolved();
			}
		}
	}

	if (!history_.empty()) {
		history_.push_back({ end1_edge, field_[end1_edge] });
		history_.push_back({ end2_edge, field_[end2_edge] });
	}

	// concatinate 2 lists
	std::swap(field_[end1_edge].list_next_edge, field_[end2_edge].list_next_edge);
	
	// update chain_size
	field_[end1_edge].chain_size = field_[end2_edge].chain_size =
		field_[edge1_id].chain_size + field_[edge2_id].chain_size;

	// update end_vertices
	field_[end1_edge].end_vertices[0] = end1_vertex;
	field_[end1_edge].end_vertices[1] = end2_vertex;
	field_[end2_edge].end_vertices[0] = end1_vertex;
	field_[end2_edge].end_vertices[1] = end2_vertex;

	// update another_end_edge
	field_[end1_edge].another_end_edge = end2_edge;
	field_[end2_edge].another_end_edge = end1_edge;

	Check(end1_vertex);
	Check(end2_vertex);
}
template <class T>
void GridLoop<T>::InspectVertex(LoopPosition vertex)
{
	static const Direction dirs[] = {
		Direction(Y(1), X(0)),
		Direction(Y(0), X(1)),
		Direction(Y(-1), X(0)),
		Direction(Y(0), X(-1))
	};

	MiniVector<int, 4> line_dir, undecided_dir;
	for (int i = 0; i < 4; ++i) {
		EdgeState status = GetEdgeSafe(vertex + dirs[i]);
		if (status == kEdgeLine) line_dir.push_back(i);
		if (status == kEdgeUndecided) undecided_dir.push_back(i);
	}

	if (line_dir.size() >= 3) {
		SetInconsistent();
		return;
	}

	if (line_dir.size() == 2) {
		int line_1 = -1, line_2 = -1;

		if (method_.avoid_three_lines) {
			for (int d : undecided_dir) {
				DecideEdge(vertex + dirs[d], kEdgeBlank);
			}
		}

		Join(vertex, dirs[line_dir[0]], dirs[line_dir[1]]);
		return;
	}

	if (line_dir.size() == 1) {
		int ld = line_dir[0];
		EdgeCount line_size = GetChainLength(vertex, dirs[ld]);
		LoopPosition line_another_end = GetAnotherEnd(vertex, dirs[ld]);

		int cand_dir = -1;
		for (int i : undecided_dir) {
			if (IsEndOfAChain(vertex + dirs[i]) && IsEndOfAChainVertex(Id(vertex + dirs[i]), Id(vertex))) {
				if (line_size == decided_lines_ || line_another_end != GetAnotherEnd(vertex, dirs[i]) || !method_.avoid_line_cycle) {
					LoopPosition pos2 = GetAnotherEnd(vertex, dirs[i]);
					if (cand_dir == -1) cand_dir = i;
					else cand_dir = -2;
				} else {
					DecideEdge(vertex + dirs[i], kEdgeBlank);
					return;
				}
			}
		}

		if (cand_dir == -1) {
			SetInconsistent();
		} else if (cand_dir != -2) {
			Join(vertex, dirs[ld], dirs[cand_dir]);
		}

		if (undecided_dir.size() == 2 && method_.hourglass_rule1) {
			// Hourglass rule
			LoopPosition line_end = GetAnotherEnd(vertex, dirs[line_dir[0]]);
			LoopPosition undecided_end[] = {
				GetAnotherEnd(vertex, dirs[undecided_dir[0]]),
				GetAnotherEnd(vertex, dirs[undecided_dir[1]])
			};
			EdgeCount line_size_total = line_size;

			int n_line_undecided_end0 = 0;
			bool is_triangle = false;
			for (Direction d : dirs) {
				if (GetEdgeSafe(undecided_end[0] + d) == kEdgeLine) {
					++n_line_undecided_end0;
					if (GetAnotherEnd(undecided_end[0], d) == undecided_end[1]) {
						is_triangle = true;
						line_size_total += GetChainLength(undecided_end[0], d);
					}
				}
			}

			if (!(n_line_undecided_end0 == 1 && is_triangle)) return;

			for (LoopPosition vertex2 : undecided_end) {
				for (Direction d : dirs) {
					if (GetEdgeSafe(vertex2 + d) == kEdgeUndecided && GetAnotherEnd(vertex2, d) == line_end && line_size_total < GetNumberOfDecidedLines()) {
						DecideEdge(vertex2 + d, kEdgeBlank);
					}
				}
			}
		}
		return;
	}

	if (line_dir.size() == 0) {
		if (undecided_dir.size() == 2) {
			Join(vertex, dirs[undecided_dir[0]], dirs[undecided_dir[1]]);
		} else if (undecided_dir.size() == 1) {
			DecideEdge(vertex + dirs[undecided_dir[0]], kEdgeBlank);
		}
	}
}

template <class T>
void GridLoop<T>::QueueProcessAll()
{
	while (!queue_.IsEmpty()) {
		int id = queue_.Pop();
		if (IsInconsistent()) continue;
		LoopPosition pos = AsPosition(id);
		static_cast<T*>(this)->Inspect(pos);
		if (IsVertex(pos)) InspectVertex(pos);
	}
}

template <class T>
void GridLoop<T>::CheckNeighborhood(LoopPosition edge)
{
	if (edge.y % 2 == 1) {
		Check(edge + Direction(Y(-1), X(0)));
		Check(edge + Direction(Y(1), X(0)));

		Check(edge + Direction(Y(0), X(-1)));
		Check(edge + Direction(Y(0), X(1)));
		Check(edge + Direction(Y(-2), X(-1)));
		Check(edge + Direction(Y(-2), X(1)));
		Check(edge + Direction(Y(2), X(-1)));
		Check(edge + Direction(Y(2), X(1)));
	} else {
		Check(edge + Direction(Y(0), X(-1)));
		Check(edge + Direction(Y(0), X(1)));

		Check(edge + Direction(Y(-1), X(0)));
		Check(edge + Direction(Y(1), X(0)));
		Check(edge + Direction(Y(-1), X(-2)));
		Check(edge + Direction(Y(1), X(-2)));
		Check(edge + Direction(Y(-1), X(2)));
		Check(edge + Direction(Y(1), X(2)));
	}
}

class PlainGridLoop : public GridLoop < PlainGridLoop >
{
public:
	PlainGridLoop() : GridLoop<PlainGridLoop>() {}
	PlainGridLoop(Y height, X width) : GridLoop<PlainGridLoop>(height, width) {}
	PlainGridLoop(const PlainGridLoop &other) : GridLoop<PlainGridLoop>(other) {}
	PlainGridLoop(PlainGridLoop &&other) : GridLoop<PlainGridLoop>(other) {}
};
}
