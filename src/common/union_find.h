#pragma once

namespace penciloid
{
class UnionFind
{
public:
	UnionFind() : parent_(nullptr) {}
	UnionFind(int size) : parent_(new int[size]) { for (int i = 0; i < size; ++i) parent_[i] = -1; }

	UnionFind(const UnionFind &) = delete;
	UnionFind(UnionFind &&) = delete;
	UnionFind &operator=(const UnionFind &) = delete;
	UnionFind &operator=(UnionFind &&) = delete;

	~UnionFind() { if (parent_) delete[] parent_; }

	// Returns the index of the leader of the union which p belongs to.
	int Root(int p) { return parent_[p] < 0 ? p : (parent_[p] = Root(parent_[p])); }
	
	// Returns the size of the union which p belongs to.
	int UnionSize(int p) { return -parent_[Root(p)]; }

	// Returns true if p and q were initially in different unions.
	bool Join(int p, int q) {
		p = Root(p);
		q = Root(q);
		if (p == q) return false;

		parent_[p] += parent_[q];
		parent_[q] = p;
		return true;
	}

private:
	int *parent_;
};
}
