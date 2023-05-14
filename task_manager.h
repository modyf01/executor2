#include <map>
#include <string>
#include <vector>
#include "task.h"

class TaskManager {
public:
    int run(const std::string &program, const std::vector<std::string> &args);

    std::string getTaskStdout(int taskId);

    std::string getTaskStderr(int taskId);

    void killTask(int taskId);

    void terminateAllTasks();

    void monitorTasks();

    pid_t getTaskPid(int taskId);


private:
    std::map<int, Task> tasks;

    int generateTaskId();

    int taskIdCounter = 0;

};
