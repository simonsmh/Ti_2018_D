#ifndef __BSP_FDC2214_H
#define __BSP_FDC2214_H


#define u8 unsigned char
void InitSingleFDC2214(void);
void InitMultiFDC2214(void);//Ë«Í¨µÀ
unsigned int ReadRegfdc2214(unsigned char add);
long int ReadFDC2X14(u8 firstAddress,u8 secondAddress);
long int FDC2X14ReadCH(u8 index);

#endif
