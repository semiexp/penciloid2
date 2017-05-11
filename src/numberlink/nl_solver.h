#include "nl_problem.h"
#include "nl_field.h"

#include <vector>
#include <cstring>

namespace penciloid
{
namespace numberlink
{
class Solver
{
public:
	typedef char CellState;
	typedef CellState* Frontier;

	Solver();
	Solver(const Problem &problem);
	Solver(const Solver &) = delete;
	Solver(Solver &&) = delete;

	~Solver();

	Solver &operator=(const Solver &) = delete;
	Solver &operator=(Solver &&) = delete;

	inline Y height() const { return height_; }
	inline X width() const { return width_; }

	void SolveBySearch();
	void SolveBySearch2();

private:
	void CopyFrontier(Frontier src, Frontier dest) {
		memcpy(dest, src, sizeof(CellState) * frontier_size_);
	}
	bool Join(Frontier f, int i, int j);

	void Search(Y y, X x, Frontier frontier);
	void Search2(Y y, X x, Field &field);

	static const int kPoolSize = 1048576;

	Y height_;
	X width_;
	Problem problem_;

	Grid<int> answer_, contiguous_line_up_, contiguous_line_left_, contiguous_empty_up_, contiguous_empty_left_;
	int frontier_size_;
};
}
}
