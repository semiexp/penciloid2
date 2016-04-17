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
std::string StringOfProblem(Problem &problem)
{
	auto int_to_char = [](int i) {
		if (0 <= i && i < 10) return (char)(i + '0');
		else if (10 <= i && i < 36) return (char)((i - 10) + 'a');
		else if (36 <= i && i < 62) return (char)((i - 36) + 'A');
		return ' ';
	};
	std::string ret;
	ret.push_back(int_to_char(problem.height()));
	ret.push_back(int_to_char(problem.width()));

	int val = 0, cnt = 0;
	for (Y y(0); y < problem.height(); ++y) {
		for (X x(0); x < problem.width(); ++x) {
			if (cnt == 0) {
				val = 1 + static_cast<int>(problem.GetClue(CellPosition(y, x)));
				cnt = 1;
			} else if (cnt == 1) {
				ret.push_back(int_to_char(val + 5 * (1 + static_cast<int>(problem.GetClue(CellPosition(y, x))))));
				val = 0;
				cnt = 0;
			}
		}
	}
	if (cnt == 1) ret.push_back(int_to_char(val));
	return ret;
}
Problem ProblemOfString(std::string &str)
{
	auto char_to_int = [](char c) {
		if ('0' <= c && c <= '9') return (int)(c - '0');
		if ('a' <= c && c <= 'z') return (int)(c - 'a') + 10;
		if ('A' <= c && c <= 'Z') return (int)(c - 'A') + 36;;
		return -1;
	};
	Y height(char_to_int(str[0]));
	X width(char_to_int(str[1]));
	Problem ret(height, width);
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			int pos = static_cast<int>(y) * static_cast<int>(width) + static_cast<int>(x);
			if (pos % 2 == 0) ret.SetClue(CellPosition(y, x), Clue(char_to_int(str[pos / 2 + 2]) % 5 - 1));
			else ret.SetClue(CellPosition(y, x), Clue(char_to_int(str[pos / 2 + 2]) / 5 - 1));
		}
	}
	return ret;
}
}
}