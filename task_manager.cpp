#include "task_manager.h"

void TaskManager::createTask(const std::string& program, const std::vector<std::string>& args) {
    tasks.emplace_back(program, args);
}

void TaskManager::printStdout(int task_id) {
    if (task_id >= 0 && task_id < tasks.size()) {
        tasks[task_id].printStdout();
    }
}

void TaskManager::printStderr(int task_id) {
    if (task_id >= 0 && task_id < tasks.size()) {
        tasks[task_id].printStderr();
    }
}

void TaskManager::killTask(int task_id) {
    if (task_id >= 0 && task_id < tasks.size()) {
        tasks[task_id].kill();
    }
}

void TaskManager::checkTaskStatus() {
    for (Task& task : tasks) {
        task.checkStatus();
    }
}
