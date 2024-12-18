#include "CP210xRuntimeDLL.h"
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <atlstr.h>

using namespace std;

int main(int argc, char* argv[])
{   
    /*
        arg 1: COM port to use (Example: COM9)
        arg 2: Mode to put the TI chip into (Examples: uartload, 4w)
    
        // Assumptions for this code to work is that
        Reset is on GPIO 2
        SOP2 is on GPIO 1
    */

    printf("Putting device on port %s into %s mode\n", argv[1], argv[2]);

    char buffer[8];
    sprintf(buffer, "%s", (char*) argv[1]);

    CString comstring;
    comstring = buffer;

    printf("comstring: %s", comstring);

    // Open up the COM port
    HANDLE h_dev = CreateFile(comstring, 
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                0,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                0);



    if(h_dev != INVALID_HANDLE_VALUE)
    {
        BYTE part_num;
        
        string mfg_part_num;
        string mfg_prod_str;
        string mfg_ser_num;
        uint32_t num_dev;

        printf("Product String: %s\n", mfg_prod_str.c_str());
        printf("Serial String: %s\n", mfg_ser_num.c_str());

        if (CP210xRT_GetPartNumber(h_dev, &part_num) == CP210x_SUCCESS)
        {
            printf("Part number: %d\n", part_num);
            switch(part_num)
            {
                case CP210x_PARTNUM_CP2102N_QFN28:
                case CP210x_PARTNUM_CP2102N_QFN20:
                case CP210x_PARTNUM_CP2102N_QFN24:
                    printf("MFG Part NUM: CP2102N\n");
                    break;
                case CP210x_CP2101_VERSION:
                    printf("MFG Part Num: CP2101\n");
                    break;
                case CP210x_CP2102_VERSION:
                    printf("MFG Part Num: CP2102\n");
                    break;
                case CP210x_CP2103_VERSION:
                    printf("MFG Part Num: CP2103\n");
                    break;
                default:
                    printf("MFG Part Num: Unknown\n");
                    break;
            }

            CP210x_PRODUCT_STRING prod_str;
            uint8_t prod_str_len;

            CP210x_SERIAL_STRING_S ser_str;
            uint8_t ser_str_len;

            if(CP210xRT_GetDeviceProductString(h_dev, prod_str, &prod_str_len) == CP210x_SUCCESS)
            {
                mfg_prod_str = prod_str;
            }

            if(CP210xRT_GetDeviceProductString(h_dev, ser_str, &ser_str_len) == CP210x_SUCCESS)
            {
                mfg_ser_num = ser_str;
            }
        }

        WORD mask = 0x000C;
        WORD latch = 0x0008;
        ;
        if(strcmp("uartload", argv[2]) == 0)
        {
            printf("Reseting device into UART load mode\n");

            // First step is to drive to reset and then put the SOP2 high
            mask = 0x000C;
            latch = 0x0004;
            
            CP210x_STATUS status = CP210xRT_WriteLatch(h_dev, mask, latch);
            if(status != CP210x_SUCCESS)
            {
                printf("Failed to write latch (SOP 2 high and reset low) on CP210x device\n");
                printf("Device in unknown state\n");
                printf("Exiting.\n");
                CloseHandle(h_dev);
                exit(1);
            }

            Sleep(250);

            // Next step is to let go of the reset
            mask = 0x0008;
            latch = 0x0008;

            status = CP210xRT_WriteLatch(h_dev, mask, latch);
            if(status != CP210x_SUCCESS)
            {
                printf("Failed to write latch (reset high leaving SOP 2 high) on CP210x device\n");
                printf("Device in unknown state\n");
                printf("Exiting \n");
                CloseHandle(h_dev);
                exit(1);
            }

        }
        else if(strcmp("4w_funct", argv[2]) == 0)
        {
            printf("Resetting device into 4W functional mode\n");

            // First step is to drive to reset (active low) and then put the SOP2 low
            mask = 0x000C;
            latch = 0x0000;
            
            CP210x_STATUS status = CP210xRT_WriteLatch(h_dev, mask, latch);
            if(status != CP210x_SUCCESS)
            {
                printf("Failed to write latch (SOP 2 low and reset low) on CP210x device\n");
                printf("Device in unknown state\n");
                printf("Exiting.\n");
                CloseHandle(h_dev);
                exit(1);
            }

            Sleep(250);

            // Next step is to let go of the reset
            mask = 0x0008;
            latch = 0x0008;

            status = CP210xRT_WriteLatch(h_dev, mask, latch);
            if(status != CP210x_SUCCESS)
            {
                printf("Failed to write latch (reset high leaving SOP 2 low) on CP210x device\n");
                printf("Device in unknown state\n");
                printf("Exiting \n");
                CloseHandle(h_dev);
                exit(1);
            }
        }
        else
            printf("Unknown requested mode\n");

        printf("Successfully put the device into %s mode\n", argv[2]);

        CloseHandle(h_dev);
    }
    else
    {
        printf("Invalid device handle - Not able to open COM port file\n");
    }

    return 0;
    }