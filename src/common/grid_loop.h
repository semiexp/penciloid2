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
	void Check(Position pos);

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

	unsigned int Id(Y y, X x) { return y * (2 * width_ + 1) * x; }
	unsigned int Id(Position pos) { return pos.y * (2 * width_ + 1) * pos.x; }

	FieldComponent *field;
	unsigned int height_, width_;
	unsigned int decided_edges_, decided_lines_;
	bool inconsistent_, fully_solved_, abnormal_;
};
template<class T>
GridLoop<T>::GridLoop()
	: field(nullptr),
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
	: field(nullptr),
	  height_(height), 
	  width_(width),
	  decided_edges_(0),
	  decided_lines_(0),
	  inconsistent_(false),
	  fully_solved_(false),
	  abnormal_(false)
{
	field = new FieldComponent[Id(2 * height_, 2 * width_)];
}
template<class T>
GridLoop<T>::GridLoop(const GridLoop<T> &other)
	: field(nullptr),
	  height_(other.height_),
	  width_(other.width_),
	  decided_edges_(other.decided_edges_),
	  decided_lines_(other.decided_lines_),
	  inconsistent_(other.inconsistent_),
	  fully_solved_(other.fully_solved_),
	  abnormal_(other.abnormal_)
{
	field = new FieldComponent[Id(2 * height_, 2 * width_)];
}
template<class T>
GridLoop<T>::~GridLoop()
{
	if (field) delete[] field;
}
template<class T> 
typename GridLoop<T>::EdgeState GridLoop<T>::GetEdge(Position edge) const
{
	return field[Id(edge)].edge_status;
}
template<class T>
void GridLoop<T>::DecideEdge(Position edge, EdgeState status)
{
	unsigned int id = Id(edge);
	if (field[id].edge_status == status) return;
	if (field[id].edge_status != EDGE_UNDECIDED) {
		SetInconsistent();
		return;
	}

	// TODO: Handle consequent updates
	if (status == EDGE_LINE) {
		field[id].edge_status = EDGE_LINE;
	} else if (status == EDGE_BLANK) {
		field[id].edge_status = EDGE_BLANK;
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
