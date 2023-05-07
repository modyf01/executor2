#include "task.h"
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>

Task::Task(const std::string& program, const std::vector<std::string>& args) {
    int stdout_pipe[2], stderr_pipe[2];
    if (pipe(stdout_pipe) < 0 || pipe(stderr_pipe) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        // Prepare arguments for execvp
        std::vector<char*> exec_args;
        exec_args.push_back(const_cast<char*>(program.c_str()));
        for (const auto& arg : args) {
            exec_args.push_back(const_cast<char*>(arg.c_str()));
        }
        exec_args.push_back(nullptr);

        execvp(program.c_str(), exec_args.data());
        perror("execvp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);
        this->pid = pid;
        this->stdout_fd = stdout_pipe[0];
        this->stderr_fd = stderr_pipe[0];
    }
}

void Task::printStdout() const {
    std::string last_line;
    ssize_t bytes_read;
    char buffer[1024];

    while ((bytes_read = read(stdout_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        last_line = buffer;
    }
    std::cout << "Task " << pid << " stdout: '" << last_line << "'\n";
}

void Task::printStderr() const {
    std::string last_line;
    ssize_t bytes_read;
    char buffer[1024];

    while ((bytes_read = read(stderr_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        last_line = buffer;
    }
    std::cout << "Task " << pid << " stderr: '" << last_line << "'\n";
}

void Task::kill() const {
    if (::kill(pid, SIGINT) < 0) {
        perror("kill");
    }
}

void Task::checkStatus() const {
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);

    if (result < 0) {
        perror("waitpid");
    } else if (result == 0) {
        std::cout << "Task " << pid << " is still running.\n";
    } else {
        if (WIFEXITED(status)) {
            std::cout << "Task " << pid << " ended: status " << WEXITSTATUS(status) << ".\n";
        } else if (WIFSIGNALED(status)) {
            std::cout << "Task " << pid << " ended: signalled.\n";
        }
    }
}
