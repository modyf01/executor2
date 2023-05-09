#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "task.h"
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void syserr(const char* fmt, ...)
{
    va_list fmt_args;

    fprintf(stderr, "ERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);
    fprintf(stderr, " (%d; %s)\n", errno, strerror(errno));
    exit(1);
}

#define ASSERT_SYS_OK(expr)                                                                \
    do {                                                                                   \
        if ((expr) == -1)                                                                  \
            syserr(                                                                        \
                "System command failed: %s\n\tIn function %s() in %s line %d.\n\tErrno: ", \
                #expr, __func__, __FILE__, __LINE__                                        \
            );                                                                             \
    } while(0)

void set_close_on_exec(int file_descriptor, bool value)
{
    int flags = fcntl(file_descriptor, F_GETFD);
    ASSERT_SYS_OK(flags);
    if (value)
        flags |= FD_CLOEXEC;
    else
        flags &= ~FD_CLOEXEC;
    ASSERT_SYS_OK(fcntl(file_descriptor, F_SETFD, flags));
}