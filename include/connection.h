#ifndef CONNECTION_H
#define CONNECTION_H

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

#include "common.h"
#include "utils/queue.h"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;
using boost::asio::ip::tcp;

class Handler {
  public:
	Handler(asio::io_context& ioc)
		: io_context_(ioc) {};

	virtual void run() = 0;
	virtual void setEndpoint(const std::string& host,
							 const std::string& port,
							 const std::string& endpoint) = 0;

	virtual void onOrderBook() {}

  protected:
	asio::io_context& io_context_;
	std::shared_ptr<LockFreeQueue<Msg>> buffer_queue_;
};

#endif	// CONNECTION_H
