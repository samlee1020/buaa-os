#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

// 全局变量
static u_int shell_envid; // shell进程envid
static int shell_id;     // shell的id，用于访问环境变量

struct History history = {.cnt= 0, .cur = -1}; // 命令历史记录

void runcmd(char *s);
/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */
int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) {
		*s++ = 0;
	}
	if (*s == 0) {
		return 0;
	}

	if (strchr(SYMBOLS, *s)) {
		// 检测是否是条件执行
		if (*s == '&' && s[1] == '&') {
			*p1 = s;
			*s++ = 0;
			*s++ = 0;
			*p2 = s;
			return 'A'; // 表示and
		}
		if (*s == '|' && s[1] == '|') {
			*p1 = s;
			*s++ = 0;
			*s++ = 0;
			*p2 = s;
			return 'O'; // 表示or
		}
		// 检测是否是追加重定向
		if (*s == '>' && s[1] == '>') {
			*p1 = s;
			*s++ = 0;
			*s++ = 0;
			*p2 = s;
			// printf("识别到追加\n");
			return 'T'; // 表示tail
		}
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		return t;
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		// 处理反引号
		if (*s == '`') {
			s++;
			while (*s && *s != '`') {
				s++;
			}
		}
		s++;
	}
	*p2 = s;
	// printf("发现单词，长度%d\n", p2-p1);
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

#define MAXARGS 128

static char argv_buffer[MAXARGS][1024];

int run = 1, flag = 0;

