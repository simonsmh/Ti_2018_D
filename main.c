#include "msp.h"
#include "msp430f5529.h"
#include "bsp_fdc2214.h"
#include "bsp_iic.h"
#include "stdio.h"
#include "delay.h"

void Show_Mode();
void Traning();
void Reseting();
long int GetSensor();
long int self_abs();
void P1_IODect();
void P11_Onclick();
void P13_Onclick();
void P14_Onclick();
void P15_Onclick();
void P16_Onclick();

long int FDC2214_Val;
long int FDC2214_STO[30];
long int FDC2214_TMP[10];
long int Gesture_STO[3] = {200000,350000,600000};       //Ĭ�ϲ�ȭ����
long int Gesture_STI[5] = {230000,320000,350000,400000,600000};       //Ĭ�ϻ�ȭ����
long int *fGesture = Gesture_STO;
unsigned int guess_mode = 0;
unsigned int train_mode = 0;
unsigned int start_training = 0;
unsigned int timeleft = 3;
unsigned int left = 0;
long int fingers[5];
long int env = 0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    UCS_Init();
    GPIO_Init();
    Timer0_Init();
    Timer1_Init();
    IIC_Init();
    InitMultiFDC2214();
    _EINT();
    LPM0;
}

void P11_Onclick() //���ػ�����ť
{
    LED_REFLOW;
    Reseting();
    Show_Mode();
    LED_FLOW;
}

void P16_Onclick() //���ð�ť
{
    if ((train_mode == 1) && (start_training == 2))
    {
        left--;
        Traning();
    }
    else
        Show_Mode();
}

void P13_Onclick() //��ʼѵ����ť
{
    if (train_mode == 1)
    {
        if (start_training == 0)
            start_training++;
        Traning();
    }
    else
        Show_Mode();
}

void P14_Onclick() //��ȭ����ȭ
{
    P1OUT ^= BIT0; //���غ��
    guess_mode++;
    timeleft = 5;
    fGesture = Gesture_STI;
    if (guess_mode > 1)
    {
        guess_mode = 0;
        timeleft = 3;
        fGesture = Gesture_STO;
    }
    Show_Mode();
}

void P15_Onclick() //ѵ�����б�
{
    P4OUT ^= BIT7; //�����̵�
    train_mode++;
    if (train_mode > 1)
    {
        train_mode = 0;
    }
    Show_Mode();
}

long int self_abs(long int i)
{
    if (i < 0)
        return -i;
    else
        return i;
}

void Show_Mode()
{
    LED_REFLOW;
    unsigned char i;
    //printf("��ȭ:%i\n", guess_mode);
    //printf("ѵ��ģʽ:%i\n", train_mode);
    //printf("��ʼѵ��:%i\n", start_training);
    //printf("������:%li\n", GetSensor(0));
    //printf("����ֵ:%li\n", env);
    //printf("ʣ�����:%i\n", left);
    for (i = 0; i < timeleft; i++)
        //printf("Ԥ������ֵ:%li\n", fGesture[i]);
    LED_REFLOW;
}

void Reseting()
{
//    //printf("��ʼ��������ֵ\n");
    LED_FLOW;
    env = GetSensor(30);
//    //printf("��������ֵ���,ƽ��ֵ%li\n", env);
    LED_REFLOW;
}

void Traning()
{
    static long int finger;
    if (start_training == 1)
    {
        LED_FLOW;
        start_training++;
    }
    else if ((start_training == 2) && (left < timeleft))
    {
        if (left == 0)
        {
            LED_ON_1;
        }
        else if (left == 1)
        {
            LED_ON_2;
        }
        else if (left == 2)
        {
            LED_ON_3;
        }
        else if (left == 3)
        {
            LED_ON_4;
        }
        else if (left == 4)
        {
            LED_ON_5;
        }
//        //printf("ѵ��%i���ڽ���\n", left + 1);
        finger = GetSensor(30);
//        //printf("ƽ��ֵ%li��ѵ��%i����\n", finger, left + 1);
        fGesture[left] = env - finger;
//        //printf("����ֵΪ%li\n", fGesture[left]);
        start_training = 2;
        left++;
        LED_RESET;
    }
    else if (left == timeleft)
    {
        start_training = 0;
        left = 0;
        LED_REFLOW;
        Show_Mode();
    }
    else
    {
        Show_Mode();
    }
}

long int GetSensor(unsigned int t)
{
    static unsigned temp = 0;
    unsigned int i;
    long int sum = 0;
    if (t != 0)
    {
        for (i = 0; i < t; i++)
        {
            FDC2214_STO[i] = FDC2X14ReadCH(4);
            delay_ms(30);
//            //printf("CH4:%li \n",FDC2214_STO[i]);
        }
        for (i = 0; i < t; i++)
            sum += FDC2214_STO[i];
        return (sum /= t);
    }
    else
    {
        if (temp > 9)
        {
            temp = 0;
        }
        FDC2214_TMP[temp] = FDC2X14ReadCH(4);
        delay_ms(30);
        for (i = 0; i < 10; i++)
            sum += FDC2214_TMP[i];
//        //printf("CH4:%li  ,%li\n",FDC2214_TMP[temp],sum/10);
        temp++;
        return (sum /= 10);
    }
//            return FDC2X14ReadCH(4);
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1_CC0_ISR() //20hz
{
    unsigned int j_;
    long int test_[3];
    long int tmp_ = 1, tmpd_, finger_;
    FDC2214_Val = GetSensor(0);
    if (train_mode == 0)
    {
        finger_ = env - FDC2214_Val;
//                //printf("ԭʼֵ%li,", FDC2214_Val);
                //printf("��������ֵΪ%li,", finger_);
        if (finger_ >= 0)
        {
            tmpd_ = self_abs(fGesture[0] - finger_);
            for (j_ = 0; j_ < timeleft; j_++)
            {
                test_[j_] = self_abs(fGesture[j_] - finger_);
                                //printf("�б�����ֵ:%li,", test_[j_]);
                if (test_[j_] < tmpd_)
                {
                    tmpd_ = test_[j_];
                    tmp_ = j_ + 1;
                }
            }
                        //printf("����ֵΪ%li\n", tmp_);
            if (tmp_ == 1)
            {
                LED_ON_1;
            }
            else if (tmp_ == 2)
            {
                LED_ON_2;
            }
            else if (tmp_ == 3)
            {
                LED_ON_3;
            }
            else if (tmp_ == 4)
            {
                LED_ON_4;
            }
            else if (tmp_ == 5)
            {
                LED_ON_5;
            }
        }
        else
        {
            //printf("δ��⵽����\n");
            LED_RESET;
        }
    }
    TA1IV &= ~TA1IV_TAIFG; //����жϱ�־
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    unsigned int Push_Key = 0;
    Push_Key = P1IFG & (~P1DIR);
    LED_RESET;
    if ((P1IN & Push_Key) == 0) //����ôΰ���ȷʵ��Ч
    {
        switch (Push_Key)
        {
        case BIT1:
            P11_Onclick();
            break;
        case BIT3:
            P13_Onclick();
            break;
        case BIT4:
            P14_Onclick();
            break;
        case BIT5:
            P15_Onclick();
            break;
        case BIT6:
            P16_Onclick();
            break;
        default:
            break;
        }
    }
    P1IFG = 0; //�˳��ж�ǰ�����ֶ����IO���жϱ�־
}
