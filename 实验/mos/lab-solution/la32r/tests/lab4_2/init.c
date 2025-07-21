void la32r_init() {
	printk("init.c:\tla32r_init() is called\n");

	la32r_detect_memory();
	la32r_vm_init();
	page_init();
	env_init();

	struct Env *ppb = ENV_CREATE_PRIORITY(test_ppb, 5);
	struct Env *ppc = ENV_CREATE_PRIORITY(test_ppc, 5);
	ppc->env_parent_id = ppb->env_id;

	schedule(0);
	panic("init.c:\tend of la32r_init() reached!");
}
