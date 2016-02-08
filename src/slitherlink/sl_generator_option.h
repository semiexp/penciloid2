#pragma once

#include "sl_database.h"

namespace penciloid
{
namespace slitherlink
{
struct GeneratorOption
{
	GeneratorOption() : use_assumption(false), field_database(nullptr) {}

	bool use_assumption;
	Database *field_database;
};
}
}