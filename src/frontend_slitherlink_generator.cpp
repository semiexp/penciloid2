#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <sstream>

#include "slitherlink/sl_dictionary.h"
#include "slitherlink/sl_generator.h"
#include "slitherlink/sl_generator_option.h"
#include "slitherlink/sl_clue_placement.h"
#include "slitherlink/sl_problem.h"

namespace
{
void ShowUsage(int argc, char** argv)
{
	std::cerr << "Usage: " << argv[0] << " [options] [file]" << std::endl;
	std::cerr << "Options:\n\
  -o <file>     Place the output into <file>\n\
  -a            Generate the clue placement automatically\n\
  -h <height>   Set the height of the problem <height>\n\
  -w <width>    Set the width of the problem <width>\n\
  -c <num>      Set the minimum number of the clues <num>\n\
  -C <num>      Set the maximum number of the clues <num>\n\
\n\
Options -h, -w, -c and -C are valid only if -a is specified.\n\
If -a is not specified, the input file should be specified for the clue placement." << std::endl;
}
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
		ShowUsage(argc, argv);
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
		bool is_next_int = false;
		int val_next_int;
		if (arg_idx + 1 < argc) {
			std::istringstream iss(argv[arg_idx + 1]);
			iss >> val_next_int;
			if (!iss.fail()) is_next_int = true;
		}

		char opt = argv[arg_idx][1];
		switch (opt)
		{
		case 'o':
			// specify the output file
			if (arg_idx + 1 >= argc) {
				std::cerr << "error: missing value after -o" << std::endl;
				return 0;
			}
			out_filename = argv[arg_idx + 1];
			++arg_idx;
			break;
		case 'a':
			// generate clue placement automatically
			gen_clue_auto = true;
			break;
		case 'h':
			// specify the height of the generated problem (enabled only if '-a' is specified)
			if (!is_next_int) {
				std::cerr << "error: missing integer value after -h" << std::endl;
				return 0;
			}
			height = val_next_int;
			++arg_idx;
			break;
		case 'w':
			// specify the width of the generated problem (enabled only if '-a' is specified)
			if (!is_next_int) {
				std::cerr << "error: missing integer value after -w" << std::endl;
				return 0;
			}
			width = val_next_int;
			++arg_idx;
			break;
		case 'c':
			// specify the minimum number of the clues
			if (!is_next_int) {
				std::cerr << "error: missing integer value after -c" << std::endl;
				return 0;
			}
			n_clue_lo = val_next_int;
			++arg_idx;
			break;
		case 'C':
			// specify the maximum number of the clues
			if (!is_next_int) {
				std::cerr << "error: missing integer value after -C" << std::endl;
				return 0;
			}
			n_clue_hi = val_next_int;
			++arg_idx;
			break;
		default:
			std::cerr << "error: unrecognized option '" << argv[arg_idx] << "'" << std::endl;
			return 0;
		}
	}
	if (gen_clue_auto) {
		if (out_filename.empty()) {
			std::cerr << "error: -o must be specified if -a is specified." << std::endl;
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
		if (ifs.fail()) {
			std::cerr << "error: couldn't open file '" << in_filename << "'" << std::endl;
			return 0;
		}
		ifs >> height >> width;
		if (ifs.fail() || height <= 0 || width <= 0) {
			std::cerr << "error: invalid input format" << std::endl;
			return 0;
		}
		CluePlacement clue_placement{ Y(height), X(width) };
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				std::string token;
				ifs >> token;
				if (ifs.fail()) {
					std::cerr << "error: invalid input format" << std::endl;
					return 0;
				}

				if (token[0] >= '0' && token[0] <= '3') {
					clue_placement.SetClue(CellPosition(y, x), Clue(static_cast<int>(token[0] - '0')));
				} else if (token[0] == '5') {
					clue_placement.SetClue(CellPosition(y, x), kSomeClue);
				} else {
					std::cerr << "error: invalid input format" << std::endl;
					return 0;
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
	if (!ofs.good()) {
		std::cerr << "error: couldn't open file '" << in_filename << "'" << std::endl;
		return 0;
	}
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
