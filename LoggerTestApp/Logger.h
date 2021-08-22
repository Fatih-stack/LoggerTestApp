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
			Quiet = 0, Fatal = 8, Error = 16, Warn = 24, Info = 32, Verb = 40, Debug = 48, Trace = 56
		};

		class Logger
		{
		private:
			Severity severity = Severity::Quiet;
			std::mutex log_mutex;
			std::string file_path = "";
			std::fstream file;

		protected:
			// template function converts arguments(wchar_t) to std::wstring type
			//input any type args but can't take char type because wostringstream for wchar_t
			template<typename ... Args>
			std::wstring wstringer(const Args& ... args)
			{
				std::wostringstream oss;
				int a[] = { 0, ((void)(oss << args), 0) ... };
				return oss.str();
			}

			// template function converts arguments(char_t, other args) to std::string type
			//input any type args but can't take wchar_t type because ostringstream for char_t
			template<typename ... Args>
			std::string stringer(const Args& ... args)
			{
				std::ostringstream oss;
				int a[] = { 0, ((void)(oss << args), 0) ... };
				return oss.str();
			}

			/*****************************************************************************************
			* Def : template function take any type input and convert it to std::string to write file
			* Args : value => specify any type input to write file
			* Output : writes given input to file
			******************************************************************************************/
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
			//severity has default value but developer can set desired severity value manually
			static void SetPriority(Severity new_severity)
			{
				instance().severity = new_severity;
			}

			/***********************************************************************************************
			* Def : Template function developer can write any severity type log in one function by changing
			* not constant parameter Severity type and severity msg
			* Args : line => lineNumber, source_file => source File path, msg_priorty_str => Severity as string
			* msg_severity => severity level to check whether severity bigger or not, 
			* message => first user message, args => any type args (wchar,char, int ...)
			* Output : Log text file which includes all logs in chosen path from GUI
			***********************************************************************************************/
			template<typename T, typename... Args>
			static void Any(int line, const std::string source_file, const std::string& msg_priorty_str,
				Severity msg_severity, const T message, Args... args)
			{
				instance().log(line, source_file, msg_priorty_str, msg_severity, message, args...);
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

			static Logger& instance()
			{
				static Logger logger;
				return logger;
			}

			/********************************************************************************************
			* Def : Takes arguments which will be written to file and format them to write file properly
			* like log. (Log format : Date, Severity type, Args, line, File)
			* Args : line => lineNumber, source => source File path, msg_priorty_str => Severity as string
			* msg_severity => severity to check whether severity bigger or not, msg => first user message
			* args => any type args (wchar,char, int ...)
			* Output : Log text file which includes all logs in chosen path from GUI 
			*********************************************************************************************/
			template<typename T, typename... Args>
			void log(int line, const std::string& source, const std::string& msg_priorty_str,
				Severity msg_severity, const T& msg, Args... args)
			{
				if (severity <= msg_severity)	//check severity
				{
					//Date operation take current time : Day, Month, Hour:Minute:Sec, Year
					time_t current_time = time(0);
					tm* timestamp = localtime(&current_time);
					char buffer[80];
					//convert time to char array then char array to std::string
					strftime(buffer, 80, "%c", timestamp);
					std::string s(buffer);
					//define std::lock_guard to support multithreading
					typename std::lock_guard lock(log_mutex);
					std::string m_msg;
					if (file.is_open())
					{
						file.close();
						file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
						//check type wchar or char convert std::string to write file
						if (typeid(msg) == typeid(wchar_t const* __ptr64)) {	
							std::wstring tmp = wstringer(msg);
							std::string resT(tmp.begin(), tmp.end());
							m_msg = resT;
						}
						else {
							std::string resT = stringer(msg);
							m_msg = resT;
						}
						//write file given inputs in a proper format for logging
						//(time, severity, message, args, line and source file path)
						file << s.c_str() << '\t' << msg_priorty_str.c_str() << " " << m_msg.c_str() << " ";
						//write all type args to file
						int dummy[] = { 0, ((void)log_writefile(std::forward<Args>(args)),0)... };
						file << " on line " << line << " in " << source.c_str() << "\n";
					}
					else
						AfxMessageBox(_T("Logger: Failed to open file "));
				}
			}

			//close file if open if don't close, don't write file
			void close_file()
			{
				if (file.is_open())
					file.close();
			}

			//create or open file in a give path
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

		/******************************************************************************************
		* Define macros to provide usefulness to developer Line and File default no need to write 
		* again and again so file and line are given developers just write their messages by using
		* macros which type developer need according to severity
		*******************************************************************************************/
		#define LOG_QUIET(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Quiet]\t", Severity::Quiet, msg, __VA_ARGS__))
		#define LOG_FATAL(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Fatal]\t", Severity::Fatal, msg, __VA_ARGS__))
		#define LOG_ERROR(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Error]\t", Severity::Error, msg, __VA_ARGS__))
		#define LOG_INFO(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Info]\t", Severity::Info, msg, __VA_ARGS__))
		#define LOG_WARN(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Warn]\t", Severity::Warn, msg, __VA_ARGS__))
		#define LOG_DEBUG(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Debug]\t", Severity::Debug, msg, __VA_ARGS__))
		#define LOG_VERB(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Verbose]\t", Severity::Verb, msg, __VA_ARGS__))
		#define LOG_TRACE(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Trace]\t", Severity::Trace, msg, __VA_ARGS__))

		//Test function to test whether Logger class run properly
		//input n: specify thread number
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
