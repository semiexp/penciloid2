#include "test_graph_separation.h"
#include "test.h"

#include <vector>
#include <cassert>

#include "../common/graph_separation.h"

namespace penciloid
{
namespace test
{
void RunAllGraphSeparationTest()
{
	GraphSeparationTest1();
}
void GraphSeparationTest1()
{
	int v = 6, e = 6;
	GraphSeparation<int> graph(v, e);
	for (int i = 0; i < v; ++i) graph.SetValue(i, 1 << i);

	graph.AddEdge(0, 1);
	graph.AddEdge(0, 2);
	graph.AddEdge(1, 2);
	graph.AddEdge(1, 4);
	graph.AddEdge(2, 3);
	graph.AddEdge(2, 5);

	graph.Construct();

	std::vector<int> expected[] = {
		{0b111110},
		{0b010000, 0b101101},
		{0b001000, 0b010011, 0b100000},
		{0b110111},
		{0b101111},
		{0b011111}
	};
	for (int i = 0; i < v; ++i) {
		std::vector<int> ans = graph.Separate(i);
		sort(ans.begin(), ans.end());
		assert(ans == expected[i]);
	}
}
}
}