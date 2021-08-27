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

		//structs are defined for operator overloadings

		struct loggerFile
		{
			loggerFile(std::wstring t, std::wstring m, std::wstring m_p) 
				: w_time(t), w_msg_priority(m_p), w_msg(m) {}
			loggerFile(std::string t, std::string m, std::string m_p) : time(t), msg_priority(m_p), msg(m) {}
			friend std::wofstream& operator << (std::wofstream& wfile, const loggerFile& lF);
			friend std::ofstream& operator << (std::ofstream& file, const loggerFile& lF);
		private:
			std::wstring w_msg, w_msg_priority, w_time;
			std::string msg, msg_priority, time;
		};

		struct loggerFileLast
		{
			loggerFileLast(int line, std::wstring source) : line(line), w_source(source) {}
			loggerFileLast(int line, std::string source) : line(line), source(source) {}
			friend std::wofstream& operator << (std::wofstream& wfile, const loggerFileLast& lF);
			friend std::ofstream& operator << (std::ofstream& wfile, const loggerFileLast& lF);
		private:
			int line;
			std::wstring w_source;
			std::string source;
		};

		//operator functions are defined as below depends only struct and data type

		std::wofstream& operator << (std::wofstream& wfile, const loggerFile& lF)
		{
			wfile << lF.w_time.c_str() << L'\t' << lF.w_msg_priority.c_str() << L" " << lF.w_msg.c_str() << L" ";
			return wfile;
		}

		std::wofstream& operator << (std::wofstream& wfile, const loggerFileLast& lF)
		{
			wfile << L" on line " << lF.line << L" in " << lF.w_source.c_str() << L"\n";
			return wfile;
		}

		std::ofstream& operator << (std::ofstream& file, const loggerFile& lF)
		{
			file << lF.time.c_str() << '\t' << lF.msg_priority.c_str() << " " << lF.msg.c_str() << " ";
			return file;
		}

		std::ofstream& operator << (std::ofstream& file, const loggerFileLast& lF)
		{
			file << " on line " << lF.line << " in " << lF.source.c_str() << "\n";
			return file;
		}

		class Logger
		{
		private:
			Severity severity = Severity::Quiet;
			std::mutex log_mutex;
			std::mutex log_mutex2;
			std::string file_path = "";
			std::wstring wfile_path = L"";

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
				typename std::lock_guard lock2(log_mutex2);
				std::ofstream file(file_path, std::ios_base::app);
				if (typeid(value) == typeid(wchar_t const* __ptr64)) {
					std::wstring temp = wstringer(value);
					std::string res(temp.begin(), temp.end());
					file << res.c_str() << " ";
					return;
				}
				file << value << " ";
			}
			
			//for wchar_t expansion no need to convert to std::string 
			//std::wofstream is used to write file wchar_t type variables
			template <typename T>
			void wlog_writefile(const T& value) {
				std::wofstream file(wfile_path, std::ios_base::app);
				typename std::lock_guard lock2(log_mutex2);
				std::wstring temp = wstringer(value);
				file << temp.c_str() << " ";
			}

			//for std::string type args to write file
			template <typename T>
			void slog_writefile(const T& value) {
				typename std::lock_guard lock2(log_mutex2);
				std::ofstream file(file_path, std::ios_base::app);
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
			static void Any(int line, const std::string& source_file, const std::string& msg_priorty_str,
				const Severity& msg_severity, const T& message, Args... args)
			{
				instance().log(line, source_file, msg_priorty_str, msg_severity, message, args...);
			}

			//template is used just for wchar_t type expansion operations
			template<typename... Args>
			static void AnyW(int line, const std::wstring& source_file, const std::wstring& msg_priorty_str,
				const Severity& msg_severity, const std::wstring& message, Args... args)
			{
				instance().wlog(line, source_file, msg_priorty_str, msg_severity, message, args...);
			}

			//template is used just for std::string type expansion operations
			template<typename... Args>
			static void AnyS(int line, const std::string& source_file, const std::string& msg_priorty_str,
				const Severity& msg_severity, const std::string& message, Args... args)
			{
				instance().slog(line, source_file, msg_priorty_str, msg_severity, message, args...);
			}

		public:
			Logger() {}

			Logger(const Logger&) = delete;
			Logger& operator= (const Logger&) = delete;

			~Logger() {}

			//define instance to make class singleton type because template functions are static 
			//so not needed to create class type variables in order to use these functions 
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
			void log(int line, std::string source, const std::string& msg_priorty_str,
				const Severity& msg_severity, T msg, Args... args)
			{
				if (severity <= msg_severity)	//check severity
				{
					//define std::lock_guard to support multithreading
					typename std::lock_guard lock(log_mutex);
					std::string m_msg;
					
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
					//Date operation take current time : Day, Month, Hour:Minute:Sec, Year
					time_t current_time = time(0);
					tm* timestamp = localtime(&current_time);
					char buffer[80];
					//convert time to char array then char array to std::string
					strftime(buffer, 80, "%c", timestamp);
					std::string s(buffer);
					//write file given inputs in a proper format for logging
					//(time, severity, message, args, line and source file path)
					{
						typename std::lock_guard lock2(log_mutex2);
						std::ofstream file(file_path, std::ios_base::app);
						file << s.c_str() << '\t' << m_msg.c_str() << " " << msg_priorty_str.c_str() << " ";
					}
					//write all type args to file
					int dummy[] = { 0, ((void)log_writefile(std::forward<Args>(args)),0)... };
					typename std::lock_guard lock2(log_mutex2);
					std::ofstream file(file_path, std::ios_base::app);
					file << " on line " << line << " in " << source.c_str() << "\n";					
				}
			}

			//same as log function difference is that below function just for wchar_t types
			template<typename... Args>
			void wlog(int line, const std::wstring& source, std::wstring msg_priorty_str,
				const Severity& msg_severity, const std::wstring& msg, Args... args)
			{
				if (severity <= msg_severity)	//check severity
				{					
					//Date operation take current time : Day, Month, Hour:Minute:Sec, Year
					time_t rawtime;
					struct tm* timeinfo;
					wchar_t buffer[80];

					//define thread to support multithreading and locks it to prevent to 
					//writing different code parts to file
					typename std::lock_guard lock(log_mutex);
					time(&rawtime);
					timeinfo = localtime(&rawtime);

					//take time in wchar_t type
					wcsftime(buffer, 80, L"%c", timeinfo);
					std::wstring wtime = wstringer(buffer);

					//write file given inputs in a proper format for logging
					//(time, severity, message, args, line and source file path)
					{
						//define std::lock_guard to support multithreading
						typename std::lock_guard lock2(log_mutex2);
						std::wofstream file(wfile_path, std::ios_base::app);
						loggerFile lF(wtime, msg_priorty_str, msg);
						file << lF;
					}

					//write args to file
					int dummy[] = { 0, ((void)wlog_writefile(std::forward<Args>(args)),0)... };

					//open file and write given inputs to file
					std::wofstream file(wfile_path, std::ios_base::app);
					loggerFileLast lF(line, source);
					file << lF;
				}
			}

			//same as log function difference is that below function just support std::string types
			template<typename... Args>
			void slog(int line, const std::string& source, const std::string& msg_priorty_str,
				const Severity& msg_severity, const std::string& msg, Args... args)
			{
				if (severity <= msg_severity)	//check severity
				{
					//define std::lock_guard to support multithreading
					typename std::lock_guard lock(log_mutex);
					
					//Date operation take current time : Day, Month, Hour:Minute:Sec, Year
					time_t current_time = time(0);
					tm* timestamp = localtime(&current_time);
					char buffer[80];
					//convert time to char array then char array to std::string
					strftime(buffer, 80, "%c", timestamp);
					std::string time(buffer);

					//write file given inputs in a proper format for logging 
					//(time, severity, message, args, line and source file path)
					//by locking with lock_guard to prevent write other things
					{
						typename std::lock_guard lock2(log_mutex2);
						std::ofstream file(file_path, std::ios_base::app);
						loggerFile lF(time, msg_priorty_str, msg);
						file << lF;
					}
					//write all type args to file
					int dummy[] = { 0, ((void)slog_writefile(std::forward<Args>(args)),0)... };

					//write last part of log row by locking with lock_guard to prevent write other things
					typename std::lock_guard lock2(log_mutex2);
					std::ofstream file(file_path, std::ios_base::app);
					loggerFileLast lF(line, source);
					file << lF;
				}
			}

			//create or open file in a give path
			void create_open_file(const std::string& new_file_path)
			{
				file_path = new_file_path;
				std::ofstream file(file_path, std::ios_base::app);
				if (!file.is_open())
				{
					AfxMessageBox(_T("Logger: Failed to open file "));
				}
			}

			//create or open file in a give path wstring type path and file
			void create_open_file(const std::wstring& new_file_path)
			{
				wfile_path = new_file_path;
				std::wofstream file(file_path, std::ios_base::app);
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
		
		//in order to use L instead of _T() macro
		#define WIDE2(x) L##x
		#define WIDE1(x) WIDE2(x)
		#define WFILE WIDE1(__FILE__)

		//Macros are defined just for wchar_t expansion
		#define WLOG_QUIET(msg, ...) (Logger::AnyW(__LINE__, WFILE, L"[Quiet]\t",Severity::Quiet,msg,__VA_ARGS__))
		#define WLOG_FATAL(msg, ...) (Logger::AnyW(__LINE__, WFILE, L"[Fatal]\t",Severity::Fatal,msg,__VA_ARGS__))
		#define WLOG_ERROR(msg, ...) (Logger::AnyW(__LINE__, WFILE, L"[Error]\t",Severity::Error,msg,__VA_ARGS__))
		#define WLOG_INFO(msg, ...) (Logger::AnyW(__LINE__, WFILE, L"[Info]\t",Severity::Info,msg,__VA_ARGS__))
		#define WLOG_WARN(msg, ...) (Logger::AnyW(__LINE__, WFILE, L"[Warn]\t",Severity::Warn,msg,__VA_ARGS__))
		#define WLOG_DEBUG(msg, ...) (Logger::AnyW(__LINE__, WFILE, L"[Debug]\t",Severity::Debug,msg,__VA_ARGS__))
		#define WLOG_VERB(msg, ...) (Logger::AnyW(__LINE__, WFILE, L"[Verbose]\t",Severity::Verb,msg,__VA_ARGS__))
		#define WLOG_TRACE(msg, ...) (Logger::AnyW(__LINE__, WFILE, L"[Trace]\t",Severity::Trace,msg,__VA_ARGS__))

		//Macros are defined just for char_t expansion
		#define SLOG_QUIET(msg, ...) (Logger::AnyS(__LINE__,__FILE__,"[Quiet]\t",Severity::Quiet,msg,__VA_ARGS__))
		#define SLOG_FATAL(msg, ...) (Logger::AnyS(__LINE__,__FILE__,"[Fatal]\t",Severity::Fatal,msg,__VA_ARGS__))
		#define SLOG_ERROR(msg, ...) (Logger::AnyS(__LINE__,__FILE__,"[Error]\t",Severity::Error,msg,__VA_ARGS__))
		#define SLOG_INFO(msg, ...) (Logger::AnyS(__LINE__,__FILE__,"[Info]\t", Severity::Info, msg, __VA_ARGS__))
		#define SLOG_WARN(msg, ...) (Logger::AnyS(__LINE__,__FILE__,"[Warn]\t", Severity::Warn, msg, __VA_ARGS__))
		#define SLOG_DEBUG(msg, ...) (Logger::AnyS(__LINE__,__FILE__,"[Debug]\t", Severity::Debug, msg, __VA_ARGS__))
		#define SLOG_VERB(msg, ...) (Logger::AnyS(__LINE__,__FILE__,"[Verbose]\t", Severity::Verb, msg, __VA_ARGS__))
		#define SLOG_TRACE(msg, ...) (Logger::AnyS(__LINE__,__FILE__,"[Trace]\t", Severity::Trace, msg, __VA_ARGS__))
	
		//Test function to test whether Logger class run properly
		//input n: specify thread number
		void log_test(int n) {
			WLOG_DEBUG(L"fatih %d", n, L"different args");
			SLOG_WARN("warn %d", n, "Warningggggg", 34);
			LOG_ERROR("error %d", n, L"Error writing", 43, "fatih");
			LOG_FATAL(L"fatal %d", n, "Fatal Error in writing");
			WLOG_TRACE(L"Trace %d", n, L"trace reading", 2345);
			SLOG_QUIET("QUIET %d", n);
		}
	}
}
