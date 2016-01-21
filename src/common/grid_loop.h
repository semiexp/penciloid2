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

	Y height() const;
	X width() const;

	// Returns the number of edges whose status is EDGE_LINE or EDGE_BLANK.
	EdgeCount GetNumberOfDecidedEdges() const;

	// Returns the number of edges whose status is EDGE_LINE.
	EdgeCount GetNumberOfDecidedLines() const;

	bool IsInconsistent() const;
	// Note that both of IsInconsistent() and IsFullySolved() may hold.
	bool IsFullySolved() const;
	bool IsInAbnormalCondition() const;
	void SetInconsistent();

	// Returns the status of edge <edge>.
	// <edge> should be a legitimate location.
	EdgeState GetEdge(Location edge) const;

	// Returns the status of edge <edge>.
	// If <edge> is out of range, EDGE_BLANK will be returned.
	EdgeState GetEdgeSafe(Location edge) const;

	// Makes the status of edge <edge> <state>.
	// Status of other decidable edges will be also changed.
	void DecideEdge(Location edge, EdgeState state);

	// Returns the another end of the chain, which starts from <point> to the direction <dir>.
	// <point> should be one of the ends of the chain.
	Location GetAnotherEnd(Location point, Direction dir) const;

	// Returns the length of the chain, which starts from <point> to the direction <dir>.
	// <point> should be one of the ends of the chain.
	EdgeCount GetChainLength(Location point, Direction dir) const;

	// Returns two end vertices of the chain which edge <edge> belongs to.
	// Non-const version changes internal status to perform "path compression".
	std::pair<Location, Location> GetEndsOfChain(Location edge);
	std::pair<Location, Location> GetEndsOfChain(Location edge) const;

	// Check around location <pos>.
	// This method internally invokes Inspect(pos).
	void Check(Location pos);

	// Perform Check(pos) for all possible (vertex / cell / edge).
	void CheckAllVertex();
	void CheckAllCell();
	void CheckAllEdge();

	//
	// Public methods below are intended to be "overridden" by the subclass.
	//

	// This method (of the subclass) will be called when the status of edge <edge> was decided.
	void HasDecided(Location edge) {}

	// This method (of the subclass) should call Check(Location) for constraint propagation.
	// This will be called "something has happened" to edge <edge>, e.g. the status of <edge> was decided.
	void CheckNeighborhood(Location edge);

	// This method (of the subclass) should inspect around location <pos>.
	// <pos> does not necessarily represent the location of an edge.
	// Don't call this method directly (instead, use Check(pos) ).
	void Inspect(Location pos) {}

private:

};
}
