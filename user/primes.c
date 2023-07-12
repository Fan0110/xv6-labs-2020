#include "kernel/types.h"
#include "user/user.h"

void primes(int p[])
{
    int first;
    // 递归终止的条件：
    if (read(p[0], &first, 4) != 4)
    {
        exit(0);
    }
    // 从上一个进程读到的第一个数一定是质数，输出这个数
    printf("prime %d\n", first);

    int fd[2];
    pipe(fd);

    if (fork() == 0)
    {
        // 子进程
        // 及时关闭不用的管道
        close(fd[1]);
        primes(fd);
        close(fd[0]);
    }
    else
    {
        // 父进程
        // 父进程不读，关闭管道的读端
        close(fd[0]);
        int second;
        while (read(p[0], &second, 4) == 4)
        {
            if (second % first != 0)
            {
                write(fd[1], &second, 4);
            }
        }
        close(fd[1]); // 写完及时关闭
        // 使用wait等待整个管道终止，包括所有子孙进程等等
        wait(0);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int p[2];
    pipe(p);

    if (fork() == 0)
    {
        // 子进程
        // 及时关闭不用的管道
        close(p[1]);
        primes(p);
        close(p[0]);
    }
    else
    {
        // 父进程
        // 父进程不读，关闭管道的读端
        close(p[0]);
        for (int i = 2; i <= 35; ++i)
        {
            write(p[1], &i, 4); // 起始进程向管道写入 2 ~ 35 的数字
        }
        close(p[1]); // 写完及时关闭
        // 使用wait等待整个管道终止，包括所有子孙进程等等
        wait(0);
    }
    exit(0);
}