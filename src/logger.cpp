#include "utils/logger.h"

std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> Logger::console_sink = nullptr;

std::shared_ptr<spdlog::logger> Logger::getLogger(const std::string& name) {
	if (console_sink == nullptr) {
		console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%L%$] [%n] %v");
	}
	
	auto logger = std::make_shared<spdlog::logger>(name, console_sink);
	logger->set_level(spdlog::level::info);

	return logger;
}