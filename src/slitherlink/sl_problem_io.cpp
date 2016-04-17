#include "sl_problem_io.h"

#include <string>

namespace penciloid
{
namespace slitherlink
{
bool InputProblem(std::istream &str, Problem &problem)
{
	int h = -1, w = -1;
	str >> h >> w;
	if (h == -1) return false;

	problem = Problem(Y(h), X(w));
	for (int i = 0; i < h; ++i) {
		std::string line;
		str >> line;
		for (int j = 0; j < w; ++j) {
			if ('0' <= line[j] && line[j] <= '3') {
				problem.SetClue(CellPosition(Y(i), X(j)), Clue(line[j] - '0'));
			}
		}
	}
	return true;
}
}
}