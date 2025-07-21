#include <env.h>
#include <lib.h>
int wait(u_int envid) {
	const volatile struct Env *e;

	e = &envs[ENVX(envid)];
	while (e->env_id == envid && e->env_status != ENV_FREE) {
		syscall_yield();
	}

	// shell新加部分
	int r = syscall_return_value(0, 1, envid);
	return r;
}
