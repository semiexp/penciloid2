#pragma once

namespace penciloid
{
template <class T, int SIZE>
class MiniVector
{
public:
	MiniVector() : index_(0) {}

	int size() const { return index_; }
	const T& operator[](int i) const {
		return data_[i];
	}
	T& operator[](int i) {
		return data_[i];
	}
	void push_back(const T& v) {
		data_[index_++] = v;
	}
	const T* begin() const {
		return &(data_[0]);
	}
	T* begin() {
		return &(data_[0]);
	}
	const T* end() const {
		return &(data_[index_]);
	}
	T* end() {
		return &(data_[index_]);
	}
private:
	T data_[SIZE];
	int index_;
};
}
