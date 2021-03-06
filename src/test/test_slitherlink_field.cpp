#include "test_slitherlink_field.h"
#include "test.h"

#include <cassert>
#include <vector>

#include "../slitherlink/sl_field.h"
#include "../slitherlink/sl_dictionary.h"

namespace
{
// Place clues and check edges
void DoAddClueTest(penciloid::Y height, penciloid::X width, std::vector<const char*> test_target, penciloid::slitherlink::Dictionary *db, bool inconsistent = false, bool fully_solved = false)
{
	using namespace penciloid;
	using namespace penciloid::slitherlink;

	Field field(height, width);
	field.SetDatabase(db);

	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if ('0' <= test_target[y * 2 + 1][x * 2 + 1] && test_target[y * 2 + 1][x * 2 + 1] <= '3') {
				field.AddClue(CellPosition(y, x), Clue(test_target[y * 2 + 1][x * 2 + 1] - '0'));
			}
		}
	}
	
	field.CheckAllCell();

	for (Y y(0); y <= 2 * height; ++y) {
		for (X x(0); x <= 2 * width; ++x) {
			if (int(y % 2) != int(x % 2)) {
				Field::EdgeState expected;
				if (test_target[y][x] == 'x') expected = Field::kEdgeBlank;
				else if (test_target[y][x] == ' ') expected = Field::kEdgeUndecided;
				else expected = Field::kEdgeLine;
				assert(field.GetEdge(LoopPosition(y, x)) == expected);
			}
		}
	}

	assert(field.IsInconsistent() == inconsistent);
	assert(field.IsFullySolved() == fully_solved);
}
void DoProblemTest(penciloid::Y height, penciloid::X width, std::vector<const char*> test_problem, penciloid::slitherlink::Dictionary *db)
{
	using namespace penciloid;
	using namespace penciloid::slitherlink;

	Field field(height, width);
	field.SetDatabase(db);

	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if ('0' <= test_problem[y][x] && test_problem[y][x] <= '3') {
				field.AddClue(CellPosition(y, x), Clue(test_problem[y][x] - '0'));
			}
		}
	}

	assert(field.IsInconsistent() == false);
	assert(field.IsFullySolved() == true);
}
}

