#pragma once

#include "strict_typedef.h"

namespace penciloid
{
// Type for y-coordinates
STRICT_TYPEDEF(int, Y);

// Type for x-coordinates
STRICT_TYPEDEF(int, X);

// Type for 2D absolute coordinates
struct Position
{
	Position(Y y, X x) : y(y), x(x) {}
	Position() : y(0), x(0) {}

	Y y;
	X x;
};

// Type for 2D absolute coordinates for cells on grids
struct CellPosition
{
	CellPosition(Y y, X x) : y(y), x(x) {}
	CellPosition() : y(0), x(0) {}

	Y y;
	X x;
};

// Type for 2D relative coordinates
struct Direction
{
	Direction(Y y, X x) : y(y), x(x) {}
	Direction() : y(0), x(0) {}

	Y y;
	X x;
};

inline bool operator==(const Position &lhs, const Position &rhs)
{
	return lhs.y == rhs.y && lhs.x == rhs.x;
}
inline bool operator!=(const Position &lhs, const Position &rhs)
{
	return lhs.y != rhs.y || lhs.x != rhs.x;
}
inline Position operator+(const Position &lhs, const Direction &rhs)
{
	return Position(lhs.y + rhs.y, lhs.x + rhs.x);
}
inline Position operator-(const Position &lhs, const Direction &rhs)
{
	return Position(lhs.y - rhs.y, lhs.x - rhs.x);
}
inline Direction operator-(const Position &lhs, const Position &rhs)
{
	return Direction(lhs.y - rhs.y, lhs.x - rhs.x);
}

inline bool operator==(const CellPosition &lhs, const CellPosition &rhs)
{
	return lhs.y == rhs.y && lhs.x == rhs.x;
}
inline bool operator!=(const CellPosition &lhs, const CellPosition &rhs)
{
	return lhs.y != rhs.y || lhs.x != rhs.x;
}
inline CellPosition operator+(const CellPosition &lhs, const Direction &rhs)
{
	return CellPosition(lhs.y + rhs.y, lhs.x + rhs.x);
}
inline CellPosition operator-(const CellPosition &lhs, const Direction &rhs)
{
	return CellPosition(lhs.y - rhs.y, lhs.x - rhs.x);
}
inline Direction operator-(const CellPosition &lhs, const CellPosition &rhs)
{
	return Direction(lhs.y - rhs.y, lhs.x - rhs.x);
}

inline Direction operator+(const Direction &lhs, const Direction &rhs)
{
	return Direction(lhs.y + rhs.y, lhs.x + rhs.x);
}
inline Direction operator-(const Direction &lhs, const Direction &rhs)
{
	return Direction(lhs.y - rhs.y, lhs.x - rhs.x);
}
}
