#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xd66e1b58, "module_layout" },
	{ 0x43745509, "param_ops_charp" },
	{ 0xb7700415, "param_ops_int" },
	{ 0x32277322, "cdev_del" },
	{ 0xedc03953, "iounmap" },
	{ 0xe6ff264b, "device_destroy" },
	{ 0x88f4b5ca, "cdev_add" },
	{ 0x816cb6c9, "cdev_init" },
	{ 0xa836db36, "class_destroy" },
	{ 0x62686042, "device_create" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xe609c355, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x1d37eeed, "ioremap" },
	{ 0x7c32d0f0, "printk" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x28118cb6, "__get_user_1" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "E4C850B8677929F93CD6852");
