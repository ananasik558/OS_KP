#pragma once
#include <iostream>
#include <cstring>
#include <cmath>


struct BuddyBlock {
    size_t blockSize;
    bool isFree;
};

class BuddyAllocator {
private:
    BuddyBlock *head = nullptr;
    BuddyBlock *tail = nullptr;
    void *data = nullptr;

    bool expanded = false;

    BuddyBlock* next(BuddyBlock *block) {
        return (BuddyBlock *)((char *)(block) + block->blockSize);
    }

    BuddyBlock* split(BuddyBlock *block, size_t size) {
        if (block != nullptr && size != 0) {
            while (size < block->blockSize) {
                size_t sz = block->blockSize >> 1;
                block->blockSize = sz;
                block = this->next(block);
                block->blockSize = sz;
                block->isFree = true;
            }
            if (size <= block->blockSize){
                return block;
            }
        }
        return nullptr;
    }

    BuddyBlock* find_best(size_t size) {
        if (size == 0){
            return nullptr;
        }

        BuddyBlock *bestBlock = nullptr;
        BuddyBlock *block = this->head;
        BuddyBlock *buddy = this->next(block);

        if (buddy == this->tail && block->isFree) {
            return this->split(block, size);
        }

        while (block < this->tail && buddy < this->tail) {
            if (block->isFree && buddy->isFree && block->blockSize == buddy->blockSize) {
                block->blockSize <<= 1;
                if (size <= block->blockSize && (bestBlock == nullptr || block->blockSize <= bestBlock->blockSize)) {
                    bestBlock = block;
                }

                block = this->next(buddy);
                if (block < this->tail)  {
                    buddy = this->next(block);
                }
                continue;
            }

            if (block->isFree && size <= block->blockSize && (bestBlock == nullptr || block->blockSize <= bestBlock->blockSize))  {
                bestBlock = block;
            }
            if (buddy->isFree && size <= buddy->blockSize && (bestBlock == nullptr || buddy->blockSize < bestBlock->blockSize)) {
                bestBlock = buddy;
            }

            if (block->blockSize <= buddy->blockSize) {
                block = this->next(buddy);
                if (block < this->tail) {
                    buddy = this->next(block);
                }
            }
            else {
                block = buddy;
                buddy = this->next(buddy);
            }
        }

        if (bestBlock != nullptr) {
            return this->split(bestBlock, size);
        }

        return nullptr;
    }

    void coalescence() {
        while (true) {
            BuddyBlock *block = this->head;
            BuddyBlock *buddy = this->next(block);

            bool no_coalescence = true;
            while (block < this->tail && buddy < this->tail) {
                if (block->isFree && buddy->isFree && block->blockSize == buddy->blockSize) {
                    block->blockSize <<= 1;
                    block = this->next(block);
                    if (block < this->tail) {
                        buddy = this->next(block);
                        no_coalescence = false;
                    }
                }
                else if (block->blockSize < buddy->blockSize) {
                    block = buddy;
                    buddy = this->next(buddy);
                }
                else {
                    block = this->next(buddy);
                    if (block < this->tail) {
                        buddy = this->next(block);
                    }
                }
            }

            if (no_coalescence) {
                return;
            }
        }
    }

    static size_t align(size_t size) {
        int i = 0;
        while (pow(2, i) < size){
            i++;
        }
        return (size_t) pow(2, i);
    }

public:

    BuddyAllocator(size_t size) {
        this->expand(size);
    }

    ~BuddyAllocator() {
        this->head = nullptr;
        this->tail = nullptr;
        std::free(this->data);
    }

    void expand(size_t size) {

        if (this->head) {
            size += this->head->blockSize;
        }
        size = align(size);

        this->data = std::realloc(this->data, size);

        this->head = (BuddyBlock *)(data);
        this->head->blockSize = size;
        this->head->isFree = true;

        this->tail = next(head);
    }


    void *allocate(size_t size) {
        if (size == 0) {
            std::cerr << "Size must be bigger than 0\n";
            return nullptr;
        }

        size_t new_size = align(size + sizeof(BuddyBlock));

        BuddyBlock *found = this->find_best(new_size);
        if (found == nullptr) {
            this->coalescence();
            found = this->find_best(new_size);
        }

        if (found != nullptr) {
            found->isFree = false;
            this->expanded = false;
            return (void *)((char *)(found) + sizeof(BuddyBlock));
        }

        if (this->expanded) {
            this->expanded = false;
            return nullptr;
        }
        this->expanded = true;
        this->expand(size);
        return this->allocate(size);
    }


    void deallocate(void *ptr) {
        if (ptr == nullptr) {
            std::cerr << "Bad ptr";
            return;
        }

        BuddyBlock *block = (BuddyBlock *)((char *)(ptr) - sizeof(BuddyBlock));
        block->isFree = true;

        this->coalescence();
    }
};
