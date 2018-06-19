#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x59caa4c3, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x1d7d15c9, __VMLINUX_SYMBOL_STR(debugfs_remove) },
	{ 0x24598c4e, __VMLINUX_SYMBOL_STR(misc_deregister) },
	{ 0xe111afde, __VMLINUX_SYMBOL_STR(klisten) },
	{ 0xe0de76ac, __VMLINUX_SYMBOL_STR(kbind) },
	{ 0xc34cc613, __VMLINUX_SYMBOL_STR(ksocket) },
	{ 0x4c4fef19, __VMLINUX_SYMBOL_STR(kernel_stack) },
	{ 0x774f6678, __VMLINUX_SYMBOL_STR(misc_register) },
	{ 0x21585d42, __VMLINUX_SYMBOL_STR(debugfs_create_file) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xed2a538c, __VMLINUX_SYMBOL_STR(ksend) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x10cde6fd, __VMLINUX_SYMBOL_STR(inet_ntoa) },
	{ 0x249a625, __VMLINUX_SYMBOL_STR(kclose) },
	{ 0x2e258755, __VMLINUX_SYMBOL_STR(kaccept) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x8d3ad329, __VMLINUX_SYMBOL_STR(pv_mmu_ops) },
	{ 0x92ae1f74, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ksocket";


MODULE_INFO(srcversion, "8F53F343428D36EFD71C64F");
