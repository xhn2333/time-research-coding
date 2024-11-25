#ifndef RESTAPI_H
#define RESTAPI_H

#include "connection.h"

class RestApiHandler : public std::enable_shared_from_this<RestApiHandler>, public ConnectionHandler {
  private:
	std::string host_;
	std::string port_;
	std::string endpoint_;

	asio::steady_timer timer_;
	std::atomic<std::chrono::seconds> polling_interval_;

	ip::tcp::resolver resolver_;
	boost::asio::ssl::context ssl_context_;
	std::shared_ptr<ssl::stream<tcp::socket>> socket_;

	http::request<http::string_body> req_;
	boost::asio::streambuf response_;

  public:
	RestApiHandler(const std::string& symbol, asio::io_context& ioc)
		: ConnectionHandler(symbol, ioc),
		  timer_(ioc),
		  resolver_(ioc),
		  ssl_context_(boost::asio::ssl::context::sslv23) {
		this->polling_interval_.store(std::chrono::seconds(5));
		this->ssl_context_.set_default_verify_paths();
		this->socket_ = std::make_shared<ssl::stream<tcp::socket>>(ioc, this->ssl_context_);
	}

	void setEndpoint(const std::string& host,
					 const std::string& port,
					 const std::string& endpoint) override;

	void setPollingInterval(std::chrono::seconds new_interval) {
		polling_interval_.store(new_interval);
	}

	Msg parse(const std::string& data_str) override {
		Msg msg = Msg::createMsg(Msg::Source::RestApi,
								 Msg::Type::OrderBook,
								 symbol_,
								 data_str);
		return msg;
	}

	void run() override;

  private:
	void on_resolve(const boost::system::error_code& error,
					tcp::resolver::results_type results,
					std::shared_ptr<boost::asio::ssl::stream<tcp::socket>> socket);
	void on_connect(const boost::system::error_code& error,
					std::shared_ptr<ssl::stream<tcp::socket>> socket);
	void on_handshake(const boost::system::error_code& error,
					  std::shared_ptr<ssl::stream<tcp::socket>> socket);
	void on_write(const boost::system::error_code& error,
				  std::shared_ptr<ssl::stream<tcp::socket>> socket);
	void on_read(const boost::system::error_code& error,
				 std::size_t bytes_transferred,
				 std::shared_ptr<ssl::stream<tcp::socket>> socket,
				 std::shared_ptr<boost::asio::streambuf> response);
};

#endif	// RESTAPI_H
