#include "LogGenerator.h"
#include <random>
#include <algorithm>
#include <ranges>
#include <errno.h>
#include "logger.h"


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

void LogGenerator::writeLog(size_t numberOfBlocks) {

    size_t blocksWritten = 0;

    while (blocksWritten < numberOfBlocks){
        Block randomBlock = Block();

        size_t totalBytesWritten = 0;
        
        while (totalBytesWritten < randomBlock._blockSize) {
            
            long int bytesRemaining = randomBlock._blockSize - totalBytesWritten;

            ssize_t bytesWritten = write(_fd, randomBlock._block.c_str() + totalBytesWritten, bytesRemaining);
            
            if (bytesWritten == -1) {  
                std::cout << "Failed to Write" << std::endl;
                break; // Exit loop on a fatal write error
            }
            
            totalBytesWritten += bytesWritten;
        } 
        blocksWritten += 1;
        LOG_INFO("Block Written: " + std::to_string(blocksWritten));
    }

    std::cout << "Finished! Successfully writing all Blocks";
    return;
}
