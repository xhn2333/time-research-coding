#include "client.h"
#include "ini.h"
#include <algorithm>  // std::transform
#include <cctype>

Client::Client(const std::string& name, const Config& cfg)
	: cfg_(cfg) {
	logger_ = Logger::getLogger(name);
	logger_->info("Client is initializing");
	buffer_queue_ = std::make_shared<Cache>();
};

Client::~Client() {
	for (int i = 0; i < n_instrument_; ++i)
		(*iocs_)[i].stop();
	for (auto& thread_ : threads_)
		thread_.join();
};

void Client::setup() {
	logger_->info("Client is setting up");

	auto cfg = this->cfg_;
	n_instrument_ = cfg.subscriptions.size();
	iocs_ = std::make_shared<std::vector<asio::io_context>>(n_instrument_);
	for (int i = 0; i < n_instrument_; i++) {
		std::string endpoint_restapi = "/fapi/v1/" + cfg.subscriptions[i].channel + "?symbol=" + cfg.subscriptions[i].symbol + "&limit=" + cfg.subscriptions[i].limit;
		;
		std::string endpoint_ws = "/ws/" + cfg.subscriptions[i].symbol + "@" + cfg.subscriptions[i].channel + cfg.subscriptions[i].limit;
		logger_->info("endpoint_restapi: " + endpoint_restapi);
		logger_->info("endpoint_ws: " + endpoint_ws);
		auto connection_manager_ = std::make_shared<ConnectionManager>(
			cfg.subscriptions[i].symbol,
			(*iocs_)[i],
			cfg_.setup.host_restapi, cfg_.setup.port_restapi, endpoint_restapi,
			cfg_.setup.host_ws, cfg_.setup.port_ws, endpoint_ws,
			cfg_.setup.replicas_restapi, cfg_.setup.replicas_ws,
			cfg_.setup.period_restapi);
		logger_->info("connection_manager setup! ");
		connection_manager_->bind_cache(buffer_queue_);
		connection_managers_.push_back(std::move(connection_manager_));
	}

	consumer_ = std::make_shared<Consumer>(ioc_consumer_);
	consumer_->bind_cache(buffer_queue_);
	consumer_->bind_callback_onOrderBook(std::bind(&Client::onOrderBookWS, this, std::placeholders::_1), true);
	consumer_->bind_callback_onOrderBook(std::bind(&Client::onOrderBookREST, this, std::placeholders::_1), false);
}

void Client::run() {
	assert(buffer_queue_ != nullptr);
	assert(connection_managers_.size() > 0);
	assert(consumer_ != nullptr);
	logger_->info("Client is running");
	for (auto& connection_manager_ : connection_managers_) {
		connection_manager_->run();
	}
	consumer_->run();
	for (int i = 0; i < n_instrument_; ++i)
		threads_.emplace_back([=]() { (*iocs_)[i].run(); });

	ioc_consumer_.run();
};

static int setup_callback(void* user, const char* section, const char* name, const char* value) {
	Client::Config* config = reinterpret_cast<Client::Config*>(user);

	if (strcmp(section, "restapi") == 0) {
		if (strcmp(name, "host") == 0) {
			config->setup.host_restapi = value;
		} else if (strcmp(name, "port") == 0) {
			config->setup.port_restapi = value;
		} else if (strcmp(name, "replicas") == 0) {
			config->setup.replicas_restapi = std::stoi(value);
		} else if (strcmp(name, "period_second") == 0) {
			config->setup.period_restapi = std::stoi(value);
		}
	} else if (strcmp(section, "websocket") == 0) {
		if (strcmp(name, "host") == 0) {
			config->setup.host_ws = value;
		} else if (strcmp(name, "port") == 0) {
			config->setup.port_ws = value;
		} else if (strcmp(name, "replicas") == 0) {
			config->setup.replicas_ws = std::stoi(value);
		}
	}
	return 1;  // Continue parsing
}

// Subscription Section
static int subscription_callback(void* user, const char* section, const char* name, const char* value) {
	Client::Config* config = reinterpret_cast<Client::Config*>(user);

	// Each section represents a subscription (symbol)
	if (name && value) {
		config->subscriptions.emplace_back(section, name, value);
	}
	return 1;  // Continue parsing
}

Client::Config Client::Config::load_config(const std::string& setup_file, const std::string& subscription_file) {
	Client::Config config;
	int result;
	// Parse setup.ini
	result = ini_parse(setup_file.c_str(), setup_callback, &config);
	if (result < 0) {
		std::cerr << "Can't load " << setup_file << std::endl;
		throw std::runtime_error("Can't load " + subscription_file);
		return config;
	}

	// Parse subscription.ini
	result = ini_parse(subscription_file.c_str(), subscription_callback, &config);
	if (result < 0) {
		std::cerr << "Can't load " << subscription_file << std::endl;
		throw std::runtime_error("Can't load " + subscription_file);
		return config;
	}

	return config;
}