#include "ctr.h"

#include "WS2812.h"
#include "dma.h"
#include "stdlib.h"
#include "vibra.h"
#include "bsp_dwt.h"
EnergySys ES;

int num_fan = 0;
int icount = 0;
int flag_flow;  // 0,1,2
uint8_t rand_num1 = 0;
uint8_t rands[2][5] = {{0, 1, 2, 3, 4}, {4, 3, 2, 1, 0}};
TIM_HandleTypeDef* ESfanTimList[5] = {&htim4, &htim2, &htim5, &htim3,
                                      &htim1};  // PB6 PA15 PA0 PB1 PA8
uint8_t ESfanChannelList[5] = {TIM_CHANNEL_1, TIM_CHANNEL_1, TIM_CHANNEL_1,
                               TIM_CHANNEL_4, TIM_CHANNEL_1};
void fan_list_init() {
  for (int8_t i = 0; i <= 4; i++) {
    if (i != 4) ES.fan[i].next = i + 1;
    if (i != 0) ES.fan[i].last = i - 1;
  }
  ES.fan[0].last = 4;
  ES.fan[4].next = 0;
  for (uint8_t i; i < 5; i++) {
    ES.fan[i].mode = OFF;
  }
}

void energyMachine_init() {
  for (uint8_t i = 0; i <= 4; i++) {
    ES.fan[i].tim = ESfanTimList[i];
    ES.fan[i].Channel = ESfanChannelList[i];
  }
}

void control_template_buff(uint16_t left, uint16_t right,
                                  uint32_t value)  // control the base fan
{
  for (uint8_t i = left; i < right; i++) {
    WS2812B_Buf[i] = value;
  }
}

void control_template_bit6(uint16_t left,uint16_t right,uint32_t bitmask)
{
  for (uint8_t j = left; j < right; j++) {
    for (uint8_t i = 0; i < 24; i++) {
      bitmask=0x800000>>i;
      WS2812B_Bit[j * 24 + i + 1] = (WS2812B_Buf[j] & bitmask) ? 45 : 25;
      // bitmask >>= 1;
    }
  }
}
void fan_init() {
  uint8_t ESFanModeList[6] = {0, 0, 0, 0, 0, 1};
  for (uint8_t i = 0; i <= 5; i++) {
    ES.fan[i].mode = ESFanModeList[i];
  }
}

void led_circle_Init() {
  control_template_buff(WS2812B_LED_QUANTITY, WS2812B_LED_QUANTITY + OUTER,
                        0x0000FF);
  control_template_bit6(TWO_Number, All_Number, 1 << (16));
}

void line_init() {
  control_template_buff(TWO_Number, All_Number, 0x0000FF);
  control_template_bit6(TWO_Number, All_Number, 0x800000);
}

// target init
void target_init() {
  control_template_buff(0, Target_Number, 0x000010);
  control_template_bit6(TWO_Number, All_Number, 0x800000);
  HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_3, &WS2812B_Bit[0],
                        24 * Target_Number + 1);
  HAL_Delay(30);
  Vibra_monitor();
}

void boot_init() {
  ES.timeStamp = 0;
  ES.state = 0;
  fan_init();
  vibra_init();
}

// about mode 0-off,1-on,2-flow
void ctr_fan(int s, int RGB) {
  // Void lighting
  if (ES.state == 2)
    ES.fan[5].mode = 2;
  else if (ES.state == 3)
    ES.fan[5].mode = 1;

  for (uint8_t i = 0; i < 6; i++) {
    clear_all();
    update_all();
    switch (ES.fan[i].mode) {
      case 0:
        close_all();
        break;

      case 1:
        light_on(RGB);
        line_init();
        led_circle_Init();
        break;

      case 2:
        light_flow(0, RGB,flag_flow++);  
        led_circle_Init();
        break;
    }
    HAL_TIM_PWM_Start_DMA(ESfanTimList[i], ESfanChannelList[i], &WS2812B_Bit[0],
                          24 * All_Number + 1);
    if (ES.state == 5) {
      fan_list_init();
      HAL_Delay(2000);
    } else
      HAL_Delay(30);
    Vibra_monitor();
  }
}

inline uint8_t rand_num() {
  srand(DWT_GetTimeline());
  return rand() % 5;
  // return rand_num_test();
}

inline void del_list(uint8_t num) {
  ES.fan[ES.fan[num].last].next = ES.fan[num].next;
  ES.fan[ES.fan[num].next].last = ES.fan[num].last;
}

void fan_logic() {
  uint8_t num = 0;
  for (uint8_t i = 0; i < 5; i++) num += (ES.fan[i].mode == 1);
  ES.state = (EnergyMechanismState)num;
  ES.HitErrFlag = 0;

  static uint8_t logic_check = 0;
  ES.timeStamp = DWT_GetTimeline();
  Vibra_monitor();
  if(num != 0)
    ES.fan[ES.need_hit].mode =(vibra_flag[ES.need_hit]==0)?Flow:ON;  // detect the vibra and Shield other vibration
  if (num != 0 && (ES.fan[ES.need_hit].mode != ON)) {
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
    return;
  }
  // if(num!=0&&(ES.fan[ES.need_hit].mode !=
  // ON)&&(ES.timeStamp-ES.last_mode_timeStamp)>2000) return;
  if (num != 0) {
    ES.state = (EnergyMechanismState)(num + 1);
    ES.last_mode_timeStamp = ES.timeStamp;
  }
  if (num == 5) return;
  if (num == 0) fan_list_init();
  uint8_t rand_num_use,use_num = rand_num();
	ES.need_hit=4;
  // while (ES.need_hit--) rand_num_use = ES.fan[rand_num_use].next;
  // del_list(rand_num_use);
  ES.need_hit = rand_num_use;
  ES.fan[rand_num_use].mode = Flow;
}

void close_all() {
  control_template_buff(0, WS2812B_LED_QUANTITY, 0x000000);
  control_template_bit6(0, WS2812B_LED_QUANTITY, 0x800000);
}

void light_on(int RGB) {
  control_template_buff(0, WS2812B_LED_QUANTITY, RGB);
  control_template_buff(TWO_Number, All_Number, RGB);
  control_template_bit6(0, WS2812B_LED_QUANTITY, 0x800000);
  control_template_bit6(TWO_Number, All_Number, 0x800000);
}

void light_flow(uint8_t s, uint8_t RGB, uint8_t flag) {
  uint8_t arrows[3][7] = {{0, 5, 10, 15, 20, 25, 30},
                          {2, 7, 12, 17, 22, 27, 32},
                          {4, 9, 14, 19, 24, 29, 34}};
  draw_arrows(arrows[flag], 7, s, RGB);
  control_template_bit6(0, WS2812B_LED_QUANTITY, 0x800000);
}

void Test(int s, int RGB) {
  // control_template_buff(0, WS2812B_LED_QUANTITY + OUTER, RGB);
  // control_template_bit6(0, WS2812B_LED_QUANTITY + OUTER, 0x800000);
	uint32_t i,j;
	for(i=0;i<WS2812B_LED_QUANTITY+OUTER;i++)
	{
		WS2812B_Buf[i]=RGB;
	}
//	WS2812B_SetBuf(RGB);
	
	for(j=0;j<WS2812B_LED_QUANTITY+OUTER;j++)
	{
		for(i=0;i<24;i++)
		{
			if(WS2812B_Buf[j]&(0x800000>>i)){WS2812B_Bit[j*24+i+1]=45;}  //BITλΪ1 ������Ӧ��ռ�ձ�
			else{WS2812B_Bit[j*24+i+1]=25;}
		}
	}
	

}
