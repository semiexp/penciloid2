#pragma once

#include "type.h"
#include "../common/auto_array.h"
#include <algorithm>

#include <cassert>

namespace penciloid
{
template <class T>
class Grid
{
public:
	Grid() : data_(), height_(0), width_(0) {}
	Grid(Y height, X width, const T &init_value = T());

	Grid(const Grid &other);
	Grid(Grid &&other);
	Grid &operator=(const Grid &other);
	Grid &operator=(Grid &&other);

	~Grid() { }

	Y height() const { return height_; }
	X width() const { return width_; }

	T &operator()(CellPosition pos) { return at(pos); }
	const T &operator()(CellPosition pos) const { return at(pos); }
	T &operator()(Y y, X x) { return at(CellPosition(y, x)); }
	const T &operator()(Y y, X x) const { return at(CellPosition(y, x)); }

	T &at(int pos) {
		assert(0 <= pos && pos < (int)height_ * (int)width_);
		return data_[pos];
	}
	const T &at(int pos) const {
		assert(0 <= pos && pos < (int)height_ * (int)width_);
		return data_[pos];
	}

	// Returns whether <pos> is within the range of this grid.
	bool IsPositionOnGrid(CellPosition pos) const { return 0 <= pos.y && pos.y < height_ && 0 <= pos.x && pos.x < width_; }

	unsigned int NumberOfCells() const { return static_cast<int>(height_) * static_cast<int>(width_); }
	unsigned int GetIndex(CellPosition pos) const { return static_cast<int>(pos.y) * static_cast<int>(width_) + static_cast<int>(pos.x); }
	CellPosition AsPosition(unsigned int index) const {
		return CellPosition(Y(index / static_cast<int>(width_)), X(index % static_cast<int>(width_)));
	}
private:
	T &at(CellPosition pos) { return data_[GetIndex(pos)]; }
	const T &at(CellPosition pos) const { return data_[GetIndex(pos)]; }

	AutoArray<T> data_;
	Y height_;
	X width_;
};
template<class T>
Grid<T>::Grid(Y height, X width, const T &init_value) : data_(static_cast<int>(height) * static_cast<int>(width)), height_(height), width_(width)
{
	std::fill(data_.begin(), data_.end(), init_value);
}
template<class T>
Grid<T>::Grid(const Grid<T> &other) : data_(other.data_), height_(other.height_), width_(other.width_)
{
}
template<class T>
Grid<T>::Grid(Grid<T> &&other) : height_(other.height_), width_(other.width_)
{
	data_ = std::move(other.data_);
}
template<class T>
Grid<T> &Grid<T>::operator=(const Grid<T> &other)
{
	data_ = other.data_;
	height_ = other.height_;
	width_ = other.width_;

	return *this;
}
template<class T>
Grid<T> &Grid<T>::operator=(Grid<T> &&other)
{
	data_ = std::move(other.data_);
	height_ = other.height_;
	width_ = other.width_;

	return *this;
}
}
