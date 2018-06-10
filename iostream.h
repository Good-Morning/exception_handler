#pragma once

#include <stdlib.h>
#include <unistd.h>

namespace sig {

    class radix;
    struct precision {
        int p;
        explicit precision(int a):p(a) {}
    };
    extern precision default_precision;
    extern radix hex, dec;

    class output {
        int rdx;
        int prec;

        static char to_digit(int n);
    public:
        output();
        const output& operator<<(int n) const;
        const output& operator<<(long long n) const;
        const output& operator<<(const char c) const;
        const output& operator<<(const char* st) const;
        const output& operator<<(const u_int8_t* pointer) const;

        const output& operator<<(const precision& p);
        const output& operator<<(const radix& r);
    };
    extern output cout;
}