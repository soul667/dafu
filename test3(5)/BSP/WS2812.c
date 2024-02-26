#include "ws2812.h"
#include <stdio.h>
#include "dma.h"
#include "tim.h"
#include <stdlib.h>
#include "ctr.h"


uint32_t WS2812B_Buf[All_Number];	//0xGGRRBB
uint32_t WS2812B_Bit[24*All_Number+1];//这个是将上面的buffer拆解成bit
uint8_t WS2812B_Flag;

uint32_t tim5_flag = 0;

void WS2812B_IRQHandler(void);

/*
*外框
*
**/
void LINE_UpdateBuf(void)
{
	uint32_t i,j;
	for(j=TWO_Number;j<All_Number;j++)
	{
		for(i=0;i<24;i++)
		{
			if(WS2812B_Buf[j]&(0x800000>>i)){WS2812B_Bit[j*24+i+1]=45;}  //BIT位为1 设置相应的占空比
			else{WS2812B_Bit[j*24+i+1]=25;}
		}
	}

}

/*
*外框
*
**/
void LINE_SetBuf(int RGB)
{
	uint32_t i,j;
	for(i=TWO_Number;i<All_Number;i++)
	{
		WS2812B_Buf[i]=RGB;
	}
}


/*
*
*圆型板
*
**/
void OUT_ClearBuf(void)
{
	uint32_t i,j;
	for(i=WS2812B_LED_QUANTITY;i<WS2812B_LED_QUANTITY+OUTER;i++)
	{
		WS2812B_Buf[i]=0x00ff00;
	}
}

/*
*
*圆型板
*
**/
void OUT_UpdateBuf(void)
{
	uint32_t i,j;
	for(j=WS2812B_LED_QUANTITY;j<WS2812B_LED_QUANTITY+OUTER;j++)
	{
		for(i=0;i<24;i++)
		{
			if(WS2812B_Buf[j]&(0x800000>>i)){WS2812B_Bit[j*24+i+1]=45;}  //BIT位为1 设置相应的占空比
			else{WS2812B_Bit[j*24+i+1]=25;}
		}
	}

}


/*
*灯板
*
**/
void WS2812B_ClearBuf(void)//所有数据清零，关灯
{
	uint32_t i;
	for(i=0;i<WS2812B_LED_QUANTITY;i++)
	{
		WS2812B_Buf[i]=0x000000;
	}
}

/*
*灯板
*
**/
void WS2812B_SetBuf(uint32_t Color)//设置灯的颜色
{
	uint32_t i;
	for(i=0;i<WS2812B_LED_QUANTITY;i++)
	{
		WS2812B_Buf[i]=Color;
	}
}

/*
*灯板
*
**/
void WS2812B_UpdateBuf(void)//将设置好的颜色写入到灯里面，让它亮
{
	uint32_t i,j;
	for(j=0;j<WS2812B_LED_QUANTITY;j++)
	{
		for(i=0;i<24;i++)
		{
			if(WS2812B_Buf[j]&(0x800000>>i)){WS2812B_Bit[j*24+i+1]=45;}  //BIT位为1 设置相应的占空比
			else{WS2812B_Bit[j*24+i+1]=25;}
		}
	}
	
////传输
//	HAL_TIM_PWM_Start_DMA(&htim4,TIM_CHANNEL_1,&WS2812B_Bit[0],24*WS2812B_LED_QUANTITY+1);
//	
//	HAL_TIM_PWM_Start_DMA(&htim1,TIM_CHANNEL_1,&WS2812B_Bit[0],24*WS2812B_LED_QUANTITY+1);
//	
//	HAL_TIM_PWM_Start_DMA(&htim3,TIM_CHANNEL_4,&WS2812B_Bit[0],24*WS2812B_LED_QUANTITY+1);
//	
//	HAL_TIM_PWM_Start_DMA(&htim2,TIM_CHANNEL_1,&WS2812B_Bit[0],24*WS2812B_LED_QUANTITY+1);
//	
//	HAL_TIM_PWM_Start_DMA(&htim5,TIM_CHANNEL_1,&WS2812B_Bit[0],24*WS2812B_LED_QUANTITY+1);

}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)//PWM信号传输完成回调函数，该函数非常之重要
{
	if(htim == &htim4 )
	{
	//恒定输入高电平
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,0);//PB6
   HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_1);
	}
	
	if(htim == &htim1)
	{
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);//PA8
   HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	}
	
	if(htim == &htim2)
	{
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,0);//PA15
   HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	}
	
	if(htim == &htim3)
	{
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);//PB1
   HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_4);
	}
	
	if(htim == &htim5)
	{
	__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,0);//PA0
   HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_1);
		
	}
	
	if(htim == &htim8)
	{
	
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_3,0);//PA0
   HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_3);
		
	}
}//停止DMA并复位，注意STM32F1的多路DMA需要中断回调，并且DMA发送模式应该是循环


// 再添加一个支持列表的函数
// void draw_arrows(void *station,int Step,int Color)
// {

// }
void draw_arrows(uint8_t * stations,uint8_t  length,uint8_t Step,uint8_t Color)
{
	uint8_t number;
	uint16_t location1,location2;
	int new1;
	for (uint8_t w=0;w<length;w++)
    {
      uint8_t  station=stations[w];
      for(uint8_t i=0;i<6;i++)
      {
        if(i<3)
        {
          for(uint8_t j=0;j<=i;j++)
          {
            location1 = j + i * 7 +Step*7 + station*7;
            location1 = (location1>237)?location1 -238:location1;
            location2 = location1 + (6-2*j);
            WS2812B_Buf[location1]=Color;
            WS2812B_Buf[location2]=Color;
          }
        }
        else
        {
          number = (i - 2) * 2;
          for (uint8_t k = 0; k < (7 - number); k++)
          {
            new1 = i * 7 + k + (number / 2)+Step*7+station*7;
            if(new1>237)
            {
              new1=new1-238;
            }
            WS2812B_Buf[new1]=Color;
          }
        }
      }
    }
}


void Scroll_pattern(int time,int Scolor,int lStep)
{
        uint8_t arrows1[7]={0,5,10,15,20,25,30};
		draw_arrows(arrows1,7,lStep,Scolor);
		WS2812B_UpdateBuf();
		HAL_Delay(time);//复位信号
		WS2812B_ClearBuf();
		WS2812B_UpdateBuf();

}


/*
*
*总赋值
*
**/
void update_all(void)
{
	uint32_t i,j;
	for(j=0;j<All_Number;j++)
	{
		for(i=0;i<24;i++)
		{
			if(WS2812B_Buf[j]&(0x800000>>i)){WS2812B_Bit[j*24+i+1]=45;}  //BIT位为1 设置相应的占空比
			else{WS2812B_Bit[j*24+i+1]=25;}
		}
	}
}	

void ALL_SetBuf(void)
{

	uint32_t i;
	for(i=0;i<All_Number;i++)
	{
		WS2812B_Buf[i]=0x00ff00;
	}

}

void clear_all(void)
{
	uint32_t i;
	for(i=0;i<All_Number;i++)
	{
		WS2812B_Buf[i]=0x000000;
	}

}