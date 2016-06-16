#include "test.h"

namespace penciloid
{
namespace test
{
void RunAllTest()
{
	RunAllGridLoopTest();
	RunAllGraphSeparationTest();
	RunAllSlitherlinkFieldTest();
	RunAllAkariProblemTest();
	RunAllAkariFieldTest();
	RunAllYajilinFieldTest();
	RunAllMasyuFieldTest();
	RunAllNurikabeFieldTest();
}
}
}