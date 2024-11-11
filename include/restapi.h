#ifndef RESTAPI_H
#define RESTAPI_H

#include <openssl/ssl.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
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

class RestApiHandler : public std::enable_shared_from_this<RestApiHandler> {
  public:
	RestApiHandler(asio::io_context& ioc, ssl::context& ctx)
		: timer_(ioc),
		  resolver_(ioc),
		  stream_(ioc, ctx),
		  retry_timer_(ioc),
		  polling_interval_(std::chrono::seconds(5)),
		  ioc_(ioc) {}

	void setEndpoint(const std::string& host,
					 const std::string& port,
					 const std::string& endpoint) {
		this->host_ = host;
		this->port_ = port;
		this->endpoint_ = endpoint;
	}

	void setPollingInterval(std::chrono::seconds new_interval) {
		polling_interval_.store(new_interval);
	}

	void startPolling() {
		schedulePolling();
	}

  private:
	void schedulePolling() {
		timer_.expires_after(polling_interval_.load());
		timer_.async_wait([this](boost::system::error_code ec) {
			if (!ec) {
				req_.version(11);
				req_.method(http::verb::get);
				req_.set(http::field::host, host_);
				req_.target(endpoint_);
				req_.set(http::field::user_agent, "binancebeast");
				// req_.insert("X-MBX-APIKEY", m_apiKeys.api);

				resolver_.async_resolve(
					host_,
					port_,
					beast::bind_front_handler(&RestApiHandler::on_resolve, shared_from_this()));
				schedulePolling();
			} else {
				std::cerr << "Polling timer error: " << ec.message() << std::endl;
			}
		});
	}

	void on_resolve(beast::error_code ec, ip::tcp::resolver::results_type results) {
		if (ec) {
			std::cerr << "Resolve error: " << ec.message() << std::endl;
			return;
		}

		stream_.set_verify_mode(ssl::verify_peer);
		stream_.set_verify_callback(ssl::rfc2818_verification(host_));

		beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(10));  // TODO is this ok
		beast::get_lowest_layer(stream_).async_connect(
			results,
			beast::bind_front_handler(&RestApiHandler::on_connect, shared_from_this()));
	}

	void on_connect(beast::error_code ec, ip::tcp::resolver::results_type::endpoint_type) {
		if (ec) {
			std::cerr << "Connect error: " << ec.message() << std::endl;
			return;
		}
		beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(5));

		stream_.async_handshake(
			ssl::stream_base::client,
			beast::bind_front_handler(&RestApiHandler::on_handshake, shared_from_this()));
	}

	void on_handshake(beast::error_code ec) {
		// if (ec) {
		// 	std::cerr << "Handshake error: " << ec.message() << std::endl;
		// 	return;
		// } else {
		// 	beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(5));
		// 	beast::http::async_write(
		// 		stream_,
		// 		req_,
		// 		beast::bind_front_handler(&RestApiHandler::on_write, shared_from_this()));
		// }
	}

	void on_write(beast::error_code ec, std::size_t /*bytes_transferred*/) {
		if (ec) {
			std::cerr << "Write error: " << ec.message() << std::endl;
			return;
		} else {
			beast::http::request<beast::http::string_body> req{beast::http::verb::get, endpoint_, 11};
			req.set(beast::http::field::host, host_);
			req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		}
	}

	void on_read(beast::error_code ec, std::size_t /*bytes_transferred*/) {
		if (ec) {
			std::cerr << "Read error: " << ec.message() << std::endl;
			return;
		}
	}

	void on_shutdown(beast::error_code ec) {
		if (ec == asio::error::eof) {
			// Rationale:
			// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
			ec = {};
		}

		if (ec)
			return;

		// If we get here then the connection is closed gracefully
	}

	asio::io_context& ioc_;
	asio::steady_timer timer_;
	ip::tcp::resolver resolver_;
	ssl::stream<beast::tcp_stream> stream_;	 // SSL stream for secure communication
	asio::steady_timer retry_timer_;
	std::atomic<std::chrono::seconds> polling_interval_;

	http::request<http::string_body> req_;
	http::response<http::string_body> res_;
	std::string host_;
	std::string port_;
	std::string endpoint_;

	boost::beast::flat_buffer buffer_;
};

#endif	// RESTAPI_H
