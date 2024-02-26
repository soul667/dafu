#ifndef __ws2812_H__
#define __ws2812_H__
#include <stdint.h>
//#include "stm32f1xx_hal_tim.h"
// 63+175+162

#define MID_LED_NUM 63
#define R_NUM 37
#define  WS2812B_LED_QUANTITY	175//ws1218????   --175 流水灯的？
#define	 OUTER 162    // ---49
#define TWO_Number 337    //  ----224
#define All_Number 401    //   ----282
#define Target_Number 30

extern uint32_t WS2812B_Buf[All_Number];	//0xGGRRBB
extern uint32_t WS2812B_Bit[24*All_Number+1];
void WS2812B_ClearBuf(void);
void WS2812B_SetBuf(uint32_t Color);
void WS2812B_UpdateBuf(void);

void OUT_ClearBuf(void);
void OUT_UpdateBuf(void);

void LINE_SetBuf(int RGB);
void LINE_UpdateBuf(void);
//void draw_arrows(const uint8_t * stations,uint8_t length,uint8_t Step,uint8_t Color);
//void Scroll_pattern(int time,int Scolor,int lStep);


void ALL_SetBuf(void);
// void update_all(void);
// void clear_all(void);

extern uint32_t tim5_flag;
void control_template_buff(uint16_t left, uint16_t right,uint32_t value); // control the base fan
void control_template_bit(uint16_t left,uint16_t right,uint32_t bitmask); // control the base fan

#endif
