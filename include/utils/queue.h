#pragma once

#include <atomic>
#include <vector>
#include <memory>

template<typename T>
class LockFreeQueue {
public:
	LockFreeQueue(size_t capacity) : capacity_(capacity), head_(0), tail_(0) {
		// Ensure capacity is a power of 2 for bitwise operations
		if ((capacity & (capacity - 1)) != 0) {
			throw std::invalid_argument("Capacity must be a power of 2");
		}
		buffer_.resize(capacity);
	}

	bool enqueue(const T& item) {
		size_t tail = tail_.load(std::memory_order_relaxed);
		size_t next_tail = (tail + 1) & (capacity_ - 1);
		if (next_tail == head_.load(std::memory_order_acquire)) {
			return false; // Queue is full
		}
		buffer_[tail] = item;
		tail_.store(next_tail, std::memory_order_release);
		return true;
	}

	bool dequeue(T& item) {
		size_t head = head_.load(std::memory_order_relaxed);
		if (head == tail_.load(std::memory_order_acquire)) {
			return false; // Queue is empty
		}
		item = buffer_[head];
		head_.store((head + 1) & (capacity_ - 1), std::memory_order_release);
		return true;
	}

	size_t size() const {
		size_t head = head_.load(std::memory_order_relaxed);
		size_t tail = tail_.load(std::memory_order_relaxed);
		return (tail - head) & (capacity_ - 1);
	}

private:
	std::vector<T> buffer_;
	const size_t capacity_;
	std::atomic<size_t> head_;
	std::atomic<size_t> tail_;
};