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
	asio::io_context& io_context_;

  public:
	Consumer(asio::io_context& ioc)
		: io_context_(ioc) {
		  };
	~Consumer() = default;

	void bind_cache(std::shared_ptr<Cache> buffer_queue) {
		buffer_queue_ = buffer_queue;
	}

	void bind_callback_onOrderBook(const std::function<void(const OrderBook&)>& callback, bool is_ws) {
		if (is_ws) {
			callback_onOrderBookWS = callback;
		} else {
			callback_onOrderBookREST = callback;
		}
	}

	void run();

  private:
	void onData(const Msg& msg);
	std::function<void(const OrderBook&)> callback_onOrderBookWS;
	std::function<void(const OrderBook&)> callback_onOrderBookREST;

	std::shared_ptr<Cache> buffer_queue_;
};

#endif	// CONSUMER_H