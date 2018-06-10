#include "iostream.h"

#include <signal.h>
#include <ucontext.h>
#include <limits.h>

#include <iostream>

extern "C" bool memory_checker(const u_int8_t* pointer);

struct mem_t {
    bool valid;
    u_int8_t data;
};

void _handler(int sig, siginfo_t* info, void* ucontext) {
    ucontext_t* context = (ucontext_t*)ucontext;
    context->uc_mcontext.gregs[REG_RIP] += context->uc_mcontext.gregs[REG_RBX];
    context->__fpregs_mem.mxcsr = 895;
    setcontext(context);
}

mem_t get_mem(const u_int8_t* pointer) {
    struct sigaction action;
    action.sa_sigaction = _handler;
    auto sigset = sigset_t();
    sigemptyset(&sigset);
    action.sa_mask = sigset;
    action.sa_flags = SA_SIGINFO | SA_NODEFER | SA_RESETHAND;
    sigaction(SIGSEGV, &action, 0);

    if (memory_checker(pointer)) {
        return { true, *pointer };
    } else {
        return { false, 0xCC };
    }
}

namespace sig {

    class radix {
    public:
        int rdx;
        explicit radix(int r): rdx(r) {}
    } hex(16), dec(10);

    precision default_precision{-1};

    output::output() : rdx(dec.rdx), prec(-1) {}
    char output::to_digit(int n) {
        if (n < 10) {return '0' + n;}
            return 'A' + n - 10;
    }
    const output& output::operator<<(int n) const {
        return *this << (long long)n;
    }
    const output& output::operator<<(long long n) const {
        char buffer[32];
        int size = 0;
        bool neg_flag = false;
        if (n == 0) {
            size = 1;
            buffer[31] = '0';
        } else if(n == LLONG_MIN) {
            if (rdx == 10) {
                write(2, "FFFFFFFFFFFFFFFF", 16);
            } else {
                write(2, "-9223372036854775808", 20);
            }
            return *this;
        } else {
            if (n < 0 && rdx == 10) {
                n = -n;
                neg_flag = true;
            }
            while (n) {
                buffer[31 - size] = to_digit(((n%rdx) + rdx) % rdx);
                if (rdx == 16) {
                    n = ((unsigned long long)n) >> 4;
                } else {
                    n /= rdx;
                }
                size += 1;
            }
        }
        if (prec != -1) {
            while (size < prec) {
                buffer[31 - size] = '0';
                size += 1;
            }
        }
        if (neg_flag) {
            buffer[31 - size] = '-';
            size += 1;
        }
        write(2, buffer + 32 - size, size);
        return *this;
    }
    const output& output::operator<<(const char* st) const {
        for (int i = 0; true; i++) {
            if (st[i] == 0) {
                write(2, st, i);
                return *this;
            }
        }
    }
    const output& output::operator<<(const char c) const {
        write(2, &c, 1);
        return *this;
    }
    const output& output::operator<<(const u_int8_t* pointer) const {
        auto mem = get_mem(pointer);
        if (mem.valid) {
            return *this << mem.data;
        } else {
            return *this << "**";
        }
    }
    const output& output::operator<<(const radix& r) {
        rdx = r.rdx;
        return *this;
    }
    const output& output::operator<<(const precision& p) {
        prec = p.p;
    }
    output cout;
}