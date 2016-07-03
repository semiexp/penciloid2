#include "test_nk_field.h"
#include "test.h"

#include <cassert>
#include <cstdio>

#include "../nurikabe/nk_field.h"
#include "../nurikabe/nk_problem.h"

namespace penciloid
{
namespace test
{
void RunAllNurikabeFieldTest()
{
	NurikabeFieldAdjacentClueTest();
	NurikabeFieldCloseGroupTest();
	NurikabeFieldExpandBlackTest();
	NurikabeFieldExpandWhiteTest();
	NurikabeFieldConsistencyTest();
}
void NurikabeFieldAdjacentClueTest()
{
	using namespace nurikabe;
	
	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(1)), Clue(5));
		p.SetClue(CellPosition(Y(0), X(3)), Clue(5));

		Field f(p);
		assert(f.GetCell(CellPosition(Y(0), X(2))) == Field::kCellBlack);
		assert(f.IsInconsistent() == false);
	}
	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(1), X(1)), Clue(5));
		p.SetClue(CellPosition(Y(2), X(2)), Clue(5));

		Field f(p);
		assert(f.GetCell(CellPosition(Y(1), X(2))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(2), X(1))) == Field::kCellBlack);
		assert(f.IsInconsistent() == false);
	}
	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(0)), Clue(1, 25));
		p.SetClue(CellPosition(Y(0), X(4)), Clue(1, 25));

		Field f(p);
		f.DecideCell(CellPosition(Y(0), X(2)), Field::kCellWhite);
		f.DecideCell(CellPosition(Y(0), X(1)), Field::kCellWhite);
		assert(f.GetCell(CellPosition(Y(0), X(3))) == Field::kCellBlack);
	}
}
void NurikabeFieldCloseGroupTest()
{
	using namespace nurikabe;

	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(0)), Clue(15));
		p.SetClue(CellPosition(Y(2), X(2)), Clue(1));

		Field f(p);
		assert(f.GetCell(CellPosition(Y(1), X(2))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(2), X(1))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(3), X(2))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(2), X(3))) == Field::kCellBlack);
		assert(f.IsInconsistent() == false);
	}
	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(1), X(2)), Clue(2));
		p.SetClue(CellPosition(Y(4), X(4)), Clue(12));

		Field f(p);
		f.DecideCell(CellPosition(Y(2), X(2)), Field::kCellWhite);
		assert(f.GetCell(CellPosition(Y(0), X(2))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(1), X(1))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(1), X(3))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(2), X(1))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(2), X(3))) == Field::kCellBlack);
		assert(f.GetCell(CellPosition(Y(3), X(2))) == Field::kCellBlack);
		assert(f.IsInconsistent() == false);
	}
}
void NurikabeFieldExpandBlackTest()
{
	using namespace nurikabe;

	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(1)), Clue(10));
		p.SetClue(CellPosition(Y(0), X(3)), Clue(3));

		Field f(p);
		f.DecideCell(CellPosition(Y(4), X(4)), Field::kCellBlack);
		f.ExpandBlack();

		assert(f.GetCell(CellPosition(Y(1), X(2))) == Field::kCellBlack);
	}
}
void NurikabeFieldExpandWhiteTest()
{
	using namespace nurikabe;

	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(0)), Clue(8));
		p.SetClue(CellPosition(Y(0), X(2)), Clue(8));

		Field f(p);
		f.ExpandWhite();

		assert(f.GetCell(CellPosition(Y(1), X(0))) == Field::kCellWhite);
		assert(f.IsInconsistent() == false);
	}
	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(0)), Clue(11));
		p.SetClue(CellPosition(Y(0), X(4)), Clue(6));

		Field f(p);
		f.DecideCell(CellPosition(Y(0), X(2)), Field::kCellBlack);
		f.DecideCell(CellPosition(Y(2), X(2)), Field::kCellBlack);
		f.DecideCell(CellPosition(Y(3), X(2)), Field::kCellBlack);
		f.DecideCell(CellPosition(Y(4), X(2)), Field::kCellBlack);
		f.ExpandWhite();

		assert(f.GetCell(CellPosition(Y(1), X(1))) == Field::kCellWhite);
		assert(f.GetCell(CellPosition(Y(1), X(2))) == Field::kCellWhite);
		assert(f.IsInconsistent() == false);
	}
	{
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(0)), Clue(5));
		p.SetClue(CellPosition(Y(4), X(4)), Clue(9));

		Field f(p);
		f.DecideCell(CellPosition(Y(0), X(1)), Field::kCellBlack);
		f.DecideCell(CellPosition(Y(2), X(1)), Field::kCellBlack);
		f.DecideCell(CellPosition(Y(3), X(0)), Field::kCellBlack);
		f.DecideCell(CellPosition(Y(1), X(0)), Field::kCellWhite);
		f.ExpandWhite();

		assert(f.GetCell(CellPosition(Y(1), X(1))) == Field::kCellWhite);
		assert(f.GetCell(CellPosition(Y(1), X(2))) == Field::kCellWhite);
		assert(f.IsInconsistent() == false);
	}
}
void NurikabeFieldConsistencyTest()
{
	using namespace nurikabe;

	{
		// Consistent
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(0)), Clue(1, 25));
		p.SetClue(CellPosition(Y(4), X(4)), Clue(1, 25));

		Field f(p);
		f.DecideCell(CellPosition(Y(0), X(4)), Field::kCellWhite);
		f.DecideCell(CellPosition(Y(0), X(3)), Field::kCellWhite);
		f.DecideCell(CellPosition(Y(1), X(4)), Field::kCellBlack);
		f.CheckConsistency();

		assert(f.IsInconsistent() == false);
	}
	{
		// White group without a clue
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(0)), Clue(1, 25));
		p.SetClue(CellPosition(Y(4), X(4)), Clue(1, 25));

		Field f(p);
		f.DecideCell(CellPosition(Y(0), X(4)), Field::kCellWhite);
		f.DecideCell(CellPosition(Y(0), X(3)), Field::kCellBlack);
		f.DecideCell(CellPosition(Y(1), X(4)), Field::kCellBlack);
		f.CheckConsistency();

		assert(f.IsInconsistent() == true);
	}
	{
		// Disconnected black group
		Problem p(Y(5), X(5));
		p.SetClue(CellPosition(Y(0), X(0)), Clue(1, 25));
		p.SetClue(CellPosition(Y(4), X(4)), Clue(1, 25));

		Field f(p);
		f.DecideCell(CellPosition(Y(0), X(4)), Field::kCellBlack);
		f.DecideCell(CellPosition(Y(0), X(3)), Field::kCellWhite);
		f.DecideCell(CellPosition(Y(1), X(4)), Field::kCellWhite);
		f.DecideCell(CellPosition(Y(4), X(0)), Field::kCellBlack);
		f.CheckConsistency();

		assert(f.IsInconsistent() == true);
	}
}
}
}
