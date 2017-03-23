#include "test_slitherlink_dictionary.h"
#include "test.h"

#include "../slitherlink/sl_dictionary.h"

namespace penciloid
{
namespace test
{
void RunAllSlitherlinkDictionaryTest()
{
	SlitherlinkDictionaryRestricted();
}
void SlitherlinkDictionaryRestricted()
{
	using namespace slitherlink;
	Dictionary dic_default, dic_restricted;
	dic_default.CreateDefault();
	dic_restricted.CreateRestricted(DictionaryMethod());

	for (int i = 0; i < 531441; ++i) {
		for (int j = 0; j < 4; ++j) {
			int val_default = dic_default.Get(i, j);
			int val_restricted = dic_restricted.Get(i, j);
			
			assert(val_default == val_restricted);
		}
	}
}
}
}