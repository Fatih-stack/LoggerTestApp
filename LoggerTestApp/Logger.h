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
			template<typename ... Args>
			std::wstring wstringer(const Args& ... args)
			{
				std::wostringstream oss;
				int a[] = { 0, ((void)(oss << args), 0) ... };
				return oss.str();
			}

			template<typename ... Args>
			std::string stringer(const Args& ... args)
			{
				std::ostringstream oss;
				int a[] = { 0, ((void)(oss << args), 0) ... };
				return oss.str();
			}

			template <typename T>
			void log_writefile(const T& value) {
				file.close();
				file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
				if (typeid(value) == typeid(wchar_t const* __ptr64)) {
					std::wstring temp = wstringer(value);
					std::string res(temp.begin(), temp.end());
					file << res.c_str() << " ";
					return;
				}
				file << value << " ";
			}

		public:
			static void SetPriority(Severity new_severity)
			{
				get_instance().severity = new_severity;
			}

			template<typename T, typename... Args>
			static void Quiet(int line, const std::string source_file, const T message, Args... args)
			{
				get_instance().log(line, source_file, "[Quiet]\t", Severity::Quiet, message, args...);
			}

			template<typename T, typename... Args>
			static void Fatal(int line, const std::string source_file, const T message, Args... args)
			{
				get_instance().log(line, source_file, "[Fatal]\t", Severity::Fatal, message, args...);
			}

			template<typename T, typename... Args>
			static void Error(int line, const std::string source_file, const T message, Args... args)
			{
				get_instance().log(line, source_file, "[Error]\t", Severity::Error, message, args...);
			}

			template<typename T, typename... Args>
			static void Warning(int line, const std::string source_file, const T message, Args... args)
			{
				get_instance().log(line, source_file, "[Warn]\t", Severity::Warning, message, args...);
			}

			template<typename T, typename... Args>
			static void Info(int line, const std::string source_file, const T message, Args... args)
			{
				get_instance().log(line, source_file, "[Info]\t", Severity::Info, message, args...);
			}

			template<typename T, typename... Args>
			static void Verbose(int line, const std::string source_file, const T message, Args... args)
			{
				get_instance().log(line, source_file, "[Verbose]\t", Severity::Verbose, message, args...);
			}

			template<typename T, typename... Args>
			static void Debug(int line, const std::string source_file, const T message, Args... args)
			{
				get_instance().log(line, source_file, "[Debug]\t", Severity::Debug, message, args...);
			}

			template<typename T, typename... Args>
			static void Trace(int line, const std::string source_file, const T message, Args... args)
			{
				get_instance().log(line, source_file, "[Trace]\t", Severity::Trace, message, args...);
			}

		public:
			Logger() {}

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

			template<typename T, typename... Args>
			void log(int line, const std::string& source, const std::string& msg_priorty_str,
				Severity msg_severity, const T& msg, Args... args)
			{
				if (severity <= msg_severity)
				{
					time_t current_time = time(0);
					tm* timestamp = localtime(&current_time);
					char buffer[80];
					strftime(buffer, 80, "%c", timestamp);
					std::string s(buffer);
					typename std::lock_guard lock(log_mutex);
					std::string m_source, m_msg;
					if (file.is_open())
					{
						file.close();
						file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
						if (typeid(msg) == typeid(wchar_t const* __ptr64)) {
							std::wstring tmp = wstringer(msg);
							std::string resT(tmp.begin(), tmp.end());
							m_msg = resT;
						}
						else {
							std::string res = stringer(source);
							m_source = res;
							std::string resT = stringer(msg);
							m_msg = resT;
						}
						file << s.c_str() << '\t' << msg_priorty_str.c_str() << " " << m_msg.c_str() << " ";
						int dummy[] = { 0, ((void)log_writefile(std::forward<Args>(args)),0)... };
						file << " on line " << line << " in " << m_source.c_str() << "\n";
					}
					else
						AfxMessageBox(_T("Logger: Failed to open file "));
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

		void log_test(int n) {
			LOG_DEBUG(_T("fatih"), n, _T("Write Args"), 3434);
			LOG_WARN(_T("warning %d"), n, "sdsdsd", _T("fatih"), 7853);
			LOG_ERROR("error %d", n, "Args Errrrrrrorrrrr");
			LOG_FATAL("fatal error %d", n);
			LOG_TRACE("Trace : %d", n);
			LOG_QUIET("QUIET %d", n);
		}
	}
}
