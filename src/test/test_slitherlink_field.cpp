#include "test_slitherlink_field.h"
#include "test.h"

#include <cassert>
#include <vector>

#include "../slitherlink/sl_field.h"
#include "../slitherlink/sl_database.h"

namespace
{
// Place clues and check edges
void DoAddClueTest(penciloid::Y height, penciloid::X width, std::vector<const char*> test_target, penciloid::slitherlink::Database *db)
{
	using namespace penciloid;
	using namespace penciloid::slitherlink;

	Field field(height, width);
	field.SetDatabase(db);

	for (Y y = 0; y < height; ++y) {
		for (X x = 0; x < width; ++x) {
			if ('0' <= test_target[y * 2 + 1][x * 2 + 1] && test_target[y * 2 + 1][x * 2 + 1] <= '3') {
				field.AddClue(Position(y, x), test_target[y * 2 + 1][x * 2 + 1] - '0');
			}
		}
	}
	
	for (Y y = 0; y <= 2 * height; ++y) {
		for (X x = 0; x <= 2 * width; ++x) {
			if (y % 2 != x % 2) {
				Field::EdgeState expected;
				if (test_target[y][x] == 'x') expected = Field::EDGE_BLANK;
				else if (test_target[y][x] == ' ') expected = Field::EDGE_UNDECIDED;
				else expected = Field::EDGE_LINE;

				if (field.GetEdge(Position(y, x)) != expected) {
					y = y;
				}
				assert(field.GetEdge(Position(y, x)) == expected);
			}
		}
	}
}
}

namespace penciloid
{
namespace test
{
void RunAllSlitherlinkFieldTest()
{
	SlitherlinkFieldAddClue();
	SlitherlinkFieldTheorem();
}
void SlitherlinkFieldAddClue()
{
	using namespace slitherlink;

	Database db;
	db.CreateDefault();

	DoAddClueTest(3, 3, {
		"+ + + +",
		"       ",
		"+ +x+ +",
		"  x0x  ",
		"+ +x+ +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(3, 3, {
		"+x+ + +",
		"x1     ",
		"+ + + +",
		"       ",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(3, 3, {
		"+ +-+ +",
		" 2     ",
		"+ + + +",
		"|      ",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(3, 3, {
		"+-+ + +",
		"|3     ",
		"+ + + +",
		"       ",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(3, 3, {
		"+x+x+-+",
		"x0x2| |",
		"+x+-+x+",
		"x | x |",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(3, 3, {
		"+-+-+-+",
		"|2x1x |",
		"+x+x+ +",
		"| x    ",
		"+ + + +",
		"       ",
		"+ + + +",
	}, &db);
	DoAddClueTest(3, 3, {
		"+-+-+-+",
		"|2x1x |",
		"+x+x+-+",
		"| x2| x",
		"+ +-+x+",
		"    x x",
		"+ +x+x+",
	}, &db);
}
void SlitherlinkFieldTheorem()
{
	using namespace slitherlink;

	Database db;
	db.CreateDefault();

	DoAddClueTest(3, 3, {
		"+ + + +",
		"  x    ",
		"+ + + +",
		"|3|3|  ",
		"+ + + +",
		"  x    ",
		"+ + + +",
	}, &db);
	DoAddClueTest(3, 4, {
		"+ + +-+x+",
		"     3| x",
		"+ + + + +",
		"  |3     ",
		"+x+-+ + +",
		"  x      ",
		"+ + + + +",
	}, &db);
	DoAddClueTest(3, 3, {
		"+-+-+-+",
		"| x x |",
		"+x+-+x+",
		"|3|3| |",
		"+-+x+ +",
		"x1x    ",
		"+x+x+ +",
	}, &db);
}
}
}
