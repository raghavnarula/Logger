#include "LogGenerator.h"
#include <random>
#include <algorithm>
#include <ranges>
#include <errno.h>


LogGenerator::LogGenerator(std::string fileName)
{
    _fileName = fileName;
    _fd = open(_fileName.c_str(),O_WRONLY | O_CREAT | O_APPEND | O_SYNC,0644);
    if (_fd == -1){
        if (errno == EINTR){
            std::cout << "File Not Found";
        }
        return;
    }

}

LogGenerator::~LogGenerator() {
    if (_fd != -1) {
        close(_fd);
    }
}

void LogGenerator::writeLog() {
    long int bytesSize = 10000000000; // 10GB 
    std::string randomString = randomStringGenerator(bytesSize);
    
    long int totalBytesWritten = 0;
    
    while (totalBytesWritten < static_cast<long int>(randomString.length())) {
        
        long int bytesRemaining = randomString.length() - totalBytesWritten;

        ssize_t bytesWritten = write(_fd, randomString.c_str() + totalBytesWritten, bytesRemaining);
        
        if (bytesWritten == -1) {  
            std::cout << "Failed to Write" << std::endl;
            break; // Exit loop on a fatal write error
        }
        
        totalBytesWritten += bytesWritten;
        std::cout << "Successfully wrote total of " << bytesWritten << " bytes.\n";
    } 
    
    std::cout << "Finished! Successfully wrote total of " << totalBytesWritten << " bytes.\n";
    return;
}


std::string LogGenerator::randomStringGenerator(size_t _stringLength) {
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