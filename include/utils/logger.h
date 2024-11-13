#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <string>
#include <unordered_map>

class Logger {
  public:
	// 获取一个全局的logger
	static std::shared_ptr<spdlog::logger> getLogger(const std::string& name);

	// 可选：关闭所有日志
	static void shutdown() {
		spdlog::shutdown();
	}

  private:
	static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
};

#endif	// LOGGER_H
