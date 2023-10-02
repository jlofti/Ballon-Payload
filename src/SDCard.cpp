#include "../include/SDCard.h"

SD_Card_t::SD_Card_t()
{
}
bool SD_Card_t::begin()
{
    // Initialize SD card
    if (!sd_init_driver())
    {
        printf("ERROR: Could not initialize SD card\r\n");
        while (true)
            ;
    }

    this->connected = true;
    return true;
}

void SD_Card_t::write(std::string *data)
{ // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK)
    {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        while (true)
            ;
    }

    // Open file for writing ()
    fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK)
    {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while (true)
            ;
    }

    // Write something to file
    ret = f_printf(&fil, "This is a test\r\n");
    if (ret < 0)
    {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        while (true)
            ;
    }
    ret = f_printf(&fil, "of writing to an SD card.\r\n");

    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK)
    {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true)
            ;
    }
}