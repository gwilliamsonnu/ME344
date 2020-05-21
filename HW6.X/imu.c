#include "imu.h"
#include "i2c.h"

void imu_setup(){
    unsigned char who = 0;
    unsigned char WA, RA;
    
    WA = 0b11010110; // Write address
    RA = 0b11010111; // Read address 
    
    // read from IMU_WHOAMI
    i2c_master_start();
    i2c_master_send(WA);
    i2c_master_send(0x0F);
    i2c_master_restart();
    i2c_master_send(RA);
    who = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    
    if (who != 0b01101001){
        while(1){}
    }

    // init IMU_CTRL1_XL
    i2c_master_start();
    i2c_master_send(WA);
    i2c_master_send(0x10); 
    i2c_master_send(0b10000010);
    i2c_master_stop();
    
    // init IMU_CTRL2_G
    i2c_master_start();
    i2c_master_send(WA);
    i2c_master_send(0x11); 
    i2c_master_send(0b10001000);
    i2c_master_stop();

    // init IMU_CTRL3_C
    i2c_master_start();
    i2c_master_send(WA);
    i2c_master_send(0x12); 
    i2c_master_send(0b00000100);
    i2c_master_stop();

}

void imu_read(unsigned char reg, signed short * data, int len){
    
    unsigned char WA, RA;
    
    WA = 0b11010110; // Write address
    RA = 0b11010111; // Read address
    
    // read multiple from the imu, each data takes 2 reads so you need len*2 chars
    i2c_master_start();
    i2c_master_send(WA);
    i2c_master_send(reg); // start at reg=0x20, which is OUT_TEMP_L
    i2c_master_restart();
    i2c_master_send(RA);
    
    // Read multiple times
    unsigned char reads[len*2];
    int i;
    for (i=0; i<(len*2); i++) {
        reads[i] = i2c_master_recv();
        if (i<((len*2)-1)) {
            i2c_master_ack(0);
        }
        else {
            i2c_master_ack(1);
        }
    }
    
    i2c_master_stop();
    
    // turn the chars into the shorts
    for (i=0; i<(len*2); i+=2) {       
        data[i/2] = reads[i] | ((reads[i+1]) << 8);     
    }
}