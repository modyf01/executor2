#include "utils.h"

std::vector<std::string> split_string(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::string current_token;

    for (char c : input) {
        if (c == delimiter) {
            if (!current_token.empty()) {
                result.push_back(current_token);
                current_token.clear();
            }
        } else {
            current_token.push_back(c);
        }
    }

    if (!current_token.empty()) {
        result.push_back(current_token);
    }

    return result;
}
