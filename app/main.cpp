#include "client.h"

class DemoClient : public Client {
  public:
	DemoClient(const std::string& name, const Client::Config& cfg_)
		: Client(name, cfg_) {};

	void onOrderBookWS(const OrderBook& orderBook) override {
		logger_->info("onOrderBookWS----[" + orderBook.symbol + "]  E: " + std::to_string(uint64_t(orderBook.data["E"])));
	}

	void onOrderBookREST(const OrderBook& orderBook) override {
		logger_->info("onOrderBookREST--[" + orderBook.symbol + "]  E: " + std::to_string(uint64_t(orderBook.data["E"])));
	}
};

int main() {
	Client::Config cfg = Client::Config::load_config("../config/setup.ini", "../config/subscription.ini");
	DemoClient client("Demo", std::move(cfg));
	client.setup();
	client.run();
	return 0;
}