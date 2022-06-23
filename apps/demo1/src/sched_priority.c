/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel/thread_stack.h>
#include <string.h>

#define STACK_SIZE 1024

#define H_PRIORITY 3
#define M_PRIORITY 5
#define L_PRIORITY 7

struct k_thread t1_thread_data;
struct k_thread t2_thread_data;
struct k_thread t3_thread_data;

K_THREAD_STACK_DEFINE(t1_stack_area, STACK_SIZE);
K_THREAD_STACK_DEFINE(t2_stack_area, STACK_SIZE);
K_THREAD_STACK_DEFINE(t3_stack_area, STACK_SIZE);

void counter_thread(void * n, void * e, void * unused)
{
	int id = (int)n;
	int limit = (int)e;
	printk("T%d: HELLO\n", id);
	for (int i = 0; i < limit * 1000000; i++) {
		if (i % 1000000 == 0) {
			printk("T%d\n", id);
		}
	}
	printk("T%d: BYE\n", id);
}

int main()
{
	k_tid_t t1_tid = k_thread_create(&t1_thread_data, t1_stack_area,
					 K_THREAD_STACK_SIZEOF(t1_stack_area), 
					 counter_thread, ((void *)1), ((void *)1), NULL, H_PRIORITY, 0, K_MSEC(20));

	k_tid_t t2_tid = k_thread_create(&t2_thread_data, t2_stack_area,
					 K_THREAD_STACK_SIZEOF(t2_stack_area), 
					 counter_thread, ((void*)2), ((void*)5), NULL, M_PRIORITY, 0, K_MSEC(10));

	k_tid_t t3_tid = k_thread_create(&t3_thread_data, t3_stack_area,
					 K_THREAD_STACK_SIZEOF(t3_stack_area), 
					 counter_thread, ((void*)3), ((void*)10), NULL, L_PRIORITY, 0, K_NO_WAIT);
	return 0;
}
