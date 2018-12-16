#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <sstream>

#include "appstatus.h"

enum LogLevel
{
  Info,
  Warning,
  Error
};

class Debug
{
public:
  static void Log(LogLevel level, std::string message);
  static void GLFWError(int error, const char* description);
  static void LogStatus(AppStatus status);

private:
    
};

#endif
