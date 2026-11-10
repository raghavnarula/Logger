#include "LogGenerator.h"

int main()
{
    LogGenerator logger("/mnt/usb/app.log");
    logger.writeLog();

    return 0;
}