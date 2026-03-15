#include "ps2.h"

//#include "stm32f10x.h"
/*
#define DO_H         GPIOA->BSRR = GPIO_Pin_9
#define DO_L         GPIOA->BRR  = GPIO_Pin_9
#define CLC_H        GPIOA->BSRR = GPIO_Pin_11
#define CLC_L        GPIOA->BRR  = GPIO_Pin_11
#define CS_H         GPIOA->BSRR = GPIO_Pin_10
#define CS_L         GPIOA->BRR  = GPIO_Pin_10
#define DI      GPIOA->IDR  & GPIO_Pin_8	 */
uint16_t Handkey;
uint8_t Comd[2]={0x01,0x42};	//开始命令。请求数据
uint8_t scan[9]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//{0x01,0x42,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};	// 类型读取

uint8_t Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //数据存储数组
uint16_t MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
	};	//按键值与按键明


void PS2_Init(void)
{
	    time_init();
			    gpio_config_t cfg_in = {
        .pin_bit_mask = (1ULL << GPIO_NUM_8),
        .mode = GPIO_MODE_INPUT,
		.intr_type=GPIO_INTR_DISABLE,
		.pull_down_en=GPIO_PULLDOWN_ENABLE
    };
			gpio_config(&cfg_in);
			    gpio_config_t cfg_out = {
        .pin_bit_mask = (1ULL << GPIO_NUM_9)|(1ULL << GPIO_NUM_10)|(1ULL << GPIO_NUM_11),
        .mode = GPIO_MODE_OUTPUT,
		.intr_type=GPIO_INTR_DISABLE,
    };
			gpio_config(&cfg_out);

	DO_H;
	CLC_H;
	CS_H;

}

//读取手柄数据
u8 PS2_ReadData(u8 command)
{

	u8 i,j=1;
	u8 res=0; 
    for(i=0; i<=7; i++)          
    {
		if(command&0x01)
			DO_H;
		else
			DO_L;
		command = command >> 1;
		mydelay_us(10);
		CLC_L;
		mydelay_us(10);
		if(DI) 
			res = res + j;
		j = j << 1; 
		CLC_H;
		mydelay_us(10);	 
    }
    DO_H;
	mydelay_us(50);
    return res;	
}

//对读出来的 PS2 的数据进行处理
//按下为 0， 未按下为 1
unsigned char PS2_DataKey()
{
	u8 index = 0, i = 0;

	PS2_ClearData();
	CS_L;
	for(i=0;i<9;i++)	//更新扫描按键
	{
		Data[i] = PS2_ReadData(scan[i]);	
	} 
	CS_H;
	

	Handkey=(Data[4]<<8)|Data[3];     //这是16个按键  按下为0， 未按下为1
	for(index=0;index<16;index++)
	{	    
		if((Handkey&(1<<(MASK[index]-1)))==0)
			return index+1;
	}
	return 0;          //没有任何按键按下
}

//得到一个摇杆的模拟量	 范围0~256
u8 PS2_AnologData(u8 button)
{
	return Data[button];
}

//清除数据缓冲区
void PS2_ClearData()
{
	u8 a;
	for(a=0;a<9;a++)
		Data[a]=0x00;
}
