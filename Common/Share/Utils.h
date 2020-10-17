#pragma once
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <limits>

namespace utils
{

void setServerLogLevel(const std::string& level);

/// Determine if the string is an integer
bool isInteger(const std::string &str);

/// Split the string into multiple separated strings.
/**
 * @param acceptEmptyString if true, empty strings are accepted in the
 * result, for example, splitting the ",1,2,,3," by "," produces
 * ["","1","2","","3",""]
 */
std::vector<std::string> splitString(const std::string& s,
    const std::string& delimiter,
    bool acceptEmptyString = false);

std::set<std::string> splitStringToSet(const std::string& str,
    const std::string& separator);

std::string charToHex(char c);

    /// Get a formatted string
std::string formattedString(const char *format, ...);

/// Generate random a string
/**
 * @param length The string length
 * The returned string consists of uppercase and lowercase letters and numbers
 */
std::string genRandomString(int length);

/// Convert a binary string to hex format
std::string binaryStringToHex(const unsigned char *ptr, size_t length);

/// Get a binary string from hexadecimal format
std::string hexToBinaryString(const char *ptr, size_t length);

/// Get a binary vector from hexadecimal format
std::vector<char> hexToBinaryVector(const char *ptr, size_t length);

bool bytesSize(std::string sizeStr, size_t &size);

std::string demangle(const char *mangled_name);

/// Recursively create a file system path
/**
 * Return 0 or -1 on success or failure.
 */
int createPath(const std::string& path);

}  // namespace utils
