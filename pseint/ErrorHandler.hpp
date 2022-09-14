#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H
#include <string>
#include "Code.h"

class ErrorHandler {
public:
	void SyntaxError(int num, const std::string &desc);
	void SyntaxError(int num, const std::string &desc, CodeLocation loc);
	void ExecutionError(int num, const std::string &desc);
	void ErrorIfRunning(int num, const std::string &desc);
	void AnytimeError(int num, const std::string &desc);
	bool IsOk() const { return errors_count==0; }
	int GetErrorsCount() const { return errors_count; }
private:
	int errors_count = 0;
};

#endif

