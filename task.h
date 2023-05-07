#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>

class Task {
public:
    Task(const std::string& program, const std::vector<std::string>& args);

    void printStdout();
    void printStderr();
    void kill();
    void checkStatus();
    bool isRunning() const { return running; }
    pid_t getPid() const { return pid; }

private:
    std::string readPipeContent(int pipe_fd);

    std::string program;
    std::vector<std::string> args;
    bool running;
    pid_t pid;
    int stdout_pipe[2];
    int stderr_pipe[2];
    std::string stdout_last_line;
    std::string stderr_last_line;
};

#endif // TASK_H
