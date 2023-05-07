#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>

class Task {
public:
    Task(const std::string& program, const std::vector<std::string>& args);
    void printStdout() const;
    void printStderr() const;
    void kill() const;
    void checkStatus() const;

private:
    pid_t pid;
    int stdout_fd;
    int stderr_fd;
};

#endif // TASK_H
