#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include "restapi.h"

using boost::asio::ip::tcp;

class REATAPITest : public ::testing::Test {
  protected:
	asio::io_context ioc;
	std::shared_ptr<RestApiHandler> restHandler;
	std::shared_ptr<Cache> cache_;

	void SetUp() override {
		restHandler = std::make_shared<RestApiHandler>("btcusdt", ioc);
		cache_ = std::make_shared<Cache>();
	}
};

TEST_F(REATAPITest, DISABLED_TestREST) {
	restHandler->setEndpoint("fapi.binance.com",
							 "443",
							 "/fapi/v1/depth?symbol=BTCUSDT&limit=5");
	restHandler->setPollingInterval(std::chrono::seconds(1));
	restHandler->run();
	restHandler->bind_cache(cache_);
	// 在单独的线程中运行 io_context，以便非阻塞执行
	std::thread t([&]() {
		ioc.run();
	});

	std::this_thread::sleep_for(std::chrono::seconds(30));
	ioc.stop();
	// restHandler->setPollingInterval(std::chrono::seconds(-1));
	t.join();

	SUCCEED();
}
