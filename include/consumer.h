#ifndef CONSUMER_H
#define CONSUMER_H

#include "cache.h"
#include "common.h"

/**
 * @brief Consumer class
 * This class is responsible for consuming messages from the cache.
 * @details
 * The Consumer class is responsible for consuming messages from the cache. It has a run() function that is called to start consuming messages.
 */
class Consumer {
  public:
	Consumer() = default;
	~Consumer() = default;

	void run();

  private:
	asio::io_context io_context_;
	std::shared_ptr<Cache> buffer_queue_;
};

#endif	// CONSUMER_H