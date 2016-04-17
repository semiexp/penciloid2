#include "sl_evaluator_training_set.h"

#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>

#include "sl_evaluator.h"

namespace penciloid
{
namespace slitherlink
{
std::vector<double> EvaluatorTrainingSet::ComputeDifficultyAll(EvaluatorParameter param, int n_threads)
{
	std::vector<std::pair<Problem*, int> > entries;
	std::vector<double> score_result;

	for (int i = 0; i < problem_set_.size(); ++i) {
		score_result.push_back(-1.0);
		if (evaluability_[i] != kUnevaluable) {
			entries.push_back({ &(problem_set_[i]), i });
		}
	}

	std::sort(entries.begin(), entries.end(), [](const std::pair<Problem*, int> &e1, const std::pair<Problem*, int> &e2) {
		int size_e1 = static_cast<int>(e1.first->height()) * static_cast<int>(e1.first->width());
		int size_e2 = static_cast<int>(e2.first->height()) * static_cast<int>(e2.first->width());
		return size_e1 > size_e2;
	});

	std::mutex mtx; int index = 0;
	auto worker = [&param, &entries, &mtx, &index, &score_result]() {
		for (;;) {
			mtx.lock();
			int current_index = index++;
			mtx.unlock();
			if (current_index >= entries.size()) break;

			Evaluator e(*(entries[current_index].first));
			e.SetParameter(param);
			double score = e.Evaluate();

			mtx.lock();
			score_result[entries[current_index].second] = score;
			mtx.unlock();
		}
	};

	std::vector<std::thread> workers(n_threads);
	for (int i = 0; i < n_threads; ++i) {
		workers[i] = std::thread(worker);
	}
	for (int i = 0; i < n_threads; ++i) workers[i].join();

	for (int i = 0; i < problem_set_.size(); ++i) {
		if (score_result[i] < 0) evaluability_[i] = kUnevaluable;
		else evaluability_[i] = kEvaluable;
	}
	return score_result;
}
}
}