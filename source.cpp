#include <algorithm>

#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <immintrin.h>

#include "iostream.h"

int* i = (int*)17;//(int*)malloc(512);
void handler(int sig, siginfo_t* info, void* ucontext);

int main() {
    struct sigaction action;
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO | SA_NODEFER;
    auto sigset = sigset_t();
    sigemptyset(&sigset);
    action.sa_mask = sigset;
    sigaction(SIGSEGV, &action, 0);
    for (int* j = i; ; j--) {
        j[0] = 0;
    }
    return 0;
}

void print_reg(const char* reg, long long n) {
    sig::cout << reg << "   ";
    sig::cout << n << "     0x";
    sig::cout << sig::hex;
    sig::cout << n;
    sig::cout << sig::dec << '\n';
}
#include <iostream>
void handler(int sig, siginfo_t* info, void* ucontext) {
    sig::cout << "seg fault\nWe're just collecting some error info...\n\n";
    ucontext_t* context = (ucontext_t*)ucontext;
    long long* gregs = context->uc_mcontext.gregs;

    print_reg("rax", gregs[REG_RAX]);
    print_reg("rbx", gregs[REG_RBX]);
    print_reg("rcx", gregs[REG_RCX]);
    print_reg("rdx", gregs[REG_RDX]);
    print_reg("rdi", gregs[REG_RSI]);
    print_reg("rsi", gregs[REG_RSI]);
    print_reg("rbp", gregs[REG_RBP]);
    print_reg("rsp", gregs[REG_RSP]);

    print_reg("r8 ", gregs[REG_R8]);
    print_reg("r9 ", gregs[REG_R9]);
    print_reg("r10", gregs[REG_R10]);
    print_reg("r11", gregs[REG_R11]);
    print_reg("r12", gregs[REG_R12]);
    print_reg("r13", gregs[REG_R13]);
    print_reg("r14", gregs[REG_R14]);
    print_reg("r15", gregs[REG_R15]);

    sig::cout << '\n';
    print_reg("RIP", gregs[REG_RIP]);
    print_reg("eflags", gregs[REG_EFL]);
    sig::cout << "\nMemory dump around seg fault:\n";
    u_int8_t* spot = (u_int8_t*)info->si_addr;
    sig::cout << sig::hex;
    auto start = spot;
    if (start >= (u_int8_t*)48) {
        start -= 48;
    } else {
        while (start >= (u_int8_t*)16) {
            start -= 16;
        }
        if (start) {                                
            sig::cout << "                         ";
            for (int j = 0; j < 16 - (long long)start; j++) {
                sig::cout << "   ";
            }
            sig::cout << sig::precision(2);
            sig::cout << sig::hex;
            for (u_int8_t* j = 0; j < start; j++) {
                sig::cout << j << ' ';
            }
            sig::cout << '\n';
        }
    }
    for (
        u_int8_t* i = start; 
        i < spot + 48 || spot >= (u_int8_t*)(ULLONG_MAX - 48); 
        i += 16
    ) {
        sig::cout << sig::precision(16);
        if (i == spot) {
            sig::cout << "==> ";
        } else {
            sig::cout << "    ";
        }
        sig::cout << "0x" << (long long)i << ":  ";
        sig::cout << sig::precision(2);
        for (int j = 0; j < 16; j++) {
            sig::cout << i+j << ' ';
            if ((unsigned long long)(i + j) == ULLONG_MAX) {
                break;
            }
        }
        sig::cout << '\n';
        if (i > (u_int8_t*)(ULLONG_MAX - 16)) {
            break;
        }
    }
    exit(EXIT_FAILURE);
}
