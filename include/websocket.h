#ifndef WEBOCKET_H
#define WEBOCKET_H

#include <openssl/ssl.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;
using boost::asio::ip::tcp;

class WebSocketHandler : public std::enable_shared_from_this<WebSocketHandler> {
  public:
	WebSocketHandler(asio::io_context& ioc)
		: io_context_(ioc),
		  timer_(ioc),
		  resolver_(ioc),
		  ssl_context_(ssl::context::sslv23),
		  ws_(ioc, ssl_context_) {
		this->ssl_context_.set_default_verify_paths();
		this->ssl_context_.set_verify_mode(boost::asio::ssl::verify_peer);
	}

	void setEndpoint(const std::string& host,
					 const std::string& port,
					 const std::string& endpoint) {
		this->host_ = host;
		this->port_ = port;
		this->endpoint_ = endpoint;
	}

	void run();

	void close();

  private:
	void on_resolve(const boost::system::error_code& ec,
					tcp::resolver::results_type results);
	void on_connect(const boost::system::error_code& ec);
	void on_ssl_handshake(const boost::system::error_code& ec);
	void on_handshake(const boost::system::error_code& ec);
	void do_read();
	void on_read(const boost::system::error_code& ec, std::size_t bytes_transferred);

	asio::io_context& io_context_;
	boost::asio::ssl::context ssl_context_;

	asio::steady_timer timer_;
	tcp::resolver resolver_;
	beast::websocket::stream<ssl::stream<tcp::socket> > ws_;
	beast::flat_buffer buffer;
	std::string host_;
	std::string port_;
	std::string endpoint_;
};

#endif	// WEBOCKET_H