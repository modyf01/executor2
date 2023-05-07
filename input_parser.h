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
    INVALID
};

struct Command {
    CommandType type;
    std::string program;
    std::vector<std::string> args;
    int task_id;
    int sleep_duration;
};

class InputParser {
public:
    static Command parse(const std::string& line);
};
