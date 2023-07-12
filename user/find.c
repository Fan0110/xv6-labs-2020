#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, const char *filename)
{
    // 声明文件名缓冲区buf 和 指针p
    char buf[512], *p;
    // 声明文件描述符fd
    int fd;
    // 声明与文件相关的结构体
    struct dirent de;
    struct stat st;

    // 根据参数path路径打开目录
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot fstat %s\n", path);
        close(fd);
        return;
    }

    // 参数错误，find的第一个参数必须是目录
    if (st.type != T_DIR)
    {
        fprintf(2, "usage: find <DIRECTORY> <filename>\n");
        return;
    }

    // 检查路径长度
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }
    // 将绝对路径path复制到buf缓冲区中
    strcpy(buf, path);
    // p是指针，指向一个文件名
    p = buf + strlen(buf);
    // p指向最后一个 / 之后
    *p++ = '/';

    // 读取目录中的每个目录项
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ); // 添加路径名称
        p[DIRSIZ] = 0;               // 添加字符串结束标志
        if (stat(buf, &st) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }

        // 如果是目录类型则递归查找 (注意：不在“.”和“..”目录中递归）
        if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
        {
            find(buf, filename);
        }
        // 如果是文件类型 并且 名称与要查找的文件名相同，就直接 打印缓冲区存放的路径
        else if (strcmp(filename, p) == 0)
            printf("%s\n", buf);
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    // 如果参数个数不为 3 则报错
    if (argc != 3)
    {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }

    // 调用 find 函数查找指定目录下的文件
    find(argv[1], argv[2]);
    exit(0);
}