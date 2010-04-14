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
 * Simplified greatly by Michal Nazarewicz <mina86@mina86.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 */
#include <linux/module.h>

/*
 * Define an 'ops' struct containing a single mostly-pointless
 * function. We just do this to try to make this code look vaguely
 * like something that the actual kernel might contain.
 */
struct my_ops {
	ssize_t (*do_it)(void);
};

/* Define a pointer to our ops struct, "accidentally" initialized to NULL. */
static struct my_ops *ops = NULL;

/*
 * Reading the 'read' file just reads the value of the do_it function
 * pointer from the NULL ops pointer.
 */
static int null_read(char *buffer, struct kernel_param *kp)
{
	return sprintf(buffer, "%p\n", (void *)ops->do_it);
}

module_param_call(read, NULL, null_read, NULL, 0444);

/*
 * Reading the 'call' file calls the do_it member of ops, which
 * results in reading a function pointer from NULL and then calling
 * it.
 */
static int null_call(char *buffer, struct kernel_param *kp)
{
	ssize_t ret = ops->do_it();
	return sprintf(buffer, "%zd\n", ret);
}

module_param_call(call, NULL, null_call, NULL, 0444);


MODULE_AUTHOR("Nelson Elhage <nelhage@ksplice.com>");
MODULE_DESCRIPTION("Provides sysfs files to trigger NULL pointer dereferences.");
MODULE_LICENSE("GPL");
