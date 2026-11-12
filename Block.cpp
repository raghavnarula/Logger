#include <iostream>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include "Block.h"
#include <random>
#include <algorithm>
#include <ranges>

// Generates a Block of size 4MB
Block::Block(){
    static constexpr std::string_view characters = 
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

    _block.resize(_blockSize); // 4 MB block size

    // Fill the string using a modern C++ view / generator approach
    std::ranges::generate(_block, [&]() {
        return characters[distribution(generator)];
    });

}