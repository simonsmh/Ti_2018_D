#include "msp430f5529.h"
#include "delay.h"
void delay_ms(unsigned int ms)
{
  unsigned int temp;
  for (temp = 0; temp < ms; temp++)
    __delay_cycles(25000); //ÑÓÊ±1ms
}
void delay_us(unsigned long us)
{
  unsigned long temp;
  for (temp = 0; temp < us; temp++)
    __delay_cycles(25); //1us
}
