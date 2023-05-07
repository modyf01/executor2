#include "task_manager.h"
#include <iostream>

void TaskManager::createTask(const std::string& program, const std::vector<std::string>& args) {
    tasks.emplace_back(program, args);
    std::cout << "Task " << tasks.size() - 1 << " started: pid " << tasks.back().getPid() << "." << std::endl;
}

void TaskManager::printStdout(int task_id) {
    if (task_id >= 0 && task_id < tasks.size()) {
        tasks[task_id].checkStatus();
        tasks[task_id].printStdout();
    }
}

void TaskManager::printStderr(int task_id) {
    if (task_id >= 0 && task_id < tasks.size()) {
        tasks[task_id].checkStatus();
        tasks[task_id].printStderr();
    }
}

void TaskManager::killTask(int task_id) {
    if (task_id >= 0 && task_id < tasks.size()) {
        tasks[task_id].kill();
    }
}
