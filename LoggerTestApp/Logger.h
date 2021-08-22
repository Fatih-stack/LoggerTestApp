#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <mutex>
#include <ctime>
#include <fstream>
#include <tchar.h>
#include <windows.h>
#include <string>
#include <sstream>

namespace aricanli {
	namespace general {
		enum class Severity
		{
			Quiet = 0, Fatal = 8, Error = 16, Warning = 24, Info = 32, Verbose = 40, Debug = 48, Trace = 56
		};

		class Logger
		{
		private:
			Severity severity = Severity::Quiet;
			std::mutex log_mutex;
			std::string file_path = "";
			std::fstream file;

		protected:
			// Second version:
			template< typename ... Args >
			std::wstring stringer(const Args& ... args)
			{
				std::wostringstream oss;
				int a[] = { 0, ((void)(oss << args), 0) ... };

				return oss.str();
			}

			template <typename T>
			void log_argument(T t) {
				std::cout << t << " ";
			}
			template <typename T>
			void wlog_argument(T value) {
				std::wcout << value << " ";
			}
			template <typename T>
			void wlog_writefile(const T& value) {
				auto& x = typeid(value);
				//		std::cout << x.name();
				file.close();
				file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
				if (typeid(value) == typeid(wchar_t const* __ptr64)) {
					std::wstring temp = stringer(value);

					std::string res(temp.begin(), temp.end());
					file << res.c_str() << " ";
					return;
				}
				file << value << " ";
			}
			template <typename T>
			void log_writefile(const T& t) {
				file.close();
				file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
				file << t << " ";
			}

		public:
			static void SetPriority(Severity new_severity)
			{
				get_instance().severity = new_severity;
			}

			template<typename... Args>
			static void Quiet(int line, const std::string& source_file, const std::string& message, Args... args)
			{
				get_instance().log(line, source_file, "[Quiet]\t", Severity::Quiet, message, args...);
			}

			template<typename... Args>
			static void Quiet(int line, const std::wstring& source_file, const std::wstring& message, Args... args)
			{
				get_instance().log(line, std::wstring(source_file), _T("[Quiet]\t"),
					Severity::Quiet, std::wstring(message), args...);
			}

			template<typename... Args>
			static void Fatal(int line, const std::string& source_file, const std::string& message, Args... args)
			{
				get_instance().log(line, source_file, "[Fatal]\t", Severity::Fatal, message, args...);
			}

			template<typename... Args>
			static void Fatal(int line, const std::wstring& source_file, const std::wstring& message, Args... args)
			{
				get_instance().log(line, std::wstring(source_file), _T("[Fatal]\t"), Severity::Fatal,
					std::wstring(message), args...);
			}

			template<typename... Args>
			static void Error(int line, const std::string& source_file, const std::string& message, Args... args)
			{
				get_instance().log(line, source_file, "[Error]\t", Severity::Error, message, args...);
			}

			template<typename... Args>
			static void Error(int line, const std::wstring& source_file, const std::wstring& message, Args... args)
			{
				get_instance().log(line, std::wstring(source_file), _T("[Error]\t"), Severity::Error,
					std::wstring(message), args...);
			}

			template<typename... Args>
			static void Warning(int line, const char* source_file, const char* message, Args... args)
			{
				get_instance().log(line, source_file, "[Warn]\t", Severity::Warning, message, args...);
			}

			template<typename... Args>
			static void Warning(int line, const wchar_t* source_file, const wchar_t* message, Args... args)
			{
				get_instance().log(line, std::wstring(source_file), _T("[Warn]\t"), Severity::Warning,
					std::wstring(message), args...);
			}

			template<typename... Args>
			static void Info(int line, const std::string& source_file, const std::string& message, Args... args)
			{
				get_instance().log(line, source_file, "[Info]\t", Severity::Info, message, args...);
			}

			template<typename... Args>
			static void Info(int line, const std::wstring& source_file, const std::wstring& message, Args... args)
			{
				get_instance().log(line, std::wstring(source_file), _T("[Info]\t"),
					Severity::Info, std::wstring(message), args...);
			}

			template<typename... Args>
			static void Verbose(int line, const char* source_file, const char* message, Args... args)
			{
				get_instance().log(line, source_file, "[Verbose]\t", Severity::Verbose, message, args...);
			}

			template<typename... Args>
			static void Verbose(int line, const wchar_t* source_file, const wchar_t* message, Args... args)
			{
				get_instance().log(line, std::wstring(source_file), _T("[Verbose]\t"), Severity::Verbose,
					std::wstring(message), args...);
			}

			template<typename... Args>
			static void Debug(int line, const char* source_file, const char* message, Args... args)
			{
				get_instance().log(line, source_file, "[Debug]\t", Severity::Debug, message, args...);
			}

			template<typename... Args>
			static void Debug(int line, const wchar_t* source_file, const wchar_t* message, Args... args)
			{
				get_instance().log(line, std::wstring(source_file), _T("[Debug]\t"),
					Severity::Debug, std::wstring(message), args...);
			}

			template<typename... Args>
			static void Trace(int line, const char* source_file, const char* message, Args... args)
			{
				get_instance().log(line, source_file, "[Trace]\t", Severity::Trace, message, args...);
			}

			template<typename... Args>
			static void Trace(int line, const wchar_t* source_file, const wchar_t* message, Args... args)
			{
				get_instance().log(line, std::wstring(source_file), _T("[Trace]\t"),
					Severity::Trace, std::wstring(message), args...);
			}

		public:
			Logger()
			{
				
			}

			Logger(const Logger&) = delete;
			Logger& operator= (const Logger&) = delete;

