#pragma once

#include "strict_typedef.h"

namespace penciloid
{
// Type for y-coordinates
STRICT_TYPEDEF(int, Y);

// Type for x-coordinates
STRICT_TYPEDEF(int, X);

// Type for 2D absolute coordinates
template <class T>
struct position_base
{
	position_base(Y y, X x) : y(y), x(x) {}
	position_base() : y(0), x(0) {}

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

template <class T>
inline bool operator==(const position_base<T> &lhs, const position_base<T> &rhs)
{
	return lhs.y == rhs.y && lhs.x == rhs.x;
}
template <class T>
inline bool operator!=(const position_base<T> &lhs, const position_base<T> &rhs)
{
	return lhs.y != rhs.y || lhs.x != rhs.x;
}
template <class T>
inline position_base<T> operator+(const position_base<T> &lhs, const Direction &rhs)
{
	return position_base<T>(lhs.y + rhs.y, lhs.x + rhs.x);
}
template <class T>
inline position_base<T> operator-(const position_base<T> &lhs, const Direction &rhs)
{
	return position_base<T>(lhs.y - rhs.y, lhs.x - rhs.x);
}
template <class T>
inline Direction operator-(const position_base<T> &lhs, const position_base<T> &rhs)
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
inline Direction operator*(const Direction &lhs, int rhs)
{
	return Direction(lhs.y * rhs, lhs.x * rhs);
}
inline Direction operator*(int lhs, const Direction &rhs)
{
	return rhs * lhs;
}
struct position_Position_t {};
typedef position_base<position_Position_t> LoopPosition;
struct position_CellPosition_t {};
typedef position_base<position_CellPosition_t> CellPosition;

}
