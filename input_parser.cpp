#include "input_parser.h"
#include "utils.h"

Command InputParser::parse(const std::string& line) {
    Command command;
    command.type = CommandType::INVALID;

    auto tokens = split_string(line, ' ');

    if (tokens.empty()) {
        return command;
    }

    if (tokens[0] == "run") {
        command.type = CommandType::RUN;
        if (tokens.size() >= 2) {
            command.program = tokens[1];
            command.args = std::vector<std::string>(tokens.begin() + 2, tokens.end());
        }
    } else if (tokens[0] == "out" || tokens[0] == "err" || tokens[0] == "kill") {
        if (tokens.size() >= 2) {
            command.task_id = std::stoi(tokens[1]);
            if (tokens[0] == "out") {
                command.type = CommandType::OUT;
            } else if (tokens[0] == "err") {
                command.type = CommandType::ERR;
            } else {
                command.type = CommandType::KILL;
            }
        }
    } else if (tokens[0] == "sleep") {
        if (tokens.size() >= 2) {
            command.type = CommandType::SLEEP;
            command.sleep_duration = std::stoi(tokens[1]);
        }
    } else if (tokens[0] == "quit") {
        command.type = CommandType::QUIT;
    }

    return command;
}
