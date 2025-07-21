void la32r_init() {
	printk("init.c:\tla32r_init() is called\n");
	la32r_detect_memory();
	la32r_vm_init();
	page_init();
	env_init();

	envid2env_check();
	halt();
}
