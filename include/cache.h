#ifndef CACHE_H
#define CACHE_H

#include <mutex>
#include <unordered_map>
#include "common.h"
#include "utils/logger.h"
#include "utils/queue.h"

class Cache {
  public:
	Cache(int capacity = 1024);
	~Cache();

	bool encache(const Msg& msg);
	size_t size();
	bool decache(Msg& msg);

  private:
	std::mutex mutex_;
	uint64_t U_;
	std::shared_ptr<spdlog::logger> logger_;
	// std::unordered_map<int64_t, bool> cache_hash_;
	std::shared_ptr<LockFreeQueue<Msg>> cache_queue_;
};

#endif	// CACHE_H