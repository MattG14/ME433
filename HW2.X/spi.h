
#ifndef _SPI_H    /* Guard against multiple inclusion */
#define _SPI_H

#include <xc.h>                     // processor SFR definitions
#include <sys/attribs.h>            // __ISR macro

void initSPI();
unsigned char spi_io(unsigned char o);
#endif