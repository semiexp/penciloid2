#include "test.h"

namespace penciloid
{
namespace test
{
void RunAllTest()
{
	RunAllGridLoopTest();
	RunAllSlitherlinkFieldTest();
	RunAllAkariProblemTest();
	RunAllAkariFieldTest();
	RunAllYajilinFieldTest();
	RunAllMasyuFieldTest();
	RunAllNurikabeFieldTest();
}
}
}