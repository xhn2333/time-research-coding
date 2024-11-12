#include "restapi.h"
#include "websocket.h"
#include "connection.h"

ConnectionManager::ConnectionManager(const std::string& host,
									 const std::string& port,
									 const std::string& endpoint_restapi,
									 const std::string& endpoint_ws,
									 int num_restapi,
									 int num_ws) {
	for (int i = 0; i < num_restapi; i++)
		this->createConnection(host, port, endpoint_restapi, false);
	for (int i = 0; i < num_ws; i++)
		this->createConnection(host, port, endpoint_ws, true);
}

void ConnectionManager::createConnection(const std::string& host,
										 const std::string& port,
										 const std::string& endpoint,
										 bool is_websocket) {
	if (is_websocket) {
		std::shared_ptr<ConnectionHandler> wsHandler = std::make_shared<WebSocketHandler>(io_context_);
		wsHandler->setEndpoint(host, port, endpoint);
		handlers_.push_back(wsHandler);
	} else {
		std::shared_ptr<ConnectionHandler> restHandler = std::make_shared<RestApiHandler>(io_context_);
		restHandler->setEndpoint(host, port, endpoint);
		handlers_.push_back(restHandler);
	}
}