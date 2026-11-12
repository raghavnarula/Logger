#pragma once
#include <iostream>

// Generates a Block of size 4MB
class Block{
    public:
        explicit Block();
        size_t _blockSize = 4 * 1024 * 1024;
        std::string _block; 
    private:
};