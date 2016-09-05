#pragma once

#include "sl_dictionary.h"
#include "sl_method.h"

namespace penciloid
{
namespace slitherlink
{
struct GeneratorOption
{
	GeneratorOption() : use_assumption(false), method(), field_dictionary(nullptr) {}

	bool use_assumption;
	Method method;
	Dictionary *field_dictionary;
};
}
}