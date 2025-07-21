#include <lib.h>

int main(int argc, char **argv) {
    if (argc == 2) {
        char *path = argv[1];
        // 检查目录状态
        struct Stat st; 
        int r = stat(path, &st);
        if (r < 0) {
            printf("rm: cannot remove '%s': No such file or directory\n", path);
            return -1;
        }
        if (st.st_isdir) {
            printf("rm: cannot remove '%s': Is a directory\n", path);
            return -1;
        }
        // 删除文件
        remove(path);
        return 0;
    } else if (argc == 3) {
        char *path = argv[2];
        if (strcmp(argv[1], "-r") == 0) {
            struct Stat st; 
            int r = stat(path, &st);
            if (r < 0) {
                printf("rm: cannot remove '%s': No such file or directory\n", path);
                return -1;
            }
            // 删除文件或目录
            remove(path);
            return 0;
        } else if (strcmp(argv[1], "-rf") == 0) {
            struct Stat st; 
            int r = stat(path, &st);
            if (r < 0) {
                return 0;
            }
            // 删除文件或目录
            remove(path);
            return 0;
        }
    } else {
        return -1;
    }
    return 0;
}