int parsecmd(char **argv, int *rightpipe) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int c = gettoken(0, &t);
		int child;

		if (c == 0) {
			return argc;
		}
		if (!run) {
			if (c == 'O' && flag == 1) {
				run = 1;
			} else if (c == 'A' && flag == 0) {
				run = 1;
			}
			continue;
		}

		switch (c) {
		case 0:
			return argc;
		case 'w':
			if (argc >= MAXARGS) {
				debugf("too many arguments\n");
				exit();
			}
			// printf("当前单词%s\n", t);
			// 处理环境变量和反引号
			int i,j;
			char *buffer = argv_buffer[argc];
			int change = 0;
			for (i = 0, j = 0; i < strlen(t); i++) {
				if (t[i] == '$') {
					// 处理环境变量
					i++;
					char name[1024];
					char value[1024];
					int k = 0;
					while (t[i] != '/' && t[i] != '$' && t[i] != '\0') {
						name[k++] = t[i++];
					}
					int r = syscall_get_shell_var(shell_id, name, value);
					
					if (r < 0) {
						change = 0;
						break;
					}
					//printf("变量值%s\n",value);
					strcpy(buffer + j, value);
					j += strlen(value);
					change = 1;
					i--;
				} else {
					buffer[j] = t[i];
					j++;
				}
			}
			buffer[j] = '\0';
			if (change == 0) {
				argv[argc++] = t;
			} else {
				argv[argc++] = buffer;
			}
			//printf("解析前：%s 解析后%s\n",t,buffer);
			break;
		case '<':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: < not followed by word\n");
				exit();
			}
			// Open 't' for reading, dup it onto fd 0, and then close the original fd.
			// If the 'open' function encounters an error,
			// utilize 'debugf' to print relevant messages,
			// and subsequently terminate the process using 'exit'.
			/* Exercise 6.5: Your code here. (1/3) */
			
			// 对.mos_history进行特判
			// char abs_path[1024];
			// parse_dir(t, abs_path, shell_envid);
			// if (strcmp(abs_path, "/.mos_history") == 0) {
			// 	fd = open("/.mos_history", O_WRONLY);
			// 	if (fd < 0) {
			// 		exit();
			// 	}
			// 	// printf("打开成功\n");
			// 	for (int i = 0; i < history.cnt; i++) {
            // 		fprintf(fd, "%s\n", history.cmd[i]);
        	// 	}
			// 	close(fd);
			// } 
			fd = open(t, O_RDONLY);
			if (fd < 0) {
				debugf("failed to open '%s'\n", t);
				exit();
			}
			dup(fd, 0);
			close(fd);

			break;
		case '>':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: > not followed by word\n");
				exit();
			}
			// Open 't' for writing, create it if not exist and trunc it if exist, dup
			// it onto fd 1, and then close the original fd.
			// If the 'open' function encounters an error,
			// utilize 'debugf' to print relevant messages,
			// and subsequently terminate the process using 'exit'.
			/* Exercise 6.5: Your code here. (2/3) */
			

			fd = open(t, O_WRONLY | O_CREAT | O_TRUNC);
			if (fd < 0) {
				debugf("failed to open '%s'\n", t);
				exit();
			}
			dup(fd, 1);
			close(fd);

			break;
		case 'T':
			// 追加打开
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: > not followed by word\n");
				exit();
			}
			fd = open(t, O_WRONLY | O_CREAT | O_APPEND);
			if (fd < 0) {
				debugf("failed to open '%s'\n", t);
				exit();
			}
			dup(fd, 1);
			close(fd);
			break;
		case '|':
			/*
			 * First, allocate a pipe.
			 * Then fork, set '*rightpipe' to the returned child envid or zero.
			 * The child runs the right side of the pipe:
			 * - dup the read end of the pipe onto 0
			 * - close the read end of the pipe
			 * - close the write end of the pipe
			 * - and 'return parsecmd(argv, rightpipe)' again, to parse the rest of the
			 *   command line.
			 * The parent runs the left side of the pipe:
			 * - dup the write end of the pipe onto 1
			 * - close the write end of the pipe
			 * - close the read end of the pipe
			 * - and 'return argc', to execute the left of the pipeline.
			 */
			/* Exercise 6.5: Your code here. (3/3) */
			int p[2];
			r = pipe(p);
			if (r != 0) {
				debugf("pipe: %d\n", r);
				exit();
			}
			r = fork();
			if (r < 0) {
				debugf("fork: %d\n", r);
				exit();
			}
			*rightpipe = r;
			if (r == 0) {
				dup(p[0], 0);
				close(p[0]);
				close(p[1]);
				return parsecmd(argv, rightpipe);
			} else {
				dup(p[1], 1);
				close(p[1]);
				close(p[0]);
				return argc;
			}
			break;
		case ';' :
			// 一行多命令
			child = fork();
			if (child < 0) {
				debugf("fork: %d\n", child);
				exit();
			}
			if (child) {
				// 父进程
				wait(child);

				// 前面如果有重定向的话，这里应该恢复的。
				close(0);
				close(1);
				int con_dev = opencons();
				dup(con_dev, 1);

				return parsecmd(argv, rightpipe);
			} else {
				// 子进程
				return argc;
			}
			break;
		case 'A':
			// 条件执行&&
			child = fork();
			if (child < 0) {
				debugf("fork: %d\n", child);
			}
			if (child) {
				// 父进程，等待子进程结束，如果子进程返回0，则执行后面的命令，否则不执行
				flag = wait(child);

				// 前面如果有重定向的话，这里应该恢复的。
				close(0);
				close(1);
				int con_dev = opencons();
				dup(con_dev, 1);

				// printf("测试：前一条执行结果为: %d\n", cr);
				if (flag == 0) {
					run = 1;
				} else {
					run = 0;
				}
				return parsecmd(argv, rightpipe);
			} else {
				// 子进程，执行当前的命令
				return argc;
			}
			break;
		case 'O':
			// 条件执行||
			child = fork();
			if (child < 0) {
				debugf("fork: %d\n", child);
			}
			if (child) {
				// 父进程，等待子进程结束，如果子进程返回0，则不执行后面的命令，否则执行
				flag = wait(child);

				// 前面如果有重定向的话，这里应该恢复的。
				close(0);
				close(1);
				int con_dev = opencons();
				dup(con_dev, 1);

				// printf("测试：前一条执行结果为: %d\n", cr);
				if (flag == 0) {
					run = 0;
				} else {
					run = 1;
				}
				return parsecmd(argv, rightpipe);
			} else {
				// 子进程，执行当前的命令
				return argc;
			}
			break;
		}
	}

	return argc;
}

