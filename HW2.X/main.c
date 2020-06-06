#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include"spi.h"
#include<math.h>

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 00000001 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations



#define ONESEC 24000000 

// write DAC to SPI
void write_DAC(unsigned char channel, unsigned int n){
    
    // divide message into two halves, P1 and P2
    unsigned char P1, P2;
    
    // set and configure P1
    P1 = channel << 7;
    P1 |= (0b111 << 4);
    P1 |= (n >> 8) & 0b1111;
    
    // set P2
    P2 = (unsigned char) n;
    
    // set CS to zero
    LATAbits.LATA0 = 0;
    
    // call spi function
    spi_io(P1);
    spi_io(P2);
    
    // set CS to 1
    LATAbits.LATA0 = 1;
   
}

// pause function for 1/256 of a second
void pause(){
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < ONE_SEC/256);
}

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    initSPI();
    
    __builtin_enable_interrupts();
    
    // set periods of waves using arrays
    unsigned int triangleWave[256];
    unsigned int sineWave[256];
    
    // Triangle Wave
    int t = 0;
    while (t < 256){
        // wave rises for first half
        if (t < 128){
            triangle[t] = 32*t;
        }
        // avoids wave dropping to zero at peak
        else if (t == 128){  
            triangleWave[t] = 4095;
        }
        // wave falls for second half
        else{
            triangleWave[t] = 8192-(32*t);
        }
        t++;
    }
    
    // Sine Wave
    ts = 0;
    const float pi = 3.14;
    while (ts < 256){
        // define sine function
        sineWave[ts] = 2048*sin(2.0 * pi * ts / 128) + 2048;
        // avoids wave dropping to zero at peak
        if (sineWave[ts] == 4096) sine[ts] = 4095;
        ts++;
    }
    return -1;
}