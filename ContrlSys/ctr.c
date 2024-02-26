
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
uint8_t arrows[3][7] = {{0, 5, 10, 15, 20, 25, 30},
                        {2, 7, 12, 17, 22, 27, 32},
                        {4, 9, 14, 19, 24, 29, 34}};
uint8_t rands[2][5] = {{0, 1, 2, 3, 4}, {4, 3, 2, 1, 0}};
TIM_HandleTypeDef *ESfanTimList[6] = {&htim4, &htim2, &htim5, &htim3,
                                      &htim1, &htim8};  // PB6 PA15 PA0 PB1 PA8
uint8_t ESfanChannelList[6] = {TIM_CHANNEL_1, TIM_CHANNEL_1, TIM_CHANNEL_1,
                               TIM_CHANNEL_4, TIM_CHANNEL_1, TIM_CHANNEL_3};
void fan_list_init() {
  for (int8_t i = 0; i <= 4; i++) {
    if (i != 4) ES.fan[i].next = i + 1;
    if (i != 0) ES.fan[i].last = i - 1;
  }
  ES.fan[0].last = 4;
  ES.fan[4].next = 0;
  for (uint8_t i = 0; i < 5; i++) {
    ES.fan[i].mode = OFF;
  }
  ES.last_mode_timeStamp = ES.timeStamp;
}

void energyMachine_init() {
  for (uint8_t i = 0; i <= 4; i++) {
    ES.fan[i].tim = ESfanTimList[i];
    ES.fan[i].Channel = ESfanChannelList[i];
  }
}

void fan_init() {
  uint8_t ESFanModeList[6] = {0, 0, 0, 0, 0, 1};
  for (uint8_t i = 0; i <= 5; i++) {
    ES.fan[i].mode = ESFanModeList[i];
  }
}

void led_circle_Init(int RGB) {
  control_template_buff(WS2812B_LED_QUANTITY, WS2812B_LED_QUANTITY + OUTER,
                        RGB);
  control_template_bit(WS2812B_LED_QUANTITY, WS2812B_LED_QUANTITY + OUTER, 0x800000);
}

void line_init(int RGB) {
  control_template_buff(TWO_Number, All_Number, RGB);
  control_template_bit(TWO_Number, All_Number, 0x800000);
}

// target init
void target_init() {
  control_template_buff(0, Target_Number, 0x000010);
  control_template_bit(TWO_Number, All_Number, 0x800000);
  HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_3, &WS2812B_Bit[0],
                        24 * Target_Number + 1);
  HAL_Delay(10);
  Vibra_monitor();
}

void boot_init() {
  ES.timeStamp = 0;
  ES.state = 0;
  fan_init();
  vibra_init();
  fan_list_init();
}

void ctr_fan(uint8_t s, int RGB) {

  //  ES.fan[2].mode = ON; // 2
  if (ES.state == 2)
    ES.fan[5].mode = Flow; // 2
  else if (ES.state == 3)
    ES.fan[5].mode = 4; // 1

  if (ES.state == 6) {
    // light_on(RGB);
    // line_init(RGB);
    // led_circle_Init(RGB);
    control_template_buff(0, WS2812B_LED_QUANTITY + OUTER, RGB);
    control_template_bit(0, WS2812B_LED_QUANTITY + OUTER, 0x800000);
    for (uint8_t i = 0; i < 5; i++) {
      HAL_TIM_PWM_Start_DMA(ESfanTimList[i], ESfanChannelList[i], &WS2812B_Bit[0],
                            24 * All_Number + 1);
    }

    HAL_Delay(1000);

    fan_list_init();
    close_all();
    boot_init();
    // ???????????
    // When restarting, need to re-initialize but regardless of the center
    for (uint8_t i = 0; i < 5; i++) {
      HAL_TIM_PWM_Start_DMA(ESfanTimList[i], ESfanChannelList[i], &WS2812B_Bit[0],
                            24 * All_Number + 1);
    }
//    return;
  }
  if (ES.timeStamp - ES.last_mode_timeStamp > 3149 || ES.status_same) {
    if (ES.status_same)
      ES.status_same = 0;
    ES.state = 0;
    ES.last_need_hit = ES.need_hit;
    fan_list_init();
    close_all();
    fan_init();
    vibra_init();

    ES.timeStamp = ES.last_mode_timeStamp;

    // for (uint8_t i = 0; i < 6; i++)
    // {
    //   HAL_TIM_PWM_Start_DMA(ESfanTimList[i], ESfanChannelList[i], &WS2812B_Bit[0],
    //                         24 * All_Number + 1);
    // }
  }
  for (uint8_t i = 0; i < 6; i++) {
    control_template_buff(0, All_Number, 0x000000);
    control_template_bit(0, All_Number, 0x800000);
    switch (ES.fan[i].mode) {
      case 0:close_all();
        break;

      case 1:light_on(RGB);
        line_init(RGB);
        led_circle_Init(RGB);
        break;

        // flow
      case 2:light_flow(s, RGB, flag_flow); // passes the value of flag_flow to the
        flag_flow = (flag_flow == 2) ? 0 : flag_flow + 1;
        LED_YUAN_Init(RGB);
        break;
    }

    if (i == 5) {
      control_template_buff(0, All_Number, RGB);
      control_template_bit(0, All_Number, 0x800000);
    }

    HAL_TIM_PWM_Start_DMA(ESfanTimList[i], ESfanChannelList[i], &WS2812B_Bit[0],
                          24 * All_Number + 1);
    // HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_3, &WS2812B_Bit[0],
    //  24 * All_Number + 1);
    Vibra_monitor();
    HAL_Delay(30);
    Vibra_monitor();
  }
  //    Test(s,RGB);
}

