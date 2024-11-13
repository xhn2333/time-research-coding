#include <gtest/gtest.h>

#include "connection.h"

class ConnectionManagerTest : public ::testing::Test {
  protected:
	asio::io_context ioc_;
	std::shared_ptr<ConnectionManager> connection_manager_;
	std::shared_ptr<Cache> cache_;

	void SetUp() override {
		connection_manager_ = std::make_shared<ConnectionManager>(
			"btcusdt",
			ioc_,
			"fapi.binance.com", "443", "/fapi/v1/depth?symbol=btcusdt&limit=5",
			"fstream.binance.com", "443", "/ws/btcusdt@depth5",
			1, 0);
		cache_ = std::make_shared<Cache>();
		connection_manager_->bind_cache(cache_);
	}
};

TEST_F(ConnectionManagerTest, SetConnectionManager) {
	ASSERT_NE(connection_manager_, nullptr);
	ASSERT_NE(cache_, nullptr);
	connection_manager_->run();
	std::thread t1([&]() {
		ioc_.run();
	});

	std::thread t2([&]() {
		while (true) {
			Msg msg;
			bool flag = cache_->decache(msg);
			if (flag)
				std::cout << "Decache: " << std::dynamic_pointer_cast<OrderBook>(msg.data)->data["T"] << std::endl;
		}
	});

	std::this_thread::sleep_for(std::chrono::seconds(30));

	ioc_.stop();
	t1.join();
	SUCCEED();
}