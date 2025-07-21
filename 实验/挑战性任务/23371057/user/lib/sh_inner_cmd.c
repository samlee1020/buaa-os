#include <lib.h>

// cd命令
int sh_cd(char **argv, int argc, u_int envid) {
    if (argc == 1) { 
        // 切换至根目录
        syscall_workdir("/", 0, envid);
        return 0;
    } else if (argc == 2) {
        // 获取绝对路径
        char path[1024];
        parse_dir(argv[1], path, envid);

        // 检查路径状态
        struct Stat st; // Stat在user/lib/fd.h
        int r = stat(path, &st); // stat()在user/lib/fd.c
        if (r < 0) {
            // 路径不存在
            printf("cd: The directory '%s' does not exist\n", argv[1]);
            return 1;
        } 
        if (!st.st_isdir) {
            // 路径存在但不是目录
            printf("cd: '%s' is not a directory\n", argv[1]);
            return 1;
        }
        // 路径存在且是目录，切换到该目录
        syscall_workdir(path, 0, envid); 
        return 0;
    } else {
        // 参数过多
        printf("Too many args for cd command\n");
        return 1;
    }
}

// pwd命令
int sh_pwd(char **argv, int argc, u_int envid) {
    if (argc == 1) {
        // 输出当前工作目录
        char path[1024];
        syscall_workdir(path, 1, envid); 
        printf("%s\n", path);
        return 0;
    } else {
        printf("pwd: expected 0 arguments; got %d\n", argc - 1);
        return 2;
    }
}

// declare命令
int sh_declare(char **argv, int argc, int shell_id) {
    int r;
    if (argc == 1) {
        // 没有参数，显示所有变量
        char output[2048];
        r = syscall_get_shell_var(shell_id, NULL, output);
        printf("%s\n", output);   
    } else if (argc == 2) {
        char *name_e_value = argv[1];
        char name[32], value[32];
        int i=0;

        // printf("调试变量%s\n",name_e_value);

        char *p = strchr(name_e_value, '=');
        if (p) {
            char *pos = name_e_value;
            while(pos != p) {
                name[i++] = *pos;
                pos++;
            }
            name[i] = '\0';
            strcpy(value, p+1);
        } else {
            // 没有等号，错误输入
            r = -1;
        }
        // printf("调试变量%s: %s\n",name,value);
        // 有等号，声明变量
        r = syscall_declare_shell_var(shell_id, name, value, 0, 0); // 局部非共享变量
        
    } else if (argc == 3) {
        char *mode = argv[1];
        char *name_e_value = argv[2];
        int share = 0, rondly = 0;
        if (strchr(mode, 'x')) {
            share = 1;
        }
        if (strchr(mode, 'r')) {
            rondly = 1;
        }
        char name[32], value[32];
        int i = 0, j = 0, k = 0; 
        int pos;
        for (pos = 0; pos < strlen(name_e_value); pos++) {
            if (k == 0) {
                if (name_e_value[pos] == '=') {
                    k = 1;
                    name[i] = '\0';
                    continue;
                }
                name[i++] = name_e_value[pos];
            } else if (k == 1) {
                if (name_e_value[pos] == '\0') {
                    value[j] = '\0';
                    break;
                }
                value[j++] = name_e_value[pos];
            }
        }
        if (k == 0) {
            // 没有等号，错误输入
            r = -1;
        } else {
            // 有等号，声明变量
            r = syscall_declare_shell_var(shell_id, name, value, share, rondly);
        }
    } else {
        r = -1;
    }
    return r;
}

// unset命令
int sh_unset(char **argv, int argc, int shell_id) {
    int r;
    if (argc == 2) {
        char *name = argv[1];
        r = syscall_unset_shell_var(shell_id, name);
    } else {
        r = -1;
    }
    return r;
}

// history命令
void load_history(struct History *history) {
    // 从文件中加载历史命令
    int fd = open("/.mos_history", O_RDWR | O_CREAT);
    if (fd < 0) {
        return;
    }
    char buf[20 * 1024];
    int n = read(fd, buf, sizeof(buf));
    close(fd);
    if (n <= 0) {
        return;
    }
    // 解析历史命令
    history->cnt = 0;
    char *line_start = buf;
    for (int i = 0; i < n && history->cnt < 20; i++) {
        if (buf[i] == '\n' || i == n - 1) {
            int len = &buf[i] - line_start;
            if (i == n - 1 && buf[i] != '\n') {
                len++;
            }
            if (len > 0 && len < 1024) {
                memcpy(history->cmd[history->cnt], line_start, len);
                history->cmd[history->cnt][len] = '\0';
                history->cnt++;
            }
            line_start = &buf[i + 1];
        }
    }
}

void save_history(char *cmd, struct History *history) {
    // 保存命令到文件
    if (!cmd || !*cmd) {
        return;
    }
    // 如果记录已满，整体前移动一位
    if (history->cnt >= 20) {
        for (int i = 0; i < 20 - 1; i++) {
            strcpy(history->cmd[i], history->cmd[i + 1]);
        }
        history->cnt = 20 - 1;
    }
    // 添加新命令到末尾
    strcpy(history->cmd[history->cnt], cmd);
    history->cnt++;

    // 保存到文件
    int fd = open("/.mos_history", O_WRONLY);
    if (fd < 0) {
        return;
    }
    for (int i = 0; i < history->cnt; i++) {
        fprintf(fd, "%s\n", history->cmd[i]);
    }
    close(fd);
}

int sh_history(char **argv, int argc, struct History *history) {
    if (argc == 1) {
        for (int i = 0; i < history->cnt; i++) {
            printf("%s\n", history->cmd[i]);
        }
        return 0;
    }
    return -1;
}