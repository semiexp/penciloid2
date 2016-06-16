#include "test_graph_separation.h"
#include "test.h"

#include <vector>
#include <algorithm>
#include <cassert>

#include "../common/graph_separation.h"
#include "../common/union_find.h"

namespace penciloid
{
namespace test
{
void RunAllGraphSeparationTest()
{
	GraphSeparationTest();
}
void GraphSeparationRunTestCase(int n_vertex, const std::vector<std::pair<int, int> > &edges)
{
	GraphSeparation<int> graph(n_vertex, edges.size());
	for (int i = 0; i < n_vertex; ++i) graph.SetValue(i, 1 << i);
	for (auto &e : edges) graph.AddEdge(e.first, e.second);
	graph.Construct();
	
	UnionFind uf_base(n_vertex);
	for (auto &e : edges) uf_base.Join(e.first, e.second);

	for (int p = 0; p < n_vertex; ++p) {
		UnionFind uf(n_vertex);
		for (auto &e : edges) {
			if (e.first != p && e.second != p) uf.Join(e.first, e.second);
		}
		std::vector<int> expected;
		for (int i = 0; i < n_vertex; ++i) {
			if (i != p && uf.Root(i) == i && uf_base.Root(i) == uf_base.Root(p)) {
				int val = 0;
				for (int j = 0; j < n_vertex; ++j) {
					if (uf.Root(j) == i) val += 1 << j;
				}
				expected.push_back(val);
			}
		}
		std::vector<int> result = graph.Separate(p);
		sort(expected.begin(), expected.end());
		sort(result.begin(), result.end());
		assert(expected == result);
	}
}
void GraphSeparationTest()
{
	std::vector<std::pair<int, int> > ordinary_graph = {
		{ 0, 1 },
		{ 0, 2 },
		{ 1, 2 },
		{ 1, 4 },
		{ 2, 3 },
		{ 2, 5 }
	};
	// Ordinary graph
	GraphSeparationRunTestCase(6, ordinary_graph);
	
	// Multiple edges
	for (int i = 0; i < ordinary_graph.size(); ++i) {
		auto multiple_edges = ordinary_graph;
		multiple_edges.push_back(ordinary_graph[i]);
		GraphSeparationRunTestCase(6, multiple_edges);
	}

	// Self edges
	for (int i = 0; i < 6; ++i) {
		auto self_edges = ordinary_graph;
		self_edges.push_back({ i, i });
		GraphSeparationRunTestCase(6, self_edges);
	}

	// Tree
	GraphSeparationRunTestCase(6, {
		{ 0, 1 },
		{ 1, 2 },
		{ 0, 3 },
		{ 3, 5 },
		{ 5, 4 }
	});

	// Disconnected
	GraphSeparationRunTestCase(8, {
		{ 0, 1 },
		{ 1, 2 },
		{ 1, 3 },
		{ 2, 3 },
		{ 4, 5 },
		{ 4, 7 }
	});

	// Empty graph
	GraphSeparationRunTestCase(5, {
	});
}
}
}