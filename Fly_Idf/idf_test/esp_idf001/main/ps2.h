
#ifndef __PS2_H__
#define __PS2_H__


#include "driver/gpio.h"
#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define DI   gpio_get_level(8)           //PB12  输入

#define DO_H gpio_set_level(9,1)        //命令位高
#define DO_L gpio_set_level(9,0)        //命令位低

#define CS_H gpio_set_level(10,1)       //CS拉高
#define CS_L gpio_set_level(10,0)       //CS拉低

#define CLC_H gpio_set_level(11,1)       //时钟拉高
#define CLC_L gpio_set_level(11,0)       //时钟拉低



//These are our button constants
#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16
#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      16



//These are stick values
#define PSS_RX 5                //右摇杆X轴数据
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8



extern uint8_t Data[9];
extern uint16_t MASK[16];
extern uint16_t Handkey;

void PS2_Init(void);
uint8_t PS2_DataKey(void);		  //键值读取
uint8_t PS2_AnologData(uint8_t button); //得到一个摇杆的模拟量
void PS2_ClearData(void);	  //清除数据缓冲区


#endif
