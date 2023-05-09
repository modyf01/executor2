#pragma once

#include <string>
#include <vector>

enum class CommandType {
    RUN,
    OUT,
    ERR,
    KILL,
    SLEEP,
    QUIT,
    UNKNOWN
};

struct ParsedCommand {
    CommandType type;
    std::string program;
    std::vector<std::string> args;
    int taskId;
    int sleepTimeMs;
};

class InputParser {
public:
    ParsedCommand parse(const std::string& inputLine);
private:
    std::vector<std::string> split(const std::string& str, char delimiter);
};
