#include <iostream>
#include "task_manager.h"

int TaskManager::run(const std::string &program, const std::vector<std::string> &args) {
    int taskId = generateTaskId();
    tasks.emplace(std::piecewise_construct,
                  std::forward_as_tuple(taskId),
                  std::forward_as_tuple(taskId, program, args));
    tasks.at(taskId).start();
    return taskId;
}


std::string TaskManager::getTaskStdout(int taskId) {
    return tasks.at(taskId).getLastStdoutLine();
}

std::string TaskManager::getTaskStderr(int taskId) {
    return tasks.at(taskId).getLastStderrLine();
}

void TaskManager::killTask(int taskId) {
    tasks.at(taskId).terminate();
}

void TaskManager::terminateAllTasks() {
    for (auto &taskPair : tasks) {
        taskPair.second.terminate();
    }
}

void TaskManager::monitorTasks() {
    for (auto it = tasks.begin(); it != tasks.end(); ) {
        Task &task = it->second;
        if (!task.poll())
            ++it;
    }
}

pid_t TaskManager::getTaskPid(int taskId) {
    return tasks.at(taskId).getPid();
}

int TaskManager::generateTaskId() {
    return taskIdCounter++;
}
