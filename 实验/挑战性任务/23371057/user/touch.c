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
            int fd = open(path, O_CREAT);
            if (fd < 0) {
                // 创建失败，父目录不存在
                printf("touch: cannot touch '%s': No such file or directory\n", path);
                return -1;
            } else {
                // 创建成功，关闭文件
                close(fd);
                return 0;
            }
        } else {
            // 目录存在，放弃创建
            return 0;
        }
    } else {
        // 参数个数错误
        return -1;
    }
}
