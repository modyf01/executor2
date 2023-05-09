#include "task.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <cstring>
#include <iostream>

Task::Task(int taskId, const std::string& command, const std::vector<std::string>& args)
        : taskId(taskId), command(command), args(args), pid(-1), status(0), running(false), signalled(false) {
    pipe2(stdoutPipe.data(), O_NONBLOCK);
    pipe2(stderrPipe.data(), O_NONBLOCK);
}

Task::~Task() {
    if (running) {
        kill(pid, SIGTERM);
    }
    close(stdoutPipe[0]);
    close(stdoutPipe[1]);
    close(stderrPipe[0]);
    close(stderrPipe[1]);
}

void Task::start() {
    pid = fork();

    if (pid == 0) {
        dup2(stdoutPipe[1], STDOUT_FILENO);
        dup2(stderrPipe[1], STDERR_FILENO);

        std::vector<char*> c_args;
        c_args.reserve(args.size() + 2);

        c_args.push_back(const_cast<char*>(command.c_str()));
        for (const auto& arg : args) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        execvp(command.c_str(), c_args.data());
        perror("execvp");
        exit(1);
    } else {
        close(stdoutPipe[1]);
        close(stderrPipe[1]);
        running = true;
        readerThread = std::thread(&Task::readPipes, this);
    }
}

bool Task::poll() {
    if (!running) {
        return false;
    }

    int ret = waitpid(pid, &status, WNOHANG);
    if (ret == -1) {
        perror("waitpid");
    } else if (ret == 0) {
        return true;
    } else {
        running = false;
        readerThread.join();

        if (WIFSIGNALED(status)) {
            signalled = true;
        }
    }
    return false;
}

int Task::getExitStatus() const {
    if (running) {
        return -1;
    }
    return WEXITSTATUS(status);
}

std::string Task::getLastStdoutLine() {
    std::unique_lock<std::mutex> lock(mutex);
    return lastStdoutLine;
}

std::string Task::getLastStderrLine() {
    std::unique_lock<std::mutex> lock(mutex);
    return lastStderrLine;
}

int Task::getTaskId() {
    return taskId;
}

void Task::readPipes() {
    std::array<char, 4096> buffer;

    while (running) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            ssize_t bytesRead = read(stdoutPipe[0], buffer.data(), buffer.size() - 1);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                lastStdoutLine = std::string(buffer.data());
                cv.notify_one();
            }
        }

        {
            std::unique_lock<std::mutex> lock(mutex);
            ssize_t bytesRead = read(stderrPipe[0], buffer.data(), buffer.size() - 1);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                lastStderrLine = std::string(buffer.data());
                cv.notify_one();
            }
        }

        std::unique_lock<std::mutex> lock(mutex);
        cv.wait_for(lock, std::chrono::milliseconds(10));
    }
}

pid_t Task::getPid() {
    return pid;
}

bool Task::isSignalled() const {
    return signalled;
}
