
#include "timer.h"


/*****************************************************************************/
void TIM1_PWM_Init(void)
{
	//NVIC_InitTypeDef          NVIC_InitStructure;
	GPIO_InitTypeDef          GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef         TIM_OCInitStructure;
	TIM_BDTRInitTypeDef       TIM_BDTRInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_DeInit(TIM1);
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned3;
	TIM_TimeBaseInitStructure.TIM_Period = 3500;     //168M/7000=24KHz
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 2;      //重复计数器，每(2+1)次产生一个溢出中断，所以中断频率是8KHz
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //使能输出
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  //使能互补输出
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         //输出极性为高
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;       //输出互补极性为高
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;   //空闲状态为低
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset; //空闲状态为低
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = 100;    // 100/168M=595ns
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}
/*****************************************************************************/
void TIM7_1ms_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE); //使能时钟
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn;       //配置中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);                     //初始化NVIC
	
	TIM_TimeBaseInitStructure.TIM_Period = 1000-1;       //定时器周期
	TIM_TimeBaseInitStructure.TIM_Prescaler= 84-1;       //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);  //初始化TIM2
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);            //设置定时器更新中断
	TIM_Cmd(TIM7,ENABLE);                               //使能TIM2
}
/*****************************************************************************/



