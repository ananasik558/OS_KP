#pragma once


#include <iostream>
#include <unordered_map>
#include <vector>
#include <cmath>

class Allocator_pow_of_2{
    public:
    struct Header {
        Header * next;
     };
    std::unordered_map<size_t, Header *> free_blocks_lists;
    std::vector<size_t> powsOf2 = {16, 32, 64, 128, 256, 512, 1024};
    void * start_point;
    void * data;
    void * end_point;
    static size_t align(size_t size) {
        int i = 0;
        while (pow(2, i) < size || pow(2, i) < 16){
            i++;
        }
        return (size_t) pow(2, i);
    }
    Allocator_pow_of_2(std::vector<size_t>& blocks_amount);
    ~Allocator_pow_of_2();
    void* allocate(size_t bytes_amount);
    void deallocate(void *ptr);

};


Allocator_pow_of_2::Allocator_pow_of_2(std::vector<size_t>& blocks_amount){
    size_t bytes_sum = 0;
    for(size_t i = 0; i < blocks_amount.size(); i++){
        bytes_sum += blocks_amount[i] * (sizeof(Header *) + powsOf2[i]);
    }
    
    data = malloc(bytes_sum + (sizeof(Header *) * blocks_amount.size()));
    start_point = data;
    end_point = (void *)((char *) start_point + bytes_sum + (sizeof(Header *) * blocks_amount.size()));
    void * point_now = start_point;
    for(size_t i = 0; i < blocks_amount.size(); i++){
        Header * head = (Header *) point_now;
        free_blocks_lists[powsOf2[i]] = head;
        point_now = (void *)((char *)point_now + sizeof(Header *));
        for(size_t j = 0; j < blocks_amount[i]; j++){
            head->next = (Header *) point_now;
            head = head->next;
            point_now = (void *)((char *)point_now + powsOf2[i] + sizeof(Header *));
        }
        head->next = nullptr;
    }
}


void * Allocator_pow_of_2::allocate(size_t bytes_amount){
    if(bytes_amount == 0){
        std::cerr << "Size must be bigger than 0\n";
        return nullptr;
    }
    size_t size = align(bytes_amount);
    Header * head = free_blocks_lists[size];
    if(head->next == nullptr){
        std::cerr << "Zero free element\n";
        return nullptr;
    }
    Header * alloc_elem = head->next;
    head->next = head->next->next;
    alloc_elem->next = head;
    return (void *) ((char *)alloc_elem + sizeof(Header *));
}


void Allocator_pow_of_2::deallocate(void * ptr){
    if (ptr < start_point or ptr > end_point){
        std::cerr << "Uncorrect ptr\n";
        return;
    }
    Header * ptr_header = (Header *)((char *)ptr - sizeof(Header *));
    for(size_t i = 0; i < free_blocks_lists.size(); i++){
        if (free_blocks_lists[powsOf2[i]] == ptr_header->next){
            ptr_header->next = free_blocks_lists[powsOf2[i]]->next;
            free_blocks_lists[powsOf2[i]]->next = ptr_header;
            return;
        }
    }
    std::cerr << "Uncorrect ptr\n";
}


Allocator_pow_of_2::~Allocator_pow_of_2(){
    free(data);
}