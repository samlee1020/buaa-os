static inline void pre_env_run(struct Env *e) {
	struct Trapframe *tf = (curenv == e) ? (struct Trapframe *)KSTACKTOP - 1 : &e->env_tf;
	u_int era = tf->era;
	if (era == 0x400180) {
		printk("env %08x reached end pc: 0x%08x, $a0=0x%08x\n", e->env_id, era,
		       tf->regs[4]);
		env_destroy(e);
		schedule(0);
	}
}
