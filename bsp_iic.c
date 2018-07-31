#include "msp430f5529.h"
#include "delay.h"
#include "bsp_iic.h"



/*******************************************************************************
函数名称：IIC_Init()
函数功能：IIC初始化
入口参数：无
出口参数：无
函数说明：
*******************************************************************************/
void IIC_Init(void)
{
  P3DIR |= BIT5 + BIT6;
  IIC_SDA_H;
  IIC_SCL_H;
}   
/*******************************************************************************
函数名称：IIC_Start()
函数功能：产生IIC起始信号
入口参数：无
出口参数：无
函数说明：
*******************************************************************************/
void IIC_Start(void)
{
  IIC_SDA_OUT();
  IIC_SDA_H;
  IIC_SCL_H;
  delay_us(4);
  IIC_SDA_L;
  delay_us(4);
  IIC_SCL_L;
}
/*******************************************************************************
函数名称：IIC_Stop()
函数功能：产生IIC停止信号
入口参数：无
出口参数：无
函数说明：
*******************************************************************************/
void IIC_Stop(void)
{
  IIC_SDA_OUT();
  IIC_SDA_L;
  IIC_SCL_L;
  delay_us(4);
  IIC_SCL_H;
  delay_us(4); 
  IIC_SDA_H;
 
}
/*******************************************************************************
函数名称：IIC_Wait_Ack()
函数功能：等待应答信号来
入口参数：无
出口参数：返回1：应答失败，返回0应答成功
函数说明：
*******************************************************************************/
unsigned char IIC_Wait_Ack(void)
{
  unsigned char ucErrTime = 0;
  IIC_SDA_IN();
  IIC_SDA_H;delay_us(1);
  IIC_SCL_H;delay_us(1);
  while((IIC_READ_SDA))
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      IIC_Stop();
      return 1;
    }
  }
  IIC_SCL_L;//	 
  delay_us(1);  
  return 0;  
}
/*******************************************************************************
函数名称：IIC_Ack()
函数功能：产生应答信号
入口参数：无
出口参数:
函数说明：
*******************************************************************************/
void IIC_Ack(void)
{
  IIC_SCL_L;
  IIC_SDA_OUT();
  IIC_SDA_L;
  delay_us(1);
  IIC_SCL_H;
  delay_us(1);
  IIC_SCL_L;
  delay_us(1);
  IIC_SDA_H;
}
/*******************************************************************************
函数名称：IIC_NAck()
函数功能：不产生应答信号
入口参数：无
出口参数:
函数说明：
*******************************************************************************/
void IIC_NAck(void)
{
 IIC_SCL_L;
  IIC_SDA_OUT();
  IIC_SDA_H;
  delay_us(1);
  IIC_SCL_H;
  delay_us(1);
  IIC_SCL_L;
  delay_us(1);
}
/*******************************************************************************
函数名称：IIC_Send_Byte()
函数功能：IIC发送函数
入口参数：txd
出口参数:
函数说明：
*******************************************************************************/

void IIC_Send_Byte(unsigned char txd)
{
  unsigned char t;
  IIC_SDA_OUT();
  IIC_SCL_L;
  for(t = 0; t < 8; t++)
  {
    if((txd<<t)&0x80)
    {
      IIC_SDA_H;
    }
    else 
    {
      IIC_SDA_L;
    }
    delay_us(4);
    IIC_SCL_H;
    delay_us(4);
    IIC_SCL_L;
    delay_us(4);
  }
}

/*******************************************************************************
函数名称：IIC_Read_Byte()
函数功能：IIC接收函数
入口参数：ack
出口参数:返回读到的数据
函数说明：ack=1时，发送应答，ack=0时，不发送应答
*******************************************************************************/
unsigned char IIC_Read_Byte(unsigned char ack)
{
  unsigned char i, reveive = 0;
  IIC_SDA_IN();
  for(i = 0; i < 8; i++)
  {
      IIC_SCL_L;
      delay_us(2);
      IIC_SCL_H;
      reveive <<= 1;
    if(IIC_READ_SDA) reveive++; 
    delay_us(2);
    IIC_SCL_L;
    delay_us(2);
  }
  if(!ack)
      IIC_NAck();
  else
      IIC_Ack();
  return reveive;
}
