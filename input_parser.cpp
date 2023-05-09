#include "input_parser.h"
#include <sstream>

ParsedCommand InputParser::parse(const std::string& inputLine) {
    ParsedCommand command;
    command.type = CommandType::UNKNOWN;

    std::vector<std::string> tokens = split(inputLine, ' ');

    if (tokens.empty()) {
        return command;
    }

    if (tokens[0] == "run") {
        command.type = CommandType::RUN;
        if (tokens.size() > 1) {
            command.program = tokens[1];
            for (size_t i = 2; i < tokens.size(); ++i) {
                command.args.push_back(tokens[i]);
            }
        }
    } else if (tokens[0] == "out") {
        command.type = CommandType::OUT;
        if (tokens.size() > 1) {
            command.taskId = std::stoi(tokens[1]);
        }
    } else if (tokens[0] == "err") {
        command.type = CommandType::ERR;
        if (tokens.size() > 1) {
            command.taskId = std::stoi(tokens[1]);
        }
    } else if (tokens[0] == "kill") {
        command.type = CommandType::KILL;
        if (tokens.size() > 1) {
            command.taskId = std::stoi(tokens[1]);
        }
    } else if (tokens[0] == "sleep") {
        command.type = CommandType::SLEEP;
        if (tokens.size() > 1) {
            command.sleepTimeMs = std::stoi(tokens[1]);
        }
    } else if (tokens[0] == "quit") {
        command.type = CommandType::QUIT;
    }

    return command;
}

std::vector<std::string> InputParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}
