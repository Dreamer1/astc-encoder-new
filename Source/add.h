#ifndef ADD_H
#define ADD_H
#define ADD_PUBLIC extern "C" __attribute__ ((visibility ("default")))


namespace adder {
    ADD_PUBLIC int add(int a, int b);
}

#endif
