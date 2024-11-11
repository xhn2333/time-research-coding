#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include "restapi.h"

using boost::asio::ip::tcp;

TEST(TestWS, FailureCheck) {
	asio::io_context ioc;

	auto restHandler = std::make_shared<RestApiHandler>(ioc);

	restHandler->setEndpoint("fapi.binance.com",
							 "443",
							 "/fapi/v1/depth?symbol=BTCUSDT&limit=5");
	restHandler->setPollingInterval(std::chrono::seconds(1));
	restHandler->run();

	// 在单独的线程中运行 io_context，以便非阻塞执行
	std::thread t([&ioc]() {
		ioc.run();
	});

	t.join();
}