// 处理字符串中的反引号
void parse_subcmd(char *t) {
	int i, j, r;
	int change = 0;
	char buffer[1024];
	for (i = 0, j = 0; i < strlen(t); i++) {
		if (t[i] == '`') {
			// 处理反引号
			i++;
			char cmd[1024];
			int k = 0;
			while (t[i] != '`' && t[i] != '\0') {
				cmd[k++] = t[i++];
			}
			i++;
			cmd[k] = '\0';
			// printf("获取cmd为%s,长度为%d\n",cmd,k);

			// 获取cmd命令输出
			char output[1024];
			int p[2];
			r = pipe(p);
			if (r != 0) {
				debugf("pipe: %d\n", r);
				exit();
			}
			r = fork();
			if (r < 0) {
				debugf("fork: %d\n", r);
				exit();
			}
			if (r) {
				// 父进程
				// printf("反引号父进程开始\n");
				close(p[1]);
				int len = 0;
				while(read(p[0], output + len, 1) > 0) {
					len++;
				}
				if (len > 0) {
					// 截断尾部的空白字符
					char *pos = output + len - 1;
					// while(strchr(WHITESPACE, *pos)) {
					// 	pos--;
					// }
					*(pos + 1) = '\0';
				} else {
					output[0] = '\0';
				}
				// printf("父进程读到的字符串%s。原始长度%d\n",output, len);
				close(p[0]);
				wait(r);
			} else {
				// 子进程
				// printf("反引号子进程开始\n");
				// printf("子进程执行的命令%s\n",cmd);
				close(p[0]);
				dup(p[1], 1);
				runcmd(cmd);
				close(p[1]);	
				exit();
			}
			strcpy(buffer + j, output);
			j += strlen(output);
			change = 1;
			i--;
		} else {
			buffer[j] = t[i];
			j++;
		}
	}
	buffer[j] = '\0';
	if (change) {
		strcpy(t, buffer);
	} 
	return;
}

void runcmd(char *s) {
	// 处理字符串中的反引号
	parse_subcmd(s);
	
	gettoken(s, 0);

	char *argv[MAXARGS];
	int rightpipe = 0;
	int argc = parsecmd(argv, &rightpipe);
	if (argc == 0) {
		return;
	}
	argv[argc] = 0;
	
	// 左端命令
	int l_r = -1;
	// 内建命令
	if (strcmp(argv[0], "cd") == 0) {
        l_r = sh_cd(argv, argc, shell_envid);
    } else if (strcmp(argv[0], "pwd") == 0) {
        l_r = sh_pwd(argv, argc, shell_envid);
    } else if (strcmp(argv[0], "exit") == 0) {
		l_r = 99999;
		// exit命令忽略右端，直接退出
		u_int id = syscall_getenvid();
		syscall_return_value(l_r, 0, id);
		exit();
	} else if (strcmp(argv[0], "declare") == 0) {
		l_r = sh_declare(argv, argc, shell_id);
	} else if (strcmp(argv[0], "unset") == 0) {
		l_r = sh_unset(argv, argc, shell_id);
	} else if (strcmp(argv[0], "history") == 0) {
		l_r = sh_history(argv, argc, &history);
	}


	// 外部命令
	else {
		// 如果不以.b结尾，则尝试添加.b后缀
		char newcmd[1024];
		int len = strlen(argv[0]);
		if (argv[0][len - 1] == 'b' && argv[0][len - 2] == '.') {
			// 以.b结尾，不需要添加
		} else {
			// 尝试添加.b后缀
			strcpy(newcmd, argv[0]);
			newcmd[len] = '.';
			newcmd[len + 1] = 'b';
			newcmd[len + 2] = '\0';
			argv[0] = newcmd;
		}
		// 特判cat history
		int is_cat_history = 0;
		if (argc == 2) {
			char abs_cmd[1024];
			char abs_path[1024];
			parse_dir(argv[0], abs_cmd, shell_envid);
			parse_dir(argv[1], abs_path, shell_envid);
			if (strcmp(abs_cmd, "/cat.b") == 0 && strcmp(abs_path, "/.mos_history") == 0) {
				// printf("指令: /cat.b /.mos_history特判成功。原本指令: %s %s\n", argv[0], argv[1]);
				is_cat_history = 1;
				l_r = sh_history(0, 1, &history);
			}
		} 
		if (!is_cat_history) {
			int child = spawn(argv[0], argv);
			close_all();
			if (child >= 0) {
				l_r = wait(child);
			} else {
				debugf("spawn %s: %d\n", argv[0], child);
			}
		}
	}

	close_all();
	// printf("指令%s执行完毕\n",argv[0]);

	// 右端命令
	int r_r = -1;
	if (rightpipe) {
		r_r = wait(rightpipe);
	}

	// 保存返回值
	if (rightpipe) {
		// 如果有右端，则返回右端值
		u_int id = syscall_getenvid();
		syscall_return_value(r_r, 0, id);
		exit();
	} 
	// 没有右端则返回左端值
	u_int id = syscall_getenvid();
	syscall_return_value(l_r, 0, id);

	// printf("指令%s所在runcmd进程即将退出\n",argv[0]);

	exit();
}

