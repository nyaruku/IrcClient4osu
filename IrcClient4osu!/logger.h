#pragma once
#include "extension.h"

// actual console logging
void logError(const char* error, int code) {
	// Get the current system time
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	// Convert to a time structure
	std::tm timeInfo;
	localtime_s(&timeInfo, &currentTime);

	// Format the date and time as strings
	char dateBuffer[20];
	strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", &timeInfo);

	char timeBuffer[9];
	strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeInfo);

	std::cout << "\033[1m\033[38;2;255;255;255m[" << dateBuffer << " " << timeBuffer << "]\033[0m " << "\033[38;2;255;255;255m[\033[48;2;255;0;0m" << code << "\033[0m\033[38;2;255;255;255m] \033[38;2;255;0;0m" << error << "\033[0m" << std::endl;
}

// actual console logging
void logToConsole(std::string content) {
	// Get the current system time
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	// Convert to a time structure
	std::tm timeInfo;
	localtime_s(&timeInfo, &currentTime);

	// Format the date and time as strings
	char dateBuffer[20];
	strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", &timeInfo);

	char timeBuffer[9];
	strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeInfo);

	std::cout << "\033[1m\033[38;2;255;255;255m[" << dateBuffer << " " << timeBuffer << "]\033[0m  " << content << std::endl;
}
