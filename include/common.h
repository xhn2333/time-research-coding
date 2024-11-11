#ifndef COMMON_H
#define COMMON_H

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/beast.hpp>
#include <string>

using std::string;
namespace asio = boost::asio;
namespace beast = boost::beast;

inline void fail(beast::error_code ec, const char* what) {
	throw std::runtime_error(ec.message() + what);
}

inline void fail(const char* what) {
	throw std::runtime_error(what);
}

inline void fail(const string& what) {
	throw std::runtime_error(what);
}

/// Call the user's callback with the failure on the calling thread.
template <typename ResultT>
inline void fail(beast::error_code ec, const string what, std::function<void(ResultT)> callback) {
	if (callback) {
		callback(ResultT{std::move(what + " " + ec.message())});
	}
}

/// Call the user's callback with the failure on the calling thread.
template <typename ResultT>
inline void fail(const string what, std::function<void(ResultT)> callback) {
	if (callback) {
		callback(ResultT{std::move(what)});
	}
}

#endif	// COMMON_H