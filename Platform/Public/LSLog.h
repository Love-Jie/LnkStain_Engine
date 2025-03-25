#pragma once

#include"LSEngine.h"

#include"spdlog/common.h"


namespace lse {
	class LSLog {
	public:
		LSLog() = delete;
		LSLog(const LSLog&) = delete;
		LSLog& operator=(const LSLog&) = delete;
		static void Init();

		static LSLog* GetLoggerInstance() {
			return &sLoggerInstance;
		}

		template<typename... Args>
		void Log(spdlog::source_loc loc,spdlog::level::level_enum lvl,spdlog::format_string_t<Args...>fmt, Args &&...args) {
			spdlog::memory_buf_t buf;
			fmt::vformat_to(fmt::appender(buf),fmt,fmt::make_format_args(args...));
			Log(loc,lvl,buf);
		}
	private:
		void Log(spdlog::source_loc loc,spdlog::level::level_enum lvl,const spdlog::memory_buf_t &buffer);

		static LSLog sLoggerInstance;

	};

#define LS_LOG_LOGGER_CALL(lslog,level,...)\
		(lslog)->Log(spdlog::source_loc{__FILE__,__LINE__,SPDLOG_FUNCTION},level,__VA_ARGS__)

#define LOG_T(...) LS_LOG_LOGGER_CALL(lse::LSLog::GetLoggerInstance(),spdlog::level::trace,__VA_ARGS__)
#define LOG_D(...) LS_LOG_LOGGER_CALL(lse::LSLog::GetLoggerInstance(),spdlog::level::debug,__VA_ARGS__)
#define LOG_I(...) LS_LOG_LOGGER_CALL(lse::LSLog::GetLoggerInstance(),spdlog::level::info,__VA_ARGS__)
#define LOG_W(...) LS_LOG_LOGGER_CALL(lse::LSLog::GetLoggerInstance(),spdlog::level::warn,__VA_ARGS__)
#define LOG_E(...) LS_LOG_LOGGER_CALL(lse::LSLog::GetLoggerInstance(),spdlog::level::err,__VA_ARGS__)

}



