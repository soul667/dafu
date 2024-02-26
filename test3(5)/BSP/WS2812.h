#ifndef __ws2812_H__
#define __ws2812_H__
#include <stdint.h>
//typedef unsigned int uint32_t ;
//typedef unsigned char uint8_t;

//OUTER����Բ��LINE�Ǳ߿�
#define  WS2812B_LED_QUANTITY	175//ws1218����   --175
#define	 OUTER 162    // ---49
#define 	 LINE  64    //  ---58
#define TWO_Number 337    //  ----224
#define All_Number 401    //   ----282
#define Target_Number 30

extern uint32_t WS2812B_Buf[All_Number];	//0xGGRRBB
extern uint32_t WS2812B_Bit[24*All_Number+1];//����ǽ������buffer����bit

//�ư�
void WS2812B_ClearBuf(void);
void WS2812B_SetBuf(uint32_t Color);
void WS2812B_UpdateBuf(void);

//Բ��
void OUT_ClearBuf(void);
void OUT_UpdateBuf(void);

//���
void LINE_SetBuf(int RGB);
void LINE_UpdateBuf(void);
void draw_arrows(uint8_t * stations,uint8_t length,uint8_t Step,uint8_t Color);
void Scroll_pattern(int time,int Scolor,int lStep);//��Ҷ��ʼ��

//����
void ALL_SetBuf(void);
void update_all(void);
void clear_all(void);

extern uint32_t tim5_flag;

/*
#define COLOR_BLACK		(0x000000)
#define COLOR_WHITE		(0xffffff)
#define COLOR_RED		(0xff0000)
#define COLOR_GREEN		(0x00ff00)
#define COLOR_YELLOW	(0x0000ff)
*/
#endif
