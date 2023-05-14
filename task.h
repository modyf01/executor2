#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>

class FileDescriptor {
public:
    FileDescriptor();

    explicit FileDescriptor(int fd);

    ~FileDescriptor();

    FileDescriptor(const FileDescriptor &) = delete;

    FileDescriptor &operator=(const FileDescriptor &) = delete;

    FileDescriptor &operator=(int fd);

    FileDescriptor &operator=(FileDescriptor &&other) noexcept;

    [[nodiscard]] int get() const;

    void reset(int newFd = -1);

    int release();

private:
    int fd;
};

class Task {
public:
    Task(int taskId, const std::string &program, const std::vector<std::string> &args);

    ~Task();

    void start();

    bool poll();

    void terminate() const;

    void terminate_and_wait();

    [[nodiscard]] pid_t getPid() const;

    std::string getLastStdoutLine();

    std::string getLastStderrLine();

private:
    static void updateLastLineFromPipe(int pipeFd, std::string &lastLine);

    int taskId;
    std::string program;
    std::vector<char *> execArgs;
    pid_t pid;
    int exitStatus;

    FileDescriptor stdoutPipe[2];
    FileDescriptor stderrPipe[2];

    std::string lastStdoutLine;
    std::string lastStderrLine;
};

#endif // TASK_H
