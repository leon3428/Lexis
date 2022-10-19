#include "Utils.hpp"

std::string utils::trim(const std::string& s) {
    int start = 0;
    while(start < s.size() && (s[start] == ' ' || s[start] == '\t'))
        start++;

    int end = s.size() - 1;
    while(end >= 0 && (s[end] == ' ' || s[end] == '\t'))
        end--;

    return s.substr(start, end - start + 1);
}