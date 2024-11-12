#include <gtest/gtest.h>
#include "websocket.h"

class WebSocketHandlerTest : public ::testing::Test {
  protected:
	boost::asio::io_context ioc;
	ssl::context ssl_context{ssl::context::sslv23};
	std::shared_ptr<WebSocketHandler> wsHandler;

	void SetUp() override {
		wsHandler = std::make_shared<WebSocketHandler>(ioc);
	}
};

TEST_F(WebSocketHandlerTest, TestResolve) {
	// Set endpoint parameters
	wsHandler->setEndpoint("fstream.binance.com", "443", "/ws/btcusdt@depth5@100ms");
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
