//
// Created by mahie on 07/04/2024.
//

#ifndef DIAGNOSE_H
#define DIAGNOSE_H

#include <array>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdexcept>

// Code of Singleton implementation found here
// https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern
//
// Changed to fit Requirements
class Diagnose {
public:
    static Diagnose& get() {
        static Diagnose dg; // Guaranteed to be destroyed.
        return dg;          // Instantiated on first use.
    }

    static char* callback() { return get().data; }

    static void unwrap() {
        std::cout << get().data << std::endl;
        if (strlen(get().data) == 0) {
            return;
        }
        throw std::runtime_error(get().data);
    }

    // static void clear() { get().data[0] = 0; }

    //
    // C++ 11
    // =======
    // We can use the better technique of deleting the methods
    // we don't want.
    Diagnose(Diagnose const&) = delete;
    void operator=(Diagnose const&) = delete;

    // Note: Scott Meyers mentions in his Effective Modern
    //       C++ book, that deleted functions should generally
    //       be public as it results in better error messages
    //       due to the compilers behavior to check accessibility
    //       before deleted status

private:
    char data[512];
    Diagnose() { data[0] = 0; }
};

#endif