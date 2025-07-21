#include <env.h>
#include <io.h>
#include <mmu.h>
#include <pmap.h>
#include <printk.h>
#include <sched.h>
#include <syscall.h>

extern struct Env *curenv;

// shell 新增系统调用
/*
op == 0时，把工作目录设置为path
op == 1时，把path设置为工作目录
*/
int sys_workdir(char *path, int op, u_int envid) {
	struct Env *e;
	envid2env(envid, &e, 0);
	if (op == 0) {
		strcpy(e->env_workdir, path);
	} else if (op == 1) {
		strcpy(path, e->env_workdir);
	}
	return 0;
}

/*
op == 0时，把envid的返回值设置为value
op == 1时，返回进程返回值
*/
extern struct Env envs[NENV];

int sys_return_value(int value, int op, u_int envid) {
	// 不能用envid2env，因为wait调用该系统调用时，envid对应的进程已经停止。而envid2env只能获取活跃的块
	struct Env *e = &envs[ENVX(envid)];
	if (op == 0) {
		e->env_return_value = value;
		// printk("%d设置%d\n",envid,e->env_return_value);
	} else if (op == 1) {
		// printk("%d返回%d\n",envid,e->env_return_value);
	}
	return e->env_return_value;
}

// 环境变量部分
struct Shell_var {
	char name[32];
	char value[32];
	int shell_id;       // 创建者的shell_id
	int is_share;		// 1表示为环境变量
	int is_rdonly;		// 1表示为只读
	int is_valid;		// 1表示该变量是否有效
};

struct Shell_var vars[1024]; 
int var_num = 0;
int sid = 0;

// 环境变量封装函数
int get_parent_var_index(char *name, int shell_id) {
	// 获取父shell的全局变量索引
	int i;
	for (i = 0; i < var_num; i++) {
		if (strcmp(vars[i].name, name) == 0 && vars[i].shell_id == shell_id && vars[i].is_share) {
			return i;
		}
	}
	return -1;
}

int get_self_var_index(char *name, int shell_id) {
	// 获取当前shell的变量索引
	int i;
	for (i = 0; i < var_num; i++) {
		if (strcmp(vars[i].name, name) == 0 && vars[i].shell_id == shell_id) {
			return i;
		}
	}
	return -1;
}

int is_visible(struct Shell_var var, int shell_id) {
	if (!var.is_valid) {
		return 0;
	} 
	if (var.shell_id != shell_id) {
		return 0;
	}
	return 1;
}

// 环境变量系统调用
int sys_shell_id_alloc() {
	// 给shell分配一个id
	sid ++;
	return sid;
}

int sys_declare_shell_var(int shell_id, char *name, char *value, int share, int rdonly) {
	// printk("调试变量，声明%s : %s\n", name, value);
	// 声明一个环境变量
	int varid = get_self_var_index(name, shell_id);
	if (varid == -1) {
		// 不存在该环境变量，新建一个
		varid = var_num++;
		strcpy(vars[varid].name, name);
		strcpy(vars[varid].value, value);
		vars[varid].shell_id = shell_id;
		vars[varid].is_share = share;
		vars[varid].is_rdonly = rdonly;
		vars[varid].is_valid = 1;
	} else if (vars[varid].is_rdonly) {
		// 存在但只读
		return -1;
	} else {
		// 存在且可以修改
		strcpy(vars[varid].name, name);
		strcpy(vars[varid].value, value);
		vars[varid].shell_id = shell_id;
		vars[varid].is_share = share;
		vars[varid].is_rdonly = rdonly;
		vars[varid].is_valid = 1;
	}
	
	return 0;
}

