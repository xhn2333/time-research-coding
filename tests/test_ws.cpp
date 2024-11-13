#include <gtest/gtest.h>
#include "websocket.h"

class WebSocketHandlerTest : public ::testing::Test {
  protected:
	boost::asio::io_context ioc;
	std::shared_ptr<WebSocketHandler> wsHandler;
	std::shared_ptr<Cache> cache_;
	void SetUp() override {
		wsHandler = std::make_shared<WebSocketHandler>("btcusdt", ioc);
		cache_ = std::make_shared<Cache>();
	}
};

TEST_F(WebSocketHandlerTest, DISABLED_TestResolve) {
	// Set endpoint parameters
	wsHandler->setEndpoint("fstream.binance.com", "443", "/ws/btcusdt@depth5@100ms");
	wsHandler->bind_cache(cache_);
	wsHandler->run();

	std::thread t([&]() {
		ioc.run();
	});

	std::this_thread::sleep_for(std::chrono::seconds(30));

	ioc.stop();
	if (t.joinable()) {
		t.join();
	}

	SUCCEED();
}
