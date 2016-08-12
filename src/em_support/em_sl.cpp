#include <emscripten.h>
#include <emscripten/bind.h>

#include <random>
#include <vector>
#include <algorithm>
#include <string>

#include "../slitherlink/sl_generator.h"
#include "../slitherlink/sl_database.h"

using namespace emscripten;
using namespace penciloid;
using namespace slitherlink;

namespace
{
std::mt19937 rnd;
Database sl_database;

void em_slitherlink_generator_init()
{
	std::random_device dev;
	std::vector<int> v(10);
	std::generate(v.begin(), v.end(), std::ref(dev));
	std::seed_seq seed(v.begin(), v.end());
	
	rnd = std::mt19937(seed);
	sl_database.CreateDefault();
}
std::string em_slitherlink_generate(int height, int width, int n_clue, int symmetry)
{
	CluePlacement clue = GenerateCluePlacement(Y(height), X(width), n_clue,
		  symmetry == 0 ? 0
		: symmetry == 1 ? kSymmetryDyad
		: symmetry == 2 ? kSymmetryTetrad
						: 0
		, &rnd);
	Problem prob;
	
	GeneratorOption opt;
	opt.field_database = &sl_database;
	
	bool res = GenerateByLocalSearch(clue, opt, &rnd, &prob);
	
	if (!res) return std::string("");
	
	std::string ret;
	for (Y y(0); y < height; ++y) {
		if (y != 0) ret.push_back(',');
		for (X x(0); x < width; ++x) {
			Clue c = prob.GetClue(CellPosition(y, x));
			if (c == kNoClue) ret.push_back('.');
			else ret.push_back(static_cast<int>(c) + '0');
		}
	}
	return ret;
}
}

EMSCRIPTEN_BINDINGS(mod) {
	function("slGeneratorInit", &em_slitherlink_generator_init);
	function("slGeneratorGenerate", &em_slitherlink_generate);
}

