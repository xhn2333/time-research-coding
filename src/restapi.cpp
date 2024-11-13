#include "restapi.h"

#include <openssl/ssl.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/lockfree/queue.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;
using boost::asio::ip::tcp;

void RestApiHandler::run() {
	timer_.expires_after(polling_interval_.load());
	timer_.async_wait([this](boost::system::error_code ec) {
		if (!ec) {
			this->ssl_context_.set_default_verify_paths();
			this->socket_ = std::make_shared<boost::asio::ssl::stream<tcp::socket>>(io_context_, ssl_context_);

			resolver_.async_resolve(
				host_,
				port_,
				[this](const boost::system::error_code& error,
					   tcp::resolver::results_type results) {
					on_resolve(error, results, this->socket_);
				});
			run();
		} else {
			std::cerr << "Polling timer error: " << ec.message() << std::endl;
		}
	});
}

void RestApiHandler::setEndpoint(const std::string& host,
								 const std::string& port,
								 const std::string& endpoint) {
	this->host_ = host;
	this->port_ = port;
	this->endpoint_ = endpoint;
	// std::cout << "SETTING ENDPOINT "<< endpoint << std::endl;
	this->req_ = http::request<http::string_body>{http::verb::get, endpoint, 11};
	// this->req_ = http::request<http::string_body>{http::verb::get, "/fapi/v1/depth?symbol=BTCUSDT&limit=5", 11};
	this->req_.set(http::field::host, "fapi.binance.com");
	this->req_.set(http::field::accept, "*/*");
	this->req_.set(http::field::connection, "close");
}

void RestApiHandler::on_resolve(const boost::system::error_code& error,
								tcp::resolver::results_type results,
								std::shared_ptr<boost::asio::ssl::stream<tcp::socket>> socket) {
	if (!error) {
		boost::asio::async_connect(socket->lowest_layer(), results,
								   [this](const boost::system::error_code& error, const tcp::endpoint&) {
									   on_connect(error, this->socket_);
								   });
	} else {
		std::cerr << "Resolve error: " << error.message() << std::endl;
	}
}

void RestApiHandler::on_connect(const boost::system::error_code& error,
								std::shared_ptr<boost::asio::ssl::stream<tcp::socket>> socket) {
	if (!error) {
		socket->async_handshake(boost::asio::ssl::stream_base::client,
								[this](const boost::system::error_code& error) {
									on_handshake(error, this->socket_);
								});
	} else {
		std::cerr << "Connect error: " << error.message() << std::endl;
	}
}

void RestApiHandler::on_handshake(const boost::system::error_code& error,
								  std::shared_ptr<boost::asio::ssl::stream<tcp::socket>> socket) {
	if (!error) {
		// std::string request =
		// 	"GET /fapi/v1/depth?symbol=BTCUSDT&limit=5 HTTP/1.1\r\n"
		// 	"Host: fapi.binance.com\r\n"
		// 	"Accept: */*\r\n"
		// 	"Connection: close\r\n\r\n";
		// boost::asio::async_write(*socket, boost::asio::buffer(request),
		// 						 [this](const boost::system::error_code& error, std::size_t) {
		// 							 on_write(error, this->socket_);
		// 						 });

		// std::cout << "On handshake" << std::endl;
		http::async_write(*socket, std::forward<http::request<http::string_body>>(this->req_),
						  [this](const boost::system::error_code& error, std::size_t) {
							  on_write(error, this->socket_);
						  });

	} else {
		std::cerr << "Handshake error: " << error.message() << std::endl;
	}
}

void RestApiHandler::on_write(const boost::system::error_code& error,
							  std::shared_ptr<boost::asio::ssl::stream<tcp::socket>> socket) {
	if (!error) {
		auto response = std::make_shared<boost::asio::streambuf>();
		boost::asio::async_read_until(*socket, *response, "\r\n0\r\n\r\n",
									  [this, response](const boost::system::error_code& error, std::size_t bytes_transferred) {
										  on_read(error, bytes_transferred, this->socket_, response);
									  });
	} else {
		std::cerr << "Write error: " << error.message() << std::endl;
	}
}

void RestApiHandler::on_read(const boost::system::error_code& error,
							 std::size_t bytes_transferred,
							 std::shared_ptr<boost::asio::ssl::stream<tcp::socket>> socket,
							 std::shared_ptr<boost::asio::streambuf> response) {
	if (!error || error == boost::asio::error::eof) {
		std::istream response_stream(response.get());
		std::string data_str;
		std::string line;
		while (std::getline(response_stream, line) && !line.empty()) {
			// std::cout << line << std::endl;
			// data_str = data_str + '\n' + line;
			data_str = line;
		}
		asio::post(io_context_, std::bind(&RestApiHandler::on_data, this, std::move(data_str)));
	} else {
		std::cerr << "Read error: " << error.message() << std::endl;
	}
}