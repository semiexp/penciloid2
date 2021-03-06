#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <sstream>
#include <thread>
#include <mutex>

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
  --help         Display this information\n\
  -o <file>      Place the output into <file>\n\
  -pb            Output in the PencilBox format (default)\n\
  -pl            Output in the Penciloid format\n\
  -n <num>       Generate <num> problems under the given setting\n\
  -p <threads>   Generate problems using <threads> threads\n\
  -a             Append to the output file\n\
  -c             Generate the clue placement automatically\n\
  -h <height>    Set the height of the problem <height>\n\
  -w <width>     Set the width of the problem <width>\n\
  -m <num>       Set the minimum number of the clues <num>\n\
  -M <num>       Set the maximum number of the clues <num>\n\
  -s <symmetry>  Specify the symmetry of the clue placement\n\
  -t             Use the trial-and-error technique\n\
\n\
Options -h, -w, -m, -M and -s are valid only if -c is specified.\n\
-a is automatically set if -n is specified.\n\
If -c is not specified, the input file should be specified for the clue placement." << std::endl;
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
int ParseSymmetry(std::string str)
{
	int ret = 0;
	std::istringstream iss(str);
	std::string token;
	while (std::getline(iss, token, ',')) {
		if (token == "h" || token == "horizontal") {
			ret |= penciloid::slitherlink::kSymmetryHorizontalLine;
		} else if (token == "v" || token == "vertical") {
			ret |= penciloid::slitherlink::kSymmetryVerticalLine;
		} else if (token == "90" || token == "t" || token == "tetrad") {
			ret |= penciloid::slitherlink::kSymmetryTetrad;
		} else if (token == "180" || token == "d" || token == "dyad") {
			ret |= penciloid::slitherlink::kSymmetryDyad;
		} else {
			std::cerr << "error: unrecognized symmetry '" << token << "'" << std::endl;
			exit(0);
		}
	}
	return ret;
}
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		ShowUsage(argc, argv);
		return 0;
	}

	std::string in_filename = "", out_filename = "";
	int height = -1, width = -1, n_clue_lo = -1, n_clue_hi = -1, symmetry = 0;
	int n_problems = 1;
	int n_threads = 1;

	bool gen_clue_auto = false;
	bool append_to_output = false;
	bool output_in_penciloid_format = false;
	bool use_trial_and_error = false;

	// parse options
	int arg_idx = 1;
	for (; arg_idx < argc; ++arg_idx) {
		if (argv[arg_idx][0] != '-') {
			// option ended
			break;
		}
		std::string opt = argv[arg_idx];
		if (opt[1] == 'o') {
			if (opt.size() == 2) {
				if (arg_idx + 1 >= argc) {
					std::cerr << "error: missing value after -o" << std::endl;
					return 0;
				}
				out_filename = argv[arg_idx + 1];
				++arg_idx;
			} else {
				out_filename = opt.substr(2);
			}
		} else if (opt == "-pb") {
			output_in_penciloid_format = false;
		} else if (opt == "-pl") {
			output_in_penciloid_format = true;
		} else if (opt == "-c") {
			gen_clue_auto = true;
		} else if (opt == "-a") {
			append_to_output = true;
		} else if (opt == "-t") {
			use_trial_and_error = true;
		} else if (opt[1] == 's') {
			if (opt.size() == 2) {
				if (arg_idx + 1 >= argc) {
					std::cerr << "error: missing value after -s" << std::endl;
					return 0;
				}
				symmetry = ParseSymmetry(argv[arg_idx + 1]);
				++arg_idx;
			} else {
				symmetry = ParseSymmetry(opt.substr(2));
			}
		} else if (opt[1] == 'h' || opt[1] == 'w' || opt[1] == 'm' || opt[1] == 'M' || opt[1] == 'n' || opt[1] == 'p') {
			std::istringstream iss;
			if (opt.size() == 2) {
				if (arg_idx + 1 >= argc) {
					std::cerr << "error: missing value after -" << opt[1] << std::endl;
					return 0;
				}
				iss.str(argv[arg_idx + 1]);
				++arg_idx;
			} else {
				iss.str(opt.substr(2));
			}
			int val;
			iss >> val;
			if (iss.fail()) {
				std::cerr << "error: missing value after -" << opt[1] << std::endl;
				return 0;
			}

			switch (opt[1]) {
			case 'h': height = val; break;
			case 'w': width = val; break;
			case 'm': n_clue_lo = val; break;
			case 'M': n_clue_hi = val; break;
			case 'n': n_problems = val; append_to_output = true; break;
			case 'p': n_threads = val; break;
			}
		} else if (opt == "--help") {
			ShowUsage(argc, argv);
			return 0;
		} else {
			std::cerr << "error: unrecognized option '" << argv[arg_idx] << "'" << std::endl;
			return 0;
		}
	}
	if (gen_clue_auto) {
		if (out_filename.empty()) {
			std::cerr << "error: -o must be specified if -c is specified." << std::endl;
			return 0;
		}
	}
	if (!gen_clue_auto) {
		if (arg_idx < argc) {
			in_filename = argv[arg_idx];
		} else {
			std::cerr << "error: no input file" << std::endl;
			return 0;
		}
	}

	using namespace penciloid;
	using namespace slitherlink;

	GeneratorOption opt;
	Dictionary dic;
	dic.CreateDefault();
	opt.field_dictionary = &dic;
	opt.use_assumption = use_trial_and_error;

	std::ofstream *ofs = nullptr;

	CluePlacement clue_placement;
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
		clue_placement = CluePlacement(Y(height), X(width));
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
				} else if (token[0] != '.') {
					std::cerr << "error: invalid input format" << std::endl;
					return 0;
				}
			}
		}
	}

	std::mutex mtx;
	int gen_problems = 0;

	auto worker = [&]() {
		Problem problem;
		std::random_device dev;
		std::vector<int> seed(10);
		std::generate(seed.begin(), seed.end(), std::ref(dev));
		std::seed_seq seq(seed.begin(), seed.end());
		std::mt19937 rnd(seq);
		for (;;) {
			if (!gen_clue_auto) {
				GenerateWithCluePlacement(clue_placement, opt, &rnd, &problem);
			} else {
				if (n_clue_lo == -1 || n_clue_hi == -1) {
					n_clue_lo = static_cast<int>(height * width * 0.3);
					n_clue_hi = static_cast<int>(height * width * 0.4);
				}
				GenerateAuto(height, width, n_clue_lo, n_clue_hi, symmetry, opt, &rnd, &problem);
			}

			mtx.lock();
			if (out_filename.empty()) {
				out_filename = in_filename + ".generated.txt";
			}
			if (ofs == nullptr) {
				ofs = new std::ofstream(out_filename, append_to_output ? std::ios::app : std::ios::out);
				if (!ofs->good()) {
					std::cerr << "error: couldn't open file '" << in_filename << "'" << std::endl;
					return 0;
				}
			}
			if (output_in_penciloid_format) {
				*ofs << height << " " << width << std::endl;
				for (Y y(0); y < height; ++y) {
					for (X x(0); x < width; ++x) {
						Clue c = problem.GetClue(CellPosition(y, x));
						if (c == kNoClue) *ofs << ".";
						else *ofs << static_cast<int>(c);
					}
					*ofs << std::endl;
				}
				*ofs << std::endl;
			} else {
				*ofs << height << std::endl;
				*ofs << width << std::endl;
				for (Y y(0); y < height; ++y) {
					for (X x(0); x < width; ++x) {
						Clue c = problem.GetClue(CellPosition(y, x));
						if (c == kNoClue) *ofs << ". ";
						else *ofs << static_cast<int>(c) << " ";
					}
					*ofs << std::endl;
				}
			}
			if (++gen_problems == n_problems) {
				mtx.unlock();
				break;
			}
			mtx.unlock();
		}
	};
	std::vector<std::thread> threads;
	n_threads = std::min(n_threads, n_problems);
	for (int i = 0; i < n_threads; ++i) {
		threads.push_back(std::thread(worker));
	}
	for (int i = 0; i < n_threads; ++i) threads[i].join();

	if (ofs) delete ofs;
	return 0;
}
