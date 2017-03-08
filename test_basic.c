/*
 * Copyright 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * test_basic.c -- unit tests for strace.eBPF
 */

#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/syscall.h>
#include <linux/futex.h>

#define PATTERN_START	0x12345678
#define PATTERN_END	0x87654321

#define MARK_START()	close(PATTERN_START)
#define MARK_END()	close(PATTERN_END)


/*
 * test_0 -- test basic syscalls
 */
static void test_0(void)
{
	MARK_START();
	syscall(SYS_futex, 1, 2, 3, 4, 5, 6); /* futex */
	MARK_END();
}

/*
 * test_2 -- test unsupported syscalls
 */
static void test_2(void)
{
	MARK_START();
	syscall(SYS_poll, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107);
	MARK_END();
}

/*
 * run_test -- array of tests
 */
static void (*run_test[])(void) = {
	test_0,
	test_2
};

int
main(int argc, char *argv[])
{
	int max = sizeof(run_test) / sizeof(run_test[0]) - 1;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <test-number: 0..%i>\n",
				argv[0], max);
		return -1;
	}

	int n = atoi(argv[1]);
	if (n > max) {
		fprintf(stderr, "Error: test number can take only following"
				" values: 0..%i (%i is not allowed)\n",
				max, n);
		return -1;
	}

	/* Hook to syncronize with trace.py */
	if (NULL != getenv("WAIT_TRACE_PY"))
		raise(SIGSTOP);

	printf("Starting: test_%i ...\n", n);

	run_test[n]();

	printf("Done (test_%i)\n", n);
}
