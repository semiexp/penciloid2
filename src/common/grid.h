#pragma once

#include "type.h"

namespace penciloid
{
template <class T>
class Grid
{
public:
	Grid() : data_(nullptr), height_(0), width_(0) {}
	Grid(Y height, X width);

	Grid(const Grid &other);
	Grid(Grid &&other);
	Grid &operator=(const Grid &other);
	Grid &operator=(Grid &&other);

	~Grid() { if (data_) delete[] data_; }

	Y height() const { return height_; }
	X width() const { return width_; }

protected:
	T &at(Position pos) { return data_[GetIndex(pos)]; }
	const T &at(Position pos) const { return data_[GetIndex(pos)]; }

private:
	unsigned int NumberOfCells() const { return static_cast<int>(height_)* static_cast<int>(width_); }
	unsigned int GetIndex(Position pos) const { return static_cast<int>(pos.y) * static_cast<int>(width_) + static_cast<int>(pos.x); }
	T *data_;
	Y height_;
	X width_;
};
template<class T>
Grid<T>::Grid(Y height, X width) : data_(nullptr), height_(height), width_(width)
{
	data_ = new T[NumberOfCells()];
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