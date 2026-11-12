#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include "Block.h"


class LogGenerator{
    public:
        explicit LogGenerator(std::string _fileName);
        void writeLog(size_t numberOfBlocks);
        ~LogGenerator();

    private:
        std::string _fileName = "app.log";
        size_t numberOfBlocks = 10;
        int _fd;
        std::ofstream logFile_;
        Block blockGenerator();
};