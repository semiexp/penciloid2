#pragma once

#include <vector>
#include <algorithm>

namespace penciloid
{
// Abelian should be the type of an Abelian group.
// It should be equipped with binary operator+ and unary operator-.
// Also, it should suffice the axioms of Abelian groups:
// 1. for any a, a + Abelian() == Abelian() + a == a.
// 2. for any a, b and c, (a + b) + c == a + (b + c).
// 3. for any a, a + (-a) == (-a) + a == Abelian().
// 4. for any a and b, a + b == b + a.
template <typename Abelian>
class GraphSeparation
{
public:
	GraphSeparation();

	// V: number of the vertices. E: (maximum) number of the edges.
	GraphSeparation(int V, int E);

	~GraphSeparation();

	// Add a bidirectional edge between vertex <p> and <q>.
	inline void AddEdge(int p, int q);

	// Set the value of vertex <p> to <val>.
	inline void SetValue(int p, const Abelian &val);

	// Call this method before calling Separate.
	// Once it was called, further AddEdge / SetValue operations will lead to an inconsistency.
	void Construct();

	// Let S(V) be \sum_{v \in V} value[v].
	// After removing vertex <p>, the connected component which <p> belonged before will be separated into some components (let them be V1, V2, ...)
	// This method returns {S(V1), S(V2), ...} (in random order).
	// As the base type is an Abelian group, the return value is well-defined.
	std::vector<Abelian> Separate(int p);

private:
	struct Edge
	{
		Edge *next_edge;
		int destination;
		bool tree_edge;
		bool is_connective;
	};

	inline void AddEdgeDirectional(int p, int q);

	void Dfs(int p, int rt, int *idx, int *lowlink, int *idx_next);

	Edge *pool_, **graph_;
	Abelian *value_;
	int *root_;
	int n_vertex_, n_edge_, pool_last_;
};
template <typename Abelian>
GraphSeparation<Abelian>::GraphSeparation() : pool_(nullptr), graph_(nullptr), value_(nullptr), root_(nullptr), n_vertex_(0), n_edge_(0), pool_last_(0)
{
}
template <typename Abelian>
GraphSeparation<Abelian>::GraphSeparation(int V, int E) : pool_(new Edge[2 * E]), graph_(new Edge*[V]), value_(new Abelian[V]), root_(new int[V]), n_vertex_(V), n_edge_(E), pool_last_(0)
{
	for (int i = 0; i < V; ++i) {
		graph_[i] = nullptr;
	}
}
template <typename Abelian>
GraphSeparation<Abelian>::~GraphSeparation()
{
	if (pool_) delete[] pool_;
	if (graph_) delete[] graph_;
	if (value_) delete[] value_;
	if (root_) delete[] root_;
}
template <typename Abelian>
void GraphSeparation<Abelian>::AddEdge(int p, int q)
{
	AddEdgeDirectional(p, q);
	AddEdgeDirectional(q, p);
}
template <typename Abelian>
void GraphSeparation<Abelian>::SetValue(int p, const Abelian &val)
{
	value_[p] = val;
}
template <typename Abelian>
void GraphSeparation<Abelian>::AddEdgeDirectional(int p, int q)
{
	Edge *e = &(pool_[pool_last_++]);
	e->destination = q;
	e->next_edge = graph_[p];
	e->tree_edge = false;
	graph_[p] = e;
}
template <typename Abelian>
void GraphSeparation<Abelian>::Construct()
{
	int *idx = new int[n_vertex_], *lowlink = new int[n_vertex_];
	int idx_next = 0;

	for (int i = 0; i < n_vertex_; ++i) idx[i] = -1;
	for (int i = 0; i < n_vertex_; ++i) if (idx[i] == -1) {
		Dfs(i, -1, idx, lowlink, &idx_next);
	}

	delete[] idx;
	delete[] lowlink;
}
template <typename Abelian>
void GraphSeparation<Abelian>::Dfs(int p, int rt, int *idx, int *lowlink, int *idx_next)
{
	idx[p] = (*idx_next)++;
	lowlink[p] = idx[p];
	if (rt >= 0) root_[p] = root_[rt];
	else root_[p] = p;

	for (Edge *e = graph_[p]; e != nullptr; e = e->next_edge) {
		int q = e->destination;
		if (idx[q] == -1) {
			Dfs(q, p, idx, lowlink, idx_next);
			e->tree_edge = true;
			lowlink[p] = std::min(lowlink[p], lowlink[q]);
			value_[p] = value_[p] + value_[q];
			e->is_connective = (idx[p] > lowlink[q]);
		} else if (q == rt) {
			e->tree_edge = false;
			e->is_connective = false;
		} else {
			e->tree_edge = false;
			e->is_connective = false;
			lowlink[p] = std::min(lowlink[p], idx[q]);
		}
	}
}
template <typename Abelian>
std::vector<Abelian> GraphSeparation<Abelian>::Separate(int p)
{
	std::vector<Abelian> ret;
	if (root_[p] == p) {
		for (Edge *e = graph_[p]; e != nullptr; e = e->next_edge) {
			if (e->tree_edge) ret.push_back(value_[e->destination]);
		}
	} else {
		ret.push_back(value_[root_[p]] + (-value_[p]));
		for (Edge *e = graph_[p]; e != nullptr; e = e->next_edge) {
			if (e->tree_edge) {
				if (e->is_connective) ret[0] = ret[0] + value_[e->destination];
				else ret.push_back(value_[e->destination]);
			}
		}
	}
	return ret;
}
}
