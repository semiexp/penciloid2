#include "sl_evaluator_trainer.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <random>
#include <iostream>
#include <string>
#include <ctime>

namespace
{
void EvaluateWorker(std::vector<penciloid::slitherlink::Problem*> &problems, penciloid::slitherlink::EvaluatorParameter &param, std::vector<double> *scores, int *index, std::mutex *mtx)
{
	for (;;) {
		mtx->lock();
		if (*index == problems.size()) {
			mtx->unlock();
			break;
		}
		int current_index = (*index)++;
		mtx->unlock();

		penciloid::slitherlink::Problem *problem = problems[current_index];
		penciloid::slitherlink::Evaluator e(*problem);
		e.SetParameter(param);
		double score = e.Evaluate();

		mtx->lock();
		(*scores)[current_index] = score;
		mtx->unlock();
	}
}
double correl(const std::vector<double> &xs, const std::vector<double> &ys)
{
	double x_sum = 0, y_sum = 0;
	for (double x : xs) x_sum += x;
	for (double y : ys) y_sum += y;
	double x_ave = x_sum / xs.size(), y_ave = y_sum / ys.size();

	double var_x = 0, var_y = 0, covar = 0;
	for (int i = 0; i < xs.size(); ++i) {
		covar += (xs[i] - x_ave) * (ys[i] - y_ave);
		var_x += (xs[i] - x_ave) * (xs[i] - x_ave);
		var_y += (ys[i] - y_ave) * (ys[i] - y_ave);
	}
	return covar / sqrt(var_x * var_y);
}
}

namespace penciloid
{
namespace slitherlink
{
void EvaluatorTrainer::LoadProblemSetFromFile(std::istream &is)
{
	std::vector<Problem> problem_set;
	int last_h = -2, last_w = -2;
	for (;;) {
		int h = -1, w = -1;
		is >> h >> w;
		if (h == -1) break;

		Problem problem{ Y(h), X(w) };
		for (int i = 0; i < h; ++i) {
			std::string line;
			is >> line;
			for (int j = 0; j < w; ++j) {
				if ('0' <= line[j] && line[j] <= '3') {
					problem.SetClue(CellPosition(Y(i), X(j)), Clue(line[j] - '0'));
				}
			}
		}
		if ((last_h != h || last_w != w) && last_h != -2) {
			AddProblemSet(problem_set);
			problem_set.clear();
		}
		problem_set.push_back(problem);
		last_h = h;
		last_w = w;
	}
	if (!problem_set.empty()) AddProblemSet(problem_set);
}
void EvaluatorTrainer::Train()
{
	double temperature = 0.001;
	double technique_step = 0.1;
	double technique_min = 1.0;
	double technique_max = 5.0;
	const double EPS = 1e-8;

	std::random_device rnddev;
	std::mt19937 rnd(rnddev());
	std::uniform_real_distribution<double> probability_gen(0.0, 1.0);

	fprintf(stderr, "initial scoring\n");
	fflush(stderr);
	time_t initial_start = clock();
	double current_score = ComputeScore();
	time_t initial_end = clock();
	fprintf(stderr, "initial scoring ended (%.3f[s])\n", (double)(initial_end - initial_start) / CLOCKS_PER_SEC);
	for (int i = 0; i < 300; ++i) {
		std::vector<int> cand;
		for (int j = 0; j < EvaluatorParameter::kNumberOfEffectiveParameters; ++j) {
			if (_param[j] - technique_step >= technique_min - EPS) {
				cand.push_back(~j);
			}
			if (_param[j] + technique_step <= technique_max - EPS) {
				cand.push_back(j);
			}
		}
		std::shuffle(cand.begin(), cand.end(), rnd);

		fprintf(stderr, "start step #%d\n", i);
		fflush(stderr);
		for (int v : cand) {
			if (v >= 0) _param[v] += technique_step;
			else _param[~v] -= technique_step;

			double next_score = ComputeScore();
			fprintf(stderr, "next_score: %f\n", next_score);
			if (current_score < next_score) {
				current_score = next_score;
				goto nex;
			}
			if (probability_gen(rnd) < exp((next_score - current_score) / temperature)) {
				current_score = next_score;
				goto nex;
			}
			if (v >= 0) _param[v] -= technique_step;
			else _param[~v] += technique_step;
		}

	nex:
		fprintf(stderr, "after step #%d:\n", i);
		fprintf(stderr, "  score: %f\n", current_score);
		for (int j = 0; j < EvaluatorParameter::kNumberOfEffectiveParameters; ++j) fprintf(stderr, "  param[%d]: %f\n", j, _param[j]);
		fprintf(stderr, "\n");
		fflush(stderr);

		temperature *= 0.99;
	}
}
double EvaluatorTrainer::ComputeScore()
{
	// reorder problems
	struct problem_entry
	{
		int set_id;
		int problem_id; // In the problem set [set_id]
		Problem *problem;
	};
	
	std::vector<problem_entry> entries;
	for (int i = 0; i < _problem_sets.size(); ++i) {
		for (int j = 0; j < _problem_sets[i].size(); ++j) {
			if (_is_unsolvable.empty() || !_is_unsolvable[i][j]) {
				entries.push_back({ i, j, &(_problem_sets[i][j]) });
			}
		}
	}
	// Sort by the decreasing order of (height * width)
	std::sort(entries.begin(), entries.end(), [](const problem_entry &e1, const problem_entry &e2) {
		int size1 = static_cast<int>(e1.problem->height()) * static_cast<int>(e1.problem->width());
		int size2 = static_cast<int>(e2.problem->height()) * static_cast<int>(e2.problem->width());
		return size1 > size2;
	});

	std::vector<Problem*> worker_problems;
	std::vector<double> worker_scores;
	for (int i = 0; i < entries.size(); ++i) {
		worker_problems.push_back(entries[i].problem);
		worker_scores.push_back(Evaluator::kScoreImpossible);
	}

	int worker_index = 0;
	std::mutex worker_mtx;

	std::vector<std::thread> workers(_num_threads);
	for (int i = 0; i < _num_threads; ++i) {
		workers[i] = std::thread(EvaluateWorker, std::ref(worker_problems), std::ref(_param), &worker_scores, &worker_index, &worker_mtx);
	}
	for (int i = 0; i < _num_threads; ++i) workers[i].join();

	std::vector<std::vector<double> > set_scores;
	for (int i = 0; i < _problem_sets.size(); ++i) {
		set_scores.push_back(std::vector<double>(_problem_sets[i].size(), Evaluator::kScoreImpossible));
	}
	for (int i = 0; i < entries.size(); ++i) {
		set_scores[entries[i].set_id][entries[i].problem_id] = worker_scores[i];
	}

	if (_is_unsolvable.empty()) {
		for (int i = 0; i < _problem_sets.size(); ++i) {
			std::vector<bool> s(_problem_sets[i].size(), false);
			for (int j = 0; j < _problem_sets[i].size(); ++j) {
				if (set_scores[i][j] <= 0.0) s[j] = true;
			}
			_is_unsolvable.push_back(s);
		}
	}

	double ret = 0, ret_weight = 0;
	for (int i = 0; i < _problem_sets.size(); ++i) {
		std::vector<double> correl_x, correl_y;
		for (int j = 0; j < _problem_sets[i].size(); ++j) {
			if (set_scores[i][j] > 0.0) {
				correl_x.push_back(static_cast<double>(j));
				correl_y.push_back(set_scores[i][j]);
			}
		}

		ret += correl(correl_x, correl_y) * correl_x.size();
		ret_weight += correl_x.size();
	}
	return ret / ret_weight;
}
}
}
