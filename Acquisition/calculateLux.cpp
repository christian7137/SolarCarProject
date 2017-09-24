#include <stdio.h>
#include <math.h>

float calculateLux(unsigned int, unsigned int);

int main()
{
    printf("%f", calculateLux(2, 2));
    return 0;
}

float calculateLux(unsigned int ch0, unsigned int ch1)
/*  Calculates the approximate illuminance (lux) given the raw channel values of the TSL2561T. 
    
    Arguments:  unsigned int ch0 - raw channel value from channel 0 of TSL2561T
                unsigned int ch1 - raw channel value from channel 1 of TSL2561T
    
    Return: unsigned float - the approximate illuminance (lux)
*/
{
    float channelRatio = ch1/ ch0;  // ratio of ch1: ch0 used to determine parameters for lux calculations
    float coefficient0 = 0;
    float coefficient1 = 0;
    float exponent0 = 0.0;
    float exponent1 = 1.0;
    
    if ((0 < channelRatio) && (channelRatio <= 0.50))
    {
        coefficient0 = 0.0304;
        coefficient1 = 0.062;
        exponent0 = -0.4;
        exponent1 = 1.4;
    }   
    else if ((0.50 < channelRatio) && (channelRatio <= 0.61))
    {
        coefficient0 = 0.0224;
        coefficient1 = 0.031;
    }   
    else if ((0.61 < channelRatio) && (channelRatio <= 0.80))
    {
        coefficient0 = 0.0128;
        coefficient1 = 0.0153;
    }   
    else if ((0.80 < channelRatio) && (channelRatio <= 1.30))
    {
        coefficient0 = 0.00146;
        coefficient1 = 0.00112;
    }   
    printf("%f, %f, %f, %f, %f\n", channelRatio, coefficient0, coefficient1, exponent0, exponent1);
    float lux = coefficient0 * ch0 - coefficient1 * exp(exponent0 * log(ch0)) * exp(exponent1 * log(ch1));
    return lux;
}
