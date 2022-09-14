#include "ErrorHandler.hpp"
#include "utils.h"
#include "intercambio.h"

void ErrorHandler::SyntaxError (int num, const std::string & desc) {
	++errors_count;
	SynError_impl(num,desc);
}

void ErrorHandler::SyntaxError (int num, const std::string & desc, CodeLocation loc) {
	++errors_count;
	SynError_impl(num,desc,loc);
}

void ErrorHandler::ExecutionError (int num, const std::string & desc) {
	++errors_count;
	ExeError_impl(num,desc);
}

void ErrorHandler::AnytimeError (int num, const std::string & desc) {
	if (Inter.IsRunning()) ExecutionError(num,desc);
	else                   SyntaxError(num,desc);
}

void ErrorHandler::ErrorIfRunning(int num, const std::string & desc) {
	if (Inter.IsRunning()) ExecutionError(num,desc);
}
