
#include "Bubby.h"
#include <vector>
#include <chrono>

int main(){
    
     auto t1 = std::chrono::steady_clock::now();
     BuddyAllocator allocator = BuddyAllocator(4096);
     auto t2 = std::chrono::steady_clock::now();
     auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
     std::cout << "Time to create BuddyAllocator : " << duration << '\n';
     std::vector<void*> v(1000);
    
     auto t3 = std::chrono::high_resolution_clock::now();
    
     for (int i = 1; i < v.size(); i++) {
         v[i] = allocator.allocate(i);
     }
     auto t4 = std::chrono::high_resolution_clock::now();
     auto duration1 = std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
     std::cout << "Time to allocate BuddyAllocator : " << duration1 << '\n';
     auto t5 = std::chrono::high_resolution_clock::now();
     for (auto & i : v) {
         allocator.deallocate(i);
     }
     auto t6 = std::chrono::high_resolution_clock::now();
     auto duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count();
     std::cout << "Time to deallocate BuddyAllocator : " << duration2 << '\n';

}