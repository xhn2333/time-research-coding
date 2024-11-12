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

namespace asio = boost::asio;
namespace beast = boost::beast;

struct BaseData {
	virtual int64_t hash() = 0;
};

struct OrderBook : public BaseData {
	std::string data;
	
	OrderBook(const std::string& data) : data(data) {}

	int64_t hash() override {
		return std::hash<std::string>{}(data);
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

	enum class Status {
		None,
		Succeeded,
		Failed
	};

	enum class Source {
		None,
		RestApi,
		WebSocket
	};

	Type type = Type::None;
	Status status = Status::None;
	Source source = Source::None;
	std::shared_ptr<BaseData> data;

	
	static Msg createOrderBookMsg(const std::string& data) {
		Msg msg;
		msg.type = Type::OrderBook;
		msg.data = std::make_shared<OrderBook>(OrderBook{data});
		return msg;
	}


	bool valid() {
		bool flag = true;
		flag &= type != Type::None;
		flag &= status != Status::None;
		flag &= source != Source::None;
		flag &= data != nullptr;
		return flag;
	}
};

#endif	// COMMON_H