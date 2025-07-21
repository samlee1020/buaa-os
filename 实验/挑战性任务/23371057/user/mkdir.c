#include <lib.h>

int main(int argc, char **argv) {
    if (argc == 2) {
        // 文件目录
        char *path = argv[1];
        // 检查目录状态
        struct Stat st; 
        int r = stat(path, &st);
        if (r < 0) {
            // 目录不存在，尝试创建目录
            int fd = open(path, O_MKDIR);
            if (fd < 0) {
                // 创建失败，父目录不存在
                printf("mkdir: cannot create directory '%s': No such file or directory\n", path);
                return -1;
            } else {
                // 创建成功，关闭文件
                close(fd);
                return 0;
            }
        } else {
            // 目录存在，放弃创建
            printf("mkdir: cannot create directory '%s': File exists\n", path);
            return -1;
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "-p") == 0) {
            // 递归创建目录
            char path[1024];
            u_int envid = syscall_getenvid();
            parse_dir(argv[2], path, envid);
            char buffer[1024];
            buffer[0] = '/';
            buffer[1] = '\0';
            int i;
            for (i = 1; i <= strlen(path); i++) {
                if (path[i] == '/' || path[i] == '\0') {
                    // 遇到路径分隔符，尝试创建目录
                    struct Stat st; 
                    int r = stat(buffer, &st);
                    if (r < 0) {
                        // 目录不存在，尝试创建目录
                        int fd = open(buffer, O_MKDIR);
                        close(fd);
                    } 
                }
                buffer[i] = path[i];
                buffer[i+1] = '\0';
            }
            return 0;
        } else {
            // 无效参数
            return -1;
        }
    } else {
        return -1;
    }
}

