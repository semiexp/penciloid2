#include "test_akari_field.h"
#include "test.h"

#include <cassert>
#include <cstdio>

#include "../akari/ak_field.h"
#include "../akari/ak_problem.h"

namespace penciloid
{
namespace test
{
void RunAllAkariFieldTest()
{
	AkariFieldClueTest();
}
void AkariFieldClueTest()
{
	using namespace akari;

	{
		Problem prob(Y(4), X(4));
		prob.SetClue(CellPosition(Y(0), X(0)), Clue(2));

		Field field(prob);
		assert(field.GetCell(CellPosition(Y(0), X(1))) == Field::CELL_LIGHT);
		assert(field.GetCell(CellPosition(Y(1), X(0))) == Field::CELL_LIGHT);
		assert(field.GetCell(CellPosition(Y(0), X(2))) == Field::CELL_LIT_BY_OTHER);
		assert(field.GetCell(CellPosition(Y(1), X(1))) == Field::CELL_LIT_BY_OTHER);
		assert(field.GetCell(CellPosition(Y(2), X(0))) == Field::CELL_LIT_BY_OTHER);

		assert(field.GetNumberOfDecidedCells() == 12);
	}

	{
		Problem prob(Y(4), X(4));
		prob.SetClue(CellPosition(Y(0), X(1)), Clue(3));

		Field field(prob);
		assert(field.GetCell(CellPosition(Y(0), X(0))) == Field::CELL_LIGHT);
		assert(field.GetCell(CellPosition(Y(1), X(1))) == Field::CELL_LIGHT);
		assert(field.GetCell(CellPosition(Y(0), X(2))) == Field::CELL_LIGHT);
		assert(field.GetCell(CellPosition(Y(1), X(0))) == Field::CELL_LIT_BY_OTHER);
		assert(field.GetCell(CellPosition(Y(1), X(2))) == Field::CELL_LIT_BY_OTHER);

		assert(field.GetNumberOfDecidedCells() == 14);
	}

	{
		Problem prob(Y(5), X(5));
		prob.SetClue(CellPosition(Y(0), X(1)), Clue(3));
		prob.SetClue(CellPosition(Y(1), X(4)), Clue(-1));
		prob.SetClue(CellPosition(Y(2), X(2)), Clue(2));
		prob.SetClue(CellPosition(Y(3), X(0)), Clue(-1));
		prob.SetClue(CellPosition(Y(4), X(3)), Clue(-1));

		Field field(prob);
		assert(field.GetCell(CellPosition(Y(4), X(0))) == Field::CELL_LIGHT);
		assert(field.GetNumberOfDecidedCells() == 25);
		assert(field.IsInconsistent() == false);
		assert(field.IsFullySolved() == true);
	}

	{
		const char *pb[10] = {
			".....11...",
			"..#.......",
			"..#....11.",
			"2.........",
			"#...##....",
			"....#1...1",
			".........1",
			".0#....#..",
			".......1..",
			"...2#....."
		};
		Problem prob(Y(10), X(10), pb);
		Field field(prob);

		assert(field.IsFullySolved() == true);
		assert(field.IsInconsistent() == false);
	}
}
}
}
