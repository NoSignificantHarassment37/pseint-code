#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H
#include <string>
#include "Code.h"

class ErrorHandler {
public:
	void SyntaxError(int num, const std::string &desc);
	void SyntaxError(int num, const std::string &desc, CodeLocation loc);
	void ExecutionError(int num, const std::string &desc);
	void CompileTimeWarning(int num, const std::string &desc);
	void RunTimeWarning(int num, const std::string &desc);
	void ErrorIfRunning(int num, const std::string &desc);
	void AnytimeError(int num, const std::string &desc);
	bool IsOk() const { return m_errors_count==0; }
	int GetErrorsCount() const { return m_errors_count; }
	
	void SetForRealTimeSyntax() { m_for_realtime_syntax = true; }
private:
	int m_errors_count = 0, m_warnings_count = 0;
	bool m_for_realtime_syntax = false;
};

#endif

