#ifndef LIB_H
#define LIB_H
#include <args.h>
#include <env.h>
#include <fd.h>
#include <mmu.h>
#include <pmap.h>
#include <syscall.h>
#include <trap.h>

#define vpt ((const volatile Pte *)UVPT)
#define vpd ((const volatile Pde *)(UVPT + (PDX(UVPT) << PGSHIFT)))
#define envs ((const volatile struct Env *)UENVS)
#define pages ((const volatile struct Page *)UPAGES)

// libos
void exit(void) __attribute__((noreturn));

extern const volatile struct Env *env;

#define USED(x) (void)(x)

// debugf
void debugf(const char *fmt, ...);

void _user_panic(const char *, int, const char *, ...) __attribute__((noreturn));
void _user_halt(const char *, int, const char *, ...) __attribute__((noreturn));

#define user_panic(...) _user_panic(__FILE__, __LINE__, __VA_ARGS__)
#define user_halt(...) _user_halt(__FILE__, __LINE__, __VA_ARGS__)

#undef panic_on
#define panic_on(expr)                                                                             \
	do {                                                                                       \
		int r = (expr);                                                                    \
		if (r != 0) {                                                                      \
			user_panic("'" #expr "' returned %d", r);                                  \
		}                                                                                  \
	} while (0)

/// fork, spawn
int spawn(char *prog, char **argv);
int spawnl(char *prot, char *args, ...);
int fork(void);

/// syscalls
extern int msyscall(int, ...);

void syscall_putchar(int ch);
int syscall_print_cons(const void *str, u_int num);
u_int syscall_getenvid(void);
void syscall_yield(void);
int syscall_env_destroy(u_int envid);
int syscall_set_tlb_mod_entry(u_int envid, void (*func)(struct Trapframe *));
int syscall_mem_alloc(u_int envid, void *va, u_int perm);
int syscall_mem_map(u_int srcid, void *srcva, u_int dstid, void *dstva, u_int perm);
int syscall_mem_unmap(u_int envid, void *va);

__attribute__((always_inline)) inline static int syscall_exofork(void) {
	return msyscall(SYS_exofork, 0, 0, 0, 0, 0);
}

int syscall_set_env_status(u_int envid, u_int status);
int syscall_set_trapframe(u_int envid, struct Trapframe *tf);
void syscall_panic(const char *msg) __attribute__((noreturn));
int syscall_ipc_try_send(u_int envid, u_int value, const void *srcva, u_int perm);
int syscall_ipc_recv(void *dstva);
int syscall_cgetc(void);
int syscall_write_dev(void *va, u_int dev, u_int len);
int syscall_read_dev(void *va, u_int dev, u_int len);

// shell新增系统调用
int syscall_workdir(char *path, int op, u_int envid); // set/get工作目录
int syscall_return_value(int value, int op, u_int envid); // set/get进程返回值
int syscall_shell_id_alloc(); // 分配shell_id
int syscall_declare_shell_var(int shell_id, char *name, char *value, int share, int rdonly); // 声明shell变量
int syscall_get_shell_var(int shell_id, char *name, char *value); // 获取shell变量
int syscall_unset_shell_var(int shell_id, char *name); // 删除shell变量
int syscall_init_shell_var(int shell_id); // 初始化shell变量
int syscall_exit_shell_var(int shell_id); // 退出shell变量


// ipc.c
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm);
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm);

// wait.c
int wait(u_int envid);

// console.c
int opencons(void);
int iscons(int fdnum);

// pipe.c
int pipe(int pfd[2]);
int pipe_is_closed(int fdnum);

// pageref.c
int pageref(void *);

// fprintf.c
int fprintf(int fd, const char *fmt, ...);
int printf(const char *fmt, ...);

// fsipc.c
int fsipc_open(const char *, u_int, struct Fd *);
int fsipc_map(u_int, u_int, void *);
int fsipc_set_size(u_int, u_int);
int fsipc_close(u_int);
int fsipc_dirty(u_int, u_int);
int fsipc_remove(const char *);
int fsipc_sync(void);
int fsipc_incref(u_int);

// fd.c
int close(int fd);
int read(int fd, void *buf, u_int nbytes);
int write(int fd, const void *buf, u_int nbytes);
int seek(int fd, u_int offset);
void close_all(void);
int readn(int fd, void *buf, u_int nbytes);
int dup(int oldfd, int newfd);
int fstat(int fdnum, struct Stat *stat);
int stat(const char *path, struct Stat *);

// file.c
int open(const char *path, int mode);
int read_map(int fd, u_int offset, void **blk);
int remove(const char *path);
int ftruncate(int fd, u_int size);
int sync(void);

void parse_dir(const char *path, char *result, u_int envid); // 解析相对路径为绝对路径

#define user_assert(x)                                                                             \
	do {                                                                                       \
		if (!(x))                                                                          \
			user_panic("assertion failed: %s", #x);                                    \
	} while (0)

// sh_inner_cmd.c 新增内建指令
int sh_cd(char **argv, int argc, u_int envid);
int sh_pwd(char **argv, int argc, u_int envid);
int sh_declare(char **argv, int argc, int shell_id);
int sh_unset(char **argv, int argc, int shell_id);

struct History {
    char cmd[20][1024];
    int cnt; // 命令数量
    int cur; // 当前命令索引
};

void load_history(struct History *history);
void save_history(char *cmd, struct History *history);
int sh_history(char **argv, int argc, struct History *history);



// File open modes
#define O_RDONLY 0x0000	 /* open for reading only */
#define O_WRONLY 0x0001	 /* open for writing only */
#define O_RDWR 0x0002	 /* open for reading and writing */
#define O_ACCMODE 0x0003 /* mask for above modes */
#define O_CREAT 0x0100	 /* create if nonexistent */
#define O_TRUNC 0x0200	 /* truncate to zero length */

// Unimplemented open modes
#define O_EXCL 0x0400  /* error if already exists */
#define O_MKDIR 0x0800 /* create directory, not regular file */
#define O_APPEND 0x0008

// 键位定义
// 键盘扫描码定义
#define KEY_UP    0x48
#define KEY_DOWN  0x50
#define KEY_LEFT  0x4B
#define KEY_RIGHT 0x4D
#define CTRL_A    0x01
#define CTRL_E    0x05
#define CTRL_K    0x0B
#define CTRL_U    0x15
#define CTRL_W    0x17

#endif
