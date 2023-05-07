#include "task.h"
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

Task::Task(const std::string& program, const std::vector<std::string>& args)
        : program(program), args(args), running(true) {
    if (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        std::vector<char*> c_args;
        c_args.reserve(args.size() + 2);
        c_args.push_back(const_cast<char*>(program.c_str()));
        for (const auto& arg : args) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        execvp(program.c_str(), c_args.data());
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        // Set non-blocking read for pipes
        int flags = fcntl(stdout_pipe[0], F_GETFL, 0);
        fcntl(stdout_pipe[0], F_SETFL, flags | O_NONBLOCK);

        flags = fcntl(stderr_pipe[0], F_GETFL, 0);
        fcntl(stderr_pipe[0], F_SETFL, flags | O_NONBLOCK);
    }
}

void Task::printStdout() {
    std::cout << "Task " << pid << " stdout: '" << stdout_last_line << "'." << std::endl;
}

void Task::printStderr() {
    std::cout << "Task " << pid << " stderr: '" << stderr_last_line << "'." << std::endl;
}

void Task::kill() {
    if (running) {
        ::kill(pid, SIGINT);
    }
}

void Task::checkStatus() {
    if (running) {
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == pid) {
            running = false;
            std::cout << "Task " << pid << " ended: ";
            if (WIFEXITED(status)) {
                std::cout << "status " << WEXITSTATUS(status) << "." << std::endl;
            } else if (WIFSIGNALED(status)) {
                std::cout << "signalled." << std::endl;
            }
        }
    }

    stdout_last_line = readPipeContent(stdout_pipe[0]);
    stderr_last_line = readPipeContent(stderr_pipe[0]);
}

std::string Task::readPipeContent(int pipe_fd) {
    static constexpr int buffer_size = 1024;
    char buffer[buffer_size];
    std::string content;
    ssize_t bytes_read;

    while ((bytes_read = read(pipe_fd, buffer, buffer_size - 1)) > 0) {
        buffer[bytes_read] = '\0';
        content += buffer;
    }

    // Get the last line of content
    auto last_newline = content.find_last_of('\n');
    if (last_newline != std::string::npos) {
        content.erase(content.begin() + last_newline, content.end());
        return content.substr(content.find_last_of('\n') + 1);
    }
    return content;
}



