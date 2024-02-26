#include "ws2812.h"
#include <stdio.h>
#include "dma.h"
#include "tim.h"
#include <stdlib.h>
#include "ctr.h"


uint32_t WS2812B_Buf[All_Number];	//0xGGRRBB
uint32_t WS2812B_Bit[24*All_Number+1];//����ǽ������buffer����bit
uint8_t WS2812B_Flag;
TIM_HandleTypeDef* ESfanTimList1[6] = {&htim4, &htim1, &htim2, &htim3,
                                      &htim5,&htim8};  // PB6 PA8 PA15 PB1 PA0 //PA0
uint8_t ESfanChannelList1[6] = {TIM_CHANNEL_1, TIM_CHANNEL_1, TIM_CHANNEL_1,
                               TIM_CHANNEL_4, TIM_CHANNEL_1,TIM_CHANNEL_3};
uint32_t tim5_flag = 0;

void WS2812B_IRQHandler(void);
inline void control_template_buff(uint16_t left, uint16_t right,uint32_t value)  // control the base fan
{
  for (uint16_t i0 = left; i0 < right; i0++) {
    WS2812B_Buf[i0] = value;
  }
}
inline void control_template_bit(uint16_t left, uint16_t right,
                                        uint32_t bitmask)  // control the base fan
{
  for (uint16_t j0 = left; j0 < right; j0++) {
    for (uint16_t i0 = 0; i0 < 24; i0++)
      if( WS2812B_Buf[j0] & (bitmask>>i0))  WS2812B_Bit[j0 * 24 + i0 + 1]=45;
      else WS2812B_Bit[j0 * 24 + i0 + 1]=25;
//       WS2812B_Bit[j0 * 24 + i0 + 1] = (WS2812B_Buf[j0] & (bitmask>>i0)) ? 45 : 25;
    }
  }
/*
*���
 * __attribute__((unused)) ����δʹ�õľ�̬�����ı���
**/
__attribute__((unused))  inline void LINE_UpdateBuf(void)
{
  control_template_bit(TWO_Number,All_Number,0x800000);
}

__attribute__((unused))  inline void LINE_SetBuf(int RGB) // ���
{
  control_template_buff(TWO_Number, All_Number, RGB);
}

__attribute__((unused))  inline  void OUT_ClearBuf(void) //Բ�Ͱ�
{
  control_template_buff(WS2812B_LED_QUANTITY, WS2812B_LED_QUANTITY + OUTER, 0x00ff00);
}

__attribute__((unused))  inline  void OUT_UpdateBuf(void) //Բ�Ͱ�
{
  control_template_bit(WS2812B_LED_QUANTITY,WS2812B_LED_QUANTITY+OUTER,0x800000);
}

__attribute__((unused))  inline void WS2812B_ClearBuf(void)//�����������㣬�ص�
{
  control_template_buff(0, WS2812B_LED_QUANTITY, 0x000000);
}

__attribute__((unused))  inline void WS2812B_SetBuf(uint32_t Color)//���õƵ���ɫ
{
  control_template_buff(0, WS2812B_LED_QUANTITY, Color);
}

void WS2812B_UpdateBuf(void)//�����úõ���ɫд�뵽�����棬������
{
  control_template_bit(0,WS2812B_LED_QUANTITY,0x800000);
}

__attribute__((unused))  inline void update_all(void)
{
  control_template_bit(0,All_Number,0x800000);
}

__attribute__((unused))  inline void ALL_SetBuf(void)
{
  control_template_buff(0,All_Number,0x00ff00);
}

__attribute__((unused))  inline void clear_all(void)
{
  control_template_buff(0, All_Number, 0x000000);
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)//PWM�źŴ�����ɻص��������ú����ǳ�֮��Ҫ
{
  for(uint8_t i=0;i<5;i++)
  {
    if(htim == ESfanTimList1[i])
    {
      __HAL_TIM_SET_COMPARE(ESfanTimList1[i],ESfanChannelList1[i],0);//PB6
      HAL_TIM_PWM_Stop_DMA(ESfanTimList1[i],ESfanChannelList1[i]);
    }
  }

	if(htim == &htim8)
	{

		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_3,0);//PA0
   HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_3);

	}
}//ֹͣDMA����λ��ע��STM32F1�Ķ�·DMA��Ҫ�жϻص�������DMA����ģʽӦ����ѭ��

//void Scroll_pattern(int time,int Scolor,int lStep)
//{
//        uint8_t arrows1[7]={0,5,10,15,20,25,30};
//		draw_arrows(arrows1,7,lStep,Scolor);
//		WS2812B_UpdateBuf();
//		HAL_Delay(time);//��λ�ź�
//		WS2812B_ClearBuf();
//		WS2812B_UpdateBuf();
//}


