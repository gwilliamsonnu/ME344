#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <math.h>
#include "imu.h"
#include "ws2812b.h"
#include "i2c.h"
#include "ssd.h"
#include "font.h"



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
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations



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
    LATAbits.LATA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    __builtin_enable_interrupts();

    // SETUPS for i2c, display, IMU, and LEDS
    i2c_master_setup();
    ssd1306_setup();
    imu_setup();
    ws2812b_setup();

    unsigned char WA, RA;
    
    WA = 0b11010110; // Write address
    RA = 0b11010111; // Read address 
    
    signed short vals[7];
    
    
    while (1) { 
        
        imu_read(IMU_OUT_TEMP_L, vals, 7); // read from chip and store all values in array
        
        // Print acceleration readings
        char accel[50];
        sprintf(accel, "A: x=%d; y=%d; z=%d", vals[4], vals[5], vals[6]);
        ssd1306_drawString(0, 0, accel);
        
        // Print gyroscope readings
        char gyro[50];
        sprintf(gyro, "G: x=%d; y=%d; z=%d", vals[1], vals[2], vals[3]);
        ssd1306_drawString(0, 1, gyro);
        
        // Print temperature reading
        char temp[20];
        sprintf(temp, "T: t=%d", vals[0]);
        ssd1306_drawString(0, 2, temp);
        
        /*
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 24000000/20) {}
         */                                      
    }
}





