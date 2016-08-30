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
		problem.SetClue(CellPosition(Y(0), X(1)), Clue(kNoClueValue, 3));
		problem.SetClue(CellPosition(Y(0), X(2)), Clue(kNoClueValue, 8));
		problem.SetClue(CellPosition(Y(1), X(0)), Clue(4, kNoClueValue));
		problem.SetClue(CellPosition(Y(2), X(0)), Clue(7, kNoClueValue));

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
		problem.SetClue(CellPosition(Y(0), X(1)), Clue(kNoClueValue, 17));
		problem.SetClue(CellPosition(Y(0), X(2)), Clue(kNoClueValue, 12));
		problem.SetClue(CellPosition(Y(1), X(0)), Clue(16, kNoClueValue));
		problem.SetClue(CellPosition(Y(2), X(0)), Clue(13, kNoClueValue));

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