#include "msp.h"
#include "msp430f5529.h"

void Timer0_Init()
{

  TA0CTL |= TASSEL__SMCLK + ID__8;
  TA0EX0 |= TAIDEX_7;
  TA0CTL |= TACLR;
  TA0CCR0 = 391;
  TA0CCTL4 |= OUTMOD_6;
  TA0CCTL4 &= ~CAP;
  TA0CCR4 = 100;
  TA0CCTL3 |= OUTMOD_6;
  TA0CCTL3 &= ~CAP;
  TA0CCR3 = 380;
  TA0CTL |= MC__UP;
}

void Timer1_Init()
{

  TA1CTL |= TASSEL__SMCLK + ID__8; //25M  8��Ƶ
  TA1EX0 = TAIDEX_7;               //8��Ƶ
  TA1CTL |= TACLR;                 //��ռ�����   ��ʱ����Ƶ����λ
  TA1CTL |= TAIE;                  //�򿪶�ʱ���ж�
  TA1CCTL0 |= CCIE;                //��ͨ��0�ж�
  TA1CCR0 = 39060 / 2;             //Ƶ�ʴ��Ϊ20 hz Ϊ�˵͹���
  TA1CTL |= MC__UP;                //��������ģʽ ������CCR0
}

void SetVCoreUp(unsigned int level) //���ú��Ĺ�����ѹ
{
  PMMCTL0_H = 0xa5;                 // open pmm regisgers for write access
  SVSMHCTL = SVSHE + SVSHRVL0 + level + SVMHE + SVSMHRRL0 + level;  // set SVS/SVM high side new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 + level;  // Set SVM low side to new level
  while (PMMIFG & SVSMLDLYIFG == 0);  // wait till SVM is settled
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);  // Clear already set flags
  PMMCTL0_L = PMMCOREV0 + level;  // Set V CORE to new level
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);  // wait till new level reached
  SVSMLCTL = SVSLE + SVSLRVL0 + level + SVMLE + SVSMLRRL0 + level;  // Set SVS/SVM low side to new level
  PMMCTL0_H = 0x00;  // lock PMM registers for write access
}

void UCS_Init()
{

  P5SEL |= (BIT4 | BIT5); //XT1��������
  UCSCTL6 |= XCAP_3;      //12pf ����
  UCSCTL6 &= ~XT1OFF;     //��XT1
  SetVCoreUp(2);           //���ĵ�ѹ1.75v  ��Ȼ��Ƶ��ѹ����
  __bis_SR_register(SCG0); //ֹͣMCLK����
  UCSCTL0 = 0;             //FLL�п����Զ��޸�
  UCSCTL1 |= DCORSEL_6;    //DCOƵ�ʷ�Χ
  UCSCTL2 |= FLLD_1 | 380; //DCOCLK = (380+1)*(1+1)*32.768K = 24.97M
  __bic_SR_register(SCG0); //���MCLK��ʼ����
  __delay_cycles(782000);  //�ȴ�MCLK��������
  while (SFRIFG1 & OFIFG)  //�ڿ����Ĺ��������׳��ִ��� �����ᵼ��DCOʱ��Դ��ѡ��XT1 ����������Ҫһֱ�����������־
  {
    SFRIFG1 &= ~OFIFG;
    UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG); //��������־
  }
  UCSCTL4 &= ~(SELM_7 + SELS_7);          //������ռĴ���  ��ΪĬ��ACLKΪXT1 SMCLKΪDCODIV MCLKΪDCODIV   Ĭ��1.04M hz
  UCSCTL4 |= SELM__DCOCLK + SELS__DCOCLK; //SCLK MCLK����ʱ��ԴΪDCO  ԼΪ25M
}

void GPIO_Init()
{
  P1DIR |= BIT0;
  P1OUT &= ~BIT0;
  P4DIR |= BIT7;
  P4OUT &= ~BIT7;
  P6DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4);
  P6OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4);
  //-----��ϻ�е�����������ڲ���������-----
  P1REN |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6); //����P1.1�ڲ�����������
  P1OUT |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6); //����������Ϊ����
  //-----����P1.1�жϲ���-----
  P1DIR &= ~(BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6); // P1.1��Ϊ����(��ʡ��)
  P1IES |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6);  // P1.1��Ϊ�½����ж�
  P1IE |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6);   // ����P1.1�ж�
}
