#include "LogGenerator.h"

int main()
{
    LogGenerator logger("/mnt/usb/app.log");
    logger.writeLog(1000);

    return 0;
}