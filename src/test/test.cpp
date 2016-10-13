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
	RunAllYajilinProblemTest();
	RunAllYajilinFieldTest();
	RunAllMasyuFieldTest();
	RunAllNurikabeFieldTest();
	RunAllKakuroFieldTest();
}
}
}