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

	{
		Field field(3, 3);
		field.SetDatabase(&db);
		field.AddClue(Position(1, 1), 0);

		assert(field.GetEdge(Position(2, 3)) == Field::EDGE_BLANK);
		assert(field.GetEdge(Position(3, 2)) == Field::EDGE_BLANK);
		assert(field.GetEdge(Position(4, 3)) == Field::EDGE_BLANK);
		assert(field.GetEdge(Position(3, 4)) == Field::EDGE_BLANK);
	}

	{
		Field field(3, 3);
		field.SetDatabase(&db);
		field.AddClue(Position(0, 0), 1);

		assert(field.GetEdge(Position(0, 1)) == Field::EDGE_BLANK);
		assert(field.GetEdge(Position(1, 0)) == Field::EDGE_BLANK);
	}

	{
		Field field(3, 3);
		field.SetDatabase(&db);
		field.AddClue(Position(0, 0), 2);

		assert(field.GetEdge(Position(0, 3)) == Field::EDGE_LINE);
		assert(field.GetEdge(Position(3, 0)) == Field::EDGE_LINE);
	}

	{
		Field field(3, 3);
		field.SetDatabase(&db);
		field.AddClue(Position(0, 0), 3);

		assert(field.GetEdge(Position(0, 1)) == Field::EDGE_LINE);
		assert(field.GetEdge(Position(1, 0)) == Field::EDGE_LINE);
	}
}
}
}
