#include "yj_problem.h"

#include <utility>

namespace penciloid
{
namespace yajilin
{
Problem::Problem() : cells_()
{
}
Problem::Problem(Y height, X width) : cells_(height, width, Clue(kNoClue, 0))
{
}
Problem::Problem(Y height, X width, const char *clues[]) : cells_(height, width, Clue(kNoClue, 0))
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			ClueDirection dir = kNoClue;
			switch (clues[y][x * 2]) {
			case '^':
				dir = kClueNorth; break;
			case '<':
				dir = kClueWest; break;
			case '>':
				dir = kClueEast; break;
			case 'v':
				dir = kClueSouth; break;
			}
			if (dir != kNoClue) {
				cells_(CellPosition(y, x)) = Clue(dir, static_cast<int>(clues[y][x * 2 + 1] - '0'));
			}
		}
	}
}
Problem::Problem(const Problem &other) : cells_(other.cells_)
{
}
Problem::Problem(Problem &&other) : cells_(std::move(other.cells_))
{
}
Problem::~Problem()
{
}
}
}
