#include "connection.h"
#include "restapi.h"
#include "websocket.h"

ConnectionManager::ConnectionManager(const std::string& symbol,
									 asio::io_context& io_context_,
									 const std::string& host_restapi,
									 const std::string& port_restapi,
									 const std::string& endpoint_restapi,
									 const std::string& host_ws,
									 const std::string& port_ws,
									 const std::string& endpoint_ws,
									 int replicas_restapi,
									 int replicas_ws,
									 int period_restapi)
	: symbol_(symbol),
	  period_restapi_(period_restapi),
	  io_context_(io_context_) {
	for (int i = 0; i < replicas_restapi; i++)
		this->createConnection(host_restapi, port_restapi, endpoint_restapi, false);
	for (int i = 0; i < replicas_ws; i++)
		this->createConnection(host_ws, port_ws, endpoint_ws, true);
}

void ConnectionManager::createConnection(const std::string& host,
										 const std::string& port,
										 const std::string& endpoint,
										 bool is_websocket) {
	if (is_websocket) {
		std::shared_ptr<ConnectionHandler> wsHandler = std::make_shared<WebSocketHandler>(symbol_, io_context_);
		wsHandler->setEndpoint(host, port, endpoint);
		handlers_.push_back(std::move(wsHandler));
	} else {
		std::shared_ptr<ConnectionHandler> restHandler = std::make_shared<RestApiHandler>(symbol_, io_context_);
		restHandler->setEndpoint(host, port, endpoint);
		std::dynamic_pointer_cast<RestApiHandler>(restHandler)->setPollingInterval(std::chrono::seconds(period_restapi_));
		handlers_.push_back(std::move(restHandler));
	}
}

void ConnectionManager::bind_cache(std::shared_ptr<Cache> buffer) {
	buffer_ = buffer;
	for (auto& handler : handlers_) {
		handler->bind_cache(buffer_);
	}
}

void ConnectionManager::run() {
	for (auto& handler : handlers_) {
		handler->run();
	}
}