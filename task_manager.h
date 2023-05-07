#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "task.h"
#include <vector>

class TaskManager {
public:
    void createTask(const std::string& program, const std::vector<std::string>& args);
    void printStdout(int task_id);
    void printStderr(int task_id);
    void killTask(int task_id);
    void checkTaskStatus();

private:
    std::vector<Task> tasks;
};

#endif // TASK_MANAGER_H
