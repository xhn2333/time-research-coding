#ifndef CACHE_H
#define CACHE_H

#include <unordered_map>
#include "common.h"
#include "utils/queue.h"

class Cache {
  public:
	Cache();
	~Cache();

	bool encache(const Msg& msg);
	size_t size();
	Msg decache();

  private:
	std::unordered_map<int64_t, bool> cache_hash_;
	std::shared_ptr<LockFreeQueue<Msg>> cache_queue_;
};

#endif	// CACHE_H