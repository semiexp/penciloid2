namespace penciloid
{
// Type for y-coordinates
typedef int Y;

// Type for x-coordinates
typedef int X;

// Type for 2D absolute coordinates
struct Location
{
	Location(Y y, X x) : y(y), x(x) {}
	Location() : y(0), x(0) {}

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

inline Location operator+(const Location &lhs, const Direction &rhs)
{
	return Location(lhs.y + rhs.y, lhs.x + rhs.x);
}
inline Location operator-(const Location &lhs, const Direction &rhs)
{
	return Location(lhs.y - rhs.y, lhs.x - rhs.x);
}
inline Direction operator-(const Location &lhs, const Location &rhs)
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
