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
	ConnectionHandler(asio::io_context& ioc)
		: io_context_(ioc) {};

	virtual void run() = 0;
	virtual void setEndpoint(const std::string& host,
							 const std::string& port,
							 const std::string& endpoint) = 0;

  protected:

	// Please Override it
	virtual Msg parse(const std::string& data_str) {
		Msg msg = Msg::createOrderBookMsg(data_str);
		return msg;
	};

	virtual void onData(const std::string& data_str){
		Msg msg = parse(data_str);
		if (msg.valid())
			buffer_queue_->encache(msg);
	}
	
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
	 * @param num_restapi The number of REST API connections.
	 * @param num_ws The number of WebSocket connections.
	 */
	ConnectionManager(
		const std::string& host,
		const std::string& port,
		const std::string& endpoint_restapi,
		const std::string& endpoint_ws,
		int num_restapi = 1,
		int num_ws = 1);

	~ConnectionManager() {
		io_context_.stop();
		thread_.join();
	};

	void createConnection(const std::string& host,
						  const std::string& port,
						  const std::string& endpoint,
						  bool is_websocket = false);

	void run() {
		for (auto& handler : handlers_) {
			handler->run();
		}
		thread_ = std::move(std::thread([&]() {
			io_context_.run();
		}));
	}

  private:
	std::thread thread_;
	asio::io_context io_context_;
	std::vector<std::shared_ptr<ConnectionHandler>> handlers_;
	std::shared_ptr<Cache> buffer_;
};

#endif	// CONNECTION_H
