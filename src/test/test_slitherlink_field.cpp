#include "test_slitherlink_field.h"
#include "test.h"

#include <cassert>

#include "../slitherlink/sl_field.h"
#include "../slitherlink/sl_database.h"

namespace penciloid
{
namespace test
{
void RunAllSlitherlinkFieldTest()
{
	SlitherlinkFieldAddClue();
}
void SlitherlinkFieldAddClue()
{
	using namespace slitherlink;

	Database db;
	db.CreateDefault();

	Field field(3, 3);
	field.SetDatabase(&db);
	field.AddClue(Position(1, 1), 0);

	assert(field.GetEdge(Position(2, 3)) == Field::EDGE_BLANK);
}
}
}
