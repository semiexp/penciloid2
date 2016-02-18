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
	}
}
}
}