void readline(char *buf, u_int n) {
    int i = 0;          // 缓冲区中的字符数
    int cursor = 0;     // 光标位置
    int r;
    char c;
    // 重置历史记录浏览位置
    history.cur = history.cnt;
    char temp_buf[1024];
    temp_buf[0] = '\0';
    
    buf[0] = '\0';
    
    while (i < n - 1) {
        r = read(0, &c, 1);
        if (r != 1) {
            if (r < 0) {
                debugf("read error: %d\n", r);
            }
            exit();
        }
        
        // 处理特殊键（可能是多字节序列）
        if (c == '\033') {  
            char seq[2];
            if (read(0, &seq[0], 1) != 1) continue;
            if (read(0, &seq[1], 1) != 1) continue;
            
            if (seq[0] == '[') {
                switch (seq[1]) {
                case 'A':  // 上箭头
		    printf("\033[B");  // 抵消向上移动
                    if (history.cur > 0) {
                        // 保存当前输入
                        if (history.cur== history.cnt) {
                            strcpy(temp_buf, buf);
                        }
                        history.cur--;
                        // 清除当前行
                        while (cursor < i) {
                            printf(" ");
                            cursor++;
                        }
                        while (i > 0) {
                            printf("\b \b");
                            i--;
                        }
                        cursor = 0;                       
                        // 显示历史命令
                        strcpy(buf, history.cmd[history.cur]);
                        i = strlen(buf);
                        cursor = i;
                        printf("%s", buf);
                    }
                    continue;                  
                case 'B':  // 下箭头
                    if (history.cur < history.cnt) {
                        history.cur++;                    
                        // 清除当前行
                        while (cursor < i) {
                            printf(" ");
                            cursor++;
                        }
                        while (i > 0) {
                            printf("\b \b");
                            i--;
                        }
                        cursor = 0;                       
                        // 显示命令
                        if (history.cur == history.cnt) {
                            strcpy(buf, temp_buf);
                        } else {
                            strcpy(buf, history.cmd[history.cur]);
                        }
                        i = strlen(buf);
                        cursor = i;
                        printf("%s", buf);
                    }
                    continue;              
                case 'C':  // 右箭头
                    if (cursor < i) {
                        cursor++;
                        // 不需要打印，终端会处理光标移动
                    } else {
						printf("\033[D");  // 在最右端，向左抵消
					}
                    continue;          
                case 'D':  // 左箭头
                    if (cursor > 0) {
                        cursor--;
                        // 不需要打印，终端会处理光标移动
                    } else {
						printf("\033[C");  // 在最左端，向右抵消
					}
                    continue;
                }
            }
            continue;
        }    
        // 处理控制字符
        if (c == CTRL_A) {  // 跳到行首
            while (cursor > 0) {
                printf("\b");
                cursor--;
            }
            continue;
        }  
        if (c == CTRL_E) {  // 跳到行尾
            while (cursor < i) {
                printf("\033[C");  // 使用ANSI转义序列向右移动光标
                cursor++;
            }
            continue;
        }   
        if (c == CTRL_K) {  // 删除到行尾
            if (cursor < i) {
                i = cursor;
                buf[i] = '\0';
                // refresh_line(buf, cursor, i);
				printf("\r\x1b[K"); // 回到行首并清除整行
				printf("$ "); // 打印提示符
				printf("%s", buf); // 打印当前输入内容
				if (cursor < i) {
					// 计算需要左移的字符数
					int move_left = i - cursor;
					while (move_left-- > 0) {
						printf("\x1b[D"); // ANSI 左移光标
					}
				}
            }
            continue;
        }  
        if (c == CTRL_U) {  // 删除到行首
            int x = cursor;
			// 执行多次退格键
            for (; x > 0; x--) {
				if (cursor > 0) {
					// 删除光标前的字符，重新显示整行
					for (int j = cursor - 1; j < i - 1; j++) {
						buf[j] = buf[j + 1];
					}
					cursor--;
					int temp = cursor; // 记录cursor位置
					i--;
					buf[i] = '\0';            
					// 回到行首并重新打印整行
					while (cursor > 0) {
						printf("\b");
						cursor--;
					}
					printf("\r$ %s", buf);
					// 清除多余字符
					printf(" \b");
					// 移动光标到正确位置
					cursor = strlen(buf);
					while (cursor != temp) {
						printf("\b");
						cursor--;
					}
				}
			}
            continue;
        }     
        if (c == CTRL_W) {  // 删除前一个单词
            if (cursor == 0) continue;         
            int end = cursor;
            // 向左跳过空白
            while (cursor > 0 && (buf[cursor-1] == ' ' || buf[cursor-1] == '\t')) {
                cursor--;
                printf("\b");
            }
            // 向左删除非空白字符
            while (cursor > 0 && buf[cursor-1] != ' ' && buf[cursor-1] != '\t') {
                cursor--;
                printf("\b");
            }        
            // 移动后面的字符
            int del_len = end - cursor;
            int j;
            for (j = cursor; j < i - del_len; j++) {
                buf[j] = buf[j + del_len];
                printf("%c", buf[j]);
            }
            for (; j < i; j++) {
                printf(" ");
            }
            for (j = cursor; j < i; j++) {
                printf("\b");
            }
            i -= del_len;
            buf[i] = '\0';
            continue;
        }       
        // 处理退格键
        if (c == '\b' || c == 0x7f) {
            if (cursor > 0) {
                // 删除光标前的字符，重新显示整行
                for (int j = cursor - 1; j < i - 1; j++) {
                    buf[j] = buf[j + 1];
                }
                cursor--;
				int temp = cursor; // 记录cursor位置
                i--;
                buf[i] = '\0';            
                // 回到行首并重新打印整行
                while (cursor > 0) {
                    printf("\b");
                    cursor--;
                }
                printf("\r$ %s", buf);
                // 清除多余字符
                printf(" \b");
                // 移动光标到正确位置
                cursor = strlen(buf);
                while (cursor != temp) {
					printf("\b");
					cursor--;
				}
            }
            continue;
        } 
        // 处理回车
        if (c == '\r' || c == '\n') {
            buf[i] = '\0';
            // 终端会自动换行，不需要我们打印
            return;
        }        
        // 普通字符插入 - 不打印字符，让终端回显处理
        if (cursor < i) {
            // 在中间插入字符
            for (int j = i; j > cursor; j--) {
                buf[j] = buf[j - 1];
            }
            buf[cursor] = c;
            cursor++;
            i++;
            buf[i] = '\0';           
            // 需要重新显示光标后的字符
            for (int j = cursor; j < i; j++) {
                printf("%c", buf[j]);
            }
            // 把光标移回正确位置
            for (int j = cursor; j < i; j++) {
                printf("\b");
            }
        } else {
            // 在末尾添加字符
            buf[i] = c;
            cursor++;
            i++;
            buf[i] = '\0';
            // 不需要打印，终端会回显
        }
    }   
    debugf("line too long\n");
    while ((r = read(0, &c, 1)) == 1 && c != '\r' && c != '\n') {
        ;
    }
    buf[n - 1] = '\0';
}

