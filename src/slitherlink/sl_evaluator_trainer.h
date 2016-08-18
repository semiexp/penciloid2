#pragma once 

#include <vector>
#include <iostream>
#include <ctime>
#include <cstdio>

#include "sl_evaluator_training_set.h"
#include "sl_evaluator.h"
#include "sl_evaluator_parameter.h"
#include "sl_problem.h"

namespace penciloid
{
namespace slitherlink
{
template <class ScoreCalculator>
EvaluatorParameter TrainEvaluator(EvaluatorTrainingSet &training_set, const std::vector<double> &reference_difficulty, EvaluatorParameter param, int n_threads, ScoreCalculator sc)
{
	double temperature = 0.0002;
	double technique_step = 0.05;
	double technique_min = 0.1;
	double technique_max = 5.0;
	const double EPS = 1e-8;

	std::random_device rnddev;
	std::mt19937 rnd(rnddev());
	std::uniform_real_distribution<double> probability_gen(0.0, 1.0);

	fprintf(stderr, "initial scoring\n");
	fflush(stderr);
	time_t initial_start = clock();
	std::vector<double> computed_difficulty = training_set.ComputeDifficultyAll(param, n_threads);
	double current_score = sc(reference_difficulty, computed_difficulty);
	time_t initial_end = clock();
	fprintf(stderr, "initial scoring ended (%.3f[s])\n", (double)(initial_end - initial_start) / CLOCKS_PER_SEC);
	fprintf(stderr, "score: %f\n", current_score);
	for (int i = 0; i < 300; ++i) {
		std::vector<int> cand;
		for (int j = 0; j < EvaluatorParameter::kNumberOfEffectiveParameters; ++j) {
			if (j == EvaluatorParameter::kIndexLocalityDistance) continue;
			if (param[j] - technique_step >= technique_min - EPS) {
				cand.push_back(~j);
			}
			if (param[j] + technique_step <= technique_max - EPS) {
				cand.push_back(j);
			}
		}
		std::shuffle(cand.begin(), cand.end(), rnd);

		fprintf(stderr, "start step #%d\n", i);
		fflush(stderr);
		for (int v : cand) {
			if (v >= 0) {
				param[v] += technique_step;
			} else {
				param[~v] -= technique_step;
			}

			std::vector<double> computed_difficulty = training_set.ComputeDifficultyAll(param, n_threads);
			double next_score = sc(reference_difficulty, computed_difficulty);

			fprintf(stderr, "next_score: %f\n", next_score);
			if (current_score < next_score) {
				current_score = next_score;
				goto nex;
			}
			if (probability_gen(rnd) < exp((next_score - current_score) / temperature)) {
				current_score = next_score;
				goto nex;
			}
			if (v >= 0) param[v] -= technique_step;
			else param[~v] += technique_step;
		}

	nex:
		fprintf(stderr, "after step #%d:\n", i);
		fprintf(stderr, "  score: %f\n", current_score);
		for (int j = 0; j < EvaluatorParameter::kNumberOfEffectiveParameters; ++j) fprintf(stderr, "  param[%d]: %f\n", j, param[j]);
		fprintf(stderr, "\n");
		fflush(stderr);

		temperature *= 0.99;
	}
	return param;
}
}
}