#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>
#include <unistd.h>

class Task {
public:
    Task(int taskId, const std::string &program, const std::vector<std::string> &args);
    ~Task();

    void start();
    bool poll();
    void terminate();

    int getTaskId() const;
    pid_t getPid() const;
    int getExitStatus() const;

    std::string getLastStdoutLine();
    std::string getLastStderrLine();

    void terminate_and_wait();

private:
    int taskId;
    std::string program;
    std::vector<std::string> args;

    pid_t pid;
    int exitStatus;

    int stdoutPipe[2];
    int stderrPipe[2];
    std::string lastStdoutLine;
    std::string lastStderrLine;

    void updateLastLineFromPipe(int pipeFd, std::string &lastLine);
};

#endif // TASK_H
