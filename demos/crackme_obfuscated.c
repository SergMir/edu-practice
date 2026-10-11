#define _GNU_SOURCE

#include <sys/ptrace.h>
#include <sys/mman.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <signal.h>
#include <ucontext.h>
#include <unistd.h>

#include "check_creds.h"

#if defined (__x86_64__)
#	define GUARD_SIZE 2
#	define GUARD() do { asm volatile("ud2\n\t"); } while(0)
#elif defined (__riscv) && __riscv_xlen == 64
#	define GUARD_SIZE 4
#	define GUARD() do { asm volatile(".word 0xDEB65AFE\n\t"); } while(0)
#else
#	error "Unsupported arch"
#endif

static uint64_t source_hash = 0;

uint64_t texthash() {
	extern void* __text_start;
	extern void* __text_end;
	unsigned long hash = 0;

	for (unsigned char *c = (unsigned char*)&__text_start;
			c < (unsigned char*)&__text_end; ++c)
		hash = hash * 33 + (uint64_t)*c;

	return hash;
}

void stuck_protector() {
	uint64_t current_hash = texthash();
	if (current_hash != source_hash) {
		puts("REMOVE YOUR DUMMY SOFTWARE BREAKPOINTS, DUDE!\n");
		abort();
	}
}

void sigill_handler(int signo, siginfo_t *info, void *ucontext) {
	(void)signo;
	(void)info;

	/* OBFUSCATED */

	ucontext_t *uc = (ucontext_t*)ucontext;
#if defined(__x86_64__)
	uc->uc_mcontext.gregs[REG_RIP]
#elif defined(__riscv) &&  __riscv_xlen == 64
	uc->uc_mcontext.__gregs[REG_PC]
#else
#	error "Unsupported arch"
#endif
	+= GUARD_SIZE;
}

__attribute__((section(".plt.goat"), constructor(101)))
static void executable_hash() {
	source_hash = texthash();

	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sa.sa_sigaction = sigill_handler;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGILL, &sa, NULL) == -1) {
		perror("CRACKME ITSELF WORKS WRONG, PLEASE CALL THE AUTHOR");
		abort();
	};
}

void do_decode_decoder(void* data, unsigned long l);

__attribute__((noinline))
void *decode_decoder() {
	/* OBFUSCATED */
	/* OBFUSCATED */
	void *data = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (data == NULL) {
		perror("CRACKME ITSELF WORKS WRONG, PLEASE CALL THE AUTHOR");
		abort();
	}

	memcpy(data, check_creds_enc, check_creds_enc_len);

	do_decode_decoder(data, check_creds_enc_len);
	/* OBFUSCATED */
	/* OBFUSCATED */
	/* OBFUSCATED */
	/* OBFUSCATED */
	/* OBFUSCATED */
	/* OBFUSCATED */
	/* OBFUSCATED */
	return data;
}

static char login_buf[16] = {};
static char password_buf[16] = {};

void read_creds() {
	printf("Login: ");
	(void)scanf("%15s", login_buf);

	printf("Password: ");
	(void)scanf("%15s", password_buf);
}

/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
/* OBFUSCATED */
