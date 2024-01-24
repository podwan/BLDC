
#include "stm32f4xx.h"
#include <string.h>
#include "MyProject.h"


/******************************************************************************/
//串口打印用USART2(排针的GPIO 3/4引脚)，3为TXD2，4为RXD2
/************************************************
电机驱动板405
闭环控制  演示
1、力矩模式
2、速度模式
3、位置模式
=================================================
使用教程：https://blog.csdn.net/loop222/article/details/128339581
        《SimpleFOC移植STM32（七）—— 移植STM32F405RGT6》
创建日期：20230403
作    者：loop222 @郑州
************************************************/
//1、串口发送升级为DMA方式，配合sprintf函数，在主循环中打印不再影响电机运行
//2、初始化过程中的打印信息，为了防止数据覆盖，仍用printf()
/******************************************************************************/
#define LED_blink   GPIOD->ODR^=(1<<2)  //PD2
/******************************************************************************/
extern void delay_s(uint32_t i);
void commander_run(void);
/******************************************************************************/
float target;
uint32_t timecntr_pre=0;
uint32_t time_cntr=0;
/******************************************************************************/
//us计时，每71.5分钟溢出循环一次
uint32_t timecount(void)
{
	uint32_t  diff,now_us;
	
	now_us = _micros();    //0xFFFFFFFF=4294967295 us=71.5分钟
	if(now_us>=timecntr_pre)diff = now_us - timecntr_pre;   //us
	else
		diff = 0xFFFFFFFF - timecntr_pre + now_us;
	timecntr_pre = now_us;
	
	return diff;
}
/******************************************************************************/
void GPIO_Configure(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOA,ENABLE);//使能GPIOD,GPIOC,GPIOB,GPIOA;
	
	//指示灯为PD2，低电平灯亮，高电平灯灭，
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;  //IO输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP; 
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);       //亮
}
/******************************************************************************/
/******************************************************************************/
int main(void)
{
	GPIO_Configure();
	USART2_Init(115200);       //GPIO 3/4引脚，3为TXD2，4为RXD2
	TIM1_PWM_Init();           //M0接口
	systick_CountInit();       //systick时钟开启1ms中断模式
	
	delay_ms(500);             //最大延时798ms
	MagneticSensor_Init();
	LPF_init();                //LPF参数初始化
	PID_init();                //PID参数初始化
	
	voltage_power_supply=12;   //V 电源电压
	pole_pairs=7;              //电机极对数，按照实际设置，虽然可以上电检测但有失败的概率
	voltage_sensor_align=2;    //V 航模电机设置的值小一点比如0.5-1，云台电机设置的大一点比如2-3
	voltage_limit=5;           //V，主要为限制电机最大电流，最大值需小于12/1.732=6.9
	velocity_limit=20;         //rad/s 角度模式时限制最大转速，力矩模式和速度模式不起作用
	current_limit=50;          //A，foc_current和dc_current模式限制电流，不能为0。速度模式和位置模式起作用
	torque_controller=Type_voltage;  //当前只有电压模式
	controller=Type_velocity;  //Type_angle; //Type_torque; //Type_velocity
	PID_velocity.P=0.1;        //0.2, 速度环PI参数，只用P参数方便快速调试
	PID_velocity.I=1;
	P_angle.P=20;              //位置环参数，只需P参数，一般不需要改动
	PID_velocity.output_ramp=100; //速度爬升斜率，如果不需要可以设置为0
	LPF_velocity.Tf=0.02;
	target=0;
	
	Motor_init();
	Motor_initFOC(0,UNKNOWN);  //(0,UNKNOWN);  //(1.1,CW); 第一次先获得偏移角和方向，填入代码编译后再下载，以后可以跳过零点校准
	printf("Motor ready.\r\n");
	
	while(1)
	{
		time_cntr +=timecount();
		if(time_cntr>=500000)  //us
		{
			time_cntr=0;
			LED_blink;
			//USART2_SendDMA(sprintf(snd2_buff,"Vel=%.4f\r\n",shaft_velocity));    //DMA方式发送,不影响电机运行；sprintf函数把要打印的字符串格式化
		}
		move(target);
		loopFOC();
		commander_run();
	}
}
/******************************************************************************/
void commander_run(void)
{
	if(rcv2_flag==1)
	{
		rcv2_flag=0;
		switch(rcv2_buff[0])
		{
			case 'H':
				USART2_SendDMA(sprintf(snd2_buff,"Hello World!\r\n"));
				break;
			case 'T':   //T6.28
				target=atof((const char *)(rcv2_buff+1));
				USART2_SendDMA(sprintf(snd2_buff,"RX=%.2f\r\n",target));
				break;
			case 'P':   //P0.5  设置速度环的P参数  
				PID_velocity.P=atof((const char *)(rcv2_buff+1));
				USART2_SendDMA(sprintf(snd2_buff,"P=%.2f\r\n",PID_velocity.P));
				break;
			case 'I':   //I0.2  设置速度环的I参数  
				PID_velocity.I=atof((const char *)(rcv2_buff+1));
				USART2_SendDMA(sprintf(snd2_buff,"I=%.2f\r\n",PID_velocity.I));
				break;
			case 'V':   //V  读实时速度
				USART2_SendDMA(sprintf(snd2_buff,"Vel=%.2f\r\n",shaft_velocity));
				break;
			case 'A':   //A  读绝对角度
				USART2_SendDMA(sprintf(snd2_buff,"Ang=%.2f\r\n",shaft_angle));
				break;
		}
		memset(rcv2_buff,0,16);  //USART2_BUFFER_SIZE //清空接收数组,长度覆盖接收的字节数即可
	}
}
/******************************************************************************/


