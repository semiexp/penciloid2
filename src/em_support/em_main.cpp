#include <emscripten.h>
#include <emscripten/bind.h>

#include <random>
#include <vector>
#include <algorithm>
#include <string>

#include "../slitherlink/sl_generator.h"
#include "../slitherlink/sl_dictionary.h"
#include "../nurikabe/nk_generator.h"

using namespace emscripten;
using namespace penciloid;

namespace
{
std::mt19937 rnd;
slitherlink::Dictionary sl_dictionary;

void em_slitherlink_generator_init()
{
	using namespace slitherlink;
	std::random_device dev;
	std::vector<int> v(10);
	std::generate(v.begin(), v.end(), std::ref(dev));
	std::seed_seq seed(v.begin(), v.end());

	rnd = std::mt19937(seed);
	sl_dictionary.CreateDefault();
}
std::string em_slitherlink_generate(int height, int width, int n_clue, int symmetry)
{
	using namespace slitherlink;
	CluePlacement clue = GenerateCluePlacement(Y(height), X(width), n_clue,
		symmetry == 0 ? 0
		: symmetry == 1 ? kSymmetryDyad
		: symmetry == 2 ? kSymmetryTetrad
		: 0
		, &rnd);
	Problem prob;

	GeneratorOption opt;
	opt.field_dictionary = &sl_dictionary;

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
void em_nurikabe_generator_init()
{
	using namespace nurikabe;
	std::random_device dev;
	std::vector<int> v(10);
	std::generate(v.begin(), v.end(), std::ref(dev));
	std::seed_seq seed(v.begin(), v.end());

	rnd = std::mt19937(seed);
}
std::string em_nurikabe_generate(int height, int width)
{
	using namespace nurikabe;
	Problem prob;
	bool res = GenerateByLocalSearch(Y(height), X(width), &rnd, &prob);

	if (!res) return std::string("");

	std::string ret;
	for (Y y(0); y < height; ++y) {
		if (y != 0) ret.push_back(',');
		for (X x(0); x < width; ++x) {
			Clue c = prob.GetClue(CellPosition(y, x));
			if (c == kNoClue) ret.push_back('.');
			else {
				int c_int = c.clue_low;
				if (c_int <= 9) {
					ret.push_back(static_cast<char>(c_int + '0'));
				} else if (c_int <= 36) {
					ret.push_back(static_cast<char>(c_int - 10 + 'a'));
				}
			}
		}
	}
	return ret;
}
}

EMSCRIPTEN_BINDINGS(mod) {
	// slitherlink
	function("slGeneratorInit", &em_slitherlink_generator_init);
	function("slGeneratorGenerate", &em_slitherlink_generate);

	// nurikabe
	function("nkGeneratorInit", &em_nurikabe_generator_init);
	function("nkGeneratorGenerate", &em_nurikabe_generate);
}

