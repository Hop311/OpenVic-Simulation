#pragma once

#include <functional>
#include <iostream>
#include <queue>
#include <sstream>

#ifdef __cpp_lib_source_location
#include <source_location>
#endif

#include "openvic-simulation/utility/StringUtils.hpp"

namespace OpenVic {

#ifndef __cpp_lib_source_location
#include <string>
	// Implementation of std::source_location for compilers that do not support it
	// Note: uses non-standard extensions that are supported by Clang, GCC, and MSVC
	// https://clang.llvm.org/docs/LanguageExtensions.html#source-location-builtins
	// https://stackoverflow.com/a/67970107
	class source_location {
		std::string _file;
		int _line;
		std::string _function;

		source_location(std::string f, int l, std::string n) : _file(f), _line(l), _function(n) {}

	public:
		static source_location current(
			std::string f = __builtin_FILE(), int l = __builtin_LINE(), std::string n = __builtin_FUNCTION()
		) {
			return source_location(f, l, n);
		}

		inline char const* file_name() const {
			return _file.c_str();
		}
		inline int line() const {
			return _line;
		}
		inline char const* function_name() const {
			return _function.c_str();
		}
	};
#endif

	class Logger final {
		using log_func_t = std::function<void(std::string&&)>;
		using log_queue_t = std::queue<std::string>;

#ifdef __cpp_lib_source_location
		using source_location = std::source_location;
#else
		using source_location = OpenVic::source_location;
#endif

	public:
		static void set_logger_funcs() {
			set_info_func([](std::string&& str) {
				std::cout << "[INFO] " << str;
			});
			set_warning_func([](std::string&& str) {
				std::cerr << "[WARNING] " << str;
			});
			set_error_func([](std::string&& str) {
				std::cerr << "[ERROR] " << str;
			});
		}

	private:
		struct log_channel_t {
			log_func_t func;
			log_queue_t queue;
			size_t message_count;
		};

		template<typename... Ts>
		struct log {
			log(log_channel_t& log_channel, Ts&&... ts, source_location const& location) {
				std::stringstream stream;
				stream << StringUtils::get_filename(location.file_name()) << "("
					/* Function name removed to reduce clutter. It is already included
					* in Godot's print functions, so this was repeating it. */
					//<< location.line() << ") `" << location.function_name() << "`: ";
					<< location.line() << "): ";
				((stream << std::forward<Ts>(ts)), ...);
				stream << std::endl;
				log_channel.queue.push(stream.str());
				if (log_channel.func) {
					do {
						log_channel.func(std::move(log_channel.queue.front()));
						log_channel.queue.pop();
						/* Only count printed messages, so that message_count matches what is seen in the console. */
						log_channel.message_count++;
					} while (!log_channel.queue.empty());
				}
			}
		};

#define LOG_FUNC(name) \
private: \
	static inline log_channel_t name##_channel {}; \
\
public: \
	static inline void set_##name##_func(log_func_t log_func) { \
		name##_channel.func = log_func; \
	} \
	static inline size_t get_##name##_count() { \
		return name##_channel.message_count; \
	} \
	template<typename... Ts> \
	struct name { \
		name(Ts&&... ts, source_location const& location = source_location::current()) { \
			log<Ts...> { name##_channel, std::forward<Ts>(ts)..., location }; \
		} \
	}; \
	template<typename... Ts> \
	name(Ts&&...) -> name<Ts...>;

		LOG_FUNC(info)
		LOG_FUNC(warning)
		LOG_FUNC(error)

#undef LOG_FUNC
	};
}
