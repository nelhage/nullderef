/*
 * Copyright (C) 2010 Nelson Elhage
 *
 * Trivial kernel module to provoke a NULL pointer dereference.
 *
 * Based in part on provoke-crash.c
 * <http://thread.gmane.org/gmane.linux.kernel/942633>
 *
 * Author: Nelson Elhage <nelhage@ksplice.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 */
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/hardirq.h>
#include <linux/debugfs.h>

struct my_ops {
	ssize_t (*do_it)(void);
};

static struct my_ops *ops = NULL;

static ssize_t null_read_write(struct file *f, const char __user *buf,
		size_t count, loff_t *off)
{
	f->private_data = ops->do_it;

	return count;
}

static ssize_t null_call_write(struct file *f, const char __user *buf,
		size_t count, loff_t *off)
{
	return ops->do_it();
}

static struct dentry *nullderef_root;

static const struct file_operations null_read_fops = {
	.write = null_read_write,
};
static const struct file_operations null_call_fops = {
	.write = null_call_write,
};

static int __init nullderef_init(void)
{
	static struct dentry *read_de, *call_de;

	nullderef_root = debugfs_create_dir("nullderef", NULL);
	if (!nullderef_root) {
		printk(KERN_ERR "nullderef: creating root dir failed\n");
		return -ENODEV;
	}

	read_de = debugfs_create_file("null_read", 0222, nullderef_root,
				      NULL, &null_read_fops);
	call_de = debugfs_create_file("null_call", 0222, nullderef_root,
				      NULL, &null_call_fops);

	if (!read_de || !call_de)
		goto out_err;

	return 0;
out_err:
	debugfs_remove_recursive(nullderef_root);

	return -ENODEV;
}

static void __exit nullderef_exit(void)
{
	debugfs_remove_recursive(nullderef_root);
}

module_init(nullderef_init);
module_exit(nullderef_exit);

MODULE_AUTHOR("Nelson Elhage <nelhage@ksplice.com>");
MODULE_DESCRIPTION("Provides debugfs files to trigger NULL pointer dereferences.");
MODULE_LICENSE("GPL");
