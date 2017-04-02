#pragma once

#include <algorithm>

namespace penciloid
{
template <typename T>
class AutoArray
{
public:
	AutoArray() : data_(nullptr), length_(0) {}
	AutoArray(int length) :
		data_(new T[length]),
		length_(length) {}
	AutoArray(const AutoArray<T> &other) :
		data_(new T[other.length_]),
		length_(other.length_) {
		std::copy(other.begin(), other.end(), begin());
	}
	AutoArray(AutoArray<T> &&other) :
		data_(other.data_),
		length_(other.length_) {
		other.data_ = nullptr;
		other.length_ = 0;
	}
	~AutoArray() { delete[] data_; }

	AutoArray<T> &operator=(const AutoArray<T> &other) {
		delete[] data_;
		data_ = new T[other.length_];
		length_ = other.length_;
		std::copy(other.begin(), other.end(), begin());

		return *this;
	}
	AutoArray<T> &operator=(AutoArray<T> &&other) {
		delete[] data_;
		data_ = other.data_;
		length_ = other.length_;
		other.data_ = nullptr;
		other.length_ = 0;

		return *this;
	}

	T &operator[](int i) { return data_[i]; }
	const T &operator[](int i) const { return data_[i]; }

	T *begin() { return data_; }
	T *end() { return &(data_[length_]); }
	const T *begin() const { return data_; }
	const T *end() const { return &(data_[length_]); }

private:
	T *data_;
	unsigned int length_;
};
}
