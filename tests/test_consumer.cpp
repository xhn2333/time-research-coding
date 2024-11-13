#include <gtest/gtest.h>
#include "consumer.h"

class TestConsumer : public ::testing::Test {
  protected:
	asio::io_context ioc;
	std::shared_ptr<Cache> cache = std::make_shared<Cache>();
	std::shared_ptr<Consumer> consumer = std::make_shared<Consumer>(ioc);
};

TEST_F(TestConsumer, DISABLED_TestRun) {
	consumer->bind_cache(cache);
	consumer->bind_callback_onOrderBook(
		[](const OrderBook& orderBook) {
			std::cout << "onOrderBookWS" << std::endl;
		},
		true);
	std::cout << "bind onOrderBookWS" << std::endl;
	consumer->bind_callback_onOrderBook(
		[](const OrderBook& orderBook) {
			std::cout << "onOrderBookREST" << std::endl;
		},
		false);
	std::cout << "bind onOrderBookREST" << std::endl;

	consumer->run();
	std::cout << "running" << std::endl;

	std::thread t([&]() {
		ioc.run();
	});

	for(int i = 0; i < 10; i++) {
		Msg msg = Msg::createMsg( Msg::Source::WebSocket, Msg::Type::OrderBook, std::string("test"), "2333");
		cache->encache(msg);
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));
	ioc.stop();
	t.join();
	SUCCEED();
}