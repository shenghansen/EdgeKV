#ifndef HEADER_FILE_NAME_H
#define HEADER_FILE_NAME_H

// spdlog
#ifndef HIGH_PERFORMANCE
#include "spdlog/spdlog.h"
namespace utill {
#define DEBUG(...)                                                             \
  SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#define INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#define WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#define ERROR(...)                                                             \
  SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#define DEBUG(...)
#define LOG(...)
#define WARN(...)
#define ERROR(...)
#endif
}

// prefetch range
#define PREFETCH_RANGE 5

#endif
