#pragma once

#include <memory>
#include <spdlog/spdlog.h>

struct log {
public:
  static void init();

  inline static std::shared_ptr<spdlog::logger> &get_logger() {
    return core_logger;
  }

private:
  static std::shared_ptr<spdlog::logger> core_logger;
};

// log macros
#define GK2_PUMA_TRACE(...) ::log::get_logger()->trace(__VA_ARGS__)
#define GK2_PUMA_INFO(...) ::log::get_logger()->info(__VA_ARGS__)
#define GK2_PUMA_WARN(...) ::log::get_logger()->warn(__VA_ARGS__)
#define GK2_PUMA_ERROR(...) ::log::get_logger()->error(__VA_ARGS__)
#define GK2_PUMA_CRITICAL(...) ::log::get_logger()->critical(__VA_ARGS__)
