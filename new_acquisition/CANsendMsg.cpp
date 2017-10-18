#include "mbed.h"
#include "CAN.h"
#include "TSL2561.h"
#include <stdio.h>
#include <stdlib.h>
 
#define LM75_REG_TEMP (0x00) // Temperature Register
#define LM75_REG_CONF (0x01) // Configuration Register
#define LM75_ADDR     (0x90) // LM75 address

DigitalOut messageSignalLED(LED1);
DigitalOut LedD1(PD_15);
I2C i2c(I2C_SDA, I2C_SCL);
InterruptIn sendButton(BUTTON1);
TSL2561 test(I2C_SDA, I2C_SCL);
Serial pc(SERIAL_TX, SERIAL_RX);

CAN testCANbus(PD_0, PD_1);

int CANaddress = 1337;
void sendTestMessage(); 
uint16_t data_read;
char data[8];

void reverse(char *s)  {
char *j;
int c;
 
  j = s + strlen(s) - 1;
  while(s < j) {
    c = *s;
    *s++ = *j;
    *j-- = c;
  }
}

 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0) 
         n = -n;         
     i = 0;
     do {       
         s[i++] = n % 10 + '0';   
     } while ((n /= 10) > 0);    
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void setup()
{
    sendButton.rise(&sendTestMessage);      //butting a rising interrupt on the blue button, so when I press it stuff happens
    testCANbus.frequency(100000);
    test.enable();
    //data[0] = 3;
}

int main() 
{
    setup();
    while(1)
    {
        data_read = test.getLuminosity(0);
        //pc.printf("%d",data_read);
        wait(2);
        ;
    }
    
}

void sendTestMessage()
{
 messageSignalLED = 1; //just using this so I can see when it's sending
 wait(.5);
 char data[] = "Hello";
 LedD1 = 0;
 wait(0.1);
 LedD1 = 1;
 //itoa(data_read,(data+1));
 //data[1] = data_read;
 pc.printf(data);
 CANMessage testMessage(CANaddress, data, sizeof(data));
 testCANbus.write(testMessage);
 messageSignalLED = 0;   
 LedD1 = 0;
 wait(0.1);
 LedD1 = 1;
}
