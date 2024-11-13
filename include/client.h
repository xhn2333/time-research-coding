#ifndef CLIENT_H
#define CLIENT_H

#include <spdlog/spdlog.h>
#include "cache.h"
#include "common.h"
#include "connection.h"
#include "consumer.h"
#include "utils/logger.h"

class Client {
  public:
	struct Config {
		static Config load_config(const std::string& setup_file,
								  const std::string& subscription_file);

		struct Setup {
			std::string host_restapi;
			std::string port_restapi;
			std::string host_ws;
			std::string port_ws;
			int replicas_restapi;
			int replicas_ws;
			int period_restapi;
		} setup;

		struct Subscription {
			std::string symbol;
			std::string channel;
			std::string limit;
			Subscription(const std::string& symbol, const std::string& channel, const std::string& limit)
				: symbol(symbol), channel(channel) , limit(limit){}
		};

		std::vector<Subscription> subscriptions;
	};

	Client(const std::string& name, const Config& cfg);
	~Client();

	void setup();
	void run();

	virtual void onOrderBookWS(const OrderBook& orderBook) = 0;
	virtual void onOrderBookREST(const OrderBook& orderBook) = 0;

  protected:
	std::shared_ptr<spdlog::logger> logger_;

  private:
	Config cfg_;
	int n_instrument_;
	std::shared_ptr<std::vector<asio::io_context>> iocs_;
	asio::io_context ioc_consumer_;
	std::shared_ptr<Cache> buffer_queue_;

	std::vector<std::shared_ptr<ConnectionManager>> connection_managers_;
	std::shared_ptr<Consumer> consumer_;

	std::vector<std::thread> threads_;
};

#endif	// CLIENT_H
