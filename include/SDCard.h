#include "sd_card.h"
#include "ff.h"
#include <stdio.h>
#include <string>

class SD_Card_t
{
public:
    FRESULT fr;
    FATFS fs;
    FIL fil;

    int ret;
    char buf[100];
    char filename[9] = "data.txt";
    bool connected = false;
    unsigned long pos = 0;

    SD_Card_t();
    bool begin();
    void write(char *data);
};