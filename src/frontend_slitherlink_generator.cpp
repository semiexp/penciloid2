#include <iostream>
#include <fstream>
#include <string>
#include <random>

#include "slitherlink/sl_dictionary.h"
#include "slitherlink/sl_generator.h"
#include "slitherlink/sl_generator_option.h"
#include "slitherlink/sl_clue_placement.h"
#include "slitherlink/sl_problem.h"

namespace
{
bool GenerateWithCluePlacement(
	penciloid::slitherlink::CluePlacement &placement, 
	penciloid::slitherlink::GeneratorOption &opt, 
	std::mt19937 *rnd, 
	penciloid::slitherlink::Problem *problem,
	int n_trial = -1)
{
	if (n_trial < 0) {
		while (!penciloid::slitherlink::GenerateByLocalSearch(placement, opt, rnd, problem));
		return true;
	}
	while (n_trial) {
		if (penciloid::slitherlink::GenerateByLocalSearch(placement, opt, rnd, problem)) {
			return true;
		}
		--n_trial;
	}
	return false;
}
void GenerateAuto(
	int height, 
	int width, 
	int n_clue_lo, 
	int n_clue_hi, 
	int symmetry, 
	penciloid::slitherlink::GeneratorOption &opt,
	std::mt19937 *rnd, 
	penciloid::slitherlink::Problem *problem)
{
	using namespace penciloid;
	using namespace slitherlink;
	for (;;) {
		int n_clues = std::uniform_int_distribution<int>(n_clue_lo, n_clue_hi)(*rnd);
		CluePlacement placement = GenerateCluePlacement(Y(height), X(width), n_clues, symmetry, rnd);
		if (GenerateWithCluePlacement(placement, opt, rnd, problem, 3)) {
			break;
		}
	}
}
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " [options] file" << std::endl;
		return 0;
	}

	std::string in_filename = "", out_filename = "";
	int height = -1, width = -1, n_clue_lo = -1, n_clue_hi = -1;
	bool gen_clue_auto = false;

	// parse options
	int arg_idx = 1;
	for (; arg_idx < argc; ++arg_idx) {
		if (argv[arg_idx][0] != '-') {
			// option ended
			break;
		}
		char opt = argv[arg_idx][1];
		switch (opt)
		{
		case 'o':
			// specify the output file
			out_filename = argv[arg_idx + 1];
			++arg_idx;
			break;
		case 'a':
			// generate clue placement automatically
			gen_clue_auto = true;
			break;
		case 'h':
			// specify the height of the generated problem (enabled only if '-a' is specified)
			height = atoi(argv[arg_idx + 1]);
			++arg_idx;
			break;
		case 'w':
			// specify the width of the generated problem (enabled only if '-a' is specified)
			width = atoi(argv[arg_idx + 1]);
			++arg_idx;
			break;
		case 'c':
			// specify the minimum number of the clues
			n_clue_lo = atoi(argv[arg_idx + 1]);
			++arg_idx;
			break;
		case 'C':
			// specify the minimum number of the clues
			n_clue_hi = atoi(argv[arg_idx + 1]);
			++arg_idx;
			break;
		}
	}
	if (gen_clue_auto) {
		if (out_filename.empty()) {
			std::cerr << "Error: -o must be specified if -a is specified." << std::endl;
			return 0;
		}
	}
	if (!gen_clue_auto) {
		in_filename = argv[arg_idx];
	}

	using namespace penciloid;
	using namespace slitherlink;

	GeneratorOption opt;
	Dictionary dic;
	dic.CreateDefault();
	opt.field_dictionary = &dic;

	Problem problem;
	std::random_device dev;
	std::mt19937 rnd(dev());

	if (!gen_clue_auto) {
		std::ifstream ifs(in_filename);
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

		GenerateWithCluePlacement(clue_placement, opt, &rnd, &problem);
	} else {
		if (n_clue_lo == -1 || n_clue_hi == -1) {
			n_clue_lo = static_cast<int>(height * width * 0.3);
			n_clue_hi = static_cast<int>(height * width * 0.4);
		}
		GenerateAuto(height, width, n_clue_lo, n_clue_hi, penciloid::slitherlink::kSymmetryDyad, opt, &rnd, &problem);
	}

	if (out_filename.empty()) {
		out_filename = in_filename + ".generated.txt";
	}
	std::ofstream ofs(out_filename);
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
