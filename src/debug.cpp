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
    std::string output;
    std::stringstream stream;

    switch(status)
    {
    case AppStatus::FailureGLFWInit:
        stream << "Failed to initialize GLFW";
        break;

    case AppStatus::FailureWindow:
        stream << "Failed to initialize window";
        break;

    case AppStatus::FailureGLEWInit:
        stream << "Failed to initialize GLEW";
        break;

    case AppStatus::FailureShaderLoad:
        stream << "Failed to load shader";
        break;

    case AppStatus::FailureShaderProgramLoad:
        stream << "Failed to load shader program";
        break;

    case AppStatus::FailureSampleSet:
        stream << "Failed to set sample";
        break;

    case AppStatus::FailureTextureLoad:
        stream << "Failed to load texture";
        break;

    case AppStatus::Success:
        // stream << ""; // No need to log this
        break;

    default:
        stream << "Unhandled status, can't log! ID: " << status;
    }

    stream >> output;

    std::cout << output << std::endl;
}