			~Logger()
			{
				if (file.is_open())
					file.close();
			}

			static Logger& get_instance()
			{
				static Logger logger;
				return logger;
			}

			template<typename... Args>
			void log(int line, const std::string& source, const std::string& msg_priorty_str,
				Severity msg_severity, const std::string& msg, Args... args)
			{
				if (severity <= msg_severity)
				{
					time_t current_time = time(0);
					tm* timestamp = localtime(&current_time);
					char buffer[80];
					strftime(buffer, 80, "%c", timestamp);
					std::string s(buffer);
					typename std::lock_guard lock(log_mutex);
					std::cout << s << '\t' << msg_priorty_str << " " << msg;
					int dummy[] = { 0, ((void)log_argument(std::forward<Args>(args)),0)... };

					if (file.is_open())
					{
						file.close();
						file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
						file << s.c_str() << '\t' << msg_priorty_str.c_str() << " " << msg.c_str() << " ";
						int dummy[] = { 0, ((void)log_writefile(std::forward<Args>(args)),0)... };
						file << " on line " << line << " in " << source.c_str() << "\n";
					}
					else
						AfxMessageBox(_T("Logger: Failed to open file "));
				}
			}

			template<typename... Args>
			void log(int line, const std::wstring& source, const std::wstring& msg_priorty_str,
				Severity msg_severity, const std::wstring& msg, Args... args)
			{
				if (severity <= msg_severity)
				{
					time_t current_time = time(0);
					tm* timestamp = localtime(&current_time);
					wchar_t buffer[80];
					wcsftime(buffer, 80, _T("%c"), timestamp);
					std::wstring s(buffer);
					typename std::lock_guard lock(log_mutex);
					std::string strS(s.begin(), s.end());
					std::string strMsgPrty(msg_priorty_str.begin(), msg_priorty_str.end());
					std::string strMsg(msg.begin(), msg.end());
					std::cout << strS << '\t' << strMsgPrty << " " << strMsg;
					int dummy[] = { 0, ((void)wlog_argument(std::forward<Args>(args)),0)... };
					std::string strSource(source.begin(), source.end());

					if (file.is_open())
					{
						file.close();
						file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
						file << strS.c_str() << '\t' << strMsgPrty.c_str() << " " << strMsg.c_str() << " ";
						int dummy[] = { 0, ((void)wlog_writefile(std::forward<Args>(args)),0)... };
						file << " on line " << line << " in " << strSource.c_str() << "\n";
					}
					else {
						AfxMessageBox(_T("Logger: Failed to open file "));
					}
				}
			}

			void close_file()
			{
				if (file.is_open())
					file.close();
			}

			void enable_file_output(const std::string& new_file_path)
			{
				file.close();
				file_path = new_file_path;
				file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
				if (!file.is_open()) {
					AfxMessageBox(_T("File creation failed."));
					return;
				}
			}
		};

		#define LOG_QUIET(Message, ...) (Logger::Quiet(__LINE__, __FILE__, Message, __VA_ARGS__))
		#define LOG_FATAL(Message, ...) (Logger::Fatal(__LINE__, __FILE__, Message, __VA_ARGS__))
		#define LOG_ERROR(Message, ...) (Logger::Error(__LINE__, __FILE__, Message, __VA_ARGS__))
		#define LOG_INFO(Message, ...) (Logger::Info(__LINE__, __FILE__, Message, __VA_ARGS__))
		#define LOG_WARN(Message, ...) (Logger::Warning(__LINE__, __FILE__, Message, __VA_ARGS__))
		#define LOG_DEBUG(Message, ...) (Logger::Debug(__LINE__, __FILE__, Message, __VA_ARGS__))
		#define LOG_VERBOSE(Message, ...) (Logger::Verbose(__LINE__, __FILE__, Message, __VA_ARGS__))
		#define LOG_TRACE(Message, ...) (Logger::Trace(__LINE__, __FILE__, Message, __VA_ARGS__))
		#define WLOG_QUIET(Message, ...) (Logger::Quiet(__LINE__, _T(__FILE__), Message, __VA_ARGS__))
		#define WLOG_FATAL(Message, ...) (Logger::Fatal(__LINE__, _T(__FILE__), Message, __VA_ARGS__))
		#define WLOG_ERROR(Message, ...) (Logger::Error(__LINE__, _T(__FILE__), Message, __VA_ARGS__))
		#define WLOG_INFO(Message, ...) (Logger::Info(__LINE__, _T(__FILE__), Message, __VA_ARGS__))
		#define WLOG_WARN(Message, ...) (Logger::Warning(__LINE__, _T(__FILE__), Message, __VA_ARGS__))
		#define WLOG_DEBUG(Message, ...) (Logger::Debug(__LINE__, _T(__FILE__), Message, __VA_ARGS__))
		#define WLOG_VERBOSE(Message, ...) (Logger::Verbose(__LINE__, _T(__FILE__), Message, __VA_ARGS__))
		#define WLOG_TRACE(Message, ...) (Logger::Trace(__LINE__, _T(__FILE__), Message, __VA_ARGS__))

		void log_test(int n) {
			WLOG_DEBUG(_T("fatih"), n, _T("Write Args"), 3434);
			WLOG_WARN(_T("warning %d"), n, "sdsdsd", _T("fatih"), 7853);
			LOG_ERROR("error %d", n, "Args Errrrrrrorrrrr");
			LOG_FATAL("fatal error %d", n);
			LOG_TRACE("Trace : %d", n);
			LOG_QUIET("QUIET %d", n);
		}
	}
}
