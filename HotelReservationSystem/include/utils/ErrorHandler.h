#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include "../core/HotelException.h" // Adjust path if necessary

class ErrorHandler {
public:
    // ANSI Color Codes for Professional Console Output
    static constexpr const char* RESET = "\033[0m";
    static constexpr const char* RED = "\033[31m";
    static constexpr const char* YELLOW = "\033[33m";
    static constexpr const char* CYAN = "\033[36m";
    static constexpr const char* BOLD = "\033[1m";

    // Handle HotelExceptions
    static void handle(const HotelException& e) {
        logToFile(to_string(e.getCode()), e.what()); // Log first
        display(e);   // Then display
    }

    // Handle generic std::exceptions
    static void handle(const std::exception& e) {
        std::cerr << RED << BOLD << "\n💥 ERREUR SYSTÈME IMPRÉVUE: " << e.what() << RESET << std::endl;
        logToFile("SYSTEM_ERROR", e.what());
    }

private:
    static void display(const HotelException& e) {
        std::cout << "\n";
        switch(e.getLevel()) {
            case ErrorLevel::WARNING:
                // Yellow for warnings (user errors like "wrong password")
                std::cout << YELLOW << "⚠️  ATTENTION (Code " << e.getCode() << "): " << RESET;
                std::cout << YELLOW << e.what() << RESET << "\n" << std::endl;
                break;
            case ErrorLevel::ERR:
                // Red for operation errors
                std::cout << RED << "❌ ERREUR (Code " << e.getCode() << "): " << RESET;
                std::cout << RED << e.what() << RESET << "\n" << std::endl;
                break;
            case ErrorLevel::FATAL:
                // Bold Red for crashes
                std::cout << RED << BOLD << "💀 ERREUR CRITIQUE (Code " << e.getCode() << "): " << RESET;
                std::cout << RED << e.what() << RESET << "\n" << std::endl;
                break;
            default:
                std::cout << CYAN << "ℹ️  INFO: " << e.what() << RESET << "\n" << std::endl;
        }
    }

    // Writes errors to 'app_errors.log' with timestamp
    static void logToFile(const string& code, const string& msg) {
        std::ofstream logFile("app_errors.log", std::ios::app);
        if (logFile.is_open()) {
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char buf[100];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            
            logFile << "[" << buf << "] [CODE:" << code << "] " << msg << std::endl;
        }
    }
};

#endif