#ifndef WEBOCKET_H
#define WEBOCKET_H

#include "connection.h"

class WebSocketHandler : public std::enable_shared_from_this<WebSocketHandler>, public Handler {
  public:
	WebSocketHandler(asio::io_context& ioc)
		: Handler(ioc),
		  timer_(ioc),
		  resolver_(ioc),
		  ssl_context_(ssl::context::sslv23),
		  ws_(ioc, ssl_context_) {
		this->ssl_context_.set_default_verify_paths();
		this->ssl_context_.set_verify_mode(boost::asio::ssl::verify_peer);
	}

	void setEndpoint(const std::string& host,
					 const std::string& port,
					 const std::string& endpoint) override;
	void run() override;

	void close();

  private:
	void on_resolve(const boost::system::error_code& ec,
					tcp::resolver::results_type results);
	void on_connect(const boost::system::error_code& ec);
	void on_ssl_handshake(const boost::system::error_code& ec);
	void on_handshake(const boost::system::error_code& ec);
	void do_read();
	void on_read(const boost::system::error_code& ec, std::size_t bytes_transferred);

	asio::steady_timer timer_;

	boost::asio::ssl::context ssl_context_;
	tcp::resolver resolver_;
	beast::websocket::stream<ssl::stream<tcp::socket> > ws_;

	beast::flat_buffer buffer;
	std::string host_;
	std::string port_;
	std::string endpoint_;
};

#endif	// WEBOCKET_H