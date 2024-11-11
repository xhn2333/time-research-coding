#ifndef WEBOCKET_H
#define WEBOCKET_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace boost::asio;
using namespace boost::beast;
using namespace boost::asio::ip;

class WebSocketHandler {
  public:
	WebSocketHandler(io_context& ioc,
					 const std::string& host,
					 const std::string& port)
		: resolver(ioc), ws(ioc), host(host), port(port) {}

	void connect(const std::string& host, const std::string& port) {
		resolver.async_resolve(
			host, port,
			[this](boost::system::error_code ec, tcp::resolver::results_type results) {
				if (!ec) {
					async_connect(
						ws.next_layer(), results.begin(), results.end(),
						[this](boost::system::error_code ec, tcp::endpoint) {
							if (!ec)
								ws.async_handshake("example.com", "/",
												   [this](boost::system::error_code ec) {
													   if (!ec) read();
												   });
						});
				}
			});
	}

	void read() {
		ws.async_read(buffer,
					  [this](boost::system::error_code ec, std::size_t bytes_transferred) {
						  if (!ec) {
							  std::cout << buffers_to_string(buffer.data()) << std::endl;
							  buffer.consume(bytes_transferred);
							  read();
						  }
					  });
	}

  private:
	tcp::resolver resolver;
	websocket::stream<tcp::socket> ws;
	flat_buffer buffer;
	std::string host;
	std::string port;
};

#endif	// WEBOCKET_H