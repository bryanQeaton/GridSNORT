#include <chrono>
#include <iostream>

#include "perft.h"
#include "snort.h"
#include "solver.h"

int main() {
    auto pos=Snort::Position(7,5);
    std::cout<<pos<<"\n";

    auto t0=std::chrono::high_resolution_clock::now();

    std::cout<<solver(pos)<<"\n";
    auto t1=std::chrono::high_resolution_clock::now();
    int time=std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
    std::cout<<time<<"\n";

    std::cout<<nodes<<"\n";






    return 0;
}
