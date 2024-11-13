#ifndef COMMON_H
#define COMMON_H

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

#include "utils/parser.h"

namespace asio = boost::asio;
namespace beast = boost::beast;

struct BaseData {
	virtual int64_t hash() = 0;
};

struct OrderBook : public BaseData {
	// std::string data;
	std::string symbol;
	json data;

	OrderBook() = default;
	OrderBook(const std::string& symbol,
			  const std::string& data)
		: symbol(symbol),
		  data(parseJsonString(data)) {}

	int64_t hash() override {
		try {
			std::string hash_data = std::to_string(uint64_t(this->data["U"])) +
									std::to_string(uint64_t(this->data["u"])) +
									std::to_string(uint64_t(this->data["pu"]));

			return std::hash<std::string>{}(hash_data);
		} catch (...) {
			return -1;
		}
	}
};

struct Msg {
	enum class Type {
		None,
		OrderBook,
		Order,
		Trade,
		Account,
		Position
	};

	enum class Source {
		None,
		RestApi,
		WebSocket
	};

	Type type = Type::None;
	Source source = Source::None;
	std::shared_ptr<BaseData> data;

	static Msg createMsg(Source source,
						 Type type,
						 const std::string& symbol,
						 const std::string& data) {
		Msg msg;
		msg.type = type;
		msg.source = source;
		if (type == Type::OrderBook)
			msg.data = std::make_shared<OrderBook>(symbol, data);
		return msg;
	}

	bool valid() {
		bool flag = true;
		flag &= type != Type::None;
		flag &= source != Source::None;
		flag &= data != nullptr;
		return flag;
	}
};

#endif	// COMMON_H