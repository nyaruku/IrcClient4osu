#pragma once
// MOST OF THIS SHIT IS MADE WITH HELP OF CHAT GPT BRUH
#include <iostream>
#include <cstdarg> // for va_list and va_start
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include <charconv>
#include <string>

// Function to calculate the length of a null-terminated char*
size_t custom_strlen(const char* str) {
	size_t length = 0;
	while (str[length] != '\0') {
		length++;
	}
	return length;
}

// Function to concatenate multiple null-terminated char* strings
char* custom_strcat(size_t numStrings, ...) {
	// Calculate the total length of the concatenated string
	size_t totalLen = 0;
	va_list args;
	va_start(args, numStrings);
	for (size_t i = 0; i < numStrings; i++) {
		const char* currentString = va_arg(args, const char*);
		totalLen += custom_strlen(currentString);
	}
	va_end(args);

	// Allocate memory for the result string
	char* result = new char[totalLen + 1]; // +1 for the null terminator

	// Copy the contents of each string to the result
	size_t currentIndex = 0;
	va_start(args, numStrings);
	for (size_t i = 0; i < numStrings; i++) {
		const char* currentString = va_arg(args, const char*);
		size_t currentLen = custom_strlen(currentString);
		for (size_t j = 0; j < currentLen; j++) {
			result[currentIndex++] = currentString[j];
		}
	}
	va_end(args);

	// Null-terminate the result string
	result[totalLen] = '\0';

	return result;
}

// Function to remove a substring from a string and return it as const char*
const char* removeSubstring(const char* inputString, const char* substringToRemove) {
	const char* startPos = strstr(inputString, substringToRemove);

	if (startPos != nullptr) {
		size_t substringLength = std::strlen(substringToRemove);
		size_t inputStringLength = std::strlen(inputString);

		// Calculate the length of the modified string
		size_t modifiedStringLength = inputStringLength - substringLength;

		// Create a new char* buffer to hold the modified string
		char* modifiedString = new char[modifiedStringLength + 1];

		// Copy the part before the substring
		strncpy(modifiedString, inputString, startPos - inputString);
		modifiedString[startPos - inputString] = '\0';

		// Copy the part after the substring
		strcat(modifiedString, startPos + substringLength);

		return modifiedString;
	}

	// If the substring is not found, return the original inputString
	return inputString;
}
// Function to check if a line starts with a specific substring
bool lineStartsWith(const char* line, const char* prefix) {
	return strncmp(line, prefix, strlen(prefix)) == 0;
}
bool endsWithChar(const std::string& input, char search) {
	return input.back() == search;
}


wchar_t* MultiByteToWideChar(const char* string) {
	// Calculate the required buffer size for the wide string
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, string, -1, NULL, 0);

	// Allocate a buffer for the wide string
	wchar_t* wideString = new wchar_t[bufferSize];

	// Convert the UTF-8 string to UTF-16 (wide string)
	MultiByteToWideChar(CP_UTF8, 0, string, -1, wideString, bufferSize);

	return wideString;
}

struct Color {
	int r, g, b;

	Color(int red, int green, int blue) : r(red), g(green), b(blue) {}

	// Linear interpolation between two colors
	static Color interpolate(const Color& start, const Color& end, double t) {
		int r = static_cast<int>((1 - t) * start.r + t * end.r);
		int g = static_cast<int>((1 - t) * start.g + t * end.g);
		int b = static_cast<int>((1 - t) * start.b + t * end.b);

		return Color(r, g, b);
	}
};

Color getColorForStep(const Color& start, const Color& end, int steps, int step) {
	if (step < 0) step = 0;
	if (step > steps) step = steps;

	double t = static_cast<double>(step) / steps;
	return Color::interpolate(start, end, t);
}

Color HexColorToRGB(const char* hexColor) {
	Color color{ 255,255,255 };
	if (hexColor[0] == '#' && strlen(hexColor) == 7) {
		std::stringstream ss;
		ss << std::hex << hexColor + 1; // Skip the '#' character

		int colorValue;
		ss >> colorValue;
		color.r = (colorValue >> 16) & 0xFF;
		color.g = (colorValue >> 8) & 0xFF;
		color.b = colorValue & 0xFF;
		ss.clear();
	}
	else {
		// Invalid hex color format, return the original color
	}

	return color;
}
std::string RGBColorToHex(Color color) {
	std::stringstream ss;
	ss << "#" << std::setfill('0') << std::setw(2) << std::hex << color.r
		<< std::setw(2) << std::hex << color.g << std::setw(2) << std::hex << color.b;
	std::string temp = ss.str();
	ss.clear();
	return temp;
}

void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::string replaceString(std::string subject, const std::string& search, const std::string& replace) {
	size_t pos = subject.find(search);
	while (pos != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos = subject.find(search, pos + replace.length());
	}
	return subject;
}


std::string FormatWithCommas(double value)
{
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << trunc(value);
	return ss.str();
}

const char* convert_to_char(int value) {
	static std::string buffer;
	buffer.clear();
	buffer.resize(10); // maximum number of digits in an int
	auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
	if (ec != std::errc{}) {
		return "0";
	}
	//ptr = '\0';
	return buffer.c_str();
}

const char* convert_to_char(float value) {
	static std::string buffer;
	buffer.clear();
	buffer.resize(20); // maximum number of digits in a float
	auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
	if (ec != std::errc{}) {
		return "0";
	}
	//	ptr = '\0';
	return buffer.c_str();
}

const char* convert_to_char(double value) {
	static std::string buffer;
	buffer.clear();
	buffer.resize(20); // maximum number of digits in a double
	auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
	if (ec != std::errc{}) {
		return "0";
	}
	//ptr = '\0';
	return buffer.c_str();
}
std::string boolToString(bool b) {
	return b ? "true" : "false";
}
const char* convert_to_char(const std::string& value) {
	return value.c_str();
}

bool string_contains(std::string input, std::string key) {
	if (input.find(key) != std::string::npos) {
		return true;
	}
	return false;
}
