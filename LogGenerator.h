#pragma once
#include <iostream>
#include <string>
#include <fstream>

class LogGenerator{
    public:
        explicit LogGenerator(std::string _fileName);
        void writeLog();
    private:
        std::string _fileName = "app.log";
        int _stringLength = 10;
        std::ofstream logFile_;
        std::string randomStringGenerator(int _stringLength);
};