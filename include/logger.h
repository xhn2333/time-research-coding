#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <string>
#include <unordered_map>

class Logger
{
public:
    static void init(const std::string& defaultLogFile = "logs/default_log.txt")
    {
        if (defaultLogger == nullptr)
        {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::trace);

            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(defaultLogFile, true);
            file_sink->set_level(spdlog::level::trace);

            defaultLogger = std::make_shared<spdlog::logger>("default_logger", spdlog::sinks_init_list{console_sink, file_sink});
            spdlog::register_logger(defaultLogger);
            spdlog::set_default_logger(defaultLogger);

            defaultLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
            defaultLogger->set_level(spdlog::level::trace);

            spdlog::info("Default logger initialized with file: {}", defaultLogFile);
        }
    }

    static std::shared_ptr<spdlog::logger> getLogger(const std::string& topic)
    {
        if (loggers.find(topic) == loggers.end())
        {
            createTopicLogger(topic);
        }
        return loggers[topic];
    }

    static void setLevel(spdlog::level::level_enum level)
    {
        if (defaultLogger)
        {
            defaultLogger->set_level(level);
        }
        for (auto& [topic, logger] : loggers)
        {
            logger->set_level(level);
        }
    }

private:
    Logger() = default;
    ~Logger() = default;

    static void createTopicLogger(const std::string& topic)
    {
        std::string fileName = "logs/" + topic + "_log.txt";
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true);
        file_sink->set_level(spdlog::level::trace);

        auto logger = std::make_shared<spdlog::logger>(topic, file_sink);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
        logger->set_level(spdlog::level::trace);

        spdlog::register_logger(logger);
        loggers[topic] = logger;

        spdlog::info("Logger for topic '{}' initialized with file: {}", topic, fileName);
    }

    static std::shared_ptr<spdlog::logger> defaultLogger;
    static std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> loggers;
};

std::shared_ptr<spdlog::logger> Logger::defaultLogger = nullptr;
std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> Logger::loggers;

#define LOG_TOPIC_TRACE(topic, ...) Logger::getLogger(topic)->trace(__VA_ARGS__)
#define LOG_TOPIC_DEBUG(topic, ...) Logger::getLogger(topic)->debug(__VA_ARGS__)
#define LOG_TOPIC_INFO(topic, ...) Logger::getLogger(topic)->info(__VA_ARGS__)
#define LOG_TOPIC_WARN(topic, ...) Logger::getLogger(topic)->warn(__VA_ARGS__)
#define LOG_TOPIC_ERROR(topic, ...) Logger::getLogger(topic)->error(__VA_ARGS__)
#define LOG_TOPIC_CRITICAL(topic, ...) Logger::getLogger(topic)->critical(__VA_ARGS__)

#define LOG_TRACE(...) Logger::getLogger("default")->trace(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::getLogger("default")->debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::getLogger("default")->info(__VA_ARGS__)
#define LOG_WARN(...) Logger::getLogger("default")->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::getLogger("default")->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::getLogger("default")->critical(__VA_ARGS__)

#endif  // LOGGER_H
