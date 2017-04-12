#include "test_kakuro_field.h"
#include "test.h"

#include <cassert>

#include "../kakuro/kk_problem.h"
#include "../kakuro/kk_field.h"
#include "../kakuro/kk_dictionary.h"

namespace penciloid
{
namespace test
{
void RunAllKakuroFieldTest()
{
	KakuroFieldCheckGroupAllTest();
}
void KakuroFieldCheckGroupAllTest()
{
	using namespace kakuro;

	Dictionary dic;
	dic.CreateDefault();

	{
		Problem problem(Y(3), X(3));
		problem.SetClue(CellPosition(Y(0), X(0)), kNoClueCell);
		problem.SetClue(CellPosition(Y(0), X(1)), Clue(3, kNoClueValue));
		problem.SetClue(CellPosition(Y(0), X(2)), Clue(8, kNoClueValue));
		problem.SetClue(CellPosition(Y(1), X(0)), Clue(kNoClueValue, 4));
		problem.SetClue(CellPosition(Y(2), X(0)), Clue(kNoClueValue, 7));

		Field field(problem, &dic);
		field.CheckGroupAll();

		assert(field.GetCell(CellPosition(Y(1), X(1))) == 1);
		assert(field.GetCell(CellPosition(Y(1), X(2))) == 3);
		assert(field.GetCell(CellPosition(Y(2), X(1))) == 2);
		assert(field.GetCell(CellPosition(Y(2), X(2))) == 5);
		assert(field.IsInconsistent() == false);
	}
	{
		Problem problem(Y(3), X(3));
		problem.SetClue(CellPosition(Y(0), X(0)), kNoClueCell);
		problem.SetClue(CellPosition(Y(0), X(1)), Clue(17, kNoClueValue));
		problem.SetClue(CellPosition(Y(0), X(2)), Clue(12, kNoClueValue));
		problem.SetClue(CellPosition(Y(1), X(0)), Clue(kNoClueValue, 16));
		problem.SetClue(CellPosition(Y(2), X(0)), Clue(kNoClueValue, 13));

		Field field(problem, &dic);
		field.CheckGroupAll();

		assert(field.GetCell(CellPosition(Y(1), X(1))) == 9);
		assert(field.GetCell(CellPosition(Y(1), X(2))) == 7);
		assert(field.GetCell(CellPosition(Y(2), X(1))) == 8);
		assert(field.GetCell(CellPosition(Y(2), X(2))) == 5);
		assert(field.IsInconsistent() == false);
	}
}
}
}