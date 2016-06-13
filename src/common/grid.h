#pragma once

#include "type.h"
#include <algorithm>

namespace penciloid
{
template <class T>
class Grid
{
public:
	Grid() : data_(nullptr), height_(0), width_(0) {}
	Grid(Y height, X width, const T &init_value = T());

	Grid(const Grid &other);
	Grid(Grid &&other);
	Grid &operator=(const Grid &other);
	Grid &operator=(Grid &&other);

	~Grid() { if (data_) delete[] data_; }

	Y height() const { return height_; }
	X width() const { return width_; }

	T &at(CellPosition pos) { return data_[GetIndex(pos)]; }
	const T &at(CellPosition pos) const { return data_[GetIndex(pos)]; }
	T &at(int pos) { return data_[pos]; }
	const T &at(int pos) const { return data_[pos]; }

	// Returns whether <pos> is within the range of this grid.
	bool IsPositionOnGrid(CellPosition pos) const { return 0 <= pos.y && pos.y < height_ && 0 <= pos.x && pos.x < width_; }

	unsigned int NumberOfCells() const { return static_cast<int>(height_) * static_cast<int>(width_); }
	unsigned int GetIndex(CellPosition pos) const { return static_cast<int>(pos.y) * static_cast<int>(width_) + static_cast<int>(pos.x); }
	CellPosition AsPosition(unsigned int index) const {
		return CellPosition(Y(index / static_cast<int>(width_)), X(index % static_cast<int>(width_)));
	}
private:
	T *data_;
	Y height_;
	X width_;
};
template<class T>
Grid<T>::Grid(Y height, X width, const T &init_value) : data_(nullptr), height_(height), width_(width)
{
	unsigned int n_cells = NumberOfCells();
	data_ = new T[n_cells];
	std::fill(data_, data_ + n_cells, init_value);
}
template<class T>
Grid<T>::Grid(const Grid<T> &other) : data_(nullptr), height_(other.height_), width_(other.width_)
{
	int cell_count = NumberOfCells();
	data_ = new T[cell_count];
	for (int i = 0; i < cell_count; ++i) data_[i] = other.data_[i];
}
template<class T>
Grid<T>::Grid(Grid<T> &&other) : data_(other.data_), height_(other.height_), width_(other.width_)
{
	other.data_ = nullptr;
}
template<class T>
Grid<T> &Grid<T>::operator=(const Grid<T> &other)
{
	height_ = other.height_;
	width_ = other.width_;

	if (data_) delete[] data_;

	int cell_count = NumberOfCells();
	data_ = new T[cell_count];
	for (int i = 0; i < cell_count; ++i) data_[i] = other.data_[i];

	return *this;
}
template<class T>
Grid<T> &Grid<T>::operator=(Grid<T> &&other)
{
	height_ = other.height_;
	width_ = other.width_;

	if (data_) delete[] data_;
	data_ = other.data_;
	other.data_ = nullptr;

	return *this;
}
}
