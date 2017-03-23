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
	RunAllSlitherlinkDictionaryTest();
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