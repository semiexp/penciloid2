#pragma once

#include "auto_array.h"

namespace penciloid
{
class SearchQueue
{
public:
	SearchQueue() : queue_(), is_stored_(), size_(0), top_(-1), end_(-1) {}
	SearchQueue(int size) : queue_(size + 1), is_stored_(size), size_(size + 1), top_(-1), end_(-1) {
		std::fill(is_stored_.begin(), is_stored_.end(), false);
	}
	SearchQueue(const SearchQueue &other) : queue_(other.queue_), is_stored_(other.is_stored_), size_(other.size_), top_(-1), end_(-1) {}
	SearchQueue(SearchQueue &&other) : queue_(std::move(other.queue_)), is_stored_(std::move(other.is_stored_)), size_(other.size_), top_(-1), end_(-1) {}

	SearchQueue &operator=(const SearchQueue &other) {
		queue_ = other.queue_;
		is_stored_ = other.is_stored_;
		size_ = other.size_;
		top_ = other.top_;
		end_ = other.end_;
		return *this;
	}
	SearchQueue &operator=(SearchQueue &&other) {
		queue_ = std::move(other.queue_);
		is_stored_ = std::move(other.is_stored_);
		size_ = other.size_;
		top_ = other.top_;
		end_ = other.end_;
		return *this;
	}
	void Activate() {
		top_ = end_ = 0;
	}
	void Deactivate() {
		while (!IsEmpty()) Pop();
		top_ = end_ = -1;
	}
	bool IsActive() const {
		return top_ != -1;
	}
	void Push(int e) {
		if (!is_stored_[e]) {
			is_stored_[e] = true;
			queue_[end_++] = e;
			if (end_ == size_) end_ = 0;
		}
	}
	int Pop() {
		int ret = queue_[top_++];
		is_stored_[ret] = false;
		if (top_ == size_) top_ = 0;
		return ret;
	}
	bool IsEmpty() const {
		return top_ == end_;
	}
private:
	AutoArray<int> queue_;
	AutoArray<bool> is_stored_;
	int size_, top_, end_;
};
}
