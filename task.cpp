#include <iostream>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include "task.h"

FileDescriptor::FileDescriptor() : fd(-1) {}

FileDescriptor::FileDescriptor(int fd) : fd(fd) {}

FileDescriptor::~FileDescriptor() {
    if (fd != -1) {
        close(fd);
    }
}

FileDescriptor &FileDescriptor::operator=(FileDescriptor &&other) noexcept {
    if (this != &other) {
        reset(other.fd);
        other.fd = -1;
    }
    return *this;
}

int FileDescriptor::get() const {
    return fd;
}

void FileDescriptor::reset(int newFd) {
    if (fd != -1) {
        close(fd);
    }
    fd = newFd;
}

int FileDescriptor::release() {
    int oldFd = fd;
    fd = -1;
    return oldFd;
}

FileDescriptor &FileDescriptor::operator=(int my_fd) {
    reset(my_fd);
    return *this;
}

Task::Task(int taskId, const std::string &program, const std::vector<std::string> &args)
        : taskId(taskId), program(program), pid(-1), exitStatus(-1) {
    stdoutPipe[0] = -1;
    stdoutPipe[1] = -1;
    stderrPipe[0] = -1;
    stderrPipe[1] = -1;

    execArgs.push_back(const_cast<char *>(program.c_str()));
    for (const auto &arg: args) {
        execArgs.push_back(const_cast<char *>(arg.c_str()));
    }
    execArgs.push_back(nullptr);
}

Task::~Task() {
    terminate_and_wait();
}

void Task::start() {
    int stdoutPipeRaw[2];
    int stderrPipeRaw[2];

    if (pipe(stdoutPipeRaw) < 0 || pipe(stderrPipeRaw) < 0) {
        perror("pipe");
        return;
    }

    stdoutPipe[0] = FileDescriptor(stdoutPipeRaw[0]);
    stdoutPipe[1] = FileDescriptor(stdoutPipeRaw[1]);
    stderrPipe[0] = FileDescriptor(stderrPipeRaw[0]);
    stderrPipe[1] = FileDescriptor(stderrPipeRaw[1]);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) { // child process
        close(stdoutPipe[0].release());
        close(stderrPipe[0].release());
        if (dup2(stdoutPipe[1].get(), STDOUT_FILENO) < 0 || dup2(stderrPipe[1].get(), STDERR_FILENO) < 0) {
            perror("dup2");
            exit(1);
        }

        // Close unnecessary file descriptors after dup2
        close(stdoutPipe[1].release());
        close(stderrPipe[1].release());

        // Close all file descriptors greater than or equal to 3
        closefrom(3);

        if (execvp(program.c_str(), execArgs.data()) < 0) {
            perror("execvp");
            exit(1);
        }
    } else { // parent process
        close(stdoutPipe[1].release());
        close(stderrPipe[1].release());
        if (fcntl(stdoutPipe[0].get(), F_SETFL, O_NONBLOCK) < 0 ||
            fcntl(stderrPipe[0].get(), F_SETFL, O_NONBLOCK) < 0) {
            perror("fcntl");
            return;
        }
    }
}



bool Task::poll() {
    if (pid == -1) {
        return false;
    }

    updateLastLineFromPipe(stdoutPipe[0].get(), lastStdoutLine);
    updateLastLineFromPipe(stderrPipe[0].get(), lastStderrLine);

    int status = -1;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        return false;
    }

    if (WIFEXITED(status)) {
        exitStatus = WEXITSTATUS(status);
        std::cout << "Task " << taskId << " ended: status " << exitStatus << ".\n";
    } else if (WIFSIGNALED(status)) {
        std::cout << "Task " << taskId << " ended: signalled.\n";
    } else {
        exitStatus = -1;
        std::cout << "Task " << taskId << " ended: signalled.\n";
    }

    pid = -1;
    return true;
}

void Task::terminate() const {
    if (pid != -1) {
        kill(pid, SIGINT);
    }
}

void Task::terminate_and_wait() {
    if (pid != -1) {
        kill(pid, SIGINT);
        updateLastLineFromPipe(stdoutPipe[0].get(), lastStdoutLine);
        updateLastLineFromPipe(stderrPipe[0].get(), lastStderrLine);
        int status = -1;
        if (waitpid(pid, &status, 0) == -1) {
            kill(pid, SIGKILL);
        }

        if (WIFEXITED(status)) {
            exitStatus = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            exitStatus = -WTERMSIG(status);
        } else {
            exitStatus = -1;
        }
    }
}

pid_t Task::getPid() const {
    return pid;
}

std::string Task::getLastStdoutLine() {
    updateLastLineFromPipe(stdoutPipe[0].get(), lastStdoutLine);
    return lastStdoutLine;
}

std::string Task::getLastStderrLine() {
    updateLastLineFromPipe(stderrPipe[0].get(), lastStderrLine);
    return lastStderrLine;
}

void Task::updateLastLineFromPipe(int pipeFd, std::string &lastLine) {
    std::string output;
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(pipeFd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        output.append(buffer);

        size_t pos = 0;
        size_t lastNewlinePos;
        while ((lastNewlinePos = output.find('\n', pos)) != std::string::npos) {
            lastLine.assign(output, pos, lastNewlinePos - pos);
            pos = lastNewlinePos + 1;
        }
        output.erase(0, pos);
    }

    if (!output.empty()) {
        lastLine = std::move(output);
    }
}

