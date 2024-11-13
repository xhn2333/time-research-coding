#include "cache.h"

Cache::Cache(int capacity) {
	logger_ = Logger::getLogger("Cache");
	cache_queue_ = std::make_shared<LockFreeQueue<Msg>>(capacity);
}

Cache::~Cache() {
}

size_t Cache::size() {
	return cache_queue_->size();
}

bool Cache::encache(const Msg& msg) {
	auto data_value = std::dynamic_pointer_cast<OrderBook>(msg.data)->data;
	if (msg.source == Msg::Source::WebSocket) {
		uint64_t U = data_value["U"];
		if (U <= U_) {
			return false;
		}
		U_ = U;
	}
	return cache_queue_->enqueue(msg);
}

bool Cache::decache(Msg& msg) {
	return cache_queue_->dequeue(msg);
}