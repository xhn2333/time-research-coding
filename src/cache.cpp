#include "cache.h"

Cache::Cache() {
	cache_queue_ = std::make_shared<LockFreeQueue<Msg>>(1024);
}

Cache::~Cache() {
}

size_t Cache::size() {
	return cache_queue_->size();
}

bool Cache::encache(const Msg& msg) {
	if (cache_hash_.find(msg.data->hash()) != cache_hash_.end()) {
		if (cache_hash_[msg.data->hash()] == true) {
			return false;
		}
	}
	cache_hash_[msg.data->hash()] = true;
	return cache_queue_->enqueue(msg);
}

Msg Cache::decache() {
	Msg msg;
	cache_queue_->dequeue(msg);
	cache_hash_[msg.data->hash()] = false;
	return msg;
}