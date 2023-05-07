#include "input_parser.h"
#include "task_manager.h"
#include <iostream>
#include <unistd.h>

int main() {
    TaskManager task_manager;

    std::string line;
    while (std::getline(std::cin, line)) {
        auto command = InputParser::parse(line);

        if (command.type == CommandType::RUN) {
            task_manager.createTask(command.program, command.args);
        } else if (command.type == CommandType::OUT) {
            task_manager.printStdout(command.task_id);
        } else if (command.type == CommandType::ERR) {
            task_manager.printStderr(command.task_id);
        } else if (command.type == CommandType::KILL) {
            task_manager.killTask(command.task_id);
        } else if (command.type == CommandType::SLEEP) {
            usleep(command.sleep_duration * 1000); // Convert milliseconds to microseconds
        } else if (command.type == CommandType::QUIT) {
            break;
        }
    }

    return 0;
}
