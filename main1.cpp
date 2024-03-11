#include "allocator_pow_of_2.h"
#include <vector>
#include <chrono>

int main(){

    std::vector<size_t> k = {16, 16, 32, 64, 128, 256, 512};
    auto t1 = std::chrono::high_resolution_clock::now();
    Allocator_pow_of_2 alloc(k);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    std::cout << "Time to create Allocator_pow_of_2 : " << duration << '\n';
    std::vector<void*> v1(1000);
    auto t3 = std::chrono::high_resolution_clock::now();
    for (size_t i = 1; i < v1.size(); i++) {
        v1[i] = alloc.allocate(i);
    }
    auto t4 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
    std::cout << "Time to allocate Allocator_pow_of_2 : " << duration1 << '\n';
    auto t5 = std::chrono::high_resolution_clock::now();
    for(size_t i = 1; i < v1.size(); i++){
        alloc.deallocate(v1[i]);
    }
     auto t6 = std::chrono::high_resolution_clock::now();
     auto duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count();
     std::cout << "Time to deallocate Allocator_pow_of_2 : " << duration2 << '\n';
}