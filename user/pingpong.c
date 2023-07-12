#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int p1[2], p2[2];
    // 创建管道。管道是单向数据通道，不支持双向读写，创建两个。
    pipe(p1);       // 写端父进程，读端子进程
    pipe(p2);       // 写端子进程，读端父进程
    char buf = 'f'; // 来回传输的字符
    // 创建一个子进程
    int pid = fork();
    if (pid < 0)
    {
        fprintf(2, "fork() error!\n");
        exit(1);
    }
    else if (pid == 0)
    {
        // 子进程
        read(p1[0], &buf, 1);
        printf("%d: received pong\n", getpid());
        write(p2[1], &buf, 1);
    }
    else
    {
        // 父进程
        write(p1[1], &buf, 1);
        read(p2[0], &buf, 1);
        printf("%d: received ping\n", getpid());
    }
    exit(0);
}