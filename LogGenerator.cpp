#include "LogGenerator.h"
#include <random>
#include <algorithm>
#include <ranges>


LogGenerator::LogGenerator(std::string fileName)
{
    _fileName = fileName;
    logFile_.open(_fileName, std::ios::app);
}

void LogGenerator::writeLog() {
    std::string randomString = randomStringGenerator(10000000);
    logFile_ << randomString << std::endl;
    return;
}

std::string LogGenerator::randomStringGenerator(int _stringLength) {
    static constexpr std::string_view characters = 
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

    std::string randomStr;
    randomStr.resize(_stringLength);

    // Fill the string using a modern C++ view / generator approach
    std::ranges::generate(randomStr, [&]() {
        return characters[distribution(generator)];
    });

    return randomStr;
}