int sys_get_shell_var(int shell_id, char *name, char *value) {
	// 如果name为NULL，则返回一个多行字符串，每一行是[name]=[value]的形式
	// 如果name不为NULL，则返回name对应的value
	char *tmp = value;
	int i;
	if (name == NULL) {
		for (i = 0; i < var_num; i++) {
			// printk("调试变量%s : %s\n",vars[i].name,vars[i].value);
			if (is_visible(vars[i], shell_id)) {
				strcpy(tmp, vars[i].name);
				tmp += strlen(vars[i].name);
				strcpy(tmp, "=");
				tmp += 1;
				strcpy(tmp, vars[i].value);
				tmp += strlen(vars[i].value);
				*tmp = '\n';
				tmp += 1;
			}
		}
		return 0;
	} else {
		int varid = get_self_var_index(name, shell_id);
		if (varid == -1) {
			return -1;
		}
		if (!is_visible(vars[varid], shell_id)) {
			return -1;
		}
		strcpy(value, vars[varid].value);
		return 0;
	}
}

int sys_unset_shell_var(int shell_id, char *name) {
	// 删除一个环境变量
	int varid = get_self_var_index(name, shell_id);
	if (varid == -1 || vars[varid].is_rdonly) {
		return -1;
	}
	vars[varid].is_valid = 0;
	return 0;
}

int sys_init_shell_var(int shell_id) {
	// 初始化shell的环境变量
	int parent_shell_id = shell_id - 1;
	int i, cur_num = var_num;
	for (i = 0; i < cur_num; i++) {
		if (vars[i].shell_id == parent_shell_id && vars[i].is_share && vars[i].is_valid) {
			// 复制一份。shell_id改为自己的，其余不变
			int varid = var_num++;
			strcpy(vars[varid].name, vars[i].name);
			strcpy(vars[varid].value, vars[i].value);
			vars[varid].shell_id = shell_id;              
			vars[varid].is_share = vars[i].is_share;
			vars[varid].is_rdonly = vars[i].is_rdonly;
			vars[varid].is_valid = vars[i].is_valid;
		}
	}
	return 0;
}

int sys_exit_shell_var(int shell_id) {
	// 退出shell时，清除shell的环境变量
	while (var_num > 0 && vars[var_num - 1].shell_id == shell_id) {
		var_num--;
	}
	sid--;
	return 0;
}




/* Overview:
 * 	This function is used to print a character on screen.
 *
 * Pre-Condition:
 * 	`c` is the character you want to print.
 */
void sys_putchar(int c) {
	printcharc((char)c);
	return;
}

/* Overview:
 * 	This function is used to print a string of bytes on screen.
 *
 * Pre-Condition:
 * 	`s` is base address of the string, and `num` is length of the string.
 */
int sys_print_cons(const void *s, u_int num) {
	if (((u_int)s + num) > UTOP || ((u_int)s) >= UTOP || (s > s + num)) {
		return -E_INVAL;
	}
	u_int i;
	for (i = 0; i < num; i++) {
		printcharc(((char *)s)[i]);
	}
	return 0;
}

/* Overview:
 *	This function provides the environment id of current process.
 *
 * Post-Condition:
 * 	return the current environment id
 */
u_int sys_getenvid(void) {
	return curenv->env_id;
}

/* Overview:
 *   Give up remaining CPU time slice for 'curenv'.
 *
 * Post-Condition:
 *   Another env is scheduled.
 *
 * Hint:
 *   This function will never return.
 */
// void sys_yield(void);
void __attribute__((noreturn)) sys_yield(void) {
	// Hint: Just use 'schedule' with 'yield' set.
	/* Exercise 4.7: Your code here. */
	
	// 把yield置位
	schedule(1);
}

/* Overview:
 * 	This function is used to destroy the current environment.
 *
 * Pre-Condition:
 * 	The parameter `envid` must be the environment id of a
 * process, which is either a child of the caller of this function
 * or the caller itself.
 *
 * Post-Condition:
 *  Returns 0 on success.
 *  Returns the original error if underlying calls fail.
 */
