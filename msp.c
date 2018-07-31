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

  TA1CTL |= TASSEL__SMCLK + ID__8; //25M  8分频
  TA1EX0 = TAIDEX_7;               //8分频
  TA1CTL |= TACLR;                 //清空计数器   定时器分频器复位
  TA1CTL |= TAIE;                  //打开定时器中断
  TA1CCTL0 |= CCIE;                //打开通道0中断
  TA1CCR0 = 39060 / 2;             //频率大概为20 hz 为了低功耗
  TA1CTL |= MC__UP;                //上升计数模式 计数到CCR0
}

void SetVCoreUp(unsigned int level) //设置核心工作电压
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

  P5SEL |= (BIT4 | BIT5); //XT1晶振引脚
  UCSCTL6 |= XCAP_3;      //12pf 电容
  UCSCTL6 &= ~XT1OFF;     //打开XT1
  SetVCoreUp(2);           //核心电压1.75v  不然高频电压不够
  __bis_SR_register(SCG0); //停止MCLK工作
  UCSCTL0 = 0;             //FLL中可以自动修改
  UCSCTL1 |= DCORSEL_6;    //DCO频率范围
  UCSCTL2 |= FLLD_1 | 380; //DCOCLK = (380+1)*(1+1)*32.768K = 24.97M
  __bic_SR_register(SCG0); //清除MCLK开始工作
  __delay_cycles(782000);  //等待MCLK正常工作
  while (SFRIFG1 & OFIFG)  //在开启的过程中容易出现错误 这样会导致DCO时钟源不选择XT1 所以我们需要一直软件清除错误标志
  {
    SFRIFG1 &= ~OFIFG;
    UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG); //清除错误标志
  }
  UCSCTL4 &= ~(SELM_7 + SELS_7);          //首先清空寄存器  因为默认ACLK为XT1 SMCLK为DCODIV MCLK为DCODIV   默认1.04M hz
  UCSCTL4 |= SELM__DCOCLK + SELS__DCOCLK; //SCLK MCLK设置时钟源为DCO  约为25M
}

void GPIO_Init()
{
  P1DIR |= BIT0;
  P1OUT &= ~BIT0;
  P4DIR |= BIT7;
  P4OUT &= ~BIT7;
  P6DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4);
  P6OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4);
  //-----配合机械按键，启用内部上拉电阻-----
  P1REN |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6); //启用P1.1内部上下拉电阻
  P1OUT |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6); //将电阻设置为上拉
  //-----配置P1.1中断参数-----
  P1DIR &= ~(BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6); // P1.1设为输入(可省略)
  P1IES |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6);  // P1.1设为下降沿中断
  P1IE |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6);   // 允许P1.1中断
}
