#pragma once

#include "../slitherlink/sl_database.h"

namespace penciloid
{
namespace test
{
void SlitherlinkFieldAddClue(penciloid::slitherlink::Database &db);
void SlitherlinkFieldTheorem(penciloid::slitherlink::Database &db);
void SlitherlinkFieldFullySolvableProblem(penciloid::slitherlink::Database &db);
void SlitherlinkFieldSolveProblem(penciloid::slitherlink::Database &db);
void SlitherlinkFieldDiagonalChain(penciloid::slitherlink::Database &db);
}
}