/**
*********************************************************************************************************
*                                                touchpad
*                                      (c) Copyright 2011-2031
*                                           All Rights Reserved
*
* @File    : 
* @By      : liwei
* @Version : V0.01
*
*********************************************************************************************************
**/

/*
*********************************************************************************************************
Includes 
*********************************************************************************************************
*/
#include "include_all.h"
/*
*********************************************************************************************************
Define
*********************************************************************************************************
*/

#define CACHE_BUFF_LONGTH 		10
#define CACHE_BUFF_NUM			50
#define IDLE_NUM				5
/*
*********************************************************************************************************
Typedef
*********************************************************************************************************
*/
typedef struct commnuication_frame
{
	u8	data_pointer;	   									//通讯缓存指针
	u8	data_cache[CACHE_BUFF_LONGTH][CACHE_BUFF_NUM];     	//通讯缓存
	u16	data_cache_long[CACHE_BUFF_LONGTH]; 				//通讯缓存长度

									
	u16 refresh_num;
	u16 current_num;
	u8	receive_buff[CACHE_BUFF_NUM];
	u8 	analysis_buff[CACHE_BUFF_NUM];
	u16 analysis_long;
	u16 null;
	u16 null2;

}COM_FRAME;

/*
*********************************************************************************************************
Variables
*********************************************************************************************************
*/

COM_FRAME   rs485_commnuication_frame;
/*
*********************************************************************************************************
Function 
*********************************************************************************************************
*/

/******************************************
函数名称：
功    能：刷新串口延时,存放接收数据 ,在串口中断函数中使用
void UART4_RX_ISR_PC(void)    interrupt 11
{
	res=SBUF2_RX;
	Common_Receive_Refresh(res,&rs485_commnuication_frame); 
}
参    数：
返回值  ：
*******************************************/
void Common_Receive_Refresh(u8 receive_data, COM_FRAME *com_pointer)
{
	//刷新计数器
	com_pointer->refresh_num=0;
	//缓存数据
	if(com_pointer->current_num<(CACHE_BUFF_NUM-1))
		com_pointer->receive_buff[com_pointer->current_num++]=receive_data;		
}
/******************************************
函数名称：
功    能：超过5MS串口空闲判断，将本函数放入1ms定时器中
void T0_ISR_PC(void)    interrupt 1
{
	//串口空闲判断
	Common_Idle_Judge(&rs485_commnuication_frame);
}
参    数：
返回值  ：
*******************************************/
void Common_Idle_Judge(COM_FRAME *com_pointer)
{
	com_pointer->refresh_num++;
	if(com_pointer->refresh_num>=IDLE_NUM)
	{
		com_pointer->refresh_num=IDLE_NUM;
		if(com_pointer->current_num>0)
		{	
			//缓存数据
			Common_Cache_Data(com_pointer->current_num,com_pointer);
			com_pointer->current_num=0;
		}		
	}
}
/******************************************
函数名称：
功    能：缓存串口数据，串口空闲后缓存数据
参    数：
返回值  ：
*******************************************/
void Common_Cache_Data(u16 buff_long,COM_FRAME *com_pointer)
{
	//判断缓存队列是否满
	if(com_pointer->data_pointer<(CACHE_BUFF_LONGTH-1))
	{
		//拷贝数据
		memcpy(com_pointer->data_cache[com_pointer->data_pointer],com_pointer->receive_buff,sizeof(com_pointer->data_cache[com_pointer->data_pointer]));		
		//保存数据长度
		com_pointer->data_cache_long[com_pointer->data_pointer]=buff_long;
		//数据序号加一
		com_pointer->data_pointer++;		
	}
}
/******************************************
函数名称：
功    能： 通用接收数据解析，执行无形参无返回函数
参    数：
返回值  ：
*******************************************/
void Common_Data_Analysis_Handle(COM_FRAME  *com_pointer,void (*Common_Analysis_Handle)(u8 * receive_buff , u8 length))
{
	if(com_pointer->data_pointer>0)
	{			
		//清空解析BUFF
		memset(com_pointer->analysis_buff,0,sizeof(com_pointer->analysis_buff));
		//从缓存中拷贝数据到解析BUFF
		memcpy(com_pointer->analysis_buff,com_pointer->data_cache[com_pointer->data_pointer-1],sizeof(com_pointer->analysis_buff));
		//拷贝数据长度
		com_pointer->analysis_long= com_pointer->data_cache_long[com_pointer->data_pointer-1];
		// 清空缓存
		memset(com_pointer->data_cache[com_pointer->data_pointer-1],0,sizeof(com_pointer->data_cache[com_pointer->data_pointer-1]));	
		com_pointer->data_cache_long[com_pointer->data_pointer-1]=0;
 		//数据解析		
		Common_Analysis_Handle(com_pointer->analysis_buff , com_pointer->analysis_long);
		//指针自减
		com_pointer->data_pointer--;					
	}		
}
/******************************************
函数名称：
功    能： 
参    数：
返回值  ：
*******************************************/
void User_Communication_Analysis(void)
{
	/*增加用户协议处理*/
	
}
/******************************************
函数名称：
功    能： 
参    数：
返回值  ：
*******************************************/
void Communication_Analysis_Handle(void)
{
	/*数据解析*/
	Common_Data_Analysis_Handle( &rs485_commnuication_frame, User_Communication_Analysis );
}
/***********************************************END*****************************************************/