int sys_env_destroy(u_int envid) {
	struct Env *e;
	try(envid2env(envid, &e, 1));

	printk("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}

/* Overview:
 *   Register the entry of user space TLB Mod handler of 'envid'.
 *
 * Post-Condition:
 *   The 'envid''s TLB Mod exception handler entry will be set to 'func'.
 *   Returns 0 on success.
 *   Returns the original error if underlying calls fail.
 */
int sys_set_tlb_mod_entry(u_int envid, u_int func) {
	struct Env *env;

	/* Step 1: Convert the envid to its corresponding 'struct Env *' using 'envid2env'. */
	/* Exercise 4.12: Your code here. (1/2) */

	try(envid2env(envid, &env, 1));

	/* Step 2: Set its 'env_user_tlb_mod_entry' to 'func'. */
	/* Exercise 4.12: Your code here. (2/2) */

	env->env_user_tlb_mod_entry = func;

	return 0;
}

/* Overview:
 *   Check 'va' is illegal or not, according to include/mmu.h
 */
static inline int is_illegal_va(u_long va) {
	return va < UTEMP || va >= UTOP;
}

static inline int is_illegal_va_range(u_long va, u_int len) {
	if (len == 0) {
		return 0;
	}
	return va + len < va || va < UTEMP || va + len > UTOP;
}

/* Overview:
 *   Allocate a physical page and map 'va' to it with 'perm' in the address space of 'envid'.
 *   If 'va' is already mapped, that original page is sliently unmapped.
 *   'envid2env' should be used with 'checkperm' set, like in most syscalls, to ensure the target is
 * either the caller or its child.
 *
 * Post-Condition:
 *   Return 0 on success.
 *   Return -E_BAD_ENV: 'checkperm' of 'envid2env' fails for 'envid'.
 *   Return -E_INVAL:   'va' is illegal (should be checked using 'is_illegal_va').
 *   Return the original error: underlying calls fail (you can use 'try' macro).
 *
 * Hint:
 *   You may want to use the following functions:
 *   'envid2env', 'page_alloc', 'page_insert', 'try' (macro)
 */
int sys_mem_alloc(u_int envid, u_int va, u_int perm) {
	struct Env *env;
	struct Page *pp;

	/* Step 1: Check if 'va' is a legal user virtual address using 'is_illegal_va'. */
	/* Exercise 4.4: Your code here. (1/3) */

	// 检查va是否是合法的用户态虚地址
	if (is_illegal_va(va)) {
		return -E_INVAL;
	}

	/* Step 2: Convert the envid to its corresponding 'struct Env *' using 'envid2env'. */
	/* Hint: **Always** validate the permission in syscalls! */
	/* Exercise 4.4: Your code here. (2/3) */
	
	// 调用envid2env获取envid对应的进程
	try(envid2env(envid, &env, 1));

	/* Step 3: Allocate a physical page using 'page_alloc'. */
	/* Exercise 4.4: Your code here. (3/3) */

	// 分配一个物理页
	try(page_alloc(&pp));

	/* Step 4: Map the allocated page at 'va' with permission 'perm' using 'page_insert'. */
	return page_insert(env->env_pgdir, env->env_asid, pp, va, perm);
}

/* Overview:
 *   Find the physical page mapped at 'srcva' in the address space of env 'srcid', and map 'dstid''s
 *   'dstva' to it with 'perm'.
 *
 * Post-Condition:
 *   Return 0 on success.
 *   Return -E_BAD_ENV: 'checkperm' of 'envid2env' fails for 'srcid' or 'dstid'.
 *   Return -E_INVAL: 'srcva' or 'dstva' is illegal, or 'srcva' is unmapped in 'srcid'.
 *   Return the original error: underlying calls fail.
 *
 * Hint:
 *   You may want to use the following functions:
 *   'envid2env', 'page_lookup', 'page_insert'
 */
int sys_mem_map(u_int srcid, u_int srcva, u_int dstid, u_int dstva, u_int perm) {
	struct Env *srcenv;
	struct Env *dstenv;
	struct Page *pp;

	/* Step 1: Check if 'srcva' and 'dstva' are legal user virtual addresses using
	 * 'is_illegal_va'. */
	/* Exercise 4.5: Your code here. (1/4) */

	// 检查虚地址是否合法 
	if (is_illegal_va(srcva) || is_illegal_va(dstva)) {
		return -E_INVAL;
	}

	/* Step 2: Convert the 'srcid' to its corresponding 'struct Env *' using 'envid2env'. */
	/* Exercise 4.5: Your code here. (2/4) */
	
	// 获取srcid对应的进程
	try(envid2env(srcid, &srcenv, 1));

	/* Step 3: Convert the 'dstid' to its corresponding 'struct Env *' using 'envid2env'. */
	/* Exercise 4.5: Your code here. (3/4) */

	// 获取dstid对应的进程
	try(envid2env(dstid, &dstenv, 1));

	/* Step 4: Find the physical page mapped at 'srcva' in the address space of 'srcid'. */
	/* Return -E_INVAL if 'srcva' is not mapped. */
	/* Exercise 4.5: Your code here. (4/4) */

	// 在srcid的地址空间内找到一个物理页与srcva进行映射
	// 如果srcva没有映射则报错
	pp = page_lookup(srcenv->env_pgdir, srcva, NULL);
	if (pp == NULL) {
		return -E_INVAL;
	}

	/* Step 5: Map the physical page at 'dstva' in the address space of 'dstid'. */
	return page_insert(dstenv->env_pgdir, dstenv->env_asid, pp, dstva, perm);
}

/* Overview:
 *   Unmap the physical page mapped at 'va' in the address space of 'envid'.
 *   If no physical page is mapped there, this function silently succeeds.
 *
 * Post-Condition:
 *   Return 0 on success.
 *   Return -E_BAD_ENV: 'checkperm' of 'envid2env' fails for 'envid'.
 *   Return -E_INVAL:   'va' is illegal.
 *   Return the original error when underlying calls fail.
 */
int sys_mem_unmap(u_int envid, u_int va) {
	struct Env *e;

	/* Step 1: Check if 'va' is a legal user virtual address using 'is_illegal_va'. */
	/* Exercise 4.6: Your code here. (1/2) */

	// 检查虚地址合法性
	if (is_illegal_va(va)) {
		return -E_INVAL;
	}

	/* Step 2: Convert the envid to its corresponding 'struct Env *' using 'envid2env'. */
	/* Exercise 4.6: Your code here. (2/2) */

	// 获取对应的进程
	try(envid2env(envid, &e, 1));

	/* Step 3: Unmap the physical page at 'va' in the address space of 'envid'. */
	page_remove(e->env_pgdir, e->env_asid, va);
	return 0;
}

/* Overview:
 *   Allocate a new env as a child of 'curenv'.
 *
 * Post-Condition:
 *   Returns the child's envid on success, and
 *   - The new env's 'env_tf' is copied from the kernel stack, except for $v0 set to 0 to indicate
 *     the return value in child.
 *   - The new env's 'env_status' is set to 'ENV_NOT_RUNNABLE'.
 *   - The new env's 'env_pri' is copied from 'curenv'.
 *   Returns the original error if underlying calls fail.
 *
 * Hint:
 *   This syscall works as an essential step in user-space 'fork' and 'spawn'.
 */
int sys_exofork(void) {
	struct Env *e;

	/* Step 1: Allocate a new env using 'env_alloc'. */
	/* Exercise 4.9: Your code here. (1/4) */
	
	// 分配一个新进程
	try(env_alloc(&e, curenv->env_id));

	/* Step 2: Copy the current Trapframe below 'KSTACKTOP' to the new env's 'env_tf'. */
	/* Exercise 4.9: Your code here. (2/4) */
	
	e->env_tf = *((struct Trapframe *)KSTACKTOP - 1);

	/* Step 3: Set the new env's 'env_tf.regs[2]' to 0 to indicate the return value in child. */
	/* Exercise 4.9: Your code here. (3/4) */
	
	// 系统调用在内核态返回的envid 只传递给父进程，对于子进程，则需要将其现场中的 v0 寄存器修改为 0。
	e->env_tf.regs[2] = 0;

	/* Step 4: Set up the new env's 'env_status' and 'env_pri'.  */
	/* Exercise 4.9: Your code here. (4/4) */
	
	// 设置"不可运行"状态，并字段初始化
	e->env_status = ENV_NOT_RUNNABLE;
	e->env_pri = curenv->env_pri;

	return e->env_id;
}

/* Overview:
 *   Set 'envid''s 'env_status' to 'status' and update 'env_sched_list'.
 *
 * Post-Condition:
 *   Returns 0 on success.
 *   Returns -E_INVAL if 'status' is neither 'ENV_RUNNABLE' nor 'ENV_NOT_RUNNABLE'.
 *   Returns the original error if underlying calls fail.
 *
 * Hint:
 *   The invariant that 'env_sched_list' contains and only contains all runnable envs should be
 *   maintained.
 */
int sys_set_env_status(u_int envid, u_int status) {
	struct Env *env;

	/* Step 1: Check if 'status' is valid. */
	/* Exercise 4.14: Your code here. (1/3) */
	
	// 检测合法
	if (status != ENV_RUNNABLE && status != ENV_NOT_RUNNABLE) {
		return -E_INVAL;
	}

	/* Step 2: Convert the envid to its corresponding 'struct Env *' using 'envid2env'. */
	/* Exercise 4.14: Your code here. (2/3) */
	
	// 获取env
	try(envid2env(envid, &env, 1));

	/* Step 3: Update 'env_sched_list' if the 'env_status' of 'env' is being changed. */
	/* Exercise 4.14: Your code here. (3/3) */
	
	// 更新进程调度队列
	if (status == ENV_RUNNABLE && env->env_status != ENV_RUNNABLE) {
		TAILQ_INSERT_TAIL(&env_sched_list, env, env_sched_link);
	} else if (status == ENV_NOT_RUNNABLE && env->env_status != ENV_NOT_RUNNABLE) {
		TAILQ_REMOVE(&env_sched_list, env, env_sched_link);
	}

	/* Step 4: Set the 'env_status' of 'env'. */
	env->env_status = status;

	/* Step 5: Use 'schedule' with 'yield' set if ths 'env' is 'curenv'. */
	if (env == curenv) {
		schedule(1);
	}
	return 0;
}

/* Overview:
 *  Set envid's trap frame to 'tf'.
 *
 * Post-Condition:
 *  The target env's context is set to 'tf'.
 *  Returns 0 on success (except when the 'envid' is the current env, so no value could be
 * returned).
 *  Returns -E_INVAL if the environment cannot be manipulated or 'tf' is invalid.
 *  Returns the original error if other underlying calls fail.
 */
int sys_set_trapframe(u_int envid, struct Trapframe *tf) {
	if (is_illegal_va_range((u_long)tf, sizeof *tf)) {
		return -E_INVAL;
	}
	struct Env *env;
	try(envid2env(envid, &env, 1));
	if (env == curenv) {
		*((struct Trapframe *)KSTACKTOP - 1) = *tf;
		// return `tf->regs[2]` instead of 0, because return value overrides regs[2] on
		// current trapframe.
		return tf->regs[2];
	} else {
		env->env_tf = *tf;
		return 0;
	}
}

/* Overview:
 * 	Kernel panic with message `msg`.
 *
 * Post-Condition:
 * 	This function will halt the system.
 */
void sys_panic(char *msg) {
	panic("%s", TRUP(msg));
}

/* Overview:
 *   Wait for a message (a value, together with a page if 'dstva' is not 0) from other envs.
 *   'curenv' is blocked until a message is sent.
 *
 * Post-Condition:
 *   Return 0 on success.
 *   Return -E_INVAL: 'dstva' is neither 0 nor a legal address.
 */
int sys_ipc_recv(u_int dstva) {
	/* Step 1: Check if 'dstva' is either zero or a legal address. */
	if (dstva != 0 && is_illegal_va(dstva)) {
		return -E_INVAL;
	}

	/* Step 2: Set 'curenv->env_ipc_recving' to 1. */
	/* Exercise 4.8: Your code here. (1/8) */

	// 把reciving置为1
	curenv->env_ipc_recving = 1;

	/* Step 3: Set the value of 'curenv->env_ipc_dstva'. */
	/* Exercise 4.8: Your code here. (2/8) */
	
	// 设置dstva
	curenv->env_ipc_dstva = dstva;

	/* Step 4: Set the status of 'curenv' to 'ENV_NOT_RUNNABLE' and remove it from
	 * 'env_sched_list'. */
	/* Exercise 4.8: Your code here. (3/8) */
	
	// 设置当前进程为“不可运行”，并把它移出调度队列
	curenv->env_status = ENV_NOT_RUNNABLE;
	TAILQ_REMOVE(&env_sched_list, curenv, env_sched_link);

	/* Step 5: Give up the CPU and block until a message is received. */
	((struct Trapframe *)KSTACKTOP - 1)->regs[2] = 0;
	schedule(1);
}

/* Overview:
 *   Try to send a 'value' (together with a page if 'srcva' is not 0) to the target env 'envid'.
 *
 * Post-Condition:
 *   Return 0 on success, and the target env is updated as follows:
 *   - 'env_ipc_recving' is set to 0 to block future sends.
 *   - 'env_ipc_from' is set to the sender's envid.
 *   - 'env_ipc_value' is set to the 'value'.
 *   - 'env_status' is set to 'ENV_RUNNABLE' again to recover from 'ipc_recv'.
 *   - if 'srcva' is not NULL, map 'env_ipc_dstva' to the same page mapped at 'srcva' in 'curenv'
 *     with 'perm'.
 *
 *   Return -E_IPC_NOT_RECV if the target has not been waiting for an IPC message with
 *   'sys_ipc_recv'.
 *   Return the original error when underlying calls fail.
 */
int sys_ipc_try_send(u_int envid, u_int value, u_int srcva, u_int perm) {
	struct Env *e;
	struct Page *p;

	/* Step 1: Check if 'srcva' is either zero or a legal address. */
	/* Exercise 4.8: Your code here. (4/8) */
	
	// 检查srcva非0且合法
	if (srcva != 0 && is_illegal_va(srcva)) {
		return -E_INVAL;
	}

	/* Step 2: Convert 'envid' to 'struct Env *e'. */
	/* This is the only syscall where the 'envid2env' should be used with 'checkperm' UNSET,
	 * because the target env is not restricted to 'curenv''s children. */
	/* Exercise 4.8: Your code here. (5/8) */
	
	// 获取env
	try(envid2env(envid, &e, 0));

	/* Step 3: Check if the target is waiting for a message. */
	/* Exercise 4.8: Your code here. (6/8) */
	
	if (e->env_ipc_recving == 0) {
		return -E_IPC_NOT_RECV;
	}

	/* Step 4: Set the target's ipc fields. */
	e->env_ipc_value = value;
	e->env_ipc_from = curenv->env_id;
	e->env_ipc_perm = PTE_V | perm;
	e->env_ipc_recving = 0;

	/* Step 5: Set the target's status to 'ENV_RUNNABLE' again and insert it to the tail of
	 * 'env_sched_list'. */
	/* Exercise 4.8: Your code here. (7/8) */
	
	// 恢复状态设置
	e->env_status = ENV_RUNNABLE;
	TAILQ_INSERT_TAIL(&env_sched_list, e, env_sched_link);

	/* Step 6: If 'srcva' is not zero, map the page at 'srcva' in 'curenv' to 'e->env_ipc_dstva'
	 * in 'e'. */
	/* Return -E_INVAL if 'srcva' is not zero and not mapped in 'curenv'. */
	if (srcva != 0) {
		/* Exercise 4.8: Your code here. (8/8) */
		
		// 找到物理页，尝试insert,建立映射
		p = page_lookup(curenv->env_pgdir, srcva, NULL);
		if (p == NULL) {
			return -E_INVAL;
		}
		try(page_insert(e->env_pgdir, e->env_asid, p, e->env_ipc_dstva, perm));

	}
	return 0;
}

// XXX: kernel does busy waiting here, blocking all envs
int sys_cgetc(void) {
	int ch;
	while ((ch = scancharc()) == 0) {
	}
	return ch;
}

/* Overview:
 *  This function is used to write data at 'va' with length 'len' to a device physical address
 *  'pa'. Remember to check the validity of 'va' and 'pa' (see Hint below);
 *
 *  'va' is the starting address of source data, 'len' is the
 *  length of data (in bytes), 'pa' is the physical address of
 *  the device (maybe with a offset).
 *
 * Pre-Condition:
 *  'len' must be 1, 2 or 4, otherwise return -E_INVAL.
 *
 * Post-Condition:
 *  Data within [va, va+len) is copied to the physical address 'pa'.
 *  Return 0 on success.
 *  Return -E_INVAL on bad address.
 *
 * Hint:
 *  You can use 'is_illegal_va_range' to validate 'va'.
 *  You may use the unmapped and uncached segment in kernel address space (KSEG1)
 *  to perform MMIO by assigning a corresponding-lengthed data to the address,
 *  or you can just simply use the io function defined in 'include/io.h',
 *  such as 'iowrite32', 'iowrite16' and 'iowrite8'.
 *
 *  All valid device and their physical address ranges:
 *	* ---------------------------------*
 *	|   device   | start addr | length |
 *	* -----------+------------+--------*
 *	|  console   | 0x180003f8 | 0x20   |
 *	|  IDE disk  | 0x180001f0 | 0x8    |
 *	* ---------------------------------*
 */

int valid_addr_space_num = 2;
unsigned int valid_addr_start[2] = {0x180003f8, 0x180001f0};
unsigned int valid_addr_end[2] = {0x180003f8 + 0x20, 0x180001f8};

// 辅助函数，判断物理地址范围的合法性
static inline int is_illegal_dev_range(u_long pa, u_long len) {
	if ((pa % 4 != 0 && len != 1 && len != 2) || (pa % 2 != 0 && len != 1)) {
		return 1;
	}
	int i;
	u_int target_start = pa;
	u_int target_end = pa + len;
	for (i = 0; i < valid_addr_space_num; i++) {
		if (target_start >= valid_addr_start[i] && target_end <= valid_addr_end[i]) {
			return 0;
		}
	}
	return 1;
}

int sys_write_dev(u_int va, u_int pa, u_int len) {
	/* Exercise 5.1: Your code here. (1/2) */

	// 判断地址区间的合法性
	if (is_illegal_va_range(va, len) || is_illegal_dev_range(pa, len) || va % len != 0) {
		return -E_INVAL;
	}
	// 根据写字、写半字、写字节进行分类，调用不同的iowrite
	if (len == 4) {
		iowrite32(*(uint32_t *)va, pa);
	} else if (len == 2) {
		iowrite16(*(uint16_t *)va, pa);
	} else if (len == 1) {
		iowrite8(*(uint8_t *)va, pa);
	} else {
		return -E_INVAL;
	}
	return 0;
}

/* Overview:
 *  This function is used to read data from a device physical address.
 *
 * Pre-Condition:
 *  'len' must be 1, 2 or 4, otherwise return -E_INVAL.
 *
 * Post-Condition:
 *  Data at 'pa' is copied from device to [va, va+len).
 *  Return 0 on success.
 *  Return -E_INVAL on bad address.
 *
 * Hint:
 *  You can use 'is_illegal_va_range' to validate 'va'.
 *  You can use function 'ioread32', 'ioread16' and 'ioread8' to read data from device.
 */
int sys_read_dev(u_int va, u_int pa, u_int len) {
	/* Exercise 5.1: Your code here. (2/2) */

	// 判断地址区间的合法性
	if (is_illegal_va_range(va, len) || is_illegal_dev_range(pa, len) || va % len != 0) {
		return -E_INVAL;
	}
	// 根据读字、读半字、读字节进行分类，调用不同的ioread
	if (len == 4) {
		*(uint32_t *)va = ioread32(pa);
	} else if (len == 2) {
		*(uint16_t *)va = ioread16(pa);
	} else if (len == 1) {
		*(uint8_t *)va = ioread8(pa);
	} else {
		return -E_INVAL;
	}
	return 0;
}

void *syscall_table[MAX_SYSNO] = {
    [SYS_putchar] = sys_putchar,
    [SYS_print_cons] = sys_print_cons,
    [SYS_getenvid] = sys_getenvid,
    [SYS_yield] = sys_yield,
    [SYS_env_destroy] = sys_env_destroy,
    [SYS_set_tlb_mod_entry] = sys_set_tlb_mod_entry,
    [SYS_mem_alloc] = sys_mem_alloc,
    [SYS_mem_map] = sys_mem_map,
    [SYS_mem_unmap] = sys_mem_unmap,
    [SYS_exofork] = sys_exofork,
    [SYS_set_env_status] = sys_set_env_status,
    [SYS_set_trapframe] = sys_set_trapframe,
    [SYS_panic] = sys_panic,
    [SYS_ipc_try_send] = sys_ipc_try_send,
    [SYS_ipc_recv] = sys_ipc_recv,
    [SYS_cgetc] = sys_cgetc,
    [SYS_write_dev] = sys_write_dev,
    [SYS_read_dev] = sys_read_dev,
	// shell新增
	[SYS_workdir] = sys_workdir, 
	[SYS_return_value] = sys_return_value,
	[SYS_shell_id_alloc] = sys_shell_id_alloc,
	[SYS_declare_shell_var] = sys_declare_shell_var,
	[SYS_get_shell_var] = sys_get_shell_var,
	[SYS_unset_shell_var] = sys_unset_shell_var,
	[SYS_init_shell_var] = sys_init_shell_var,
	[SYS_exit_shell_var] = sys_exit_shell_var,
};

/* Overview:
 *   Call the function in 'syscall_table' indexed at 'sysno' with arguments from user context and
 * stack.
 *
 * Hint:
 *   Use sysno from $a0 to dispatch the syscall.
 *   The possible arguments are stored at $a1, $a2, $a3, [$sp + 16 bytes], [$sp + 20 bytes] in
 *   order.
 *   Number of arguments cannot exceed 5.
 */
void do_syscall(struct Trapframe *tf) {
	int (*func)(u_int, u_int, u_int, u_int, u_int);
	int sysno = tf->regs[4];
	if (sysno < 0 || sysno >= MAX_SYSNO) {
		tf->regs[2] = -E_NO_SYS;
		return;
	}

	/* Step 1: Add the EPC in 'tf' by a word (size of an instruction). */
	/* Exercise 4.2: Your code here. (1/4) */

	// EPC加上一个指令偏移的地址
	tf->cp0_epc += 4;

	/* Step 2: Use 'sysno' to get 'func' from 'syscall_table'. */
	/* Exercise 4.2: Your code here. (2/4) */

	// 用sysno获取func数组中对应的处理函数指针
	func = syscall_table[sysno];

	/* Step 3: First 3 args are stored in $a1, $a2, $a3. */
	u_int arg1 = tf->regs[5];
	u_int arg2 = tf->regs[6];
	u_int arg3 = tf->regs[7];

	/* Step 4: Last 2 args are stored in stack at [$sp + 16 bytes], [$sp + 20 bytes]. */
	u_int arg4, arg5;
	/* Exercise 4.2: Your code here. (3/4) */
	
	// arg4和arg5分别存在栈指针sp加上4x4和4x5个字节处。
	// 这是由于arg0-4虽然不用栈空间传参，仍然需要为它们预留空间，故arg4-5存在栈空间16-20 bytes处。
	u_int *p_arg4 = (u_int *)tf->regs[29] + 4;
	u_int *p_arg5 = (u_int *)tf->regs[29] + 5;
	arg4 = *p_arg4;
	arg5 = *p_arg5;

	/* Step 5: Invoke 'func' with retrieved arguments and store its return value to $v0 in 'tf'.
	 */
	/* Exercise 4.2: Your code here. (4/4) */
	
	// 利用arg1-5调用func函数，把返回值存到v0寄存器
	tf->regs[2] = func(arg1, arg2, arg3, arg4, arg5);

}