inline void del_list(uint8_t num) {
  ES.fan[ES.fan[num].last].next = ES.fan[num].next;
  ES.fan[ES.fan[num].next].last = ES.fan[num].last;
}

void fan_logic(uint8_t s) {
  uint8_t num = 0;
  for (uint8_t i = 0; i < 5; i++) num += (ES.fan[i].mode == ON) || (ES.fan[i].mode == Flow);
  ES.state = (EnergyMechanismState) num;
  ES.HitErrFlag = 0;
  static uint8_t logic_check = 0;
//  ES.timeStamp = DWT_GetTimeline();
  ES.timeStamp = HAL_GetTick();

  if (num != 0) {
//    if(ES.timeStamp-ES.last_mode_timeStamp>20000000){
//      fan_list_init();
//    }
    Vibra_monitor();
    if (vibra_flag[ES.need_hit] == 0) {
//      __set_FAULTMASK(1);
//      HAL_NVIC_SystemReset();
      return;
    } else {
      ES.fan[ES.need_hit].mode = ON; // detect the vibra and Shield other vibration
      ES.state = (EnergyMechanismState) (num + 1);
      vibra_init();
    }
  }
//  uint8_t rand_num_use,use_num = ES.timeStamp%5;
  if (ES.state == success_activate + 1) {
    return;
  }
  uint8_t rand_num_use = 0, use_num = ES.timeStamp%5+1;//确保肯定不是0
  while (use_num--) rand_num_use = ES.fan[rand_num_use].next;
  del_list(rand_num_use);
//  if(rand_num_use==ES.last_need_hit&&ES.no_first_hid){
//    ES.last_mode_timeStamp = ES.timeStamp;
//    ES.status_same=1;
//return;
//  } // 直接重启
  if (!ES.no_first_hid) {
    ES.no_first_hid = 1;
  }
  ES.need_hit = rand_num_use;
  ES.fan[ES.need_hit].mode = Flow;
  ES.last_mode_timeStamp = ES.timeStamp;

}

void close_all() {
  control_template_buff(0, All_Number, 0x000000);
  control_template_bit(0, All_Number, 0x80000);
}

void light_on(int RGB) {
  control_template_buff(0, WS2812B_LED_QUANTITY, RGB);
  control_template_buff(TWO_Number, All_Number, RGB);
  control_template_bit(0, WS2812B_LED_QUANTITY, 0x80000);
  control_template_bit(TWO_Number, All_Number, 0x80000);
}

void light_flow(uint8_t s, uint32_t RGB, uint8_t flag) {
  draw_arrows(flag, s, RGB);
  control_template_bit(0, WS2812B_LED_QUANTITY, 0x800000);
}

void Test(int s, int RGB) {
  control_template_buff(0, WS2812B_LED_QUANTITY + OUTER, RGB);
  control_template_bit(0, WS2812B_LED_QUANTITY + OUTER, 0x800000);
  for (uint8_t i = 0; i < 5; i++) {
    HAL_TIM_PWM_Start_DMA(ESfanTimList[i], ESfanChannelList[i], &WS2812B_Bit[0],
                          24 * All_Number + 1);
  }
}

void LED_YUAN_Init(uint32_t Color) {
  control_template_buff(All_Number - OUTER, All_Number, Color);
  control_template_bit(All_Number - OUTER, All_Number, 0x800000);
}

void draw_arrows(uint8_t flag, uint8_t Step, uint32_t Color) {
  uint8_t number = 0;
  uint16_t location1 = 0, location2 = 0;
  uint32_t new1 = 0;
  for (uint8_t w = 0; w < 7; w++) {
    number = 0, location1 = 0, location2 = 0, new1 = 0;
    uint8_t station = arrows[flag][w];
    for (uint8_t i = 0; i < 6; i++) {
      if (i < 3) {
        for (uint8_t j = 0; j <= i; j++) {
          location1 = j + i * 7 + Step * 7 + (uint16_t) station * 7;
          location1 = (location1 > 237) ? location1 - 238 : location1;
          location2 = location1 + (6 - 2 * j);
          WS2812B_Buf[location1] = Color;
          WS2812B_Buf[location2] = Color;
        }
      } else {
        number = (i - 2) * 2;
        for (uint8_t k = 0; k < (7 - number); k++) {
          new1 = i * 7 + k + ((uint32_t) number / 2) + Step * 7 + (uint32_t) station * 7;
          if (new1 > 237) {
            new1 = new1 - 238;
          }
          WS2812B_Buf[new1] = Color;
        }
      }
    }
  }
}
void MY_Delay(uint32_t Delay) {
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY) {
    wait += (uint32_t) (uwTickFreq);
  }
  while ((HAL_GetTick() - tickstart) < wait) {
    Vibra_monitor();
  }
}
