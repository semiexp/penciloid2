#include <algorithm>

#include "type.h"

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
		EDGE_UNDECIDED,
		EDGE_LINE,
		EDGE_BLANK
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

	// Returns the number of edges whose status is EDGE_LINE or EDGE_BLANK.
	EdgeCount GetNumberOfDecidedEdges() const { return decided_edges_; }

	// Returns the number of edges whose status is EDGE_LINE.
	EdgeCount GetNumberOfDecidedLines() const { return decided_lines_; }

	// Note that both of IsInconsistent() and IsFullySolved() may hold.
	bool IsInconsistent() const { return inconsistent_; }
	bool IsFullySolved() const { return fully_solved_; }
	bool IsInAbnormalCondition() const { return abnormal_; }
	void SetInconsistent() { inconsistent_ = true; }

	// Returns the status of edge <edge>.
	// <edge> should be a legitimate position.
	EdgeState GetEdge(Position edge) const;

	// Returns the status of edge <edge>.
	// If <edge> is out of range, EDGE_BLANK will be returned.
	EdgeState GetEdgeSafe(Position edge) const;

	// Makes the status of edge <edge> <state>.
	// Status of other decidable edges will be also changed.
	void DecideEdge(Position edge, EdgeState status);

	// Returns the another end of the chain, which starts from <point> to the direction <dir>.
	// <point> should be one of the ends of the chain.
	Position GetAnotherEnd(Position point, Direction dir) const;

	// Returns the length of the chain, which starts from <point> to the direction <dir>.
	// <point> should be one of the ends of the chain.
	EdgeCount GetChainLength(Position point, Direction dir) const;

	// Returns two end vertices of the chain which edge <edge> belongs to.
	// Non-const version changes internal status to perform "path compression".
	std::pair<Position, Position> GetEndsOfChain(Position edge);
	std::pair<Position, Position> GetEndsOfChain(Position edge) const;

	// Check around position <pos>.
	// This method internally invokes Inspect(pos).
	void Check(Position pos) { Check(Id(pos)); }

	// Perform Check(pos) for all possible (vertex / cell / edge).
	void CheckAllVertex();
	void CheckAllCell();
	void CheckAllEdge();

	//
	// Public methods below are intended to be "overridden" by the subclass.
	//

	// This method (of the subclass) will be called when the status of edge <edge> was decided.
	void HasDecided(Position edge) {}

	// This method (of the subclass) should call Check(Position) for constraint propagation.
	// This will be called "something has happened" to edge <edge>, e.g. the status of <edge> was decided.
	void CheckNeighborhood(Position edge);

	// This method (of the subclass) should inspect around position <pos>.
	// <pos> does not necessarily represent the position of an edge.
	// Don't call this method directly (instead, use Check(pos) ).
	void Inspect(Position pos) {}

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

	bool IsVertex(Position pos) const { return pos.y % 2 == 0 && pos.x % 2 == 0; }
	bool IsEdge(Position pos) const { return pos.y % 2 != pos.x % 2; }
	bool IsPositionOnField(Position pos) const { return 0 <= pos.y && pos.y <= 2 * height_ && 0 <= pos.x && pos.x <= 2 * width_; }

	unsigned int Id(Y y, X x) const { return y * (2 * width_ + 1) + x; }
	unsigned int Id(Position pos) const { return pos.y * (2 * width_ + 1) + pos.x; }
	Position AsPosition(unsigned int id) const { return Position(id / (2 * width_ + 1), id % (2 * width_ + 1)); }

	bool IsEndOfAChain(Position edge) const { return IsEndOfAChain(Id(edge)); }
	bool IsEndOfAChain(unsigned int edge_id) const { return field_[field_[edge_id].another_end_edge].another_end_edge == edge_id; }
	bool IsEndOfAChainVertex(unsigned int edge_id, unsigned int vertex_id) const;
	unsigned int GetAnotherEndAsId(Position point, Direction dir) const;

	void Check(unsigned int id);
	void DecideChain(unsigned int id, EdgeState status);
	void CheckNeighborhoodOfChain(unsigned int id);
	// Merge the edge at (vertex + dir1) and at (vertex + dir2)
	void Join(Position vertex, Direction dir1, Direction dir2);
	void InspectVertex(Position vertex);

	FieldComponent *field_;
	Y height_;
	X width_;
	EdgeCount decided_edges_, decided_lines_;
	bool inconsistent_, fully_solved_, abnormal_;
};
template<class T>
GridLoop<T>::GridLoop()
	: field_(nullptr),
	  height_(0),
	  width_(0),
	  decided_edges_(0),
	  decided_lines_(0),
	  inconsistent_(false),
	  fully_solved_(false),
	  abnormal_(false)
{
}
template<class T>
GridLoop<T>::GridLoop(Y height, X width)
	: field_(nullptr),
	  height_(height), 
	  width_(width),
	  decided_edges_(0),
	  decided_lines_(0),
	  inconsistent_(false),
	  fully_solved_(false),
	  abnormal_(false)
{
	field_ = new FieldComponent[Id(2 * height_, 2 * width_) + 1];

	for (Y y = 0; y <= 2 * height_; ++y) {
		for (X x = 0; x <= 2 * width_; ++x) {
			unsigned int id = Id(y, x);
			if (y % 2 != x % 2) { // Edge
				field_[id].edge_status = EDGE_UNDECIDED;
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

	Join(Position(0, 0), Direction(1, 0), Direction(0, 1));
	Join(Position(2 * height, 0), Direction(-1, 0), Direction(0, 1));
	Join(Position(0, 2 * width), Direction(1, 0), Direction(0, -1));
	Join(Position(2 * height, 2 * width), Direction(-1, 0), Direction(0, -1));
}
template<class T>
GridLoop<T>::GridLoop(const GridLoop<T> &other)
	: field_(nullptr),
	  height_(other.height_),
	  width_(other.width_),
	  decided_edges_(other.decided_edges_),
	  decided_lines_(other.decided_lines_),
	  inconsistent_(other.inconsistent_),
	  fully_solved_(other.fully_solved_),
	  abnormal_(other.abnormal_)
{
	field_ = new FieldComponent[Id(2 * height_, 2 * width_) + 1];
}
template<class T>
GridLoop<T>::~GridLoop()
{
	if (field_) delete[] field_;
}
template<class T> 
typename GridLoop<T>::EdgeState GridLoop<T>::GetEdge(Position edge) const
{
	return field_[Id(edge)].edge_status;
}
template<class T>
typename GridLoop<T>::EdgeState GridLoop<T>::GetEdgeSafe(Position edge) const
{
	if (IsPositionOnField(edge)) return GetEdge(edge);
	return EDGE_BLANK;
}
template<class T>
void GridLoop<T>::DecideEdge(Position edge, EdgeState status)
{
	unsigned int id = Id(edge);
	if (field_[id].edge_status == status) return;
	if (field_[id].edge_status != EDGE_UNDECIDED) {
		SetInconsistent();
		return;
	}

	// TODO: Handle consequent updates
	if (status == EDGE_LINE) {
		DecideChain(id, EDGE_LINE);
		CheckNeighborhoodOfChain(id);
	} else if (status == EDGE_BLANK) {
		DecideChain(id, EDGE_BLANK);
		CheckNeighborhoodOfChain(id);
	}
}
template<class T>
Position GridLoop<T>::GetAnotherEnd(Position point, Direction dir) const
{
	unsigned int edge_id = Id(point + dir);
	return AsPosition(field_[edge_id].end_vertices[0] + field_[edge_id].end_vertices[1] - Id(point));
}
template<class T>
typename GridLoop<T>::EdgeCount GridLoop<T>::GetChainLength(Position point, Direction dir) const
{
	return field_[Id(point + dir)].chain_size;
}

template<class T>
bool GridLoop<T>::IsEndOfAChainVertex(unsigned int edge_id, unsigned int vertex_id) const {
	return field_[edge_id].end_vertices[0] == vertex_id || field_[edge_id].end_vertices[1] == vertex_id;
}
template<class T>
unsigned int GridLoop<T>::GetAnotherEndAsId(Position point, Direction dir) const {
	unsigned int edge_id = Id(point + dir);
	return field_[edge_id].end_vertices[0] + field_[edge_id].end_vertices[1] - Id(point);
}
template <class T>
void GridLoop<T>::Check(unsigned int id)
{
	// TODO: implement queue
	Position pos = AsPosition(id);
	static_cast<T*>(this)->Inspect(pos);
	if (IsVertex(pos)) InspectVertex(pos);
}
template <class T>
void GridLoop<T>::DecideChain(unsigned int id, EdgeState status)
{
	unsigned int id_start = id;
	do {
		field_[id].edge_status = status;
		++decided_edges_;
		if (status == EDGE_LINE) ++decided_lines_;
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
void GridLoop<T>::Join(Position vertex, Direction dir1, Direction dir2)
{
	unsigned int edge1_id = Id(vertex + dir1);
	unsigned int edge2_id = Id(vertex + dir2);

	if (field_[edge1_id].end_vertices[0] != Id(vertex) && field_[edge1_id].end_vertices[1] != Id(vertex)) return;
	if (field_[edge2_id].end_vertices[0] != Id(vertex) && field_[edge2_id].end_vertices[1] != Id(vertex)) return;
	if (!IsEndOfAChain(edge1_id) || !IsEndOfAChain(edge2_id)) return;

	unsigned int end1_vertex = GetAnotherEndAsId(vertex, dir1);
	unsigned int end2_vertex = GetAnotherEndAsId(vertex, dir2);
	unsigned int end1_edge = field_[edge1_id].another_end_edge;
	unsigned int end2_edge = field_[edge2_id].another_end_edge;

	if (end1_vertex == end2_vertex) {
		// TODO
	}

	// change the status of edges if necessary
	if (field_[edge1_id].edge_status == EDGE_UNDECIDED && field_[edge2_id].edge_status != EDGE_UNDECIDED) {
		DecideChain(edge1_id, field_[edge2_id].edge_status);
		CheckNeighborhoodOfChain(edge1_id);
	}
	if (field_[edge2_id].edge_status == EDGE_UNDECIDED && field_[edge1_id].edge_status != EDGE_UNDECIDED) {
		DecideChain(edge2_id, field_[edge1_id].edge_status);
		CheckNeighborhoodOfChain(edge2_id);
	}

	// concatinate 2 lists
	std::swap(field_[edge1_id].list_next_edge, field_[edge2_id].list_next_edge);

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
void GridLoop<T>::InspectVertex(Position vertex)
{
	static const Direction dirs[] = {
		Direction(1, 0),
		Direction(0, 1),
		Direction(-1, 0),
		Direction(0, -1)
	};

	unsigned int n_line = 0, n_undecided = 0;
	for (int i = 0; i < 4; ++i) {
		EdgeState status = GetEdgeSafe(vertex + dirs[i]);
		if (status == EDGE_LINE) ++n_line;
		if (status == EDGE_UNDECIDED) ++n_undecided;
	}

	if (n_line >= 3) {
		SetInconsistent();
		return;
	}

	if (n_line == 2) {
		int line_1 = -1, line_2 = -1;

		for (int i = 0; i < 4; ++i) {
			if (GetEdgeSafe(vertex + dirs[i]) == EDGE_UNDECIDED) {
				DecideEdge(vertex + dirs[i], EDGE_BLANK);
			}

			if (GetEdgeSafe(vertex + dirs[i]) == EDGE_LINE) {
				if (line_1 == -1) line_1 = i;
				else line_2 = i;
			}
		}

		Join(vertex, dirs[line_1], dirs[line_2]);
		return;
	}

	if (n_line == 1) {
		EdgeCount line_size = 0;
		Position line_another_end(-1, -1);
		int line_dir = 0;
		for (int i = 0; i < 4; ++i) {
			if (GetEdgeSafe(vertex + dirs[i]) == EDGE_LINE) {
				line_size = GetChainLength(vertex, dirs[i]);
				line_another_end = GetAnotherEnd(vertex, dirs[i]);
				line_dir = i;
			}
		}

		int cand_dir = -1;
		for (int i = 0; i < 4; ++i) {
			if (GetEdgeSafe(vertex + dirs[i]) == EDGE_UNDECIDED && IsEndOfAChain(vertex + dirs[i]) && IsEndOfAChainVertex(Id(vertex + dirs[i]), Id(vertex))) {
				if (line_size == decided_lines_ || line_another_end != GetAnotherEnd(vertex, dirs[i])) {
					Position pos2 = GetAnotherEnd(vertex, dirs[i]);
					if (cand_dir == -1) cand_dir = i;
					else cand_dir = -2;
				} else {
					DecideEdge(vertex + dirs[i], EDGE_BLANK);
				}
			}
		}

		if (cand_dir == -1) {
			SetInconsistent();
		} else if (cand_dir != -2) {
			Join(vertex, dirs[line_dir], dirs[cand_dir]);
		}

		return;
	}

	if (n_line == 0) {
		if (n_undecided == 2) {
			int undecided_1 = -1, undecided_2 = -1;

			for (int i = 0; i < 4; ++i) {
				if (GetEdgeSafe(vertex + dirs[i]) == EDGE_UNDECIDED) {
					if (undecided_1 == -1) undecided_1 = i;
					else undecided_2 = i;
				}
			}

			Join(vertex, dirs[undecided_1], dirs[undecided_2]);
		} else if (n_undecided == 1) {
			int undecided_1 = -1;

			for (int i = 0; i < 4; ++i) {
				if (GetEdgeSafe(vertex + dirs[i]) == EDGE_UNDECIDED) {
					undecided_1 = i;
				}
			}

			DecideEdge(vertex + dirs[undecided_1], EDGE_BLANK);
		}
	}
}

template <class T>
void GridLoop<T>::CheckNeighborhood(Position edge)
{
	if (edge.y % 2 == 1) {
		Check(edge + Direction(-1, 0));
		Check(edge + Direction(1, 0));

		Check(edge + Direction(0, -1));
		Check(edge + Direction(0, 1));
		Check(edge + Direction(-2, -1));
		Check(edge + Direction(-2, 1));
		Check(edge + Direction(2, -1));
		Check(edge + Direction(2, 1));
	} else {
		Check(edge + Direction(0, -1));
		Check(edge + Direction(0, 1));

		Check(edge + Direction(-1, 0));
		Check(edge + Direction(1, 0));
		Check(edge + Direction(-1, -2));
		Check(edge + Direction(1, -2));
		Check(edge + Direction(-1, 2));
		Check(edge + Direction(1, 2));
	}
}

class PlainGridLoop : public GridLoop < PlainGridLoop >
{
public:
	PlainGridLoop() : GridLoop<PlainGridLoop>() {}
	PlainGridLoop(Y height, X width) : GridLoop<PlainGridLoop>(height, width) {}
	PlainGridLoop(const GridLoop &other) : GridLoop<PlainGridLoop>(other) {}
	PlainGridLoop(PlainGridLoop &&other) : GridLoop<PlainGridLoop>(other) {}
};
}
