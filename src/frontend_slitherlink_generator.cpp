#include <iostream>
#include <fstream>
#include <string>
#include <random>

#include "slitherlink/sl_dictionary.h"
#include "slitherlink/sl_generator.h"
#include "slitherlink/sl_generator_option.h"
#include "slitherlink/sl_clue_placement.h"
#include "slitherlink/sl_problem.h"

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " file" << std::endl;
		return 0;
	}

	using namespace penciloid;
	using namespace slitherlink;

	std::ifstream ifs(argv[1]);
	int height, width;
	ifs >> height >> width;

	CluePlacement clue_placement{ Y(height), X(width) };
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			std::string token;
			ifs >> token;

			if (token[0] >= '0' && token[0] <= '3') {
				clue_placement.SetClue(CellPosition(y, x), Clue(static_cast<int>(token[0] - '0')));
			} else if (token[0] == '5') {
				clue_placement.SetClue(CellPosition(y, x), kSomeClue);
			}
		}
	}

	GeneratorOption opt;
	Dictionary dic;
	dic.CreateDefault();
	opt.field_dictionary = &dic;

	Problem problem;
	std::random_device dev;
	std::mt19937 rnd(dev());

	while (!GenerateByLocalSearch(clue_placement, opt, &rnd, &problem));

	std::string output_fn = argv[1];
	output_fn += ".generated.txt";
	std::ofstream ofs(output_fn);
	ofs << height << std::endl;
	ofs << width << std::endl;
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			Clue c = problem.GetClue(CellPosition(y, x));
			if (c == kNoClue) ofs << ". ";
			else ofs << static_cast<int>(c) << " ";
		}
		ofs << std::endl;
	}
	return 0;
}