char buf[1024];

void usage(void) {
	printf("usage: sh [-ix] [script-file]\n");
	exit();
}

int main(int argc, char **argv) {
	// 设置全局shell进程id
	shell_envid = syscall_getenvid();
	// 分配shell_id
	shell_id = syscall_shell_id_alloc();
	// 初始化shell变量
	syscall_init_shell_var(shell_id);
	// 加载历史记录
	load_history(&history);
	

	int r;
	int interactive = iscons(0);
	int echocmds = 0;
	printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	printf("::                                                         ::\n");
	printf("::                     MOS Shell 2024                      ::\n");
	printf("::                                                         ::\n");
	printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");

	ARGBEGIN {
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
	ARGEND

	if (argc > 1) {
		usage();
	}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[0], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[0], r);
		}
		user_assert(r == 0);
	}

	for (;;) {
		if (interactive) {
			printf("\n$ ");
		}

		if (buf[0] == '#') {
			continue;
		}

		readline(buf, sizeof buf);

		// 记录指令到history
		save_history(buf, &history);

		//截断尾部的注释
		char *p = strchr(buf, '#');
		if (p) {
			*p = '\0';
		}

		if (echocmds) {
			printf("# %s\n", buf);
		}
		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) {
			runcmd(buf);
			exit();
		} else {
			int return_value = wait(r);
			// printf("wait返回值：%d\n", return_value);
			if (return_value == 99999) { // 检测到exit指令，退出shell
				break;
			}
		}
	}
	syscall_exit_shell_var(shell_id);
	return 0;
}
