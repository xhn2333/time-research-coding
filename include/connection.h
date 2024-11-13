#ifndef CONNECTION_H
#define CONNECTION_H

#include "cache.h"
#include "common.h"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;
using boost::asio::ip::tcp;

/**
 * @brief ConnectionHandler class
 * This class is an abstract class that provides an interface for connection handlers.
 * @details
 * The ConnectionHandler class is an abstract class that provides an interface for connection handlers. It has a pure virtual function run() that must be implemented by the derived class.
 */
class ConnectionHandler {
  public:
	/**
	 * @brief Construct a new Connection Handler object
	 * @param ioc The io_context object.
	 */
	ConnectionHandler(const std::string& symbol,
					  asio::io_context& ioc)
		: symbol_(symbol),
		  io_context_(ioc) {};

	virtual void run() = 0;
	virtual void setEndpoint(const std::string& host,
							 const std::string& port,
							 const std::string& endpoint) = 0;

	void bind_cache(std::shared_ptr<Cache> buffer_queue) {
		buffer_queue_ = buffer_queue;
	}

  protected:
	// Please Override it
	// virtual Msg parse(const std::string& data_str) {
	// 	Msg::Source source = Msg::Source::None;
	// 	Msg::Type type = Msg::Type::OrderBook;
	// 	Msg msg = Msg::createMsg(source, type, data_str);
	// 	return msg;
	// };
	virtual Msg parse(const std::string& data_str) = 0;

	virtual void on_data(const std::string& data_str) {
		assert(buffer_queue_ != nullptr);

		Msg msg = parse(data_str);
		if (msg.valid())
			buffer_queue_->encache(msg);
	}

	std::string symbol_;
	asio::io_context& io_context_;
	std::shared_ptr<Cache> buffer_queue_;
};

/**
 * @brief ConnectionManager class
 * This class is responsible for creating and managing connections to the server.
 * @details
 * The ConnectionManager class is responsible for creating and managing connections to the server. Each symbol is controlled by 1 ConnectionManager.
 */
class ConnectionManager {
  public:
	/**
	 * @brief Construct a new Connection Manager object
	 * @param host The host name of the server.
	 * @param port The port number of the server.
	 * @param endpoint_restapi The endpoint of the REST API.
	 * @param endpoint_ws The endpoint of the WebSocket.
	 * @param replicas_restapi The number of REST API connections.
	 * @param replicas_ws The number of WebSocket connections.
	 */
	ConnectionManager(
		const std::string& symbol,
		asio::io_context& io_context_,
		const std::string& host_restapi,
		const std::string& port_restapi,
		const std::string& endpoint_restapi,
		const std::string& host_ws,
		const std::string& port_ws,
		const std::string& endpoint_ws,
		int replicas_restapi = 1,
		int replicas_ws = 1,
		int period_restapi = 1);

	~ConnectionManager() {
	};

	void createConnection(const std::string& host,
						  const std::string& port,
						  const std::string& endpoint,
						  bool is_websocket = false);
	void bind_cache(std::shared_ptr<Cache> buffer);

	void run();

	int get_cache_size() {
		if (buffer_ == nullptr)
			return -1;
		return buffer_->size();
	}

  private:
	std::string symbol_;
	int period_restapi_;
	asio::io_context& io_context_;
	std::vector<std::shared_ptr<ConnectionHandler>> handlers_;
	std::shared_ptr<Cache> buffer_;
};

#endif	// CONNECTION_H
