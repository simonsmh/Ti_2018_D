#ifndef _TIMER_H
#define _TIMER_H

#define LED_RESET   P6OUT&=~(BIT0+BIT1+BIT2+BIT3+BIT4)
#define LED_ON_1    LED_RESET;P6OUT|=(BIT0)
#define LED_ON_2    LED_RESET;P6OUT|=(BIT0+BIT1)
#define LED_ON_3    LED_RESET;P6OUT|=(BIT0+BIT1+BIT2)
#define LED_ON_4    LED_RESET;P6OUT|=(BIT0+BIT1+BIT2+BIT3)
#define LED_ON_5    LED_RESET;P6OUT|=(BIT0+BIT1+BIT2+BIT3+BIT4)
#define LED_1       LED_RESET;P6OUT|=(BIT0)
#define LED_2       LED_RESET;P6OUT|=(BIT1)
#define LED_3       LED_RESET;P6OUT|=(BIT2)
#define LED_4       LED_RESET;P6OUT|=(BIT3)
#define LED_5       LED_RESET;P6OUT|=(BIT4)
#define LED_FLOW    LED_1;delay_ms(30);LED_2;delay_ms(30);LED_3;delay_ms(30);LED_4;delay_ms(30);LED_5;delay_ms(30);LED_RESET
#define LED_REFLOW  LED_5;delay_ms(30);LED_4;delay_ms(30);LED_3;delay_ms(30);LED_2;delay_ms(30);LED_1;delay_ms(30);LED_RESET


void Timer1_Init();
void Timer0_Init();
void UCS_Init();  //³õÊ¼»¯SMCLK  MCLK Îª 25M hz
void GPIO_Init();


#endif
