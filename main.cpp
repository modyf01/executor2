#include <iostream>
#include <string>
#include <unistd.h>

#include "input_parser.h"
#include "task_manager.h"

int main() {
    TaskManager taskManager;

    std::string inputLine;
    while (std::getline(std::cin, inputLine)) {
        if (inputLine.empty()) {
            continue;
        }

        ParsedCommand command = InputParser::parse(inputLine);

        bool isQuitCommand = command.type == CommandType::QUIT;
        switch (command.type) {
            case CommandType::RUN: {
                int taskId = taskManager.run(command.program, command.args);
                std::cout << "Task " << taskId << " started: pid " << taskManager.getTaskPid(taskId) << "."
                          << std::endl;
                break;
            }
            case CommandType::OUT: {
                int taskId = command.taskId;
                std::string output = taskManager.getTaskStdout(taskId);
                std::cout << "Task " << taskId << " stdout: '" << output << "'." << std::endl;
                break;
            }
            case CommandType::ERR: {
                int taskId = command.taskId;
                std::string error = taskManager.getTaskStderr(taskId);
                std::cout << "Task " << taskId << " stderr: '" << error << "'." << std::endl;
                break;
            }
            case CommandType::KILL: {
                int taskId = command.taskId;
                taskManager.killTask(taskId);
                break;
            }
            case CommandType::SLEEP: {
                int sleepTimeMs = command.sleepTimeMs;
                usleep(sleepTimeMs * 1000);
                break;
            }
            case CommandType::QUIT: {
                taskManager.terminateAllTasks();
                break;
            }
            default:
                break;
        }
        taskManager.monitorTasks();
        if (isQuitCommand) {
            break;
        }
    }
    taskManager.terminateAllTasks();
    taskManager.monitorTasks();
    return 0;
}
