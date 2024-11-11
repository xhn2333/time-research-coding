#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include "restapi.h"

TEST(TestWS, FailureCheck) {
	asio::io_context ioc;
	ssl::context ctx(ssl::context::sslv23);

	// 使用 std::make_shared 创建 RestApiHandler 实例
	auto restHandler = std::make_shared<RestApiHandler>(ioc, ctx);

	restHandler->setEndpoint("fapi.binance.com",
							 "443",
							 "/fapi/v1/depth?symbol=BTCUSDT&limit=5");
	restHandler->setPollingInterval(std::chrono::seconds(1));
	restHandler->startPolling();

	// 在单独的线程中运行 io_context，以便非阻塞执行
	std::thread t([&ioc]() {
		ioc.run();
	});

	// std::this_thread::sleep_for(std::chrono::seconds(10));

	// restHandler->setPollingInterval(std::chrono::seconds(-1));
	t.join();
}

// TEST(TestDemo, TestCurl) {
// 	try {
// 		std::string host = "fapi.binance.com";
// 		std::string target = "/fapi/v1/time";
// 		std::string port = "443";

// 		asio::io_context io_context;

// 		asio::ssl::context ssl_context(asio::ssl::context::sslv23);
// 		ssl_context.set_default_verify_paths();

// 		tcp::resolver resolver(io_context);
// 		asio::ssl::stream<tcp::socket> ssl_stream(io_context, ssl_context);

// 		auto endpoints = resolver.resolve(host, port);
// 		asio::connect(ssl_stream.lowest_layer(), endpoints);

// 		ssl_stream.set_verify_mode(asio::ssl::verify_peer);
// 		ssl_stream.set_verify_callback(asio::ssl::rfc2818_verification(host));

// 		ssl_stream.handshake(asio::ssl::stream_base::client);

// 		asio::streambuf request;
// 		std::ostream request_stream(&request);
// 		request_stream << "GET " << target << " HTTP/1.1\r\n";
// 		request_stream << "Host: " << host << "\r\n";
// 		request_stream << "Accept: */
// 							  *\r\n ";
// 							  request_stream
// 					   << "User-Agent: BoostAsioClient/1.0\r\n";
// 		request_stream << "Connection: close\r\n\r\n";

// 		asio::write(ssl_stream, request);

// 		asio::streambuf response;
// 		asio::read_until(ssl_stream, response, "\r\n");

// 		std::istream response_stream(&response);
// 		std::string http_version;
// 		unsigned int status_code;
// 		std::string status_message;
// 		response_stream >> http_version;
// 		response_stream >> status_code;
// 		std::getline(response_stream, status_message);
// 		if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
// 			throw std::runtime_error("无效的响应");
// 		}
// 		if (status_code != 200) {
// 			throw std::runtime_error("请求失败，状态码: " + status_code);
// 		}

// 		asio::read_until(ssl_stream, response, "\r\n\r\n");
// 		std::string header;
// 		while (std::getline(response_stream, header) && header != "\r") {
// 			std::cout << header << std::endl;
// 		}
// 		std::cout << std::endl;

// 		std::ostringstream response_body_stream;
// 		beast::error_code error;
// 		while (asio::read(ssl_stream, response, asio::transfer_at_least(1), error)) {
// 			response_body_stream << &response;
// 		}
// 		if (error != asio::error::eof) {
// 			throw std::system_error();
// 		}

// 		// 解析 JSON 响应
// 		std::string response_body = response_body_stream.str();
// 		std::cout << "响应正文: " << response_body << std::endl;

// 		try {
// 			Json::CharReaderBuilder reader_builder;
// 			Json::Value json_value;
// 			std::string errs;
// 			std::istringstream response_body_stream(response_body);
// 			if (!Json::parseFromStream(reader_builder,
// 									   response_body_stream,
// 									   &json_value,
// 									   &errs)) {
// 				std::cerr << "JSON 解析错误: " << errs << std::endl;
// 				throw std::system_error();
// 			}

// 			// 打印 JSON 对象
// 			std::cout << "解析后的 JSON:" << std::endl;
// 			for (const auto& key : json_value.getMemberNames()) {
// 				std::cout << key << ": " << json_value[key] << std::endl;
// 			}
// 		} catch (std::exception& e) {
// 			std::cerr << "JSON 解析异常: " << e.what() << std::endl;
// 			throw std::system_error();
// 		}
// 		ssl_stream.shutdown();
// 	} catch (std::exception& e) {
// 		std::cout << "异常: " << e.what() << std::endl;
// 	}
// }
