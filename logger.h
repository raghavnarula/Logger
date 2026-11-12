#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

enum class Level { INFO, WARNING, ERROR };

inline void logMessage(Level level, const std::string& message) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    // Format time string
    std::string levelStr;
    switch (level) {
        case Level::INFO:    levelStr = "INFO"; break;
        case Level::WARNING: levelStr = "WARN"; break;
        case Level::ERROR:   levelStr = "ERROR"; break;
    }

    // Print to Console with timestamp
    std::cout << "[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << "] "
              << "[" << levelStr << "] " << message << std::endl;

    // Optional: Also save to a file named "app.log"
    std::ofstream file("app.log", std::ios::app);
    if (file.is_open()) {
        file << "[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << "] "
             << "[" << levelStr << "] " << message << std::endl;
    }
}

// Convenient helper macros
#define LOG_INFO(msg) logMessage(Level::INFO, msg)
#define LOG_WARN(msg) logMessage(Level::WARNING, msg)
#define LOG_ERR(msg)  logMessage(Level::ERROR, msg)