namespace penciloid
{
namespace test
{
void RunAllSlitherlinkFieldTest()
{
	using namespace slitherlink;

	Dictionary db;
	db.CreateDefault();

	SlitherlinkFieldAddClue(db);
	SlitherlinkFieldTheorem(db);
	SlitherlinkFieldFullySolvableProblem(db);
	SlitherlinkFieldSolveProblem(db);
	SlitherlinkFieldDiagonalChain(db);
}
void SlitherlinkFieldAddClue(penciloid::slitherlink::Dictionary &db)
{
	using namespace slitherlink;

	DoAddClueTest(Y(3), X(3), {
		"+ + + +",
		"       ",
		"+ +x+ +",
		"  x0x  ",
		"+ +x+ +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(Y(3), X(3), {
		"+x+ + +",
		"x1     ",
		"+ + + +",
		"       ",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(Y(3), X(3), {
		"+ +-+ +",
		" 2     ",
		"+ + + +",
		"|      ",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(Y(3), X(3), {
		"+-+ + +",
		"|3     ",
		"+ + + +",
		"       ",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(Y(3), X(3), {
		"+x+x+-+",
		"x0x2| |",
		"+x+-+x+",
		"x | x |",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(Y(3), X(3), {
		"+-+-+-+",
		"|2x1x |",
		"+x+x+ +",
		"| x    ",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(Y(3), X(3), {
		"+-+-+-+",
		"|2x1x |",
		"+x+x+-+",
		"| x2| x",
		"+ +-+x+",
		"    x x",
		"+ +x+x+",
	}, &db);
}
void SlitherlinkFieldTheorem(penciloid::slitherlink::Dictionary &db)
{
	using namespace slitherlink;

	DoAddClueTest(Y(3), X(3), {
		"+ + + +",
		"  x    ",
		"+ + + +",
		"|3|3|  ",
		"+ + + +",
		"  x    ",
		"+ + + +",
	}, &db);
	DoAddClueTest(Y(3), X(4), {
		"+ + +-+x+",
		"     3| x",
		"+ + + + +",
		"  |3     ",
		"+x+-+ + +",
		"  x      ",
		"+ + + + +",
	}, &db);
	DoAddClueTest(Y(3), X(3), {
		"+-+-+-+",
		"| x x |",
		"+x+-+x+",
		"|3|3| |",
		"+-+x+ +",
		"x1x    ",
		"+x+x+ +",
	}, &db);
}
void SlitherlinkFieldFullySolvableProblem(penciloid::slitherlink::Dictionary &db)
{
	using namespace slitherlink;

	DoAddClueTest(Y(3), X(3), {
		"+-+-+-+",
		"| x x |",
		"+x+-+x+",
		"|3|3| |",
		"+-+x+-+",
		"x1x x1x",
		"+x+x+x+",
	}, &db, false, true);
	DoAddClueTest(Y(3), X(3), {
		"+-+-+-+",
		"|2x2x2|",
		"+x+-+x+",
		"|3|3|3|",
		"+-+x+-+",
		"x1x0x1x",
		"+x+x+x+",
	}, &db, false, true);
}
void SlitherlinkFieldSolveProblem(penciloid::slitherlink::Dictionary &db)
{
	using namespace slitherlink;

	// these problems are generated by Penciloid Slitherlink generator
	DoProblemTest(Y(10), X(10), {
		"1-33---1-1",
		"-3--1-3---",
		"------0-1-",
		"3-31-----0",
		"-2---0----",
		"----1---3-",
		"3-----01-3",
		"-0-3------",
		"---0-3--2-",
		"1-3---02-1"
	}, &db);

	DoProblemTest(Y(10), X(10), {
		"21---1-1--",
		"--2--3-333",
		"---3-----1",
		"21-------0",
		"-3--3--31-",
		"-12--0--2-",
		"3-------33",
		"2-----3---",
		"011-3--0--",
		"--3-1---22"
	}, &db);

	DoProblemTest(Y(10), X(10), {
		"01-02-2-20",
		"-----2----",
		"--0--3---1",
		"331----33-",
		"----3-----",
		"-----0----",
		"-02----010",
		"3---3--3--",
		"----0-----",
		"12-3-31-13",
	}, &db);
}
void SlitherlinkFieldDiagonalChain(penciloid::slitherlink::Dictionary &db)
{
	using namespace slitherlink;

	DoAddClueTest(Y(5), X(4), {
		"+x+x+-+ +",
		"x0x2|    ",
		"+x+-+x+ +",
		"x1| x    ",
		"+x+ + + +",
		"x  2     ",
		"+ + + + +",
		"     1x  ",
		"+ + +x+ +",
		"         ",
		"+ + + + +",
	}, &db);
	DoAddClueTest(Y(4), X(4), {
		"+x+x+-+ +",
		"x0x2|    ",
		"+x+-+x+ +",
		"x1| x    ",
		"+x+ + + +",
		"x  2     ",
		"+ + + + +",
		"     2  |",
		"+ + + +-+",
	}, &db);
	DoAddClueTest(Y(5), X(4), {
		"+x+x+-+ +",
		"x0x2|    ",
		"+x+-+x+ +",
		"x1| x    ",
		"+x+ + + +",
		"x  2    |",
		"+ + + +x+",
		"     2| |",
		"+ + +x+-+",
		"    x0x x",
		"+ +x+x+x+",
	}, &db);
	DoAddClueTest(Y(5), X(4), {
		"+x+x+-+ +",
		"x0x2|    ",
		"+x+-+x+ +",
		"x1| x    ",
		"+x+ + + +",
		"x  2     ",
		"+ + + + +",
		"     3|  ",
		"+ + +-+x+",
		"      x  ",
		"+ + + + +",
	}, &db);
}
}
}
