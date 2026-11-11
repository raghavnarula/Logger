#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>


class LogGenerator{
    public:
        explicit LogGenerator(std::string _fileName);
        void writeLog();
        ~LogGenerator();

    private:
        std::string _fileName = "app.log";
        int _stringLength = 10;
        int _fd;
        std::ofstream logFile_;
        std::string randomStringGenerator(size_t _stringLength);
};