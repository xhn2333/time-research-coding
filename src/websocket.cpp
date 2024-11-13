#include "websocket.h"

void WebSocketHandler::run() {
	// Resolve the domain name into an IP address.
	asio::post(
		io_context_, [this]() {
			this->ssl_context_.set_default_verify_paths();
			this->resolver_.async_resolve(
				host_,
				port_,
				beast::bind_front_handler(&WebSocketHandler::on_resolve, shared_from_this()));
		});
}

void WebSocketHandler::setEndpoint(const std::string& host,
								   const std::string& port,
								   const std::string& endpoint) {
	this->host_ = host;
	this->port_ = port;
	this->endpoint_ = endpoint;
}

void WebSocketHandler::close() {
	boost::system::error_code ec;
	ws_.close(beast::websocket::close_code::normal, ec);
	if (ec) {
		std::cerr << "Close error: " << ec.message() << std::endl;
	}
}

void WebSocketHandler::on_resolve(const boost::system::error_code& ec,
								  tcp::resolver::results_type results) {
	if (ec) {
		std::cerr << "Resolve error: " << ec.message() << std::endl;
		return;
	}

	asio::async_connect(
		ws_.next_layer().next_layer(),
		results,
		std::bind(
			&WebSocketHandler::on_connect,
			shared_from_this(),
			std::placeholders::_1));
}

void WebSocketHandler::on_connect(const boost::system::error_code& ec) {
	if (ec) {
		std::cerr << "Connect error: " << ec.message() << std::endl;
		return;
	}
	ws_.next_layer().async_handshake(
		ssl::stream_base::client,
		std::bind(
			&WebSocketHandler::on_ssl_handshake, shared_from_this(), std::placeholders::_1));
}

void WebSocketHandler::on_ssl_handshake(const boost::system::error_code& ec) {
	if (ec) {
		std::cerr << "SSL Handshake error: " << ec.message() << std::endl;
		return;
	}
	ssl_context_.set_verify_mode(ssl::verify_client_once);
	// ssl_context_.set_default_verify_paths();
	tcp::socket& m_socket = ws_.next_layer().next_layer();
	// m_socket.set_option(boost::asio::ip::tcp::no_delay(true));

	ws_.set_option(beast::websocket::stream_base::decorator(
		[](beast::websocket::request_type& req) {
			req.set(http::field::user_agent, "Boost.Beast WebSocket Client");
			req.set(http::field::sec_websocket_protocol, "stream");
		}));

	ws_.async_handshake(host_, endpoint_,
						std::bind(&WebSocketHandler::on_handshake,
								  shared_from_this(),
								  std::placeholders::_1));
}

void WebSocketHandler::on_handshake(const boost::system::error_code& ec) {
	if (ec) {
		std::cerr << "Handshake error: " << ec.message() << std::endl;
		return;
	}

	do_read();
}

void WebSocketHandler::do_read() {
	ws_.async_read(
		buffer,
		[this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
			on_read(ec, bytes_transferred);
		});
}

void WebSocketHandler::on_read(const boost::system::error_code& ec, std::size_t bytes_transferred) {
	if (ec) {
		if (ec == beast::websocket::error::closed) {
			std::cout << "WebSocket closed by server." << std::endl;
		} else {
			std::cerr << "Read failed: " << ec.message() << std::endl;
		}
		return;
	}
	std::string data_str = beast::buffers_to_string(buffer.data());
	// std::cout << beast::make_printable(buffer.data()) << std::endl;
	// std::cout << data_str << std::endl;
	buffer.consume(buffer.size());

	asio::post(io_context_,
			   std::bind(&WebSocketHandler::on_data, this, data_str));

	// Continue to read messages.
	ws_.async_read(
		buffer,
		[this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
			on_read(ec, bytes_transferred);
		});
}
