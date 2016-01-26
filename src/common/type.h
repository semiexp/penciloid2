namespace penciloid
{
// Type for y-coordinates
typedef int Y;

// Type for x-coordinates
typedef int X;

// Type for 2D absolute coordinates
struct Position
{
	Position(Y y, X x) : y(y), x(x) {}
	Position() : y(0), x(0) {}

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
inline Direction operator+(const Direction &lhs, const Direction &rhs)
{
	return Direction(lhs.y + rhs.y, lhs.x + rhs.x);
}
inline Direction operator-(const Direction &lhs, const Direction &rhs)
{
	return Direction(lhs.y - rhs.y, lhs.x - rhs.x);
}
}
