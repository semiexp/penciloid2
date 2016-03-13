#pragma once

#include "sl_database.h"
#include "sl_method.h"

namespace penciloid
{
namespace slitherlink
{
struct GeneratorOption
{
	GeneratorOption() : use_assumption(false), method(), field_database(nullptr) {}

	bool use_assumption;
	Method method;
	Database *field_database;
};
}
}