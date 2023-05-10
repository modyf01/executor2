#include <iostream>
#include <sys/wait.h>
#include <fcntl.h>
#include "task.h"

Task::Task(int taskId, const std::string &program, const std::vector<std::string> &args)
        : taskId(taskId), program(program), args(args), pid(-1), exitStatus(-1) {
    stdoutPipe[0] = -1;
    stdoutPipe[1] = -1;
    stderrPipe[0] = -1;
    stderrPipe[1] = -1;
}


Task::~Task() {
    terminate_and_wait();
    close(stdoutPipe[0]);
    close(stdoutPipe[1]);
    close(stderrPipe[0]);
    close(stderrPipe[1]);
}

void Task::start() {
    pipe(stdoutPipe);
    pipe(stderrPipe);

    pid = fork();
    if (pid == 0) {
        close(stdoutPipe[0]);
        close(stderrPipe[0]);
        dup2(stdoutPipe[1], STDOUT_FILENO);
        dup2(stderrPipe[1], STDERR_FILENO);

        std::vector<char *> execArgs;
        execArgs.push_back(const_cast<char *>(program.c_str()));
        for (const std::string &arg: args) {
            execArgs.push_back(const_cast<char *>(arg.c_str()));
        }
        execArgs.push_back(nullptr);

        execvp(program.c_str(), &execArgs[0]);

        // If execvp returns, there was an error
        perror("execvp");
        exit(1);
    } else {
        close(stdoutPipe[1]);
        close(stderrPipe[1]);
        fcntl(stdoutPipe[0], F_SETFL, O_NONBLOCK);
        fcntl(stderrPipe[0], F_SETFL, O_NONBLOCK);
    }
}

bool Task::poll() {
    if (pid == -1) {
        return false;
    }

    int status = -1;;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        return false;
    }

    if (WIFEXITED(status)) {
        exitStatus = WEXITSTATUS(status);
        std::cout << "Task " << taskId << " ended: status " << exitStatus << ".\n";
    } else if (WIFSIGNALED(status)) {
        exitStatus = -WTERMSIG(status);
        std::cout << "Task " << taskId << " ended: signalled.\n";
    } else {
        exitStatus = -1;
        std::cout << "Task " << taskId << " ended: signalled.\n";
    }

    pid = -1;
    return true;
}



void Task::terminate() {
    if (pid != -1) {
        kill(pid, SIGKILL);
    }
}

void Task::terminate_and_wait() {
    if (pid != -1) {
        kill(pid, SIGTERM);
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            exitStatus = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            exitStatus = -WTERMSIG(status);
        } else {
            exitStatus = -1;
        }
    }
}


int Task::getTaskId() const {
    return taskId;
}

pid_t Task::getPid() const {
    return pid;
}

int Task::getExitStatus() const {
    return exitStatus;
}

std::string Task::getLastStdoutLine() {
    updateLastLineFromPipe(stdoutPipe[0], lastStdoutLine);
    return lastStdoutLine;
}

std::string Task::getLastStderrLine() {
    updateLastLineFromPipe(stderrPipe[0], lastStderrLine);
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