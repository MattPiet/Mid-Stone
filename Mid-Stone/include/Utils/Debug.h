#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

enum class Message_type : uint8_t {
	type_none = 0,
	type_fatal_error,
	type_error,
	type_warning,
	type_trace,
	type_info
};

class Debug {
public:
	Debug(const Debug&) = delete;
	Debug(Debug&&) = delete;
	Debug& operator=(const Debug&) = delete;
	Debug& operator=(Debug&&) = delete;

	Debug() = delete;
	
	static void DebugInit(const std::string& fileName_);

	static void Info(const std::string& message_,
		const std::string& fileName_, const int line_);

	static void Trace(const std::string& message_,
		const std::string& fileName_, const int line_);

	static void Warning(const std::string& message_,
		const std::string& fileName_, const int line_);

	static void Error(const std::string& message_,
		const std::string& fileName_, const int line_);

	static void FatalError(const std::string& message_,
		const std::string& fileName_, const int line_);

private:
	static void Log(const Message_type type_,
		const std::string& message_,
		const std::string& fileName_,
		const int line_);
	static std::string logFileName;
};

#endif // !DEBUG_H