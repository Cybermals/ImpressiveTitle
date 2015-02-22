#pragma once

#include <sstream>
static long s_debugLineCount = 0;


template <typename T>
void debugTrace(T msg)
{
	std::wstringstream debug;
	debug << msg;
	debugTraceStr(debug);
}

void printStack();

void debugTraceStr(const std::wstringstream& item);
void debugTraceVars(const char *format, ...);


#   define DEBUG_TRACE(msg)  debugTrace(msg)
#   define DEBUG_TRACE_S(args) debugTraceVars args
