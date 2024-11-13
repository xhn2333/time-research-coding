#include "consumer.h"
#include "common.h"

void Consumer::run() {
	assert(buffer_queue_ != nullptr);
	asio::post(io_context_, [this]() {
		Msg msg;
		if (this->buffer_queue_->decache(msg)) {
			asio::post(io_context_, [this, msg]() {
				this->onData(msg);
			});
		}
		run();
	});
}

void Consumer::onData(const Msg& msg) {
	std::shared_ptr<BaseData> data = msg.data;
	switch (msg.type) {
		case Msg::Type::OrderBook:
			if (msg.source == Msg::Source::WebSocket) {
				if (callback_onOrderBookWS != nullptr)
					callback_onOrderBookWS(*std::dynamic_pointer_cast<OrderBook>(data));
			} 
			if (msg.source == Msg::Source::RestApi) {
				if (callback_onOrderBookREST != nullptr)
					callback_onOrderBookREST(*std::dynamic_pointer_cast<OrderBook>(data));
			}
			break;
		default:
			std::cout << "Unknown message type" << std::endl;
			break;
	}
}