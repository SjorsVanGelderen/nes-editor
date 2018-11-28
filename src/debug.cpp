#include "debug.h"

void Debug::Log(LogLevel level, std::string message)
{
    auto levelString = level == Info ? "INFO"
        : level == Warning ? "WARNING"
        : "ERROR";
    
    std::cout << levelString
              << ": "
              << message
              << std::endl;
}

void Debug::GLFWError(int error, const char* description)
{
    std::cout << "GLFW internal error: "
              << error
              << " "
              << std::string(description)
              << std::endl;
}

void Debug::LogStatus(AppStatus status)
{
    switch(status)
    {
    case AppStatus::FailureGLFWInit:
        std::cout << "Failed to initialize GLFW" << std::endl;
        break;

    case AppStatus::FailureWindow:
        std::cout << "Failed to initialize window" << std::endl;
        break;

    case AppStatus::FailureGLEWInit:
        std::cout << "Failed to initialize GLEW" << std::endl;
        break;

    case AppStatus::FailureShaderLoad:
        std::cout << "Failed to load shader" << std::endl;
        break;

    case AppStatus::FailureShaderProgramLoad:
        std::cout << "Failed to load shader program" << std::endl;
        break;

    case AppStatus::FailureSampleSet:
        std::cout << "Failed to set sample" << std::endl;

    case AppStatus::Success:
        // std::cout << "All good" << std::endl;
        break;

    default:
        std::cout << "Unhandled status, can't log! ID: " << status << std::endl;
    }
}
