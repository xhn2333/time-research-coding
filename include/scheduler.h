#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <memory>
#include <thread>

using namespace boost::asio;
using namespace boost::beast;

class EventLoop {
  public:
	EventLoop()
		: io_context(),
		  work_guard(make_work_guard(io_context)) {}

	void run() {
		io_context.run();
	}

	template <typename Task>
	void addTask(Task&& task) {
		post(io_context, std::forward<Task>(task));
	}

	io_context& getContext() {
		return io_context;
	}

  private:
	io_context io_context;
	executor_work_guard<io_context::executor_type> work_guard;
};

#endif	// SCHEDULER_H