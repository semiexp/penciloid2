#pragma once

#include "../slitherlink/sl_dictionary.h"

namespace penciloid
{
namespace test
{
void SlitherlinkFieldAddClue(penciloid::slitherlink::Dictionary &db);
void SlitherlinkFieldTheorem(penciloid::slitherlink::Dictionary &db);
void SlitherlinkFieldFullySolvableProblem(penciloid::slitherlink::Dictionary &db);
void SlitherlinkFieldSolveProblem(penciloid::slitherlink::Dictionary &db);
void SlitherlinkFieldDiagonalChain(penciloid::slitherlink::Dictionary &db);
}
}